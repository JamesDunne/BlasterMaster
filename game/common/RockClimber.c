// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

void RockClimber_FaceDir(e_rockclimber *self, int facedir) {
/*
	if (self->index == 0x0D)
		fprintf(stderr, "[0x%03X] %s TO ", self->index, 
			(self->face_dir == 0 ? "UP   " :
				(self->face_dir == 2 ? "RIGHT" :
					(self->face_dir == 4 ? "DOWN " :
						(self->face_dir == 6 ? "LEFT " : "UNK  ") ) ) ) );
	switch (facedir) {
		case 0:
			self->dy = -self->accel; self->dx = 0;
			if (self->index == 0x0D)
				fprintf(stderr, "UP:	{%4d, %4d}		{%4d, %4d}\n", (int)self->x, (int)self->y,
					((long)(self->x * 0.03125)) * 32, ((long)(self->y * 0.03125)) * 32);
			break;
		case 2:
			self->dx =  self->accel; self->dy = 0;
			if (self->index == 0x0D)
				fprintf(stderr, "RIGHT: {%4d, %4d}		{%4d, %4d}\n", (int)self->x, (int)self->y,
					((long)(self->x * 0.03125)) * 32, ((long)(self->y * 0.03125)) * 32);
			break;
		case 4:
			self->dy =  self->accel; self->dx = 0;
			if (self->index == 0x0D)
				fprintf(stderr, "DOWN:  {%4d, %4d}		{%4d, %4d}\n", (int)self->x, (int)self->y,
					((long)(self->x * 0.03125)) * 32, ((long)(self->y * 0.03125)) * 32);
			break;
		case 6:
			self->dx = -self->accel; self->dy = 0;
			if (self->index == 0x0D)
				fprintf(stderr, "LEFT:  {%4d, %4d}		{%4d, %4d}\n", (int)self->x, (int)self->y,
					((long)(self->x * 0.03125)) * 32, ((long)(self->y * 0.03125)) * 32);
			break;
	}
*/
	// 0x0E is around a floating block
	// 0x08 is around a wall
/*
	{
		fprintf(stderr, "[0x%03X] %d TO %d: ", self->index, self->face_dir, facedir);
		fprintf(stderr, "{%4d, %4d}		{%4d, %4d}\n", (int)self->x, (int)self->y,
			((long)(self->x * 0.03125)) * 32, ((long)(self->y * 0.03125)) * 32);
	}
*/
	switch (facedir) {
		case 0:
			self->dy = -self->accel; self->dx = 0;
			break;
		case 2:
			self->dx =  self->accel; self->dy = 0;
			break;
		case 4:
			self->dy =  self->accel; self->dx = 0;
			break;
		case 6:
			self->dx = -self->accel; self->dy = 0;
			break;
	}

	// Coordinate adjustment to keep it stable:
	if ((self->face_dir == 6) && (facedir == 4)) {
		self->x = ((long)(self->x * 0.03125)) * 32;
		self->y = ((long)((self->y + self->accel) * 0.03125)) * 32;
	} else if ((self->face_dir == 6) && (facedir == 0)) {
	}

	if ((self->face_dir == 4) && (facedir == 2)) {
		self->x = ((long)(self->x * 0.03125)) * 32;
		self->y = ((long)((self->y + self->accel) * 0.03125)) * 32;
	} else if ((self->face_dir == 4) && (facedir == 6)) {
		self->x = ((long)(self->x * 0.03125)) * 32;
		self->y = ((long)((self->y + self->accel) * 0.03125)) * 32;
	}

	if ((self->face_dir == 2) && (facedir == 0)) {
		self->x = ((long)(self->x * 0.03125)) * 32;
		self->y = ((long)(self->y * 0.03125)) * 32;
	} else if ((self->face_dir == 2) && (facedir == 4)) {
		self->x = ((long)(self->x * 0.03125)) * 32/* + 32*/;
		self->y = ((long)(self->y * 0.03125)) * 32;
	}

	if ((self->face_dir == 0) && (facedir == 6)) {
//		self->x = ((long)(self->x * 0.03125)) * 32;
//		self->y = ((long)(self->y * 0.03125)) * 32;
	} else if ((self->face_dir == 0) && (facedir == 2)) {
	
	}

	self->face_dir = facedir;
}

