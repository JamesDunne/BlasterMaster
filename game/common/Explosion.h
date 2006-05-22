// C Header File
// Created 1/6/2004; 3:28:45 PM

#ifndef EXPLOSION_C
#define EXPLOSION_C

// Needs bullet size information:
#include "Bullet.h"

typedef struct e_explosion {
#	include "basic.h"
	// Determines the size of the explosion:
	bullet_type		kind;
} e_explosion;

void Explosion_PreThink(entity e);
//void Explosion_PostThink(entity e);
//void Explosion_Touched(entity e, entity o);
void Explosion_Draw(entity e);
void Explosion_Init(entity e);
entity BecomeExplosion(entity e, bullet_type bullet);

extern class_properties_t	ClassProperties_Explosion;

#endif

