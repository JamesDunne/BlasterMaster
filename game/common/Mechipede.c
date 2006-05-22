// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

int Mechipede_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	return -1;
}

void Mechipede_PreThink(entity e) {
	DEFINE_SELF(e_mechipede);

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	if (self->fr_time++ >= 7) {
		self->fr_time = 0;
		if (self->frame++ >= 3) self->frame = 0;

		if (self->x > (*host->player)->x + (*host->player)->ecrx2) {
			if (!(self->flags & FLAG_JUMPING)) {
				self->dx = -self->accel; self->face_dir = 2;
			}
		} else if (self->x < (*host->player)->x + (*host->player)->ecrx1) {
			if (!(self->flags & FLAG_JUMPING)) {
				self->dx = self->accel; self->face_dir = 6;
			}
		} else {
			// Right on top of him
			if (!(self->flags & FLAG_JUMPING) && (self->flags & FLAG_ONGROUND)) {
				self->dy -= 6;
				// Don't move left or right, to annoy him more ;)
				self->dx = 0;
				self->flags |= FLAG_JUMPING;
				self->flags &= ~(FLAG_ONGROUND);
			}
		}
	}
	
	// Disable jumping if falling!
	if ((self->flags & FLAG_JUMPING) && (self->dy >= 0)) {
		self->flags &= ~(FLAG_JUMPING);
	}
	
	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
}

void Mechipede_Death(entity e) {
	DEFINE_SELF(e_mechipede);
	e_powerup *p;
	// If the odds are 1/4, spawn a powerup:
	if (rand() % 32768 > 16384) {
		p = (e_powerup *) host->e_spawn(CLASS_POWERUP_POWER);
		p->x = e->x;
		p->y = e->y;
	}

	host->sndPlay(sounds.generic.explode1, SOUNDCHANNEL_PAIN, 0);
	BecomeExplosion(e, BULLET_LARGE);
	return;
}

void Mechipede_Touched(entity e, entity o) {
	DEFINE_SELF(e_mechipede);

	// Touched the world?
	if (o == *(host->world)) {
		if ((e->collide_flags & COLLIDE_LEFT) || (e->collide_flags & COLLIDE_RIGHT)) {
			if (!(self->flags & FLAG_JUMPING) && (self->flags & FLAG_ONGROUND)) {
				self->flags |= FLAG_JUMPING;
				self->dy -= 6;
				// Try to jump over whatever is blocking us:
				if (self->flags & COLLIDE_LEFT) self->dx = -self->accel;
				else if (self->flags & COLLIDE_RIGHT) self->dx = self->accel;
			}
		}
	}
	
	if ((o->class == CLASS_BULLET) && (o->team != e->team)) return;
	
	if (o->team != e->team) {
		host->e_damage(o, e, 16);
	}
}

void Mechipede_Draw(entity e) {
	DEFINE_SELF(e_mechipede);
	float	r, g, b;
	long	nx, ny;

	// Flicker random colors:
	if (self->flags & FLAG_FLICKER) {
		r = (rand() % 32768) / 32768.0;
		g = (rand() % 32768) / 32768.0;
		b = (rand() % 32768) / 32768.0;

		glColor4f(r, g, b, 1.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	// Account for the 16.15 fixed format.
	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Draw the Mechipede:
	if (self->face_dir == 2) {
		switch (self->frame) {
			case 0:
				host->put_sprite(nx     , ny + 16, 1, 0x0C);
				host->put_sprite(nx + 16, ny + 16, 1, 0x0D);
				break;
			case 1:
			case 3:
				host->put_sprite(nx     , ny + 16, 1, 0x1C);
				host->put_sprite(nx + 16, ny + 16, 1, 0x1D);
				break;
			case 2:
				host->put_sprite(nx     , ny + 16, 1, 0x2C);
				host->put_sprite(nx + 16, ny + 16, 1, 0x2D);
				break;
		}
	} else if (self->face_dir == 6) {
		switch (self->frame) {
			case 0:
				host->put_sprite_hflip(nx     , ny + 16, 1, 0x0D);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0x0C);
				break;
			case 1:
			case 3:
				host->put_sprite_hflip(nx     , ny + 16, 1, 0x1D);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0x1C);
				break;
			case 2:
				host->put_sprite_hflip(nx     , ny + 16, 1, 0x2D);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0x2C);
				break;
		}
	}
	
	if (self->flags & FLAG_FLICKER) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	
};

void Mechipede_Preview(int nx, int ny, class_type class) {
	host->put_sprite(nx    , ny + 8, 1, 0x0C);
	host->put_sprite(nx + 8, ny + 8, 1, 0x0D);
}

void Mechipede_Init(entity e) {
	DEFINE_SELF(e_mechipede);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 4;
	self->accel = 2;
	self->frame = 0;
	self->flags = FLAG_NOFRICTION | FLAG_TAKEDAMAGE;
	self->flags |= FLAG_ONSCREEN_ONLY;

	self->health = 48;
	self->maxhealth = 48;

	// Collision rectangle:
	self->ecrx1 =  0;	self->ecry1 =  16;
	self->ecrx2 = 31;	self->ecry2 =  31;

	self->mcrx1 =  0;	self->mcry1 =  16;
	self->mcrx2 = 31;	self->mcry2 =  31;

	self->dmg_time = 20;
	
	self->face_dir = 2;
	self->dx = -self->accel;
}

class_properties_t	ClassProperties_Mechipede = {
	size:			sizeof(e_mechipede),
	name:			"Mechipede",

	init:			Mechipede_Init,
	prethink:		Mechipede_PreThink,
	postthink:		NULL,
	touched:		Mechipede_Touched,
	draw:			Mechipede_Draw,
	preview:		Mechipede_Preview,
	death:			Mechipede_Death,
	maptest:		NULL,
	attacked:		Mechipede_Attacked
};

