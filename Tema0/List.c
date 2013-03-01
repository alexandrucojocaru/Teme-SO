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

int addElement(List *list, const char *elem) {
	Node *head = NULL;
	Node *newNode = NULL;
	unsigned int len;
	if (strcmp("", elem) == 0)
		return FALSE;
	head = *list;
	newNode = (Node *)calloc(1, sizeof(Node));
	DIE(newNode == NULL, "Could not create new Node");

	/* Need to allocate len + 1, because of '\0' character */
	len = strlen(elem);
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

int findElement(const List list, const char *elem) {
	Node *head = list;
	if (head == NULL)
		return FALSE;

	while (head) {
		if (strcmp(elem, head->data) == 0)
			return TRUE;
		head = head->next;
	}
	return FALSE;
}

int deleteElement(List *list, const char *elem) {
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

int printList(const List list, FILE *file) {
	Node *head = list;
	if (head == NULL || (head && head->data == NULL) || (head &&
			strcmp("", head->data) == 0))
		return FALSE;
	while (head) {
		fprintf(file, "%s", head->data);
		head = head->next;
		if (head)
			fprintf(file, " ");
	}
	return TRUE;
}
