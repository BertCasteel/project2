/**
Robert Casteel and Kevin Beick
CIS415: Operating Systems
Spring 2014, University of Oregon
Project 2
**/

#include <signal.h>
#include <unistd.h>
#include "tokenizer.h"

#define STDOUT 1
#define STDIN 0
#define bufSize 1024
#define MAX_NUM_ARGS (50)

char *newargv[] = {NULL, NULL}; /*hold arguments for child process creation*/
char prompt[] = "GALACTUS# ";
char time_up[] = "...galactus hungers\n";
char in_time[] = "I shall spare this planet. Herald, find me another.\n";
char input[bufSize]; /*buffer for command*/
pid_t pid = -1; /*global process id for command process. */
TOKENIZER *tokenizer;

/* Temp solution - TODO arbitrary length? */
char* cmd[MAX_NUM_ARGS];

int main(int argc, char* argv[])
{
	
	/* shell's loop.*/
	while(1){
		write(STDOUT, (void *) prompt, sizeof(prompt));
		fsync(STDOUT);	
		int i = read(STDIN, input, bufSize);	
		input[i-1] = '\0'; /* remove trailing \n*/
		newargv[0] = input;

		/* tokenize command */
		tokenizer = init_tokenizer( newargv[0] );
		char* token;
		int j=0;
		while ( (token = get_next_token( tokenizer )) != NULL && j<MAX_NUM_ARGS ){
			// printf("Got token '%s'\n", token);
			cmd[j] = token;
			j++;
		}
		
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
