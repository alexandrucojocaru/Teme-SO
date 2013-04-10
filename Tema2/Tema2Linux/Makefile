# Makefile for Tema 2 SO
# Author: Constantin Serban-Radoi 333CA
CC = gcc

CFLAGS = -Wall -g
LDFLAGS = -lsompi -L.

GENERICS = unix_queue.o unix_sem.o unix_shm.o
MPI_DEP_OBJS = mpi.o $(GENERICS)
MPI_DEP = $(MPI_DEP_OBJS) mpi.h mpi_err.h generic_queue.h generic_sem.h generic_shm.h

BIN = mpirun
MPISO = libsompi.so
MAIN_OBJ = main.o

all: build

build: $(MPISO) $(BIN)

$(BIN): $(MAIN_OBJ) $(GENERICS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -lrt 

$(MPISO): $(MPI_DEP)
	$(CC) $(CFLAGS) -shared $(MPI_DEP_OBJS) -o $@ -lrt

$(MAIN_OBJ): main.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(MPI_DEP_OBJS) $(MPISO)
	rm -f $(BIN) $(MAIN_OBJ)
