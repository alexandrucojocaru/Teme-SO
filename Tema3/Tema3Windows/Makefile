# Makefile Tema 3 SO
# Constantin Serban-Radoi 333CA
# Aprilie 2013
CC = cl
CFLAGS = /nologo /W3 -D_CRT_SECURE_NO_DEPRECATE
LINK = link

TARGET = libvmsim.dll
TARGET_LIB = libvmsim.lib

OBJECTS = common_win.obj vmsim.obj
HEADERS = common.h debug.h util.h vmsim.h
TARGET_DEP = $(OBJECTS) $(HEADERS)

all: build

build: $(TARGET)

$(TARGET): $(TARGET_DEP)
	$(LINK) /nologo /dll /out:$(TARGET) /implib:$(TARGET_LIB) $(OBJECTS)

.PHONY: clean
	
clean:
	del /Q /F $(TARGET) $(OBJECTS) $(TARGET_LIB) *~
