#include <stdarg.h>
#include "common.h"
#include "opengl.h"
#include "entflags.h"
#include "newmap.h"
#include "bm_game.h"
#include "font.h"
#include "glwindows.h"

long	screen_mx, screen_my;
long	screen_w, screen_h;
int		quit, game_paused;
int		selected_tile, select_top, select_height, select_width;
int		box_x1, box_y1, box_x2, box_y2;
Uint8	*clipboard;
Uint16	clipboard_w, clipboard_h;
Uint32	numclasses;

int		dx = 0, dy = 0;

mapclass_properties_t	**mapclass_properties;
void	*game_library = NULL;

dllfunctions		*game;
hostfunctions		*host;

typedef enum {
	OPERATION_NONE,
	OPERATION_SELECTBOX,
	OPERATION_SELECTEDBOX,
	OPERATION_PLACEENTITY,
	OPERATION_MOVEENTITY
} operations;

TWindow	*wMain, *wTiles, *wEnts, *wDoors, *wEntSelect;
TButton	*btnAddWindow, *btnModify, *btnRemove;
TButton	*btnAdd;
TButton	*btnNextDoor, *btnPrevDoor;
TButton	*btnStageUp, *btnStageDn, *btnTagUp, *btnTagDn;
TLabel	*lblStage, *lblTag;

int		door_stage, door_tag, door_selected;

int		level = 0;
Uint16	mx = 0, my = 0, tx = 0, ty = 0;
Uint8	left_clicked = 0, right_clicked = 0;
Uint8	*p;
operations	operation;
SDLMod		keymods;
unsigned long	cur_entity = -1;

char *format(char *fmt, ...) {
	va_list	vl;
	char	tempbuf[8192];
	char	*result;

	va_start(vl, fmt);
	vsprintf(tempbuf, fmt, vl);
	va_end(vl);

	result = calloc(sizeof(char), strlen(tempbuf)+1);
	strcpy(result, tempbuf);

	return result;
}

void GenerateRadioButtons();

void LoadLevel(const char *filename) {
	FreeTextures();
	LoadMap(filename);
	if (map.game_filename) {
		if (game) UnloadGame(&game_library);
		// Extend this to load per-level DLLs:
		LoadGameDLL(map.game_filename);
		if ((game) && (game->init)) {
			game->init();
		} else {
			LoadTexture(0, "textures/fgtile000.png");
			LoadTexture(1, "textures/fgtile001.png");
			LoadTexture(2, "textures/fgtile002.png");
			LoadTexture(3, "textures/fgtile003.png");
		}
	}
	InitTextures();
	// Re-generate the list of radio-buttons for the entity-select window:
	if (wEntSelect) GenerateRadioButtons();
	door_selected = -1;
	door_tag = 0;
	door_stage = 0;
}

int LoadGameDLL(const char *gamename) {
	int	i, j;

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

	sendfunction(LoadTexture);

	// Notify the DLL that the map editor is connecting to it
	// so it doesn't whine about missing functions.
	host->devmode = 1;

	// Send our copy, now receive their copy:
	game = GetGameAPI(gamename, host, &game_library);
	if (game == NULL) {
		mapclass_properties = NULL;
		class_properties = NULL;
		numclasses = 0;
		return -1;
	}

	// Get the classes:
	class_properties = game->class_properties;

	// Now, size them down to map-spawnable classes:
	if (!game->spawnflags) {
		fprintf(stderr, "Spawnflags were not sent from the DLL!\n");
		return -1;
	}

	// Count number of map-spawnable classes:
	numclasses = 0;
	for (i=0; i<game->numclasses; ++i)
		if (game->spawnflags[i] & SPAWNFLAG_MAPSPAWNABLE)
			++numclasses;

	// Allocate memory for the list of map-spawnable classes:
	mapclass_properties = calloc(sizeof(mapclass_properties_t *) * numclasses, 1);

	j = 0;
	for (i=0; i<game->numclasses; ++i)
		if (game->spawnflags[i] & SPAWNFLAG_MAPSPAWNABLE) {
			mapclass_properties[j] = malloc(sizeof(mapclass_properties_t));
			mapclass_properties[j]->class = i;
			mapclass_properties[j]->size = class_properties[i]->size;
			mapclass_properties[j]->name = class_properties[i]->name;
			mapclass_properties[j]->preview = class_properties[i]->preview;
			++j;
		}

	return 0;
}

void DrawMapSquare(int x, int y, GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor4f(r, g, b, a);
	glBegin(GL_QUADS);
	glVertex2i((x<<4)   , (y<<4)   );
	glVertex2i((x<<4)+16, (y<<4)   );
	glVertex2i((x<<4)+16, (y<<4)+16);
	glVertex2i((x<<4)   , (y<<4)+16);
	glEnd();
}

