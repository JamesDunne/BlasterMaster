// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef RockWalker_C
#define RockWalker_C

typedef struct e_rockwalker {
#	include "basic.h"
} e_rockwalker;

void RockWalker_PreThink(entity e);
//void RockWalker_PostThink(entity e);
void RockWalker_Touched(entity e, entity o);
void RockWalker_Draw(entity e);
void RockWalker_Init(entity e);
void RockWalker_Preview(int x, int y, class_type class);
void RockWalker_Death(entity e);
int RockWalker_Attacked(entity e, entity o, long damage);

extern	class_properties_t	ClassProperties_RockWalker;

#endif
