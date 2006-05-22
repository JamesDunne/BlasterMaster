// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

int RockGunner_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	host->sndPlay(sounds.generic.damage, SOUNDCHANNEL_PAIN, 0);
	return -1;
}

void RockGunner_Death(entity e) {
	DEFINE_SELF(e_rockgunner);
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

void RockGunner_PreThink(entity e) {
	DEFINE_SELF(e_rockgunner);

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;

	// Walk for a while:
	self->fr_time++;
	if (self->fr_time > 16) {
		self->frame++;
		if (self->frame > 1) self->frame = 0;
		self->fr_time = 0;
	}

	if ((*host->player) && !(self->flags & FLAG_JUMPING) && (self->flags & FLAG_ONGROUND)) {
		// Close enough?  Jump at him:
		if ((abs((*host->player)->x - self->x) <= 64) && (abs((*host->player)->y - self->y) <= 32)) {
			self->dy = -6.0;
			if ((*host->player)->x > self->x) self->face_dir = 2;
			if ((*host->player)->x < self->x) self->face_dir = 6;
			self->flags |= FLAG_JUMPING;
			self->flags &= ~FLAG_ONGROUND;
		}
	}
	
	// Attack every 80 frames if able to:
	if (self->attack_time++ >= 32) {
		// Is vertical distance in range?
		if (abs((*host->player)->y - self->y) <= 32) {
			if (((*host->player)->x >= self->x) && (self->face_dir == 2)) {
				// Shoot right:
				e_bullet *bullet = (e_bullet *) host->e_spawn(CLASS_BULLET);
				if (bullet) {
					bullet->owner = e;
					bullet->team = self->team;
					bullet->dx = 6.0;
					bullet->dy = 0;
					bullet->x = self->x + 32;
					bullet->y = self->y - 4;
					bullet->kind = BULLET_MEDIUM;
					bullet->killtime = 140;
					bullet->damage = 32;
				}
			} else if (((*host->player)->x <= self->x) && (self->face_dir == 6)) {
				// Shoot left:
				e_bullet *bullet = (e_bullet *) host->e_spawn(CLASS_BULLET);
				if (bullet) {
					bullet->owner = e;
					bullet->team = self->team;
					bullet->dx = -6.0;
					bullet->dy = 0;
					bullet->x = self->x - 16;
					bullet->y = self->y - 4;
					bullet->kind = BULLET_MEDIUM;
					bullet->killtime = 140;
					bullet->damage = 32;
				}
			}
		}
		self->attack_time = 0;
	}
	
	if ((self->flags & FLAG_JUMPING) && (self->dy >= 0)) {
		self->flags &= ~(FLAG_JUMPING);
	}

	if (!(self->flags & FLAG_ONGROUND)) {
		if (self->face_dir == 2) self->dx = 2.0;
		if (self->face_dir == 6) self->dx = -2.0;
	} else {
		if (self->face_dir == 2) {
			self->dx =  self->accel;
			if (!(host->map->mapflags[host->gettileat(self->x + 32 + self->dx, self->y + 32)] & MAPFLAG_SOLID))
				self->face_dir = 6;
		}
		if (self->face_dir == 6) {
			self->dx = -self->accel;
			if (!(host->map->mapflags[host->gettileat(self->x + self->dx, self->y + 32)] & MAPFLAG_SOLID))
				self->face_dir = 2;
		}
	}
}

void RockGunner_Touched(entity e, entity o) {
	DEFINE_SELF(e_rockgunner);

	if (o == (*host->world)) {
		if (e->collide_flags & COLLIDE_LEFT) {
			e->face_dir = 2;
		}
		if (e->collide_flags & COLLIDE_RIGHT) {
			e->face_dir = 6;
		}
		if (e->collide_flags & COLLIDE_BELOW) {
			self->flags &= ~FLAG_JUMPING;
		}
		return;
	}

	if ((o->class == CLASS_BULLET) && (o->team != e->team)) return;
	
	if (o->team != e->team) {
		host->e_damage(o, e, 32);
	}
}

void RockGunner_Draw(entity e) {
	DEFINE_SELF(e_rockgunner);
	long	nx, ny;

	// Flicker on and off 4 frames
	if (self->flags & FLAG_FLICKER) self->flicker_count++;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count >= 8)) self->flicker_count -= 8;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count % 8 < 4)) return;

	// Account for the 16.15 fixed format.
	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Draw the RockGunner:
	if (self->face_dir == 6) {
		switch (self->frame) {
			case 0:
				host->put_sprite(nx	    , ny - 16, 1, 0x0C);
				host->put_sprite(nx + 16, ny - 16, 1, 0x0D);
				host->put_sprite(nx	    , ny	 , 1, 0x1C);
				host->put_sprite(nx + 16, ny	 , 1, 0x1D);
				host->put_sprite(nx	    , ny + 16, 1, 0x2C);
				host->put_sprite(nx + 16, ny + 16, 1, 0x2D);
				// Gun:
				host->put_sprite(nx - 16, ny     , 1, 0x5C);
				break;
			case 1:
				host->put_sprite(nx	    , ny - 14, 1, 0x0C);
				host->put_sprite(nx + 16, ny - 14, 1, 0x0D);
				host->put_sprite(nx	    , ny +  2, 1, 0x3C);
				host->put_sprite(nx + 16, ny +  2, 1, 0x3D);
				host->put_sprite(nx	    , ny + 18, 1, 0x4C);
				host->put_sprite(nx + 16, ny + 18, 1, 0x4D);
				// Gun:
				host->put_sprite(nx - 16, ny +  2, 1, 0x5C);
				break;
		}
	} else {
		switch (self->frame) {
			case 0:
				host->put_sprite_hflip(nx	  , ny - 16, 1, 0x0D);
				host->put_sprite_hflip(nx + 16, ny - 16, 1, 0x0C);
				host->put_sprite_hflip(nx	  , ny     , 1, 0x1D);
				host->put_sprite_hflip(nx + 16, ny     , 1, 0x1C);
				host->put_sprite_hflip(nx	  , ny + 16, 1, 0x2D);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0x2C);
				// Gun:
				host->put_sprite_hflip(nx + 32, ny     , 1, 0x5C);
				break;
			case 1:
				host->put_sprite_hflip(nx	  , ny - 14, 1, 0x0D);
				host->put_sprite_hflip(nx + 16, ny - 14, 1, 0x0C);
				host->put_sprite_hflip(nx	  , ny +  2, 1, 0x3D);
				host->put_sprite_hflip(nx + 16, ny +  2, 1, 0x3C);
				host->put_sprite_hflip(nx	  , ny + 18, 1, 0x4D);
				host->put_sprite_hflip(nx + 16, ny + 18, 1, 0x4C);
				// Gun:
				host->put_sprite_hflip(nx + 32, ny +  2, 1, 0x5C);
				break;
		}
	}
};

