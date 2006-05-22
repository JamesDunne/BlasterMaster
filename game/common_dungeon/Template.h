// C Header File
// Created 1/6/2004; 9:44:12 AM

#ifndef TEMPLATE_C
#define TEMPLATE_C

typedef struct e_template {
#	include "basic.h"
} e_template;

void Template_PreThink(entity e);
void Template_Touched(entity e, entity o);
void Template_Draw(entity e);
void Template_Init(entity e);
void Template_Preview(int x, int y, class_type class);
void Template_Death(entity e);
int Template_Attacked(entity e, entity o, long damage);

extern	class_properties_t	ClassProperties_Template;

#endif
