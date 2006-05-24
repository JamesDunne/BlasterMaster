#include "sound.h"
#include "opengl.h"
#include "entflags.h"
#include "entities.h"
#include "newmap.h"
#include "bm_game.h"

typedef struct cache_entry_t {
	char		*filename;			// The map filename associated with these entities.
	entity_t	**state;			// The list of entities.

	long		count;

	struct cache_entry_t	*next;	// Next entry in list.
} cache_entry_t;

cache_entry_t		*cache_head, **cache_tail;

controlled_entity	player = NULL;
entity				world = NULL;
void				*game_library = NULL;
void				*client_library = NULL;
dllfunctions		*game, *client;
hostfunctions		*host;
int			sound_available;

fixed		screen_mx, screen_my;
fixed		old_screen_mx, old_screen_my;
fixed		screen_w, screen_h;
fixed		scroll_left, scroll_right;
fixed		scroll_top, scroll_bottom;
int			xaxis_scale, yaxis_scale;
int			quit, game_paused;

char		last_mapfilename[1024];

int LoadLevel(const char *reqmapfilename) {
	int		retcode, i, j;
	char	filename[1024];
	entity	e;
	cache_entry_t	*curr;

//	fprintf(stderr, "LoadLevel('%s')\n", reqmapfilename);

	// All interrelated entity pointers are maintained, because no entities are freed and respawned.
	// They all maintain their own memory, just the list of pointers to them is swapped out.

	if (level_loaded) {
		FreeTextures();

		// Find a previously existing node in the cache for this map:
//		fprintf(stderr, "  Searching for previously used cache entry for '%s'...", last_mapfilename);
		curr = cache_head;
		while (curr != NULL) {
			if ( strcmp(curr->filename, last_mapfilename) == 0 )
				break;
			curr = curr->next;
		}
		// Upon exit, curr is either NULL (no cache entry exists), or curr is a valid pointer to
		// the previous cache entry.

		if (curr == NULL) {
// 			fprintf(stderr, "not found!\n");
			// Allocate a new node in the cache:
			curr = calloc(sizeof(cache_entry_t), 1);
			// Copy the filename to the cache entry:
			curr->filename = calloc(strlen(last_mapfilename) + 1, 1);
			strcpy(curr->filename, last_mapfilename);
			curr->state = NULL;
			curr->next = NULL;
			// Link it in:
			*cache_tail = curr;
			cache_tail = &(curr->next);
		} else {
// 			fprintf(stderr, "found at 0x%08lp! ", curr);
			// Use the last created node in the cache for this map:
			// filename is still good, but we must free the entity list memory:
			free(curr->state);
			curr->state = NULL;
// 			fprintf(stderr, " (old memory freed)\n");
		}

		// We should be all set to just fill in the entities' states now:
// 		fprintf(stderr, "  Saving entity states...");

		curr->count = last_entity;
		// Size the list to fit all entities in:
		curr->state = calloc(sizeof(entity_t *) * (last_entity + 1), 1);
		for (i=0; i<=last_entity; ++i) {
			if (entities[i] != (entity) player) {
				// Just move the pointer over, without killing the entity:
				curr->state[i] = entities[i];
				entities[i] = NULL;
			}
		}

// 		fprintf(stderr, "done!\n");
	}

	// Load the map, cached or not:
	retcode = LoadMap(reqmapfilename);
	if (retcode != 0) return retcode;

	level_loaded = 1;
	InitTextures();

	if (map.game_filename) {
 		fprintf(stderr, "Loading Game DLL... '%s'\n", map.game_filename);
		if (LoadGameDLL(map.game_filename) == -1) return -1;
//  	fprintf(stderr, "game DLL loaded.  game->init();\n");
		game->init();
	}

	// Find out if theres a cached copy of the entities for this map:
// 	fprintf(stderr, "  Searching for cache entry for '%s'...", reqmapfilename);
	curr = cache_head;
	while (curr != NULL) {
		if ( strcmp(curr->filename, reqmapfilename) == 0 )
			break;
		curr = curr->next;
	}
	
	// No cached copy, so go ahead and load the map's entities:
	if (curr == NULL) {
// 		fprintf(stderr, "not found!  loading default entities...");
		// Now, spawn real copies of the map entities:
		for (i=0; i<map.num_entities; ++i) {
			e = e_spawn(map.entities[i]->class);
			if (e == NULL) continue;

			e->x = map.entities[i]->x;
			e->y = map.entities[i]->y;
			e->savex = e->x;
			e->savey = e->y;

			// Switch control to the _first_ PLAYER entity:
			if ((player == NULL) && (game->spawnflags[e->class] & SPAWNFLAG_PLAYER)) {
				control_switch(e);
			} else if (game->spawnflags[e->class] & SPAWNFLAG_PLAYER) {
				e_kill(e);
			}
		}
		free_killed();

		if (player == NULL) {
 			fprintf(stderr, "No player entity (with SPAWNFLAG_PLAYER) was found on the map\n");
			return -1;
		}
// 		fprintf(stderr, "done!\n");
	} else {
		// Yeah, let's load the cached copy:
// 		fprintf(stderr, "found at 0x%08lp!\n", curr);
// 		fprintf(stderr, "  Loading %d cached entities\n", curr->count);
		
		// Copy over the saved list, but don't overwrite the player:
		for (i=0; i<=curr->count; ++i) if (entities[i] != (entity) player) {
			entities[i] = curr->state[i];
		}
		last_entity = curr->count;
	}
	
	strcpy(last_mapfilename, reqmapfilename);
	level_changed = 1;

	if ( sound_available ) {

		// load the song
		musPlay( map.music_filename );

	}

	return 0;
}