void RockGunner_Preview(int nx, int ny, class_type class) {
	host->put_sprite(nx	   , ny - 8, 1, 0x0C);
	host->put_sprite(nx + 8, ny - 8, 1, 0x0D);
	host->put_sprite(nx    , ny    , 1, 0x1C);
	host->put_sprite(nx + 8, ny    , 1, 0x1D);
	host->put_sprite(nx    , ny + 8, 1, 0x2C);
	host->put_sprite(nx + 8, ny + 8, 1, 0x2D);
	// Gun:
	host->put_sprite(nx - 8, ny    , 1, 0x5C);
}

void RockGunner_Init(entity e) {
	DEFINE_SELF(e_rockgunner);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 4.0;
	self->accel = 1.0;
	self->frame = 0;
	self->flags = FLAG_TAKEDAMAGE;
	self->flags |= FLAG_ONSCREEN_ONLY;

	self->health = 32;
	self->maxhealth = 32;

	// Collision rectangle:
	self->ecrx1 =  0;	self->ecry1 = -15;
	self->ecrx2 = 31;	self->ecry2 =  31;

	self->mcrx1 =  0;	self->mcry1 = -15;
	self->mcrx2 = 31;	self->mcry2 =  31;

	self->dmg_time = 20;

	self->face_dir = 6;
}

class_properties_t	ClassProperties_RockGunner = {
	name:			"RockGunner",
	size:			sizeof(e_rockgunner),

	init:			RockGunner_Init,
	prethink:		RockGunner_PreThink,
	postthink:		NULL,
	touched:		RockGunner_Touched,
	draw:			RockGunner_Draw,
	preview:		RockGunner_Preview,
	death:			RockGunner_Death,
	maptest:		NULL,
	attacked:		RockGunner_Attacked
};
