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
	name:		"Jason (Indoors)",
	init:		Jason_Init,
	prethink:	Jason_PreThink,
	postthink:	Jason_PostThink,
	touched:	Jason_Touched,
	draw:		Jason_Draw,
	preview:	Jason_Preview
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
		if (self->face_dir == 2) {
			bullet->x = self->x + 16;
			bullet->y = self->y - 16;
			bullet->dx = 8;
			bullet->dy = 0;
		} else if (self->face_dir == 6) {
			bullet->x = self->x - 16;
			bullet->y = self->y - 16;
			bullet->dx = -8;
			bullet->dy = 0;
		} else if (self->face_dir == 0) {
			bullet->x = self->x + 24;
			bullet->y = self->y - 20;
			bullet->dx = 0;
			bullet->dy = -8;
		} else if (self->face_dir == 4) {
			bullet->x = self->x - 8;
			bullet->y = self->y;
			bullet->dx = 0;
			bullet->dy = 8;
		}
		host->sndPlay(sounds.jason.shoot, SOUNDCHANNEL_PLAYER, 0);
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

	// Take down the warp "safe" time.
	if (self->warp_downtime > 0) self->warp_downtime--;
	
	// Initiate warp sequence: (not Star-Trek (TM))
	if ((m & MAPFLAG_ALL) == MAPFLAG_DOOR) {
		Jason_Warp(e);
	}
	
	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	}
	
	self->flags &= ~FLAG_ONGROUND;

	if (!(self->flags & FLAG_THRUDOOR)) {
		if (self->control_keys & BUT_LEFT) {
			if (self->old_control_keys & BUT_LEFT) self->dx -= self->accel * (host->map->friction * 8);
			self->face_dir = 6;
		} else if (self->control_keys & BUT_RIGHT) {
			if (self->old_control_keys & BUT_RIGHT) self->dx += self->accel * (host->map->friction * 8);
			self->face_dir = 2;
		}
		if (self->control_keys & BUT_UP) {
			if (self->old_control_keys & BUT_UP) self->dy -= self->accel * (host->map->friction * 8);
			self->face_dir = 0;
		} else if (self->control_keys & BUT_DOWN) {
			if (self->old_control_keys & BUT_DOWN) self->dy += self->accel * (host->map->friction * 8);
			self->face_dir = 4;
		}

		// Shooting:
		if ((self->control_keys & BUT_SHOOT) && !(self->old_control_keys & BUT_SHOOT))
			Jason_Shoot(e);
	
		if ((self->control_keys & BUT_LEFT) || (self->control_keys & BUT_RIGHT) ||
		    (self->control_keys & BUT_UP) || (self->control_keys & BUT_DOWN))
		{
			self->fr_time++;
		} else {
			self->frame = 0;
			self->fr_time = 0;
		}
	}
	
	// Increase walking frame if the time is right.
	if (self->fr_time >= 8) {
		self->fr_time = 0;
		self->frame++;
 		if (self->frame >= 4) self->frame = 0;
	}
	
	// Clip speed:
	if (self->dx < -self->max_dx) self->dx = -self->max_dx;
	if (self->dx > self->max_dx) self->dx = self->max_dx;
	if (self->dy < -self->max_dx) self->dy = -self->max_dx;
	if (self->dy > self->max_dx) self->dy = self->max_dx;

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
	}
	
	if (!(self->flags & FLAG_THRUDOOR) && !(self->flags & FLAG_NOFRICTION)) {
		self->dx *= host->map->friction;
		self->dy *= host->map->friction;
	}
	
	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
}

