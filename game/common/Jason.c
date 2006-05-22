// C Source File
// Created 1/6/2004; 6:33:22 AM

#include <math.h>
#include <string.h>
#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

class_properties_t	ClassProperties_Jason = {
	size:		sizeof(e_jason),
	name:		"Jason",
	init:		Jason_Init,
	prethink:	Jason_PreThink,
	postthink:	Jason_PostThink,
	touched:	Jason_Touched,
	draw:		Jason_Draw,
	preview:	Jason_Preview
};

void Jason_SetCollisionTall(entity e) {
	// Collision rectangle:
	e->ecrx1 = 0;	e->ecry1 = -8;
	e->ecrx2 = 15;	e->ecry2 = 15;

	e->mcrx1 = 0;	e->mcry1 = -8;
	e->mcrx2 = 15;	e->mcry2 = 15;
};

void Jason_SetCollisionWide(entity e) {
	// Collision rectangle:
	e->ecrx1 = 0;	e->ecry1 = 0;
	e->ecrx2 = 15;	e->ecry2 = 15;

	e->mcrx1 = 0;	e->mcry1 = 0;
	e->mcrx2 = 15;	e->mcry2 = 15;
};

void Jason_Shoot(entity e) {
	DEFINE_SELF(e_jason);

	// Spawn a bullet:
	e_bullet *bullet = (e_bullet *) host->e_spawn(CLASS_BULLET);
	if (bullet != NULL) {
		// It's a small gun bullet:
		bullet->kind = BULLET_SMALL;
		bullet->owner = (entity) self;
		bullet->team = self->team;
		bullet->flags &= ~FLAG_TAKEDAMAGE;

		bullet->damage = 16;
		bullet->killtime = 40;

		bullet->face_dir = self->face_dir;

		// Position it correctly and give it some speed:
		if (self->flags & FLAG_CRAWLING) {
			bullet->y = self->y;
			if (self->face_dir == 2) {
				bullet->x = self->x + 16;
				bullet->dx = 8;
			}
			if (self->face_dir == 6) {
				bullet->x = self->x - 8;
				bullet->dx = -8;
			}
		} else {
			bullet->y = self->y;
			if (self->face_dir == 2) {
				bullet->x = self->x + 16;
				bullet->dx = 8;
			}
			if (self->face_dir == 6) {
				bullet->x = self->x;
				bullet->dx = -8;
			}
		}
		host->sndPlay(sounds.jason.shoot, SOUNDCHANNEL_PLAYER, 0);
	}
}

void Jason_EnableSwimming(entity e) {
	e->flags |= FLAG_SWIMMING;
	e->flags &= ~FLAG_CRAWLING;
	e->max_dx = 4.0;
	e->frame = 0;
	Jason_SetCollisionWide(e);
}

void Jason_DisableSwimming(entity e) {
	e->flags &= ~FLAG_SWIMMING;
	e->flags &= ~FLAG_CRAWLING;
	e->max_dx = 2.0;
	e->y = (long)(e->y * 0.0625) * 16.0;
	Jason_SetCollisionTall(e);
}

void Jason_EnableClimbing(entity e) {
	e->flags |= FLAG_CLIMBING;
	e->flags |= FLAG_NOGRAVITY;
	e->x = (long)(e->x * 0.03125) * 32.0;
	e->frame = 0;
	e->dx = 0; e->dy = 0;
}

void Jason_DisableClimbing(entity e) {
	e->flags &= ~FLAG_CLIMBING;
	e->flags &= ~FLAG_NOGRAVITY;
	e->x = e->x + 8;
}

