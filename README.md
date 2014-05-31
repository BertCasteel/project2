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
* linked_list.c -- implementation of linked list data structure


###Extra Cedit:
    Disadvantages of synchronous job control:  It's not an efficient use of resources or time. Polling each job to see if
        it has completed inherently means that all jobs that have not changed state will be checked for a changed state.
        Spending time and resources to check if each child has changed state is a waste on those children that have not 
        changed state. Alternatively, asynchronous job handling will handle a changed state as it happens, thus resources
        and time are devoted to the task only when it needs to be done.

    How to tell the difference between system calls and library functions:   System calls are in manual section 2 (eg: fork(2))
        and library functions are in manual section 3 (eg: execvp(3))


###Compilation Instructions:


###Project Overview:
    who wrote what?


###Code Description and Layout:


###General Comments


time spent:
