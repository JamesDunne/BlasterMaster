// C Header File
// Created 1/6/2004; 7:53:37 AM

#ifndef World_C
#define World_C

typedef struct e_world {
#	include "basic.h"
} e_world;

void World_Draw(entity e);
void World_Init(entity e);

extern	class_properties_t	ClassProperties_World;

#endif
