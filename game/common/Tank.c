// C Source File
// Created 6/28/2002; 5:25:10 PM

// #define CHEATING in order to have infinite hover and a better gun
#define CHEATING

#include <math.h>
#include <string.h>
#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "interface.h"
#include "sounds.h"

int Tank_Attacked(entity e, entity o, long damage) {
	// Give a little kick:
	if (!(e->flags & FLAG_NOFRICTION)) {
		if (e->face_dir == 2) {
			e->dx = -(0.65 * damage * host->map->friction);
		} else if (e->face_dir == 6) {
			e->dx = (0.65 * damage * host->map->friction);
		}
	}

	// Flicker on and off:
	e->flicker_time = e->dmg_time;
	e->flags |= FLAG_FLICKER;

	// Okay to accept damage:
	return -1;
}

void Tank_Death(entity e) {
	DEFINE_SELF(e_tank);

	// Turn on the DEAD flag:
	self->flags |= FLAG_DEAD;
	self->flags |= FLAG_NOGRAVITY;
	self->flags |= FLAG_NOFRICTION;
	self->frame = 0;
	self->dx = self->dy = 0;

	// Where's the Earth-shattering Kaboom?
	host->sndPlay(sounds.tank.explode, SOUNDCHANNEL_PLAYER, 0);
}

void Tank_DoWarp(entity e) {
	DEFINE_SELF(e_tank);
	int	i, j;
	int	same = 0;
	char	*filename;

	// Load the level and then position the tank:

	i = self->warp_door;
	// If we have to, load up a different level:
	if (host->map->doors[i]->targetmap != NULL) {
		if (strcmp(host->map->filename, host->map->doors[i]->targetmap) == 0) {
			same = -1;
		} else {
			filename = calloc(strlen(host->map->doors[i]->targetmap)+1, 1);
			strcpy(filename, host->map->doors[i]->targetmap);
			host->LoadLevel(filename);
			free(filename);
		}
	} else {
		same = -1;
	}
	if (same) {
		// Search in the same level for the _other_ tag:
		for (j=0; j<host->map->num_doors; ++j)
			if ((i != j) && (host->map->doors[j]->tag == self->warp_tag)) {
				e->x = host->map->doors[j]->x * 32.0 + 8;
				e->y = host->map->doors[j]->y * 32.0 + 15;
				if (host->map->mapflags[host->gettileat(e->x + e->mcrx1, e->y)] & MAPFLAG_SOLID)
					e->x -= e->mcrx1;
				else if (host->map->mapflags[host->gettileat(e->x + e->mcrx2, e->y)] & MAPFLAG_SOLID)
					e->x -= e->mcrx2;
				e->savex = e->x;
				e->savey = e->y;
				e->dx = e->dy = 0;
				*(host->screen_mx) = host->wrap_map_coord_x( e->x - *(host->screen_w) / 2 );
				*(host->screen_my) = host->wrap_map_coord_y( e->y - *(host->screen_h) / 2 );
				break;
			}
	} else {
		// Search for the tag we have:
		for (j=0; j<host->map->num_doors; ++j)
			if (host->map->doors[j]->tag == self->warp_tag) {
				e->x = host->map->doors[j]->x * 32.0 + 8;
				e->y = host->map->doors[j]->y * 32.0 + 15;
				if (host->map->mapflags[host->gettileat(e->x + e->mcrx1, e->y)] & MAPFLAG_SOLID)
					e->x -= e->mcrx1;
				else if (host->map->mapflags[host->gettileat(e->x + e->mcrx2, e->y)] & MAPFLAG_SOLID)
					e->x -= e->mcrx2;
				e->savex = e->x;
				e->savey = e->y;
				e->dx = e->dy = 0;
				*(host->screen_mx) = host->wrap_map_coord_x( e->x - *(host->screen_w) / 2 );
				*(host->screen_my) = host->wrap_map_coord_y( e->y - *(host->screen_h) / 2 );
				break;
			}
	}
}

