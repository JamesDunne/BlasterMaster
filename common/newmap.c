#include "common.h"
#include "newmap.h"

//#ifdef WIN32
//#define ZLIB_WINAPI
//#endif
#include <zlib.h>

map_t	map;

#define MakeReadFunc(type) \
type	Read##type(FILE *f) { \
	type	dummy; \
    fread(&dummy, 1, sizeof(type), f); \
    return dummy; \
}

#define MakeWriteFunc(type) \
void	Write##type(FILE *f, type dummy) { \
    fwrite(&dummy, 1, sizeof(type), f); \
}

MakeReadFunc(Uint8)
MakeReadFunc(Uint16)
MakeReadFunc(Uint32)
MakeWriteFunc(Uint8)
MakeWriteFunc(Uint16)
MakeWriteFunc(Uint32)

#define	MAP_SIGNATURE	0x1A414D42

int LoadMap(const char *filename) {
	FILE	*mapfile;
    Uint32	sig;
	uLongf	uncomplen;
	uLong	complen;
	Bytef	*compbuf;
    Uint8	len;
    int		i;

	mapchunk	chunktype;
	Uint32		chunksize;
	Uint32		marker;

    mapfile = fopen(filename, "rb");
    if (!mapfile) {
    	fprintf(stderr, "Could not open '%s'\n", filename);
		return -1;
    }

    // Check the signature:
	fread(&sig, 1, 4, mapfile);
	if (sig != MAP_SIGNATURE) {
		fprintf(stderr, "%s does not seem to be a valid map file\n", filename);
        return -2;
    }

    // Free the last map loaded (if exists):  (safe to call regardless)
    FreeMap();

	// Read chunks and ignore unknown ones:
	while (!feof(mapfile)) {
		// Read the chunk type and size:
		fread(&chunktype, 1, sizeof(mapchunk), mapfile);
		if (feof(mapfile)) break;
		fread(&chunksize, 1, sizeof(Uint32), mapfile);
		if (feof(mapfile)) break;

		marker = ftell(mapfile);

		switch (chunktype) {
			case MAPCHUNK_TEXTURES:
				// How many texture files this map uses:
				map.numtextures = ReadUint8(mapfile);
				// Allocate memory to store the filenames:
				map.texturefile = (char **) malloc(sizeof(char *) * map.numtextures);
				map.texturefile_loaded = 1;

				// Read which texture files this map uses:
				for (i=0; i<map.numtextures; ++i) {
					// Allocate memory with room for trailing \:
					len = ReadUint8(mapfile) + 1;
					map.texturefile[i] = (char *) malloc(len * sizeof(char));
					// Zero the memory:
					memset(map.texturefile[i], 0, len);
					// Read the string:
					fread(map.texturefile[i], 1, len - 1, mapfile);
				}

				break;
			case MAPCHUNK_MAPTILES:
				// Load the 2x2 tile lookup table:
				map.map2x2_size = chunksize >> 2;
				map.map2x2 = (Uint8 *) malloc(sizeof(Uint8) * map.map2x2_size * 4);
				map.map2x2_loaded = 1;
				memset(map.map2x2, 0, sizeof(Uint8) * map.map2x2_size * 4);
				fread(map.map2x2, sizeof(Uint8) * 4, map.map2x2_size, mapfile);
				break;
			case MAPCHUNK_MAPFLAGS:
				// Load the mapflags structure (always same size as 2x2 lookup table):
				map.mapflags = (Uint8 *) malloc(chunksize);
				map.mapflags_loaded = 1;
				memset(map.mapflags, 0, sizeof(Uint8) * chunksize);
				fread(map.mapflags, sizeof(Uint8), chunksize, mapfile);
				break;
			case MAPCHUNK_MAPEXTRA:
				// Load extra information for the map (friction, gravity constants, etc.)
				fread(&map.friction, 1, sizeof(fixed), mapfile);
				fread(&map.gravity, 1, sizeof(fixed), mapfile);
				break;
			case MAPCHUNK_MAPDATA:
				// Load the size of the map:
				map.width = ReadUint16(mapfile);
				map.height = ReadUint16(mapfile);

				// Load the map itself:
				map.map = (Uint8 *) malloc(map.width * map.height * sizeof(Uint8));
				map.map_loaded = 1;
				memset(map.map, 0, map.width * map.height * sizeof(Uint8));

				// Decompress the map data:
				complen = ReadUint32(mapfile);
				compbuf = (Uint8 *)malloc(complen);
				fread(compbuf, 1, complen, mapfile);

				uncomplen = map.width * map.height * sizeof(Uint8);
				if (uncompress(map.map, &uncomplen, compbuf, complen) != Z_OK) {
					free(compbuf);
					fprintf(stderr, "Could not uncompress map data!\n");
					return -1;
				}
				free(compbuf);
				break;
			case MAPCHUNK_MAPENTS:
				// Load the entity placeholders:
				map.num_entities = chunksize / sizeof(mapentity_t);
				// Allocate memory and read the block straight in:
				map.entities = calloc(sizeof(mapentity_t *), map.num_entities);
				for (i=0; i<map.num_entities; ++i) {
					map.entities[i] = calloc(sizeof(mapentity_t), 1);
					fread(map.entities[i], 1, sizeof(mapentity_t), mapfile);
				}
				map.entities_loaded = 1;
				break;
			case MAPCHUNK_MAPDOORS:
				// Load the linking door placeholders:
				map.num_doors = ReadUint32(mapfile);
				// Load one at a time, because we must read dynamically
				// sized strings:
				map.doors = calloc(sizeof(mapdoor_t *), map.num_doors);
				for (i=0; i<map.num_doors; ++i) {
					map.doors[i] = calloc(sizeof(mapdoor_t), 1);
					map.doors[i]->x = ReadUint16(mapfile);
					map.doors[i]->y = ReadUint16(mapfile);
					len = ReadUint8(mapfile);
					if (len != 0) {
						map.doors[i]->targetmap = calloc(len + 1, 1);
						fread(map.doors[i]->targetmap, 1, len, mapfile);
						map.doors[i]->targetmap[len] = 0;
					} else {
						map.doors[i]->targetmap = NULL;
					}
					map.doors[i]->tag = ReadUint32(mapfile);
				}
				map.doors_loaded = 1;
				break;
			case MAPCHUNK_MAPMUSIC:
				// Read the filename, and use the chunksize as the length:
				map.music_filename = calloc(chunksize+1, 1);
				fread(map.music_filename, 1, chunksize, mapfile);
				break;
			case MAPCHUNK_GAMEDLL:
				// Read the filename, and use the chunksize as the length:
				map.game_filename = calloc(chunksize+1, 1);
				fread(map.game_filename, 1, chunksize, mapfile);
				break;
			case MAPCHUNK_END:
				break;
		}

		// If the file marker is not to the next chunk, then move to it:
		if ( (ftell(mapfile) - marker) < (marker + chunksize) )
			fseek(mapfile, (marker + chunksize) - ftell(mapfile), SEEK_CUR);
	}

    // We're done, close the file:
    fclose(mapfile);

	return 0;
}