void create_host() {
	// Quick macro to set the function pointers:
	#define sendfunction(func) host->func = func
	#define sendvariable(var) host->var = &var

	host = (hostfunctions *)malloc(sizeof(hostfunctions));
	// Zero to begin with:
	memset(host, 0, sizeof(hostfunctions));

	// Send functions and addresses of variables to the DLL:
	sendfunction(put_sprite);
	sendfunction(put_sprite_hflip);
	sendfunction(put_sprite_vflip);
	sendfunction(put_sprite_hvflip);
	sendfunction(put_bgtile);

	sendfunction(Draw2x2BGTile);
	
	sendfunction(LoadTexture);

	sendfunction(e_spawn);
	sendfunction(e_spawnat);
	sendfunction(e_spawnatmap);
	sendfunction(e_findfirst);
	sendfunction(e_findnext);
	sendfunction(e_ptr2uid);
	sendfunction(e_uid2ptr);
	sendfunction(e_kill);
	sendfunction(e_damage);
	sendfunction(control_switch);
	sendfunction(gettileat);
	sendfunction(settileat);

	sendfunction(wrap_map_coord_x);
	sendfunction(wrap_map_coord_y);

	sendfunction(LoadLevel);

	sendfunction(sndPrecache);
	sendfunction(sndFree);
	sendfunction(sndPlay);
	sendfunction(sndSetVolume);
	sendfunction(sndIsPlaying);
	sendfunction(sndStop);

	// Pointers to the original variables:
	sendvariable(map);

	// Entity who has control of input:
	sendvariable(player);
	sendvariable(world);
	host->entities = entities;
	sendvariable(last_entity);
	
	// Send address of acceleration scalar value (from joystick)  where (0 <= x <= 1.0):
	host->accel_scale = &accel_scale;

	// Game state variables:
	sendvariable(screen_mx);
	sendvariable(screen_my);
	sendvariable(old_screen_mx);
	sendvariable(old_screen_my);
	sendvariable(screen_w);
	sendvariable(screen_h);
	sendvariable(scroll_left);
	sendvariable(scroll_right);
	sendvariable(scroll_top);
	sendvariable(scroll_bottom);
	sendvariable(quit);
	sendvariable(game_paused);

	sendvariable(world);

	host->devmode = 0;
}

int LoadGameDLL(const char *gamename) {
	// Free previously loaded DLL:
	if (game != NULL) {
		free(game);
		game = NULL;
		UnloadGame(&game_library);
	}

	// Send our copy, now receive their copy:
	game = GetGameAPI(gamename, host, &game_library);
	if (game == NULL) return -1;

	class_properties = game->class_properties;

	return 0;
}

int LoadClientDLL(const char *gamename) {
	// Quick macro to set the function pointers:
	#define sendfunction(func) host->func = func
	#define sendvariable(var) host->var = &var

	// Send our copy, now receive their copy:
	client = GetGameAPI(gamename, host, &client_library);
	if (client == NULL) return -1;

	return 0;
}

// Main game loop:
void gameloop() {
	int		i = 0;
	unsigned char control_keys = 0, old_control_keys = 0;
	const char	*gamename = "bmgame";
	entity		e;

	sound_available = sndInit();

	// Set screen dimensions:
	scroll_left = (screen_w * 0.3125);
	scroll_right = (screen_w * 0.6875);
	scroll_top = (screen_h * 0.3125);
	scroll_bottom = (screen_h * 0.6875);
	quit = 0;
	game_paused = 0;

	// Allocate the entity-storage array:
	last_entity = -1;
	entities = malloc(sizeof(entity_t *) * MAX_ENTITIES);
	for (i=0;i<MAX_ENTITIES;i++)
		entities[i] = NULL;

	// Initialize the map cache properly:
	cache_head = NULL;
	cache_tail = &(cache_head);

	// Set initial settings for a blank map so that we have a
	// consistent basis to check pointers with.
	InitMap();

	// Create our host instance:
	create_host();

	// Load the client DLL:
	if (LoadClientDLL(gamename) == -1) return;

	// Call the client DLL's init function:
	client->init();

	if (game == NULL) {
		fprintf(stderr, "Game DLL not loaded from client DLL!\n");
		return;
	}

	// Spawn world entity:
	world = calloc(sizeof(entity_t), 1);

	if (map.map_loaded == 0) {
		fprintf(stderr, "You must load a map in the init() function!\n");
		return;
	}

	// Main game loop:
	while (quit == 0) {
		sys_clearscreen();

		// Read the keyboard:
		old_control_keys = control_keys;

		// Call the system-specific function to fill the control_keys bitflags:
		sys_eventloop(&control_keys);

		// Toggle pausing:
		if ((control_keys & BUT_PAUSE) && !(old_control_keys & BUT_PAUSE)) game_paused = !game_paused;

		// Pass the control_keys on to the player entity:
		if (player) player->control_keys = control_keys;

		// Draw things before drawing entities:
		if (game->pre_render) game->pre_render();

		// Think for and draw each entity:
		process_entities();

		// Draw things after drawing entities:
		if (game->post_render) game->post_render();

		sys_updatescreen();
	}

	if ( sound_available) {
/*		if ( music != NULL )
			Mix_FreeMusic(music);
*/
		sndClose();
	}

	// Free up all our memory:
	for (i=0;i<MAX_ENTITIES;i++)
		if (entities[i] != NULL) {
			free(entities[i]);
			entities[i] = NULL;
		}

	free(entities);
	
	UnloadGame(&client_library);
	UnloadGame(&game_library);

	return;
}

int main( int argc, char *argv[] ) {
	// Initialize the system-dependent stuff (OpenGL & SDL):
	sys_init(argc, argv);

	// Run the main game loop:
	gameloop();

	// Close the system-dependent stuff (SDL):
	sys_close();

	return 0;
}
