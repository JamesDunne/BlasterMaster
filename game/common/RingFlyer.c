// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

#define PI 3.14159265358979323846

int RingFlyer_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	return -1;
}

void RingFlyer_PreThink(entity e) {
	DEFINE_SELF(e_ringflyer);

	self->osc_time += PI / 32;
	if (self->osc_time >= 2 * PI) self->osc_time -= 2 * PI;

	self->dy = cos(self->osc_time) * 4.0;

	if ( (self->x > ((*host->screen_mx) + (*host->screen_w)) - 16) || 
	     (self->x < ((*host->screen_mx) + 16)) || 
		 (self->y > ((*host->screen_my) + (*host->screen_h)) - 16) || 
		 (self->y < ((*host->screen_my) + 16)) )
	{
		host->e_kill(e);
		return;
	}

	// Rotate:
	if (++self->fr_time >= 8) {
		self->fr_time = 0;
		if (++self->frame >= 4) self->frame = 0;
	}

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	}

	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
}

void RingFlyer_Death(entity e) {
	DEFINE_SELF(e_ringflyer);
	e_powerup *p;
	
	// If the odds are 1/4, spawn a powerup:
	if (rand() % 32768 > 16384) {
		// RED rings give you POWER, grey rings give you HOVER.
		if (self->class == CLASS_RINGFLYER_RED) {
			p = (e_powerup *) host->e_spawn(CLASS_POWERUP_POWER);
		} else {
			p = (e_powerup *) host->e_spawn(CLASS_POWERUP_HOVER);
		}
		p->x = e->x;
		p->y = e->y;
	}

	host->sndPlay(sounds.generic.explode1, SOUNDCHANNEL_PAIN, 0);
	BecomeExplosion(e, BULLET_LARGE);
	return;
}

void RingFlyer_Touched(entity e, entity o) {
	DEFINE_SELF(e_ringflyer);

	if ((o->class == CLASS_BULLET) && (o->team != e->team)) return;
	
	if (o->team != e->team) {
		host->e_damage(o, e, 32);
	}
}

void RingFlyer_Draw(entity e) {
	DEFINE_SELF(e_ringflyer);
	long	nx, ny;
	int		ringcolor = 1;

	// Flicker on and off 4 frames
	if (self->flags & FLAG_FLICKER) self->flicker_count++;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count >= 8)) self->flicker_count -= 8;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count % 8 < 4)) return;

	// Account for the 16.15 fixed format.
	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	if (self->class == CLASS_RINGFLYER_RED) ringcolor = 0;
	
	// Draw the RingFlyer:
	if (self->face_dir == 6) {
		switch (self->frame) {
			case 0:
				host->put_sprite      (nx     , ny     , ringcolor, 0x6C);
				host->put_sprite      (nx + 16, ny     , ringcolor, 0x6D);
				host->put_sprite_vflip(nx     , ny + 16, ringcolor, 0x6C);
				host->put_sprite_vflip(nx + 16, ny + 16, ringcolor, 0x6D);
				break;
			case 1:
			case 3:
				host->put_sprite(nx     , ny     , ringcolor, 0x7C);
				host->put_sprite(nx + 16, ny     , ringcolor, 0x7D);
				host->put_sprite(nx     , ny + 16, ringcolor, 0x8C);
				host->put_sprite(nx + 16, ny + 16, ringcolor, 0x8D);
				break;
			case 2:
				host->put_sprite(nx     , ny +  8, ringcolor, 0x8E);
				host->put_sprite(nx + 16, ny +  8, ringcolor, 0x8F);
				break;
		}
	} else if (self->face_dir == 2) {
		switch (self->frame) {
			case 0:
				host->put_sprite_hflip (nx     , ny     , ringcolor, 0x6D);
				host->put_sprite_hflip (nx + 16, ny     , ringcolor, 0x6C);
				host->put_sprite_hvflip(nx     , ny + 16, ringcolor, 0x6D);
				host->put_sprite_hvflip(nx + 16, ny + 16, ringcolor, 0x6C);
				break;
			case 1:
			case 3:
				host->put_sprite_hflip(nx     , ny     , ringcolor, 0x7D);
				host->put_sprite_hflip(nx + 16, ny     , ringcolor, 0x7C);
				host->put_sprite_hflip(nx     , ny + 16, ringcolor, 0x8D);
				host->put_sprite_hflip(nx + 16, ny + 16, ringcolor, 0x8C);
				break;
			case 2:
				host->put_sprite_hflip(nx     , ny +  8, ringcolor, 0x8F);
				host->put_sprite_hflip(nx + 16, ny +  8, ringcolor, 0x8E);
				break;
		}
	}
};

void RingFlyer_Preview(int nx, int ny, class_type class) {
	int		ringcolor = 1;
	if (class == CLASS_RINGFLYER_RED) ringcolor = 0;
	
	host->put_sprite      (nx    , ny    , ringcolor, 0x6C);
	host->put_sprite      (nx + 8, ny    , ringcolor, 0x6D);
	host->put_sprite_vflip(nx    , ny + 8, ringcolor, 0x6C);
	host->put_sprite_vflip(nx + 8, ny + 8, ringcolor, 0x6D);
}

void RingFlyer_Init(entity e) {
	DEFINE_SELF(e_ringflyer);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 4;
	self->accel = 0;
	self->frame = 0;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_NOMAPCOLLISION | FLAG_TAKEDAMAGE;
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

class_properties_t	ClassProperties_RingFlyer = {
	size:			sizeof(e_ringflyer),
	name:			"RingFlyer",

	init:			RingFlyer_Init,
	prethink:		RingFlyer_PreThink,
	postthink:		NULL,
	touched:		RingFlyer_Touched,
	draw:			RingFlyer_Draw,
	preview:		RingFlyer_Preview,
	death:			RingFlyer_Death,
	maptest:		NULL,
	attacked:		RingFlyer_Attacked
};

