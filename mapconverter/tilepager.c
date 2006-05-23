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
unsigned long	etyoffsets[16];
unsigned long	numetys;
unsigned char	*etyclass, *etyx, *etyy;
unsigned long	paletteofs[16], colormapofs[16];
unsigned char	mappalette[16];
unsigned char	scrollv[8], scrollh[8];
unsigned char	*map2x2;
unsigned char	*map4x4;
unsigned char	*map8x8;
unsigned char	*map32x32;
unsigned char	*mapflags;
unsigned long	prgsize;
int		level;

typedef struct {
	unsigned char	levela, xa, ya;
	unsigned char	levelb, xb, yb;
} gateway_t;

int			gatewaycount;
gateway_t	*gateways;

// Mapping table for old sprite #s to new sprite #s:
long	spritemapping[4 * 256];
long	allsprites[4 * 256];

// Memory for the sprite data:
Uint8	*sprite;
Uint8	*tiles;
Uint32	*palette;

FILE	*nesrom;

#define fromargb(a, r, g, b) ((Uint32)r | ((Uint32)(g) << 8) | ((Uint32)(b) << 16) | ((Uint32)(a) << 24))

unsigned short ReadUInt16()
{
	unsigned short a;
	fread(&a, 1, 2, nesrom);
	return a;
}

void ReadLevel(int l, int n)
{
	int	page;
	unsigned long	offs_etyclass;
	unsigned long	offs_etyx;
	unsigned long	offs_etyy;

	if (map2x2) free(map2x2);
	fseek(nesrom, leveloffsets[(l<<2)+0], SEEK_SET);
	map2x2 = calloc(levelsizes[(l<<2)+0], 1);
	fread(map2x2, 1, levelsizes[(l<<2)+0], nesrom);

	if (map4x4) free(map4x4);
	fseek(nesrom, leveloffsets[(l<<2)+1], SEEK_SET);
	map4x4 = calloc(levelsizes[(l<<2)+1], 1);
	fread(map4x4, 1, levelsizes[(l<<2)+1], nesrom);

	if (map8x8) free(map8x8);
	fseek(nesrom, leveloffsets[(l<<2)+2], SEEK_SET);
	map8x8 = calloc(levelsizes[(l<<2)+2], 1);
	fread(map8x8, 1, levelsizes[(l<<2)+2], nesrom);

	if (map32x32) free(map32x32);
	fseek(nesrom, leveloffsets[(l<<2)+3], SEEK_SET);
	map32x32 = calloc(levelsizes[(l<<2)+3], 1);
	fread(map32x32, 1, levelsizes[(l<<2)+3], nesrom);

	// Read scroll table for the level:
	fseek(nesrom, scrolloffsets[l], SEEK_SET);
	fread(scrollv, 1, 8, nesrom);
	fread(scrollh, 1, 8, nesrom);

	map.width = 128;
	map.height = (levelsizes[(l<<2)+3] >> 5) << 2;

	if (mapflags) free(mapflags);
	fseek(nesrom, colormapofs[l], SEEK_SET);
	mapflags = calloc(levelsizes[(l<<2)+0] >> 2, 1);
	fread(mapflags, 1, levelsizes[(l<<2)+0] >> 2, nesrom);

	// Read entities:
	fseek(nesrom, etyoffsets[n], SEEK_SET);
	offs_etyclass = (ReadUInt16() - 0x8000) + 0xC010;
	offs_etyx = (ReadUInt16() - 0x8000) + 0xC010;
	offs_etyy = (ReadUInt16() - 0x8000) + 0xC010;

	numetys = offs_etyx - offs_etyclass;

	if (etyclass) free(etyclass);
	etyclass = calloc(numetys, 1);
	fseek(nesrom, offs_etyclass, SEEK_SET);
	fread(etyclass, 1, numetys, nesrom);

	if (etyx) free(etyx);
	etyx = calloc(numetys, 1);
	fseek(nesrom, offs_etyx, SEEK_SET);
	fread(etyx, 1, numetys, nesrom);

	if (etyy) free(etyy);
	etyy = calloc(numetys, 1);
	fseek(nesrom, offs_etyy, SEEK_SET);
	fread(etyy, 1, numetys, nesrom);

	// Sprite stuff:
	fseek(nesrom, paletteofs[l], SEEK_SET);
	fread(&mappalette, 1, 0x10, nesrom);

	// Read bg sprite page:
	if (tiles) free(tiles);
	tiles = calloc(0x1000, 1);
	// Select while BG tile page to use:
	switch (l) {
		case  0: page =  8; break;
		case  1: page =  9; break;
		case  2: page = 10; break;
		case  3: page = 11; break;
		case  4: page = 12; break;
		case  5: page = 13; break;
		case  6: page = 14; break;
		case  7: page = 15; break;
		case  8: page = 17; break;
		case  9: page = 18; break;
		case 10: page = 18; break;
		case 11: page = 19; break;
		case 12: page = 19; break;
		case 13: page = 17; break;
		case 14: page = 20; break;
		case 15: page = 20; break;
	}
	fseek(nesrom, prgsize + (page << 12), SEEK_SET);
	fread(tiles, 1, 0x1000, nesrom);
}

