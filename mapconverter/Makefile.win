# Project: Map Extractor
# Makefile created by Dev-C++ 4.9.9.0

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = tilepager.o ../common/newmap.o $(RES)
LINKOBJ  = tilepager.o ../common/newmap.o $(RES)
LIBS = -lmingw32 -lSDLmain -lSDL -lpng -lz  
INCS = -I"../common" 
CXXINCS = -I"../common" 
BIN  = "../Map Extractor.exe"
CXXFLAGS = $(CXXINCS) -fexpensive-optimizations
CFLAGS = $(INCS) -Dmain=SDL_main -fexpensive-optimizations -g

.PHONY: all all-before all-after clean clean-custom

all: all-before "../Map Extractor.exe" all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o "..\Map Extractor.exe" $(LIBS)

tilepager.o: tilepager.c
	$(CC) -c tilepager.c -o tilepager.o $(CFLAGS)

../common/newmap.o: ../common/newmap.c
	$(CC) -c ../common/newmap.c -o ../common/newmap.o $(CFLAGS)
