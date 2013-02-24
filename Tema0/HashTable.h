/*
 * @author: Constantin Serban-Radoi 333CA
 *
 * This file contains the header of a Hash Table
 *
 * */
#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include "List.h"
#include "Globals.h"
#include "hash.h"

typedef List Bucket;

/* Structure for a Hash Table */
typedef struct HashTable_ {
	int hashSize;
	Bucket *table;
}HashTable;

/* Creates a Hash Table with a given hashSize for the hash function */
void initHashTable(HashTable *table, int hashSize);

/* Adds a word to the Hash Table. If it already exists, it does nothing.
 * Returns TRUE if word is added successfully, FALSE otherwise */
int addWord(HashTable *table, char *word);

/* Removes a word from the Hash Table. The word could be inexistent. Returns
 * TRUE on success, FALSE otherwise */
int deleteWord(HashTable *table, char *word);

/* Searches for a word in the Hash Table. Returns TRUE if found, FALSE
 * otherwise */
int findWord(HashTable table, char *word);

/* Prints bucket TODO: directly to FILE (stdout or f), or to buffer */

#endif
