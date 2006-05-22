// C Header File
// Created 1/6/2004; 7:53:37 AM

#ifndef Bullet_C
#define Bullet_C

// Bullet types:
typedef enum {
	BULLET_SMALL, BULLET_MEDIUM, BULLET_MEDIUM_RED,
	BULLET_LARGE, BULLET_CANNON, BULLET_CRUSHER,
	BULLET_HYPER, BULLET_MISSILE, BULLET_HOMING_MISSILE
} bullet_type;

typedef struct e_bullet {
#	include "basic.h"

	bullet_type		kind;
	entity			target;

	short			damage;

	long			killtime;
} e_bullet;

void Bullet_PreThink(entity e);
//void Bullet_PostThink(entity e);
void Bullet_Touched(entity e, entity o);
void Bullet_Draw(entity e);
void Bullet_Init(entity e);

extern	class_properties_t	ClassProperties_Bullet;

#endif
