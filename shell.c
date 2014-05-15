/**
Robert Casteel
CIS415: Operating Systems
Spring 2014, University of oregon
Project 1
**/

#include <signal.h>
#include <unistd.h>

#define STDOUT 1
#define STDIN 0
#define bufSize 1024

char *newargv[] = {NULL, NULL}; /*hold arguments for child process creation*/
char prompt[] = "GALACTUS# ";
char time_up[] = "...galactus hungers\n";
char in_time[] = "I shall spare this planet. Herald, find me another.\n";
char cmd[bufSize]; /*buffer for command*/
pid_t pid = -1; /*global process id for command process. */

/* Argument: Time limit*/
int main(int argc, char* argv[])
{
	/* shell's loop.*/
	while(1){
		write(STDOUT, (void *) prompt, sizeof(prompt));
  		fsync(STDOUT);	
		int i = read(STDIN, cmd, bufSize);
		/*Only need first argument of command... */
		int j;
		for (j = 0; j<i; j++){
			if (cmd[j] == ' ' || cmd[j] == '\n'){
				cmd[j] = '\0'; //end string
			}
		}
		newargv[0] = cmd;
		
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
	} //end shell loop
	return 0;
}