// This code is executed while Jason is swimming:
void Jason_HandleSwimming(entity e, unsigned char m) {
	DEFINE_SELF(e_jason);

	self->dx -= (self->dx * 0.12);

	if (!(m & MAPFLAG_WATER)) {
		Jason_DisableSwimming(e);
		return;
	}

	// Maneuvering:
	if (self->control_keys & BUT_UP) {
		self->dy -= 2;
		if (self->dy < -4) self->dy = -4;
	}

	if (self->control_keys & BUT_DOWN) {
		self->dy += 1;
	}

	if (self->control_keys & BUT_LEFT) {
		self->dx -= 1;
		self->face_dir = 6;
	}

	if (self->control_keys & BUT_RIGHT) {
		self->dx += 1;
		self->face_dir = 2;
	}

	// Shooting:
	if ((self->control_keys & BUT_SHOOT) && !(self->old_control_keys & BUT_SHOOT))
		Jason_Shoot(e);


	// Is Jason over a ladder?
	if ((self->control_keys & BUT_JUMP) && (m & MAPFLAG_LADDER) && !(self->old_control_keys & BUT_JUMP)) {
		Jason_DisableSwimming(e);
		Jason_EnableClimbing(e);
	}

}

void Jason_HandleClimbing(entity e, unsigned char m) {
	DEFINE_SELF(e_jason);

	self->dy = 0;

	if (self->control_keys & BUT_UP) {
		if (m & MAPFLAG_LADDER) {
			self->dy = -1;
		} else {
			Jason_DisableClimbing(e);
		}
	}

	if (self->control_keys & BUT_DOWN) {
		if (host->map->mapflags[host->gettileat(self->x, self->y + 16)] & MAPFLAG_LADDER)
			self->dy = 1;
		else
			Jason_DisableClimbing(e);
	}

	if ((self->control_keys & BUT_JUMP) && !(self->old_control_keys & BUT_JUMP)) {
		Jason_DisableClimbing(e);
		self->dy = -4;
	}
}

void Jason_DoWarp(entity e) {
	DEFINE_SELF(e_jason);
	int	i, j;
	char	*filename;

	// Load the level and then position Jason:

	i = self->warp_door;
	// If we have to, load up a different level:
	if (host->map->doors[i]->targetmap != NULL) {
		filename = calloc(strlen(host->map->doors[i]->targetmap)+1, 1);
		strcpy(filename, host->map->doors[i]->targetmap);
		host->LoadLevel(filename);
		free(filename);
		// Search for the tag we have:
		for (j=0; j<host->map->num_doors; ++j)
			if (host->map->doors[j]->tag == self->warp_tag) {
				// Reappear at the new door in the NEW level:
				e->x = host->map->doors[j]->x * 32.0 + 20;
				e->y = host->map->doors[j]->y * 32.0 + 15;
				e->savex = e->x;
				e->savey = e->y;
				e->dx = e->dy = 0;
				*(host->screen_mx) = host->wrap_map_coord_x( e->x - *(host->screen_w) / 2 );
				*(host->screen_my) = host->wrap_map_coord_y( e->y - *(host->screen_h) / 2 );
				break;
			}
	} else {
		// Search in the same level for the _other_ tag:
		for (j=0; j<host->map->num_doors; ++j)
			if ((i != j) && (host->map->doors[j]->tag == self->warp_tag)) {
				// Reappear at the new door in the SAME level:
				e->x = host->map->doors[j]->x * 32.0 + 20;
				e->y = host->map->doors[j]->y * 32.0 + 15;
				// Save our entry coordinates:
				e->savex = e->x;
				e->savey = e->y;
				e->dx = e->dy = 0;
				*(host->screen_mx) = host->wrap_map_coord_x( e->x - *(host->screen_w) / 2 );
				*(host->screen_my) = host->wrap_map_coord_y( e->y - *(host->screen_h) / 2 );
				break;
			}
	}
	
	self->warp_downtime = 128;
}

void Jason_Warp(entity e) {
	DEFINE_SELF(e_jason);
	int		i;

	if (self->warp_downtime == 0) {
		// Go thru the list of doors and find this door.
		for (i=0; i<host->map->num_doors; ++i)
			if ( (host->map->doors[i]->x == (long)(e->x) >> 5) &&
				(host->map->doors[i]->y == (long)(e->y) >> 5) )
			{
				self->warp_tag = host->map->doors[i]->tag;
				self->warp_time = 0;
				self->warp_door = i;
				self->dx = self->dy = 0;
				self->flags |= FLAG_WARPING;
				host->sndPlay(sounds.tank.warp, SOUNDCHANNEL_PLAYER, 0);
				break;
			}
	}
}

