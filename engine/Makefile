# Project: Blaster Master
# Makefile created by Dev-C++ 4.9.9.0

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = bm.o bm_game.o entities.o opengl.o sound.o ../common/newmap.o $(RES)
LINKOBJ  = bm.o bm_game.o entities.o opengl.o sound.o ../common/newmap.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -mwindows -lmingw32 -lopengl32 -lglu32 -lSDLmain -lSDL -lSDL_image -lSDL_mixer -lz
INCS =  -I"C:/Dev-Cpp/include"  -I"../common" 
CXXINCS =  -I"C:/Dev-Cpp/include/c++/3.3.1"  -I"C:/Dev-Cpp/include/c++/3.3.1/mingw32"  -I"C:/Dev-Cpp/include/c++/3.3.1/backward"  -I"C:/Dev-Cpp/lib/gcc-lib/mingw32/3.3.1/include"  -I"C:/Dev-Cpp/include"  -I"../common" 
BIN  = "../Blaster Master.exe"
CXXFLAGS = $(CXXINCS) -fexpensive-optimizations
CFLAGS = $(INCS) -Dmain=SDL_main -fexpensive-optimizations

.PHONY: all all-before all-after clean clean-custom

all: all-before "../Blaster Master.exe" all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o "..\Blaster Master.exe" $(LIBS)

bm.o: bm.c
	$(CC) -c bm.c -o bm.o $(CFLAGS)

bm_game.o: bm_game.c
	$(CC) -c bm_game.c -o bm_game.o $(CFLAGS)

entities.o: entities.c
	$(CC) -c entities.c -o entities.o $(CFLAGS)

opengl.o: opengl.c
	$(CC) -c opengl.c -o opengl.o $(CFLAGS)

sound.o: sound.c
	$(CC) -c sound.c -o sound.o $(CFLAGS)

../common/newmap.o: ../common/newmap.c
	$(CC) -c ../common/newmap.c -o ../common/newmap.o $(CFLAGS)
