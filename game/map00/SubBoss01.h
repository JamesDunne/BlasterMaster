// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef SUBBOSS01_C
#define SUBBOSS01_C

typedef struct e_subboss01 {
#	include "basic.h"
} e_subboss01;

void SubBoss01_PreThink(entity e);
void SubBoss01_Touched(entity e, entity o);
void SubBoss01_Draw(entity e);
void SubBoss01_Init(entity e);
void SubBoss01_Preview(int x, int y, class_type class);
void SubBoss01_Death(entity e);
int SubBoss01_Attacked(entity e, entity o, long damage);

extern	class_properties_t	ClassProperties_SubBoss01;

#endif
