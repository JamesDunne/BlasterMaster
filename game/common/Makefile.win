# Project: common
# Makefile created by Dev-C++ 4.9.9.0

CPP  = g++.exe
CC   = gcc.exe
WINDRES = windres.exe
RES  = 
OBJ  = Bullet.o Explosion.o FlyBomber.o interface.o Jason.o Keyhole.o Mechipede.o Mine.o PowerUp.o RingFlyer.o RingSpawner.o RockClimber.o RockGunner.o RockWalker.o Squidy.o Tank.o Turret.o VDoor.o VFlyer.o World.o $(RES)
LINKOBJ  = Bullet.o Explosion.o FlyBomber.o interface.o Jason.o Keyhole.o Mechipede.o Mine.o PowerUp.o RingFlyer.o RingSpawner.o RockClimber.o RockGunner.o RockWalker.o Squidy.o Tank.o Turret.o VDoor.o VFlyer.o World.o $(RES)
LIBS =  -L"C:/Dev-Cpp/lib" -lopengl32  
INCS =  -I"C:/Dev-Cpp/include"  -I"../../common" 
CXXINCS =  -I"C:/Dev-Cpp/include/c++/3.3.1"  -I"C:/Dev-Cpp/include/c++/3.3.1/mingw32"  -I"C:/Dev-Cpp/include/c++/3.3.1/backward"  -I"C:/Dev-Cpp/lib/gcc-lib/mingw32/3.3.1/include"  -I"C:/Dev-Cpp/include"  -I"../../common" 
BIN  = ../common.dll
CXXFLAGS = $(CXXINCS)   -fexpensive-optimizations -O2
CFLAGS = $(INCS)   -fexpensive-optimizations -O2

.PHONY: all all-before all-after clean clean-custom

all: all-before ../common.dll all-after


clean: clean-custom
	rm -f $(OBJ) $(BIN)

DLLWRAP=dllwrap.exe
DEFFILE=../libcommon.def
STATICLIB=../libcommon.a

$(BIN): $(LINKOBJ)
	$(DLLWRAP) --output-def $(DEFFILE) --implib $(STATICLIB) $(LINKOBJ) $(LIBS) -o $(BIN)

Bullet.o: Bullet.c
	$(CC) -c Bullet.c -o Bullet.o $(CFLAGS)

Explosion.o: Explosion.c
	$(CC) -c Explosion.c -o Explosion.o $(CFLAGS)

FlyBomber.o: FlyBomber.c
	$(CC) -c FlyBomber.c -o FlyBomber.o $(CFLAGS)

interface.o: interface.c
	$(CC) -c interface.c -o interface.o $(CFLAGS)

Jason.o: Jason.c
	$(CC) -c Jason.c -o Jason.o $(CFLAGS)

Keyhole.o: Keyhole.c
	$(CC) -c Keyhole.c -o Keyhole.o $(CFLAGS)

Mechipede.o: Mechipede.c
	$(CC) -c Mechipede.c -o Mechipede.o $(CFLAGS)

Mine.o: Mine.c
	$(CC) -c Mine.c -o Mine.o $(CFLAGS)

PowerUp.o: PowerUp.c
	$(CC) -c PowerUp.c -o PowerUp.o $(CFLAGS)

RingFlyer.o: RingFlyer.c
	$(CC) -c RingFlyer.c -o RingFlyer.o $(CFLAGS)

RingSpawner.o: RingSpawner.c
	$(CC) -c RingSpawner.c -o RingSpawner.o $(CFLAGS)

RockClimber.o: RockClimber.c
	$(CC) -c RockClimber.c -o RockClimber.o $(CFLAGS)

RockGunner.o: RockGunner.c
	$(CC) -c RockGunner.c -o RockGunner.o $(CFLAGS)

RockWalker.o: RockWalker.c
	$(CC) -c RockWalker.c -o RockWalker.o $(CFLAGS)

Squidy.o: Squidy.c
	$(CC) -c Squidy.c -o Squidy.o $(CFLAGS)

Tank.o: Tank.c
	$(CC) -c Tank.c -o Tank.o $(CFLAGS)

Turret.o: Turret.c
	$(CC) -c Turret.c -o Turret.o $(CFLAGS)

VDoor.o: VDoor.c
	$(CC) -c VDoor.c -o VDoor.o $(CFLAGS)

VFlyer.o: VFlyer.c
	$(CC) -c VFlyer.c -o VFlyer.o $(CFLAGS)

World.o: World.c
	$(CC) -c World.c -o World.o $(CFLAGS)