void Jason_PreThink(entity e) {
	DEFINE_SELF(e_jason);
	// Grab the current mapflags on our current tile:  (very handy)
	int	m = host->map->mapflags[host->gettileat(e->x, e->y)];

	if (self->warp_downtime > 0) self->warp_downtime--;

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	}

	if (self->flags & FLAG_SWIMMING) Jason_HandleSwimming(e, m);
	else if (self->flags & FLAG_CLIMBING) Jason_HandleClimbing(e, m);
	else if (!(self->flags & FLAG_THRUDOOR)) {

		// Make him swim if he's in water and not on ground:
		if ((m & MAPFLAG_WATER) && !(self->flags & FLAG_ONGROUND))
			Jason_EnableSwimming(e);

		// (jump-key)
		if (self->control_keys & BUT_JUMP) {
			if (m & MAPFLAG_WATER) {
				Jason_EnableSwimming(e);
				self->dy -= 4.0;
			} else
			// In order to execute another jump, we must be on the ground,
			// not previously jumping, and have at least let go of the jump
			// button the last frame:
			if ((self->flags & FLAG_ONGROUND) && !(self->old_control_keys & BUT_JUMP)) {
				self->dy -= 8.5;
				// Don't crawl in the air...
				self->flags &= ~(FLAG_CRAWLING);
				Jason_SetCollisionTall(e);
				self->flags &= ~(FLAG_ONGROUND);
				self->flags |= FLAG_JUMPING;
			}
		}

		// If jumping and moving down, then jumping is off and we're falling:
		if ((self->flags & FLAG_JUMPING) && (self->dy >= 0)) {
			self->flags &= ~(FLAG_JUMPING);
		}

		// Left & Right keys:	(change direction first frame, move every frame after that)
		if (self->control_keys & BUT_LEFT) {
			if (self->flags & FLAG_ONGROUND) {
				if (self->old_control_keys & BUT_LEFT) self->dx -= self->accel * (host->map->friction * 8);
			} else {
				if (self->old_control_keys & BUT_LEFT) self->dx -= self->accel;
			}
			self->face_dir = 6;
		} else if (self->control_keys & BUT_RIGHT) {
			if (self->flags & FLAG_ONGROUND) {
				if (self->old_control_keys & BUT_RIGHT) self->dx += self->accel * (host->map->friction * 8);
			} else {
				if (self->old_control_keys & BUT_RIGHT) self->dx += self->accel;
			}
			self->face_dir = 2;
		}

		// Crawling and standing change with UP/DOWN buttons:
		if (self->control_keys & BUT_DOWN) {
			// Warping doors:
			if ((m & MAPFLAG_DOOR) && !(m & MAPFLAG_SOLID) && !(self->old_control_keys & BUT_DOWN)) {
				Jason_Warp(e);
				// Is Jason over a ladder?
			} else if ((m & MAPFLAG_LADDER) && !(self->old_control_keys & BUT_DOWN)) {
				e->flags &= ~FLAG_CRAWLING;
				Jason_EnableClimbing(e);
			} else if (!(self->flags & FLAG_CRAWLING) && (self->flags & FLAG_ONGROUND) &&
				!(self->old_control_keys & BUT_DOWN))
			{
				self->flags |= FLAG_CRAWLING;
				Jason_SetCollisionWide(e);
				self->dx = 0;
				self->frame = 0;
			}
		}

		// Up button:
		if (self->control_keys & BUT_UP) {
			if (m & MAPFLAG_WATER) {
				Jason_EnableSwimming(e);
				self->dy -= 4.0;
			} else

			// If crawling, get up:
			if (self->flags & FLAG_CRAWLING) {
				self->flags &= ~(FLAG_CRAWLING);
				Jason_SetCollisionTall(e);
				self->dx = 0;
				self->frame = 0;
			} else

			// Is Jason over a ladder?
			if ((m & MAPFLAG_LADDER) && !(self->old_control_keys & BUT_UP)) {
				self->flags |= FLAG_CLIMBING;
				self->flags |= FLAG_NOGRAVITY;
				self->flags &= ~FLAG_CRAWLING;
				self->x = (long)(self->x * 0.03125) * 32.0;
				self->frame = 0;
				self->dx = 0;
				self->dy = 0;
			}
		}

		// Shooting:
		if ((self->control_keys & BUT_SHOOT) && !(self->old_control_keys & BUT_SHOOT))
			Jason_Shoot(e);

		// Jumping into the tank:
		if ((self->control_keys & BUT_SWITCH) && !(self->old_control_keys & BUT_SWITCH)) {
			if (self->owner) {
				if ( (self->x >= self->owner->x - 16) &&
					(self->x <= self->owner->x + 16) &&
					(self->y >= self->owner->y - 16) &&
					(self->y <= self->owner->y + 16) &&
					(self->flags & FLAG_ONGROUND) &&
					(self->owner->flags & FLAG_ONGROUND) )
				{
					// Switch control to the tank:
					host->control_switch(self->owner);
					// HACK:  make the next player seem like the switch button was held down:
					(*(host->player))->old_control_keys = BUT_SWITCH;
					((e_tank *)(*(host->player)))->timer[TIMER_TANK_OPENHATCH] = 16;
					// TODO:  Make jumping into tank an animation:
					// Kill Jason:
					host->e_kill(e);
					return;
				}
			}
		}

		// Limit horizontal velocity if we're crawling:
		if (self->flags & FLAG_CRAWLING) {
			if (self->dx >  (self->max_dx * 0.5)) self->dx =  (self->max_dx * 0.5);
			if (self->dx < -(ABS(self->max_dx) * 0.5)) self->dx = -(ABS(self->max_dx) * 0.5);
		}
	}

	// Store the last keys pressed:
	self->old_control_keys = self->control_keys;
}

