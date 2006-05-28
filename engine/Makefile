CPP  = g++.exe
CC   = gcc.exe

INCS = -I"../common"

OBJ  = bm.o bm_game.o entities.o opengl.o sound.o ../common/newmap.o
BIN  = "../Blaster Master.exe"

LIBS = -mwindows -lmingw32 -lopengl32 -lglu32 -lSDLmain -lSDL -lSDL_image -lSDL_mixer -lz
CFLAGS = $(INCS) -Dmain=SDL_main -g

.PHONY:	clean deps

$(BIN):	deps $(OBJ)
	$(CC) $(OBJ) -o $(BIN) $(LIBS)

clean:
	rm -f $(OBJ)

deps:
	$(CC) $(CFLAGS) -MM *.c > dependencies

-include dependencies