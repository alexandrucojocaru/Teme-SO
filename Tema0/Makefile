CC = cl
CFLAGS   = /Wall /nologo -D_CRT_SECURE_NO_DEPRECATE
EXE = tema0.exe
C_DIR = .
OBJ_DIR = .

all: build

build: {$(OBJ_DIR)}*.obj
	$(CC) $(CFLAGS) /Fe$(EXE) $**

{$(C_DIR)}*.c{$(OBJ_DIR)}*.obj:
	$(CC) $(CFLAGS) /c $<

.PHONY: clean

clean:
	del {$(OBJ_DIR)}*.obj
	del $(EXE)