void updateSelectedTile() {
	if (map.mapflags[selected_tile] & MAPFLAG_JASON)	wTiles->CheckBox[0]->Checked = TRUE; else wTiles->CheckBox[0]->Checked = FALSE;
	if (map.mapflags[selected_tile] & MAPFLAG_DOOR)		wTiles->CheckBox[1]->Checked = TRUE; else wTiles->CheckBox[1]->Checked = FALSE;
	if (map.mapflags[selected_tile] & MAPFLAG_HARMFUL)	wTiles->CheckBox[2]->Checked = TRUE; else wTiles->CheckBox[2]->Checked = FALSE;
	if (map.mapflags[selected_tile] & MAPFLAG_LADDER)	wTiles->CheckBox[3]->Checked = TRUE; else wTiles->CheckBox[3]->Checked = FALSE;
	if (map.mapflags[selected_tile] & MAPFLAG_WATER)	wTiles->CheckBox[4]->Checked = TRUE; else wTiles->CheckBox[4]->Checked = FALSE;
	if (map.mapflags[selected_tile] & MAPFLAG_SOLID)	wTiles->CheckBox[5]->Checked = TRUE; else wTiles->CheckBox[5]->Checked = FALSE;
}

void updateDoorSelected() {
	if (door_selected < -1) door_selected = map.num_doors - 1;
	if (door_selected >= map.num_doors) door_selected = -1;
	if (door_selected != -1) {
		door_tag = map.doors[door_selected]->tag;
		lblTag->Caption = format("0x%02X", door_tag);
		lblStage->Caption = map.doors[door_selected]->targetmap;

		wDoors->Title = format("Door #%d", door_selected);
	} else {
		door_tag = 0;
		wDoors->Title = "Doors";
	}
}

void wMain_Click(TWindow *self, int mX, int mY, int btn) {
	Uint16	tx, ty;
	int		x, y, t;
	int		mx, my;
	int		i;
	static	int dragx, dragy;

	if ((mX <= 6) || (mY <= 27) || (mX > (screen_w << 4)+6) || (mY > (screen_h << 4)+27)) return;

	mx = mX - 6;
	my = mY - 27;
	mx >>= 4;	my >>= 4;
	tx = mx + screen_mx; ty = my + screen_my;

#if 0
	fprintf(stderr, "{0x%02X, 0x%02X} %d  keys=", tx, ty, btn);
	(keymods & KMOD_SHIFT) ? fprintf(stderr, "S") : fprintf(stderr, "s");
	(keymods & KMOD_CTRL)  ? fprintf(stderr, "C") : fprintf(stderr, "c");
	(keymods & KMOD_ALT)   ? fprintf(stderr, "A") : fprintf(stderr, "a");
	fprintf(stderr, "   op=");
	if (operation == OPERATION_NONE) fprintf(stderr, "OPERATION_NONE");
	if (operation == OPERATION_SELECTBOX) fprintf(stderr, "OPERATION_SELECTBOX");
	if (operation == OPERATION_SELECTEDBOX) fprintf(stderr, "OPERATION_SELECTEDBOX");
	if (operation == OPERATION_MOVEENTITY) fprintf(stderr, "OPERATION_MOVEENTITY");
	fprintf(stderr, "\n");
#endif
	
	// Left click:
	if ( btn == 1 ) {
		if ((keymods & KMOD_SHIFT) && ((operation == OPERATION_NONE) || (operation == OPERATION_SELECTEDBOX))) {
			operation = OPERATION_SELECTBOX;
			// Select no current tile:
			selected_tile = -1;
			cur_entity = -1;
			box_x1 = box_x2 = tx;
			box_y1 = box_y2 = ty;
		} else if ((keymods & KMOD_SHIFT) && (operation == OPERATION_SELECTBOX)) {
			box_x2 = tx; box_y2 = ty;
		}

		// Ctrl+click selects an entity:
		if ((keymods & KMOD_CTRL) && ((operation == OPERATION_NONE) || (operation == OPERATION_SELECTEDBOX))) {
			selected_tile = -1;
			cur_entity = -1;
			if (map.num_entities > 0) {
				for (i=0; i<map.num_entities; ++i) {
					if ((tx == (map.entities[i]->x >> 5)) && (ty == (map.entities[i]->y >> 5))) {
						cur_entity = i;
						break;
					}
				}
				if (cur_entity != -1) {
					operation = OPERATION_MOVEENTITY;
					wEnts->Title = format("Entity #%d", cur_entity);
					if (mapclass_properties) {
						//dragx = ((map.entities[cur_entity]->x >> 1) - (screen_mx << 4)) - (mX - 6);
						//dragy = ((map.entities[cur_entity]->y >> 1) - (screen_my << 4)) - (mY - 27);
						// Select the radiobutton in the entity-select window:
						for (i=0; i<numclasses; ++i) if (mapclass_properties[i] != NULL) {
							if (mapclass_properties[i]->class == map.entities[cur_entity]->class) {
								// Fake a click-event on the radio button:
								if (i < wEntSelect->numRadioButtons)
									TRadioButton_Click(wEntSelect->RadioButton[i]);
								break;
							}
						}
					}
				}
			}
		} else if ((keymods & KMOD_CTRL) && (operation == OPERATION_MOVEENTITY)) {
			// Move the entity around:
			if ((cur_entity >= 0) && (cur_entity < map.num_entities)) {
				// Absolute position (not snapped to tile-grid):
				//map.entities[cur_entity]->x = ((dragx + mX - 6) + (screen_mx << 4)) << 1;
				//map.entities[cur_entity]->y = ((dragy + mY - 27) + (screen_my << 4)) << 1;
				// Align to map tile:
				map.entities[cur_entity]->x = tx << 5;
				map.entities[cur_entity]->y = ty << 5;
			}
		}

		if (keymods == KMOD_NONE) {
			// Left clicking places the selected tile on the map:
			if (( selected_tile != -1 ) && ( operation == OPERATION_NONE ))
				settileat(tx << 5, ty << 5, selected_tile);
			operation = OPERATION_NONE;
			cur_entity = -1;
		}
	}

	// Right click:
	if ( btn != 1 ) {
		// Grabs the tile under the mouse:
		if (keymods == KMOD_NONE) {
			operation = OPERATION_NONE;
			selected_tile = gettileat(tx << 5, ty << 5);
			updateSelectedTile();

			// Check if it's over a door:
			if (map.mapflags[gettileat(tx << 5, ty << 5)] & MAPFLAG_DOOR) {
				for (i=0; i<map.num_doors; ++i)
					if ((tx == map.doors[i]->x) && (ty == map.doors[i]->y)) {
						door_selected = i;
						updateDoorSelected();
						break;
					}
			}
		}
	}
}

