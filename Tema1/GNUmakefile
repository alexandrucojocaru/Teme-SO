CC = g++
SRC = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))
CC_FLAGS = -Wall -g
LD_FLAGS = -Wall -g
BIN = mini-shell

all: build

build: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LD_FLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CC_FLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(OBJ) $(BIN)
	rm -f $(BIN)
