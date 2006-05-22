// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

void RingSpawner_PreThink(entity e) {
	DEFINE_SELF(e_ringspawner);

	// Randomly spawn RingFlyer's:
	if (rand() % 32768 <= 128) {
		e_ringflyer *ring;
		
		// Spawn a RingFlyer:
		if (self->class == CLASS_RINGSPAWNER_RED) {
			ring = (e_ringflyer *)host->e_spawn(CLASS_RINGFLYER_RED);
		} else {
			ring = (e_ringflyer *)host->e_spawn(CLASS_RINGFLYER);
		}
		
		if (ring) {
			// Play the fly-by sound for the ring:
			host->sndPlay(sounds.ring.flyby, SOUNDCHANNEL_AMBIENT, 0);
			ring->y = self->y - ((rand() % 9) - 4) * 16;
			if (rand() % 32768 >= 16384) {
				// Left side of screen facing right:
				ring->x = (*host->screen_mx) + 32;
				ring->dx = 8;
				ring->face_dir = 2;
			} else {
				// Right side of screen facing left:
				ring->x = (*host->screen_mx) + (*host->screen_w) - 32;
				ring->dx = -8;
				ring->face_dir = 6;
			}
		}
	}
}

void RingSpawner_Preview(int nx, int ny, class_type class) {
	int		ringcolor = 1;
	if (class == CLASS_RINGSPAWNER_RED) ringcolor = 0;
	
	host->put_sprite      (nx    , ny    , ringcolor, 0x6C);
	host->put_sprite      (nx + 8, ny    , ringcolor, 0x6D);
	host->put_sprite_vflip(nx    , ny + 8, ringcolor, 0x6C);
	host->put_sprite_vflip(nx + 8, ny + 8, ringcolor, 0x6D);
}

void RingSpawner_Init(entity e) {
	DEFINE_SELF(e_ringspawner);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 0;
	self->accel = 0;
	self->frame = 0;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_NOMAPCOLLISION;
	self->flags |= FLAG_ONSCREEN_ONLY;

	self->health = 16;
	self->maxhealth = 16;

	// Collision rectangle:
	self->ecrx1 =  0;	self->ecry1 =   0;
	self->ecrx2 = 31;	self->ecry2 =  31;

	self->mcrx1 =  0;	self->mcry1 =   0;
	self->mcrx2 = 31;	self->mcry2 =  31;

	self->dmg_time = 20;
	
	self->face_dir = 2;
}

class_properties_t	ClassProperties_RingSpawner = {
	size:			sizeof(e_ringspawner),
	name:			"RingSpawner",

	init:			RingSpawner_Init,
	prethink:		RingSpawner_PreThink,
	postthink:		NULL,
	touched:		NULL,
	draw:			NULL,
	preview:		RingSpawner_Preview,
	death:			NULL,
	maptest:		NULL,
	attacked:		NULL
};

class_properties_t	ClassProperties_RingSpawner_Red = {
	size:			sizeof(e_ringspawner),
	name:			"RingSpawner (RED)",

	init:			RingSpawner_Init,
	prethink:		RingSpawner_PreThink,
	postthink:		NULL,
	touched:		NULL,
	draw:			NULL,
	preview:		RingSpawner_Preview,
	death:			NULL,
	maptest:		NULL,
	attacked:		NULL
};