void draw_vline_on_screen(int x, int y1, int y2, int offx, int offy) {
	int	vx, vy1, vy2;

	if ((x < screen_mx) || (x > (screen_mx + screen_w))) return;

	vx = ((x - screen_mx) << 4) + offx;
	vy1 = ((y1 - screen_my) << 4);
	vy2 = ((y2 - screen_my) << 4) + offy;

	if (vy1 < 0) vy1 = 0;
	if (vy2 > (screen_h << 4)) vy2 = screen_h << 4;

	glBindTexture(GL_TEXTURE_2D, 0);
	glColor4f(0.7, 0.12, 0.5, 0.6);
	glLineWidth(3.0);
	glBegin(GL_LINES);
		glVertex2i(vx, vy1);
		glVertex2i(vx, vy2);
	glEnd();
}

void draw_hline_on_screen(int y, int x1, int x2, int offx, int offy) {
	int	vy, vx1, vx2;

	if ((y < screen_my) || (y > (screen_my + screen_h))) return;

	vy = ((y - screen_my) << 4) + offy;
	vx1 = ((x1 - screen_mx) << 4);
	vx2 = ((x2 - screen_mx) << 4) + offx;

	if (vx1 < 0) vx1 = 0;
	if (vx2 > (screen_w << 4)) vx2 = screen_w << 4;

	glBindTexture(GL_TEXTURE_2D, 0);
	glColor4f(0.7, 0.12, 0.5, 0.6);
	glLineWidth(3.0);
	glBegin(GL_LINES);
		glVertex2i(vx1, vy);
		glVertex2i(vx2, vy);
	glEnd();
}

