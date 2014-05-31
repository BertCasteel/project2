/**
Robert Casteel and Kevin Beick
CIS415: Operating Systems
Spring 2014, University of Oregon
Project 2
**/

#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "tokenizer.h"
#include "grouplist.h"
#include <assert.h>

#define STDOUT 1
#define STDIN 0
#define bufSize 1024
#define MAX_NUM_ARGS (50)
#define STOP 1
#define RESUME 0

typedef enum { false, true } bool;


char prompt[] = "GALACTUS# ";
char errCreatingChild[] = "Error occured creating child process\n";
char bg[] = "bg"; 
char fg[] = "fg";

char input[bufSize]; /*buffer for command*/
pid_t kidpid = -1; /*global process id for command process. */
TOKENIZER *tokenizer;
pid_t shell_pid;

/* Temp solution - TODO arbitrary length? */
char* cmd[MAX_NUM_ARGS];

int shell_terminal;

/* Global linked list of background processes */
struct GroupNode* bgProcessesLL;



bool stringCompare(char* a, char* b)
{
	int i=0;
	while(a[i]!='\0' && b[i]!='\0'){
		if (a[i] != b[i]){ return false; }
		i++;
	}
	if(b[i]=='\0'){
		if (a[i]=='\0'){
			return true;
		}else{return false;}
	}
	return false;
}

void backgroundForegroundCommands(char command[])
{
	/* If no most recent job in bg, then report an error */

	if(stringCompare(command,bg)==true){
		printf("%s\n", "you entered command bg! nice job dude!" );
		printf("here are the bg processes\n");
		print_grouplist(bgProcessesLL);
		/* Deliver SIGCONT signal to the most recently stopped background job */


	}
	else if(stringCompare(command,fg)==true){
		printf("%s\n", "you entered command fg! nice job dude!" );
		/* Bring the most recently backgrounded job to the foreground */
		if(tcsetpgrp(shell_terminal, bgProcessesLL->pgid)<0)
		/* Deliver SIGCONT signal in case that job is stopped */
		killpg(bgProcessesLL->pgid, SIGCONT);
		/* wait for it */
		int status;
		waitpid(bgProcessesLL->pgid, &status, 0);
		/* Remove it from the list of bg processes */
		killpg(shell_pid, SIGCONT);
		remove_group(&bgProcessesLL, bgProcessesLL->pgid);
		printf("did we get here?\n");
		if (tcsetpgrp(shell_terminal, shell_pid) <0){

			printf("bummer %d\n", errno);
		}
		printf("almost done...\n");
	}
	return;
}

void redirectionHandler(char* direction, char* file)
{
	if( direction[0] == '>' ){
		int new_out = open(file, O_TRUNC | O_WRONLY | O_CREAT, 0644);
		dup2(new_out, STDOUT_FILENO);
	}else if( direction[0] == '<' ){
		int new_in = open(file, O_RDONLY);
		dup2(new_in, STDIN_FILENO);
	}
	return;
}