void Jason_PostThink(entity e) {
	DEFINE_SELF(e_jason);

	// Are we warping?
	if (self->flags & FLAG_WARPING) {
		self->warp_time++;
		if (self->warp_time == 16) {
			Jason_DoWarp(e);
		} else if (self->warp_time == 32) {
			self->flags &= ~FLAG_WARPING;
		}
	} else if (self->flags & FLAG_CLIMBING) {
		if (self->dy != 0) {
			self->fr_time += 1;
			if (self->fr_time > 10) {
				self->frame = (self->frame + 1) % 2;
				self->fr_time = 0;
			}
		}
	} else if (self->flags & FLAG_SWIMMING) {
		if ((self->dy != 0) || (self->dx != 0)) {
			self->fr_time += 1;
			if (self->fr_time > 10) {
				self->frame = (self->frame + 1) % 2;
				self->fr_time = 0;
			}
		}
	} else {
		if (ABS(self->dx) >= 0.125) {
			self->fr_time += 1;

			if (self->fr_time > 5) {
				if (self->flags & FLAG_CRAWLING) self->frame = (self->frame + 1) % 2;
				else self->frame = (self->frame + 1) % 4;
				self->fr_time = 0;
			}
		} else {
			self->fr_time = 0;
			self->frame = 0;
		}
	}

	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
}

