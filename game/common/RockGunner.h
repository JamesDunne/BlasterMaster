// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef RockGunner_C
#define RockGunner_C

typedef struct e_rockgunner {
#	include "basic.h"
	// How long it's been since we last attacked:
	long	attack_time;
} e_rockgunner;

void RockGunner_PreThink(entity e);
//void RockGunner_PostThink(entity e);
void RockGunner_Touched(entity e, entity o);
void RockGunner_Draw(entity e);
void RockGunner_Init(entity e);
void RockGunner_Preview(int x, int y, class_type class);
void RockGunner_Death(entity e);
int RockGunner_Attacked(entity e, entity o, long damage);

extern	class_properties_t	ClassProperties_RockGunner;

#endif
