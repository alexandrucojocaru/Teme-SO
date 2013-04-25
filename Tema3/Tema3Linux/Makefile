# Makefile for Tema 3 SO
# Author: Constantin Serban-Radoi 333CA
CC = g++
CFLAGS = -Wall -g #-DDEBUG -DLOG_LEVEL=LOG_DEBUG

LIBVMSIM = libvmsim.so

LIBVMSIM_DEP_OBJS = vmsim.o vmsim_mem.o common_lin.o
LIBVMSIM_DEP = $(LIBVMSIM_DEP_OBJS) vmsim.h vmsim_mem.h common.h debug.h util.h

all: build

build: $(LIBVMSIM)

$(LIBVMSIM): $(LIBVMSIM_DEP)
	$(CC) $(CFLAGS) -shared $(LIBVMSIM_DEP_OBJS) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

.PHONY: clean

clean:
	-rm -f *~ *.o $(LIBVMSIM_DEP_OBJS) $(LIBVMSIM)
