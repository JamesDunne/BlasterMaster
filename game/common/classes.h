// C Header File
// Created 1/6/2004; 12:13:21 AM

#ifndef CLASSES_H
#define CLASSES_H

#include "common.h"
#include "entflags.h"

/*
// System flags:
define		FLAG_NOMAPCOLLISION	0x00000001
define		FLAG_NOFRICTION		0x00000002
define		FLAG_NOGRAVITY		0x00000004
define		FLAG_NODESTROY		0x00000008
define		FLAG_TAKEDAMAGE		0x00000010
define		FLAG_CONTROLLED		0x00000020

define		FLAG_DEAD			0x00000040
define		FLAG_ONGROUND		0x00000080
define		FLAG_THRUDOOR		0x00000100
define		FLAG_JUMPING		0x00000200
define		FLAG_FLICKER		0x00000400
define		FLAG_INVINCIBLE		0x00000800
*/

// Flags for the entities:
#define		FLAG_FLICKER		0x00001000
#define		FLAG_CRAWLING		0x00002000
#define		FLAG_SUPERJUMP		0x00004000
#define		FLAG_TURNING		0x00008000
#define		FLAG_BOUNCE			0x00010000
#define		FLAG_LOOKUP			0x00020000
#define		FLAG_CLIMBING		0x00040000
#define		FLAG_SWIMMING		0x00080000
#define		FLAG_WARPING		0x00100000
#define		FLAG_HOVER			0x00200000

#define		ITEM_KEY	0x01

// Class types for the 'class' field in the entity_t structure:
typedef enum {
///---Do not change this line!! It is used by scripts to add entities!!---
	CLASS_MECHIPEDE = 0x00,
	CLASS_ROCKCLIMBER_CW = 0x01,
	CLASS_ROCKCLIMBER_CCW = 0x02,
	CLASS_ROCKGUNNER = 0x04,
	CLASS_ROCKWALKER = 0x05,
	CLASS_MINE = 0x07,
	CLASS_TURRET = 0x08,
	CLASS_FLYBOMBER = 0x0B,
	CLASS_VFLYER = 0x10,
	CLASS_RINGSPAWNER_RED = 0x13,
	CLASS_RINGSPAWNER = 0x14,
	CLASS_HAND = 0x15,
	CLASS_SQUIDY = 0x16,
	CLASS_ROCKCLIMBER_RED_CW = 0x1C,
	CLASS_ROCKCLIMBER_RED_CCW = 0x1D,

	CLASS_SWIMMER = 0x17,
	CLASS_HERMIT = 0x18,
	CLASS_PTERODACTYL = 0x19,
	CLASS_SUBBOSS01 = 0x20,
	
	CLASS_BOSS = 0x20,

	// PowerUp types:
	CLASS_POWERUP_POWER = 0x21,
	CLASS_POWERUP_POWER_FLASH = 0x22,
	CLASS_POWERUP_GUNS = 0x23,
	CLASS_POWERUP_GUNS_FLASH = 0x24,
	CLASS_POWERUP_HOVER = 0x25,
	CLASS_POWERUP_HOVER_FLASH = 0x26,
	CLASS_POWERUP_HOMING_MISSILES = 0x27,
	CLASS_POWERUP_BOLTS = 0x28,
	CLASS_POWERUP_TOMAHAWKS = 0x29,

	CLASS_VDOOR = 0x2A,
	CLASS_KEYHOLEL = 0x2B,
	CLASS_KEYHOLER = 0x2C,

	CLASS_TANK = 0xF0,
	CLASS_JASON = 0xF1,
	CLASS_EXPLOSION = 0xFC,
	CLASS_BULLET = 0xFD,
	CLASS_WORLD = 0xFE,
	
	CLASS_RINGFLYER_RED = 0xE0,
	CLASS_RINGFLYER = 0xE1,

	// Must be last identifier in the enum:
	NUM_CLASSES = 0x100
} class_types;

// Now, define all the classes:
#include "World.h"
#include "Tank.h"
#include "Jason.h"
#include "Bullet.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "Squidy.h"
#include "Mine.h"
#include "VFlyer.h"
#include "Turret.h"
#include "RockClimber.h"
#include "RockWalker.h"
#include "VDoor.h"
#include "RockGunner.h"
#include "FlyBomber.h"
///---Do not change this line!! It is used by scripts to add entities!!---
#include "../map04/Pterodactyl.h"
#include "../map04/Hermit.h"
#include "../map04/Swimmer.h"
#include "../map03/Keyhole.h"
#include "../map00/SubBoss01.h"
#include "../map05/Hand.h"
#include "Mechipede.h"
#include "Mechipede.h"
#include "RingFlyer.h"
#include "RingSpawner.h"
#include "FlyBomber.h"

#define DEFINE_SELF(type)	type *self = (type *)e

#endif

