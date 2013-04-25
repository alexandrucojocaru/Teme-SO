/**
 * SO, 2011
 * Lab #7, Memoria virtuala
 *
 * Task #, Windows
 *
 * Locking memory in order to avoid swaping
 */

/* do not use UNICODE */
#undef _UNICODE
#undef UNICODE

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <windows.h>

#include "utils.h"

#define SIZE	128

static int pageSize = 0x1000;
char msg[] = "Very important real-time data";


int main(void)
{
	char data[SIZE];
	DWORD rc, i;

	rc = VirtualLock(data, SIZE);
	DIE(rc == FALSE, "VirtualLock");

	memcpy(data, msg, strlen(msg));	
	data[strlen(msg)] = '\0';
	printf("data=%s %d\n", data);

	rc = VirtualUnlock(data, SIZE);
	DIE(rc == FALSE, "VirtualUnLock");
	
	return 0;
}