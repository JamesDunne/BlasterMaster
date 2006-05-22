// C Source File
// Created 1/6/2004; 6:16:38 AM

#include <math.h>
#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

const double	bullet_speed = 8.0;

int VFlyer_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	return -1;
}

// Test if we're in water and not running into solid things:
int VFlyer_Maptest(int m) {
	if (m & MAPFLAG_SOLID) return -1;
	if (m & MAPFLAG_WATER) return -1;
	return 0;
}

void VFlyer_Death(entity e) {
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

void VFlyer_PreThink(entity e) {
	DEFINE_SELF(e_vflyer);

	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	if ((*host->player)->flags & FLAG_DEAD) return;


	self->fr_time++;
	if ( self->fr_time >= 8 ) self->frame = 0;
	if ( self->fr_time >= 64 ) {
		self->fr_time = 0;
		// Time to shoot:
		/*if ( (*host->player)->y > self->y ) */ {
			
			// Define some points:
			// {ex,ey} = enemy position
			double	ex = self->x + 16;
			double	ey = self->y + 24;
			// {px,py} = player position
			double	px = (*host->player)->x - ex;
			double	py = ((*host->player)->y - 16) - ey;
			// {vx,vy} = player velocity
			double	vx = (*host->player)->dx;
			double	vy = 0;	//(*host->player)->dy;
			
// 			fprintf(stderr, "Correct bullet trajectory:\n");
// 			fprintf(stderr, "  p: {%4.2f, %4.2f}      v: {%4.2f, %4.2f}\n\n", ex, ey, px, py, vx, vy);
			
			// Here we solve the equation for t:
			//   sqrt( (px + vx*t)^2 + (py + vy*t)^2 ) = bullet_speed * t
			// resulting in a quadratic:
			//   A = (vx^2 + vy^2 - bullet_speed^2)
			//   B = 2 * vx * px + 2 * vy * py
			//   C = px^2 + py^2
			
#			define	sqr(x)	(( x ) * ( x ))
			
			double	quadA = (sqr(vx) + sqr(vy) - sqr(bullet_speed));
			double	quadB = 2.0 * vx * px + 2.0 * vy * py;
			double	quadC = sqr(px) + sqr(py);
			
// 			fprintf(stderr, "  %4.4f = a\n", quadA);
// 			fprintf(stderr, "  %4.4f = b\n", quadB);
// 			fprintf(stderr, "  %4.4f = c\n", quadC);
			
			// t is time along player's projected path when the bullet should hit:
			double	t = 0;
			
			// Determinant
			double	dtm = sqr(quadB) - 4.0 * quadA * quadC;
			
// 			fprintf(stderr, "  %4.4f = b^2 - 4ac\n", dtm);
			
			if (dtm > 0) {
				// determinant > 0, take positive soln.
				t = (-quadB - sqrt(dtm)) / (2.0 * quadA);
// 				fprintf(stderr, "  > 0\n", t);
			} else if (dtm == 0.0) { 
				// determinant == 0, one soln.
				t = -quadB / (2.0 * quadA);
// 				fprintf(stderr, "  = 0\n", t);
			}
			// If we get an imaginary solution from above, then t will be 0.
// 			fprintf(stderr, "  %4.4f = t\n", t);
			
			// {wx,wy} is now the relative position along the player's projected path where the bullet hits
			double	wx = px + t * vx;
			double	wy = py + t * vy;
			
//			fprintf(stderr, "  w: {%4.2f, %4.2f}\n\n", wx, wy);
			
			double	len = sqrt(sqr(wx) + sqr(wy));
			
			if (len > 0) {
				e_bullet *bullet = (e_bullet *) host->e_spawn(CLASS_BULLET);
				if (bullet) {
					self->frame = 1;
					bullet->owner = e;
					bullet->team = self->team;
					bullet->dx = wx / len * bullet_speed;
					bullet->dy = wy / len * bullet_speed;
					bullet->x = ex;
					bullet->y = ey;
					bullet->kind = BULLET_SMALL;
					bullet->killtime = 140;
					bullet->damage = 32;
				}
			}
		}
	}
}

/*
void VFlyer_PostThink(entity e) {
	DEFINE_SELF(e_vflyer);
}
*/

void VFlyer_Touched(entity e, entity o) {
	DEFINE_SELF(e_vflyer);

	if (o == *(host->world)) {
		// Flip direction depending on what we collided with:
		if (self->collide_flags & COLLIDE_LEFT) {
			self->dx = 4.0;
			self->face_dir = 2;
		} else if (self->collide_flags & COLLIDE_RIGHT) {
			self->dx = -4.0;
			self->face_dir = 6;
		}
		if (self->collide_flags & COLLIDE_ABOVE) self->dy = 1.0;
		if (self->collide_flags & COLLIDE_BELOW) self->dy = -1.0;
		return;
	}

	if ((o->class == CLASS_BULLET) && (o->team != e->team)) {
		host->sndPlay(sounds.generic.damage, SOUNDCHANNEL_PAIN, 0);
		return;
	}

	if (o->team != e->team)
		host->e_damage(o, e, 32);
}

void VFlyer_Draw(entity e) {
	DEFINE_SELF(e_vflyer);
	float	r, g, b;
	long	nx, ny;

	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));
	if ( (nx < 0) || (ny < 0) || (nx > (*host->screen_w)) || (ny > (*host->screen_h)) ) return;

	// Flicker random colors:
	if (self->flags & FLAG_FLICKER) {
		r = (rand() % 32768) / 32768.0;
		g = (rand() % 32768) / 32768.0;
		b = (rand() % 32768) / 32768.0;

		glColor4f(r, g, b, 1.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	// Draw the vflyer:
	if (self->face_dir == 6) {
		switch (self->frame) {
			case 0:
				host->put_sprite(nx	 , ny	 , 1, 0x8A);
				host->put_sprite(nx + 16, ny	 , 1, 0x8B);
				host->put_sprite(nx	 , ny + 16, 1, 0x9A);
				host->put_sprite(nx + 16, ny + 16, 1, 0x9B);
				break;
			case 1:
				host->put_sprite(nx	 , ny	 , 1, 0x8A);
				host->put_sprite(nx + 16, ny	 , 1, 0x8B);
				host->put_sprite(nx	 , ny + 16, 1, 0xAA);
				host->put_sprite(nx + 16, ny + 16, 1, 0xAB);
				break;
		}
	} else if (self->face_dir == 2) {
		switch (self->frame) {
			case 0:
				host->put_sprite_hflip(nx	 , ny	 , 1, 0x8B);
				host->put_sprite_hflip(nx + 16, ny	 , 1, 0x8A);
				host->put_sprite_hflip(nx	 , ny + 16, 1, 0x9B);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0x9A);
				break;
			case 1:
				host->put_sprite_hflip(nx	 , ny	 , 1, 0x8B);
				host->put_sprite_hflip(nx + 16, ny	 , 1, 0x8A);
				host->put_sprite_hflip(nx	 , ny + 16, 1, 0xAB);
				host->put_sprite_hflip(nx + 16, ny + 16, 1, 0xAA);
				break;
		}
	}

	if (self->flags & FLAG_FLICKER) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
}

