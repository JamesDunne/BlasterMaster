// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef MECHIPEDE_C
#define MECHIPEDE_C

typedef struct e_mechipede {
#	include "basic.h"
} e_mechipede;

void Mechipede_PreThink(entity e);
void Mechipede_Touched(entity e, entity o);
void Mechipede_Draw(entity e);
void Mechipede_Init(entity e);
void Mechipede_Preview(int x, int y, class_type class);
void Mechipede_Death(entity e);
int Mechipede_Attacked(entity e, entity o, long damage);

extern	class_properties_t	ClassProperties_Mechipede;

#endif
