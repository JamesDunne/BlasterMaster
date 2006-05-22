// C Header File
// Created 12/15/2003; 5:06:11 PM
#define	MAX_ENTITIES	512

extern	entity_t		**entities;
extern	int				numentities;
extern	int				last_entity;
extern	int				level_changed;

// Entity who has control of input:
extern	controlled_entity_t	*player;
extern	entity			world;
extern	double			time_dt;

entity	e_spawn(class_type class);
entity	e_spawnat(class_type class, fixed x, fixed y);
entity	e_spawnatmap(class_type class, fixed mx, fixed my);
void	e_kill(entity e);
entity	e_findfirst();
entity	e_findnext();
void	e_ptr2uid(entity_t **p);
void	e_uid2ptr(entity_t **p);
void	e_default_store(entity e);
void	e_default_restore(entity e);
void	free_killed();
void	e_damage(entity target, entity attacker, short damage);
void	control_switch(entity e);

void	process_entities();

// Useful #defines to set correct {x,y} and {mx,my} offsets:
#define set_offset_x(a, b, ofsx) (a)->x = (b)->x + ofsx
#define set_offset_y(a, b, ofsy) (a)->y = (b)->y + ofsy

#define call_process_func(ent, func) \
	if (class_properties[ent->class]->func) \
		class_properties[ent->class]->func(ent)

#define call_interact_func(ent1, ent2, func) \
	if (class_properties[ent1->class]->func) \
		class_properties[ent1->class]->func(ent1, ent2)

