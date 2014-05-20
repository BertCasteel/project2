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
		printf("got a <\n");
	}
	return;
}


int main(int argc, char* argv[])
{
	/* Save original STDOUT so we can restore it if/when changed */
	int original_out;
	dup2(STDOUT_FILENO, original_out);

	/* shell's loop.*/
	while(1){
		dup2(original_out, STDOUT_FILENO);

		write(STDOUT, (void *) prompt, sizeof(prompt));
		fsync(STDOUT);	
		int i = read(STDIN, input, bufSize);	
		input[i-1] = '\0'; /* remove trailing \n*/
		newargv[0] = input;

		/* tokenize command */
		tokenizer = init_tokenizer( newargv[0] );
		char* token;
		int j=0;
		// bool need_more_args = false;
		while ( (token = get_next_token( tokenizer )) != NULL && j<MAX_NUM_ARGS ){
			printf("Got token '%s'\n", token);
			if(token[0]=='<' || token[0]=='>'){
				char* next_tok;
				if((next_tok = get_next_token( tokenizer )) != NULL)
				{
					redirectionHandler(token, next_tok);
				}else
				{
					write(STDOUT, "syntax error near unexpected token `newline'\n" , 100);;
				}
				j++;
				// need_more_args=true;
				continue;
			}
			cmd[j] = token;
			j++;
			// need_more_args=false;
		}

		/* We've gone thru all input. Still expecting more? */
		// if(need_more_args==true){
		// 	//ERROR
		// 	write(STDOUT, "syntax error near unexpected token `newline'\n" , 100);
		// 	fsync(STDOUT);	
		// }
		
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
