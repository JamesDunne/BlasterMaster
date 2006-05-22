# Project: map08
# Makefile created by Dev-C++ 4.9.9.0

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = bm_game.o Boss.o interface.o $(RES)
LINKOBJ  = bm_game.o Boss.o interface.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -lopengl32  
INCS =  -I"C:/Dev-Cpp/include"  -I"../../common"  -I"../common_dungeon" 
CXXINCS =  -I"C:/Dev-Cpp/include/c++/3.3.1"  -I"C:/Dev-Cpp/include/c++/3.3.1/mingw32"  -I"C:/Dev-Cpp/include/c++/3.3.1/backward"  -I"C:/Dev-Cpp/lib/gcc-lib/mingw32/3.3.1/include"  -I"C:/Dev-Cpp/include"  -I"../../common"  -I"../common_dungeon" 
BIN  = ../map08.dll
CXXFLAGS = $(CXXINCS)   -fexpensive-optimizations -O2
CFLAGS = $(INCS)   -fexpensive-optimizations -O2

.PHONY: all all-before all-after clean clean-custom

all: all-before ../map08.dll all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

DLLWRAP=dllwrap.exe
DEFFILE=../libmap08.def
STATICLIB=../libmap08.a

$(BIN): $(LINKOBJ)
	$(DLLWRAP) --output-def $(DEFFILE) --implib $(STATICLIB) $(LINKOBJ) $(LIBS) -o $(BIN)

bm_game.o: bm_game.c
	$(CC) -c bm_game.c -o bm_game.o $(CFLAGS)

Boss.o: Boss.c
	$(CC) -c Boss.c -o Boss.o $(CFLAGS)

interface.o: interface.c
	$(CC) -c interface.c -o interface.o $(CFLAGS)
