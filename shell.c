/**
Robert Casteel and Kevin Beick
CIS415: Operating Systems
Spring 2014, University of Oregon
Project 2
**/

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "tokenizer.h"
#include "linked_list.h"

#define STDOUT 1
#define STDIN 0
#define bufSize 1024
#define MAX_NUM_ARGS (50)

typedef enum { false, true } bool;


char prompt[] = "GALACTUS# ";
char errCreatingChild[] = "Error occured creating child process\n";

char input[bufSize]; /*buffer for command*/
pid_t pid = -1; /*global process id for command process. */
TOKENIZER *tokenizer;

/* Temp solution - TODO arbitrary length? */
char* cmd[MAX_NUM_ARGS];


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


int main(int argc, char* argv[])
{
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

	struct Node* bgProcessesLL = (struct Node*)malloc(sizeof(struct Node));
	bgProcessesLL = NULL;
	

	/* shell's loop.*/
	while(1){
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
					// printf("background\n");
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

			/* PIPE HANDLER */
			if(token[0] == '|'){
				
				if (pipe(pipefd) == -1) {
					perror("pipe");
					exit(EXIT_FAILURE);
				}
					
				pipeBool = true;
				/*create child process*/
				pid = fork();
				if(pid < 0) { /*error occured*/
					write(STDOUT_FILENO, errCreatingChild , sizeof(errCreatingChild));
					fsync(STDOUT_FILENO);	
					return 1;
				}
				else if( pid == 0){/*child process writes to pipe*/
					if(background){
						printf("%d\n", getpid());
						setpgid(0, getpid());
					}
					dup2(pipefd[1], STDOUT_FILENO);	/*redirect stdout to pipe*/
					close(pipefd[0]);  /*close unused read end */
					cmd[j] = NULL;     
					execvp(cmd[0], cmd); /*execute first command */
				}
				else {
					if(background) {
						pipeGrp = pid;
						printf("%d\n", pipeGrp);
					}
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

		/*create child process*/
		pid = fork();
	
		if(pid < 0) { /*error occured*/
			write(STDOUT_FILENO, errCreatingChild , sizeof(errCreatingChild));
			fsync(STDOUT_FILENO);	
			return 1;
		}
		else if (pid == 0) {/*child proccess*/	
			/* change group process id in child process to ensure execvp runs after the change */
			if (background){
				if (pipeBool){
					printf("%d\n", pipeGrp);
					if ( setpgid(0, pipeGrp) != 0){
						perror("setpgid");
						exit(EXIT_FAILURE);
					}
				}
				else {
					setpgid(0, getpid());
				}
			}		 
			if( pipeBool){
				dup2(pipefd[0], STDIN_FILENO);
			}
			// write(2, cmd[0], sizeof(cmd[0]));
			execvp(cmd[0], cmd);
		}
		else { /* parent process */
			if(background){
				/* add it to linked list */
				add_to_end(bgProcessesLL, pid);
			}
			else{
				int status;
				waitpid(pid, &status, 0);
			}
		}

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

	} //end shell loop
	return 0;
}
