#ifndef TANK_H
#define TANK_H

// Frame timers:
typedef enum {
	TIMER_TANK_SHAKE,
	TIMER_TANK_TURN,
	TIMER_TANK_LOOKUP,
	TIMER_TANK_OPENHATCH,
	TIMER_TANK_HOVERSWITCH,
	TIMER_TANKS
} tank_timer;

// The tank holds all the player's information and stats.  Jason should hold nothing.
// The tank object should never be actually destroyed until the player disconnects
// or the server is terminated.
typedef struct e_tank {
#	include "controlled.h"

	unsigned char	shake;
	signed char		turn_dir;
	long			lookup_frame;

	// Current weapon selected:
	int				weapon;

	// Inventory:  (-128..127)
	struct ammunition_t {
		char	homing_missiles;
		char	tomahawks;
		char	bolts;
		char	guns;
	} ammo;

	short		hover;

	unsigned long	timer[TIMER_TANKS];
} e_tank;

void Tank_Death(entity e);
void Tank_PreThink(entity e);
void Tank_PostThink(entity e);
void Tank_Touched(entity e, entity o);
void Tank_Draw(entity e);
void Tank_Preview(int x, int y, class_type class);
void Tank_Init(entity e);

extern	class_properties_t	ClassProperties_Tank;

#endif

