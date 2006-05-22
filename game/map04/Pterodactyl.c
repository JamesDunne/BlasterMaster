// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

int Pterodactyl_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	return -1;
}

void Pterodactyl_PreThink(entity e) {
	DEFINE_SELF(e_pterodactyl);

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	self->fr_time++;
	if (self->fr_time == 8) {
		self->fr_time = 0;
		if (self->frame == 0) self->frame = 1; else self->frame = 0;
	}
	
	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
}

void Pterodactyl_Death(entity e) {
	DEFINE_SELF(e_pterodactyl);
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

void Pterodactyl_Touched(entity e, entity o) {
	DEFINE_SELF(e_pterodactyl);

	// Bounce back and forth on walls:
	if (o == (*host->world)) {
		if (self->face_dir == 2) {
			self->face_dir = 6;
			self->dx = self->max_dx;
		} else if (self->face_dir == 6) {
			self->face_dir = 2;
			self->dx = -self->max_dx;
		}
	}
	
	if ((o->class == CLASS_BULLET) && (o->team != e->team)) return;
	
	if (o->team != e->team) {
		host->e_damage(o, e, 32);
	}
}

void Pterodactyl_Draw(entity e) {
	DEFINE_SELF(e_pterodactyl);
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

	// Draw the Pterodactyl:
	if (self->face_dir == 2) {
		switch (self->frame) {
			case 0:
				host->put_sprite(nx     , ny     , 1, 0x8D);
				host->put_sprite(nx + 16, ny     , 1, 0x8E);
				host->put_sprite(nx + 32, ny     , 1, 0x8F);
				host->put_sprite(nx     , ny + 16, 1, 0x9D);
				host->put_sprite(nx + 16, ny + 16, 1, 0x9E);
				host->put_sprite(nx + 32, ny + 16, 1, 0x9F);
				break;
			case 1:
				host->put_sprite(nx     , ny     , 1, 0x8D);
				host->put_sprite(nx + 16, ny     , 1, 0x8E);
				host->put_sprite(nx + 32, ny     , 1, 0x8F);
				host->put_sprite(nx     , ny + 16, 1, 0xAD);
				host->put_sprite(nx + 16, ny + 16, 1, 0xAE);
				host->put_sprite(nx + 32, ny + 16, 1, 0xAF);
				break;
		}
	} else {
		switch (self->frame) {
			case 0:
				host->put_sprite_hflip(nx     , ny     , 1, 0x8F);
				host->put_sprite_hflip(nx + 16, ny     , 1, 0x8E);
				host->put_sprite_hflip(nx + 32, ny     , 1, 0x8D);
				host->put_sprite_hflip(nx     , ny + 16, 1, 0x9F);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0x9E);
				host->put_sprite_hflip(nx + 32, ny + 16, 1, 0x9D);
				break;
			case 1:
				host->put_sprite_hflip(nx     , ny     , 1, 0x8F);
				host->put_sprite_hflip(nx + 16, ny     , 1, 0x8E);
				host->put_sprite_hflip(nx + 32, ny     , 1, 0x8D);
				host->put_sprite_hflip(nx     , ny + 16, 1, 0xAF);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0xAE);
				host->put_sprite_hflip(nx + 32, ny + 16, 1, 0xAD);
				break;
		}
	}
				
	if (self->flags & FLAG_FLICKER) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	
};

void Pterodactyl_Preview(int nx, int ny, class_type class) {
	host->put_sprite(nx     , ny    , 1, 0x8D);
	host->put_sprite(nx +  8, ny    , 1, 0x8E);
	host->put_sprite(nx + 16, ny    , 1, 0x8F);
	host->put_sprite(nx     , ny + 8, 1, 0x9D);
	host->put_sprite(nx +  8, ny + 8, 1, 0x9E);
	host->put_sprite(nx + 16, ny + 8, 1, 0x9F);
}

void Pterodactyl_Init(entity e) {
	DEFINE_SELF(e_pterodactyl);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 6;
	self->accel = 6;
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

	self->face_dir = 2;
	self->dx = -self->max_dx;
}

class_properties_t	ClassProperties_Pterodactyl = {
	size:			sizeof(e_pterodactyl),
	name:			"Pterodactyl",

	init:			Pterodactyl_Init,
	prethink:		Pterodactyl_PreThink,
	postthink:		NULL,
	touched:		Pterodactyl_Touched,
	draw:			Pterodactyl_Draw,
	preview:		Pterodactyl_Preview,
	death:			Pterodactyl_Death,
	maptest:		NULL,
	attacked:		Pterodactyl_Attacked
};