int RockClimber_CheckLeft(e_rockclimber *e) {
	Uint8	*flags = host->map->mapflags;
	int		x, y, test;
	
	test = 0;
	x = e->x - e->accel;
	for (y = e->y; y < e->y + 32; ++y)
		test |= (flags[host->gettileat(x, y)] & MAPFLAG_SOLID);
	
	return test;
}

int RockClimber_CheckAbove(e_rockclimber *e) {
	Uint8	*flags = host->map->mapflags;
	int		x, y, test;
	
	test = 0;
	y = e->y - e->accel;
	for (x = e->x; x < e->x + 32; ++x)
		test |= (flags[host->gettileat(x, y)] & MAPFLAG_SOLID);
	
	return test;
}

int RockClimber_CheckRight(e_rockclimber *e) {
	Uint8	*flags = host->map->mapflags;
	int		x, y, test;

	test = 0;
	x = e->x + 32;
	for (y = e->y; y < e->y + 32; ++y)
		test |= (flags[host->gettileat(x, y)] & MAPFLAG_SOLID);

	return test;
}

int RockClimber_CheckBelow(e_rockclimber *e) {
	Uint8	*flags = host->map->mapflags;
	int		x, y, test;

	test = 0;
	y = e->y + 32;
	for (x = e->x; x < e->x + 32; ++x)
		test |= (flags[host->gettileat(x, y)] & MAPFLAG_SOLID);

	return test;
}

int RockClimber_Attacked(entity e, entity o, long damage) {
	// Don't take damage from the world:
	if (o == (*host->world)) return 0;
	if (o->team == e->team) return 0;

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;
	host->sndPlay(sounds.generic.damage, SOUNDCHANNEL_PAIN, 0);
	return -1;
}

void RockClimber_PreThink(entity e) {
	DEFINE_SELF(e_rockclimber);
	entity	p = (entity)(*host->player);

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;

	self->fr_time++;
	if (self->fr_time > ((3 - self->accel) * 8)) {
		self->fr_time = 0;
		self->frame++;
		if (self->frame > 1) self->frame = 0;
	}

	if (self->cwd == CWD_CCW) {
		if (self->flags & FLAG_JUMPING) {
			if (RockClimber_CheckLeft(self) && (self->face_dir == 4)) {
				RockClimber_FaceDir(self, 0);
				self->flags &= ~FLAG_JUMPING;
			}
			if (RockClimber_CheckAbove(self) && (self->face_dir == 6)) {
				RockClimber_FaceDir(self, 2);
				self->flags &= ~FLAG_JUMPING;
			}
			if (RockClimber_CheckRight(self) && (self->face_dir == 0)) {
				RockClimber_FaceDir(self, 4);
				self->flags &= ~FLAG_JUMPING;
			}
			if (RockClimber_CheckBelow(self) && (self->face_dir == 2)) {
				RockClimber_FaceDir(self, 6);
				self->flags &= ~FLAG_JUMPING;
			}
		} else {
			// Counter-clockwise direction:
			if ((self->face_dir == 6) && !RockClimber_CheckBelow(self)) {
				// Heading left and nothing below us
				RockClimber_FaceDir(self, 4);
				return;
			} else if ((self->face_dir == 4) && !RockClimber_CheckRight(self)) {
				// Heading down and nothing right of us
				RockClimber_FaceDir(self, 2);
				return;
			} else if ((self->face_dir == 2) && !RockClimber_CheckAbove(self)) {
				// Heading right and nothing above us
				RockClimber_FaceDir(self, 0);
				return;
			} else if ((self->face_dir == 0) && !RockClimber_CheckLeft(self)) {
				// Heading up and nothing left of us
				RockClimber_FaceDir(self, 6);
				return;
			}

			if (RockClimber_CheckLeft(self) && (self->face_dir == 6)) {
				//fprintf(stderr, "[0x%03X]:  turn up from left\n", self->index);
				RockClimber_FaceDir(self, 0);
			} else if (RockClimber_CheckAbove(self) && (self->face_dir == 0)) {
				//fprintf(stderr, "[0x%03X]:  turn right from up\n", self->index);
				RockClimber_FaceDir(self, 2);
			} else if (RockClimber_CheckRight(self) && (self->face_dir == 2)) {
				//fprintf(stderr, "[0x%03X]:  turn down from right\n", self->index);
				RockClimber_FaceDir(self, 4);
			} else if (RockClimber_CheckBelow(self) && (self->face_dir == 4)) {
				// fprintf(stderr, "[0x%03X]:  turn left from down\n", self->index);
				RockClimber_FaceDir(self, 6);
			}
		}

		// Fly straight at the player if he's in range:
		if (self->face_dir == 0)
			if ( (p->x > e->x) && (p->y >= e->y + e->ecry1) && (p->y <= e->y + e->ecry2) ) {
				self->dx = 4; self->dy = 0; self->flags |= FLAG_JUMPING;
			}
		if (self->face_dir == 2)
			if ( (p->y > e->y) && (p->x >= e->x + e->ecrx1) && (p->x <= e->x + e->ecrx2) ) {
				self->dy = 4; self->dx = 0; self->flags |= FLAG_JUMPING;
			}
		if (self->face_dir == 4)
			if ( (p->x < e->x) && (p->y >= e->y + e->ecry1) && (p->y <= e->y + e->ecry2) ) {
				self->dx = -4; self->dy = 0; self->flags |= FLAG_JUMPING;
			}
		if (self->face_dir == 6)
			if ( (p->y < e->y) && (p->x >= e->x + e->ecrx1) && (p->x <= e->x + e->ecrx2) ) {
				self->dy = -4; self->dx = 0; self->flags |= FLAG_JUMPING;
			}
	}
}

