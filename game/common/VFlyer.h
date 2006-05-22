// C Header File
// Created 1/6/2004; 6:14:07 AM

#ifndef VFLYER_C
#define VFLYER_C

typedef struct e_vflyer {
#	include "basic.h"

	// The entity we're chasing after:
	struct entity_t	*target;
} e_vflyer;

void VFlyer_PreThink(entity e);
//void VFlyer_PostThink(entity e);
void VFlyer_Touched(entity e, entity o);
void VFlyer_Draw(entity e);
void VFlyer_Preview(int x, int y, class_type class);
void VFlyer_Init(entity e);
int  VFlyer_Maptest(int m);
void VFlyer_Death(entity e);

extern	class_properties_t	ClassProperties_VFlyer;

#endif
