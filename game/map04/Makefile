# DLL makefile
CPP  = g++.exe
CC   = gcc.exe
DLLWRAP=dllwrap.exe

INCS = -I"../../common" -I"../common"

OBJ  = bm_game.o Hermit.o interface.o Pterodactyl.o Swimmer.o
BIN  = "../map04.dll"
DEFFILE=../libmap04.def
STATICLIB=../libmap04.a

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