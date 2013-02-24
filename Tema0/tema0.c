#include <stdio.h>
#include <stdlib.h>

#include "List.h"
#include "Globals.h"

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
    printList(list);
    printf("\n");

    if (findElement(list, stuff[0]))
    	printf("%s found\n", stuff[0]);
    else
    	printf("%s not found\n", stuff[0]);

    addElement(&list, stuff[2]);
    printf("List before deleting 'are'\n");
    printList(list);
    printf("\n");

    deleteElement(&list, "arebibib");
    printf("After delete\n");
    printList(list);
    printf("\n");

    deleteElement(&list, "meremari");
	printf("After delete\n");
	printList(list);
	printf("\n");

	addElement(&list, "mamata");
	addElement(&list, "tutu");
	printf("After add\n");
	printList(list);
	printf("\n");

	if (findElement(list, stuff[1]))
		printf("%s found\n", stuff[1]);
	else
		printf("%s not found\n", stuff[1]);

	deleteElement(&list, "tutu");
	printf("After delete\n");
	printList(list);
	printf("\n");

    destroyList(&list);
#endif

    return 0;
}
