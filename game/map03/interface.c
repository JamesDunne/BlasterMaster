#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "sounds.h"

hostfunctions		*host = NULL;
dllfunctions		*game = NULL, *common = NULL;
controlled_entity	*player = NULL;
sounds_t			sounds;

void				*common_library = NULL;

// Initialization function.  Called when the engine is ready:
void init() {
	// First, load textures for map #0x01:
	host->LoadTexture(0, "textures/fgtile000.png");
	host->LoadTexture(1, "textures/fgtile001.png");
	host->LoadTexture(2, "textures/fgtile002.png");
	host->LoadTexture(3, "textures/fgtile003.png");

	if (host->devmode != 0) return;

	// Precache our sounds:
	sounds.tank.cannon = host->sndPrecache("sound/tank/cannon.ogg");
	sounds.tank.explode = host->sndPrecache("sound/tank/explode.ogg");
	sounds.tank.hover = host->sndPrecache("sound/tank/hover.ogg");
	sounds.tank.hyper = host->sndPrecache("sound/tank/hyper.ogg");
	sounds.tank.jump = host->sndPrecache("sound/tank/jump.ogg");
	sounds.tank.land = host->sndPrecache("sound/tank/land.ogg");
	sounds.tank.warp = host->sndPrecache("sound/tank/warp.ogg");

	sounds.jason.enter_exit = host->sndPrecache("sound/jason/enter_exit.ogg");
	sounds.jason.shoot = host->sndPrecache("sound/jason/shoot.ogg");

	sounds.mine.beep = host->sndPrecache("sound/mine/beep.ogg");

	sounds.powerup.flashy = host->sndPrecache("sound/powerup/flashy.ogg");
	sounds.powerup.regular = -1;
	sounds.powerup.weapon = host->sndPrecache("sound/powerup/weapon.ogg");

	sounds.generic.damage = host->sndPrecache("sound/generic/damage.ogg");
	sounds.generic.drop = host->sndPrecache("sound/generic/drop.ogg");
	sounds.generic.explode1 = host->sndPrecache("sound/generic/explode1.ogg");
	sounds.generic.explode2 = host->sndPrecache("sound/generic/explode2.ogg");
	sounds.generic.explode3 = host->sndPrecache("sound/generic/explode3.ogg");
	sounds.generic.monster_hit = host->sndPrecache("sound/generic/monster_hit.ogg");

	sounds.ring.flyby = host->sndPrecache("sound/ring/flyby.ogg");

	sounds.ui.menu = host->sndPrecache("sound/ui/menu.ogg");

	host->sndSetVolume(SOUNDCHANNEL_PLAYER, SDL_MIX_MAXVOLUME / 2);
	host->sndSetVolume(SOUNDCHANNEL_PAIN, SDL_MIX_MAXVOLUME / 2);
	host->sndSetVolume(SOUNDCHANNEL_MONSTERS, SDL_MIX_MAXVOLUME / 2);
	
	// Now, Initialize the common DLL:
	common->init();
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

		player = host->player;
	}

	// Return NULL if any of the above sanity checks failed:
	if (problem == -1) return NULL;

	// First, load the common DLL for common entities, so that we don't re-code them:
	common = GetGameAPI ("game/common", host, &common_library);
	if (common == NULL) {
		fprintf(stderr, "Error loading common dll!\n");
		return NULL;
	}

	game = (dllfunctions *)calloc(sizeof(dllfunctions), 1);
	// Send our functions and variables back:
	game->class_properties = calloc(sizeof(class_properties_t) * NUM_CLASSES, 1);
	if (game->class_properties == NULL) {
		fprintf(stderr, "Could not allocate memory for class_properties!\n");
		return NULL;
	}
	game->spawnflags = malloc(sizeof(Uint32) * NUM_CLASSES);
	if (game->spawnflags == NULL) {
		fprintf(stderr, "Could not allocate memory for spawnflags!\n");
		return NULL;
	}
	memset(game->class_properties, 0, sizeof(class_properties_t) * NUM_CLASSES);
	memset(game->spawnflags, 0, sizeof(Uint32) * NUM_CLASSES);

	#define DEFINE_CLASS(class, props, flags) \
		game->class_properties[class] = &props; \
		game->spawnflags[class] = flags;

	// Copy all common entities over to our list that we will return:
	memcpy(game->class_properties, common->class_properties, sizeof(class_properties_t) * common->numclasses);
	memcpy(game->spawnflags, common->spawnflags, sizeof(Uint32) * common->numclasses);

///---Do not change this line!! It is used by scripts to add new entities!!---
	DEFINE_CLASS(CLASS_KEYHOLEL, ClassProperties_KeyholeL, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_KEYHOLER, ClassProperties_KeyholeR, SPAWNFLAG_MAPSPAWNABLE)

	game->numclasses = NUM_CLASSES;
	game->init = init;
	game->pre_render = common->pre_render;
	game->post_render = common->post_render;

	return game;
};

// In order to change to an explosion, set the class, and call init.
entity BecomeExplosion(entity e, bullet_type bullet) {
	// Create a new entity
	entity p = host->e_spawn(CLASS_EXPLOSION);
	e_explosion *self = (e_explosion *)p;

	if (p == NULL) { host->e_kill(e); return NULL; }

	self->x = e->x - 8;
	self->y = e->y - 8;

	self->kind = bullet;

	host->e_kill(e);

	return p;
};