int SaveMap(const char *filename) {
	FILE	*mapfile = NULL;
    Uint32	sig;

	uLong	uncomplen;
	uLongf	complen;
	Bytef	*compbuf;

	Uint32	tmpchunksize;

	Uint8	len;
    int		i;

#define	WRITE_CHUNK(ctype, csize) { \
		mapchunk	chunktype; \
		Uint32		chunksize; \
		chunktype = ctype; \
		chunksize = csize; \
		fwrite(&chunktype, 1, sizeof(mapchunk), mapfile); \
		fwrite(&chunksize, 1, sizeof(Uint32), mapfile); \
	}

	// Check to see if we have everything to save:
    if (map.texturefile_loaded == 0) {
		fprintf(stderr, "No texture filenames were assigned to this map!\n");
        return -1;
    }

	if (map.map2x2_loaded == 0) {
		fprintf(stderr, "No tile lookup table was assigned to this map!\n");
        return -1;
    }

	if (map.mapflags_loaded == 0) {
		fprintf(stderr, "No tile flags table was assigned to this map!\n");
        return -1;
    }

	if (map.map_loaded == 0) {
		fprintf(stderr, "No map data was assigned to this map!\n");
        return -1;
    }

	// Finally, open the file to write:
    mapfile = fopen(filename, "wb");
    if (!mapfile) {
    	fprintf(stderr, "Could not open '%s' for writing\n", filename);
		return -1;
    }

    // Write the signature:
    sig = MAP_SIGNATURE;
    fwrite(&sig, 1, 4, mapfile);

	// -------------------------------------

	// Create a chunk:
	tmpchunksize = 1;
    for (i=0; i<map.numtextures; ++i)
		tmpchunksize += 1 + strlen(map.texturefile[i]);

	WRITE_CHUNK(MAPCHUNK_TEXTURES, tmpchunksize);

	// How many texture files this map uses:
	WriteUint8(mapfile, map.numtextures);

    // Write which texture files this map uses:
    for (i=0; i<map.numtextures; ++i) {
        WriteUint8(mapfile, strlen(map.texturefile[i]));
        // Write the string:
        fwrite(map.texturefile[i], 1, strlen(map.texturefile[i]), mapfile);
	}

	// -------------------------------------

	// Create a chunk:
	WRITE_CHUNK(MAPCHUNK_MAPTILES, sizeof(Uint8) * 4 * map.map2x2_size)
	fwrite(map.map2x2, sizeof(Uint8) * 4, map.map2x2_size, mapfile);

	// -------------------------------------

	// Create a chunk:
	WRITE_CHUNK(MAPCHUNK_MAPFLAGS, sizeof(Uint8) * map.map2x2_size)
    fwrite(map.mapflags, sizeof(Uint8), map.map2x2_size, mapfile);

	// -------------------------------------

	// Create a chunk:
	WRITE_CHUNK(MAPCHUNK_MAPEXTRA, sizeof(fixed) * 2)
	fwrite(&map.friction, 1, sizeof(fixed), mapfile);
	fwrite(&map.gravity, 1, sizeof(fixed), mapfile);

	// -------------------------------------

	// Compress the map data first:

	// zlib says to make the buffer 0.1% larger plus 12 bytes, so I'll make it 0.2% larger
	complen = uncomplen = (map.width * map.height * sizeof(Uint8));
	complen = (uLongf)(complen * 1.02) + 12;

	compbuf = (Uint8 *)malloc(uncomplen);
	if (compress(compbuf, &complen, map.map, uncomplen) != Z_OK) {
		fprintf(stderr, "Could not compress map data!\n");
		return -1;
	}

	// Create a chunk:
	WRITE_CHUNK(MAPCHUNK_MAPDATA, (sizeof(Uint16) * 2) + sizeof(Uint32) + complen)

	// Write the size of the map:
    WriteUint16(mapfile, map.width);
    WriteUint16(mapfile, map.height);

	WriteUint32(mapfile, complen);
	fwrite(compbuf, 1, complen, mapfile);

	free(compbuf);

	// -------------------------------------

	if (map.num_entities > 0) {
		// Write the entity placeholders:
		WRITE_CHUNK(MAPCHUNK_MAPENTS, (sizeof(mapentity_t) * map.num_entities))
		for (i=0; i<map.num_entities; ++i)
			fwrite(map.entities[i], 1, sizeof(mapentity_t), mapfile);
	}

	// -------------------------------------

	if (map.num_doors > 0) {
		// First, determine the size of the chunk:
		tmpchunksize = sizeof(Uint32);
		for (i=0; i<map.num_doors; ++i)
			if (map.doors[i]->targetmap != NULL)
				tmpchunksize += (sizeof(Uint16) * 4) + (strlen(map.doors[i]->targetmap) + sizeof(Uint8));
			else
				tmpchunksize += (sizeof(Uint16) * 4) + sizeof(Uint8);

		// Create a chunk:
		WRITE_CHUNK(MAPCHUNK_MAPDOORS, tmpchunksize)

		WriteUint32(mapfile, map.num_doors);
		for (i=0; i<map.num_doors; ++i) if (map.doors[i] != NULL) {
			WriteUint16(mapfile, map.doors[i]->x);
			WriteUint16(mapfile, map.doors[i]->y);
			if (map.doors[i]->targetmap != NULL) {
				WriteUint8(mapfile, strlen(map.doors[i]->targetmap));
				fwrite(map.doors[i]->targetmap, 1, strlen(map.doors[i]->targetmap), mapfile);
			} else {
				WriteUint8(mapfile, 0);
			}
			WriteUint32(mapfile, map.doors[i]->tag);
		}
	}

	// Write the map's music filename:
	if (map.music_filename != NULL) {
		WRITE_CHUNK(MAPCHUNK_MAPMUSIC, strlen(map.music_filename))
		fwrite(map.music_filename, 1, strlen(map.music_filename), mapfile);
	}

	// Write the map's game dll filename:
	if (map.game_filename != NULL) {
		WRITE_CHUNK(MAPCHUNK_GAMEDLL, strlen(map.game_filename))
		fwrite(map.game_filename, 1, strlen(map.game_filename), mapfile);
	}

	// Create a chunk:
	WRITE_CHUNK(MAPCHUNK_END, 0)

	// We're done, close the file:
    fclose(mapfile);

	return 0;
}

