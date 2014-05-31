project2
========

CIS 415 - Operating Systems Project #2 - 
Fully-Featured Shell Spring 2014 - 
Prof. Butler
Due date: May 30, 2014 
In this assignment, you will implement a more fully-featured shell.


:authors: Kevin Beick and Robert Casteel

:source files:
* Makefile ------- use to easily compile the project
* shell.c -------- source code for the shell's main functionality
* tokenizer.c ---- for parsing
* tokenizer.h ---- header file for tokenizer
* grouplist.c ----
* grouplist.h ---- header file
* processlist.c --
* processlist.h -- header file


###Extra Cedit:
    Disadvantages of synchronous job control:  It's not an efficient use of resources or time. Polling each job to see if
        it has completed inherently means that all jobs that have not changed state will be checked for a changed state.
        Spending time and resources to check if each child has changed state is a waste on those children that have not 
        changed state. Alternatively, asynchronous job handling will handle a changed state as it happens, thus resources
        and time are devoted to the task only when it needs to be done.

    How to tell the difference between system calls and library functions:   System calls are in manual section 2 (eg: fork(2))
        and library functions are in manual section 3 (eg: execvp(3))

    (from page 6 of project description) We maintain a queue of recent background jobs such that if one completes, the next time
        bg or fg is called the next most recent job gets teh SIGCONT signal.  We use our "grouplist" data structure to do this.

    (from page 7 of project description) We have a built-in command "jobs" that reports the current background processes.
        It displays a job id, the pid, and its running/stopped status for each background job. A user can enter 'fg/bg %x' 
        where x is a job number to apply the functionality of fg/bg to the specified job.


###Compilation Instructions:
    cd into the project directory. Enter 'make' into the terminal to compile the shell/  The executable is called 'shell' so
    enter ./shell to run it.


###Project Overview:
    Kevin Beick and Robert Casteel worked together on this project.
    
    overview of work accomplished:
        We have redirection and piping working completely, behaving as BASH does. Supplying a '&' at the end of any
        command moves that job to the background. We implemented asynchronous singal handling by setting up a 
        signal handler for the SIGCHLD signal. This handler handles terminated and stopped processes appropriately.
        Our shell delegrates terminal control appropriately in the vast majority of cases; processes in the foreground
        have terminal control while those in the background do not. There are a few edge cases that do not work as expected,
        but for most user input the shell delegates control appropriately.
        Signals sent by ctrl-Z and ctrl-C do not stop or exit the shell, but are relayed to the foreground process.
        We implemented minimal job control. We implemented the commands 'fg' and 'bg' as per the project specs. We offer
        the extra ability that 'bg' and 'fg' will deliver the SIGCONT signal to the most recent job available, not just
        latest job to be backgrounded.  We also have a built-in jobs command for reporting the current background jobs.

    * Redirection:              Kevin
    * Two-Stage Pipelines:      Rob
    * Job/Process Group Iso:    Together
    * Foreground/Background:    Together
    * Asynchronous Signals:     Rob
    * Terminal Control:         Together
    * Terminal Signal Relaying: Kevin
    * Minimal Job Control:      Together
    * Data Structure:           Rob


###Code Description and Layout:
    Our Data Structure for recording background processes is a linked list (grouplist) in which each node has its own linked list
    (processlist).  The main linked list is comprised of nodes that represent process groups.  Each of these nodes has the pgid of
    the group, and a pointer to the linked list of group processes and one to the next group node.

    h


###General Comments:


    time spent:
    ~70 hrs total between the two of us.
