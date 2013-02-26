#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "Globals.h"
#include "HashTable.h"

/* Tests for list */
void testList() {
	char stuff[3][10] = { };
	List list = NULL;
	addElement(&list, stuff[0]);
	addElement(&list, stuff[1]);
	printf("%s %s %s\n", stuff[0], stuff[1], stuff[2]);
	printf("Lista actuală: \n");
	printList(list, stdout);
	printf("\n");
	if (findElement(list, stuff[0]))
		printf("%s found\n", stuff[0]);
	else
		printf("%s not found\n", stuff[0]);

	addElement(&list, stuff[2]);
	printf("List before deleting 'are'\n");
	printList(list, stdout);
	printf("\n");
	deleteElement(&list, "arebibib");
	printf("After delete\n");
	printList(list, stdout);
	printf("\n");
	deleteElement(&list, "meremari");
	printf("After delete\n");
	printList(list, stdout);
	printf("\n");
	addElement(&list, "mamata");
	addElement(&list, "tutu");
	printf("After add\n");
	printList(list, stdout);
	printf("\n");
	if (findElement(list, stuff[1]))
		printf("%s found\n", stuff[1]);
	else
		printf("%s not found\n", stuff[1]);

	deleteElement(&list, "tutu");
	printf("After delete\n");
	printList(list, stdout);
	printf("\n");
	destroyList(&list);
}

/* Tests for HashTable */
void testHash() {
	printf("\nTests for HashTable\n\n");
	HashTable* table = NULL;
	initHashTable(&table, 5);
	addWord(table, "jeanie");
	addWord(table, "megen");
	addWord(table, "crocobaur");
	addWord(table, "ginne");
	printTable(*table, stdout);
	printf("Find megen = %d\n", findWord(*table, "megen"));
	addWord(table, "megen");
	printTable(*table, stdout);
	addWord(table, "jeanie");
	printTable(*table, stdout);
	printf(
			"Hash(jeanie)=%u\tHash(megen)=%u\tHash(crocobaur)=%u\tHash(ginne)=%u\n",
			hash("megen", 4), hash("jeanie", 4), hash("crocobaur", 4),
			hash("ginne", 4));
	printBucket(*table, stdout, 1);
	printf("\n");
	deleteWord(table, "megen");
	printf("Table after deleting 'megen'\n");
	printTable(*table, stdout);
	printf("Current hashSize: %u\n", table->hashSize);
	halveTable(&table);
	printf("Halved table hashSize: %u\n", table->hashSize);
	printTable(*table, stdout);
	doubleTable(&table);
	printf("Doubled table hashSize: %u\n", table->hashSize);
	printTable(*table, stdout);
	destroyHashTable(&table);
}

/* Check arguments. Returns the number of given parameters*/
int parseArguments(int argc, char *argv[]) {
	DIE(argc < 2, "Usage error: <hashSize> [<file1> .. <filen>]");

	return argc - 1;
}

/* Process file */
void processFile(FILE *file) {

}

/* Process input and create required output */
void processWork(int argc, char *argv[]) {
	int numParams;
	int hashSize;

	numParams = parseArguments(argc, argv);
	hashSize = atoi(argv[1]);
	fprintf(stderr, "hashSize: %d", hashSize);

	/* Input from stdin */
	if (numParams == 1) {
		processFile(stdin);
	}
	else {
		int i;
		for (i = 2; i < argc; ++i) {
			FILE *f;
			f = fopen(argv[i], "r");
			DIE(f == NULL, "Could not open file");
			processFile(f);
			fclose(f);
		}
	}

}

int main(int argc, char *argv[]) {
    

#if DBG
    /* List tests */
	testList();
#endif

#if DBG
    /* Hash tests */
	testHash();
#endif

	processWork(argc, argv);


    return 0;
}
