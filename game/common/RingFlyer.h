// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef RINGFLYER_C
#define RINGFLYER_C

typedef struct e_ringflyer {
#	include "basic.h"
	double	osc_time;
} e_ringflyer;

void RingFlyer_PreThink(entity e);
void RingFlyer_Touched(entity e, entity o);
void RingFlyer_Draw(entity e);
void RingFlyer_Init(entity e);
void RingFlyer_Preview(int x, int y, class_type class);
void RingFlyer_Death(entity e);
int RingFlyer_Attacked(entity e, entity o, long damage);

extern	class_properties_t	ClassProperties_RingFlyer;

#endif
