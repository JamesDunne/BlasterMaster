# DLL makefile
CPP  = g++.exe
CC   = gcc.exe
DLLWRAP=dllwrap.exe

INCS = -I"../../common"

OBJ  = Bullet.o Explosion.o interface.o Jason.o PowerUp.o RingFlyer.o RingSpawner.o World.o
BIN  = "../common_dungeon.dll"
DEFFILE=../libcommon_dungeon.def
STATICLIB=../libcommon_dungeon.a

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