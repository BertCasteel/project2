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

char *newargv[] = {NULL, NULL}; /*hold arguments for child process creation*/
char prompt[] = "GALACTUS# ";
char time_up[] = "...galactus hungers\n";
char in_time[] = "I shall spare this planet. Herald, find me another.\n";
char cmd[bufSize]; /*buffer for command*/
pid_t pid = -1; /*global process id for command process. */
TOKENIZER *tokenizer;

/* Argument: Time limit*/
int main(int argc, char* argv[])
{
	
	/* shell's loop.*/
	while(1){
		write(STDOUT, (void *) prompt, sizeof(prompt));
  		fsync(STDOUT);	
		int i = read(STDIN, cmd, bufSize);	
		cmd[i-1] = '\0'; /* remove trailing \n*/
		newargv[0] = cmd;
		/* tokenize command */
		tokenizer = init_tokenizer( newargv[0] );
		char* token;
		while ( (token = get_next_token( tokenizer )) != NULL ){
			printf("Got token '%s'\n", token);
			free( token );
		}
		
		/*create child process*/
		pid = fork();
	
		if(pid < 0) { /*error occured*/
			write(STDOUT, "Error occured creating child process\n" , 100);
	  		fsync(STDOUT);	
			return 1;
		}
		else if (pid == 0) {/*child proccess*/
			execvp(newargv[0], newargv);
		}
		else { /* parent process */
			int status;
			waitpid(pid, &status, 0);
			write(STDOUT, in_time, sizeof(in_time));
			fsync(STDOUT);
		}
		free_tokenizer( tokenizer );
	} //end shell loop
	return 0;
}