void Jason_Touched(entity e, entity o) {
	DEFINE_SELF(e_jason);

	// Collide with the world:
	if (o == *(host->world)) {
		// Are we traveling thru a door tile?
		if (self->flags & FLAG_THRUDOOR) {
			// It's not the SAME door we entered:
			self->flags &= ~FLAG_THRUDOOR;
			self->flags &= ~FLAG_NOFRICTION;
		} else if ((self->collide_mapflags & MAPFLAG_DOOR)) {
			// Don't go thru the door if we didn't move:
			if (self->cdx != 0) {
				// Must hold left or right to enter a door:
				if ((self->collide_flags & COLLIDE_LEFT)  && !(self->control_keys & BUT_LEFT))  return;
				if ((self->collide_flags & COLLIDE_RIGHT) && !(self->control_keys & BUT_RIGHT)) return;
				if ((self->collide_flags & COLLIDE_ABOVE) && !(self->control_keys & BUT_UP))    return;
				if ((self->collide_flags & COLLIDE_BELOW) && !(self->control_keys & BUT_DOWN))  return;
				
				self->dx = 0;
				self->dy = 0;
				self->flags |= FLAG_THRUDOOR;
				self->flags |= FLAG_NOFRICTION;

				if (self->collide_flags & COLLIDE_LEFT)  { self->dx = -self->max_dx; self->face_dir = 6; }
				if (self->collide_flags & COLLIDE_RIGHT) { self->dx =  self->max_dx; self->face_dir = 2; }
				if (self->collide_flags & COLLIDE_ABOVE) { self->dy = -self->max_dx; self->face_dir = 0; }
				if (self->collide_flags & COLLIDE_BELOW) { self->dy =  self->max_dx; self->face_dir = 4; }
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

	if (self->face_dir == 6) {
		// Face left:
		switch (self->frame) {
			case 0:
			case 2:
				host->put_sprite(nx - 16, ny - 48, 4, 0x03);
				host->put_sprite(nx     , ny - 48, 4, 0x04);
				host->put_sprite(nx + 16, ny - 48, 4, 0x05);
				host->put_sprite(nx - 16, ny - 32, 4, 0x13);
				host->put_sprite(nx     , ny - 32, 4, 0x14);
				host->put_sprite(nx + 16, ny - 32, 4, 0x15);
				host->put_sprite(nx - 16, ny - 16, 4, 0x23);
				host->put_sprite(nx     , ny - 16, 4, 0x24);
				host->put_sprite(nx + 16, ny - 16, 4, 0x25);
				host->put_sprite(nx - 16, ny     , 4, 0x33);
				host->put_sprite(nx     , ny     , 4, 0x34);
				host->put_sprite(nx + 16, ny     , 4, 0x35);
				break;
			case 1:
				host->put_sprite(nx - 16, ny - 46, 4, 0x03);
				host->put_sprite(nx     , ny - 46, 4, 0x04);
				host->put_sprite(nx + 16, ny - 46, 4, 0x05);
				host->put_sprite(nx - 16, ny - 30, 4, 0x13);
				host->put_sprite(nx     , ny - 30, 4, 0x14);
				host->put_sprite(nx + 16, ny - 30, 4, 0x15);
				host->put_sprite(nx - 16, ny - 14, 4, 0x43);
				host->put_sprite(nx     , ny - 14, 4, 0x44);
				host->put_sprite(nx + 16, ny - 14, 4, 0x45);
				host->put_sprite(nx - 16, ny +  2, 4, 0x53);
				host->put_sprite(nx     , ny +  2, 4, 0x54);
				host->put_sprite(nx + 16, ny +  2, 4, 0x55);
				break;
			case 3:
				host->put_sprite(nx - 16, ny - 46, 4, 0x03);
				host->put_sprite(nx     , ny - 46, 4, 0x04);
				host->put_sprite(nx + 16, ny - 46, 4, 0x05);
				host->put_sprite(nx - 16, ny - 30, 4, 0x13);
				host->put_sprite(nx     , ny - 30, 4, 0x14);
				host->put_sprite(nx + 16, ny - 30, 4, 0x15);
				host->put_sprite(nx - 16, ny - 14, 4, 0x63);
				host->put_sprite(nx     , ny - 14, 4, 0x64);
				host->put_sprite(nx + 16, ny - 14, 4, 0x65);
				host->put_sprite(nx - 16, ny +  2, 4, 0x73);
				host->put_sprite(nx     , ny +  2, 4, 0x74);
				host->put_sprite(nx + 16, ny +  2, 4, 0x75);
				break;
		}
	} else if (self->face_dir == 4) {
		// Face down:
		switch (self->frame) {
			case 0:
			case 2:
				host->put_sprite      (nx - 16, ny - 48, 4, 0x00);
				host->put_sprite      (nx     , ny - 48, 4, 0x01);
				host->put_sprite_hflip(nx + 16, ny - 48, 4, 0x00);
				host->put_sprite      (nx - 16, ny - 32, 4, 0x10);
				host->put_sprite      (nx     , ny - 32, 4, 0x11);
				host->put_sprite_hflip(nx + 16, ny - 32, 4, 0x10);
				host->put_sprite      (nx - 16, ny - 16, 4, 0x20);
				host->put_sprite      (nx     , ny - 16, 4, 0x21);
				host->put_sprite      (nx + 16, ny - 16, 4, 0x22);
				host->put_sprite      (nx - 16, ny     , 4, 0x30);
				host->put_sprite      (nx     , ny     , 4, 0x31);
				host->put_sprite      (nx + 16, ny     , 4, 0x32);
				break;
			case 1:
				host->put_sprite      (nx - 16, ny - 46, 4, 0x00);
				host->put_sprite      (nx     , ny - 46, 4, 0x01);
				host->put_sprite_hflip(nx + 16, ny - 46, 4, 0x00);
				host->put_sprite      (nx - 16, ny - 30, 4, 0x10);
				host->put_sprite      (nx     , ny - 30, 4, 0x11);
				host->put_sprite_hflip(nx + 16, ny - 30, 4, 0x10);
				host->put_sprite      (nx - 16, ny - 14, 4, 0x40);
				host->put_sprite      (nx     , ny - 14, 4, 0x41);
				host->put_sprite      (nx + 16, ny - 14, 4, 0x42);
				host->put_sprite      (nx - 16, ny +  2, 4, 0x50);
				host->put_sprite      (nx     , ny +  2, 4, 0x51);
				host->put_sprite      (nx + 16, ny +  2, 4, 0x52);
				break;
			case 3:
				host->put_sprite      (nx - 16, ny - 46, 4, 0x00);
				host->put_sprite      (nx     , ny - 46, 4, 0x01);
				host->put_sprite_hflip(nx + 16, ny - 46, 4, 0x00);
				host->put_sprite      (nx - 16, ny - 30, 4, 0x10);
				host->put_sprite      (nx     , ny - 30, 4, 0x11);
				host->put_sprite_hflip(nx + 16, ny - 30, 4, 0x10);
				host->put_sprite      (nx - 16, ny - 14, 4, 0x60);
				host->put_sprite      (nx     , ny - 14, 4, 0x61);
				host->put_sprite      (nx + 16, ny - 14, 4, 0x62);
				host->put_sprite      (nx - 16, ny +  2, 4, 0x70);
				host->put_sprite      (nx     , ny +  2, 4, 0x71);
				host->put_sprite      (nx + 16, ny +  2, 4, 0x72);
				break;
		}
	} else if (self->face_dir == 2) {
		// Face right:
		switch (self->frame) {
			case 0:
			case 2:
				host->put_sprite_hflip(nx - 16, ny - 48, 4, 0x05);
				host->put_sprite_hflip(nx     , ny - 48, 4, 0x04);
				host->put_sprite_hflip(nx + 16, ny - 48, 4, 0x03);
				host->put_sprite_hflip(nx - 16, ny - 32, 4, 0x15);
				host->put_sprite_hflip(nx     , ny - 32, 4, 0x14);
				host->put_sprite_hflip(nx + 16, ny - 32, 4, 0x13);
				host->put_sprite_hflip(nx - 16, ny - 16, 4, 0x25);
				host->put_sprite_hflip(nx     , ny - 16, 4, 0x24);
				host->put_sprite_hflip(nx + 16, ny - 16, 4, 0x23);
				host->put_sprite_hflip(nx - 16, ny     , 4, 0x35);
				host->put_sprite_hflip(nx     , ny     , 4, 0x34);
				host->put_sprite_hflip(nx + 16, ny     , 4, 0x33);
				break;
			case 1:
				host->put_sprite_hflip(nx - 16, ny - 46, 4, 0x05);
				host->put_sprite_hflip(nx     , ny - 46, 4, 0x04);
				host->put_sprite_hflip(nx + 16, ny - 46, 4, 0x03);
				host->put_sprite_hflip(nx - 16, ny - 30, 4, 0x15);
				host->put_sprite_hflip(nx     , ny - 30, 4, 0x14);
				host->put_sprite_hflip(nx + 16, ny - 30, 4, 0x13);
				host->put_sprite_hflip(nx - 16, ny - 14, 4, 0x45);
				host->put_sprite_hflip(nx     , ny - 14, 4, 0x44);
				host->put_sprite_hflip(nx + 16, ny - 14, 4, 0x43);
				host->put_sprite_hflip(nx - 16, ny +  2, 4, 0x55);
				host->put_sprite_hflip(nx     , ny +  2, 4, 0x54);
				host->put_sprite_hflip(nx + 16, ny +  2, 4, 0x53);
				break;
			case 3:
				host->put_sprite_hflip(nx - 16, ny - 46, 4, 0x05);
				host->put_sprite_hflip(nx     , ny - 46, 4, 0x04);
				host->put_sprite_hflip(nx + 16, ny - 46, 4, 0x03);
				host->put_sprite_hflip(nx - 16, ny - 30, 4, 0x15);
				host->put_sprite_hflip(nx     , ny - 30, 4, 0x14);
				host->put_sprite_hflip(nx + 16, ny - 30, 4, 0x13);
				host->put_sprite_hflip(nx - 16, ny - 14, 4, 0x65);
				host->put_sprite_hflip(nx     , ny - 14, 4, 0x64);
				host->put_sprite_hflip(nx + 16, ny - 14, 4, 0x63);
				host->put_sprite_hflip(nx - 16, ny +  2, 4, 0x75);
				host->put_sprite_hflip(nx     , ny +  2, 4, 0x74);
				host->put_sprite_hflip(nx + 16, ny +  2, 4, 0x73);
				break;
		}
	} else if (self->face_dir == 0) {
		// Face up:
		switch (self->frame) {
			case 0:
			case 2:
				host->put_sprite      (nx - 16, ny - 48, 4, 0x80);
				host->put_sprite      (nx     , ny - 48, 4, 0x81);
				host->put_sprite      (nx + 16, ny - 48, 4, 0x82);
				host->put_sprite      (nx - 16, ny - 32, 4, 0x90);
				host->put_sprite      (nx     , ny - 32, 4, 0x91);
				host->put_sprite      (nx + 16, ny - 32, 4, 0x92);
				host->put_sprite      (nx - 16, ny - 16, 4, 0xA0);
				host->put_sprite      (nx     , ny - 16, 4, 0xA1);
				host->put_sprite_hflip(nx + 16, ny - 16, 4, 0xA0);
				host->put_sprite      (nx - 16, ny     , 4, 0xB0);
				host->put_sprite      (nx     , ny     , 4, 0xB1);
				host->put_sprite_hflip(nx + 16, ny     , 4, 0xB0);
				break;
			case 1:
				host->put_sprite      (nx - 16, ny - 46, 4, 0x80);
				host->put_sprite      (nx     , ny - 46, 4, 0x81);
				host->put_sprite      (nx + 16, ny - 46, 4, 0x82);
				host->put_sprite      (nx - 16, ny - 30, 4, 0x90);
				host->put_sprite      (nx     , ny - 30, 4, 0x91);
				host->put_sprite      (nx + 16, ny - 30, 4, 0x92);
				host->put_sprite      (nx - 16, ny - 14, 4, 0xC0);
				host->put_sprite      (nx     , ny - 14, 4, 0xC1);
				host->put_sprite      (nx + 16, ny - 14, 4, 0xC2);
				host->put_sprite      (nx - 16, ny +  2, 4, 0xD0);
				host->put_sprite      (nx     , ny +  2, 4, 0xD1);
				host->put_sprite      (nx + 16, ny +  2, 4, 0xD2);
				break;
			case 3:
				host->put_sprite      (nx - 16, ny - 46, 4, 0x80);
				host->put_sprite      (nx     , ny - 46, 4, 0x81);
				host->put_sprite      (nx + 16, ny - 46, 4, 0x82);
				host->put_sprite      (nx - 16, ny - 30, 4, 0x90);
				host->put_sprite      (nx     , ny - 30, 4, 0x91);
				host->put_sprite      (nx + 16, ny - 30, 4, 0x92);
				host->put_sprite      (nx - 16, ny - 14, 4, 0xC0);
				host->put_sprite      (nx     , ny - 14, 4, 0xC1);
				host->put_sprite      (nx + 16, ny - 14, 4, 0xC2);
				host->put_sprite_hflip(nx - 16, ny +  2, 4, 0xD2);
				host->put_sprite_hflip(nx     , ny +  2, 4, 0xD1);
				host->put_sprite_hflip(nx + 16, ny +  2, 4, 0xD0);
				break;
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

void Jason_Preview(int nx, int ny, class_type class) {
	host->put_sprite(nx - 8, ny - 24, 4, 0x00);
	host->put_sprite(nx    , ny - 24, 4, 0x01);
	host->put_sprite_hflip(nx + 8, ny - 24, 4, 0x00);
	host->put_sprite(nx - 8, ny - 16, 4, 0x10);
	host->put_sprite(nx    , ny - 16, 4, 0x11);
	host->put_sprite_hflip(nx + 8, ny - 16, 4, 0x10);
	host->put_sprite(nx - 8, ny - 8, 4, 0x20);
	host->put_sprite(nx    , ny - 8, 4, 0x21);
	host->put_sprite(nx + 8, ny - 8, 4, 0x22);
	host->put_sprite(nx - 8, ny    , 4, 0x30);
	host->put_sprite(nx    , ny    , 4, 0x31);
	host->put_sprite(nx + 8, ny    , 4, 0x32);
}

void Jason_Init(entity e) {
	DEFINE_SELF(e_jason);

	self->team = TEAM_FROGHUNTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0; self->dy = 0;
	self->max_dx = 6;
	self->accel = 3;

	self->health = 255;
	self->maxhealth = 255;

	self->flags |= FLAG_CONTROLLED | FLAG_TAKEDAMAGE | FLAG_NOGRAVITY;
	self->dmg_time = 60;

	// Face right:
	self->face_dir = 2;
}

void Jason_Indoor(entity e) {
	DEFINE_SELF(e_jason);

	// Transition into the indoor "version" of Jason:
	self->flags |= FLAG_NOGRAVITY;
	
	self->max_dx = 3;
	self->accel = 2;
	
	self->ecrx1 = -16;	self->ecry1 = 0;
	self->ecrx2 =  31;	self->ecry2 = 15;

	self->mcrx1 = -16;	self->mcry1 = 0;
	self->mcrx2 =  31;	self->mcry2 = 15;
}
