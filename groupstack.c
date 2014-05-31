#include "groupstack.h"
#include "processlist.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#define STDOUT 1
#define STDIN 0
#define EMPTY_PROCESS_GROUP 0

/** 
Create GroupNode with "empty" processlist. Set the intial head of the process pipeline to -1.
This indicates an "empty" processlist
@return: new groupnode
**/
struct GroupNode* create_new_group(pid_t pgid, int stop){
    struct GroupNode* newGroupNode = (struct GroupNode *)malloc(sizeof(struct GroupNode));

    if(newGroupNode == NULL){ 
        write(STDOUT, "Failed to create new GroupNode.\n", 27);
        return NULL;
    }
    newGroupNode->pgid = pgid;
    newGroupNode->next = NULL;
	newGroupNode->stop = stop;
    newGroupNode->processHead = (struct ProcessNode *)malloc(sizeof(struct ProcessNode));
    newGroupNode->processHead->pid = -1;
    newGroupNode->stop = 0;
    return newGroupNode;
}

/** 
Add new process to group stack. A -1 pgid indicates an empty group stack*. If pgid of the 
new process does not exist, create a new GroupNode 
@return: head of group stack
**/
struct GroupNode* add_new_process(struct GroupNode** start, pid_t pgid, pid_t pid, int stop){
    assert(start != NULL);
    if ((*start)->pgid == -1){
        printf("empty start!\n");
        (*start)->pgid = pgid;
        (*start)->next = NULL;
	(*start)->stop = 1;
        (*start)->processHead = (struct ProcessNode *)malloc(sizeof(struct ProcessNode));
        (*start)->processHead->pid = -1;
        /*head->pgid = pgid;
        head->next = NULL;
        head->processHead = (struct ProcessNode *)malloc(sizeof(struct ProcessNode));*/
    }
    struct GroupNode* head = *start;

    while( head->next != NULL && head->pgid != pgid){
        head = head->next;
    }

    if (head->next == NULL && head->pgid != pgid){
        /* end of list */
        head = create_new_group(pgid, stop);
        head->next = *start;
    }

    head->processHead = add_to_beginning(head->processHead, pid, stop);
    return head;
}

/**
private helper function for removing processes.
@return: 
    1 on success
    EMPTY_PROCESS_GROUP if the last process of a group was removed
    -1 if not found
**/
int remove_process_recur(struct GroupNode* head, pid_t pid, int * pgid){
    if(head == NULL){
        return -1;
    }

    if (head->processHead != NULL && delete_from_processlist(&(head->processHead), pid)){
        *pgid = head->pgid;
        if(head->processHead == NULL){
            return EMPTY_PROCESS_GROUP;
        }
        return 1;
    }

    return remove_process_recur(head->next, pid, pgid); 
}

/**
Remove process from group queue.
@return:
    0 if removed succesfully
    -1 if not found;
**/
int remove_process(struct GroupNode** head, pid_t pid, int * pgid){
    
    if(*head == NULL){
        return -1;
    }

    int returnValue = remove_process_recur(*head, pid, pgid);
    if (returnValue == EMPTY_PROCESS_GROUP){
        printf("removing grouppppp\n");
        return remove_group(head, *pgid);
    }
    else
        return returnValue;
}

/**
Remove entire process group
@return:
    0 if removed succesfully
    -1 if not found
**/
int remove_group(struct GroupNode** head, pid_t pgid){
    /*
    Iterates through list and removes first occurance of _pgid_
    */

    assert(head != NULL);
    if((*head)->next == NULL){
        (*head)->pgid = -1;
        printf("only one group.\n");
        fsync(1);
        delete_all_processes((*head)->processHead);
        printf("out of delete_all_processes()\n");
        return 0;
    }
    struct GroupNode * prev;
    struct GroupNode * curr = *head;

    while(curr != NULL){
        if(curr->pgid == pgid){
            if(curr==*head){
                *head=curr->next;
                delete_all_processes(curr->processHead);
                free(curr);
                return 0;
            }else{
                prev->next = curr->next;
                delete_all_processes(curr->processHead);
                free(curr);
                return 0;
            }
        }else{
            prev = curr;
            curr = curr->next;
        }
    }
    return -1;
}

/**
mark group as stopped
**/
int stop_group(struct GroupNode* head, pid_t pgid){
    if(head == NULL){
        return 0;
    }

    if(head->pgid == pgid){
	head->stop = 1;
        stop_processlist(head->processHead);
        return 1;
    }

    return stop_group(head->next, pgid);
}

/**
turn off "stop" boolean
**/
int resume_group(struct GroupNode* head, pid_t pgid){
    if(head == NULL){
        return 0;
    }

    if(head->pgid == pgid){
	head->stop = 0;
        resume_processlist(head->processHead);
        return 1;
    }

    return resume_group(head->next, pgid);
}

/**
get most recent stopped process group
@return:
    pgid
    -1 if not found
**/
int get_most_recent_stopped(struct GroupNode* head){
    struct GroupNode* curr = head;

    while(curr != NULL){
        if(curr->processHead->stopped){
            return curr->pgid;
        }
        curr=curr->next;
    }
    return -1;
}

/**
get pgid of process group at given position in stack
**/
int get_groupid(struct GroupNode* head, int position){
	int i = 1;
	for (i = 1; i < position; i++){
		if (head == NULL){
			return -1;
		}
		head=head->next;
	}
	return head->pgid;
}

/**
print stack with each process group's process pipeling
**/
void print_groupstack(struct GroupNode* head){
    struct GroupNode* curr = head;
    write(STDOUT, "----PRINTING groupstack----\n", 27);
	int stackNumber = 1;   
 while(curr != NULL && curr->pgid != -1){
        printf("[%d] pgid: %d:", stackNumber, curr->pgid);
        print_processlist(curr->processHead);
	if(curr->stop == 1){
		printf(" (stopped)\n");
	}
	else printf(" (running)\n");
        // write(STDOUT, &(curr->pgid), sizeof(curr->pgid));
        curr=curr->next;
	stackNumber++;
    }
    write(STDOUT, "---------------------\n", 22);
}

