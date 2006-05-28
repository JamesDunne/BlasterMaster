CPP  = g++.exe
CC   = gcc.exe

INCS = -I"../common"

OBJ  = tilepager.o ../common/newmap.o
BIN  = "../Map Extractor.exe"

LIBS = -mwindows -lmingw32 -lSDLmain -lSDL -lpng -lz
CFLAGS = $(INCS) -Dmain=SDL_main -g

.PHONY:	clean deps

$(BIN):	deps $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LIBS)

clean:
	rm -f $(OBJ)

deps:
	$(CC) $(CFLAGS) -MM *.c > dependencies

-include dependencies