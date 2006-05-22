// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef KEYHOLEL_C
#define KEYHOLEL_C

typedef struct e_keyhole {
#	include "basic.h"

	char	locked;
} e_keyhole;

//void Keyhole_PreThink(entity e);
//void Keyhole_PostThink(entity e);
void Keyhole_Touched(entity e, entity o);
void Keyhole_Draw(entity e);
void Keyhole_Init(entity e);
void Keyhole_Preview(int x, int y, class_type class);
//void Keyhole_Death(entity e);

extern	class_properties_t	ClassProperties_KeyholeL;
extern	class_properties_t	ClassProperties_KeyholeR;

#endif
