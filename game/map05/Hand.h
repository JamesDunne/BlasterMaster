// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef HAND_C
#define HAND_C

typedef struct e_hand {
#	include "basic.h"
} e_hand;

void Hand_PreThink(entity e);
void Hand_Touched(entity e, entity o);
void Hand_Draw(entity e);
void Hand_Init(entity e);
void Hand_Preview(int x, int y, class_type class);
void Hand_Death(entity e);
int Hand_Attacked(entity e, entity o, long damage);

extern	class_properties_t	ClassProperties_Hand;

#endif