int LoadROM(char *fname) {
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

	// Read palette:
	palfile = fopen("nes.pal", "rb");
	if (palfile == NULL) {
		fprintf(stderr, "nes.pal was not found!\n");
		return -1;
	}

	palette = calloc(64 * sizeof(GLuint), 1);
	for (i=0; i<64; ++i) {
		fread(&r, 1, 1, palfile);
		fread(&g, 1, 1, palfile);
		fread(&b, 1, 1, palfile);
		palette[i] = fromargb(255, r, g, b);
	}

	fclose(palfile);

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

	for (i=0; i<5; ++i) {
		fseek(nesrom, leveloffsets[i<<2], SEEK_SET);

		paletteofs[i] = (ReadUInt16() - 0x8000) + 0x0010;
		colormapofs[i] = (ReadUInt16() - 0x8000) + 0x0010;
		leveloffsets[(i<<2)+0] = (ReadUInt16() - 0x8000) + 0x0010;
		leveloffsets[(i<<2)+1] = (ReadUInt16() - 0x8000) + 0x0010;
		leveloffsets[(i<<2)+2] = (ReadUInt16() - 0x8000) + 0x0010;
		leveloffsets[(i<<2)+3] = (ReadUInt16() - 0x8000) + 0x0010;
		levelsizes[(i<<2)+0] = (leveloffsets[(i<<2)+1] - leveloffsets[(i<<2)+0]);
		levelsizes[(i<<2)+1] = (leveloffsets[(i<<2)+2] - leveloffsets[(i<<2)+1]);
		levelsizes[(i<<2)+2] = (leveloffsets[(i<<2)+3] - leveloffsets[(i<<2)+2]);
		levelsizes[(i<<2)+3] = (colormapofs[i] - leveloffsets[(i<<2)+3]);
	}

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

	for (i=5; i<10; ++i)
	{
		fseek(nesrom, leveloffsets[i<<2], SEEK_SET);
			
		paletteofs[i] = (ReadUInt16() - 0x8000) + 0x4010;
		colormapofs[i] = (ReadUInt16() - 0x8000) + 0x4010;
		leveloffsets[(i<<2)+0] = (ReadUInt16() - 0x8000) + 0x4010;
		leveloffsets[(i<<2)+1] = (ReadUInt16() - 0x8000) + 0x4010;
		leveloffsets[(i<<2)+2] = (ReadUInt16() - 0x8000) + 0x4010;
		leveloffsets[(i<<2)+3] = (ReadUInt16() - 0x8000) + 0x4010;
		levelsizes[(i<<2)+0] = (int)(leveloffsets[(i<<2)+1] - leveloffsets[(i<<2)+0]);
		levelsizes[(i<<2)+1] = (int)(leveloffsets[(i<<2)+2] - leveloffsets[(i<<2)+1]);
		levelsizes[(i<<2)+2] = (int)(leveloffsets[(i<<2)+3] - leveloffsets[(i<<2)+2]);
		levelsizes[(i<<2)+3] = (int)(colormapofs[i] - leveloffsets[(i<<2)+3]);
	}

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

	for (i=10; i<16; ++i)
	{
		fseek(nesrom, leveloffsets[i<<2], SEEK_SET);

		paletteofs[i] = (ReadUInt16() - 0x8000) + 0x8010;
		colormapofs[i] = (ReadUInt16() - 0x8000) + 0x8010;
		leveloffsets[(i<<2)+0] = (ReadUInt16() - 0x8000) + 0x8010;
		leveloffsets[(i<<2)+1] = (ReadUInt16() - 0x8000) + 0x8010;
		leveloffsets[(i<<2)+2] = (ReadUInt16() - 0x8000) + 0x8010;
		leveloffsets[(i<<2)+3] = (ReadUInt16() - 0x8000) + 0x8010;
		levelsizes[(i<<2)+0] = (int)(leveloffsets[(i<<2)+1] - leveloffsets[(i<<2)+0]);
		levelsizes[(i<<2)+1] = (int)(leveloffsets[(i<<2)+2] - leveloffsets[(i<<2)+1]);
		levelsizes[(i<<2)+2] = (int)(leveloffsets[(i<<2)+3] - leveloffsets[(i<<2)+2]);
		levelsizes[(i<<2)+3] = (int)(colormapofs[i] - leveloffsets[(i<<2)+3]);
	}

	// Read entity offsets:
	fseek(nesrom, 0xC012, SEEK_SET);
	fseek(nesrom, (ReadUInt16() - 0x8000) + 0xC010, SEEK_SET);
	for (i=8; i<16; ++i) {
		etyoffsets[i] = (ReadUInt16() - 0x8000) + 0xC010;
	}
	for (i=0; i<8; ++i) {
		etyoffsets[i] = (ReadUInt16() - 0x8000) + 0xC010;
	}

	// Read gateway-pairs data:
	fseek(nesrom, 0x1DD8E, SEEK_SET);
	gatewaycount = 108 / 2;
	gateways = malloc(gatewaycount * sizeof(gateway_t));
	for (i = 0; i < gatewaycount; ++i) {
		fread(&gateways[i].levela, 1, 1, nesrom);
		fread(&gateways[i].levelb, 1, 1, nesrom);
	}
	for (i = 0; i < gatewaycount; ++i) {
		fread(&gateways[i].xa, 1, 1, nesrom);
		fread(&gateways[i].xb, 1, 1, nesrom);
	}
	for (i = 0; i < gatewaycount; ++i) {
		fread(&gateways[i].ya, 1, 1, nesrom);
		fread(&gateways[i].yb, 1, 1, nesrom);
	}

	return 0;
}

