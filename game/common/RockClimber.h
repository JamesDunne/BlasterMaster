// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef ROCKCLIMBER_C
#define ROCKCLIMBER_C

// Counter-clockwise or clockwise direction:
typedef enum { CWD_CCW, CWD_CW } cwdir;

typedef struct e_rockclimber {
#	include "basic.h"

	cwdir	cwd;
} e_rockclimber;

void RockClimber_PreThink(entity e);
//void RockClimber_PostThink(entity e);
void RockClimber_Touched(entity e, entity o);
void RockClimber_Draw(entity e);
void RockClimber_Init(entity e);
void RockClimber_Preview(int x, int y, class_type class);
void RockClimber_Death(entity e);

extern	class_properties_t	ClassProperties_RockClimber;
extern	class_properties_t	ClassProperties_RockClimber_Red;

#endif

