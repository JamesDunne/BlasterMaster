// C Source File
// Created 1/6/2004; 7:54:15 AM

#include <math.h>
#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"

int Bullet_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	return -1;
}

void Bullet_PreThink(entity e) {
	DEFINE_SELF(e_bullet);
	double	vx, vy, len;

	if (self->health <= 0) {
		BecomeExplosion(e, self->kind);
		return;
	}

	if (self->killtime-- <= 0) {
		self->killtime = 0;
		BecomeExplosion(e, self->kind);
		return;
	}

	if (self->kind == BULLET_HOMING_MISSILE) {
		if (self->target) {
			// Now, home in on the target:
			vx = self->target->x - self->x;
			vy = self->target->y - self->y;

			// If the difference is more than half the map, then move the other direction:
			if (vx <= -(host->map->width << 4)) vx += (host->map->width << 5);
			else if (vx >= (host->map->width << 4)) vx -= (host->map->width << 5);
			if (vy <= -(host->map->height << 4)) vy += (host->map->height << 5);
			else if (vy >= (host->map->height << 4)) vy -= (host->map->height << 5);

			len = sqrt((vx * vx) + (vy * vy));

			if (len > 0) {
				vx /= len;
				vy /= len;
				self->dx += vx * 0.75;
				self->dy += vy * 0.75;

				// Determine facing direction:
				if ((vy <= -0.7) && (vx > -0.3) && (vx <  0.3)) self->face_dir = 0;
				if ((vy <= -0.3) && (vx >  0.3) && (vx <  0.7)) self->face_dir = 1;
				if ((vx >=  0.7) && (vy > -0.3) && (vy <  0.3)) self->face_dir = 2;
				if ((vy >=  0.3) && (vx >  0.3) && (vx <  0.7)) self->face_dir = 3;
				if ((vy >=  0.7) && (vx > -0.3) && (vx <  0.3)) self->face_dir = 4;
				if ((vy >=  0.3) && (vx < -0.3) && (vx > -0.7)) self->face_dir = 5;
				if ((vx <= -0.7) && (vy > -0.3) && (vy <  0.3)) self->face_dir = 6;
				if ((vy <= -0.3) && (vx < -0.3) && (vx > -0.7)) self->face_dir = 7;

				len = sqrt((self->dx * self->dx) + (self->dy * self->dy));
				if (len > 6.5) {
					self->dx *= 6.5 / len;
					self->dy *= 6.5 / len;
				}
			}
		}
	}
}

/*
void Bullet_PostThink(entity e) {
	DEFINE_SELF(e_bullet);
}
*/

// The bullet touched something:
void Bullet_Touched(entity e, entity o) {
	DEFINE_SELF(e_bullet);

	if (o == *(host->world)) {
		if (self->owner->flags & FLAG_CONTROLLED) {
			// Breakable wall:
			if ((self->collide_mapflags & MAPFLAG_SOLID) && (self->collide_mapflags & MAPFLAG_WATER)) {
				// Change the tile to zero:
				host->settileat(self->ctx, self->cty, 0);
			}
		}

		if (!(self->flags & FLAG_NODESTROY)) {
			self->x = self->cmx; self->y = self->cmy;
			BecomeExplosion(e, self->kind);
		}
		return;
	}

	// Test if it's okay to hit this object:
	//   1)  Not my owner
	//   2)  Object can take damage
	//   3)  On a different team than my team (MONSTERS != FROGHUNTERS)
	if ((o != self->owner) && (o->flags & FLAG_TAKEDAMAGE) && (o->team != self->owner->team)) {
		host->e_damage(o, (entity)self, self->damage);

		// Explode: (unless we're told not to, like a large bullet)
		if (!(self->flags & FLAG_NODESTROY)) {
			host->e_kill(e);
		}
		return;
	}
}

void Bullet_Draw(entity e) {
	DEFINE_SELF(e_bullet);
	long	nx, ny;
	float	r, g, b;

	// Account for the 16.15 fixed format.
	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Face left:
	switch (self->kind) {
		case BULLET_SMALL:
			if (self->face_dir == 6) {
				host->put_sprite(nx - 8, ny - 16, 4, 0xA3);
				host->put_sprite(nx - 8, ny     , 4, 0xB3);
			} else if (self->face_dir == 4) {
				host->put_sprite_vflip(nx - 8, ny - 16, 4, 0xB4);
				host->put_sprite_vflip(nx - 8, ny     , 4, 0xA4);
			} else if (self->face_dir == 2) {
				host->put_sprite_hflip(nx - 8, ny - 16, 4, 0xA3);
				host->put_sprite_hflip(nx - 8, ny     , 4, 0xB3);
			} else if (self->face_dir == 0) {
				host->put_sprite(nx - 8, ny - 16, 4, 0xA4);
				host->put_sprite(nx - 8, ny     , 4, 0xB4);
			}
			break;
		case BULLET_MEDIUM:
			host->put_sprite(nx - 8, ny - 16, 4, 0xD5);
			host->put_sprite(nx - 8, ny     , 4, 0xD6);
			break;
	}
};

void Bullet_Init(entity e) {
	DEFINE_SELF(e_bullet);

	// Set the position
	self->x = 0L;
	self->y = 0L;

	// Appearance:
	self->fr_time = 0;
	self->frame = 0;

	// Can't be shot down by other bullets:
	self->health = 32;
	self->maxhealth = 32;

	// Deltas:
	self->dx = 0; self->dy = 0;
	self->max_dx = 16.0;
	self->accel = 0.125;

	// 0-7, 0 = up, 2 = right, 4 = down, 6 = left w/ diagonals at 1, 3, 5, and 7.
	self->flags = FLAG_NOGRAVITY | FLAG_NOFRICTION;
	self->face_dir = 2;

	// Collision rectangle:
	self->ecrx1 =  4;	self->ecry1 =  4;
	self->ecrx2 = 12;	self->ecry2 = 12;

	self->mcrx1 =  4;	self->mcry1 =  4;
	self->mcrx2 = 12;	self->mcry2 = 12;

	// Default amount of damage for a bullet:
	self->damage = 32;

	self->killtime = 120;
};

class_properties_t	ClassProperties_Bullet = {
	name:			"Bullet",
	size:			sizeof(e_bullet),
	init:			Bullet_Init,
	prethink:		Bullet_PreThink,
	touched:		Bullet_Touched,
	draw:			Bullet_Draw,
	attacked:		Bullet_Attacked
};

