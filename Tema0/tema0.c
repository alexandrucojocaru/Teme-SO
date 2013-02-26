#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "Globals.h"
#include "HashTable.h"

int main(int argc, char *argv[]) {
    printf("Hello \n");
    
#if DBG
    /* List tests */
    char stuff[3][10] = {"anamimicu", "arebibib", "meremari"};

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
#endif

#if DBG
    /* Hash tests */
    printf("\nTests for HashTable\n\n");
    HashTable *table = NULL;
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

	printf("Hash(jeanie)=%u\tHash(megen)=%u\tHash(crocobaur)=%u\tHash(ginne)="\
			"%u\n",
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

#endif

    return 0;
}
