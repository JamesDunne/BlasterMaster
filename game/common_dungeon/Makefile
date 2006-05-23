# Project: common_dungeon
# Makefile created by Dev-C++ 4.9.9.0

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = Bullet.o Explosion.o interface.o Jason.o PowerUp.o RingFlyer.o RingSpawner.o World.o $(RES)
LINKOBJ  = Bullet.o Explosion.o interface.o Jason.o PowerUp.o RingFlyer.o RingSpawner.o World.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -lopengl32  
INCS =  -I"C:/Dev-Cpp/include"  -I"../../common" 
CXXINCS =  -I"C:/Dev-Cpp/include/c++/3.3.1"  -I"C:/Dev-Cpp/include/c++/3.3.1/mingw32"  -I"C:/Dev-Cpp/include/c++/3.3.1/backward"  -I"C:/Dev-Cpp/lib/gcc-lib/mingw32/3.3.1/include"  -I"C:/Dev-Cpp/include"  -I"../../common" 
BIN  = ../common_dungeon.dll
CXXFLAGS = $(CXXINCS)   -fexpensive-optimizations -O2
CFLAGS = $(INCS)   -fexpensive-optimizations -O2

.PHONY: all all-before all-after clean clean-custom

all: all-before ../common_dungeon.dll all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

DLLWRAP=dllwrap.exe
DEFFILE=../libcommon_dungeon.def
STATICLIB=../libcommon_dungeon.a

$(BIN): $(LINKOBJ)
	$(DLLWRAP) --output-def $(DEFFILE) --implib $(STATICLIB) $(LINKOBJ) $(LIBS) -o $(BIN)

Bullet.o: Bullet.c
	$(CC) -c Bullet.c -o Bullet.o $(CFLAGS)

Explosion.o: Explosion.c
	$(CC) -c Explosion.c -o Explosion.o $(CFLAGS)

interface.o: interface.c
	$(CC) -c interface.c -o interface.o $(CFLAGS)

Jason.o: Jason.c
	$(CC) -c Jason.c -o Jason.o $(CFLAGS)

PowerUp.o: PowerUp.c
	$(CC) -c PowerUp.c -o PowerUp.o $(CFLAGS)

RingFlyer.o: RingFlyer.c
	$(CC) -c RingFlyer.c -o RingFlyer.o $(CFLAGS)

RingSpawner.o: RingSpawner.c
	$(CC) -c RingSpawner.c -o RingSpawner.o $(CFLAGS)

World.o: World.c
	$(CC) -c World.c -o World.o $(CFLAGS)