void wMain_Paint(TWindow *self) {
	int	x, y, i, t;
	int	mx, my;

	mx = (Mouse.X - self->X - 6) >> 4;
	my = (Mouse.Y - self->Y - 27) >> 4;

	for (y=0; y<screen_h; ++y) {
		if (y + screen_my >= map.height) break;
		for (x=0; x<screen_w; ++x) {
			if (x + screen_mx >= map.width) break;
			t = map.map[((screen_my + y) * map.width) | (screen_mx + x)];
			Draw2x2BGTile(x << 4, y << 4, t);
		}
	}

	// Move above the map...
	glTranslatef(0, 0, 0.1);

	// Draw regions:
	if (map.num_regions > 0) {
		for (i=0; i<map.num_regions; ++i) {
			if (map.regions[i]->ty > map.regions[i]->by) {
				draw_vline_on_screen(map.regions[i]->lx, map.regions[i]->ty, map.height, 0, 15);
				draw_vline_on_screen(map.regions[i]->rx, map.regions[i]->ty, map.height, 15, 15);
				draw_vline_on_screen(map.regions[i]->lx, 0, map.regions[i]->by, 0, 15);
				draw_vline_on_screen(map.regions[i]->rx, 0, map.regions[i]->by, 15, 15);
			} else {
				draw_vline_on_screen(map.regions[i]->lx, map.regions[i]->ty, map.regions[i]->by, 0, 15);
				draw_vline_on_screen(map.regions[i]->rx, map.regions[i]->ty, map.regions[i]->by, 15, 15);
			}
			if (map.regions[i]->lx > map.regions[i]->rx) {
				draw_hline_on_screen(map.regions[i]->ty, map.regions[i]->lx, map.width, 15, 0);
				draw_hline_on_screen(map.regions[i]->by, map.regions[i]->lx, map.width, 15, 15);
				draw_hline_on_screen(map.regions[i]->ty, 0, map.regions[i]->rx, 15, 0);
				draw_hline_on_screen(map.regions[i]->by, 0, map.regions[i]->rx, 15, 15);
			} else {
				draw_hline_on_screen(map.regions[i]->ty, map.regions[i]->lx, map.regions[i]->rx, 15, 0);
				draw_hline_on_screen(map.regions[i]->by, map.regions[i]->lx, map.regions[i]->rx, 15, 15);
			}
		}
	}

	// Draw door selector:
	if ((map.num_doors > 0) && (door_selected >= 0) && (door_selected < map.num_doors)) {
		i = door_selected;
		if ( (map.doors[i]->x >= screen_mx) && (map.doors[i]->y >= screen_my) &&
			 (map.doors[i]->x < screen_mx+screen_w) && (map.doors[i]->y < screen_my+screen_h) )
		{
			x = (map.doors[i]->x - screen_mx) << 4;
			y = (map.doors[i]->y - screen_my) << 4;
			glBindTexture(GL_TEXTURE_2D, 0);
			glColor4f(0.0, 1.0, 0.0, 0.6);
			glLineWidth(2.0);
			glBegin(GL_LINE_LOOP);
				glVertex2i(x,	y);
				glVertex2i(x+16, y);
				glVertex2i(x+16, y+16);
				glVertex2i(x,	y+16);
			glEnd();
		}
	}

	// Draw entities on the screen:
	for (i=0; i<map.num_entities; ++i) {
		if (((map.entities[i]->x >> 5) >= screen_mx) && ((map.entities[i]->y >> 5) >= screen_my) &&
			((map.entities[i]->x >> 5) < screen_mx + screen_w) && ((map.entities[i]->y >> 5) < screen_my + screen_h))
		{
			x = (map.entities[i]->x >> 1) - (screen_mx << 4);
			y = (map.entities[i]->y >> 1) - (screen_my << 4);
			if (class_properties) {
				if (class_properties[map.entities[i]->class]) {
					if (class_properties[map.entities[i]->class]->preview) {
						class_properties[map.entities[i]->class]->preview(x, y, map.entities[i]->class);
					} else {
						glBindTexture(GL_TEXTURE_2D, 0);
						glColor4f(0.2, 0.0, 1.0, 0.5);
						glBegin(GL_QUADS);
							glVertex2i(x,	y);
							glVertex2i(x+16, y);
							glVertex2i(x+16, y+16);
							glVertex2i(x,	y+16);
						glEnd();
						glWrite(x, y, format("%02X", map.entities[i]->class));
					}
				} else {
					glBindTexture(GL_TEXTURE_2D, 0);
					glColor4f(0.2, 0.0, 1.0, 0.5);
					glBegin(GL_QUADS);
						glVertex2i(x,	y);
						glVertex2i(x+16, y);
						glVertex2i(x+16, y+16);
						glVertex2i(x,	y+16);
					glEnd();
					glWrite(x, y, format("%02X", map.entities[i]->class));
				}
			} else {
				glBindTexture(GL_TEXTURE_2D, 0);
				glColor4f(0.2, 0.0, 1.0, 0.5);
				glBegin(GL_QUADS);
					glVertex2i(x,	y);
					glVertex2i(x+16, y);
					glVertex2i(x+16, y+16);
					glVertex2i(x,	y+16);
				glEnd();
				glWrite(x, y, format("%02X", map.entities[i]->class));
			}
			if (i == cur_entity) {
				glTranslatef(0, 0, 0.1);
				glBindTexture(GL_TEXTURE_2D, 0);
				glColor4f(1.0, 1.0, 1.0, 0.5);
				glLineWidth(1.0);
				glBegin(GL_LINE_LOOP);
					glVertex2i(x,	y);
					glVertex2i(x+16, y);
					glVertex2i(x+16, y+16);
					glVertex2i(x,	y+16);
				glEnd();
				glTranslatef(0, 0, -0.1);
			}
		}
	}

	if ((mx >= 0) && (my >= 0) && (mx < screen_w) && (my < screen_h)) {
		if (operation == OPERATION_NONE) {
			// Draw the current tile box:
			glBindTexture(GL_TEXTURE_2D, 0);
			glTranslatef(0, 0, 0.1);
			glColor4f(1.0, 0.0, 0.2, 1.0);
			glLineWidth(1);
			glBegin(GL_LINE_LOOP);
				glVertex2i((mx<<4)   , (my<<4)   );
				glVertex2i((mx<<4)+16, (my<<4)   );
				glVertex2i((mx<<4)+16, (my<<4)+16);
				glVertex2i((mx<<4)   , (my<<4)+16);
			glEnd();
		}

		// Write the mouse position:
		self->StatusBar = format("{0x%02X, 0x%02X}", mx + screen_mx, my + screen_my);
	}
}

