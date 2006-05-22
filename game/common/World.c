// C Source File
// Created 1/6/2004; 7:54:15 AM

#include "common.h"
#include "classes.h"
#include "bm_game.h"
#include "newmap.h"
#include "interface.h"

class_properties_t	ClassProperties_World = {
	sizeof(e_world),
	"World",
	World_Init,
	NULL,
	NULL,
	NULL,
	World_Draw,
	NULL
};

void World_Draw(entity e) {

}

void World_Init(entity e) {
	DEFINE_SELF(e_world);

	// Set the position
	self->x = 0L;
	self->y = 0L;
};
