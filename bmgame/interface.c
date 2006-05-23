#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "entflags.h"
#include "bm_game.h"

hostfunctions		*host = NULL;
dllfunctions		*game = NULL;

void init() {
	int	i, reg;
	int	lx, rx, ty, by;

	// Load the level:
	host->LoadLevel("maps/map00.bma");

	reg = -1;
	for (i = 0; i < host->map->num_regions; ++i) {
		lx = (host->map->regions[i]->lx << 5);
		rx = (host->map->regions[i]->rx << 5) + 31;
		ty = (host->map->regions[i]->ty << 5);
		by = (host->map->regions[i]->by << 5) + 31;
		int	test = 0;
		if (lx > rx) {
			if ((*host->player)->x >= lx && (*host->player)->x <= (host->map->width << 4)) test = -1;
			else if ((*host->player)->x >= 0 && (*host->player)->x <= rx) test = -1;
			else test = 0;
		} else {
			if ((*host->player)->x >= lx && (*host->player)->x <= rx) test = -1;
			else test = 0;
		}
		if (test == 0) continue;
		if (ty > by) {
			if ((*host->player)->y >= ty && (*host->player)->y <= (host->map->height << 4)) test = -1;
			else if ((*host->player)->y >= 0 && (*host->player)->y <= by) test = -1;
			else test = 0;
		} else {
			if ((*host->player)->y >= ty && (*host->player)->y <= by) test = -1;
			else test = 0;
		}
		if (test == -1) {
			reg = i;
			break;
		}
	}

	if (reg != -1) {
		(*host->screen_mx) = lx;
		(*host->screen_my) = by - (*host->screen_h);
	}
}

#ifdef WIN32
__declspec(dllexport) dllfunctions *DLLInitialize(hostfunctions *thost) {
#else
dllfunctions *DLLInitialize(hostfunctions *thost) {
#endif
	int problem = 0;

	// Checks if the passed function was received:
	#define checksanity(func, funcname) \
		if (!host->func) { \
			fprintf(stderr, "bmgame:  %s was not received from the host!\n", funcname); \
			problem = -1;\
		}

	// Grab the host functions structure:
	host = thost;
	if (host == NULL) {
		fprintf(stderr, "hwgame:  Did not receive host functions thru GetGameAPI!\n");
		return NULL;
	};

	// Sanity check!
	problem = 0;
	// Are we connected to the engine (devmode == 0) or the editor (devmode == 1)?
	if (host->devmode == 1) {
		// Connected to the map editor:
		// All we need are drawing functions:
		checksanity(put_sprite, "put_sprite()");
		checksanity(put_sprite_hflip, "put_sprite_hflip()");
		checksanity(put_sprite_vflip, "put_sprite_vflip()");
		checksanity(put_sprite_hvflip, "put_sprite_hvflip()");
	} else {
		// Connected to the game engine:
		checksanity(put_sprite, "put_sprite()");
		checksanity(put_sprite_hflip, "put_sprite_hflip()");
		checksanity(put_sprite_vflip, "put_sprite_vflip()");
		checksanity(put_sprite_hvflip, "put_sprite_hvflip()");
		checksanity(put_bgtile, "put_bgtile()");

		checksanity(Draw2x2BGTile, "Draw2x2BGTile()");

		checksanity(e_spawn, "e_spawn()");
		checksanity(e_spawnat, "e_spawnat()");
		checksanity(e_spawnatmap, "e_spawnatmap()");
		checksanity(e_findfirst, "e_findfirst()");
		checksanity(e_findnext, "e_findnext()");
		checksanity(e_kill, "e_kill()");
		checksanity(e_damage, "e_damage()");
		checksanity(control_switch, "control_switch()");
		checksanity(gettileat, "gettileat()");
		checksanity(settileat, "settileat()");

		checksanity(wrap_map_coord_x, "wrap_map_coord_x()");
		checksanity(wrap_map_coord_y, "wrap_map_coord_y()");

		checksanity(LoadLevel, "LoadLevel()");

		checksanity(sndPrecache, "sndPrecache()");
		checksanity(sndFree, "sndFree()");
		checksanity(sndPlay, "sndPlay()");
		checksanity(sndSetVolume, "sndSetVolume()");
		checksanity(sndIsPlaying, "sndIsPlaying()");
		checksanity(sndStop, "sndStop()");

		// Pointers to the original variables:
		checksanity(map, "map structure");

		// Entities:
		checksanity(player, "player variable");
		checksanity(world, "world variable");

		// Game state variables:
		checksanity(screen_mx, "screen_mx variable");
		checksanity(screen_my, "screen_my variable");
		checksanity(old_screen_mx, "old_screen_mx variable");
		checksanity(old_screen_my, "old_screen_my variable");
		checksanity(screen_w, "screen_w variable");
		checksanity(screen_h, "screen_h variable");
		checksanity(scroll_left, "scroll_left variable");
		checksanity(scroll_right, "scroll_right variable");
		checksanity(scroll_top, "scroll_top variable");
		checksanity(scroll_bottom, "scroll_bottom variable");
		checksanity(quit, "quit variable");
		checksanity(game_paused, "game_paused variable");
	}

	// Return NULL if any of the above sanity checks failed:
	if (problem == -1) return NULL;

	// Return our copy:
	game = (dllfunctions *)calloc(sizeof(dllfunctions), 1);
    game->init = init;

	return game;
};
