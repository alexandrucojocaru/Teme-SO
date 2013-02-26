/*
 * @author: Constantin Serban-Radoi 333CA
 *
 * This file contains the header of a Linked List
 *
 * */
#ifndef LIST_H_
#define LIST_H_

#include "Globals.h"

/* Structure for a Node of a list */
typedef struct Node_ {
	char *data;
	struct Node_ *next;
}Node;

typedef Node* List;

/* Deallocates resources and destroys the list */
void destroyList(List *list);

/* Inserts an element to a list. Returns TRUE on success, FALSE otherwise */
int addElement(List *list, const char *elem);

/* Searches an element in a list. Returns TRUE if found, FALSE otherwise */
int findElement(const List list, const char *elem);

/* Deletes an element in a list. Returns TRUE if element was found and deleted,
 * FALSE otherwise */
int deleteElement(List *list, const char *elem);

/* Print List to file. Return TRUE if List is empty, FALSE otherwise. */
int printList(const List list, FILE *file);

#endif