// Map stuff:

void Uncompress2x2(Uint8 *tmap, int tx, int ty, Uint8 t) {
	tmap[(ty << 7) | tx] = t;
}

void Uncompress4x4(Uint8 *tmap, int tx, int ty, Uint8 t) {
	Uncompress2x2(tmap, tx  , ty  , map4x4[(t << 2)+0]);
	Uncompress2x2(tmap, tx+1, ty  , map4x4[(t << 2)+1]);
	Uncompress2x2(tmap, tx  , ty+1, map4x4[(t << 2)+2]);
	Uncompress2x2(tmap, tx+1, ty+1, map4x4[(t << 2)+3]);
};

void Uncompress8x8(Uint8 *tmap, int tx, int ty, Uint8 t) {
	Uncompress4x4(tmap, tx  , ty  , map8x8[(t << 2)+0]);
	Uncompress4x4(tmap, tx+2, ty  , map8x8[(t << 2)+1]);
	Uncompress4x4(tmap, tx  , ty+2, map8x8[(t << 2)+2]);
	Uncompress4x4(tmap, tx+2, ty+2, map8x8[(t << 2)+3]);
};


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

int convert_level_number(int n) {
	if (n < 8) return n;
	switch (n) {
		case  8: return  8;
		case  9: return 11;
		case 10: return  9;
		case 11: return 14;
		case 12: return 10;
		case 13: return 12;
		case 14: return 15;
		case 15: return 13;
		default: return -1;
	}
}

