# DLL makefile
CPP  = g++.exe
CC   = gcc.exe
DLLWRAP=dllwrap.exe

INCS = -I"../../common"

OBJ  = Bullet.o Explosion.o FlyBomber.o interface.o Jason.o Keyhole.o Mechipede.o Mine.o PowerUp.o RingFlyer.o RingSpawner.o RockClimber.o RockGunner.o RockWalker.o Squidy.o Tank.o Turret.o VDoor.o VFlyer.o World.o
BIN  = "../common.dll"
DEFFILE=../libcommon.def
STATICLIB=../libcommon.a

LIBS = -mwindows -lopengl32
CFLAGS = $(INCS) -g

.PHONY:	clean deps

$(BIN):	deps $(OBJ)
	$(DLLWRAP) --output-def $(DEFFILE) --implib $(STATICLIB) $(OBJ) $(LIBS) -o $(BIN)

clean:
	rm -f $(OBJ)

deps:
	$(CC) $(CFLAGS) -MM *.c > dependencies

-include dependencies