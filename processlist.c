#include "processlist.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define STDOUT 1
#define STDIN 0


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

int search_for_pid(struct ProcessNode* head, pid_t pid){
    if(head == NULL){
        return 0;
    }
    if (head->pid == pid){
        return 1;
    }
    return search_for_pid(head->next, pid);

}

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


int delete_all_processes(struct ProcessNode* head){
	struct ProcessNode * next;

	while(head != NULL){
		next = head->next;
		free(head);
		head = next;
	}
	return 0;

}

void stop_processlist(struct ProcessNode* head){
    if(head == NULL){
        return;
    }
    head->stopped = 1;
    return stop_processlist(head->next);
}

void resume_processlist(struct ProcessNode* head){
    if(head == NULL){
        return;
    }
    head->stopped = 0;
    return resume_processlist(head->next);
}

void print_processlist(struct ProcessNode* head){
    struct ProcessNode* curr = head;
    printf("    Processes:  ");
    while(curr != NULL){
        if(curr->stopped){
            printf("%d(s)->", curr->pid);
        }
        else{
            printf("%d-> ", curr->pid);
        }
        curr=curr->next;
    }
    printf("\n");
}
/*
int main(){
     struct ProcessNode* head = (struct ProcessNode*)malloc(sizeof(struct ProcessNode));
     head = NULL;
     int i;
     for (i = 0; i < 10; ++i)
     {
         // printf("trying to add %d\n", i);
         head = add_to_beginning(head, i, 0);
     }
     print_processlist(head);

//     delete_from_list(&head, 4);

//     delete_from_list(&head, 7);
     printf("%d\n", search_for_pid(head, 8));
//     print_list(head);

     delete_all_processes(head);
//     delete_from_list(&head, 7);

//     print_list(head);

//     for (int i = 0; i < 11; ++i)
//     {
//         // printf("trying to add %d\n", i);
//         delete_from_list(&head, i);
//         print_list(head);
//     }

     return 0;
}*/
