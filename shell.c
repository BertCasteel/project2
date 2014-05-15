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
int processKilled = 0; /*boolean for whether proccess was terminated before completion*/

/*Signal handler for Alarm Signal. Kill command process*/
void alarm_handler(int sig_num){
	alarm(0);	/*turn off alarm to be safe...*/
	processKilled = 1; 	/* mark boolean to indicate child process terminated*/
	if ( kill(pid, SIGKILL) != 0 ){	  /*kill child process*/
		write(STDOUT, "Error killing child process\n", 100);
		fsync(STDOUT);
	}
}

/* Argument: Time limit*/
int main(int argc, char* argv[])
{
	int time = 0; /* No argument means no time limit*/
	if(argc == 2)  {
		time = atoi(argv[1]);
	}
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
			/*set up handler for alarm signal*/
			signal(SIGALRM, alarm_handler);
			alarm(time);	/*start timer*/
			int status;
			waitpid(pid, &status, 0);
			alarm(0);	/*End timer! */		
			if (processKilled == 1) { /*child process was killed*/
				write(STDOUT, time_up, sizeof(time_up));
				fsync(STDOUT);
				processKilled = 0;
			}
			else {/*child process completed*/
				write(STDOUT, in_time, sizeof(in_time));
				fsync(STDOUT);
			}
		}
	} //end shell loop
	return 0;
}