void Tank_Warp(entity e) {
	DEFINE_SELF(e_tank);
	int		i;

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

void Tank_Shoot_Special(entity e) {
	DEFINE_SELF(e_tank);
	double	vx, vy, len, closeness;
	entity	closest;

	// No ammo left?
	if (self->ammo.homing_missiles <= 0) return;
	
	// Spawn a bullet:
	e_bullet *bullet = (e_bullet *) host->e_spawn(CLASS_BULLET);
	if (bullet != NULL) {
		bullet->owner = e;
		bullet->team = self->team;
		bullet->kind = BULLET_HOMING_MISSILE;
		bullet->flags |= FLAG_NOMAPCOLLISION;
		bullet->damage = 64;
		bullet->target = NULL;
		// Position it correctly and give it some speed:
		bullet->y = self->y - 16;
		bullet->face_dir = self->face_dir;
		if (self->face_dir == 2) {
			bullet->x = self->x + 24;
			bullet->dx = 12;
		}
		if (self->face_dir == 6) {
			bullet->x = self->x - 24;
			bullet->dx = -12;
		}
		// Find a target:
		closeness = 384;
		closest = NULL;
		bullet->target = host->e_findfirst();
		while (bullet->target != NULL) {
			if ((bullet->target->team != self->team) && (bullet->target->flags & FLAG_TAKEDAMAGE)) {
				// Now, home in on the target:
				vx = bullet->target->x - self->x;
				vy = bullet->target->y - self->y;

				// If the difference is more than half the map, then move the other direction:
				if (vx <= -(host->map->width << 4)) vx += (host->map->width << 5);
				else if (vx >= (host->map->width << 4)) vx -= (host->map->width << 5);
				if (vy <= -(host->map->height << 4)) vy += (host->map->height << 5);
				else if (vy >= (host->map->height << 4)) vy -= (host->map->height << 5);

				len = sqrt((vx * vx) + (vy * vy));
				if (len < closeness) {
					closeness = len;
					closest = bullet->target;
				}
			}
			bullet->target = host->e_findnext();
		}

		bullet->target = closest;
		if (!bullet->target) {
			host->e_kill( (entity)bullet );
			return;
		}

		// Now, we have a target:
		self->ammo.homing_missiles--;
	}
}

void Tank_PreThink(entity e) {
	DEFINE_SELF(e_tank);
	int i;
	const bullet_type weapon_bullets[3] = { BULLET_CANNON, BULLET_HYPER, BULLET_CRUSHER };
	// Grab the current mapflags on our current tile:  (very handy)
	int	m = host->map->mapflags[host->gettileat(e->x, e->y)];

	// Not much to do if we're dead:
	if (self->flags & FLAG_DEAD) return;
	if (self->flags & FLAG_WARPING) return;

	// --- Frame counters:
	for (i = 0; i < TIMER_TANKS; ++i)
		if (self->timer[i] > 0) self->timer[i]--;

	// Flicker timer:
	if (self->flicker_time > 0) {
		self->flicker_time--;
		if (self->flicker_time == 0) self->flags &= ~(FLAG_FLICKER);
	};

	// When traveling thru a door, the player has nothing to do but wait.
	if (!(self->flags & FLAG_THRUDOOR)) {
		// Up button:  look up
		if ((self->control_keys & BUT_UP) && !(self->old_control_keys & BUT_UP)) {
			self->flags |= FLAG_LOOKUP;
			self->lookup_frame = 0;
			self->timer[TIMER_TANK_LOOKUP] = 2;
		}
		// Looking Up animation timer:
		// Holding UP, increase frame:
		if ((self->control_keys & BUT_UP) && (self->flags & FLAG_LOOKUP)) {
			if (self->timer[TIMER_TANK_LOOKUP] == 0) {
				self->lookup_frame++;
				if (self->lookup_frame > 4) self->lookup_frame = 4;
				self->timer[TIMER_TANK_LOOKUP] = 2;
			}
		}
		// Released UP, decrease frame:
		if (!(self->control_keys & BUT_UP) && (self->flags & FLAG_LOOKUP)) {
			if (self->timer[TIMER_TANK_LOOKUP] == 0) {
				self->lookup_frame--;
				if (self->lookup_frame < 0) {
					// Got back to the first frame? Then not looking up anymore:
					self->lookup_frame = 0;
					self->flags &= ~(FLAG_LOOKUP);
				}
				self->timer[TIMER_TANK_LOOKUP] = 2;
			}
		}

		// Jump-key (jump-key)
		if (self->control_keys & BUT_JUMP) {
			// Button hold and we're in the air on the way up?
			// The longer the button is held, the higher we go.
			// The flag is used so that one cannot partially "levitate"
			// by jumping, releasing the button, then holding it again.
			if (self->flags & FLAG_SUPERJUMP) self->dy -= 0.25;

			// In order to execute another jump, we must be on the ground,
			// not previously jumping, and have at least let go of the jump
			// button the last frame:
			if ((self->flags & FLAG_ONGROUND) && !(self->old_control_keys & BUT_JUMP)) {
				host->sndPlay(sounds.tank.jump, SOUNDCHANNEL_PLAYER, 0);
				self->dy -= 8.5;
				self->flags &= ~(FLAG_ONGROUND);
				self->flags |= FLAG_JUMPING;
				// Only allow super-jumping (holding the button to jump higher) at the start of the jump.
				self->flags |= FLAG_SUPERJUMP;
			} else if (!(self->flags & FLAG_ONGROUND) && !(self->old_control_keys & BUT_JUMP)) {
				self->flags |= FLAG_HOVER;
				self->timer[TIMER_TANK_HOVERSWITCH] = 8;
				self->flags &= ~(FLAG_JUMPING);
			}
		}

		// Hovering enabled, holding down the button, and have enough hover?
#ifdef CHEATING
		if ((self->flags & FLAG_HOVER) && (self->control_keys & BUT_JUMP)) {
#else
		if ((self->flags & FLAG_HOVER) && (self->control_keys & BUT_JUMP) && (self->hover > 0) && !(m & MAPFLAG_WATER)) {
#endif
			
			self->dy -= 0.8;
			
#ifndef CHEATING
			self->hover -= 2;
#endif
			
			// Loop the hover sound:
			if (!host->sndIsPlaying(SOUNDCHANNEL_HOVER)) host->sndPlay(sounds.tank.hover, SOUNDCHANNEL_HOVER, -1);
			
			if (self->dy < -3.0) self->dy = -3.0;
			if (self->timer[TIMER_TANK_HOVERSWITCH] == 0) {
				self->fr_time++;
				// Alternate frames for hover
				if (self->fr_time >= 3) {
					self->fr_time = 0;
					if (self->frame != 1) self->frame = 1; else self->frame = 2;
				}
			}
			
		} else if (self->flags & FLAG_HOVER) {
			
			self->frame = 0;
			if (host->sndIsPlaying(SOUNDCHANNEL_HOVER)) host->sndStop(SOUNDCHANNEL_HOVER);
			
		}

		// If we let go of the jump button while super-jumping then turn off the SUPERJUMP flag:
		if (!(self->control_keys & BUT_JUMP) && (self->flags & FLAG_SUPERJUMP))
			self->flags &= ~(FLAG_SUPERJUMP);

		// If jumping and moving down, then jumping is off and we're falling:
		if ((self->flags & FLAG_JUMPING) && (self->dy >= 0)) {
			self->flags &= ~(FLAG_JUMPING);
			self->flags &= ~(FLAG_SUPERJUMP);
		}

		// Left & Right keys:
		if (self->control_keys & BUT_LEFT) {
			if (self->flags & FLAG_ONGROUND) {
				if (!(self->flags & FLAG_TURNING)) self->dx -= self->accel * (host->map->friction * 8) * *(host->accel_scale);
			} else {
				if (!(self->flags & FLAG_TURNING)) self->dx -= self->accel * *(host->accel_scale);
			}
			if (!(self->old_control_keys & BUT_LEFT) && (self->face_dir == 2)) {
				self->timer[TIMER_TANK_TURN] = 2;
				self->face_dir = 8;
				self->turn_dir = 6;
				self->flags |= FLAG_TURNING;
			}
		}
		if (self->control_keys & BUT_RIGHT) {
			if (self->flags & FLAG_ONGROUND) {
				if (!(self->flags & FLAG_TURNING)) self->dx += self->accel * (host->map->friction * 8) * *(host->accel_scale);
			} else {
				if (!(self->flags & FLAG_TURNING)) self->dx += self->accel * *(host->accel_scale);
			}
			if (!(self->old_control_keys & BUT_RIGHT) && (self->face_dir == 6)) {
				self->timer[TIMER_TANK_TURN] = 2;
				self->face_dir = 8;
				self->turn_dir = 2;
				self->flags |= FLAG_TURNING;
			}
		}

		// Shooting:
		if ((self->control_keys & BUT_SHOOT) && !(self->old_control_keys & BUT_SHOOT) &&
			(self->face_dir != 8))
		{
			// Down+Shoot = special:
			if (self->control_keys & BUT_DOWN) {
				Tank_Shoot_Special(e);
			} else {
				// Spawn a bullet:
				e_bullet *bullet = (e_bullet *) host->e_spawn(CLASS_BULLET);
				if (bullet != NULL) {
					// The bullet type depends on what kind of weapon is selected:
					bullet->kind = weapon_bullets[self->weapon];
					bullet->owner = (entity) self;
					bullet->team = self->team;
					bullet->flags &= ~FLAG_TAKEDAMAGE;

					switch (bullet->kind) {
						case BULLET_CANNON:
							bullet->damage = 16;
							host->sndPlay(sounds.tank.cannon, SOUNDCHANNEL_PLAYER, 0);
							break;
						case BULLET_HYPER:
							bullet->damage = 32;
							host->sndPlay(sounds.tank.hyper, SOUNDCHANNEL_PLAYER, 0);
							break;
						case BULLET_CRUSHER:
							bullet->damage = 64;
							host->sndPlay(sounds.tank.hyper, SOUNDCHANNEL_PLAYER, 0);
							break;
						default: break;
					}

					// Shoot upwords:
					if (self->flags & FLAG_LOOKUP) {
						bullet->y = self->y - 12;
						if (self->face_dir == 2) {
							bullet->x = self->x - 8;
						} else if (self->face_dir == 6) {
							bullet->x = self->x + 8;
						}
						bullet->face_dir = 0;
						bullet->dx = 0;
						bullet->dy = -12;
						bullet->killtime = 120;
					} else {
						// Position it correctly and give it some speed:
						bullet->y = self->y - 16;
						bullet->face_dir = self->face_dir;
						if (self->face_dir == 2) {
							bullet->x = self->x + 24;
							bullet->dx = 12;
						}
						if (self->face_dir == 6) {
							bullet->x = self->x - 24;
							bullet->dx = -12;
						}
					}
				}
			}
		}

		// Switch control to Jason: (spawn him first)
		if ((self->control_keys & BUT_SWITCH) && !(self->old_control_keys & BUT_SWITCH)) {
			// Spawn Jason:
			e_jason *jason  = (e_jason *) host->e_spawn(CLASS_JASON);
			// This MUST be true for powerup collection to work!!!
			jason->owner = (entity)self;

			// Set up him right above the tank to 'hop out':
			jason->x = self->x;
			jason->y = self->y - 24;

			jason->dx = 0;
			jason->dy = -4;
			jason->face_dir = self->face_dir;
			jason->items = self->items;

			self->timer[TIMER_TANK_OPENHATCH] = 16;
			host->sndPlay(sounds.jason.enter_exit, SOUNDCHANNEL_PLAYER, 0);

			// Switch control over to Jason:
			host->control_switch( (entity)jason );
			return;
		}

		// Warping doors:
		if ((self->control_keys & BUT_DOWN) && !(self->old_control_keys & BUT_DOWN))
			if ((m & MAPFLAG_DOOR) && !(m & MAPFLAG_JASON) && !(m & MAPFLAG_SOLID))
				Tank_Warp(e);
	}

	// Stop hover noise if hover stops:
	if (host->sndIsPlaying(SOUNDCHANNEL_HOVER) && !(self->flags & FLAG_HOVER))
 		host->sndStop(SOUNDCHANNEL_HOVER);

	// Store the last keys pressed:
	self->old_control_keys = self->control_keys;
}

void Tank_PostThink(entity e) {
	DEFINE_SELF(e_tank);

	// Are we warping?
	if (self->flags & FLAG_WARPING) {
		self->warp_time++;
		if (self->warp_time == 16) {
			Tank_DoWarp(e);
		} else if (self->warp_time == 32) {
			self->flags &= ~FLAG_WARPING;
		}
	// Are we dead?
	} else if (self->flags & FLAG_DEAD) {
		// We are dead, so we're animating an explosion here:
		// Then, after 1 second (60 frames) are up, we reset back to healthy
		// status and move the tank to a saved position.

		self->fr_time++;
		// Increase explosion frame every 4 frames:
		if (self->fr_time >= 4) {
			self->fr_time = 0;
			self->frame++;
			// 1 second passed?  Reset to alive status, and move back to
			// a previously saved position (door entry):
			if (self->frame == 60) {
				self->flags &= ~FLAG_DEAD;
				self->health = 255;
				self->frame = 0;
				self->x = self->savex;
				self->y = self->savey;
				self->flags &= ~FLAG_NOGRAVITY;
				self->flags &= ~FLAG_NOFRICTION;
			}
		}
	} else if (self->flags & FLAG_HOVER) {
		// Hovering...

		// Just don't spin the wheels or shake the body.

		// Count the damage timer down:
		if (e->dmg_timer > 0) e->dmg_timer--;
	} else {
		// Nope, alive 'n healthy!
		
		if (!(self->control_keys & BUT_LEFT) && !(self->control_keys & BUT_RIGHT)) {
			self->shake = 0;
		}

		// Is it time to roll wheels?
		if ((self->control_keys & BUT_LEFT) || (self->control_keys & BUT_RIGHT) || (self->flags & FLAG_THRUDOOR)) {
			self->fr_time += 1;
			if (self->flags & FLAG_ONGROUND) {
				if (self->fr_time >= (int)( 4 * (3 - *(host->accel_scale) * 2)) ) {
					if (self->timer[TIMER_TANK_SHAKE] == 0) {
						self->shake = (self->shake == 0 ? 2 : 0);
						self->timer[TIMER_TANK_SHAKE] = 8;
					}
					if (self->face_dir == 6) {
						if (self->frame == 0) self->frame = 4;
						self->frame = self->frame - 1;
					} else if (self->face_dir == 2) {
						self->frame = (self->frame + 1) % 4;
					}
					// Limit frame range:
					self->fr_time = 0;
				}
			} else {
				// Wheels spin twice as fast when not on the ground:
				if (self->fr_time >= 2) {
					if (self->face_dir == 6) {
						if (self->frame == 0) self->frame = 4;
						self->frame = self->frame - 1;
					} else if (self->face_dir == 2) {
						self->frame = (self->frame + 1) % 4;
					}
					// Limit frame range:
					self->fr_time = 0;
				}
			}
		}

		// Count the damage timer down:
		if (e->dmg_timer > 0) e->dmg_timer--;
	}
}

void Tank_Touched(entity e, entity o) {
	DEFINE_SELF(e_tank);

	// Collide with the world:
	if (o == *(host->world)) {
		// Okay, we hit the floor:
		if (self->collide_flags & COLLIDE_BELOW) {
			// Landing noise:
			if (self->cdy >= 2) host->sndPlay(sounds.tank.land, SOUNDCHANNEL_PLAYER, 0);
			// Disable hover:
			if (self->flags & FLAG_HOVER) self->flags &= ~FLAG_HOVER;
		}

		if (self->flags & FLAG_THRUDOOR) {
			// It's not the SAME door we entered:
			self->flags &= ~FLAG_THRUDOOR;
			self->flags &= ~FLAG_NOFRICTION;
			self->flags &= ~FLAG_NOGRAVITY;
			// Half velocity out the door:
			self->dx = self->dx * 0.5;
			// Save position for restarting when dead:
			self->savex = self->x + self->dx * 2.0; self->savey = self->y;
			// Reposition the screen appropriately
			// This won't work unless we fix up the scroll_left and scroll_right
			// code to work with room boundaries.  Maybe that's what entity 0xFF is for?
			/*
			if (self->dx > 0) {
				// The left edge of the screen will be aligned to the map 2 tiles left of the door tile
				*(host->screen_mx) = host->wrap_map_coord_x( ((int)self->x / 32) * 32 - 128 );
			} else {
				*(host->screen_mx) = host->wrap_map_coord_x( ((int)self->x / 32) * 32 - *(host->screen_w) - 128 );
			}
			*/
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

				self->predoorx = self->x;
				self->predoory = self->y;
				self->predoormx = (*host->screen_mx);
				self->predoormy = (*host->screen_my);

				self->postdoorx = self->x;
				self->postdoory = self->y;
				self->postdoormx = (*host->screen_mx);
				self->postdoormy = (*host->screen_my);

				// Not looking up anymore:
				self->lookup_frame = 0;
				self->flags &= ~(FLAG_LOOKUP);

				if (self->cdx > 0) { self->dx = self->max_dx; self->face_dir = 2; }
				if (self->cdx < 0) { self->dx = -self->max_dx; self->face_dir = 6; }
			}
		}
		return;
	}

	if (self->flags & FLAG_DEAD) return;

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
			host->sndPlay(sounds.powerup.flashy, SOUNDCHANNEL_PLAYER, 0);
			break;
		case CLASS_POWERUP_GUNS:
			self->ammo.guns += 1;
			if (self->ammo.guns > 8) self->ammo.guns = 8;
			host->e_kill(o);
			break;
		case CLASS_POWERUP_GUNS_FLASH:
			self->ammo.guns += 4;
			if (self->ammo.guns > 8) self->ammo.guns = 8;
			host->e_kill(o);
			host->sndPlay(sounds.powerup.flashy, SOUNDCHANNEL_PLAYER, 0);
			break;
		case CLASS_POWERUP_HOVER:
			self->hover += 32;
			if (self->hover > 255) self->hover = 255;
			host->e_kill(o);
			break;
		case CLASS_POWERUP_HOVER_FLASH:
			self->hover += 128;
			if (self->hover > 255) self->hover = 255;
			host->e_kill(o);
			host->sndPlay(sounds.powerup.flashy, SOUNDCHANNEL_PLAYER, 0);
			break;
		case CLASS_POWERUP_HOMING_MISSILES:
			self->ammo.homing_missiles += 15;
			if (self->ammo.homing_missiles > 99) self->ammo.homing_missiles = 99;
			host->e_kill(o);
			host->sndPlay(sounds.powerup.weapon, SOUNDCHANNEL_PLAYER, 0);
			break;
		case CLASS_POWERUP_BOLTS:
			self->ammo.bolts += 15;
			if (self->ammo.bolts > 99) self->ammo.bolts = 99;
			host->e_kill(o);
			host->sndPlay(sounds.powerup.weapon, SOUNDCHANNEL_PLAYER, 0);
			break;
		case CLASS_POWERUP_TOMAHAWKS:
			self->ammo.tomahawks += 15;
			if (self->ammo.tomahawks > 99) self->ammo.tomahawks = 99;
			host->e_kill(o);
			host->sndPlay(sounds.powerup.weapon, SOUNDCHANNEL_PLAYER, 0);
			break;
		default: break;
	}

	// Drop from hover if hit.
	if (e->team != o->team) {
		self->flags &= ~(FLAG_HOVER);
		if (host->sndIsPlaying(SOUNDCHANNEL_HOVER)) host->sndStop(SOUNDCHANNEL_HOVER);
	}
}

