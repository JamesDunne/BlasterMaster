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
		// Spawn a RingFlyer:
		e_ringflyer *ring = (e_ringflyer *)host->e_spawn(CLASS_RINGFLYER);
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
	preview:		NULL,
	death:			NULL,
	maptest:		NULL,
	attacked:		NULL
};

