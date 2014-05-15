/**
Robert Casteel and Kevin Beick
CIS415: Operating Systems
Spring 2014, University of Oregon
Project 2
**/

#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "tokenizer.h"

#define STDOUT 1
#define STDIN 0
#define bufSize 1024
#define MAX_NUM_ARGS (50)

typedef enum { false, true } bool;

char *newargv[] = {NULL, NULL}; /*hold arguments for child process creation*/
char prompt[] = "GALACTUS# ";
char time_up[] = "...galactus hungers\n";
char in_time[] = "I shall spare this planet. Herald, find me another.\n";
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
	
	int pipefd[2];
	bool pipeBool = false;

	/* shell's loop.*/
	while(1){
		/* Restore STDOUT,STDIN to original file descriptor */
		dup2(original_out, STDOUT_FILENO);
		dup2(original_in, STDIN_FILENO);

		/* Issue prompt, read in */
		write(STDOUT, (void *) prompt, sizeof(prompt));
		fsync(STDOUT);	
		int i = read(STDIN, input, bufSize);	
		input[i-1] = '\0'; /* remove trailing \n*/
		newargv[0] = input;

		/* tokenize command */
		tokenizer = init_tokenizer( newargv[0] );
		char* token;
		int j=0; /* Index of current cmd arg */
		bool continue_to_prompt = false; /* Means of abandoning this input cmd and reissuing prompt (if true) */
		while ( (token = get_next_token( tokenizer )) != NULL && j<MAX_NUM_ARGS ){
			// printf("Got token '%s'\n", token);
			
//check for pipe
if(token[0] == '|'){
	
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}
		
	pipeBool = true;
			
	/*create child process*/
	pid = fork();
	
	if(pid < 0) { /*error occured*/
		write(STDOUT, "Error occured creating child process\n" , 100);
		fsync(STDOUT);	
		return 1;
	}
	else if( pid ==0){/*child process writes to pipe*/
		close(pipefd[0]);  /*close unused read end */
		dup2(pipefd[1], STDOUT_FILENO);	/*redirect stdout to pipe*/
		close(pipefd[1]); /*reader will see EOF */
		execvp(cmd[0], cmd); /*execute first command */
	}
	else {
		int status;
		waitpid(pid, &status, 0);		
		for (int k = 0; k < j; k++){
			cmd[k] = NULL;
		}
		j = 0;
	}

}/*end pipe*/
else{
			/* REDIRECTION HANDLER */
			if(token[0]=='<' || token[0]=='>'){
				char* next_tok;
				if((next_tok = get_next_token( tokenizer )) != NULL){
					redirectionHandler(token, next_tok);
				}else{
					write(STDOUT, "syntax error near unexpected token `newline'\n" , 100);
					continue_to_prompt = true;
				}
				continue; // Continue to next args (don't record redirection args in cmd[])
			}
			cmd[j] = token;
			j++;
		}
		}

		/* Check if we should reissue prompt */
		if(continue_to_prompt){ continue; }

		/*create child process*/
		pid = fork();
	
		if(pid < 0) { /*error occured*/
			write(STDOUT, "Error occured creating child process\n" , 100);
			fsync(STDOUT);	
			return 1;
		}
		else if (pid == 0) {/*child proccess*/
			execvp(cmd[0], cmd);
		}
		else { /* parent process */
			int status;
			waitpid(pid, &status, 0);
			// write(STDOUT, in_time, sizeof(in_time));
			// fsync(STDOUT);
		}
		free_tokenizer( tokenizer );
	} //end shell loop
	return 0;
}
