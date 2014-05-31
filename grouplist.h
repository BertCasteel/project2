#ifndef __GROUPLIST_H__
#define __GROUPLIST_H__

#include "processlist.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define STDOUT 1
#define STDIN 0

struct GroupNode
{
    pid_t pgid;
    int foreground;
    struct GroupNode * next;
    struct ProcessNode * processHead;
} GroupNode;

struct GroupNode* create_new_group(pid_t pgid);

struct GroupNode* add_new_process(struct GroupNode** start, pid_t pgid, pid_t pid, int stop);

int remove_process(struct GroupNode** head, pid_t pid, int * pgid);

//int remove_process_recur(struct GroupNode* head, pid_t pid, int * pgid);

int remove_group(struct GroupNode** head, pid_t pgid);

int stop_group(struct GroupNode* head, pid_t pgid);

int resume_group(struct GroupNode* head, pid_t pgid);

int get_most_recent_stopped(struct GroupNode* head);

void print_grouplist(struct GroupNode* head);

#endif


