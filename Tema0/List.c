/*
 * @author: Constantin Serban-Radoi 333CA
 *
 * This file contains the implementation of a Linked List
 *
 * */
#include "List.h"
#include "Globals.h"
#include "string.h"

void destroyList(List *list) {
	Node *head = *list;
	Node *p;
	while (head) {
		p = head;
		head = head->next;
		free(p->data);
		free(p);
	}
}

int addElement(List *list, char *elem) {
	Node *head = *list;
	Node *newNode = (Node *)calloc(1, sizeof(Node));
	DIE(newNode == NULL, "Could not create new Node");

	/* Need to allocate len + 1, because of '\0' character */
	int len = strlen(elem);
	newNode->data = (char *)calloc(len + 1, sizeof(char));
	DIE(newNode->data == NULL, "Could not create space for data");
	memcpy(newNode->data, elem, len + 1);
	newNode->next = NULL;

	if (head == NULL) {
		*list = newNode;
		return TRUE;
	}

	while (head->next) {
		head = head->next;
	}

	head->next = newNode;
	return TRUE;
}

int findElement(List list, char *elem) {
	Node *head = list;
	if (head == NULL)
		return FALSE;

	while (head->next) {
		if (strcmp(elem, head->data) == 0)
			return TRUE;
		head = head->next;
	}
	return FALSE;
}

int deleteElement(List *list, char *elem) {
	Node *head = *list;
	if (head == NULL)
		return FALSE;

	/* Element is at the beginning of the list */
	if (strcmp(elem, head->data) == 0) {
		*list = (*list)->next;
		free(head->data);
		free(head);
		return TRUE;
	}

	while (head->next) {
		if (strcmp(head->next->data, elem) == 0) {
			Node *temp = head->next;
			head->next = head->next->next;
			free(temp->data);
			free(temp);
			return TRUE;
		}
		head = head->next;
	}
	return FALSE;
}

void printList(List list) {
	Node *head = list;
	while (head) {
		printf("%s", head->data);
		head = head->next;
		if (head)
			printf(" ");
	}
}
