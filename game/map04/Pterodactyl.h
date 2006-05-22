// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef PTERODACTYL_C
#define PTERODACTYL_C

typedef struct e_pterodactyl {
#	include "basic.h"
} e_pterodactyl;

void Pterodactyl_PreThink(entity e);
void Pterodactyl_Touched(entity e, entity o);
void Pterodactyl_Draw(entity e);
void Pterodactyl_Init(entity e);
void Pterodactyl_Preview(int x, int y, class_type class);
void Pterodactyl_Death(entity e);
int Pterodactyl_Attacked(entity e, entity o, long damage);

extern	class_properties_t	ClassProperties_Pterodactyl;

#endif
