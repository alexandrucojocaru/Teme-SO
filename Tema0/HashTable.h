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

/* Structure that holds a Pair key-value for words with the same hash */
typedef struct Bucket_ {
	List words;
}Bucket;

/* Structure for a Hash Table */
typedef struct HashTable_ {
	unsigned int hashSize;
	Bucket *table;
}HashTable;

/* Creates a Hash Table with a given hashSize for the hash function */
void initHashTable(HashTable **table, const unsigned int hashSize);

/* Destroys a Hash Table and releases allocated memory. */
void destroyHashTable(HashTable **table);

/* Adds a word to the Hash Table. If it already exists, it does nothing.
 * Returns TRUE if word is added successfully, FALSE otherwise */
int addWord(HashTable *table, const char *word);

/* Removes a word from the Hash Table. The word could be inexistent. Returns
 * TRUE if element is found and deleted, FALSE otherwise */
int deleteWord(HashTable *table, const char *word);

/* Searches for a word in the Hash Table. Returns TRUE if found, FALSE
 * otherwise */
int findWord(const HashTable table, const char *word);

/* Prints bucket to a FILE. Each word is separated by space. Returns TRUE if
 * Bucket elements exist, FALSE otherwise. */
int printBucket(
		const HashTable table,
		FILE *file,
		const unsigned int bucketIndex);

/* Prints HashTable to a FILE. Each bucket is printed on a separate line. */
void printTable(const HashTable table, FILE *file);

/* Doubles the size of the hash function of the Hash Table. Recomputes the
 * entire table by creating a new one. Returns TRUE on success, FALSE otherwise
 * */
int doubleTable(HashTable **table);

/* Halves the size of the hash function of the Hash Table. Recomputes the
 * entire table by creating a new one. Returns TRUE on success, FALSE otherwise
 * */
int halveTable(HashTable **table);

#endif