void Tank_Draw(entity e) {
	DEFINE_SELF(e_tank);
	long	nx, ny;
	long	wheels;
	float	r, g, b;
	unsigned long y;

	#define DRAW_WHEELS(wx1,wx2,wy) \
		if (self->flags & FLAG_HOVER) { \
			if (self->timer[TIMER_TANK_HOVERSWITCH] > 0) { \
				host->put_sprite(wx1, ny, 0, 0x42); \
				host->put_sprite(wx2, ny, 0, 0x42); \
			} else { \
				host->put_sprite(wx1, ny, 0, 0x41); \
				host->put_sprite(wx2, ny, 0, 0x41); \
				switch (self->frame) { \
					case 0: \
						break; \
					case 1: \
						host->put_sprite(wx1, ny + 16, 1, 0xA4); \
						host->put_sprite(wx2, ny + 16, 1, 0xA4); \
						break; \
					case 2: \
						host->put_sprite(wx1, ny + 16, 1, 0x94); \
						host->put_sprite(wx2, ny + 16, 1, 0x94); \
						break; \
				} \
			} \
		} else { \
			switch (self->frame) { \
				case 0: \
					host->put_sprite(wx1, wy, 0, 0x04); \
					host->put_sprite_hflip(wx2, wy, 0, 0x03); \
					break; \
				case 1: \
					host->put_sprite_hflip(wx1, wy, 0, 0x03); \
					host->put_sprite(wx2, wy, 0, 0x05); \
					break; \
				case 2: \
					host->put_sprite(wx1, wy, 0, 0x05); \
					host->put_sprite(wx2, wy, 0, 0x03); \
					break; \
				case 3: \
					host->put_sprite(wx1, wy, 0, 0x03); \
					host->put_sprite(wx2, wy, 0, 0x04); \
					break; \
			} \
		}
	// Are we dead?
	if (self->flags & FLAG_DEAD) {
		// Account for the 16.15 fixed format.
		nx = host->wrap_map_coord_x((self->x) - *(host->screen_mx));
		ny = host->wrap_map_coord_y((self->y) - *(host->screen_my));
		// Draw the explosion:
		switch (self->frame) {
			case 0: case 1:
				host->put_sprite(nx	 , ny	 , 1, 0x53);
				host->put_sprite(nx	 , ny + 16, 1, 0x63);
				host->put_sprite(nx + 16, ny	 , 1, 0x73);
				host->put_sprite(nx + 16, ny + 16, 1, 0x83);
				break;
			case 2: case 3:
				host->put_sprite(nx -  8, ny -  8, 1, 0x93);
				host->put_sprite(nx -  8, ny +  8, 1, 0xA3);
				host->put_sprite(nx -  8, ny + 24, 1, 0xB3);
				host->put_sprite(nx +  8, ny -  8, 1, 0xC3);
				host->put_sprite(nx +  8, ny +  8, 1, 0xD3);
				host->put_sprite(nx +  8, ny + 24, 1, 0xE3);
				host->put_sprite(nx + 24, ny -  8, 1, 0x54);
				host->put_sprite(nx + 24, ny +  8, 1, 0x64);
				host->put_sprite(nx + 24, ny + 24, 1, 0x74);
				break;
			case 4: case 5:
				host->put_sprite(nx	 , ny	 , 1, 0xC2);
				host->put_sprite(nx	 , ny + 16, 1, 0xD2);
				host->put_sprite(nx + 16, ny	 , 1, 0xE2);
				host->put_sprite(nx + 16, ny + 16, 1, 0xF2);
				break;
			case 6: case 7:
				host->put_sprite(nx + 8, ny + 8, 1, 0xF3);
				break;
		}

		return;
	}

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

	if (self->flags & FLAG_JUMPING) wheels = ny + 8; else wheels = ny;

	// Draw tank body:
	y = ny - 18 + self->shake;

	if (self->flags & FLAG_WARPING) {
		if (self->warp_time <= 16)
			glColor4f(1.0, 1.0, 1.0, (16 - self->warp_time) / 16.0);
		else
			glColor4f(1.0, 1.0, 1.0, (self->warp_time - 16) / 16.0);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

	// Looking up:
	if (self->flags & FLAG_LOOKUP) {
		// No turning animation while looking up:
		if (self->face_dir == 8) {
			self->face_dir = self->turn_dir;
			self->flags &= ~(FLAG_TURNING);
		}

		// Facing left:
		if (self->face_dir == 6) {
			switch(self->lookup_frame) {
				case 0:		// Raise and tilt the cannon slightly.
					// Draw tank wheels:
					DRAW_WHEELS(nx - 16, nx + 16, wheels)
					host->put_sprite(nx - 12, y - 12, 0, 0x71);
					host->put_sprite(nx + 0, y, 0, 0x01);
					host->put_sprite(nx + 16, y, 0, 0x02);
					host->put_sprite(nx + 0, ny - 2 + self->shake, 0, 0xC0);
					break;
				case 1:
					// Draw tank wheels:
					DRAW_WHEELS(nx - 16, nx + 16, wheels)
					host->put_sprite(nx - 12, y - 17, 0, 0x71);
					host->put_sprite(nx +  0, y -  4, 0, 0x01);
					host->put_sprite(nx + 16, y -  4, 0, 0x02);
					host->put_sprite(nx +  0, ny - 6 + self->shake, 0, 0xC0);
					break;
				case 2:
					// Draw tank wheels:
					DRAW_WHEELS(nx - 14, nx + 14, wheels)
					host->put_sprite(nx - 12, y - 20, 0, 0x71);
					host->put_sprite(nx +  0, y -  8, 0, 0x01);
					host->put_sprite(nx + 17, y -  8, 0, 0x02);
					host->put_sprite(nx +  0, ny - 10 + self->shake, 0, 0xC0);
					break;
				case 3:
					// Draw tank wheels:
					DRAW_WHEELS(nx - 12, nx + 12, wheels)
					host->put_sprite(nx - 12, y - 24, 0, 0x71);
					host->put_sprite(nx +  0, y -  8, 0, 0x01);
					host->put_sprite(nx +  8, y -  8, 0, 0x02);
					host->put_sprite(nx +  0, ny - 14 + self->shake, 0, 0xC0);
					break;
				case 4:
					// Draw tank wheels:
					DRAW_WHEELS(nx - 10, nx + 10, wheels)
					host->put_sprite(nx +  6, y - 28, 0, 0x61);
					host->put_sprite(nx +  6, y - 12, 0, 0x11);
					host->put_sprite(nx + 22, y - 16, 0, 0x12);
					host->put_sprite(nx +  6, y	 , 0, 0x21);
					host->put_sprite(nx + 22, y	 , 0, 0x22);
					host->put_sprite(nx +  0, ny - 14 + self->shake, 0, 0xC0);
					break;
			}
		// Facing right:
		} else if (self->face_dir == 2) {
			switch(self->lookup_frame) {
				case 0:		// Raise and tilt the cannon slightly.
					// Draw tank wheels:
					DRAW_WHEELS(nx - 16, nx + 16, wheels)
					host->put_sprite_hflip(nx + 12, y - 12, 0, 0x71);
					host->put_sprite_hflip(nx +  0, y	 , 0, 0x01);
					host->put_sprite_hflip(nx - 16, y	 , 0, 0x02);
					host->put_sprite_hflip(nx +  0, ny - 2 + self->shake, 0, 0xC0);
					break;
				case 1:
					// Draw tank wheels:
					DRAW_WHEELS(nx - 16, nx + 16, wheels)
					host->put_sprite_hflip(nx + 12, y - 16, 0, 0x71);
					host->put_sprite_hflip(nx +  0, y -  4, 0, 0x01);
					host->put_sprite_hflip(nx - 16, y -  4, 0, 0x02);
					host->put_sprite_hflip(nx +  0, ny - 6 + self->shake, 0, 0xC0);
					break;
				case 2:
					// Draw tank wheels:
					DRAW_WHEELS(nx - 14, nx + 14, wheels)
					host->put_sprite_hflip(nx + 12, y - 20, 0, 0x71);
					host->put_sprite_hflip(nx +  0, y -  8, 0, 0x01);
					host->put_sprite_hflip(nx - 16, y -  8, 0, 0x02);
					host->put_sprite_hflip(nx +  0, ny - 10 + self->shake, 0, 0xC0);
					break;
				case 3:
					// Draw tank wheels:
					DRAW_WHEELS(nx - 12, nx + 12, wheels)
					host->put_sprite_hflip(nx + 12, y - 24, 0, 0x71);
					host->put_sprite_hflip(nx +  0, y -  8, 0, 0x01);
					host->put_sprite_hflip(nx - 16, y -  8, 0, 0x02);
					host->put_sprite_hflip(nx +  0, ny - 14 + self->shake, 0, 0xC0);
					break;
				case 4:
					// Draw tank wheels:
					DRAW_WHEELS(nx - 10, nx + 10, wheels)
					host->put_sprite_hflip(nx -  6, y - 28, 0, 0x61);
					host->put_sprite_hflip(nx -  6, y - 16, 0, 0x11);
					host->put_sprite_hflip(nx - 22, y - 16, 0, 0x12);
					host->put_sprite_hflip(nx -  6, y	 , 0, 0x21);
					host->put_sprite_hflip(nx - 22, y	 , 0, 0x22);
					host->put_sprite_hflip(nx +  0, ny - 14 + self->shake, 0, 0xC0);
					break;
			}
		}
 	} else {
		// Draw tank wheels:	
	   	DRAW_WHEELS(nx - 16, nx + 16, wheels)

		if (self->flags & FLAG_TURNING) {
			// Turning:
			if (self->face_dir == 8) {
				if (self->turn_dir == 2) {
					host->put_sprite(nx - 8, ny - 18, 0, 0x31);
					host->put_sprite(nx + 8, ny - 18, 0, 0x32);
					host->put_sprite_hflip(nx + 0, ny - 2 + self->shake, 0, 0xC0);
					if (self->timer[TIMER_TANK_TURN] == 0) { self->face_dir = 2; self->timer[TIMER_TANK_TURN] = 2; }
				}
				if (self->turn_dir == 6) {
					host->put_sprite_hflip(nx - 8, ny - 18, 0, 0x32);
					host->put_sprite_hflip(nx + 8, ny - 18, 0, 0x31);
					host->put_sprite(nx + 0, ny - 2 + self->shake, 0, 0xC0);
					if (self->timer[TIMER_TANK_TURN] == 0) { self->face_dir = 6; self->timer[TIMER_TANK_TURN] = 2; }
				}
			} else if (self->face_dir == 2) {
				host->put_sprite_hflip(nx - 8, ny - 18, 0, 0x32);
				host->put_sprite_hflip(nx + 8, ny - 18, 0, 0x31);
				host->put_sprite(nx + 0, ny - 2 + self->shake, 0, 0xC0);
				if (self->timer[TIMER_TANK_TURN] == 0) self->flags &= ~(FLAG_TURNING);
			} else if (self->face_dir == 6) {
				host->put_sprite(nx - 8, ny - 18, 0, 0x31);
				host->put_sprite(nx + 8, ny - 18, 0, 0x32);
				host->put_sprite_hflip(nx + 0, ny - 2 + self->shake, 0, 0xC0);
				if (self->timer[TIMER_TANK_TURN] == 0) self->flags &= ~(FLAG_TURNING);
			}
		} else {
			// Not turning:
			if (self->face_dir == 2) {
				host->put_sprite_hflip(nx - 16, y, 0, 0x02);

				// Mid-section depends on if the hatch is open or not:
				if (self->timer[TIMER_TANK_OPENHATCH] > 0) {
					host->put_sprite_hflip(nx +  0, y	 , 0, 0x20);
					host->put_sprite_hflip(nx - 16, y - 16, 0, 0x10);
				} else host->put_sprite_hflip(nx + 0, y, 0, 0x01);

				host->put_sprite_hflip(nx + 16, y, 0, 0x51);
				host->put_sprite_hflip(nx +  0, ny - 2 + self->shake, 0, 0xC0);
			} else if (self->face_dir == 6) {
				host->put_sprite(nx - 16, y, 0, 0x51);

				// Mid-section depends on if the hatch is open or not:
				if (self->timer[TIMER_TANK_OPENHATCH] > 0) {
					host->put_sprite(nx +  0, y	 , 0, 0x20);
					host->put_sprite(nx + 16, y - 16, 0, 0x10);
				} else host->put_sprite(nx + 0, y, 0, 0x01);

				host->put_sprite(nx + 16, y, 0, 0x02);
				host->put_sprite(nx +  0, ny - 2 + self->shake, 0, 0xC0);
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

	// Draw the map over the tank:
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

void Tank_Preview(int x, int y, class_type class) {
	y += 8;
	x += 8;
	// Upper body & step:
	host->put_sprite_hflip(x - 8, y - 9, 0, 0x02);
	host->put_sprite_hflip(x + 0, y - 9, 0, 0x01);
	host->put_sprite_hflip(x + 8, y - 9, 0, 0x51);
	host->put_sprite_hflip(x +  0, y - 1, 0, 0xC0);
	// Wheels:
	host->put_sprite(x - 8, y, 0, 0x04);
	host->put_sprite_hflip(x + 8, y, 0, 0x03);
};

void Tank_Init(entity e) {
	DEFINE_SELF(e_tank);

	self->team = TEAM_FROGHUNTERS;

	// Appearance:
	self->fr_time = 0; self->frame = 0;

	// Deltas:
	self->dx = 0;
	self->dy = 0;
	self->accel = 0.75;
	self->max_dx = 3.0;

#ifdef CHEATING
	self->weapon = 2;
	self->hover = 255;
#else
	self->weapon = 0;
	self->hover = 255;
#endif
	self->health = 255;
	self->maxhealth = 255;
	self->dmg_time = 60;

	// Face right:
	self->face_dir = 2;		// 0 = up, 2 = right, 4 = down, 6 = left

	self->flags = FLAG_CONTROLLED | FLAG_TAKEDAMAGE;
	self->items = ITEM_KEY;

	self->ecrx1 =   0;	self->ecry1 = -15;
	self->ecrx2 =  15;	self->ecry2 =  15;

	self->mcrx1 = -15;	self->mcry1 = -15;
	self->mcrx2 =  31;	self->mcry2 =  15;
}

class_properties_t	ClassProperties_Tank = {
	name:			"Player Tank",
	size:			sizeof(e_tank),
	init:			Tank_Init,
	prethink:		Tank_PreThink,
	postthink:		Tank_PostThink,
	touched:		Tank_Touched,
	draw:			Tank_Draw,
	preview:		Tank_Preview,
	death:			Tank_Death,
	attacked:		Tank_Attacked
};

