// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"

void VDoor_PreThink(entity e) {
	DEFINE_SELF(e_vdoor);

	if (self->tile1 == -1) {
		// Store the tiles for later restoration:
		self->tile1 = host->gettileat(e->x, e->y);
		self->tile2 = host->gettileat(e->x, e->y - 32);
		self->tile3 = host->gettileat(e->x, e->y - 64);
	}

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;

	if (self->flags & FLAG_TURNING) {
		self->frame++;
		if (self->frame == 16) {
			// Store the tile for later restoration:
			host->settileat(e->x, e->y, 0);
		} else if (self->frame == 32) {
			// Store the tile for later restoration:
			host->settileat(e->x, e->y - 32, 0);
		} else if (self->frame == 48) {
			host->settileat(e->x, e->y - 64, 0);
		} else if (self->frame == 128) {
			host->settileat(e->x, e->y - 64, self->tile3);
		} else if (self->frame == 144) {
			host->settileat(e->x, e->y - 32, self->tile2);
		} else if (self->frame == 160) {
			host->settileat(e->x, e->y, self->tile1);
			self->flags &= ~FLAG_TURNING;
			self->frame = 161;
		}
	}
}

void VDoor_Death(entity e) {
	DEFINE_SELF(e_vdoor);

	BecomeExplosion(e, BULLET_LARGE);
	return;
}

void VDoor_Touched(entity e, entity o) {
	DEFINE_SELF(e_vdoor);

	// Objects on the other team (FROGHUNTERS) and not bullets.
	if ((o->team != e->team) && !(self->flags & FLAG_TURNING) && (o->class != CLASS_BULLET)) {
		self->flags |= FLAG_TURNING;
		self->frame = 0;
	}
}

void VDoor_Init(entity e) {
	DEFINE_SELF(e_vdoor);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 0L;
	self->accel = 0L;
	self->frame = 0L;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_NOMAPCOLLISION;

	self->health = 16;
	self->maxhealth = 16;

	// Collision rectangle: (spans 3 tiles horizontally)
	self->ecrx1 = -32;	self->ecry1 =   0;
	self->ecrx2 = 63;	self->ecry2 =  31;

	self->mcrx1 = -32;	self->mcry1 =   0;
	self->mcrx2 = 63;	self->mcry2 =  31;

	self->dmg_time = 20;

	self->face_dir = 2;

	self->tile1 = self->tile2 = self->tile3 = -1;
}

class_properties_t	ClassProperties_VDoor = {
	sizeof(e_vdoor),
	"Vertical Door",
	VDoor_Init,
	VDoor_PreThink,
	NULL,
	VDoor_Touched,
	NULL,
	NULL,
	VDoor_Death
};

