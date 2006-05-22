// C Source File
// Created 1/6/2004; 12:50:11 PM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"

void PowerUp_PreThink(entity e) {
	DEFINE_SELF(e_powerup);

	// Hidden?
	if (host->map->mapflags[host->gettileat(e->x, e->y)] & MAPFLAG_SOLID) return;

	if (self->killtime-- <= 0) {
		if (self->flags & FLAG_FLICKER) {
			// We already flickered?  Kill it.
			host->e_kill(e);
			return;
		} else {
			// Flicker for 60 frames...
			self->killtime = 60;
			self->flags |= FLAG_FLICKER;
		}
	}
}

/*
void PowerUp_Touched(entity e, entity o) {
	DEFINE_SELF(e_powerup);
}
*/

void PowerUp_Draw(entity e) {
	DEFINE_SELF(e_powerup);
	long	nx, ny;
	float	r, g, b;

	// Still hidden?  Don't display:
	if (host->map->mapflags[host->gettileat(e->x, e->y)] & MAPFLAG_SOLID) return;

	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Flicker on and off 4 frames
	if (self->flags & FLAG_FLICKER) self->flicker_count++;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count >= 8)) self->flicker_count -= 8;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count % 8 < 4)) return;

	r = (rand() % 32768) / 32768.0;
	g = (rand() % 32768) / 32768.0;
	b = (rand() % 32768) / 32768.0;

	switch (self->class) {
		case CLASS_POWERUP_POWER:
			host->put_sprite(nx	 , ny	 , 1, 0x06);
			host->put_sprite(nx	 , ny + 16, 1, 0x16);
			host->put_sprite(nx + 16, ny	 , 1, 0x07);
			host->put_sprite(nx + 16, ny + 16, 1, 0x17);
			break;
		case CLASS_POWERUP_POWER_FLASH:
			glColor4f(r, g, b, 1.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			host->put_sprite(nx	 , ny	 , 1, 0x06);
			host->put_sprite(nx	 , ny + 16, 1, 0x16);
			host->put_sprite(nx + 16, ny	 , 1, 0x07);
			host->put_sprite(nx + 16, ny + 16, 1, 0x17);
			glColor4f(0.0, 0.0, 0.0, 0.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			break;
		case CLASS_POWERUP_GUNS:
			host->put_sprite(nx	 , ny	 , 1, 0xA6);
			host->put_sprite(nx	 , ny + 16, 1, 0xB6);
			host->put_sprite(nx + 16, ny	 , 1, 0xA7);
			host->put_sprite(nx + 16, ny + 16, 1, 0xB7);
			break;
		case CLASS_POWERUP_GUNS_FLASH:
			glColor4f(r, g, b, 1.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			host->put_sprite(nx	 , ny	 , 1, 0xA6);
			host->put_sprite(nx	 , ny + 16, 1, 0xB6);
			host->put_sprite(nx + 16, ny	 , 1, 0xA7);
			host->put_sprite(nx + 16, ny + 16, 1, 0xB7);
			glColor4f(0.0, 0.0, 0.0, 0.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			break;
		case CLASS_POWERUP_HOVER:
			host->put_sprite(nx	 , ny	 , 1, 0x26);
			host->put_sprite(nx	 , ny + 16, 1, 0x36);
			host->put_sprite(nx + 16, ny	 , 1, 0x27);
			host->put_sprite(nx + 16, ny + 16, 1, 0x37);
			break;
		case CLASS_POWERUP_HOVER_FLASH:
			glColor4f(r, g, b, 1.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			host->put_sprite(nx	 , ny	 , 1, 0x26);
			host->put_sprite(nx	 , ny + 16, 1, 0x36);
			host->put_sprite(nx + 16, ny	 , 1, 0x27);
			host->put_sprite(nx + 16, ny + 16, 1, 0x37);
			glColor4f(0.0, 0.0, 0.0, 0.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			break;
		case CLASS_POWERUP_HOMING_MISSILES:
			host->put_sprite(nx	 , ny	 , 1, 0x46);
			host->put_sprite(nx	 , ny + 16, 1, 0x56);
			host->put_sprite(nx + 16, ny	 , 1, 0x47);
			host->put_sprite(nx + 16, ny + 16, 1, 0x57);
			break;
		case CLASS_POWERUP_BOLTS:
			host->put_sprite(nx	 , ny	 , 1, 0x86);
			host->put_sprite(nx	 , ny + 16, 1, 0x96);
			host->put_sprite(nx + 16, ny	 , 1, 0x87);
			host->put_sprite(nx + 16, ny + 16, 1, 0x97);
			break;
		case CLASS_POWERUP_TOMAHAWKS:
			host->put_sprite(nx	 , ny	 , 1, 0x66);
			host->put_sprite(nx	 , ny + 16, 1, 0x76);
			host->put_sprite(nx + 16, ny	 , 1, 0x67);
			host->put_sprite(nx + 16, ny + 16, 1, 0x77);
			break;
	}
};

void PowerUp_Preview(int nx, int ny, class_type class) {
	float	r, g, b;

	r = (rand() % 32768) / 32768.0;
	g = (rand() % 32768) / 32768.0;
	b = (rand() % 32768) / 32768.0;

	switch (class) {
		case CLASS_POWERUP_POWER:
			host->put_sprite(nx	, ny	, 1, 0x06);
			host->put_sprite(nx	, ny + 8, 1, 0x16);
			host->put_sprite(nx + 8, ny	, 1, 0x07);
			host->put_sprite(nx + 8, ny + 8, 1, 0x17);
			break;
		case CLASS_POWERUP_POWER_FLASH:
			glColor4f(r, g, b, 1.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			host->put_sprite(nx	, ny	, 1, 0x06);
			host->put_sprite(nx	, ny + 8, 1, 0x16);
			host->put_sprite(nx + 8, ny	, 1, 0x07);
			host->put_sprite(nx + 8, ny + 8, 1, 0x17);
			glColor4f(0.0, 0.0, 0.0, 0.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			break;
		case CLASS_POWERUP_GUNS:
			host->put_sprite(nx	, ny	, 1, 0xA6);
			host->put_sprite(nx	, ny + 8, 1, 0xB6);
			host->put_sprite(nx + 8, ny	, 1, 0xA7);
			host->put_sprite(nx + 8, ny + 8, 1, 0xB7);
			break;
		case CLASS_POWERUP_GUNS_FLASH:
			glColor4f(r, g, b, 1.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			host->put_sprite(nx	, ny	, 1, 0xA6);
			host->put_sprite(nx	, ny + 8, 1, 0xB6);
			host->put_sprite(nx + 8, ny	, 1, 0xA7);
			host->put_sprite(nx + 8, ny + 8, 1, 0xB7);
			glColor4f(0.0, 0.0, 0.0, 0.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			break;
		case CLASS_POWERUP_HOVER:
			host->put_sprite(nx	, ny	, 1, 0x26);
			host->put_sprite(nx	, ny + 8, 1, 0x36);
			host->put_sprite(nx + 8, ny	, 1, 0x27);
			host->put_sprite(nx + 8, ny + 8, 1, 0x37);
			break;
		case CLASS_POWERUP_HOVER_FLASH:
			glColor4f(r, g, b, 1.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			host->put_sprite(nx	, ny	, 1, 0x26);
			host->put_sprite(nx	, ny + 8, 1, 0x36);
			host->put_sprite(nx + 8, ny	, 1, 0x27);
			host->put_sprite(nx + 8, ny + 8, 1, 0x37);
			glColor4f(0.0, 0.0, 0.0, 0.0);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			break;
		case CLASS_POWERUP_HOMING_MISSILES:
			host->put_sprite(nx	, ny	, 1, 0x46);
			host->put_sprite(nx	, ny + 8, 1, 0x56);
			host->put_sprite(nx + 8, ny	, 1, 0x47);
			host->put_sprite(nx + 8, ny + 8, 1, 0x57);
			break;
		case CLASS_POWERUP_BOLTS:
			host->put_sprite(nx	, ny	, 1, 0x86);
			host->put_sprite(nx	, ny + 8, 1, 0x96);
			host->put_sprite(nx + 8, ny	, 1, 0x87);
			host->put_sprite(nx + 8, ny + 8, 1, 0x97);
			break;
		case CLASS_POWERUP_TOMAHAWKS:
			host->put_sprite(nx	, ny	, 1, 0x66);
			host->put_sprite(nx	, ny + 8, 1, 0x76);
			host->put_sprite(nx + 8, ny	, 1, 0x67);
			host->put_sprite(nx + 8, ny + 8, 1, 0x77);
			break;
	}
}

void PowerUp_Init(entity e) {
	DEFINE_SELF(e_powerup);

	// Set the position
	self->x = 0L;
	self->y = 0L;

	// Appearance:
	self->fr_time = 0;
	self->frame = 0;

	// Deltas:
	self->dx = 0L;
	self->dy = 0L;
	self->max_dx = 0L;
	self->accel = 0L;

	// 0-7, 0 = up, 2 = right, 4 = down, 6 = left w/ diagonals at 1, 3, 5, and 7.
	self->flags = FLAG_NOGRAVITY | FLAG_NOFRICTION;
	self->flags |= FLAG_ONSCREEN_ONLY;
	self->face_dir = 2;

	// Collision rectangle: (2 pixels inward from 16x16 box)
	self->ecrx1 =  4;	self->ecry1 =  4;
	self->ecrx2 = 28;	self->ecry2 = 28;

	self->mcrx1 =  0;	self->mcry1 =  0;
	self->mcrx2 = 31;	self->mcry2 = 31;

	self->killtime = 480;
};

// Different classes for the map-editor.  Ugly I know.
class_properties_t	ClassProperties_PowerUp_Power = {
	name:			"Powerup (P)",
	size:			sizeof(e_powerup),
	init:			PowerUp_Init,
	prethink:		PowerUp_PreThink,
	draw:			PowerUp_Draw,
	preview:		PowerUp_Preview,
};

class_properties_t	ClassProperties_PowerUp_Power_Flash = {
	name:			"Powerup (P*4)",
	size:			sizeof(e_powerup),
	init:			PowerUp_Init,
	prethink:		PowerUp_PreThink,
	draw:			PowerUp_Draw,
	preview:		PowerUp_Preview,
};

class_properties_t	ClassProperties_PowerUp_Guns = {
	name:			"Powerup (Guns)",
	size:			sizeof(e_powerup),
	init:			PowerUp_Init,
	prethink:		PowerUp_PreThink,
	draw:			PowerUp_Draw,
	preview:		PowerUp_Preview,
};

class_properties_t	ClassProperties_PowerUp_Guns_Flash = {
	name:			"Powerup (Guns*4)",
	size:			sizeof(e_powerup),
	init:			PowerUp_Init,
	prethink:		PowerUp_PreThink,
	draw:			PowerUp_Draw,
	preview:		PowerUp_Preview,
};

class_properties_t	ClassProperties_PowerUp_Hover = {
	name:			"Powerup (H)",
	size:			sizeof(e_powerup),
	init:			PowerUp_Init,
	prethink:		PowerUp_PreThink,
	draw:			PowerUp_Draw,
	preview:		PowerUp_Preview,
};

class_properties_t	ClassProperties_PowerUp_Hover_Flash = {
	name:			"Powerup (H*4)",
	size:			sizeof(e_powerup),
	init:			PowerUp_Init,
	prethink:		PowerUp_PreThink,
	draw:			PowerUp_Draw,
	preview:		PowerUp_Preview,
};

class_properties_t	ClassProperties_PowerUp_Homing_Missiles = {
	name:			"Powerup (Homing)",
	size:			sizeof(e_powerup),
	init:			PowerUp_Init,
	prethink:		PowerUp_PreThink,
	draw:			PowerUp_Draw,
	preview:		PowerUp_Preview,
};

class_properties_t	ClassProperties_PowerUp_Bolts = {
	name:			"Powerup (Bolts)",
	size:			sizeof(e_powerup),
	init:			PowerUp_Init,
	prethink:		PowerUp_PreThink,
	draw:			PowerUp_Draw,
	preview:		PowerUp_Preview,
};

class_properties_t	ClassProperties_PowerUp_Tomahawks = {
	name:			"Powerup (Tomahawks)",
	size:			sizeof(e_powerup),
	init:			PowerUp_Init,
	prethink:		PowerUp_PreThink,
	draw:			PowerUp_Draw,
	preview:		PowerUp_Preview,
};


