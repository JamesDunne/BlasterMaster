// C Header File
// Created 1/6/2004; 6:14:07 AM

#ifndef SQUIDY_C
#define SQUIDY_C

typedef struct e_squidy {
#	include "basic.h"

	unsigned long	squiggle_time;

	// The entity we're chasing after:
	struct entity_t	*target;
} e_squidy;

void Squidy_PreThink(entity e);
//void Squidy_PostThink(entity e);
void Squidy_Touched(entity e, entity o);
void Squidy_Draw(entity e);
void Squidy_Preview(int x, int y, class_type class);
void Squidy_Death(entity e);
void Squidy_Init(entity e);
int  Squidy_Maptest(int m);

extern	class_properties_t	ClassProperties_Squidy;

#endif
