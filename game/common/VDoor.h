// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef VDOOR_C
#define VDOOR_C

typedef struct e_vdoor {
#	include "basic.h"
	int	tile1, tile2, tile3;
} e_vdoor;

void VDoor_PreThink(entity e);
//void VDoor_PostThink(entity e);
void VDoor_Touched(entity e, entity o);
void VDoor_Draw(entity e);
void VDoor_Init(entity e);
void VDoor_Preview(int x, int y, class_type class);
void VDoor_Death(entity e);

extern	class_properties_t	ClassProperties_VDoor;

#endif
