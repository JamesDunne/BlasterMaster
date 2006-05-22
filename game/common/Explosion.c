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
	switch (self->kind) {
		case BULLET_SMALL:
		case BULLET_MEDIUM:
		case BULLET_MEDIUM_RED:
		case BULLET_CANNON:
		case BULLET_CRUSHER:
		case BULLET_HYPER:
		case BULLET_MISSILE:
		case BULLET_HOMING_MISSILE:
			// Small explosions:
			switch (self->frame) {
				case 0: case 2:
					host->put_sprite(nx	 , ny	 , 1, 0x53);
					host->put_sprite(nx	 , ny + 16, 1, 0x63);
					host->put_sprite(nx + 16, ny	 , 1, 0x73);
					host->put_sprite(nx + 16, ny + 16, 1, 0x83);
					break;
				case 1: case 3:
					break;
				case 4:
					host->put_sprite(nx	 , ny	 , 1, 0xC2);
					host->put_sprite(nx	 , ny + 16, 1, 0xD2);
					host->put_sprite(nx + 16, ny	 , 1, 0xE2);
					host->put_sprite(nx + 16, ny + 16, 1, 0xF2);
					break;
				case 5:
					host->put_sprite(nx + 8, ny + 8, 1, 0xF3);
					break;
				case 6:
					host->e_kill(e);
					return;
					break;
			}
			break;

		case BULLET_LARGE:
			// Big explosions:
			switch (self->frame) {
				case 0:
					host->put_sprite(nx	 , ny	 , 1, 0x53);
					host->put_sprite(nx	 , ny + 16, 1, 0x63);
					host->put_sprite(nx + 16, ny	 , 1, 0x73);
					host->put_sprite(nx + 16, ny + 16, 1, 0x83);
					break;
				case 1:
					host->put_sprite(nx -  8, ny -  8, 1, 0x93);
					host->put_sprite(nx -  8, ny +  8, 1, 0xA3);
					host->put_sprite(nx -  8, ny + 24, 1, 0xB3);
					host->put_sprite(nx +  8, ny -  8, 1, 0xC3);
					host->put_sprite(nx +  8, ny +  8, 1, 0xD3);
					host->put_sprite(nx +  8, ny + 24, 1, 0xE3);
					host->put_sprite(nx + 24, ny -  8, 1, 0x54);
					host->put_sprite(nx + 24, ny +  8, 1, 0x64);
					host->put_sprite(nx + 24, ny + 24, 1, 0x74);
					break;
				case 2:
					host->put_sprite(nx	 , ny	 , 1, 0xC2);
					host->put_sprite(nx	 , ny + 16, 1, 0xD2);
					host->put_sprite(nx + 16, ny	 , 1, 0xE2);
					host->put_sprite(nx + 16, ny + 16, 1, 0xF2);
					break;
				case 3:
					host->put_sprite(nx + 8, ny + 8, 1, 0xF3);
					break;
				case 4:
					host->e_kill(e);
					return;
					break;
			}
			break;
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
