// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef SWIMMER_C
#define SWIMMER_C

typedef struct e_swimmer {
#	include "basic.h"
} e_swimmer;

void Swimmer_PreThink(entity e);
void Swimmer_Touched(entity e, entity o);
void Swimmer_Draw(entity e);
void Swimmer_Init(entity e);
void Swimmer_Preview(int x, int y, class_type class);
void Swimmer_Death(entity e);
int Swimmer_Attacked(entity e, entity o, long damage);
int Swimmer_Maptest(int m);

extern	class_properties_t	ClassProperties_Swimmer;

#endif
