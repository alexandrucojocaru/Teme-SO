#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "Globals.h"
#include "HashTable.h"
#include "string.h"

/* Tests for list */
void testList(void) {
	char stuff[3][10] = {"anamimicu", "test2", "testlung3"};
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
void testHash(void) {
	HashTable* table = NULL;
	printf("\nTests for HashTable\n\n");
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
			"Hash(jeanie)=%u\tHash(megen)=%u\tHash(crocobaur)=%u\t"\
			"Hash(ginne)=%u\n",
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
int parseArguments(int argc) {
	DIE(argc < 2, "Usage error: <hashSize> [<file1> .. <filen>]");

	return argc - 1;
}

/* Write find to file */
void printFind(int result, FILE *f) {
	if (result == TRUE)
		fprintf(f, "True\n\n");
	else if (result == FALSE)
		fprintf(f, "False\n\n");
}

/* Select command */
void selectCommand(char buf[3][BUFSZ], HashTable **hashTable, char cmd[CMDSZ]) {
	if (strcmp(cmd, "add") == 0) {
		addWord(*hashTable, buf[1]);
		return;
	}
	if (strcmp(cmd, "remove") == 0) {
		deleteWord(*hashTable, buf[1]);
		return;
	}
	if (strcmp(cmd, "clear") == 0) {
		unsigned int size = (*hashTable)->hashSize;
		destroyHashTable(hashTable);
		initHashTable(hashTable, size);
		return;
	}
	if (strcmp(cmd, "find") == 0) {
		if (buf[1]) {
			int result = findWord(**hashTable, buf[1]);
			if (buf[2] && strcmp(buf[2], "") != 0) {
				FILE *f;
				f = fopen(buf[2], "a");
				DIE(f == NULL, "Could not open file for update");
				printFind(result, f);
				fclose(f);
			}
			else {
				printFind(result, stdout);
			}
		}

		return;
	}
	if (strcmp(cmd, "print_bucket") == 0) {
		unsigned int bucket;
		bucket = atoi(buf[1]);
		if (buf[2] && strcmp(buf[2], "") != 0) {
			FILE *f;
			f = fopen(buf[2], "a");
			DIE(f == NULL, "Could not open file for append");
			if (printBucket(**hashTable, f, bucket) == TRUE)
				fprintf(f, "\n\n");
			else
				fprintf(f, "\n");
			fclose(f);
		}
		else {
			if (printBucket(**hashTable, stdout, bucket) == TRUE)
				fprintf(stdout, "\n\n");
			else
				fprintf(stdout, "\n");
		}

		return;
	}
	if (strcmp(cmd, "print") == 0) {
		if (buf[1] && strcmp(buf[1], "") != 0) {
			FILE *f;
			f = fopen(buf[1], "a");
			DIE(f == NULL, "Could not open file for append");
			printTable(**hashTable, f);
			fprintf(f, "\n");	/* Only one \n because of those from list */
			fclose(f);
		}
		else {
			printTable(**hashTable, stdout);
			printf("\n");	/* Only one \n because of those from list print */
		}
		return;
	}
	if (strcmp(cmd, "resize") == 0) {
		if (strcmp(buf[1], "double") == 0) {
			doubleTable(hashTable);
		}
		else if (strcmp(buf[1], "halve") == 0) {
			halveTable(hashTable);
		}
		return;
	}
}

/* Parse command by extracting tokens */
void parseCommand(char buf[3][BUFSZ], HashTable **hashTable) {
	char *token;
	char command[CMDSZ];
	int i;

	token = strtok(buf[0], " \n");
	if (token == NULL || strcmp(token, "") == 0)
		return;
	strcpy(command, token);

	for (i = 1; i <= 2; ++i) {
		token = strtok(NULL, " \n");
		if (token) {
			strcpy(buf[i], token);
		}
		else
			break;
	}

	selectCommand(buf, hashTable, command);
}

/* Process input file and creates corresponding output */
void processFile(FILE *file, HashTable **hashTable) {
	char buf[3][BUFSZ];
	while (fgets(buf[0], BUFSZ, file) != NULL) {
		parseCommand(buf, hashTable);
		memset(buf, 0, 3 * BUFSZ * sizeof(char));
	}
}

/* Process input and create required output */
void processWork(int argc, char *argv[]) {
	int numParams;
	int hashSize;
	HashTable *hashTable = NULL;

	numParams = parseArguments(argc);
	hashSize = atoi(argv[1]);
	fprintf(stderr, "hashSize: %d", hashSize);

	initHashTable(&hashTable, hashSize);

	/* Input from stdin */
	if (numParams == 1) {
		processFile(stdin, &hashTable);
	}
	else {
		int i;
		for (i = 2; i < argc; ++i) {
			FILE *f;
			f = fopen(argv[i], "r");
			DIE(f == NULL, "Could not open file");
			processFile(f, &hashTable);
			fclose(f);
		}
	}

	destroyHashTable(&hashTable);

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
