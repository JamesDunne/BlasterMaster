# Project: map0C
# Makefile created by Dev-C++ 4.9.9.0

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = bm_game.o interface.o $(RES)
LINKOBJ  = bm_game.o interface.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -lopengl32  
INCS =  -I"C:/Dev-Cpp/include"  -I"../../common"  -I"../common_dungeon" 
CXXINCS =  -I"C:/Dev-Cpp/include/c++/3.3.1"  -I"C:/Dev-Cpp/include/c++/3.3.1/mingw32"  -I"C:/Dev-Cpp/include/c++/3.3.1/backward"  -I"C:/Dev-Cpp/lib/gcc-lib/mingw32/3.3.1/include"  -I"C:/Dev-Cpp/include"  -I"../../common"  -I"../common_dungeon" 
BIN  = ../map0C.dll
CXXFLAGS = $(CXXINCS)   -fexpensive-optimizations -O2
CFLAGS = $(INCS)   -fexpensive-optimizations -O2

.PHONY: all all-before all-after clean clean-custom

all: all-before ../map0C.dll all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

DLLWRAP=dllwrap.exe
DEFFILE=../libmap0C.def
STATICLIB=../libmap0C.a

$(BIN): $(LINKOBJ)
	$(DLLWRAP) --output-def $(DEFFILE) --implib $(STATICLIB) $(LINKOBJ) $(LIBS) -o $(BIN)

bm_game.o: bm_game.c
	$(CC) -c bm_game.c -o bm_game.o $(CFLAGS)

interface.o: interface.c
	$(CC) -c interface.c -o interface.o $(CFLAGS)
