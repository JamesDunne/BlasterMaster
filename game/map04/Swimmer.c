// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

int Swimmer_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	return -1;
}

// Test if we're in water and not running into solid things:
int Swimmer_Maptest(int m) {
	if (m & MAPFLAG_SOLID) return -1;
	if (!(m & MAPFLAG_WATER)) return -1;
	return 0;
}

void Swimmer_PreThink(entity e) {
	DEFINE_SELF(e_swimmer);

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};
	
	// Alternate leg frames so it appears to be swimming:
	e->fr_time++;
	if (e->fr_time >= 8) {
		e->fr_time = 0;
		e->frame = (e->frame == 0 ? 1 : 0);
	}
	
	if (self->flags & FLAG_ONGROUND) self->flags &= ~FLAG_ONGROUND;
	
	// Have him swim towards the player: (slow update frequency.)
	if (e->fr_time == 3) {
		fixed	dx, dy, len;
		
		// Determine vector to player position:
		dx = ((*host->player)->x + (*host->player)->ecrx1) - e->x;
		dy = ((*host->player)->y + (*host->player)->ecry1) - 16 - e->y;
	
		// If the difference is more than half the map, then move the other direction:
		if (dx <= -(host->map->width << 4)) dx += (host->map->width << 5);
		else if (dx >= (host->map->width << 4)) dx -= (host->map->width << 5);
		if (dy <= -(host->map->height << 4)) dy += (host->map->height << 5);
		else if (dy >= (host->map->height << 4)) dy -= (host->map->height << 5);
		
		// Normalize vector and scale:
		len = e->accel / sqrt(dx * dx + dy * dy);
		// Apply direction in a progressive manner: (coefficients must add to 1)
		e->dx = (e->dx * 0.5 + (dx * len) * 0.5);
		e->dy = (e->dy * 0.5 + (dy * len) * 0.5);
		
		if (e->dx > 0) self->face_dir = 2;
		else if (e->dx < 0) self->face_dir = 6;
	}

	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
}

void Swimmer_Death(entity e) {
	DEFINE_SELF(e_swimmer);
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

void Swimmer_Touched(entity e, entity o) {
	DEFINE_SELF(e_swimmer);

	if ((o->class == CLASS_BULLET) && (o->team != e->team)) return;
	
	if (o->team != e->team) {
		host->e_damage(o, e, 32);
	}
}

void Swimmer_Draw(entity e) {
	DEFINE_SELF(e_swimmer);
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

	// Draw the Swimmer:
	if (self->face_dir == 6) {
		switch (self->frame) {
			case 0:
				host->put_sprite(nx     , ny + 16, 1, 0x6C);
				host->put_sprite(nx + 16, ny + 16, 1, 0x6D);
				break;
			case 1:
				host->put_sprite(nx     , ny + 16, 1, 0x6C);
				host->put_sprite(nx + 16, ny + 16, 1, 0x5D);
				break;
		}
	} else if (self->face_dir == 2) {
			switch (self->frame) {
			case 0:
 				host->put_sprite_hflip(nx     , ny + 16, 1, 0x6D);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0x6C);
				break;
			case 1:
				host->put_sprite_hflip(nx     , ny + 16, 1, 0x5D);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0x6C);
				break;
		}
	}
	
	if (self->flags & FLAG_FLICKER) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	
};

void Swimmer_Preview(int nx, int ny, class_type class) {
	host->put_sprite(nx    , ny + 8, 1, 0x6C);
	host->put_sprite(nx + 8, ny + 8, 1, 0x6D);
}

void Swimmer_Init(entity e) {
	DEFINE_SELF(e_swimmer);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 2;
	self->accel = 2;
	self->frame = 0;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_TAKEDAMAGE;
	self->flags |= FLAG_ONSCREEN_ONLY;

	self->health = 32;
	self->maxhealth = 32;

	// Collision rectangle:
	self->ecrx1 =  0;	self->ecry1 =  16;
	self->ecrx2 = 31;	self->ecry2 =  31;

	self->mcrx1 =  0;	self->mcry1 =  16;
	self->mcrx2 = 31;	self->mcry2 =  31;

	self->dmg_time = 20;
	
	self->face_dir = 6;
}

class_properties_t	ClassProperties_Swimmer = {
	size:			sizeof(e_swimmer),
	name:			"Swimmer",

	init:			Swimmer_Init,
	prethink:		Swimmer_PreThink,
	postthink:		NULL,
	touched:		Swimmer_Touched,
	draw:			Swimmer_Draw,
	preview:		Swimmer_Preview,
	death:			Swimmer_Death,
	maptest:		Swimmer_Maptest,
	attacked:		Swimmer_Attacked
};

