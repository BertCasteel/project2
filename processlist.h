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


struct ProcessNode* add_to_beginning(struct ProcessNode* head, pid_t pid, int stop);

int search_for_pid(struct ProcessNode* head, pid_t pid);

int delete_from_processlist(struct ProcessNode** head, pid_t pid);

int delete_all_processes(struct ProcessNode* head);

void stop_processlist(struct ProcessNode* head);

void resume_processlist(struct ProcessNode* head);

void print_processlist(struct ProcessNode* head);

#endif