#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "sounds.h"

// Statically defined class properties:
class_properties_t	**class_properties;

hostfunctions		*host = NULL;
dllfunctions		*game = NULL;
controlled_entity	*player = NULL;
sounds_t			sounds;

void init() {
	int	i;
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
	host->sndSetVolume(SOUNDCHANNEL_HOVER, SDL_MIX_MAXVOLUME / 2);

	// Fix Jason to be outdoor Jason:
	for (i=0; i<=(*host->last_entity); ++i) if ((host->entities[i]) != NULL) {
		if ((host->entities[i])->class == CLASS_JASON) {
			Jason_Outdoor(host->entities[i]);
		}
	}
}

void pre_render() {
	fixed		x, y, tx, ty, m;
	fixed		xofs, yofs;

	xofs = fmod(*(host->screen_mx), 32.0);
	yofs = fmod(*(host->screen_my), 32.0);

	// Draw whole map before entities are rendered:
	for (y = 0; y <= (*(host->screen_h) / 32.0); ++y) {
		ty = (y * 32.0 + *(host->screen_my));
		for (x = 0; x <= (*(host->screen_w) / 32.0);++x) {
			tx = (x * 32.0 + *(host->screen_mx));
			m = host->gettileat(host->wrap_map_coord_x(tx), host->wrap_map_coord_y(ty));
			host->Draw2x2BGTile((x * 32.0) - xofs, (y * 32.0) - yofs, m, 0);
		}
	}
}

void post_render() {
	int i;
	// Draw the HUD:

	// Put the HOV meter in:
	if ((*player)->class == CLASS_TANK) {
		e_tank	*tank = (e_tank *)(*player);
		for (i = 3; i >= 0; --i)
			if (tank->hover >= (i << 6) + 63)
				host->put_sprite(32, 96 + ((3-i)<<4), 1, 0xB5);
			else if (tank->hover >= (i << 6) + 31)
				host->put_sprite(32, 96 + ((3-i)<<4), 1, 0xA5);
			else
				host->put_sprite(32, 96 + ((3-i)<<4), 1, 0x95);
		host->put_sprite(32, 160, 1, 0xC5);
		host->put_sprite(32, 176, 1, 0xD5);
	}

	// Put the POW meter in:
	for (i = 3; i >= 0; --i)
		if ((*player)->health >= (i << 6) + 63)
			host->put_sprite(32, 304 + ((3-i)<<4), 0, 0xB5);
		else if ((*player)->health >= (i << 6) + 31)
			host->put_sprite(32, 304 + ((3-i)<<4), 0, 0xA5);
		else
			host->put_sprite(32, 304 + ((3-i)<<4), 0, 0x95);
	host->put_sprite(32, 368, 0, 0xE5);
	host->put_sprite(32, 384, 0, 0xF5);
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
		checksanity(entities, "entities array");
		checksanity(last_entity, "last_entity variable");

		// Acceleration scalar value from joystick (0 <= x <= 1.0):
		checksanity(accel_scale, "acceleration scalar variable");

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

///---Do not change this line!! It is used by scripts to add new entities!!---
	DEFINE_CLASS(CLASS_MECHIPEDE, ClassProperties_Mechipede, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_RINGSPAWNER_RED, ClassProperties_RingSpawner_Red, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_RINGSPAWNER, ClassProperties_RingSpawner, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_FLYBOMBER, ClassProperties_FlyBomber, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_ROCKGUNNER, ClassProperties_RockGunner, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_ROCKCLIMBER_CW, ClassProperties_RockClimber, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_ROCKCLIMBER_CCW, ClassProperties_RockClimber, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_ROCKWALKER, ClassProperties_RockWalker, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_MINE, ClassProperties_Mine, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_TURRET, ClassProperties_Turret, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_SQUIDY, ClassProperties_Squidy, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_VFLYER, ClassProperties_VFlyer, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_ROCKCLIMBER_RED_CW, ClassProperties_RockClimber_Red, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_ROCKCLIMBER_RED_CCW, ClassProperties_RockClimber_Red, SPAWNFLAG_MAPSPAWNABLE)

	DEFINE_CLASS(CLASS_POWERUP_POWER, ClassProperties_PowerUp_Power, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_POWERUP_POWER_FLASH, ClassProperties_PowerUp_Power_Flash, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_POWERUP_GUNS, ClassProperties_PowerUp_Guns, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_POWERUP_GUNS_FLASH, ClassProperties_PowerUp_Guns_Flash, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_POWERUP_HOVER, ClassProperties_PowerUp_Hover, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_POWERUP_HOVER_FLASH, ClassProperties_PowerUp_Hover_Flash, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_POWERUP_HOMING_MISSILES, ClassProperties_PowerUp_Homing_Missiles, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_POWERUP_BOLTS, ClassProperties_PowerUp_Bolts, SPAWNFLAG_MAPSPAWNABLE)
	DEFINE_CLASS(CLASS_POWERUP_TOMAHAWKS, ClassProperties_PowerUp_Tomahawks, SPAWNFLAG_MAPSPAWNABLE)

	DEFINE_CLASS(CLASS_VDOOR, ClassProperties_VDoor, SPAWNFLAG_MAPSPAWNABLE)

	DEFINE_CLASS(CLASS_TANK, ClassProperties_Tank, SPAWNFLAG_MAPSPAWNABLE | SPAWNFLAG_PLAYER)
	DEFINE_CLASS(CLASS_JASON, ClassProperties_Jason, SPAWNFLAG_PLAYER)
	DEFINE_CLASS(CLASS_EXPLOSION, ClassProperties_Explosion, 0)
	DEFINE_CLASS(CLASS_BULLET, ClassProperties_Bullet, 0)
	DEFINE_CLASS(CLASS_WORLD, ClassProperties_World, 0)

	// Not map-spawnable:
	DEFINE_CLASS(CLASS_RINGFLYER_RED, ClassProperties_RingFlyer, 0)
	DEFINE_CLASS(CLASS_RINGFLYER, ClassProperties_RingFlyer, 0)

	game->numclasses = NUM_CLASSES;
	game->init = init;
	game->pre_render = pre_render;
	game->post_render = post_render;

	return game;
};
