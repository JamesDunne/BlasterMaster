// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

int Turret_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	return -1;
}

void Turret_SpawnBullet(entity e) {
	DEFINE_SELF(e_turret);
	
	e_bullet *bullet = (e_bullet *) host->e_spawn(CLASS_BULLET);
	if (bullet) {
		bullet->owner = e;
		bullet->team = e->team;
		
		bullet->dy = 0;

		bullet->dx = (6 - (self->fr_speed - 2)) * 1.2;
		if (self->turn % 2 == 0) bullet->dx = -bullet->dx;

		if (bullet->dx >= 0) bullet->x = e->x + 28;
		else bullet->x = e->x - 8;

		bullet->y = e->y + 16;
		bullet->kind = BULLET_MEDIUM_RED;
		bullet->flags &= ~(FLAG_NOGRAVITY);
		bullet->flags |= FLAG_BOUNCE;
		bullet->killtime = 140;
		bullet->damage = 32;
	}
}

void Turret_PreThink(entity e) {
	DEFINE_SELF(e_turret);

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
	
	if (self->fr_speed < 8) {
		e->fr_time++;
		if (e->fr_time > self->fr_speed) {
			e->fr_time = 0;
			e->frame++;
			if (e->frame > 1) e->frame = 0;
			if (rand() % 32768 <= 8192) Turret_SpawnBullet(e);
		}
	}
	
	// Speed up turret and slow it down again:
	self->turn++;
	if (self->turn == 8) {
		self->fr_speed = 7;
	} else if (self->turn == 16) {
		self->fr_speed = 6;
	} else if (self->turn == 24) {
		self->fr_speed = 5;
	} else if (self->turn == 32) {
		self->fr_speed = 4;
	} else if (self->turn == 48) {
		self->fr_speed = 3;
	} else if (self->turn == 64) {
		self->fr_speed = 2;

	} else if (self->turn == 160) {
		self->fr_speed = 3;
	} else if (self->turn == 168) {
		self->fr_speed = 4;
	} else if (self->turn == 176) {
		self->fr_speed = 5;
	} else if (self->turn == 184) {
		self->fr_speed = 6;
	} else if (self->turn == 192) {
		self->fr_speed = 7;
	} else if (self->turn == 200) {
		self->fr_speed = 8;

	} else if (self->turn == 296) {
		self->turn = 0;
	}
}

void Turret_Death(entity e) {
	DEFINE_SELF(e_turret);
	e_powerup *p;

	// If the odds are 1/4, spawn a powerup:
	if (rand() % 32768 > 16384) {
		p = (e_powerup *) host->e_spawn(CLASS_POWERUP_HOVER);
		p->x = e->x;
		p->y = e->y;
	}


	host->sndPlay(sounds.generic.explode1, SOUNDCHANNEL_PAIN, 0);
	BecomeExplosion(e, BULLET_LARGE);
	return;
}

/*
void Turret_PostThink(entity e) {
	DEFINE_SELF(e_turret);
}
*/

void Turret_Touched(entity e, entity o) {
	DEFINE_SELF(e_turret);

	if ((o->class == CLASS_BULLET) && (o->team != e->team)) {
		host->sndPlay(sounds.generic.damage, SOUNDCHANNEL_PAIN, 0);
		return;
	}

	if (o->team != e->team)
		host->e_damage(o, e, 32);
}

void Turret_Draw(entity e) {
	DEFINE_SELF(e_turret);
	float	r, g, b;
	long	nx, ny;

	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Flicker on and off 4 frames
	if (self->flags & FLAG_FLICKER) {
		r = (rand() % 32768) / 32768.0;
		g = (rand() % 32768) / 32768.0;
		b = (rand() % 32768) / 32768.0;

		glColor4f(r, g, b, 1.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	// Draw the Turret:
	switch (self->frame) {
		case 0:
			host->put_sprite_vflip(nx	 , ny	 , 1, 0x2E);
			host->put_sprite_vflip(nx + 16, ny	 , 1, 0x2F);
			host->put_sprite_vflip(nx	 , ny + 16, 1, 0x0E);
			host->put_sprite_vflip(nx + 16, ny + 16, 1, 0x0F);
			break;
		case 1:
			host->put_sprite_vflip(nx	 , ny	 , 1, 0x2E);
			host->put_sprite_vflip(nx + 16, ny	 , 1, 0x2F);
			host->put_sprite_vflip(nx	 , ny + 16, 1, 0x1E);
			host->put_sprite_vflip(nx + 16, ny + 16, 1, 0x1F);
			break;
	}

	if (self->flags & FLAG_FLICKER) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
}

void Turret_Preview(int nx, int ny, class_type class) {
	host->put_sprite_vflip(nx	, ny	, 1, 0x2E);
	host->put_sprite_vflip(nx + 8, ny	, 1, 0x2F);
	host->put_sprite_vflip(nx	, ny + 8, 1, 0x0E);
	host->put_sprite_vflip(nx + 8, ny + 8, 1, 0x0F);
}

void Turret_Init(entity e) {
	DEFINE_SELF(e_turret);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 0L;
	self->accel = 0L;
	self->frame = 0L;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_NOMAPCOLLISION | FLAG_TAKEDAMAGE;
	self->flags |= FLAG_ONSCREEN_ONLY;

	self->health = 96;
	self->maxhealth = 96;

	// Collision rectangle:
	self->ecrx1 =  0;	self->ecry1 =   0;
	self->ecrx2 = 31;	self->ecry2 =  31;

	self->mcrx1 =  0;	self->mcry1 =   0;
	self->mcrx2 = 31;	self->mcry2 =  31;

	self->dmg_time = 8;
	self->fr_speed = 8;

	self->face_dir = 2;
};

class_properties_t	ClassProperties_Turret = {
	name:			"Turret",
	size:			sizeof(e_turret),
	init:			Turret_Init,
	prethink:		Turret_PreThink,
	touched:		Turret_Touched,
	draw:			Turret_Draw,
	preview:		Turret_Preview,
	death:			Turret_Death,
	attacked:		Turret_Attacked
};