int wMain_KeyDown(SDL_KeyboardEvent *kev) {
	int		i;
	int		x, y, t;
	int		maxlen, len;
	if (kev->keysym.mod & KMOD_CTRL) {
		switch ( kev->keysym.sym ) {
			case SDLK_c:	// CTRL+C copies the selection box:
				if ( (operation == OPERATION_SELECTEDBOX) ) {
					// Fill clipboard with the data:
					clipboard_w = (box_x2 - box_x1) + 1;
					clipboard_h = (box_y2 - box_y1) + 1;
					if (clipboard) free(clipboard);
					clipboard = malloc(sizeof(Uint8) * clipboard_w * clipboard_h);
					if (clipboard) {
						p = clipboard;
						for (y=0; y<clipboard_h; ++y)
							for (x=0; x<clipboard_w; ++x)
								*p++ = gettileat((x+box_x1) << 5, (y+box_y1) << 5);
					}
				}
				break;
			case SDLK_v:	// CTRL+V pastes the clipboard onto the map:
				if (clipboard) {
					p = clipboard;
					for (y=0; y<clipboard_h; ++y) {
						if (y+my+screen_my >= map.height) break;
						for (x=0; x<clipboard_w; ++x) {
							// DO NOT break HERE.  We must p++.
							if (x+mx+screen_mx >= map.width) { p++; continue; }
								settileat((x+mx+screen_mx) << 5, (y+my+screen_my) << 5, *p++);
						}
					}
				}
				break;
			case SDLK_s:	// CTRL+S saves the map
				SaveMap(format("maps/map%02X.bma", level));
				break;
			case SDLK_l:	// CTRL+L loads the map
				LoadMap(format("maps/map%02X.bma", level));
				break;
			default:
				return 0;
		}
		return -1;
	} else if (kev->keysym.mod & KMOD_SHIFT) {
		switch ( kev->keysym.sym ) {
			case SDLK_F1:
				LoadLevel("maps/map08.bma"); level = 8;
				break;
			case SDLK_F2:
				LoadLevel("maps/map09.bma"); level = 9;
				break;
			case SDLK_F3:
				LoadLevel("maps/map0A.bma"); level = 10;
				break;
			case SDLK_F4:
				LoadLevel("maps/map0B.bma"); level = 11;
				break;
			case SDLK_F5:
				LoadLevel("maps/map0C.bma"); level = 12;
				break;
			case SDLK_F6:
				LoadLevel("maps/map0D.bma"); level = 13;
				break;
			case SDLK_F7:
				LoadLevel("maps/map0E.bma"); level = 14;
				break;
			case SDLK_F8:
				LoadLevel("maps/map0F.bma"); level = 15;
				break;
			default:
				return 0;
		}
		return -1;
	} else {
		switch ( kev->keysym.sym ) {
			case SDLK_ESCAPE:	quit = 1; break;
			case SDLK_LEFT:		dx = -1; break;
			case SDLK_RIGHT:	dx =  1; break;
			case SDLK_UP:		dy = -1; break;
			case SDLK_DOWN:		dy =  1; break;
			case SDLK_RETURN:
				if ( kev->keysym.mod & KMOD_ALT )
					sys_togglefullscreen();
				break;
			case SDLK_F1:
				LoadLevel("maps/map00.bma"); level = 0;
				break;
			case SDLK_F2:
				LoadLevel("maps/map01.bma"); level = 1;
				break;
			case SDLK_F3:
				LoadLevel("maps/map02.bma"); level = 2;
				break;
			case SDLK_F4:
				LoadLevel("maps/map03.bma"); level = 3;
				break;
			case SDLK_F5:
				LoadLevel("maps/map04.bma"); level = 4;
				break;
			case SDLK_F6:
				LoadLevel("maps/map05.bma"); level = 5;
				break;
			case SDLK_F7:
				LoadLevel("maps/map06.bma"); level = 6;
				break;
			case SDLK_F8:
				LoadLevel("maps/map07.bma"); level = 7;
				break;
			default:
				return 0;
		}
		// Successfully handled the key, propogate no further
		return -1;
	}
	// Didn't handle the key, keep propogating down the window chain
	return 0;
}

int wMain_KeyUp(SDL_KeyboardEvent *kev) {
	int		i;
	int		x, y, t;
	int		maxlen, len;

	switch ( kev->keysym.sym ) {
		case SDLK_ESCAPE:	quit = 1; break;
		case SDLK_LEFT:		if (dx == -1) dx = 0; break;
		case SDLK_RIGHT:	if (dx == 1)  dx = 0; break;
		case SDLK_UP:		if (dy == -1) dy = 0; break;
		case SDLK_DOWN:		if (dy == 1)  dy = 0; break;
		case SDLK_RETURN:
			if ( kev->keysym.mod & KMOD_ALT )
				sys_togglefullscreen();
			break;
		default:
  			return 0;
	}
	return -1;
}

void wTiles_Click(TWindow *self, int mx, int my, int btn) {
	int		t;
	mx -= 6;	my -= 27;
	if ( btn == 1 ) {
		t = select_top + ((my >> 4) * 16) + (mx >> 4);
		if (t < map.map2x2_size) selected_tile = t;
		updateSelectedTile();
	}
}

// Paints the tile-selector window:
void wTiles_Paint(TWindow *self) {
	int		x, y, t;

	t = select_top;
	for (y=0; y<select_height; ++y) {
		for (x=0; x<select_width; ++x, ++t) {
			if ((t >= 0) && (t < map.map2x2_size))
				Draw2x2BGTile(x << 4, y << 4, select_top + (y * select_width) + x);
			// Draw the selected tile marker:
			if (t == selected_tile) {
				glPushMatrix();
					glBindTexture(GL_TEXTURE_2D, 0);
					glColor4f(1.0, 1.0, 1.0, 0.5);
					glTranslatef(0, 0, 0.1);
					glLineWidth(2.0);
					glBegin(GL_LINE_LOOP);
						glVertex2i((x<<4)   , (y<<4)   );
						glVertex2i((x<<4)+16, (y<<4)   );
						glVertex2i((x<<4)+16, (y<<4)+16);
						glVertex2i((x<<4)   , (y<<4)+16);
					glEnd();
				glPopMatrix();
			}
		}
	}
}

