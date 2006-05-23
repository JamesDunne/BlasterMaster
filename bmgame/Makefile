# Project: BMGame
# Makefile created by Dev-C++ 4.9.9.0

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = interface.o $(RES)
LINKOBJ  = interface.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -lopengl32  
INCS =  -I"C:/Dev-Cpp/include"  -I"../common" 
CXXINCS =  -I"C:/Dev-Cpp/include/c++/3.3.1"  -I"C:/Dev-Cpp/include/c++/3.3.1/mingw32"  -I"C:/Dev-Cpp/include/c++/3.3.1/backward"  -I"C:/Dev-Cpp/lib/gcc-lib/mingw32/3.3.1/include"  -I"C:/Dev-Cpp/include"  -I"../common" 
BIN  = ../BMGame.dll
CXXFLAGS = $(CXXINCS)  
CFLAGS = $(INCS)  

.PHONY: all all-before all-after clean clean-custom

all: all-before ../BMGame.dll all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

DLLWRAP=dllwrap.exe
DEFFILE=../libBMGame.def
STATICLIB=../libBMGame.a

$(BIN): $(LINKOBJ)
	$(DLLWRAP) --output-def $(DEFFILE) --implib $(STATICLIB) $(LINKOBJ) $(LIBS) -o $(BIN)

interface.o: interface.c
	$(CC) -c interface.c -o interface.o $(CFLAGS)