// Converts map from NES ROM into our native map format:
void ConvertMap(int l, int n, int numtex) {
	char	tempf[256];
	int		i, m, x, y, total_tag, extra;

	int		sx, sy;
	int		lx, rx, ty, by;
	int		tmpx, tmpy;

	int		revlev;

	// Assign texture filenames:
	map.numtextures = numtex;
	map.texturefile = (char **) calloc(sizeof(char *) * numtex, 1);
	map.texturefile_loaded = 1;

	for (i=0; i<map.numtextures; ++i) {
		sprintf(tempf, "textures/bgtile%X%X.png", l, i);
		map.texturefile[i] = (char *) calloc(strlen(tempf) + 1, 1);
		strcpy(map.texturefile[i], tempf);
	}

	// Directly copy the map2x2 data:
	map.map2x2_size = levelsizes[(l<<2)+0] >> 2;
	map.map2x2 = (Uint8 *) calloc(levelsizes[(l<<2)+0], 1);
	map.map2x2_loaded = 1;
	memcpy(map.map2x2, map2x2, levelsizes[(l<<2)+0]);
	for (i=0; i<map.map2x2_size; ++i) {
		//map.map2x2[i] = ((map.map2x2[i] & 0x0F) << 4) | ((map.map2x2[i] >> 4) & 0x0F);
	}

	// Directly copy the mapflags data:
	map.mapflags = (Uint8 *) calloc(map.map2x2_size * sizeof(Uint8), 1);
	memcpy(map.mapflags, mapflags, map.map2x2_size * sizeof(Uint8));
	map.mapflags_loaded = 1;

	map.map = calloc(sizeof(Uint8) * map.width * map.height, 1);
	map.map_loaded = 1;

	// Draw the map out from the "compressed" data:
	for (y=0;y<map.height >> 2;++y) {
		for (x=0;x<map.width >> 2;++x) {
			Uncompress8x8(map.map, x << 2, y << 2, map32x32[(y << 5) | x]);
		}
	}

	revlev = (n + 8) % 16;
	// First get a count of gateways for this level:
	map.num_doors = 0;
	for (i = 0; i < gatewaycount; ++i) {
		if (gateways[i].levela == revlev) {
			++map.num_doors;
		}
		if (gateways[i].levelb == revlev) {
			++map.num_doors;
		}
	}
	map.doors_loaded = 1;
	map.doors = realloc(map.doors, sizeof(mapdoor_t *) * map.num_doors);

	m = 0;
	for (i = 0; i < gatewaycount; ++i) {
		if (gateways[i].levela == revlev) {
			int	lb = ((gateways[i].levelb + 8) % 16);
			map.doors[m] = calloc(sizeof(mapdoor_t), 1);
			map.doors[m]->x = gateways[i].xa;
			map.doors[m]->y = gateways[i].ya;
			map.doors[m]->tag = i+1;
			map.doors[m]->targetmap = calloc(strlen("maps/map0X.bma") + 1, 1);
			strcpy(map.doors[m]->targetmap, "maps/map0X.bma");
			if (lb < 10)
				map.doors[m]->targetmap[9] = lb + '0';
			else
				map.doors[m]->targetmap[9] = lb - 10 + 'A';
			++m;
		}
		if (gateways[i].levelb == revlev) {
			int	la = ((gateways[i].levela + 8) % 16);
			map.doors[m] = calloc(sizeof(mapdoor_t), 1);
			map.doors[m]->x = gateways[i].xb;
			map.doors[m]->y = gateways[i].yb;
			map.doors[m]->tag = i+1;
			map.doors[m]->targetmap = calloc(strlen("maps/map0X.bma") + 1, 1);
			strcpy(map.doors[m]->targetmap, "maps/map0X.bma");
			if (la < 10)
				map.doors[m]->targetmap[9] = la + '0';
			else
				map.doors[m]->targetmap[9] = la - 10 + 'A';
			++m;
		}
	}

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

	map.num_regions = 0;
	map.regions = NULL;
	map.regions_loaded = 1;
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
					printf("{%d, %d} to {%d, %d}\n", lx*16, ty*16, rx*16+15, by*16+15);
					map.num_regions++;
					map.regions = realloc(map.regions, sizeof(mapregion_t *) * map.num_regions);
					map.regions[map.num_regions - 1] = calloc(sizeof(mapregion_t), 1);
					map.regions[map.num_regions - 1]->lx = lx*16;
					map.regions[map.num_regions - 1]->ty = ty*16;
					map.regions[map.num_regions - 1]->rx = rx*16+15;
					map.regions[map.num_regions - 1]->by = by*16+15;
					break;
				}
				//printf("reset at %d,%d back to %d,%d\n", x, y, sx, sy);
				x = sx;
				y = sy;
			}
		}
	}

	// Store all entities:
	if (l == 0) extra = 1; else extra = 0;
	map.num_entities = numetys + extra;

	map.entities_loaded = 1;
	map.entities = calloc(sizeof(mapentity_t *) * (numetys + extra), 1);

	// Add the tank for level 0 only:
	if (l == 0) {
		map.entities[0] = calloc(sizeof(mapentity_t), 1);
		map.entities[0]->class = 0xF0;			// CLASS_TANK
		map.entities[0]->x = 0x17 << 5;
		map.entities[0]->y = 0x37 << 5;
	}

	for (i=0; i<numetys; ++i) {
		map.entities[i+extra] = calloc(sizeof(mapentity_t), 1);
		map.entities[i+extra]->class = etyclass[i];
		map.entities[i+extra]->x = (int)(etyx[i]) << 5;
		map.entities[i+extra]->y = (int)(etyy[i]) << 5;
#if 0
		if (etyclass[i] == 0xFF) {
			fprintf(stderr, "Marker data: 0x%02X, 0x%02X\n", etyx[i], etyy[i]);
		}
#endif
	}

	map.gravity = 0.5;
	// Sixth level is icy...
	if (l == 5)
		map.friction = 0.0078125;
	else
		map.friction = 0.25;

	// Set the music file:
	map.music_filename = calloc(32, 1);
	sprintf(map.music_filename, "music/map%02X.ogg", n % 8);

	// Set the game DLL file:
	map.game_filename = calloc(16, 1);
	sprintf(map.game_filename, "game/map%02X", n);

}

