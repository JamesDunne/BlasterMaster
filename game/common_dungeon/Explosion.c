// C Source File
// Created 1/6/2004; 3:29:40 PM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"

class_properties_t	ClassProperties_Explosion = {
	sizeof(e_explosion),
	"Explosion",
	Explosion_Init,
	Explosion_PreThink,
	NULL,
	NULL,
	Explosion_Draw,
	NULL
};

void Explosion_PreThink(entity e) {
	DEFINE_SELF(e_explosion);
	self->fr_time++;
	if (self->fr_time >= 4) {
		self->frame++;
		self->fr_time = 0;
	}
}

/*
void Explosion_PostThink(entity e) {
	DEFINE_SELF(e_explosion);

}
*/
/*
void Explosion_Touched(entity e, entity o) {
//	DEFINE_SELF(e_explosion);
}
*/

void Explosion_Draw(entity e) {
	DEFINE_SELF(e_explosion);

	// all these fgsprites indicies were found using my 'NES Sprite Viewer'
	// program which displays NES CHR ROM in 16x16 sprite pages.

	long	nx, ny;

	// Account for the 16.15 fixed format.
	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Draw the Explosion:
	switch (self->frame) {
		case 0: case 2:
			host->put_sprite(nx	    , ny     , 5, 0xE0);
			host->put_sprite(nx	+ 16, ny     , 5, 0xE1);
			host->put_sprite(nx     , ny + 16, 5, 0xF0);
			host->put_sprite(nx + 16, ny + 16, 5, 0xF1);
			break;
		case 1: case 3:
			break;
		case 4:
			host->put_sprite(nx     , ny     , 5, 0xE2);
			host->put_sprite(nx + 16, ny     , 5, 0xE3);
			host->put_sprite(nx     , ny + 16, 5, 0xF2);
			host->put_sprite(nx + 16, ny + 16, 5, 0xF3);
			break;
		case 5:
			host->e_kill(e);
			return;
	}
};

void Explosion_Init(entity e) {
	DEFINE_SELF(e_explosion);

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 0L;
	self->accel = 0L;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY;

	self->health = 0;
	self->maxhealth = 0;

	// Collision rectangle:
	self->ecrx1 =  1;	self->ecry1 =  1;
	self->ecrx2 = -1;	self->ecry2 = -1;

	self->mcrx1 =  1;	self->mcry1 =  1;
	self->mcrx2 = -1;	self->mcry2 = -1;
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
