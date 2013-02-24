/*
 * @author: Constantin Serban-Radoi 333CA
 *
 * This file contains the header of a Linked List
 *
 * */
#ifndef LIST_H_
#define LIST_H_

/* Structure for a Node of a list */
typedef struct Node_ {
	char *data;
	struct Node_ *next;
}Node;

typedef Node* List;

/* Deallocates resources and destroys the list */
void destroyList(List *list);

/* Inserts an element to a list. Returns TRUE on success, FALSE otherwise */
int addElement(List *list, char *elem);

/* Searches an element in a list. Returns TRUE if found, FALSE otherwise */
int findElement(List list, char *elem);

/* Deletes an element in a list. Returns TRUE on success, FALSE otherwise */
int deleteElement(List *list, char *elem);

/* Print List to stdout */
void printList(List list);

#endif