void Jason_Touched(entity e, entity o) {
	DEFINE_SELF(e_jason);

	// Collide with the world:
	if (o == *(host->world)) {
		if (self->flags & FLAG_SWIMMING) {
			if (!(self->flags & FLAG_ONGROUND)) {
				self->x = self->cmx; self->y = self->cmy;
				return;
			} else {
				Jason_DisableSwimming(e);
				return;
			}
		}

		if (self->flags & FLAG_CLIMBING) {
			if (self->collide_mapflags & MAPFLAG_LADDER) {
				// Restore any changes made from the collision detection code:
				self->x  = self->cmx; self->y  = self->cmy;
				self->dx = self->cdx; self->dy = self->cdy;
				self->flags &= ~FLAG_ONGROUND;
			}
			return;
		}

		// If our downward velocity at the time of collision is high, damage Jason:
		if (self->cdy >= 10.5) host->e_damage(e, *(host->world), 128);
		if (self->cdy >= 10.0) host->e_damage(e, *(host->world), 64);
		if (self->cdy >=  9.5) host->e_damage(e, *(host->world), 64);
		if (self->cdy >=  9.0) host->e_damage(e, *(host->world), 32);

		// Are we traveling thru a door tile?
		if (self->flags & FLAG_THRUDOOR) {
			// It's not the SAME door we entered:
			self->flags &= ~FLAG_THRUDOOR;
			self->flags &= ~FLAG_NOFRICTION;
			self->flags &= ~FLAG_NOGRAVITY;
		} else if (self->collide_mapflags & MAPFLAG_DOOR) {
			// Don't go thru the door if we didn't move:
			if ((self->cdx != 0) && (self->flags & FLAG_ONGROUND)) {
				// Must hold left or right to enter a door:
				if ((self->collide_flags & COLLIDE_LEFT) && !(self->control_keys & BUT_LEFT)) return;
				if ((self->collide_flags & COLLIDE_RIGHT) && !(self->control_keys & BUT_RIGHT)) return;
				
				self->dy = 0;
				self->flags |= FLAG_THRUDOOR;
				self->flags |= FLAG_NOFRICTION;
				self->flags |= FLAG_NOGRAVITY;

				if (self->cdx > 0) { self->dx = self->max_dx; self->face_dir = 2; }
				if (self->cdx < 0) { self->dx = -self->max_dx; self->face_dir = 6; }
			}
		}
		return;
	}

	if ((o->team != e->team) && (o->class == CLASS_BULLET)) {
		host->sndPlay(sounds.generic.damage, SOUNDCHANNEL_PLAYER, 0);
		return;
	}

	// Cast the objects to their respectful types:
	e_tank		*tank = (e_tank *)(self->owner);

	if (tank == NULL) { host->e_kill(o); return; }

	// Process the powerup:
	switch (o->class) {
		case CLASS_POWERUP_POWER:
			self->health += 32;
			if (self->health > 255) self->health = 255;
			host->e_kill(o);
			break;
		case CLASS_POWERUP_POWER_FLASH:
			self->health += 128;
			if (self->health > 255) self->health = 255;
			host->e_kill(o);
			host->sndPlay(sounds.powerup.flashy, -1, 0);
			break;
		case CLASS_POWERUP_GUNS:
			tank->ammo.guns += 1;
			if (tank->ammo.guns > 8) tank->ammo.guns = 8;
			host->e_kill(o);
			break;
		case CLASS_POWERUP_GUNS_FLASH:
			tank->ammo.guns += 4;
			if (tank->ammo.guns > 8) tank->ammo.guns = 8;
			host->e_kill(o);
			host->sndPlay(sounds.powerup.flashy, -1, 0);
			break;
		case CLASS_POWERUP_HOVER:
			tank->hover += 32;
			if (tank->hover > 255) tank->hover = 255;
			host->e_kill(o);
			break;
		case CLASS_POWERUP_HOVER_FLASH:
			tank->hover += 128;
			if (tank->hover > 255) tank->hover = 255;
			host->e_kill(o);
			host->sndPlay(sounds.powerup.flashy, -1, 0);
			break;
		case CLASS_POWERUP_HOMING_MISSILES:
			tank->ammo.homing_missiles += 15;
			if (tank->ammo.homing_missiles > 99) tank->ammo.homing_missiles = 99;
			host->e_kill(o);
			host->sndPlay(sounds.powerup.weapon, -1, 0);
			break;
		case CLASS_POWERUP_BOLTS:
			tank->ammo.bolts += 15;
			if (tank->ammo.bolts > 99) tank->ammo.bolts = 99;
			host->e_kill(o);
			host->sndPlay(sounds.powerup.weapon, -1, 0);
			break;
		case CLASS_POWERUP_TOMAHAWKS:
			tank->ammo.tomahawks += 15;
			if (tank->ammo.tomahawks > 99) tank->ammo.tomahawks = 99;
			host->e_kill(o);
			host->sndPlay(sounds.powerup.weapon, -1, 0);
			break;
	}
}

