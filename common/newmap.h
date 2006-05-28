#ifndef NEWMAP_H
#define NEWMAP_H

/*
	Limitations of texture usage:
        - All 16x16 sprites for any 2x2 map tile must remain on the same texture.
        - Only 4 256x256 textures may be used for any given map.
*/

// A door linking structure:  (one must exist for a teleportation door to work)
typedef struct mapdoor_t {
	Uint16	x, y;					// Location of the door
	char	*targetmap;				// Filename of the target map
	Uint32	tag;					// Tag/group of the door (matches tag for door in other level)
} mapdoor_t;

// A scroll region:
typedef struct mapregion_t {
	Uint16	lx, ty, rx, by;
} mapregion_t;

// An entity placeholder.
typedef struct mapentity_t {
	Uint32	class;					// Class # of the entity to spawn here
	Uint32	x, y;					// Location of where to spawn.
	// Possible future expansion for spawning flags and conditions:
} mapentity_t;

typedef struct map_t {
	const char	*filename;

	Uint8	numtextures;
	char	**texturefile;
	char	*music_filename;
	char	*game_filename;

	Uint16	width, height;

    Uint8	*map2x2;
    Uint8	map2x2_size;
    Uint8	*mapflags;
    Uint8	*map;

	Uint32	num_doors, num_entities, num_regions;
	mapdoor_t		**doors;
	mapentity_t		**entities;
	mapregion_t		**regions;

    Uint8	texturefile_loaded;
    Uint8	mapflags_loaded;
    Uint8	map2x2_loaded;
    Uint8	map_loaded;
    Uint8	entities_loaded;
    Uint8	doors_loaded;
	Uint8	regions_loaded;

	fixed	friction, gravity;
} map_t;

#define		DEFNCHNK(a, b, c, d)	(((d) << 24) | ((c) << 16) | ((b) << 8) | (a))

typedef enum {
	MAPCHUNK_TEXTURES	=	DEFNCHNK('T', 'X', 'T', 'R'),
	MAPCHUNK_MAPTILES	=	DEFNCHNK('T', 'I', 'L', 'E'),
	MAPCHUNK_MAPFLAGS	=	DEFNCHNK('F', 'L', 'G', 'S'),
	MAPCHUNK_MAPEXTRA	=	DEFNCHNK('X', 'T', 'R', 'A'),
	MAPCHUNK_MAPDATA	=	DEFNCHNK('M', 'A', 'P', 'D'),
	MAPCHUNK_MAPENTS	=	DEFNCHNK('E', 'N', 'T', 'S'),
	MAPCHUNK_MAPDOORS	=	DEFNCHNK('D', 'O', 'O', 'R'),
	MAPCHUNK_MAPREGIONS	=	DEFNCHNK('R', 'G', 'N', 'S'),
	MAPCHUNK_MAPMUSIC	=	DEFNCHNK('M', 'U', 'S', 'C'),
	MAPCHUNK_GAMEDLL	=	DEFNCHNK('G', 'A', 'M', 'E'),
	MAPCHUNK_END		=	DEFNCHNK('E', 'N', 'D', 0)
} mapchunk;

extern map_t	map;

#define 	MAPFLAG_JASON   0x04
#define 	MAPFLAG_DOOR    0x08
#define 	MAPFLAG_HARMFUL 0x10
#define 	MAPFLAG_LADDER  0x20
#define 	MAPFLAG_WATER   0x40
#define 	MAPFLAG_SOLID   0x80
#define		MAPFLAG_ALL		0xFC

int	LoadMap(const char *filename);
int	SaveMap(const char *filename);
int	FreeMap();
int	NewMap();
int	InitMap();

fixed	wrap_map_coord_x(fixed a);
fixed	wrap_map_coord_y(fixed a);
long wrap_x(long x);
long wrap_y(long y);
int		test_map_x_lt(fixed x1, fixed x2);
int		test_map_x_gt(fixed x1, fixed x2);
int		test_map_y_lt(fixed y1, fixed y2);
int		test_map_y_gt(fixed y1, fixed y2);

Uint8	gettileat(long ex, long ey);
void	settileat(long ex, long ey, Uint8 tile);

#endif
