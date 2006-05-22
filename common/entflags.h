// C Header File
// Created 1/6/2004; 12:13:21 AM

#ifndef ENTFLAGS_H
#define ENTFLAGS_H

#include "common.h"

// Teams:
typedef enum { TEAM_FROGHUNTERS, TEAM_MONSTERS } teams;

// System flags:
#define		FLAG_NOMAPCOLLISION	0x00000001
#define		FLAG_NOFRICTION		0x00000002
#define		FLAG_NOGRAVITY		0x00000004
#define		FLAG_NODESTROY		0x00000008
#define		FLAG_TAKEDAMAGE		0x00000010
#define		FLAG_CONTROLLED		0x00000020
#define		FLAG_ONSCREEN_ONLY	0x00000040

#define		FLAG_DEAD			0x00000080
#define		FLAG_ONGROUND		0x00000100
#define		FLAG_THRUDOOR		0x00000200
#define		FLAG_JUMPING		0x00000400
#define		FLAG_INVINCIBLE		0x00000800

#define		SPAWNFLAG_MAPSPAWNABLE		0x00000001
#define		SPAWNFLAG_PLAYER			0x00000002

#define 	MAPFLAG_JASON	0x04
#define 	MAPFLAG_DOOR	0x08
#define 	MAPFLAG_HARMFUL	0x10
#define 	MAPFLAG_LADDER	0x20
#define 	MAPFLAG_WATER	0x40
#define 	MAPFLAG_SOLID	0x80

typedef unsigned long class_type;

struct entity_t;
typedef void (*process_func)(struct entity_t *e);
typedef void (*interact_func)(struct entity_t *e, struct entity_t *o);
typedef int (*attacked_func)(struct entity_t *e, struct entity_t *o, long damage);
typedef void (*preview_func)(int x, int y, class_type class);
typedef int (*maptest_func)(int m);

// Entity structure for ALL entities:
typedef struct entity_t {
#	include "basic.h"
	char	reserved[2048];
} entity_t;
typedef entity_t *entity;

typedef struct controlled_entity_t {
#	include "controlled.h"
} controlled_entity_t;
typedef controlled_entity_t	*controlled_entity;

// e_spawn(class_type) takes care of calling the 'init' function and setting the
// class type.
typedef struct {
	// Size of the structure:
	int				size;
	char			*name;
	// Functions:
	process_func	init;							// Class initializer
	process_func	prethink;						// Before physics processing function
	process_func	postthink;						// After physics processing function
	interact_func	touched;						// Touched by something else
	process_func	draw;							// Draw the entity
	preview_func	preview;						// Draw a preview of the entity for the map editor
	process_func	death;							// When it dies
	maptest_func	maptest;						// Returns true when a tile should be collided with
	attacked_func	attacked;						// Called when attacked by something else
} class_properties_t;

typedef struct {
	class_type		class;
	int				size;
	char			*name;
	preview_func	preview;
} mapclass_properties_t;

class_properties_t	**class_properties;

#endif

