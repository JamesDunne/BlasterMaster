// C Header File
// Created 1/6/2004; 6:32:23 AM

#ifndef JASON_C
#define JASON_C

typedef struct e_jason {
#	include "controlled.h"
	// The tank holds all the current player's information
	// The Jason entity can be killed and respawned whenever
	// he enters/exits the tank.  Since Jason can be killed, we
	// should not store any important consistent information in
	// his struct.
	long	warp_downtime;
} e_jason;

void Jason_PreThink(entity e);
void Jason_PostThink(entity e);
void Jason_Touched(entity e, entity o);
void Jason_Draw(entity e);
void Jason_Preview(int x, int y, class_type class);
void Jason_Init(entity e);
void Jason_Outdoor(entity e);

extern class_properties_t	ClassProperties_Jason;

#endif
