# DLL makefile
CPP  = g++.exe
CC   = gcc.exe
DLLWRAP=dllwrap.exe

INCS = -I"../../common" -I"../common"

OBJ  = bm_game.o interface.o SubBoss01.o
BIN  = "../map00.dll"
DEFFILE=../libmap00.def
STATICLIB=../libmap00.a

LIBS = -mwindows -lopengl32
CFLAGS = $(INCS) -g

.PHONY:	clean deps

$(BIN):	deps $(OBJ)
	$(DLLWRAP) --output-def $(DEFFILE) --implib $(STATICLIB) $(OBJ) $(LIBS) -o $(BIN)

clean:
	rm -f $(OBJ)

deps:
	$(CC) $(CFLAGS) -MM *.c > dependencies

-include dependencies