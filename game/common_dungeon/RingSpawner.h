// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef RINGSPAWNER_C
#define RINGSPAWNER_C

typedef struct e_ringspawner {
#	include "basic.h"
} e_ringspawner;

void RingSpawner_PreThink(entity e);
void RingSpawner_Init(entity e);

extern	class_properties_t	ClassProperties_RingSpawner;

#endif
