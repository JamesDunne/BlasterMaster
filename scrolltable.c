#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>  // Header File For The OpenGL32 Library
#include <OpenGL/glu.h> // Header File For The GLu32 Library
#else
#include <GL/gl.h>	  // Header File For The OpenGL32 Library
#include <GL/glu.h>	 // Header File For The GLu32 Library
#endif

#ifndef WIN32
#include <unistd.h>
#include <signal.h>
#endif

#include <stdlib.h>
#include <SDL/SDL.h>
#include <png.h>
#include "common.h"
#include "newmap.h"

unsigned long	scrolloffsets[16];
unsigned long	leveloffsets[16 * 4];
unsigned long	levelsizes[16 * 4];
unsigned char	scrollv[8], scrollh[8];
unsigned long	prgsize;
int		level;

FILE	*nesrom;

unsigned short ReadUInt16()
{
	unsigned short a;
	fread(&a, 1, 2, nesrom);
	return a;
}

void ReadLevel(int l, int n)
{
	// Read scroll table for the level:
	fseek(nesrom, scrolloffsets[l], SEEK_SET);
	fread(scrollv, 1, 8, nesrom);
	fread(scrollh, 1, 8, nesrom);

	map.width = 128;
	map.height = (levelsizes[(l<<2)+3] >> 5) << 2;
}

int LoadROM(char *fname)
{
	FILE	*palfile;
	int i;
	unsigned char	r, g, b, dummy;
	unsigned long	sig;

	nesrom = fopen(fname, "rb");
	if (nesrom == NULL) {
		fprintf(stderr, "'%s' was not found!\n", fname);
		return -1;
	}

	// Read how much PRG ROM there is:
	fread(&sig, 1, 4, nesrom);
	if (sig != 0x1A53454E) {
		fclose(nesrom);
		fprintf(stderr, "This doesn't look like a valid NES ROM file.  Make sure it has a legal iNES header!\n");
		return -2;
	}
	fread(&dummy, 1, 1, nesrom);

	prgsize = ((unsigned long)dummy * 16384) + 16;
	if (prgsize == 0) {
		fclose(nesrom);
		fprintf(stderr, "The header reports ZERO PRG ROM!!  Cannot determine where sprites are.\n");
		return -2;
	}

	// First 6 levels are on the first page of 0x4000 bytes.
	// The last 2 levels are on the second page of 0x4000 bytes:

	// Read level section offsets:
	fseek(nesrom, 0x10, SEEK_SET);
	// Subtract 0x8000 (for PRG memory) from the offset and then add 0x10 (for NES header)
	leveloffsets[0<<2] = (ReadUInt16() - 0x8000) + 0x0010;
	scrolloffsets[0] = (ReadUInt16() - 0x8000) + 0x0010;
	leveloffsets[1<<2] = (ReadUInt16() - 0x8000) + 0x0010;
	scrolloffsets[1] = (ReadUInt16() - 0x8000) + 0x0010;
	leveloffsets[2<<2] = (ReadUInt16() - 0x8000) + 0x0010;
	scrolloffsets[2] = (ReadUInt16() - 0x8000) + 0x0010;
	leveloffsets[3<<2] = (ReadUInt16() - 0x8000) + 0x0010;
	scrolloffsets[3] = (ReadUInt16() - 0x8000) + 0x0010;
	leveloffsets[4<<2] = (ReadUInt16() - 0x8000) + 0x0010;
	scrolloffsets[4] = (ReadUInt16() - 0x8000) + 0x0010;

	// Read level section offsets:
	fseek(nesrom, 0x4010, SEEK_SET);
	// Subtract 0x8000 (for PRG memory) from the offset and then add 0x4010 (for NES header)
	leveloffsets[5<<2] = (ReadUInt16() - 0x8000) + 0x4010;
	scrolloffsets[5] = (ReadUInt16() - 0x8000) + 0x4010;
	leveloffsets[6<<2] = (ReadUInt16() - 0x8000) + 0x4010;
	scrolloffsets[6] = (ReadUInt16() - 0x8000) + 0x4010;
	leveloffsets[7<<2] = (ReadUInt16() - 0x8000) + 0x4010;
	scrolloffsets[7] = (ReadUInt16() - 0x8000) + 0x4010;
	leveloffsets[8<<2] = (ReadUInt16() - 0x8000) + 0x4010;
	scrolloffsets[8] = (ReadUInt16() - 0x8000) + 0x4010;
	leveloffsets[9<<2] = (ReadUInt16() - 0x8000) + 0x4010;
	scrolloffsets[9] = (ReadUInt16() - 0x8000) + 0x4010;

	// Read level section offsets:
	fseek(nesrom, 0x8010, SEEK_SET);
	// Subtract 0x8000 (for PRG memory) from the offset and then add 0x4010 (for NES header)
	leveloffsets[10<<2] = (ReadUInt16() - 0x8000) + 0x8010;
	scrolloffsets[10] = (ReadUInt16() - 0x8000) + 0x8010;
	leveloffsets[11<<2] = (ReadUInt16() - 0x8000) + 0x8010;
	scrolloffsets[11] = (ReadUInt16() - 0x8000) + 0x8010;
	leveloffsets[12<<2] = (ReadUInt16() - 0x8000) + 0x8010;
	scrolloffsets[12] = (ReadUInt16() - 0x8000) + 0x8010;
	leveloffsets[13<<2] = (ReadUInt16() - 0x8000) + 0x8010;
	scrolloffsets[13] = (ReadUInt16() - 0x8000) + 0x8010;
	leveloffsets[14<<2] = (ReadUInt16() - 0x8000) + 0x8010;
	scrolloffsets[14] = (ReadUInt16() - 0x8000) + 0x8010;
	leveloffsets[15<<2] = (ReadUInt16() - 0x8000) + 0x8010;
	scrolloffsets[15] = (ReadUInt16() - 0x8000) + 0x8010;

	return 0;
}

