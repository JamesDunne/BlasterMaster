// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"
#include "SubBoss01.h"
// from ../common:
#include "Bullet.h"

int SubBoss01_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;
	if (o->class == CLASS_BULLET) {
		e_bullet	*bullet = (e_bullet *)o;
		
		// In pain - flinch eye:
		e->frame = 2;
		
		// Only hyper and crusher bullets cause damage:
		if ((bullet->kind == BULLET_HYPER) || (bullet->kind == BULLET_CRUSHER)) {
			e->flicker_time = e->dmg_time;
			e->flags |= FLAG_FLICKER;
			return -1;
		}
	}
	return 0;
}

void SubBoss01_PreThink(entity e) {
	DEFINE_SELF(e_subboss01);

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	if (self->fr_time++ >= 3) {
		self->frame++;
		if (self->frame == 2) self->frame = 0;
		if (self->frame == 8) self->frame = 0;
		self->fr_time = 0;
	}

	// Count the damage timer down:
	if (e->dmg_timer) e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
}

void SubBoss01_Death(entity e) {
	DEFINE_SELF(e_subboss01);

	host->sndPlay(sounds.generic.explode1, SOUNDCHANNEL_PAIN, 0);
	host->e_kill(e);
	return;
}

void SubBoss01_Touched(entity e, entity o) {
	DEFINE_SELF(e_subboss01);

	if ((o->class == CLASS_BULLET) && (o->team != e->team)) return;

	if (o->team != e->team) {
		host->e_damage(o, e, 32);
	}
}