void VFlyer_Preview(int x, int y, class_type class) {
	// Draw the vflyer:
	host->put_sprite(x	, y	, 1, 0x8A);
	host->put_sprite(x + 8, y	, 1, 0x8B);
	host->put_sprite(x	, y + 8, 1, 0x9A);
	host->put_sprite(x + 8, y + 8, 1, 0x9B);
}

void VFlyer_Init(entity e) {
	DEFINE_SELF(e_vflyer);

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
	self->ecrx1 =  4;	self->ecry1 =  4;
	self->ecrx2 = 27;	self->ecry2 = 27;

	self->mcrx1 =  4;	self->mcry1 =  4;
	self->mcrx2 = 27;	self->mcry2 = 27;

	self->health = 32;
	self->maxhealth = 32;
	self->dmg_time = 8;

	// Face right:
	self->face_dir = 2;

	self->dx = 4.0;
	self->dy = -1.0;
};

class_properties_t	ClassProperties_VFlyer = {
	name:			"V-Flyer",
	size:			sizeof(e_vflyer),
	init:			VFlyer_Init,
	prethink:		VFlyer_PreThink,
	touched:		VFlyer_Touched,
	draw:			VFlyer_Draw,
	preview:		VFlyer_Preview,
	death:			VFlyer_Death,
	maptest:		VFlyer_Maptest,
	attacked:		VFlyer_Attacked
};