////////////////////////////////
int	used[8][8];

int is_used_up(int x, int y) {
	return used[y == 0 ? 7 : y - 1][x];
}

int is_used_down(int x, int y) {
	return used[y == 7 ? 0 : y + 1][x];
}

int is_used_left(int x, int y) {
	return used[y][x == 0 ? 7 : x - 1];
}

int is_used_right(int x, int y) {
	return used[y][x == 7 ? 0 : x + 1];
}

////////////////////////////////

int is_blocked_up(int x, int y) {
	return (scrollv[y] & (1 << (7-x)));
}

int is_blocked_down(int x, int y) {
	return (scrollv[(y == 7) ? 0 : (y + 1)] & (1 << (7-x)));
}

int is_blocked_left(int x, int y) {
	return (scrollh[y] & ( 1 << (7-x) ));
}

int is_blocked_right(int x, int y) {
	return (scrollh[y] & ( 1 << ((x == 7) ? 7 : (7-x)-1) ));
}

void fill_used(int lx, int ty, int rx, int by) {
	int	tmpx, tmpy;
	if (rx < lx) {
		if (by < ty) {
			for (tmpy = ty; tmpy <= 7; ++tmpy) {
				for (tmpx = lx; tmpx <= 7; ++tmpx) {
					used[tmpy][tmpx] = -1;
				}
				for (tmpx = 0; tmpx <= rx; ++tmpx) {
					used[tmpy][tmpx] = -1;
				}
			}
			for (tmpy = 0; tmpy <= by; ++tmpy) {
				for (tmpx = lx; tmpx <= 7; ++tmpx) {
					used[tmpy][tmpx] = -1;
				}
				for (tmpx = 0; tmpx <= rx; ++tmpx) {
					used[tmpy][tmpx] = -1;
				}
			}
		} else {
			for (tmpy = ty; tmpy <= by; ++tmpy) {
				for (tmpx = lx; tmpx <= 7; ++tmpx) {
					used[tmpy][tmpx] = -1;
				}
				for (tmpx = 0; tmpx <= rx; ++tmpx) {
					used[tmpy][tmpx] = -1;
				}
			}
		}
	} else {
		if (by < ty) {
			for (tmpy = ty; tmpy <= 7; ++tmpy) {
				for (tmpx = lx; tmpx <= rx; ++tmpx) {
					used[tmpy][tmpx] = -1;
				}
			}
			for (tmpy = 0; tmpy <= by; ++tmpy) {
				for (tmpx = lx; tmpx <= rx; ++tmpx) {
					used[tmpy][tmpx] = -1;
				}
			}
		} else {
			for (tmpy = ty; tmpy <= by; ++tmpy) {
				for (tmpx = lx; tmpx <= rx; ++tmpx) {
					used[tmpy][tmpx] = -1;
				}
			}
		}
	}
}