void SubBoss01_Draw(entity e) {
	DEFINE_SELF(e_subboss01);
	float	r, g, b;
	long	nx, ny;

	// Account for the 16.15 fixed format.
	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Flicker random colors:
	if (self->flags & FLAG_FLICKER) {
		r = (rand() % 32768) / 32768.0;
		g = (rand() % 32768) / 32768.0;
		b = (rand() % 32768) / 32768.0;

		glColor4f(r, g, b, 1.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	// Draw the Sub-Boss:
	switch (self->frame) {
		case 0:
		case 2:
			// Eye (open fully):
			host->put_sprite_hflip(nx     , ny - 32, 4, 0x6C);
			host->put_sprite_hflip(nx + 16, ny - 32, 4, 0x6B);
			host->put_sprite_hflip(nx     , ny - 16, 4, 0x7C);
			host->put_sprite_hflip(nx + 16, ny - 16, 4, 0x7B);
			// Claws 1st frame and top of "mouth"
			host->put_sprite_hflip(nx     , ny     , 4, 0x8C);
			host->put_sprite_hflip(nx + 16, ny     , 4, 0x8B);
			host->put_sprite_hflip(nx     , ny + 16, 4, 0x9C);
			host->put_sprite_hflip(nx + 16, ny + 16, 4, 0x9B);
			break;
		case 1:
			// Eye (open fully):
			host->put_sprite_hflip(nx     , ny - 32, 4, 0x6C);
			host->put_sprite_hflip(nx + 16, ny - 32, 4, 0x6B);
			host->put_sprite_hflip(nx     , ny - 16, 4, 0x7C);
			host->put_sprite_hflip(nx + 16, ny - 16, 4, 0x7B);
			// Claws 2nd frame and top of "mouth"
			host->put_sprite_hflip(nx     , ny     , 4, 0xAC);
			host->put_sprite_hflip(nx + 16, ny     , 4, 0xAB);
			host->put_sprite_hflip(nx     , ny + 16, 4, 0xBC);
			host->put_sprite_hflip(nx + 16, ny + 16, 4, 0xBB);
			break;
		// Eye close and open animation:
		case 3:
		case 7:
			// Eye (open 3/4):
			host->put_sprite_hflip(nx     , ny - 32, 4, 0x4C);
			host->put_sprite_hflip(nx + 16, ny - 32, 4, 0x4B);
			host->put_sprite_hflip(nx     , ny - 16, 4, 0x5C);
			host->put_sprite_hflip(nx + 16, ny - 16, 4, 0x5B);
			// Claws 1st frame and top of "mouth"
			host->put_sprite_hflip(nx     , ny     , 4, 0xAC);
			host->put_sprite_hflip(nx + 16, ny     , 4, 0xAB);
			host->put_sprite_hflip(nx     , ny + 16, 4, 0xBC);
			host->put_sprite_hflip(nx + 16, ny + 16, 4, 0xBB);
			break;
		case 4:
		case 6:
			// Eye (open 2/4):
			host->put_sprite_hflip(nx     , ny - 32, 4, 0x2C);
			host->put_sprite_hflip(nx + 16, ny - 32, 4, 0x2B);
			host->put_sprite_hflip(nx     , ny - 16, 4, 0x3C);
			host->put_sprite_hflip(nx + 16, ny - 16, 4, 0x3B);
			// Claws 1st frame and top of "mouth"
			host->put_sprite_hflip(nx     , ny     , 4, 0x8C);
			host->put_sprite_hflip(nx + 16, ny     , 4, 0x8B);
			host->put_sprite_hflip(nx     , ny + 16, 4, 0x9C);
			host->put_sprite_hflip(nx + 16, ny + 16, 4, 0x9B);
			break;
		case 5:
			// Eye (open 1/4):
			host->put_sprite_hflip(nx     , ny - 32, 4, 0x0C);
			host->put_sprite_hflip(nx + 16, ny - 32, 4, 0x0B);
			host->put_sprite_hflip(nx     , ny - 16, 4, 0x1C);
			host->put_sprite_hflip(nx + 16, ny - 16, 4, 0x1B);
			// Claws 1st frame and top of "mouth"
			host->put_sprite_hflip(nx     , ny     , 4, 0xAC);
			host->put_sprite_hflip(nx + 16, ny     , 4, 0xAB);
			host->put_sprite_hflip(nx     , ny + 16, 4, 0xBC);
			host->put_sprite_hflip(nx + 16, ny + 16, 4, 0xBB);
			break;
	}
	// Draw the same feet all the time:
	host->put_sprite_hflip(nx     , ny + 32, 4, 0xCC);
	host->put_sprite_hflip(nx + 16, ny + 32, 4, 0xCB);
	host->put_sprite_hflip(nx     , ny + 48, 4, 0xDC);
	host->put_sprite_hflip(nx + 16, ny + 48, 4, 0xDB);

	if (self->flags & FLAG_FLICKER) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
};

void SubBoss01_Preview(int nx, int ny, class_type class) {
	host->put_sprite_hflip(nx    , ny - 16, 4, 0x6C);
	host->put_sprite_hflip(nx + 8, ny - 16, 4, 0x6B);
	host->put_sprite_hflip(nx    , ny -  8, 4, 0x7C);
	host->put_sprite_hflip(nx + 8, ny -  8, 4, 0x7B);
	host->put_sprite_hflip(nx    , ny     , 4, 0x8C);
	host->put_sprite_hflip(nx + 8, ny     , 4, 0x8B);
	host->put_sprite_hflip(nx    , ny +  8, 4, 0x9C);
	host->put_sprite_hflip(nx + 8, ny +  8, 4, 0x9B);
	host->put_sprite_hflip(nx    , ny + 16, 4, 0xCC);
	host->put_sprite_hflip(nx + 8, ny + 16, 4, 0xCB);
	host->put_sprite_hflip(nx    , ny + 24, 4, 0xDC);
	host->put_sprite_hflip(nx + 8, ny + 24, 4, 0xDB);
}

void SubBoss01_Init(entity e) {
	DEFINE_SELF(e_subboss01);

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

	self->health = 512;
	self->maxhealth = 512;

	// Collision rectangle:
	self->ecrx1 =  0;	self->ecry1 = -32;
	self->ecrx2 = 31;	self->ecry2 =  63;

	self->mcrx1 =  0;	self->mcry1 = -32;
	self->mcrx2 = 31;	self->mcry2 =  63;

	self->dmg_time = 32;

	self->face_dir = 2;
}

class_properties_t	ClassProperties_SubBoss01 = {
	size:			sizeof(e_subboss01),
	name:			"Door Guard Boss",

	init:			SubBoss01_Init,
	prethink:		SubBoss01_PreThink,
	postthink:		NULL,
	touched:		SubBoss01_Touched,
	draw:			SubBoss01_Draw,
	preview:		SubBoss01_Preview,
	death:			SubBoss01_Death,
	maptest:		NULL,
	attacked:		SubBoss01_Attacked
};

