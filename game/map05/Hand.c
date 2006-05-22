// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

int Hand_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	return -1;
}

void Hand_SpillBall(entity e) {
	DEFINE_SELF(e_hand);

	e_bullet *bullet = (e_bullet *) host->e_spawn(CLASS_BULLET);
	if (bullet) {
		bullet->owner = e;
		bullet->team = self->team;
		if (self->face_dir == 6) {
			bullet->dx = 2.0;
			bullet->x = self->x + 6;
		} else if (self->face_dir == 2) {
			bullet->dx = -2.0;
			bullet->x = self->x + 2;
		}
		bullet->dy = 0;
		bullet->y = self->y - 16;
		bullet->kind = BULLET_MEDIUM;
		bullet->flags &= ~(FLAG_NOGRAVITY);
		bullet->flags |= FLAG_BOUNCE | FLAG_NODESTROY;
		bullet->killtime = 140;
		bullet->damage = 32;
	}
}


void Hand_PreThink(entity e) {
	DEFINE_SELF(e_hand);

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	// Decreate the frame timer for the hand open/close action.
	if (self->fr_time > 0) self->fr_time--;
	if (self->fr_time > 0) {
		if (self->fr_time % 16 == 8) self->frame = 0;
	}

	if (fabs((*host->player)->y - self->y) < 64) {
		if ((*host->player)->x > self->x) {
			self->face_dir = 6;
			if (((*host->player)->x - self->x < 96) && (self->fr_time % 16 == 0)) {
				if (rand() % 32768 > 16384) {
					if (self->fr_time == 0) self->fr_time = 63;
					self->frame = 1;
					Hand_SpillBall(e);
				}
			}
		} else if ((*host->player)->x < self->x) {
			self->face_dir = 2;
			if (self->x - ((*host->player)->x < 96) && (self->fr_time % 16 == 0)) {
				if (rand() % 32768 > 16384) {
					if (self->fr_time == 0) self->fr_time = 63;
					self->frame = 1;
					Hand_SpillBall(e);
				}
			}
		}
	}
	
	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
}

void Hand_Death(entity e) {
	DEFINE_SELF(e_hand);
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

void Hand_Touched(entity e, entity o) {
	DEFINE_SELF(e_hand);

	if ((o->class == CLASS_BULLET) && (o->team != e->team)) return;
	
	if (o->team != e->team) {
		host->e_damage(o, e, 32);
	}
}

void Hand_Draw(entity e) {
	DEFINE_SELF(e_hand);
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

	// Draw the Hand:
	if (self->face_dir == 2) {
		switch (self->frame) {
			case 0:
				host->put_sprite(nx     , ny - 16, 1, 0xBC);
				host->put_sprite(nx + 16, ny - 16, 1, 0xBD);
				host->put_sprite(nx     , ny     , 1, 0xCC);
				host->put_sprite(nx + 16, ny     , 1, 0xCD);
				host->put_sprite(nx     , ny + 16, 1, 0xFC);
				host->put_sprite(nx + 16, ny + 16, 1, 0xFD);
				break;
			case 1:
				host->put_sprite(nx     , ny - 16, 1, 0xDC);
				host->put_sprite(nx + 16, ny - 16, 1, 0xDD);
				host->put_sprite(nx     , ny     , 1, 0xEC);
				host->put_sprite(nx + 16, ny     , 1, 0xED);
				host->put_sprite(nx     , ny + 16, 1, 0xFC);
				host->put_sprite(nx + 16, ny + 16, 1, 0xFD);
				break;
		}
	} else if (self->face_dir == 6) {
		switch (self->frame) {
			case 0:
				host->put_sprite_hflip(nx     , ny - 16, 1, 0xBD);
				host->put_sprite_hflip(nx + 16, ny - 16, 1, 0xBC);
				host->put_sprite_hflip(nx     , ny     , 1, 0xCD);
				host->put_sprite_hflip(nx + 16, ny     , 1, 0xCC);
				host->put_sprite_hflip(nx     , ny + 16, 1, 0xFD);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0xFC);
				break;
			case 1:
				host->put_sprite_hflip(nx     , ny - 16, 1, 0xDD);
				host->put_sprite_hflip(nx + 16, ny - 16, 1, 0xDC);
				host->put_sprite_hflip(nx     , ny     , 1, 0xED);
				host->put_sprite_hflip(nx + 16, ny     , 1, 0xEC);
				host->put_sprite_hflip(nx     , ny + 16, 1, 0xFD);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0xFC);
				break;
		}
	}
	
	if (self->flags & FLAG_FLICKER) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	
};

void Hand_Preview(int nx, int ny, class_type class) {
	host->put_sprite(nx    , ny - 8, 1, 0xBC);
	host->put_sprite(nx + 8, ny - 8, 1, 0xBD);
	host->put_sprite(nx    , ny    , 1, 0xCC);
	host->put_sprite(nx + 8, ny    , 1, 0xCD);
	host->put_sprite(nx    , ny + 8, 1, 0xFC);
	host->put_sprite(nx + 8, ny + 8, 1, 0xFD);
}

void Hand_Init(entity e) {
	DEFINE_SELF(e_hand);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 0;
	self->accel = 0;
	self->frame = 0;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_NOMAPCOLLISION | FLAG_TAKEDAMAGE;
	self->flags |= FLAG_ONSCREEN_ONLY;

	self->health = 64;
	self->maxhealth = 64;

	// Collision rectangle:
	self->ecrx1 =  0;	self->ecry1 =   0;
	self->ecrx2 = 31;	self->ecry2 =  31;

	self->mcrx1 =  0;	self->mcry1 =   0;
	self->mcrx2 = 31;	self->mcry2 =  31;

	self->dmg_time = 20;

	self->face_dir = 2;
}

class_properties_t	ClassProperties_Hand = {
	size:			sizeof(e_hand),
	name:			"Hand",

	init:			Hand_Init,
	prethink:		Hand_PreThink,
	postthink:		NULL,
	touched:		Hand_Touched,
	draw:			Hand_Draw,
	preview:		Hand_Preview,
	death:			Hand_Death,
	maptest:		NULL,
	attacked:		Hand_Attacked
};

