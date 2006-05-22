// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef HERMIT_C
#define HERMIT_C

typedef struct e_hermit {
#	include "basic.h"
} e_hermit;

void Hermit_PreThink(entity e);
void Hermit_Touched(entity e, entity o);
void Hermit_Draw(entity e);
void Hermit_Init(entity e);
void Hermit_Preview(int x, int y, class_type class);
void Hermit_Death(entity e);
int Hermit_Attacked(entity e, entity o, long damage);

extern	class_properties_t	ClassProperties_Hermit;

#endif
