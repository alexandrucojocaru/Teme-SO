# Makefile Tema 2 SO
# Constantin Serban-Radoi 333CA
CC = cl
CFLAGS = /nologo /W3 -D_CRT_SECURE_NO_DEPRECATE

LINK = link

O = .
S = .

GENERICS = win_queue.obj win_sem.obj win_shm.obj
MPI_DEP_OBJ = mpi.obj $(GENERICS)
MPI_DEP = $(MPI_DEP_OBJ) mpi.h mpi_err.h generic_queue.h generic_sem.h generic_shm.h common.h
MAIN_OBJ = main.obj 

BIN = mpirun.exe
MPISO = sompi.dll
MPISO_LIB = sompi.lib

all: build

build: $(BIN) $(MPISO)

$(BIN): $(MAIN_OBJ) $(GENERICS)
	$(CC) $(CFLAGS) /Fe$@ $**

$(MPISO): $(MPI_DEP)
	$(LINK) /nologo /dll /out:$(MPISO) /implib:$(MPISO_LIB) $(MPI_DEP_OBJ)

clean:
	del /Q /F $(BIN) $(MPISO) $(MPISO_LIB) $(MPI_DEP_OBJ) $(MAIN_OBJ) *~
