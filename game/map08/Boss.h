// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef BOSS_C
#define BOSS_C

typedef struct e_boss {
#	include "basic.h"
	int		turn;
} e_boss;

void Boss_PreThink(entity e);
//void Boss_PostThink(entity e);
void Boss_Touched(entity e, entity o);
void Boss_Draw(entity e);
void Boss_Init(entity e);
void Boss_Preview(int x, int y, class_type class);
void Boss_Death(entity e);

extern	class_properties_t	ClassProperties_Boss;

extern	const int boss_page;

#endif