int FreeMap() {
	int	i;
    if (map.texturefile_loaded) {
		for (i=0;i<map.numtextures;++i)
        	free(map.texturefile[i]);
        free(map.texturefile);
        map.numtextures = 0;
        map.texturefile = NULL;
        map.texturefile_loaded = 0;
    }
    if (map.mapflags_loaded) {
    	free(map.mapflags);
        map.mapflags = NULL;
        map.mapflags_loaded = 0;
    }
    if (map.map2x2_loaded) {
    	free(map.map2x2);
        map.map2x2 = NULL;
        map.map2x2_size = 0;
        map.map2x2_loaded = 0;
    }
    if (map.map_loaded) {
    	free(map.map);
        map.map = NULL;
        map.map_loaded = 0;
    }
	if (map.entities_loaded) {
		for (i=0; i<map.num_entities; ++i)
			free(map.entities[i]);
		free(map.entities);
		map.entities = NULL;
		map.entities_loaded = 0;
		map.num_entities = 0;
	}
	if (map.doors_loaded) {
		for (i=0; i<map.num_doors; ++i) {
			if (map.doors[i]->targetmap)
				free(map.doors[i]->targetmap);
			free(map.doors[i]);
		}
		free(map.doors);
		map.doors = NULL;
		map.doors_loaded = 0;
		map.num_doors = 0;
	}
	if (map.music_filename != NULL) {
		free(map.music_filename);
		map.music_filename = NULL;
	}
	if (map.game_filename != NULL) {
		free(map.game_filename);
		map.game_filename = NULL;
	}
    return 0;
}

