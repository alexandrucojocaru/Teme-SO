CC = gcc
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
CC_FLAGS = -Wall -g
LD_FLAGS = -Wall -g
BIN = tema0

build: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LD_FLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CC_FLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(OBJ) $(BIN)
	rm -f $(BIN)