void sigchld_handler(int sig_num){
	pid_t pid;
	int status;
	while( (pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0 ){

		/* handle stops */
		if (WIFSTOPPED(status)){
			printf("process %d stopped\n", pid);
			if(stop_group(bgProcessesLL, getpgid(pid)) == 0){
				pid_t groupid = getpgid(pid);
				bgProcessesLL = add_new_process(&bgProcessesLL, groupid, pid, STOP);
				printf("returned from adding new process\n");
			}
			else {
				print_grouplist(bgProcessesLL);
			}
			tcsetpgrp(shell_terminal, shell_pid);
			printf("about to return from handler\n");
			// printf("tc belongs to %d\n", tcgetpgrp(shell_pid));
			return; 
		}
		printf("....removing process\n");
		print_grouplist(bgProcessesLL);
		int pgid;
		if (remove_process(&bgProcessesLL, pid, &pgid) == -1){
			printf("cannot find bg process...\n");
		}
	}
}


int main(int argc, char* argv[])
{
	// char abc[]   = "abc";
	// char abc2[]   = "abc";
	// char abcd[]  = "awecd";
	// char abcde[] = "abcde";

	// if(stringCompare(abc, abcd)==true){printf("%s==%s\n",abc, abcd);}
	// else{printf("%s!=%s\n",abc, abcd);}
	
	// if(stringCompare(abcde, abcd)==true){printf("%s==%s\n",abcde, abcd);}
	// else{printf("%s!=%s\n",abcde, abcd);}
	
	// if(stringCompare(abc, abc2)==true){printf("%s==%s\n",abc, abc2);}
	// else{printf("%s!=%s\n",abc, abc2);}


	shell_terminal = STDIN_FILENO;
	shell_pid = getpid();
	setpgid(0, shell_pid);
	tcsetpgrp(shell_terminal, getpid());

	bgProcessesLL = (struct GroupNode*)malloc(sizeof(struct GroupNode));
	bgProcessesLL->pgid = -1;
	//bgProcessesLL = NULL;

	signal(SIGCHLD, sigchld_handler);
	signal(SIGTSTP, SIG_IGN);

	/* Save original STDOUT, STDIN so we can restore it if/when changed */
	// TODO, do this w/o using dup()....
	int original_out = dup(STDOUT_FILENO);
	int original_in  = dup(STDIN_FILENO);

	// int original_out;
	// int original_in;
	// dup2(STDOUT_FILENO, original_out);
	// dup2(STDIN_FILENO, original_in);
	// printf("%d\n", original_out);
	// printf("%d\n", original_in);


	/* shell's loop.*/
	while(1){
		signal(SIGTERM, SIG_IGN);
		signal(SIGINT, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);
		int pipefd[2];
		bool pipeBool = false;
		pid_t pipeGrp = -1;
		bool background = false;
		/* Restore STDOUT,STDIN to original file descriptor */
		dup2(original_out, STDOUT_FILENO);
		dup2(original_in, STDIN_FILENO);

		/* Clear cmd[] */
		int clear;
		for(clear = 0; clear < MAX_NUM_ARGS; clear++) { cmd[clear] = NULL; }

		/* Issue prompt, read in */
		write(STDOUT_FILENO, (void *) prompt, sizeof(prompt));
		fsync(STDOUT_FILENO);	
		int length = read(STDIN_FILENO, input, bufSize);	
		input[length-1] = '\0'; /* remove trailing \n*/
		//newargv[0] = input;
		bool continue_to_prompt = false; /* Means of abandoning this input cmd and reissuing prompt (if true) */


		/* Background Handler */		
		int i;
		for (i = 0; i < length; i++){
			if (input[i] == '&'){
				if(i == length - 2){
					write(1, "background registered\n", sizeof("background registered\n"));
					fsync(1);
					background = true;
					input[length - 2] = '\0';
				}
				else{
					char bgSyntaxErr[] = "syntax error: & can only be last character\n";
					write(1, bgSyntaxErr, sizeof(bgSyntaxErr));
					continue_to_prompt = true;
				}
			}
		}
		

		/* tokenize command */
		tokenizer = init_tokenizer( input );
		char* token;
		int j=0; /* Index of current cmd arg */
		
		while ( (token = get_next_token( tokenizer )) != NULL && j<MAX_NUM_ARGS ){
			//printf("Got token %s", token);
			//printf(" size:%d\n", strlen(token)); 	

			/* HOW TO QUIT OUR SHELL */
			if( j==0 && stringCompare(token,"q")==true){ exit(0); }

			/* CUSTOM BUILT-IN COMMANDS */
			if( j==0 && (stringCompare(token,bg)==true || stringCompare(token,fg)==true) ){ 
				backgroundForegroundCommands(token); 
				continue_to_prompt = true;
			}

			/* PIPE HANDLER */
			if(token[0] == '|'){
				
				if (pipe(pipefd) == -1) {
					perror("pipe");
					exit(EXIT_FAILURE);
				}
					
				pipeBool = true;
				/*create child process*/
				kidpid = fork();


				if(kidpid < 0) { /*error occured*/
					write(STDOUT_FILENO, errCreatingChild , sizeof(errCreatingChild));
					fsync(STDOUT_FILENO);	
					return 1;
				}
				else if( kidpid == 0){/*child process writes to pipe*/
					setpgid(0, getpid());

					if(background){
						/**/
					}else{
						signal(SIGTERM, SIG_DFL);
						signal(SIGINT, SIG_DFL);
						signal(SIGTSTP, SIG_DFL);
						tcsetpgrp(shell_terminal, getpid());
					}

					dup2(pipefd[1], STDOUT_FILENO);	/*redirect stdout to pipe*/
					close(pipefd[0]);  /*close unused read end */
					cmd[j] = NULL;     
					execvp(cmd[0], cmd); /*execute first command */
				}
				else { /*parent process*/
					setpgid(kidpid, kidpid);

					if(background){
						add_new_process(&bgProcessesLL, kidpid, kidpid, RESUME);

					//	tcsetpgrp(shell_terminal, getpid());
					}else{
						signal(SIGTTOU, SIG_IGN);
						tcsetpgrp(shell_terminal, kidpid);
					}

					pipeGrp = kidpid;
					// wait(NULL);
					close(pipefd[1]); /*reader will see EOF */

					/*clear cmd array. limit scope of index k */
					{ 
						int k;
						for (k = 0; k < j; k++){ cmd[k] = NULL; }
					}
					j = 0;
				}

			}/*end pipe*/

			/* REDIRECTION HANDLER */
			else if(token[0]=='<' || token[0]=='>'){
				char* next_tok;
				if((next_tok = get_next_token( tokenizer )) != NULL){
					redirectionHandler(token, next_tok);
				}
				else{
					char redirSyntaxErr[] = "syntax error near unexpected token `newline'\n";
					write(STDOUT_FILENO, redirSyntaxErr, sizeof(redirSyntaxErr));
					continue_to_prompt = true;
				}
				continue; // Continue to next arg (don't record redirection args in cmd[])
			}

			/* DEFAULT HANDLER */
			else{
				cmd[j] = token;
				j++;
			}
		}//end tokenizer

		/* Check if we should reissue prompt */
		if(continue_to_prompt){ continue; }

		/* -------------- READY TO ISSUE COMMAND ------------- */
		
		/*create child process*/
		kidpid = fork();
	
		if(kidpid < 0) { /*error occured*/
			write(STDOUT_FILENO, errCreatingChild , sizeof(errCreatingChild));
			fsync(STDOUT_FILENO);	
			return 1;
		}
		else if (kidpid == 0) {/*child proccess*/	

			pid_t child_id = getpid();

			/* change group process id in child process to ensure execvp runs after the change */
			if (background){
				/* give terminal control to shell */
//				tcsetpgrp(shell_terminal, getppid());

				signal(SIGTTIN, SIG_DFL);
				signal(SIGTTOU, SIG_DFL);

				if (pipeBool){
					child_id = pipeGrp;
					// if ( setpgid(0, pipeGrp) != 0){
					// 	perror("setpgid");
					// 	exit(EXIT_FAILURE);
					// }
				}
			}


			if ( setpgid(0, child_id) != 0){
				perror("setpgid");
				exit(EXIT_FAILURE);
			}

			if ( !background ){
				signal(SIGTERM, SIG_DFL);
				signal(SIGINT, SIG_DFL);
				signal(SIGTSTP, SIG_DFL);
				tcsetpgrp(shell_terminal, child_id);
			}

			if( pipeBool){
				dup2(pipefd[0], STDIN_FILENO);
			}

			// write(2, cmd[0], sizeof(cmd[0]));
			execvp(cmd[0], cmd);

			/* Gets here only if execvp has failed */
			write(STDOUT_FILENO, cmd[0], sizeof(cmd[0]));
			write(STDOUT_FILENO, ": command not found\n", sizeof(": command not found\n"));
			fsync(STDOUT_FILENO);	
			/* Exits child process */
			exit(errno);
		}
		else { /* parent process */
//			signal(SIGTTIN, SIG_IGN);
			
			pid_t foreground = -1;
			
			if ( pipeBool ){
				if( setpgid(kidpid, pipeGrp) != 0){		
					perror("setpgid");
					exit(EXIT_FAILURE);
				}
				foreground = pipeGrp;

			} else { 
				if ( setpgid(kidpid, kidpid) != 0){
					perror("setpgid");
					exit(EXIT_FAILURE);
				}
				foreground = kidpid;
			}
	

			if(background){
				/* add it to linked list */
				//print_grouplist(bgProcessesLL);
				bgProcessesLL = add_new_process(&bgProcessesLL, getpgid(kidpid), kidpid, RESUME);
				//print_grouplist(bgProcessesLL);
			}
			else{
				signal(SIGTTOU, SIG_IGN);
				tcsetpgrp(shell_terminal, foreground);
				int status;
				waitpid(kidpid, &status, WUNTRACED);
			}
		}

		/* terminal control retained by shell */
		tcsetpgrp(shell_terminal, shell_pid);
		/* Revert to original settings */
		/* clear out pipe */
		if (pipeBool == true){
			close(pipefd[0]);
			close(pipefd[1]);
			pipeBool = false;
		}
		background = false;
		pipeGrp = -1;
		free_tokenizer( tokenizer );
		
		fsync(STDOUT_FILENO);	
	} //end shell loop
	return 0;
}
