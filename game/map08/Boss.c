// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"
#include "Boss.h"

const int boss_page = 8;

int Boss_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	return -1;
}

void Boss_PreThink(entity e) {
	DEFINE_SELF(e_boss);

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};
	
	self->fr_time++;
	if (self->fr_time >= 16) {
		self->fr_time = 0;
		self->frame++;
		if (self->frame >= 4) self->frame = 0;
	}

	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
}

void Boss_Death(entity e) {
	DEFINE_SELF(e_boss);

	host->sndPlay(sounds.generic.explode1, SOUNDCHANNEL_PAIN, 0);
	//BecomeExplosion(e, BULLET_LARGE);
	host->e_kill(e);
	return;
}

void Boss_Touched(entity e, entity o) {
	DEFINE_SELF(e_boss);

	if ((o->class == CLASS_BULLET) && (o->team != e->team)) {
		host->sndPlay(sounds.generic.damage, SOUNDCHANNEL_PAIN, 0);
		return;
	}

	if (o->team != e->team)
		host->e_damage(o, e, 32);
}

void Boss_Draw(entity e) {
	DEFINE_SELF(e_boss);
	float	r, g, b;
	long	nx, ny;
	int		i, j;

	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx)) - 56;
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my)) - 56;

	// Flicker on and off 4 frames
	if (self->flags & FLAG_FLICKER) {
		r = (rand() % 32768) / 32768.0;
		g = (rand() % 32768) / 32768.0;
		b = (rand() % 32768) / 32768.0;

		glColor4f(r, g, b, 1.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	// Draw the Boss:
	switch (self->frame) {
		case 0:
		case 2:
			// Draw brain mass:
			for (i=1; i<7; ++i)
				host->put_sprite(nx + (i << 4), ny, boss_page, 0x80 | i);
			for (j=1; j<7; ++j)
				for (i=0; i<8; ++i)
					host->put_sprite(nx + (i << 4), ny + (j << 4), boss_page, ((8+j) << 4) | i);
			for (i=1; i<7; ++i)
				host->put_sprite(nx + (i << 4), ny + (7 << 4), boss_page, 0xF0 | i);
			// Draw tentacles:
			for (j=0; j<4; ++j)
				for (i=0; i<4; ++i)
					host->put_sprite(nx + ((i+2) << 4), ny + ((j+8) << 4), boss_page, (j << 4) | (i+6));
			break;
		case 1:
		case 3:
			// Draw brain mass:
			for (i=1; i<7; ++i)
				host->put_sprite(nx + (i << 4), ny, boss_page, 0x80 | (i+8));
			for (j=1; j<7; ++j)
				for (i=0; i<8; ++i)
					host->put_sprite(nx + (i << 4), ny + (j << 4), boss_page, ((8+j) << 4) | (i+8));
			for (i=1; i<7; ++i)
				host->put_sprite(nx + (i << 4), ny + (7 << 4), boss_page, 0xF0 | i);
			// Draw tentacles:
			for (j=0; j<4; ++j)
				for (i=0; i<4; ++i)
					host->put_sprite(nx + ((i+2) << 4), ny + ((j+8) << 4), boss_page, ((j+4) << 4) | (i+6));
			break;
	}

	if (self->flags & FLAG_FLICKER) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
}

void Boss_Preview(int nx, int ny, class_type class) {
	int	i, j;
	
	nx -= 24;
	ny -= 24;
	
	// Draw brain mass:
	for (i=1; i<7; ++i)
		host->put_sprite(nx + (i << 3), ny, boss_page, 0x80 | i);
	for (j=1; j<7; ++j)
		for (i=0; i<8; ++i)
			host->put_sprite(nx + (i << 3), ny + (j << 3), boss_page, ((8+j) << 4) | i);
	for (i=1; i<7; ++i)
		host->put_sprite(nx + (i << 3), ny + (7 << 3), boss_page, 0xF0 | i);
	// Draw tentacles:
	for (j=0; j<4; ++j)
		for (i=0; i<4; ++i)
			host->put_sprite(nx + ((i+2) << 3), ny + ((j+8) << 3), boss_page, (j << 4) | (i+6));
}

void Boss_Init(entity e) {
	DEFINE_SELF(e_boss);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 1;
	self->accel = 1;
	self->frame = 0;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_NOMAPCOLLISION | FLAG_TAKEDAMAGE;
	self->flags |= FLAG_ONSCREEN_ONLY;

	self->health = 511;
	self->maxhealth = 511;

	// Collision rectangle:
	self->ecrx1 = -8;	self->ecry1 =  -8;
	self->ecrx2 =  7;	self->ecry2 =   7;

	self->mcrx1 = -8;	self->mcry1 =  -8;
	self->mcrx2 =  7;	self->mcry2 =   7;

	self->dmg_time = 8;

	self->face_dir = 2;
};

class_properties_t	ClassProperties_Boss = {
	name:			"Stage 1 Boss",
	size:			sizeof(e_boss),
	init:			Boss_Init,
	prethink:		Boss_PreThink,
	touched:		Boss_Touched,
	draw:			Boss_Draw,
	preview:		Boss_Preview,
	death:			Boss_Death,
	attacked:		Boss_Attacked
};

