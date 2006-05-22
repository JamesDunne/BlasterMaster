// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

int FlyBomber_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	return -1;
}

void FlyBomber_DropBomb(entity e) {
	DEFINE_SELF(e_flybomber);

	// Have to wait until the mouth is closed to drop another bomb again:
	if (self->fr_time == 0) {

		e_bullet *bullet = (e_bullet *) host->e_spawn(CLASS_BULLET);
		if (bullet) {
			// Play the drop noise:
			host->sndPlay(sounds.generic.drop, SOUNDCHANNEL_AMBIENT, 0);
			bullet->owner = e;
			bullet->team = self->team;
			bullet->dx = e->dx;
			bullet->dy = 0;
			bullet->x = self->x + 4;
			bullet->y = self->y + 16;
			bullet->kind = BULLET_MEDIUM;
			bullet->flags &= ~(FLAG_NOGRAVITY);
			bullet->flags |= FLAG_BOUNCE | FLAG_NODESTROY;
			bullet->killtime = 140;
			bullet->damage = 32;
		}

		self->frame = 1;
		self->fr_time = 64;
	}
}

void FlyBomber_PreThink(entity e) {
	DEFINE_SELF(e_flybomber);

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	// If the frame counter is up, decrement it
	if (self->fr_time > 0) {
		self->fr_time--;
		// If we get down to 0, then set to initial frame (mouth closed):
		if (self->fr_time == 0) self->frame = 0;
	}
	
	// Is the player below me in proximity?
	if (((*host->player)->y >= self->y) && (abs((*host->player)->x - self->x) <= 8)) {
		// Drop a bomb :-D
		FlyBomber_DropBomb(e);
		// Fly upwards for some reason??
		self->dx = 0; self->dy = -4;
	}
	
	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
}

void FlyBomber_Death(entity e) {
	DEFINE_SELF(e_flybomber);
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

void FlyBomber_Touched(entity e, entity o) {
	DEFINE_SELF(e_flybomber);

	// Touched the world?
	if (o == (*host->world)) {
		// Turn around:
		if (self->face_dir == 6) {
			self->dx = self->max_dx;
			self->face_dir = 2;
		} else {
			self->dx = -self->max_dx;
			self->face_dir = 6;
		}
	}
	
	if ((o->class == CLASS_BULLET) && (o->team != e->team)) return;
	
	if (o->team != e->team) {
		host->e_damage(o, e, 32);
	}
}

void FlyBomber_Draw(entity e) {
	DEFINE_SELF(e_flybomber);
	long	nx, ny;

	// Flicker on and off 4 frames
	if (self->flags & FLAG_FLICKER) self->flicker_count++;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count >= 8)) self->flicker_count -= 8;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count % 8 < 4)) return;

	// Account for the 16.15 fixed format.
	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Draw the FlyBomber:
	if (self->face_dir == 6) {
		switch (self->frame) {
			case 0:
				host->put_sprite(nx     , ny     , 1, 0x4A);
				host->put_sprite(nx + 16, ny     , 1, 0x4B);
				host->put_sprite(nx     , ny + 16, 1, 0x5A);
				host->put_sprite(nx + 16, ny + 16, 1, 0x5B);
				break;
			case 1:
				host->put_sprite(nx     , ny     , 1, 0x4A);
				host->put_sprite(nx + 16, ny     , 1, 0x4B);
				host->put_sprite(nx     , ny + 16, 1, 0x6A);
				host->put_sprite(nx + 16, ny + 16, 1, 0x6B);
				host->put_sprite(nx     , ny + 32, 1, 0x7A);
				host->put_sprite(nx + 16, ny + 32, 1, 0x7B);
				break;
		}
	} else if (self->face_dir == 2) {
		switch (self->frame) {
			case 0:
				host->put_sprite_hflip(nx     , ny     , 1, 0x4B);
				host->put_sprite_hflip(nx + 16, ny     , 1, 0x4A);
				host->put_sprite_hflip(nx     , ny + 16, 1, 0x5B);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0x5A);
				break;
			case 1:
				host->put_sprite_hflip(nx     , ny     , 1, 0x4B);
				host->put_sprite_hflip(nx + 16, ny     , 1, 0x4A);
				host->put_sprite_hflip(nx     , ny + 16, 1, 0x6B);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0x6A);
				host->put_sprite_hflip(nx     , ny + 32, 1, 0x7B);
				host->put_sprite_hflip(nx + 16, ny + 32, 1, 0x7A);
				break;
		}
	}
};

void FlyBomber_Preview(int nx, int ny, class_type class) {
	host->put_sprite(nx    , ny    , 1, 0x4A);
	host->put_sprite(nx + 8, ny    , 1, 0x4B);
	host->put_sprite(nx    , ny + 8, 1, 0x5A);
	host->put_sprite(nx + 8, ny + 8, 1, 0x5B);
}

void FlyBomber_Init(entity e) {
	DEFINE_SELF(e_flybomber);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->max_dx = 4;
	self->dx = -self->max_dx;
	self->dy = 0;
	self->accel = 0;
	self->frame = 0;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_TAKEDAMAGE;
	self->flags |= FLAG_ONSCREEN_ONLY;

	self->health = 16;
	self->maxhealth = 16;

	// Collision rectangle:
	self->ecrx1 =  0;	self->ecry1 =   0;
	self->ecrx2 = 31;	self->ecry2 =  31;

	self->mcrx1 =  0;	self->mcry1 =   0;
	self->mcrx2 = 31;	self->mcry2 =  31;

	self->dmg_time = 20;

	self->face_dir = 6;
}

class_properties_t	ClassProperties_FlyBomber = {
	size:			sizeof(e_flybomber),
	name:			"FlyBomber",

	init:			FlyBomber_Init,
	prethink:		FlyBomber_PreThink,
	postthink:		NULL,
	touched:		FlyBomber_Touched,
	draw:			FlyBomber_Draw,
	preview:		FlyBomber_Preview,
	death:			FlyBomber_Death,
	maptest:		NULL,
	attacked:		FlyBomber_Attacked
};

