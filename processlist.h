#ifndef __PROCESSLIST_H__
#define __PROCESSLIST_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define STDOUT 1
#define STDIN 0

struct ProcessNode
{
    pid_t pid;
    struct ProcessNode *next;
    int stopped;
} ProcessNode;


/**
push new process onto stack
**/
struct ProcessNode* add_to_beginning(struct ProcessNode* head, pid_t pid, int stop);

/**
return 0 if not found
**/
int search_for_pid(struct ProcessNode* head, pid_t pid);
/**
return 1 if deleted succesfully
return 0 if not found
**/
int delete_from_processlist(struct ProcessNode** head, pid_t pid);

/**
delete every process
**/
int delete_all_processes(struct ProcessNode* head);

/**
mark process as stopped
**/
void stop_processlist(struct ProcessNode* head);

/**
turn off stop boolean
**/
void resume_processlist(struct ProcessNode* head);
/**
print entire process list
**/
void print_processlist(struct ProcessNode* head);


#endif