void RockClimber_Death(entity e) {
//	DEFINE_SELF(e_rockclimber);
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

void RockClimber_Touched(entity e, entity o) {
//	DEFINE_SELF(e_rockclimber);

	if (o == (*host->world)) return;
	if ((o->class == CLASS_BULLET) && (o->team != e->team)) return;
	
	if (o->team != e->team) {
		host->e_damage(o, e, 32);
	}
}

void RockClimber_Draw(entity e) {
	DEFINE_SELF(e_rockclimber);
	float	r, g, b;
	long	nx, ny;
	int		cm;

	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Flicker:
	if (self->flags & FLAG_FLICKER) {
		r = (rand() % 32768) / 32768.0;
		g = (rand() % 32768) / 32768.0;
		b = (rand() % 32768) / 32768.0;

		glColor4f(r, g, b, 1.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	// Different colors for each rcok-climber:
	if (self->class == CLASS_ROCKCLIMBER_CW)		cm = 1;
	if (self->class == CLASS_ROCKCLIMBER_CCW)		cm = 1;
	if (self->class == CLASS_ROCKCLIMBER_RED_CW)	cm = 0;
	if (self->class == CLASS_ROCKCLIMBER_RED_CCW)	cm = 0;

	if (self->flags & FLAG_JUMPING) self->frame = 0;
	
	// Draw the RockClimber:
	if (self->cwd == CWD_CCW) {
		switch (self->face_dir) {
			case 0:
				switch (self->frame) {
					case 0:
						host->put_sprite(nx	 , ny	 , cm, 0xCE);
						host->put_sprite(nx + 16, ny	 , cm, 0xCF);
						host->put_sprite(nx	 , ny + 16, cm, 0xDE);
						host->put_sprite(nx + 16, ny + 16, cm, 0xDF);
						break;
					case 1:
						host->put_sprite(nx -  2, ny	 , cm, 0xEE);
						host->put_sprite(nx + 14, ny	 , cm, 0xCF);
						host->put_sprite(nx -  2, ny + 16, cm, 0xFE);
						host->put_sprite(nx + 14, ny + 16, cm, 0xDF);
						break;
				}
				break;
			case 2:
				switch (self->frame) {
					case 0:
						host->put_sprite_hvflip(nx	 , ny	 , cm, 0xAF);
						host->put_sprite_hvflip(nx + 16, ny	 , cm, 0xAE);
						host->put_sprite_hvflip(nx	 , ny + 16, cm, 0x9F);
						host->put_sprite_hvflip(nx + 16, ny + 16, cm, 0x9E);
						break;
					case 1:
						host->put_sprite_hvflip(nx	 , ny -  2, cm, 0xBF);
						host->put_sprite_hvflip(nx + 16, ny -  2, cm, 0xBE);
						host->put_sprite_hvflip(nx	 , ny + 14, cm, 0x9F);
						host->put_sprite_hvflip(nx + 16, ny + 14, cm, 0x9E);
						break;
				}
				break;
			case 4:
				switch (self->frame) {
					case 0:
						host->put_sprite_hvflip(nx	 , ny	 , cm, 0xDF);
						host->put_sprite_hvflip(nx + 16, ny	 , cm, 0xDE);
						host->put_sprite_hvflip(nx	 , ny + 16, cm, 0xCF);
						host->put_sprite_hvflip(nx + 16, ny + 16, cm, 0xCE);
						break;
					case 1:
						host->put_sprite_hvflip(nx +  2, ny	 , cm, 0xDF);
						host->put_sprite_hvflip(nx + 18, ny	 , cm, 0xFE);
						host->put_sprite_hvflip(nx +  2, ny + 16, cm, 0xCF);
						host->put_sprite_hvflip(nx + 18, ny + 16, cm, 0xEE);
						break;
				}
				break;
			case 6:
				switch (self->frame) {
					case 0:
						host->put_sprite(nx	 , ny	 , cm, 0x9E);
						host->put_sprite(nx + 16, ny	 , cm, 0x9F);
						host->put_sprite(nx	 , ny + 16, cm, 0xAE);
						host->put_sprite(nx + 16, ny + 16, cm, 0xAF);
						break;
					case 1:
						host->put_sprite(nx	 , ny +  2, cm, 0x9E);
						host->put_sprite(nx + 16, ny +  2, cm, 0x9F);
						host->put_sprite(nx	 , ny + 18, cm, 0xBE);
						host->put_sprite(nx + 16, ny + 18, cm, 0xBF);
						break;
				}
				break;
		}
	}

	if (self->flags & FLAG_FLICKER) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
};

void RockClimber_Preview(int nx, int ny, class_type class) {
	if ((class == CLASS_ROCKCLIMBER_CW) || (class == CLASS_ROCKCLIMBER_CCW)) {
		host->put_sprite(nx	, ny	, 1, 0x9E);
		host->put_sprite(nx + 8, ny	, 1, 0x9F);
		host->put_sprite(nx	, ny + 8, 1, 0xAE);
		host->put_sprite(nx + 8, ny + 8, 1, 0xAF);
	} else {
		host->put_sprite(nx	, ny	, 0, 0x9E);
		host->put_sprite(nx + 8, ny	, 0, 0x9F);
		host->put_sprite(nx	, ny + 8, 0, 0xAE);
		host->put_sprite(nx + 8, ny + 8, 0, 0xAF);
	}
}

void RockClimber_Init(entity e) {
	DEFINE_SELF(e_rockclimber);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0; self->dy = 0;
	self->max_dx = 8.0;
	self->frame = 0;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_TAKEDAMAGE | FLAG_NOMAPCOLLISION;
	self->flags |= FLAG_ONSCREEN_ONLY;

	// Algorithms for wall-following are only stable at 1 and multiples of 2 up to 16.
	if (self->class == CLASS_ROCKCLIMBER_CW)		self->accel = 1;
	if (self->class == CLASS_ROCKCLIMBER_CCW)		self->accel = 1;
	if (self->class == CLASS_ROCKCLIMBER_RED_CW)	self->accel = 2;
	if (self->class == CLASS_ROCKCLIMBER_RED_CCW)	self->accel = 2;

	self->health = 32;
	self->maxhealth = 32;

	// Collision rectangle:
	self->ecrx1 =  0;	self->ecry1 =   0;
	self->ecrx2 = 31;	self->ecry2 =  31;

	self->mcrx1 =  0;	self->mcry1 =   0;
	self->mcrx2 = 31;	self->mcry2 =  31;

	self->dmg_time = 8;

	// Head left in a counter-clockwise direction:
	self->dx = -self->accel;
	self->face_dir = 6;
	self->cwd = CWD_CCW;
}

class_properties_t	ClassProperties_RockClimber = {
	name:			"RockClimber",
	size:			sizeof(e_rockclimber),
	init:			RockClimber_Init,
	prethink:		RockClimber_PreThink,
	touched:		RockClimber_Touched,
	draw:			RockClimber_Draw,
	preview:		RockClimber_Preview,
	death:			RockClimber_Death,
	attacked:		RockClimber_Attacked
};

class_properties_t	ClassProperties_RockClimber_Red = {
	name:			"RockClimber (red)",
	size:			sizeof(e_rockclimber),
	init:			RockClimber_Init,
	prethink:		RockClimber_PreThink,
	touched:		RockClimber_Touched,
	draw:			RockClimber_Draw,
	preview:		RockClimber_Preview,
	death:			RockClimber_Death,
	attacked:		RockClimber_Attacked
};