int NewMap() {
	char	tempf[256];
    int		i;

    // Free the last map:
	FreeMap();
	InitMap();

    // Reset texture names:
	map.numtextures = 4;
    map.texturefile = (char **) malloc(sizeof(char *) * 4);
	map.texturefile_loaded = 1;

    for (i=0; i<map.numtextures; ++i) {
    	sprintf(tempf, "textures/bgtile0%X.png", i);
		map.texturefile[i] = (char *) malloc(strlen(tempf) + 1);
		strcpy(map.texturefile[i], tempf);
    }

 	// Load the 2x2 tile lookup table:
    map.map2x2_size = 1;
    map.map2x2 = (Uint8 *) malloc(sizeof(Uint8) * map.map2x2_size * 4);
    map.map2x2_loaded = 1;
    memset(map.map2x2, 0, sizeof(Uint8) * map.map2x2_size * 4);
	// Blank block:
    map.map2x2[(0<<2) + 0] = 0;
	map.map2x2[(0<<2) + 1] = 0;
	map.map2x2[(0<<2) + 2] = 0;
	map.map2x2[(0<<2) + 3] = 0;

    // Load the mapflags structure (always same size as 2x2 lookup table):
	map.mapflags = (Uint8 *) malloc(sizeof(Uint8) * map.map2x2_size);
    map.mapflags_loaded = 1;
    memset(map.mapflags, 0, sizeof(Uint8) * map.map2x2_size);

    // Load the size of the map:
    map.width = 128;
    map.height = 128;

	// Load the map itself:
	map.map = (Uint8 *) malloc(map.width * map.height * sizeof(Uint8));
    map.map_loaded = 1;
	memset(map.map, 0, map.width * map.height * sizeof(Uint8));

	return 0;
}

