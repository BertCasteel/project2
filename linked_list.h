// #include "linked_list.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define STDOUT 1
#define STDIN 0

struct Node
{
    pid_t data;
    pid_t grp;
    struct Node *next;
} Node;


struct Node* add_to_end(struct Node* head, pid_t data){

    // Node* newNode = new Node(data);
    struct Node* newNode = (struct Node *)malloc(sizeof(struct Node));

    if(newNode == NULL){ 
        write(STDOUT, "Failed to create new Node.\n", 27);
        return head;
    }
    newNode->data = data;
    newNode->next = NULL;

    if(head == NULL){
        head = newNode;
        return head;
    }

    struct Node* curr = head;
    while(curr->next != NULL){
        curr = curr->next;
    }
    curr->next = newNode;
    return head;
}

int delete_from_list(struct Node** head, pid_t data){
    /*
    Iterates through list and removes first occurance of _data_
    */
    struct Node * prev;
    struct Node * curr = *head;

    while(curr != NULL){
        if(curr->data == data){
            if(curr==*head){
                *head=curr->next;
                free(curr);
                return 0;
            }else{
                prev->next = curr->next;
                free(curr);
                return 0;
            }
        }else{
            prev = curr;
            curr = curr->next;
        }
    }
    write(STDOUT, "Unable to find that in list\n", 28);
    return -1;
}

void print_list(struct Node* head){
    struct Node* curr = head;
    write(STDOUT, "----PRINTING LIST----\n", 22);
    while(curr != NULL){
        printf("%d\n", curr->data);
        // write(STDOUT, &(curr->data), sizeof(curr->data));
        curr=curr->next;
    }
    write(STDOUT, "---------------------\n", 22);
}

// int main(){
//     struct Node* head = (struct Node*)malloc(sizeof(struct Node));
//     head = NULL;
//     for (int i = 0; i < 10; ++i)
//     {
//         // printf("trying to add %d\n", i);
//         head = add_to_end(head, i);
//     }
//     print_list(head);

//     delete_from_list(&head, 4);

//     delete_from_list(&head, 7);

//     print_list(head);

//     delete_from_list(&head, 7);

//     print_list(head);

//     for (int i = 0; i < 11; ++i)
//     {
//         // printf("trying to add %d\n", i);
//         delete_from_list(&head, i);
//         print_list(head);
//     }

// }