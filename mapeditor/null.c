#include "common.h"

// Entity management functions:
entity	null_e_spawn(class_type class) { }
entity	null_e_spawnat(class_type class, fixed x, fixed y) { }
entity	null_e_spawnatmap(class_type class, fixed mx, fixed my) { }
entity	null_e_findfirst() { }
entity	null_e_findnext() { }
void	null_e_ptr2uid(entity_t **p) { }
void	null_e_uid2ptr(entity_t **p) { }

void	null_e_kill(entity e) { }
void	null_e_damage(entity target, entity attacker, short damage) { }
void	null_control_switch(entity e) { }
Uint8	null_gettileat(long ex, long ey) { }
void	null_settileat(long ex, long ey, Uint8 tile) { }

fixed	null_wrap_map_coord_x(fixed a) { }
fixed	null_wrap_map_coord_y(fixed a) { }

int		null_LoadLevel(const char *filename) { }

// Sound functions:
int		null_sndPrecache(const char *filename) { }
int		null_sndFree(int sound) { }
int		null_sndPlay(int sound, int channel, int loops) { }
int		null_sndSetVolume(int channel, int volume) { }
int		null_sndStop(int channel) { }

// Pointers to the original variables:
map_t	*map;

// Entity who has control of input:
controlled_entity_t	*null_player;
entity				null_world;

// Game state variables:
fixed	null_screen_mx, null_screen_my;
fixed	null_old_screen_mx, null_old_screen_my;
fixed	null_screen_w, null_screen_h;
fixed	null_scroll_left, null_scroll_right;
fixed	null_scroll_top, null_scroll_bottom;
int		null_quit, null_game_paused;