int InitMap() {
	int	i;
	map.numtextures = 0;
	map.texturefile = NULL;
	map.texturefile_loaded = 0;
	map.mapflags = NULL;
	map.mapflags_loaded = 0;
	map.map2x2 = NULL;
	map.map2x2_size = 0;
	map.map2x2_loaded = 0;
	map.map = NULL;
	map.map_loaded = 0;
	map.entities = NULL;
	map.entities_loaded = 0;
	map.num_entities = 0;
	map.doors = NULL;
	map.doors_loaded = 0;
	map.num_doors = 0;
	map.music_filename = NULL;
	map.game_filename = NULL;

	return 0;
}

fixed wrap_map_coord_x(fixed a) {
    // Help to wrap around the sides of the map:
    if (a < 0) return a + (map.width * 32.0);
    if (a >= (map.width * 32.0)) return a - (map.width * 32.0);
    return a;
}

fixed wrap_map_coord_y(fixed a) {
    // Help to wrap around the sides of the map:
    if (a < 0) return a + (map.height * 32.0);
    if (a >= (map.height * 32.0)) return a - (map.height * 32.0);
    return a;
}

int test_map_x_lt(fixed x1, fixed x2) {
	// If x1 is on the left half of the map and x2 is on the right...
	if ((x2 - x1) >= (map.width * 16)) {
		return x1 < x2 - (map.width * 32);
	} else if ((x1 - x2) >= (map.width * 16)) {
		return x1 - (map.width * 32) < x2;
	} else {
		return x1 < x2;
	}
}

int test_map_x_gt(fixed x1, fixed x2) {
	// If x1 is on the left half of the map and x2 is on the right...
	if ((x2 - x1) >= (map.width * 16)) {
		return x1 > x2 - (map.width * 32);
	} else if ((x1 - x2) >= (map.width * 16)) {
		return x1 - (map.width * 32) > x2;
	} else {
		return x1 > x2;
	}
}

int test_map_y_lt(fixed y1, fixed y2) {
	//y1 = wrap_map_coord_y(y1);
	//y2 = wrap_map_coord_y(y2);
	// If y1 is on the top half of the map and y2 is on the bottom...
	if ((y2 - y1) >= (map.height * 16)) {
		return y1 < y2 - (map.height * 32);
	} else if ((y1 - y2) >= (map.height * 16)) {
		return y1 - (map.height * 32) < y2;
	} else {
		return y1 < y2;
	}
}

int test_map_y_gt(fixed y1, fixed y2) {
	//y1 = wrap_map_coord_y(y1);
	//y2 = wrap_map_coord_y(y2);
	// If y1 is on the top half of the map and y2 is on the bottom...
	if ((y2 - y1) >= (map.height * 16)) {
		return y1 > y2 - (map.height * 32);
	} else if ((y1 - y2) >= (map.height * 16)) {
		return y1 - (map.height * 32) > y2;
	} else {
		return y1 > y2;
	}
}

long wrap_x(long x) {
	if (x < 0) return x + map.width;
	if (x >= map.width) return x - map.width;
	return x;
}

long wrap_y(long y) {
	if (y < 0) return y + map.height;
	if (y >= map.height) return y - map.height;
	return y;
}

Uint8 gettileat(long ex, long ey) {
	return map.map[(wrap_y(ey >> 5) * map.width) + wrap_x(ex >> 5)];
}

void settileat(long ex, long ey, Uint8 tile) {
	map.map[(wrap_y(ey >> 5) * map.width) + wrap_x(ex >> 5)] = tile;
}