void Jason_Draw(entity e) {
	DEFINE_SELF(e_jason);
	float	r, g, b;

	// all these fgsprites indicies were found using my 'NES Sprite Viewer'
	// program which displays NES CHR ROM in 16x16 sprite pages.

	long	nx, ny;

	// Flicker:
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

	if (self->flags & FLAG_WARPING) {
		if (self->warp_time <= 16)
			glColor4f(1.0, 1.0, 1.0, (16 - self->warp_time) / 16.0);
		else
			glColor4f(1.0, 1.0, 1.0, (self->warp_time - 16) / 16.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	if (self->flags & FLAG_SWIMMING) {
		if (self->face_dir == 6) {
			switch (self->frame) {
				case 0:
					host->put_sprite(nx - 8, ny - 16, 0, 0xA8);
					host->put_sprite(nx - 8, ny, 0, 0x88);
					host->put_sprite(nx + 8, ny, 0, 0x89);
					break;
				case 1:
					host->put_sprite(nx - 8, ny - 16, 0, 0xA8);
					host->put_sprite(nx - 8, ny, 0, 0x98);
					host->put_sprite(nx + 8, ny, 0, 0x99);
					break;
			}
		} else {
			switch (self->frame) {
				case 0:
					host->put_sprite_hflip(nx + 8, ny - 16, 0, 0xA8);
					host->put_sprite_hflip(nx - 8, ny, 0, 0x89);
					host->put_sprite_hflip(nx + 8, ny, 0, 0x88);
					break;
				case 1:
					host->put_sprite_hflip(nx + 8, ny - 16, 0, 0xA8);
					host->put_sprite_hflip(nx - 8, ny, 0, 0x99);
					host->put_sprite_hflip(nx + 8, ny, 0, 0x98);
					break;
			}
		}
	} else if (self->flags & FLAG_CLIMBING) {
		switch (self->frame) {
			case 0:
				host->put_sprite(nx +  0, ny - 16, 0, 0xA1);
				host->put_sprite(nx + 16, ny - 16, 0, 0xA2);
				host->put_sprite(nx +  0, ny	 , 0, 0xB1);
				host->put_sprite(nx + 16, ny	 , 0, 0xB2);
				break;
			case 1:
				host->put_sprite_hflip(nx +  0, ny - 16, 0, 0xA2);
				host->put_sprite_hflip(nx + 16, ny - 16, 0, 0xA1);
				host->put_sprite_hflip(nx +  0, ny	 , 0, 0xB2);
				host->put_sprite_hflip(nx + 16, ny	 , 0, 0xB1);
				break;
		}
	} else if (!(self->flags & FLAG_ONGROUND)) {
		// Jumping:
		if (self->face_dir == 6) {
			host->put_sprite(nx + 0, ny - 16, 0, 0x50);
			host->put_sprite(nx + 0, ny	 , 0, 0x60);
		} else {
			host->put_sprite_hflip(nx + 0, ny - 16, 0, 0x50);
			host->put_sprite_hflip(nx + 0, ny	 , 0, 0x60);
		}
	} else if ((ABS(self->dx) < 0.125) && !(self->flags & FLAG_CRAWLING)) {
		// Standing:
		if (self->face_dir == 6) {
			host->put_sprite(nx + 0, ny - 16, 0, 0xE4);
			host->put_sprite(nx + 0, ny	 , 0, 0xF4);
		} else {
			host->put_sprite_hflip(nx + 0, ny - 16, 0, 0xE4);
			host->put_sprite_hflip(nx + 0, ny	 , 0, 0xF4);
		}
	} else if ((ABS(self->dx) >= 0.125) && !(self->flags & FLAG_CRAWLING)) {
		if (self->face_dir == 6) {
			switch (self->frame) {
				case 0:
					host->put_sprite(nx + 0, ny - 16, 0, 0x50);
					host->put_sprite(nx + 0, ny	 , 0, 0x60);
					break;
				case 1:
					host->put_sprite(nx + 0, ny - 16, 0, 0x70);
					host->put_sprite(nx + 0, ny	 , 0, 0x80);
					break;
				case 2:
					host->put_sprite(nx + 0, ny - 16, 0, 0x50);
					host->put_sprite(nx + 0, ny	 , 0, 0x90);
					break;
				case 3:
					host->put_sprite(nx + 0, ny - 16, 0, 0x70);
					host->put_sprite(nx + 0, ny	 , 0, 0x80);
					break;
			}
		} else {
			switch (self->frame) {
				case 0:
					host->put_sprite_hflip(nx + 0, ny - 16, 0, 0x70);
					host->put_sprite_hflip(nx + 0, ny	 , 0, 0x80);
					break;
				case 1:
					host->put_sprite_hflip(nx + 0, ny - 16, 0, 0x50);
					host->put_sprite_hflip(nx + 0, ny	 , 0, 0x60);
					break;
				case 2:
					host->put_sprite_hflip(nx + 0, ny - 16, 0, 0x70);
					host->put_sprite_hflip(nx + 0, ny	 , 0, 0x80);
					break;
				case 3:
					host->put_sprite_hflip(nx + 0, ny - 16, 0, 0x50);
					host->put_sprite_hflip(nx + 0, ny	 , 0, 0x90);
					break;
			}
		}
	} else if (self->flags & FLAG_CRAWLING) {
		if (self->face_dir == 6) {
			switch (self->frame) {
				case 0:
					host->put_sprite(nx - 8, ny, 0, 0x62);
					host->put_sprite(nx + 8, ny, 0, 0x72);
					break;
				case 1:
					host->put_sprite(nx - 8, ny, 0, 0x82);
					host->put_sprite(nx + 8, ny, 0, 0x92);
					break;
			}
		} else {
			switch (self->frame) {
				case 0:
					host->put_sprite_hflip(nx - 8, ny, 0, 0x72);
					host->put_sprite_hflip(nx + 8, ny, 0, 0x62);
					break;
				case 1:
					host->put_sprite_hflip(nx - 8, ny, 0, 0x92);
					host->put_sprite_hflip(nx + 8, ny, 0, 0x82);
					break;
			}
		}
	}

	if (self->flags & FLAG_WARPING) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	if (self->flags & FLAG_FLICKER) {
		glColor4f(0.0, 0.0, 0.0, 0.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}


	// Draw the map over Jason:
	if (self->flags & FLAG_THRUDOOR) {
		long	x, y, nx1, nx2, ny1, ny2;
		fixed	tx, ty;
		int		m, mf;

		nx1 = (long)((nx + self->mcrx1) * 0.03125) - 1;
		nx2 = (long)((nx + self->mcrx2) * 0.03125) + 1;
		ny1 = (long)((ny + self->mcry1) * 0.03125) - 1;
		ny2 = (long)((ny + self->mcry2) * 0.03125) + 1;

		for (y=ny1;y<=ny2;++y) {
			ty = (y * 32.0 + *(host->old_screen_my));
			for (x=nx1;x<=nx2;++x) {
				tx = (x * 32.0 + *(host->old_screen_mx));
				m = host->gettileat(host->wrap_map_coord_x(tx), host->wrap_map_coord_y(ty));
				mf = host->map->mapflags[m];
				if ((mf & MAPFLAG_SOLID) && !(mf & MAPFLAG_DOOR))
					host->Draw2x2BGTile((x * 32.0) - fmod(*(host->old_screen_mx), 32),
										(y * 32.0) - fmod(*(host->old_screen_my), 32),
										m, 0);
			}
		}
	}
}

void Jason_Preview(int x, int y, class_type class) {
	host->put_sprite_hflip(x + 0, y - 8, 0, 0xE4);
	host->put_sprite_hflip(x + 0, y	, 0, 0xF4);
}

void Jason_Init(entity e) {
	DEFINE_SELF(e_jason);

	self->team = TEAM_FROGHUNTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0; self->dy = 0;
	self->max_dx = 2;
	self->accel = 1;

	self->health = 255;
	self->maxhealth = 255;

	Jason_SetCollisionTall(e);

	self->flags |= FLAG_CONTROLLED | FLAG_TAKEDAMAGE;
	self->dmg_time = 60;

	// Face right:
	self->face_dir = 2;
}

void Jason_Outdoor(entity e) {
	DEFINE_SELF(e_jason);
	
	// Transition into the outdoor "version" of Jason:
	if (e != NULL) e->flags &= ~FLAG_NOGRAVITY;
	self->max_dx = 2;
	self->accel = 1;

	Jason_SetCollisionTall(e);
}
