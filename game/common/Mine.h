// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef MINE_C
#define MINE_C

typedef struct e_mine {
#	include "basic.h"
} e_mine;

void Mine_PreThink(entity e);
//void Mine_PostThink(entity e);
void Mine_Touched(entity e, entity o);
void Mine_Draw(entity e);
void Mine_Init(entity e);
void Mine_Preview(int x, int y, class_type class);
void Mine_Death(entity e);

extern	class_properties_t	ClassProperties_Mine;

#endif