#define FLIP_H			0x01
#define FLIP_V			0x02

// Draw 8x8 sprites from the ROM onto the RGBA texture:
void Draw8x8Sprite (Uint32 *pixels, int s, int m, int tx, int ty,
					Uint8 *sprite, Uint32 *palette, Uint8 *sprpalette,
					int width, unsigned char flipflags, char noalpha)
{
	int	x, y;
	int a, b, idx, r, g;
	unsigned long col;

	#define SetPixel(x, y, c) { \
		pixels[( (ty + (y << 1) + 0) * width ) | (tx + (x << 1) + 0)] = c; \
		pixels[( (ty + (y << 1) + 0) * width ) | (tx + (x << 1) + 1)] = c; \
		pixels[( (ty + (y << 1) + 1) * width ) | (tx + (x << 1) + 0)] = c; \
		pixels[( (ty + (y << 1) + 1) * width ) | (tx + (x << 1) + 1)] = c; \
	}

	for (y=0;y<8;++y)
	{
		for (x=0;x<8;++x)
		{
			a = ((sprite[(s<<4)+y] & (1 << x)) >> x);
			b = ((sprite[(s<<4)+y+8] & (1 << x)) >> x) << 1;
			idx = a | b;

			// Make alpha value 0 for 0-bit.
			if (noalpha) {
				col = palette[sprpalette[idx + (m << 2)]];
				// Nice looking hack for background tiles:
				// If the grayscale value of the color is minimal, then make it transparent:
				// Works with overhanging cliffs, etc. that will partially hide the tank since
				// they are drawn after it.
#if 0
				r = (col & 0xFF);
				g = (col >> 8) & 0xFF;
				b = (col >> 16) & 0xFF;
				if ( (r * 0.325) + (g * 0.415) + (b * 0.260) < 12 ) col = fromargb(0, r, g, b);
#endif
				// Produces an unfortunate side-effect when travelling thru doors.
			} else {
				if (idx == 0) col = fromargb(0, 0, 0, 0);
				else col = palette[sprpalette[idx + (m << 2)]];
			}

			// Draw to the 128x128 texture:
			if (flipflags == 0) SetPixel((7-x), y, col)
			else if (flipflags == FLIP_H) SetPixel(x, y, col)
			else if (flipflags == FLIP_V) SetPixel((7-x), (7-y), col)
			else if (flipflags == (FLIP_H | FLIP_V)) SetPixel(x, (7-y), col)
		}
	}
}

