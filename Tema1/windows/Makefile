CC = cl
CFLAGS = /W3 /nologo 
OBJ_PARSER = parser.tab.obj parser.yy.obj
OBJ = main.obj utils-win.obj
TARGET = mini-shell.exe

all: build

build: $(TARGET)

$(TARGET): $(OBJ) $(OBJ_PARSER)
	$(CC) $(CFLAGS) $(OBJ) $(OBJ_PARSER) /Fe$(TARGET)

clean:
	del /Q /F $(OBJ) $(OBJ_PARSER) $(TARGET) *~
