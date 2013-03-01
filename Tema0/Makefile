CC = cl
CFLAGS = /Wall /wd4127 /nologo -D_CRT_SECURE_NO_DEPRECATE
BIN = tema0.exe

all: build

build: tema0.obj HashTable.obj hash.obj List.obj
	$(CC) $(CFLAGS) /Fe$(EXE) $**

tema0.obj: tema0.c
	$(CC) -c $(CFLAGS) tema0.c

HashTable.obj: HashTable.h HashTable.c
	$(CC) -c $(CFLAGS) HashTable.c

hash.obj: hash.h hash.c
	$(CC) -c $(CFLAGS) hash.c

List.obj: List.h List.c
	$(CC) -c $(CFLAGS) List.c

.PHONY: clean

clean:
	del /Q /F *.obj
	del /Q /F $(BIN)