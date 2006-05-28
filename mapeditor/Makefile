CPP  = g++.exe
CC   = gcc.exe

INCS = -I"../common"

OBJ  = bm_game.o bmedit.o font.o glwindows.o opengl.o ../common/newmap.o
BIN  = "../Map Editor.exe"

LIBS = -mwindows -lmingw32 -lopengl32 -lglu32 -lSDLmain -lSDL -lSDL_image -lz
CFLAGS = $(INCS) -Dmain=SDL_main -g

.PHONY:	clean deps

$(BIN):	deps $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LIBS)

clean:
	rm -f $(OBJ)

deps:
	$(CC) $(CFLAGS) -MM *.c > dependencies

-include dependencies