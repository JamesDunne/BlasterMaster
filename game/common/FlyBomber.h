// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef FLYBOMBER_C
#define FLYBOMBER_C

typedef struct e_flybomber {
#	include "basic.h"
} e_flybomber;

void FlyBomber_PreThink(entity e);
void FlyBomber_Touched(entity e, entity o);
void FlyBomber_Draw(entity e);
void FlyBomber_Init(entity e);
void FlyBomber_Preview(int nx, int ny, class_type class);
void FlyBomber_Death(entity e);
int FlyBomber_Attacked(entity e, entity o, long damage);

extern	class_properties_t	ClassProperties_FlyBomber;

#endif
