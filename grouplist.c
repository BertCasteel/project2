#include "grouplist.h"
#include "processlist.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#define STDOUT 1
#define STDIN 0
#define EMPTY_PROCESS_GROUP 0

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
    return 0;
}

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


void print_grouplist(struct GroupNode* head){
    struct GroupNode* curr = head;
    write(STDOUT, "----PRINTING GROUPLIST----\n", 27);
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
/*
int main(){
     struct GroupNode* head = (struct GroupNode *)malloc(sizeof(struct GroupNode));;
     int i;
     int group = -1;
     for (i = 0; i < 10; ++i)
     {
         // printf("trying to add %d\n", i);
         head = add_new_process(&head, i/2, i, 0);
         if (head->pgid % 2 == 0){
            group++;
         }
     }
     print_grouplist(head);
     int groupid = -1;
     remove_process(&head, 7, &groupid);
     print_grouplist(head);
     remove_process(&head, 6, &groupid);
     printf("group id: %d\n", groupid);
     print_grouplist(head);
     stop_group(head, 2);
     print_grouplist(head);
     resume_group(head, 2);
     print_grouplist(head);


//     delete_from_list(&head, 4);

//     delete_from_list(&head, 7);
//     printf("%d\n", search_for_pid(head, 8));
//     print_list(head);

//     delete_from_list(&head, 7);

//     print_list(head);

//     for (int i = 0; i < 11; ++i)
//     {
//         // printf("trying to add %d\n", i);
//         delete_from_list(&head, i);
//         print_list(head);
//     }

     return 0;
}
*/
