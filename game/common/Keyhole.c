// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"

void Keyhole_Touched(entity e, entity o) {
	DEFINE_SELF(e_keyhole);
	controlled_entity	p;

	// Is it a controlled entity?
	if (o->flags & FLAG_CONTROLLED) {
		p = (controlled_entity) o;
		if (p->items & ITEM_KEY) {
			// Will change into a three-stone vertical door:
			self->class = CLASS_VDOOR;
			VDoor_Init(e);
		}
	}
}

void Keyhole_Draw(entity e) {
	DEFINE_SELF(e_keyhole);
	long	nx, ny;

	if (e->class == CLASS_KEYHOLEL) nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx)) - 32;
	if (e->class == CLASS_KEYHOLER) nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx)) + 32;
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Draw the Keyhole:
	if (self->locked == 1) {
		host->put_sprite(nx	 , ny	 , 2, 0xE8);
		host->put_sprite(nx + 16, ny	 , 2, 0xE9);
		host->put_sprite(nx	 , ny + 16, 2, 0xF8);
		host->put_sprite(nx + 16, ny + 16, 2, 0xF9);
	}
};

void Keyhole_Preview(int nx, int ny, class_type class) {
	if (class == CLASS_KEYHOLEL) nx -= 16;
	if (class == CLASS_KEYHOLER) nx += 16;
	host->put_sprite(nx	, ny	, 2, 0xE8);
	host->put_sprite(nx + 8, ny	, 2, 0xE9);
	host->put_sprite(nx	, ny + 8, 2, 0xF8);
	host->put_sprite(nx + 8, ny + 8, 2, 0xF9);
}

void Keyhole_Init(entity e) {
	DEFINE_SELF(e_keyhole);

	self->team = TEAM_FROGHUNTERS;

	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_NOMAPCOLLISION;
	// Keyhole is locked by default:
	self->locked = 1;

	self->health = 16;
	self->maxhealth = 16;

	if (e->class == CLASS_KEYHOLEL) {
		// Collision rectangle:
		self->ecrx1 = -32;	self->ecry1 =   0;
		self->ecrx2 =  31;	self->ecry2 =  31;

		self->mcrx1 =   0;	self->mcry1 =   0;
		self->mcrx2 =  31;	self->mcry2 =  31;
	} else if (e->class == CLASS_KEYHOLER) {
		// Collision rectangle:
		self->ecrx1 =   0;	self->ecry1 =   0;
		self->ecrx2 =  63;	self->ecry2 =  31;

		self->mcrx1 =   0;	self->mcry1 =   0;
		self->mcrx2 =  31;	self->mcry2 =  31;
	}
};

// Same functions used for left and right-side keyholes:
class_properties_t	ClassProperties_KeyholeL = {
	sizeof(e_keyhole),
	"Keyhole (L)",
	Keyhole_Init,
	NULL,
	NULL,
	Keyhole_Touched,
	Keyhole_Draw,
	Keyhole_Preview,
	NULL
};

class_properties_t	ClassProperties_KeyholeR= {
	sizeof(e_keyhole),
	"Keyhole (R)",
	Keyhole_Init,
	NULL,
	NULL,
	Keyhole_Touched,
	Keyhole_Draw,
	Keyhole_Preview,
	NULL
};