// Converts map from NES ROM into our native map format:
void ConvertMap(int l, int n, int numtex) {
	int		i, rooms;
	int		x, y, sx, sy;
	int		lx, rx, ty, by;
	int		tmpx, tmpy;

	// Create the scroll region map:
	printf("\n");
	for (y = 0; y < 8; ++y) {
		for (x = 7; x >= 0; --x) {
			printf("+");
			if (scrollv[y] & (1 << x)) {
				printf("---");
			} else {
				printf("   ");
			}
		}
		printf("+\n");
		for (i = 0; i < 3; ++i) {
			for (x = 7; x >= 0; --x) {
				if (scrollh[y] & (1 << x))
					printf("|");
				else
					printf(" ");
				printf("   ");
			}
			if (scrollh[y] & (1 << 7))
				printf("|");
			else
				printf(" ");
			printf("\n");
		}
	}
	for (x = 7; x >= 0; --x) {
		printf("+");
		if (scrollv[0] & (1 << x)) {
			printf("---");
		} else {
			printf("   ");
		}
	}
	printf("+\n");

	// Find extents of each scroll region:

	// Scroll regions are *always* rectangular, so we can take advantage of that.

	for (y = 0; y < 8; ++y) {
		for (x = 0; x < 8; ++x) {
			used[y][x] = 0;
		}
	}

	// Exceptions for oddly-shaped levels with unused space:
	if (n == 8) {
		for (tmpy = 0; tmpy <= 7; ++tmpy)
			for (tmpx = 6; tmpx <= 7; ++tmpx)
				used[tmpy][tmpx] = -1;
		for (tmpy = 6; tmpy <= 7; ++tmpy)
			for (tmpx = 0; tmpx <= 5; ++tmpx)
				used[tmpy][tmpx] = -1;
	} else if (n == 10) {
		for (tmpy = 0; tmpy <= 7; ++tmpy)
			used[tmpy][7] = -1;
		for (tmpy = 6; tmpy <= 7; ++tmpy)
			for (tmpx = 4; tmpx <= 6; ++tmpx)
				used[tmpy][tmpx] = -1;
	}

	for (;;) {
		// Find a starting room:
		for (sy = 0; sy < 8; ++sy) {
			for (sx = 0; sx < 8; ++sx) {
				if (used[sy][sx] == 0) {
					break;
				}
			}
			if (sx < 8) {
				if (used[sy][sx] == 0) break;
			}
		}
		if (sy == 8 || sx == 8) break;

		// Make the rectangle:
		used[sy][sx] = -1;
		x = sx; y = sy;
		lx = sx; ty = sy;
		rx = sx; by = sy;
		for (;;) {
			if ((is_used_right(x,y) == 0) && (is_blocked_right(x,y) == 0)) {
				//printf("right at %d,%d\n", x, y);
				x++;
				if (x > 7) x = 0;
				if (x > rx || x == 0) rx = x;
				fill_used(lx, ty, rx, by);
			} else if ((is_used_down(x,y) == 0) && (is_blocked_down(x,y) == 0)) {
				//printf("down at %d,%d\n", x, y);
				y++;
				if (y > 7) y = 0;
				if (y > by || y == 0) by = y;
				fill_used(lx, ty, rx, by);
			} else if ((is_used_left(x,y) == 0) && (is_blocked_left(x,y) == 0)) {
				//printf("left at %d,%d\n", x, y);
				x--;
				if (x < 0) x = 7;
				if (x < lx || x == 7) lx = x;
				fill_used(lx, ty, rx, by);
			} else if ((is_used_up(x,y) == 0) && (is_blocked_up(x,y) == 0)) {
				//printf("up at %d,%d\n", x, y);
				y--;
				if (y < 0) y = 7;
				if (y < ty || y == 7) ty = y;
				fill_used(lx, ty, rx, by);
			} else {
				if (x == sx && y == sy) {
					//printf("stop at %d,%d\n", x, y);
					printf("{%d, %d} to {%d, %d}\n", lx, ty, rx, by);
					break;
				}
				//printf("reset at %d,%d back to %d,%d\n", x, y, sx, sy);
				x = sx;
				y = sy;
			}
		}
	}

}


// Saves all variants of sprites and background textures to files:
void SaveTextures() {
	int		i, j, k, l, n, cm;
	unsigned short	m, totalsprites;

	for (n=0; n<16; ++n) {
		level = n;
		printf("Converting level %d...", n+1);

		// Funky mapping for Jason's dungeons:
		if (n < 8) l = n;
		switch (n) {
			case  8: l =  8; break;
			case  9: l = 11; break;
			case 10: l =  9; break;
			case 11: l = 14; break;
			case 12: l = 10; break;
			case 13: l = 12; break;
			case 14: l = 15; break;
			case 15: l = 13; break;
		}

		ReadLevel(l, n);			// Loads the original map data

		// Do the map conversion:
		ConvertMap(l, n, 4);

	}

}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return main(__argc, __argv);
}

int main(int argc, char **argv) {
	int i;
	unsigned long sdlflags;

	if (argc != 2) {
		fprintf(stderr, "Please specify the name of the NES ROM file whose graphics you wish to convert!\n");
		return -1;
	}

	if (LoadROM(argv[1]) != 0)
		return -2;

	InitMap();
	SaveTextures();

	return 0;
};