/* write a png file */
int SDL_SavePNG(SDL_Surface *texture, const char *filename) {
	FILE	*fp;
	png_structp		png_ptr;
	png_infop		info_ptr;
	png_color_8p	sig_bit;
	png_uint_32	k;
	png_bytep	row_pointers[256];

	/* open the file */
	fp = fopen(filename, "wb");
	if (fp == NULL)
		return -1;

	/* Create and initialize the png_struct with the desired error handler
		* functions.  If you want to use the default stderr and longjump method,
		* you can supply NULL for the last three parameters.  We also check that
		* the library version is compatible with the one used at compile time,
		* in case we are using dynamically linked libraries.  REQUIRED.
		*/
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		fclose(fp);
		return -1;
	}

	/* Allocate/initialize the image information data.  REQUIRED */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
		return -1;
	}

	/* Set error handling.  REQUIRED if you aren't supplying your own
		* error handling functions in the png_create_write_struct() call.
		*/
	if (setjmp(png_jmpbuf(png_ptr))) {
		/* If we get here, we had a problem reading the file */
		fclose(fp);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return -1;
	}

	/* One of the following I/O initialization functions is REQUIRED */
	/* set up the output control if you are using standard C streams */
	png_init_io(png_ptr, fp);

	/* Set the image information here.  Width and height are up to 2^31,
		* bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
		* the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
		* PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
		* or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
		* PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
		* currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
		*/
	png_set_IHDR(png_ptr, info_ptr, 256, 256, 8, PNG_COLOR_TYPE_RGB_ALPHA,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	/* Write the file header information.  REQUIRED */
	png_write_info(png_ptr, info_ptr);

	/* swap location of alpha bytes from ARGB to RGBA */
	//png_set_swap_alpha(png_ptr);

	for (k = 0; k < 256; k++)
		row_pointers[k] = (Uint8 *)(texture->pixels) + k*256*4;

	png_write_image(png_ptr, row_pointers);

	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);

	/* that's it */
	return 0;
}