// Called when the user clicks on one of the mapflag checkboxes:
void chkMapflag_Click(TCheckBox *self) {
	if ((selected_tile >= 0) && (selected_tile < map.map2x2_size)) {
		switch (self->Index) {
			case 0:
				if (self->Checked) map.mapflags[selected_tile] |= MAPFLAG_JASON;
				else map.mapflags[selected_tile] &= ~MAPFLAG_JASON;
				break;
			case 1:
				if (self->Checked) map.mapflags[selected_tile] |= MAPFLAG_DOOR;
				else map.mapflags[selected_tile] &= ~MAPFLAG_DOOR;
				break;
			case 2:
				if (self->Checked) map.mapflags[selected_tile] |= MAPFLAG_HARMFUL;
				else map.mapflags[selected_tile] &= ~MAPFLAG_HARMFUL;
				break;
			case 3:
				if (self->Checked) map.mapflags[selected_tile] |= MAPFLAG_LADDER;
				else map.mapflags[selected_tile] &= ~MAPFLAG_LADDER;
				break;
			case 4:
				if (self->Checked) map.mapflags[selected_tile] |= MAPFLAG_WATER;
				else map.mapflags[selected_tile] &= ~MAPFLAG_WATER;
				break;
			case 5:
				if (self->Checked) map.mapflags[selected_tile] |= MAPFLAG_SOLID;
				else map.mapflags[selected_tile] &= ~MAPFLAG_SOLID;
				break;
		}
	}
}

void btnAdd_Click(TButton *self) {
	int		i;
	TWindow	*w = self->Parent;

	if (mapclass_properties) {
		for (i=0; i<w->numRadioButtons; ++i)
			if (w->RadioButton[i]->Checked) {
				// Resize the array for more room:
				map.entities = realloc(map.entities, sizeof(mapentity_t *) * ++map.num_entities);
				cur_entity = map.num_entities - 1;
				map.entities[cur_entity] = calloc(sizeof(mapentity_t), 1);
				map.entities[cur_entity]->class = mapclass_properties[i]->class;
				map.entities[cur_entity]->x = (screen_mx << 5) + (screen_w << 4);
				map.entities[cur_entity]->y = (screen_my << 5) + (screen_h << 4);
				// Found the selected radio button, we can bail now:
				w->Visible = FALSE;
				break;
			}
	}
}

void btnAddWindow_Click(TButton *self) {
	wEntSelect->Visible = TRUE;
}

void btnRemove_Click(TButton *self) {
	int		i;

	if ((cur_entity < 0) || (cur_entity >= map.num_entities)) return;

	// Removes cur_entity:
	free(map.entities[cur_entity]);
	// Realigns data:
	for (i=cur_entity+1; i<map.num_entities; ++i)
		map.entities[i-1] = map.entities[i];

	map.entities = realloc(map.entities, sizeof(mapentity_t *) * --map.num_entities);
	cur_entity--;
}

void rbEntity_Click(TRadioButton *self) {
	if ((cur_entity < 0) || (cur_entity >= map.num_entities)) return;

	// Reassign the class-type for the currently selected entity:
	map.entities[cur_entity]->class = mapclass_properties[self->Index]->class;
}

void btnNextDoor_Click(TButton *self) {
	door_selected++;
	updateDoorSelected();
	if (door_selected != -1) {
		screen_mx = map.doors[door_selected]->x - (screen_w / 2);
		screen_my = map.doors[door_selected]->y - (screen_h / 2);

		if (screen_mx < 0) screen_mx = 0;
		if (screen_my < 0) screen_my = 0;
		if (screen_mx + screen_w >= map.width) screen_mx = map.width - screen_w;
		if (screen_my + screen_h >= map.height) screen_my = map.height - screen_h;
	}
}

void btnPrevDoor_Click(TButton *self) {
	door_selected--;
	updateDoorSelected();
	if (door_selected != -1) {
		screen_mx = map.doors[door_selected]->x - (screen_w / 2);
		screen_my = map.doors[door_selected]->y - (screen_h / 2);

		if (screen_mx < 0) screen_mx = 0;
		if (screen_my < 0) screen_my = 0;
		if (screen_mx + screen_w >= map.width) screen_mx = map.width - screen_w;
		if (screen_my + screen_h >= map.height) screen_my = map.height - screen_h;
	}
}

void btnStageUp_Click(TButton *self) {
	if (door_selected == -1) return;

	door_stage++;
	if (door_stage > 16) door_stage = 0;
	if (door_stage == 0) {
		lblStage->Caption = map.doors[door_selected]->targetmap = NULL;
	} else {
		lblStage->Caption = map.doors[door_selected]->targetmap = format("maps/map%02X.bma", door_stage - 1);
	}
}

void btnStageDn_Click(TButton *self) {
	if (door_selected == -1) return;
	
	door_stage--;
	if (door_stage < 0) door_stage = 16;
	if (door_stage == 0)
		lblStage->Caption = map.doors[door_selected]->targetmap = NULL;
	else
		lblStage->Caption = map.doors[door_selected]->targetmap = format("maps/map%02X.bma", door_stage - 1);
}

void btnTagUp_Click(TButton *self) {
	if (door_selected == -1) return;

	door_tag++;
	lblTag->Caption = format("0x%02X", door_tag);
	map.doors[door_selected]->tag = door_tag;
}

void btnTagDn_Click(TButton *self) {
	if (door_selected == -1) return;

	door_tag--;
	if (door_tag < 0) door_tag = 0;
	lblTag->Caption = format("0x%02X", door_tag);
	map.doors[door_selected]->tag = door_tag;
}

