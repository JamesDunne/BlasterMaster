# DLL makefile
CPP  = g++.exe
CC   = gcc.exe
DLLWRAP=dllwrap.exe

INCS = -I"../common"

OBJ  = interface.o
BIN  = "../BMGame.dll"
DEFFILE=../libBMGame.def
STATICLIB=../libBMGame.a

LIBS = -mwindows -lmingw32 -lopengl32
CFLAGS = $(INCS) -g

.PHONY:	clean deps

$(BIN):	deps $(OBJ)
	$(DLLWRAP) --output-def $(DEFFILE) --implib $(STATICLIB) $(OBJ) $(LIBS) -o $(BIN)

clean:
	rm -f $(OBJ)

deps:
	$(CC) $(CFLAGS) -MM *.c > dependencies

-include dependencies