#include "processlist.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define STDOUT 1
#define STDIN 0


/**
push new process onto stack
**/
struct ProcessNode* add_to_beginning(struct ProcessNode* head, pid_t pid, int stop){
    if(head->pid == -1){
        printf("starting processlist \n");
        head->pid = pid;
        head->next = NULL;
        head->stopped = stop;
        return head;
    }

    // ProcessNode* newProcessNode = new ProcessNode(pid);
    struct ProcessNode* newProcessNode = (struct ProcessNode *)malloc(sizeof(struct ProcessNode));

    if(newProcessNode == NULL){ 
        write(STDOUT, "Failed to create new ProcessNode.\n", 27);
        return head;
    }
    newProcessNode->pid = pid;
    newProcessNode->next = head;
    newProcessNode->stopped = stop;

    return newProcessNode;
}

/**
return 0 if not found
**/
int search_for_pid(struct ProcessNode* head, pid_t pid){
    if(head == NULL){
        return 0;
    }
    if (head->pid == pid){
        return 1;
    }
    return search_for_pid(head->next, pid);

}

/**
return 1 if deleted succesfully
return 0 if not found
**/
int delete_from_processlist(struct ProcessNode** head, pid_t pid){
    /*
    Iterates through list and removes first occurance of _pid_
    */
    struct ProcessNode * prev;
    struct ProcessNode * curr = *head;

    while(curr != NULL){
        if(curr->pid == pid){
            if(curr==*head){
                *head= curr->next;
                free(curr);
                return 1;
            }else{
                prev->next = curr->next;
                free(curr);
                return 1;
            }
        }else{
            prev = curr;
            curr = curr->next;
        }
    }
    return 0;
}

/**
delete every process
**/
int delete_all_processes(struct ProcessNode* head){
	struct ProcessNode * next;

    printf("in delete_all_processes()\n");

	while(head != NULL){
		next = head->next;
		free(head);
        printf("freed one\n");
		head = next;
	}
    printf("about to return\n");
	return 0;

}

/**
mark process as stopped
**/
void stop_processlist(struct ProcessNode* head){
    if(head == NULL){
        return;
    }
    head->stopped = 1;
    return stop_processlist(head->next);
}

/**
turn off stop boolean
**/
void resume_processlist(struct ProcessNode* head){
    if(head == NULL){
        return;
    }
    head->stopped = 0;
    return resume_processlist(head->next);
}

/**
print entire process list
**/
void print_processlist(struct ProcessNode* head){
    struct ProcessNode* curr = head;
    printf("   Processes: ");
    while(curr != NULL){
	printf("%d", curr->pid);
	if(curr->next != NULL) printf(" | ");
        curr=curr->next;
    }
}

