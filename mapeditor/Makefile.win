# Project: Map Editor
# Makefile created by Dev-C++ 4.9.9.0

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = bm_game.o bmedit.o font.o glwindows.o opengl.o ../common/newmap.o $(RES)
LINKOBJ  = bm_game.o bmedit.o font.o glwindows.o opengl.o ../common/newmap.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -mwindows -lmingw32 -lopengl32 -lglu32 -lSDLmain -lSDL -lSDL_image -lzdll
INCS =  -I"C:/Dev-Cpp/include"  -I"../common" 
CXXINCS =  -I"C:/Dev-Cpp/include/c++/3.3.1"  -I"C:/Dev-Cpp/include/c++/3.3.1/mingw32"  -I"C:/Dev-Cpp/include/c++/3.3.1/backward"  -I"C:/Dev-Cpp/lib/gcc-lib/mingw32/3.3.1/include"  -I"C:/Dev-Cpp/include"  -I"../common" 
BIN  = "../Map Editor.exe"
CXXFLAGS = $(CXXINCS)  
CFLAGS = $(INCS) -Dmain=SDL_main  -g

.PHONY: all all-before all-after clean clean-custom

all: all-before "../Map Editor.exe" all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o "..\Map Editor.exe" $(LIBS)

bm_game.o: bm_game.c
	$(CC) -c bm_game.c -o bm_game.o $(CFLAGS)

bmedit.o: bmedit.c
	$(CC) -c bmedit.c -o bmedit.o $(CFLAGS)

font.o: font.c
	$(CC) -c font.c -o font.o $(CFLAGS)

glwindows.o: glwindows.c
	$(CC) -c glwindows.c -o glwindows.o $(CFLAGS)

opengl.o: opengl.c
	$(CC) -c opengl.c -o opengl.o $(CFLAGS)

../common/newmap.o: ../common/newmap.c
	$(CC) -c ../common/newmap.c -o ../common/newmap.o $(CFLAGS)
