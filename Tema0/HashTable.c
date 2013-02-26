/*

 * @author: Constantin Serban-Radoi 333CA
 *
 * This file contains the implementation of a Hash Table
 *
 */

#include "hash.h"
#include "HashTable.h"

/* Resizes a Hash Table by multiplying the hashSize with multiplyFactor and
 * recomputing the entire table */
static int resizeTable(HashTable **table, const float multiplyFactor);

void initHashTable(HashTable **table, const unsigned int hashSize) {
	*table = (HashTable *)calloc(1, sizeof(HashTable));
	DIE((*table) == NULL, "Could not allocate enough memory for Hash Table");

	(*table)->hashSize = hashSize;
	(*table)->table = (Bucket *)calloc(hashSize, sizeof(Bucket));
	DIE((*table)->table == NULL, "Could not allocate enough memory for buckets");
}

void destroyHashTable(HashTable **table) {
	if ((*table) == NULL) {
		return;
	}

	int i;
	for (i = 0; i < (*table)->hashSize; ++i) {
		destroyList(&(*table)->table[i].words);
	}
	free((*table)->table);
	free((*table));
}

int addWord(HashTable *table, const char *word) {
	DIE(table == NULL, "Table inexistnt");

	unsigned int index = hash(word, table->hashSize);
	if (findElement(table->table[index].words, word) == FALSE) {
		int retVal = addElement(&table->table[index].words, word);
		if (retVal == TRUE)
			return TRUE;
	}
	else
		return TRUE;

	return FALSE;
}

int deleteWord(HashTable *table, const char *word) {
	DIE(table == NULL, "Table inexistent");

	unsigned int index = hash(word, table->hashSize);
	return deleteElement(&table->table[index].words, word);
}

int findWord(const HashTable table, const char *word) {

	unsigned int index = hash(word, table.hashSize);
	return findElement(table.table[index].words, word);
}

int printBucket(
		const HashTable table,
		FILE *file,
		const unsigned int bucketIndex) {
	DIE(bucketIndex >= table.hashSize, "Index out of bounds");

	return printList(table.table[bucketIndex].words, file);
}

void printTable(const HashTable table, FILE *file) {
	int i;
	for (i = 0; i < table.hashSize; ++i) {
		if (printBucket(table, file, i) == TRUE) {
			fprintf(file, "\n");
		}
	}
}

int doubleTable(HashTable **table) {
	return resizeTable(table, 2);
}

int halveTable(HashTable **table) {
	return resizeTable(table, 0.5);
}

static int resizeTable(HashTable **table, const float multiplyFactor) {
	HashTable *newTable = NULL;
	int i;
	initHashTable(&newTable, (unsigned int)(*table)->hashSize * multiplyFactor);

	for (i = 0; i < (*table)->hashSize; ++i) {
		List list = (*table)->table[i].words;
		while (list) {
			addWord(newTable, list->data);
			list = list->next;
		}
	}
	destroyHashTable(table);
	*table = newTable;
	return TRUE;
}