void GenerateRadioButtons() {
	int		x, y, i;
	int		maxlen, len, maxy;
	TRadioButton	*rb;
	char	*label;

	// Clear out the old radio buttons:
	TWindow_RemoveAllRadioButtons(wEntSelect);
	// Add radio-buttons for map-spawnable classes:
	x = 12;	y = 64;
	maxlen = 0; maxy = y;
	if (mapclass_properties) {
		for (i=0; i<numclasses; ++i) {
			label = format("[0x%02X] %s", (int) mapclass_properties[i]->class, mapclass_properties[i]->name);
			len = fontWidth(label);
			if (len > maxlen) maxlen = len;
			rb = TWindow_AddRadioButton(wEntSelect, x, y, 1, FALSE, label);
	
			y += 16;
			if (y > maxy) maxy = y;
			
			// Wrap around to the next column:
			if (y >= 384+27-16) {
				y = 32;
				x += maxlen + 8;
				maxlen = 0;
			}
		}
	}
	
	if (x == 12) x = 96;
	
	// Resize the entity-select window:
	wEntSelect->Width = x + maxlen + 8 + 12 + 12;
	wEntSelect->Height = maxy + 27;
}

// Main game loop:
void gameloop() {
	SDL_Event	event;
	int		i;
	int		x, y, t;
	int		maxlen, len, maxy;
	char	*label;

	// Set screen dimensions:
	screen_w = 46;	screen_h = 44;
	quit = 0; game_paused = 0;

	// Initialize pointers and counters for the map structure:
	InitMap();

	// Create a new map:
	LoadLevel(format("maps/map%02X.bma", level));

	selected_tile = -1;
	left_clicked = 0; right_clicked = 0;

	clipboard = NULL;
	operation = OPERATION_NONE;

	// The current screen location:
	screen_mx = 0;	screen_my = 0;
	select_top = -1;
	select_height = 16; select_width = 16;

	// --------------- Setup the glWindows module ------------------

	Window.Width = 1024;
	Window.Height = 768;
	Window.CaptionBarHeight = 26;

	// Create the main window
	wMain = calloc(sizeof(TWindow), 1);
	TWindow_Init(wMain, 0, 0, (screen_w << 4)+12, (screen_h << 4)+27+26, "Map Editor");
	wMain->onClick = wMain_Click;
	wMain->Paint = wMain_Paint;
	wMain->onKeyDown = wMain_KeyDown;
	wMain->onKeyUp = wMain_KeyUp;

	// Create the setup window
	wTiles = calloc(sizeof(TWindow), 1);
	TWindow_Init(wTiles, Window.Width - 274, 0, 256+12, 256+48+27+26, "Tile Chooser");
	TWindow_AddCheckBox(wTiles, 32, 256+27   , FALSE, "Jason");
	TWindow_AddCheckBox(wTiles, 32, 256+27+16, FALSE, "Door");
	TWindow_AddCheckBox(wTiles, 32, 256+27+32, FALSE, "Harmful");
	TWindow_AddCheckBox(wTiles, 96, 256+27   , FALSE, "Ladder");
	TWindow_AddCheckBox(wTiles, 96, 256+27+16, FALSE, "Water");
	TWindow_AddCheckBox(wTiles, 96, 256+27+32, FALSE, "Solid");
	wTiles->ZOrder = 1;
	wTiles->onClick = wTiles_Click;
	wTiles->Paint = wTiles_Paint;
	// Set the onClick method for each checkbox:
	for (i=0; i<6; ++i) wTiles->CheckBox[i]->onClick = chkMapflag_Click;
	TWindow_AddWindow(wMain, wTiles);


	// Create the entities window:
	wEnts = calloc(sizeof(TWindow), 1);
	TWindow_Init(wEnts, Window.Width - 274, 357, 256+12, 34+27+26, "Entities");
	btnAddWindow = TWindow_AddButton(wEnts, 20, 32, 64, 24, "Add...");
	btnRemove	= TWindow_AddButton(wEnts, 188, 32, 64, 24, "Remove");
	btnAddWindow->onClick = btnAddWindow_Click;
	btnRemove->onClick = btnRemove_Click;
	wEnts->ZOrder = 2;
	TWindow_AddWindow(wTiles, wEnts);


	door_stage = 1;	door_tag = 0;
	door_selected = -1;

	// Create the door-editing window:
	wDoors = calloc(sizeof(TWindow), 1);
	TWindow_Init(wDoors, Window.Width - 274, 446, 256+12, 64+27+26, "Doors");
	TWindow_AddLabel(wDoors, 20, 32, "Target map:");
	TWindow_AddLabel(wDoors, 20, 48, "Tag:");
	btnNextDoor = TWindow_AddButton(wDoors, 20, 64, 48, 24, "Next");
	btnPrevDoor = TWindow_AddButton(wDoors, 96, 64, 48, 24, "Prev");
	lblStage	= TWindow_AddLabel(wDoors, 96, 32, NULL);
	lblTag	  = TWindow_AddLabel(wDoors, 96, 48, format("0x%02X", door_tag));
	btnStageDn  = TWindow_AddButton(wDoors, 192, 32, 16, 16, "-");
	btnStageUp  = TWindow_AddButton(wDoors, 210, 32, 16, 16, "+");
	btnTagDn	= TWindow_AddButton(wDoors, 192, 48, 16, 16, "-");
	btnTagUp	= TWindow_AddButton(wDoors, 210, 48, 16, 16, "+");
	btnNextDoor->onClick = btnNextDoor_Click;
	btnPrevDoor->onClick = btnPrevDoor_Click;
	btnStageDn->onClick = btnStageDn_Click;
	btnStageUp->onClick = btnStageUp_Click;
	btnTagDn->onClick = btnTagDn_Click;
	btnTagUp->onClick = btnTagUp_Click;
	wDoors->ZOrder = 3;
	TWindow_AddWindow(wEnts, wDoors);

	// Create the non-visible entity-selection window:
	wEntSelect = calloc(sizeof(TWindow), 1);
	TWindow_Init(wEntSelect, 128, 32, 64+8+12+12, 64+27, "Choose entity...");
	btnAdd = TWindow_AddButton(wEntSelect, 20, 32, 64, 24, "Add");
	btnAdd->onClick = btnAdd_Click;
	wEntSelect->ZOrder = 4;
	wEntSelect->Visible = FALSE;
	GenerateRadioButtons();
	
	// Add the window as a sub-window of doors window:
	TWindow_AddWindow(wDoors, wEntSelect);
	
	// Editing loop:
	while (quit == 0) {
		sys_clearscreen();

		screen_mx += dx;
		screen_my += dy;

		if (screen_mx < 0) screen_mx = 0;
		if (screen_my < 0) screen_my = 0;
		if (screen_mx + screen_w >= map.width) screen_mx = map.width - screen_w;
		if (screen_my + screen_h >= map.height) screen_my = map.height - screen_h;

		// Render the glWindows:
		glPushMatrix();
		TWindow_Render(wMain);
		//DrawMouse();
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, 0);

		// Poll SDL events:
		while ( SDL_PollEvent(&event) ) {
			if ( event.type == SDL_QUIT ) {
				quit = 1;
			}
			if ( event.type == SDL_MOUSEMOTION ) {
				Mouse.X = event.motion.x;
				Mouse.Y = event.motion.y;
				mx = (event.motion.x - wMain->X - 6) >> 4;
				my = (event.motion.y - wMain->Y - 27) >> 4;
			}
			if ( event.type == SDL_MOUSEBUTTONDOWN ) {
				Mouse.Button = event.button.button;
				OnMouseDown(wMain);
				if ( event.button.button == SDL_BUTTON_LEFT ) left_clicked = 1;
				if ( event.button.button == SDL_BUTTON_RIGHT ) right_clicked = 1;
				// This code doesn't compile under SDL <= 1.2.4
/*				if ( event.button.button == SDL_BUTTON_WHEELUP ) {
					if (--selected_tile < -1) selected_tile = -1;
				}
				if ( event.button.button == SDL_BUTTON_WHEELDOWN ) {
					if (++selected_tile > map.map2x2_size) selected_tile = map.map2x2_size;
				}
*/
			}
			if ( event.type == SDL_MOUSEBUTTONUP ) {
				OnMouseUp();
				if ( event.button.button == SDL_BUTTON_LEFT ) left_clicked = 0;
				if ( event.button.button == SDL_BUTTON_RIGHT ) right_clicked = 0;
			}
			if ( event.type == SDL_KEYDOWN ) {
				//OnKeyDown(wMain, &(event.key));
				if (wMain->onKeyDown)
					wMain->onKeyDown(&(event.key));
			}
			if ( event.type == SDL_KEYUP ) {
				//OnKeyUp(wMain, &(event.key));
				if (wMain->onKeyUp)
					wMain->onKeyUp(&(event.key));
			}
		}

		keymods = SDL_GetModState();
		if (keymods == KMOD_NONE) {
			switch (operation) {
				case OPERATION_MOVEENTITY:
					operation = OPERATION_NONE;
					break;
			}
		}

		if (left_clicked || right_clicked) OnMouseDown(wMain);

		if ((keymods == KMOD_NONE) && (operation == OPERATION_SELECTBOX)) {
			// Swap coordinates:
			if (box_x2 < box_x1) { t = box_x1; box_x1 = box_x2; box_x2 = t; }
			if (box_y2 < box_y1) { t = box_y1; box_y1 = box_y2; box_y2 = t; }
			operation = OPERATION_SELECTEDBOX;
		}

		if ((operation == OPERATION_SELECTBOX) || (operation == OPERATION_SELECTEDBOX)) {
			// Draw the map selection box:
			int	x1, y1, x2, y2;
			x1 = box_x1 - screen_mx;
			x2 = box_x2 - screen_mx;
			y1 = box_y1 - screen_my;
			y2 = box_y2 - screen_my;

			glEnable(GL_BLEND);
			glPushMatrix();
				glBindTexture(GL_TEXTURE_2D, 0);
				glTranslatef(wMain->X + 6, wMain->Y + 27, 0.2);
				glColor4f(0.0, 0.0, 0.4, 0.5);
				glBegin(GL_QUADS);
					glVertex2i((x1<<4)   , (y1<<4)   );
					glVertex2i((x2<<4)+16, (y1<<4)   );
					glVertex2i((x2<<4)+16, (y2<<4)+16);
					glVertex2i((x1<<4)   , (y2<<4)+16);
				glEnd();
			glPopMatrix();
		}

		sys_updatescreen();
	}

	return;
}

int main( int argc, char *argv[] ) {
	int	retcode;

	// Initialize the system-dependent stuff (OpenGL & SDL):
	retcode = sys_init(argc, argv);
	if (retcode != 0) {
		return retcode;
	}

	// Run the main game loop:
	gameloop();

	// Close the system-dependent stuff (SDL):
	sys_close();

	return 0;
}
