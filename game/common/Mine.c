// C Source File
// Created 1/6/2004; 9:45:02 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

class_properties_t	ClassProperties_Mine = {
	sizeof(e_mine),
	"Mine",
	Mine_Init,
	Mine_PreThink,
	NULL,
	Mine_Touched,
	Mine_Draw,
	Mine_Preview,
	Mine_Death
};

void Mine_PreThink(entity e) {
	DEFINE_SELF(e_mine);

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	self->fr_time++;

	// Do the blinky thing:
	if (self->fr_time == 80) host->sndPlay(sounds.mine.beep, -1, 0);
	if (self->fr_time > 80) {
		self->frame = 1;
	}
	if (self->fr_time >= 90) {
		self->frame = 0;
		// Limit frame range:
		self->fr_time = 0;
	}

	// Count the damage timer down:
	e->dmg_timer--;
	if (e->dmg_timer < 0) e->dmg_timer = 0;
}

void Mine_Death(entity e) {
	DEFINE_SELF(e_mine);
	int i;

	e_powerup *p;
	// If the odds are 1/4, spawn a powerup:
	if (rand() % 32768 > 24576) {
		p = (e_powerup *) host->e_spawn(CLASS_POWERUP_POWER);
		p->x = e->x;
		p->y = e->y;
	}

	// Explodes 3 - 5 random bullets out from the mine:
	for (i=0; i<rand()%3 + 3; ++i) {
		e_bullet *bullet = (e_bullet *) host->e_spawn(CLASS_BULLET);
		if (bullet) {
			bullet->owner = e;
			bullet->team = self->team;
			bullet->dx = ( ((long)rand() % 32768) - 16384 ) / 8192.0;
			bullet->dy = ( -((long)rand() % 32768) / 16384.0 );
			bullet->x = self->x - 8;
			bullet->y = self->y - 16;
			bullet->kind = BULLET_MEDIUM;
			bullet->flags &= ~(FLAG_NOGRAVITY);
			bullet->flags |= FLAG_BOUNCE | FLAG_NODESTROY;
			bullet->killtime = 140;
			bullet->damage = 32;
		}
	}

	host->sndPlay(sounds.generic.explode1, -1, 0);
	BecomeExplosion(e, BULLET_LARGE);
	return;
}

/*
void Mine_PostThink(entity e) {
	DEFINE_SELF(e_mine);
}
*/

void Mine_Touched(entity e, entity o) {
	DEFINE_SELF(e_mine);

	// Mines explode as long as the contactor takes damage.
	if ((o->flags & FLAG_TAKEDAMAGE) && (o->team != e->team)) {
		// Damage ourself as well as our attacker:
		host->e_damage((entity)self, o, 256);
		host->e_damage(o, (entity)self, 32);
	}
}

void Mine_Draw(entity e) {
	DEFINE_SELF(e_mine);
	long	nx, ny;

	// Flicker on and off 4 frames
	if (self->flags & FLAG_FLICKER) self->flicker_count++;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count >= 8)) self->flicker_count -= 8;
	if ((self->flags & FLAG_FLICKER) && (self->flicker_count % 8 < 4)) return;

	// Account for the 16.15 fixed format.
	nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
	ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));

	// Draw the Mine:
	switch (self->frame) {
		case 0:
			host->put_sprite(nx	 , ny + 16, 1, 0xAC);
			host->put_sprite(nx + 16, ny + 16, 1, 0xAD);
			break;
		case 1:
			host->put_sprite(nx	 , ny + 16, 1, 0x9C);
			host->put_sprite(nx + 16, ny + 16, 1, 0x9D);
			break;
	}
};

void Mine_Preview(int nx, int ny, class_type class) {
	host->put_sprite(nx	, ny + 8, 1, 0xAC);
	host->put_sprite(nx + 8, ny + 8, 1, 0xAD);
}

void Mine_Init(entity e) {
	DEFINE_SELF(e_mine);

	self->team = TEAM_MONSTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->max_dx = 0L;
	self->accel = 0L;
	self->frame = 0L;
	self->flags = FLAG_NOFRICTION | FLAG_NOGRAVITY | FLAG_TAKEDAMAGE;
	self->flags |= FLAG_ONSCREEN_ONLY;

	self->health = 8;
	self->maxhealth = 8;

	// Collision rectangle:
	self->ecrx1 =  0;	self->ecry1 =  20;
	self->ecrx2 = 31;	self->ecry2 =  31;

	self->mcrx1 =  0;	self->mcry1 =  20;
	self->mcrx2 = 31;	self->mcry2 =  32;

	self->dmg_time = 20;

	self->face_dir = 2;
};
