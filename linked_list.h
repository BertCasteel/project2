#ifndef LINKEDLIST_H
#define LINKEDLIST_H

struct Node
{
    int data;
    struct Node *next;
};

struct LinkedList
{
    int count;
    struct Node* head;

    void add_to_end(int);
    int delete_from_list(int);
    void print_list();
    int size();
};
#endif