// Saves all variants of sprites and background textures to files:
void SaveTextures() {
	int		i, j, k, l, n, cm;
	unsigned short	m, totalsprites;

	mkdir("textures");
	chmod("textures", 0755);
	mkdir("maps");
	chmod("maps", 0755);
	
	// Palette lookup for sprites:  (I should read these from the ROM instead
	// but I'll have to find them first.... dammit)
	unsigned char sprpalette[16] = 
		{0x0F, 0x30, 0x15, 0x0F,		// Stage 1
		 0x0F, 0x30, 0x00, 0x0F,
		 0x0F, 0x3B, 0x1B, 0x0F,
		 0x00, 0x10, 0x20, 0x30};
	char	filename[256], tempf[256];
	long	numtiles[4];

	SDL_Surface		*texture;
	Uint32			rmask, gmask, bmask, amask;

	/* SDL interprets each pixel as a 32-bit number, so our masks must depend
	   on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	texture = SDL_CreateRGBSurface(SDL_SWSURFACE, 256, 256, 32,
								   rmask, gmask, bmask, amask);
	// Ignore the alpha channel, it screws up the colorkey and distorts colors slightly
	SDL_SetAlpha(texture, 0, 0);

	if(texture == NULL) {
		fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
		exit(1);
	}

#define SAVE_TEXTURE(format, mysprt, mypal, flipflags, noalpha) \
	for (j=0; j<4; ++j) { \
		SDL_LockSurface(texture); \
		for (i=0; i<256; ++i) \
			Draw8x8Sprite(texture->pixels, i, j, (i >> 4) << 4, (i & 0x0F) << 4, \
						  mysprt, palette, mypal, 256, flipflags, noalpha); \
		SDL_UnlockSurface(texture); \
		sprintf(filename, format, j); \
		SDL_SavePNG(texture, filename); \
	}

	if (sprite) free(sprite);
	sprite = calloc(0x1000, 1);

	// Read fg sprite pages:
	for (n=0; n<4; ++n) {
		fseek(nesrom, prgsize + (n << 12), SEEK_SET);
		fread(sprite, 1, 0x1000, nesrom);

		sprintf(tempf, "textures/fgtile%02X%%X.png", n);
		SAVE_TEXTURE(tempf, sprite, sprpalette, 0, 0)
	}

	for (n=6; n<7; ++n) {
		fseek(nesrom, prgsize + (n << 12), SEEK_SET);
		fread(sprite, 1, 0x1000, nesrom);

		sprintf(tempf, "textures/fgtile%02X%%X.png", n);
		SAVE_TEXTURE(tempf, sprite, sprpalette, 0, 0)
	}

	for (n=0x10; n<0x15; ++n) {
		fseek(nesrom, prgsize + (n << 12), SEEK_SET);
		fread(sprite, 1, 0x1000, nesrom);

		sprintf(tempf, "textures/fgtile%02X%%X.png", n);
		SAVE_TEXTURE(tempf, sprite, sprpalette, 0, 0)
	}

	for (n=0x1E; n<0x1F; ++n) {
		fseek(nesrom, prgsize + (n << 12), SEEK_SET);
		fread(sprite, 1, 0x1000, nesrom);

		sprintf(tempf, "textures/fgtile%02X%%X.png", n);
		SAVE_TEXTURE(tempf, sprite, sprpalette, 0, 0)
	}

	typedef struct {
		int		sprpage, map;
	} bosspageinfo_t;

	const bosspageinfo_t	bossinfos[8] = {
		{25, 8},		// Level 1 boxx
		{24, 11},		// Level 2 boss
		{25, 9},		// Level 3 boss
		{24, 14},		// Level 4 boss
		{26, 10},		// Level 5 boss
		{24, 12},		// Level 6 boss
		{24, 15},		// Level 7 boss
		{27, 13},		// Level 8 boss
	};

	for (n=0; n<8; ++n) {
		// Read the boss pages:
		fseek(nesrom, prgsize + (bossinfos[n].sprpage << 12), SEEK_SET);
		fread(sprite, 1, 0x1000, nesrom);

		// Read the palette:
		fseek(nesrom, paletteofs[bossinfos[n].map], SEEK_SET);
		fread(&mappalette, 1, 0x10, nesrom);

		sprintf(tempf, "textures/boss%02X%%X.png", n + 8);
		SAVE_TEXTURE(tempf, sprite, mappalette, 0, 0)
	}
	
	// Save 8 level background tilesets:
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

		for (i=0; i<4 * 256; ++i)
			spritemapping[i] = allsprites[i] = -1;
		for (k=0; k<4; ++k)
			numtiles[k] = 0;

		// Map out which sprites are on which colormap:
		/*
		for (i=0; i<(levelsizes[(l<<2)+0] >> 2); ++i) {
			for (k=0; k<4; ++k) {
				j = ((mapflags[i] & 3) << 8) | (map2x2[(i<<2)+k]);
				m = ((mapflags[i] & 3) << 8) | (numtiles[mapflags[i] & 3]);
				if (allsprites[j] == -1) {
					allsprites[j] = numtiles[mapflags[i] & 3];
					spritemapping[m] = j;
					map2x2[(i<<2)+k] = numtiles[mapflags[i] & 3];
					// Keep a running count of how many tiles are in this colormap:
					numtiles[mapflags[i] & 3]++;
				} else {
					spritemapping[m] = allsprites[j];
					map2x2[(i<<2)+k] = allsprites[j];
				}
			}
		}
		*/

		// Draw out the textures:
		m = 0;
		for (j=0; j<4; ++j) {
			SDL_LockSurface(texture);
			memset(texture->pixels, 0, sizeof(Uint32) * 256 * 256);
			m = j << 8;
			for (i=0; i<256; ++i, ++m) {
				Draw8x8Sprite(texture->pixels,
							m & 0xFF, (m >> 8) & 3,
							(i >> 4) << 4, (i & 0x0F) << 4,
							tiles, palette, mappalette, 256, 0, 1);
			}
			SDL_UnlockSurface(texture);
			// Save the texture:
			sprintf(filename, "textures/bgtile%X%X.png", l, j);
			SDL_SavePNG(texture, filename);
		}

		// Do the map conversion:
		ConvertMap(l, n, 4);
		// Save the map:
		sprintf(tempf, "maps/map%02X.bma", n);
		printf("Saving to %s\n", tempf);
		SaveMap(tempf);
		//FreeMap();

	}

	SDL_FreeSurface(texture);
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

	printf("All textures successfully stored in textures/\n");

	return 0;
};

