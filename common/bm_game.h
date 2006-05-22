// bm_game.h
#include "entflags.h"
#include "newmap.h"

// Functions and variables from the engine given to the DLL:
typedef struct hostfunctions {
	// Determines what functions to check:
	// devmode = 0:		dll connected to the game engine
	// devmode = 1:		dll connected to the map editor
	int	devmode;

    // -- Main sprite routines --
    int (*put_sprite)(fixed x,fixed y,int m,GLuint sprite);
    int (*put_sprite_hflip)(fixed x,fixed y,int m,GLuint sprite);
    int (*put_sprite_vflip)(fixed x,fixed y,int m,GLuint sprite);
    int (*put_sprite_hvflip)(fixed x,fixed y,int m,GLuint sprite);
    int (*put_bgtile)(fixed x,fixed y,int m,unsigned char t, int bg);

    void (*Draw2x2BGTile)(fixed x, fixed y, int t, int bg);

	void	(*LoadTexture)(int page, const char *filename);

    // Entity management functions:
    entity	(*e_spawn)(class_type class);
    entity	(*e_spawnat)(class_type class, fixed x, fixed y);
    entity	(*e_spawnatmap)(class_type class, fixed mx, fixed my);
	entity	(*e_findfirst)();
	entity	(*e_findnext)();
	void	(*e_ptr2uid)(entity_t **p);
	void	(*e_uid2ptr)(entity_t **p);

    void	(*e_kill)(entity e);
    void	(*e_damage)(entity target, entity attacker, short damage);
    void	(*control_switch)(entity e);
    Uint8	(*gettileat)(long ex, long ey);
    void	(*settileat)(long ex, long ey, Uint8 tile);

	fixed	(*wrap_map_coord_x)(fixed a);
	fixed	(*wrap_map_coord_y)(fixed a);

	int		(*LoadLevel)(const char *filename);

	// Sound functions:
	int		(*sndPrecache)(const char *filename);
	int		(*sndFree)(int sound);
	int		(*sndPlay)(int sound, int channel, int loops);
	int		(*sndSetVolume)(int channel, int volume);
	int		(*sndIsPlaying)(int channel);
	int		(*sndStop)(int channel);

	// Pointers to the original variables:
	map_t	*map;

	// Entity who has control of input:
	controlled_entity_t	**player;
	entity	*world;
	entity	*entities;
	int		*last_entity;

	// Acceleration scalar value perceived from joystick:
	double	*accel_scale;
	
	// Game state variables:
	fixed	*screen_mx, *screen_my;
	fixed	*old_screen_mx, *old_screen_my;
	fixed	*screen_w, *screen_h;
	fixed 	*scroll_left, *scroll_right;
	fixed 	*scroll_top, *scroll_bottom;
	int		*quit, *game_paused;
} hostfunctions;

// Functions and variables from the DLL given to the engine:
typedef struct dllfunctions {
	void (*init)();
	void (*pre_render)();
	void (*post_render)();
	// All the class functions:
	int		numclasses;
	class_properties_t	**class_properties;			// properties for each class
	Uint32				*spawnflags;				// flags for each class
} dllfunctions;

void UnloadGame (void **game_library);
dllfunctions *GetGameAPI (const char *gamename, hostfunctions *thost, void **game_library);
