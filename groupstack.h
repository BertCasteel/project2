#ifndef __GROUPSTACK_H__
#define __GROUPSTACK_H__

#include "processlist.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define STDOUT 1
#define STDIN 0

struct GroupNode
{
    pid_t pgid;
    int stop;
    struct GroupNode * next;
    struct ProcessNode * processHead;
} GroupNode;

/** 
Create GroupNode with "empty" processlist. Set the intial head of the process pipeline to -1.
This indicates an "empty" processlist
@return: new groupnode
**/
struct GroupNode* create_new_group(pid_t pgid, int stop);

/** 
Add new process to group stack. A -1 pgid indicates an empty group stack*. If pgid of the 
new process does not exist, create a new GroupNode 
@return: head of group stack
**/
struct GroupNode* add_new_process(struct GroupNode** start, pid_t pgid, pid_t pid, int stop);

/**
Remove process from group queue.
@return:
    0 if removed succesfully
    -1 if not found;
**/
int remove_process(struct GroupNode** head, pid_t pid, int * pgid);

/**
Remove entire process group
@return:
    0 if removed succesfully
    -1 if not found
**/
int remove_group(struct GroupNode** head, pid_t pgid);

/**
mark group as stopped
**/
int stop_group(struct GroupNode* head, pid_t pgid);

/**
turn off "stop" boolean
**/
int resume_group(struct GroupNode* head, pid_t pgid);

/**
get most recent stopped process group
@return:
    pgid
    -1 if not found
**/
int get_most_recent_stopped(struct GroupNode* head);

/**
get pgid of process group at given position in stack
**/
int get_groupid(struct GroupNode* head, int position);

/**
print stack with each process group's process pipeling
**/
void print_groupstack(struct GroupNode* head);
#endif


