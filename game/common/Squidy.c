// C Source File
// Created 1/6/2004; 6:16:38 AM

#include <math.h>
#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

int Squidy_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	return -1;
}

// Test if we're in water and not running into solid things:
int Squidy_Maptest(int m) {
	if (m & MAPFLAG_SOLID) return -1;
	if (!(m & MAPFLAG_WATER)) return -1;
	return 0;
}

void Squidy_Death(entity e) {
	e_powerup *p;
	// If the odds are 1/4, spawn a powerup:
	if (rand() % 32768 > 16384) {
		p = (e_powerup *) host->e_spawn(CLASS_POWERUP_POWER);
		p->x = e->x;
		p->y = e->y;
	}

	host->sndPlay(sounds.generic.explode1, SOUNDCHANNEL_PAIN, 0);
	BecomeExplosion(e, BULLET_LARGE);
}

void Squidy_PreThink(entity e) {
	DEFINE_SELF(e_squidy);
	double	len, vx, vy;

	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	if (self->fr_time++ > 7) {
		self->frame = (self->frame + 1) % 2;
		// Limit frame range:
		self->fr_time = 0;
	}

	if ((*(host->player))->flags & FLAG_DEAD) return;

	// Now, home in on the player:
	vx = (*host->player)->x - self->x;
	vy = (*host->player)->y - 16 - self->y;

	// If the difference is more than half the map, then move the other direction:
	if (vx <= -(host->map->width << 4)) vx += (host->map->width << 5);
	else if (vx >= (host->map->width << 4)) vx -= (host->map->width << 5);
	if (vy <= -(host->map->height << 4)) vy += (host->map->height << 5);
	else if (vy >= (host->map->height << 4)) vy -= (host->map->height << 5);

	len = sqrt((vx * vx) + (vy * vy));

	if (self->flags & FLAG_ONGROUND) self->flags &= ~FLAG_ONGROUND;

	if (len > 128.0) {
		self->dx = vx / len * 0.5;
		if (self->squiggle_time++ > 96) {
			self->dy = -1.75;
			self->fr_time++;
		} else {
			self->dy = 1.0;
		}
		if (self->squiggle_time > 160) {
			self->squiggle_time = 0;
		}
	} else {
		self->dx = vx / len * 2.5;
		self->dy = vy / len * 2.5;
	}

	if (self->dx > 0) self->face_dir = 2;
	if (self->dx < 0) self->face_dir = 6;

}

/*
void Squidy_PostThink(entity e) {
	DEFINE_SELF(e_squidy);
}
*/

void Squidy_Touched(entity e, entity o) {
	if (o == (*host->world)) return;
	
	if ((o->class == CLASS_BULLET) && (o->team != e->team)) {
		host->sndPlay(sounds.generic.damage, SOUNDCHANNEL_PAIN, 0);
		return;
	}

	if (o->team != e->team)
		host->e_damage(o, e, 32);
}

void Squidy_Draw(entity e) {
	DEFINE_SELF(e_squidy);

	// all these fgsprites indicies were found using my 'NES Sprite Viewer'
	// program which displays NES CHR ROM in 16x16 sprite pages.

	long	nx, ny;

	// Flicker on and off 4 frames
	if (self->flags & FLAG_FLICKER) self->flicker_count++;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count >= 8)) self->flicker_count -= 8;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count % 8 < 4)) return;

	// Account for the 16.15 fixed format.
	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Draw the squidy:
	switch (self->frame) {
		case 0:
			host->put_sprite(nx	 , ny	 , 1, 0x4E);
			host->put_sprite(nx + 16, ny	 , 1, 0x4F);
			host->put_sprite(nx	 , ny + 16, 1, 0x5E);
			host->put_sprite(nx + 16, ny + 16, 1, 0x5F);
			break;
		case 1:
			host->put_sprite(nx	 , ny	 , 1, 0x6E);
			host->put_sprite(nx + 16, ny	 , 1, 0x6F);
			host->put_sprite(nx	 , ny + 16, 1, 0x7E);
			host->put_sprite(nx + 16, ny + 16, 1, 0x7F);
			break;
	}

	if (self->flags & FLAG_FLICKER) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}

};

void Squidy_Preview(int x, int y, class_type class) {
	// Draw the squidy:
	host->put_sprite(x	, y	, 1, 0x4E);
	host->put_sprite(x + 8, y	, 1, 0x4F);
	host->put_sprite(x	, y + 8, 1, 0x5E);
	host->put_sprite(x + 8, y + 8, 1, 0x5F);
};

void Squidy_Init(entity e) {
	DEFINE_SELF(e_squidy);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0; self->dy = 0;
	self->max_dx = 2.5;
	self->accel = 0.25;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_TAKEDAMAGE;
	self->flags |= FLAG_ONSCREEN_ONLY;

	// Collision rectangle:
	self->ecrx1 =  0;	self->ecry1 =  0;
	self->ecrx2 = 31;	self->ecry2 = 31;

	self->mcrx1 =  0;	self->mcry1 =  0;
	self->mcrx2 = 31;	self->mcry2 = 31;

	self->health = 32;
	self->maxhealth = 32;
	self->dmg_time = 20;

	self->squiggle_time = 0;

	// Face right:
	self->face_dir = 2;
};

class_properties_t	ClassProperties_Squidy = {
	name:			"Squidy",
	size:			sizeof(e_squidy),
	init:			Squidy_Init,
	prethink:		Squidy_PreThink,
	touched:		Squidy_Touched,
	draw:			Squidy_Draw,
	preview:		Squidy_Preview,
	death:			Squidy_Death,
	maptest:		Squidy_Maptest,
	attacked:		Squidy_Attacked
};

