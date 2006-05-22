// C Header File
// Created 1/6/2004; 12:45:24 PM

#ifndef POWERUP_C
#define POWERUP_C

typedef struct e_powerup {
#	include "basic.h"

	long			killtime;
} e_powerup;

void PowerUp_PreThink(entity e);
void PowerUp_Draw(entity e);
void PowerUp_Init(entity e);

extern	class_properties_t	ClassProperties_PowerUp_Power;
extern	class_properties_t	ClassProperties_PowerUp_Power_Flash;
extern	class_properties_t	ClassProperties_PowerUp_Guns;
extern	class_properties_t	ClassProperties_PowerUp_Guns_Flash;
extern	class_properties_t	ClassProperties_PowerUp_Hover;
extern	class_properties_t	ClassProperties_PowerUp_Hover_Flash;
extern	class_properties_t	ClassProperties_PowerUp_Homing_Missiles;
extern	class_properties_t	ClassProperties_PowerUp_Bolts;
extern	class_properties_t	ClassProperties_PowerUp_Tomahawks;

#endif
