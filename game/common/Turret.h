// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef TEMPLATE_C
#define TEMPLATE_C

typedef struct e_turret {
#	include "basic.h"
	int		fr_speed;
	int		turn;
} e_turret;

void Turret_PreThink(entity e);
//void Turret_PostThink(entity e);
void Turret_Touched(entity e, entity o);
void Turret_Draw(entity e);
void Turret_Init(entity e);
void Turret_Preview(int x, int y, class_type class);
void Turret_Death(entity e);

extern	class_properties_t	ClassProperties_Turret;

#endif
