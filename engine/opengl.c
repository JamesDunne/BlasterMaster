#include <time.h>
#include "common.h"
#include "bm.h"
#include "opengl.h"
#include "newmap.h"

GLuint		fgsprites[16];
GLuint		bgsprites[16];

SDL_Surface	*sdlscreen;
Uint32		curticks;
int			fullscreen, level_loaded;
double		accel_scale;
double		time_dt;

// -- Main sprite routines --
int put_sprite(fixed x,fixed y,int m,GLuint sprite)
{
	GLfloat	tx1, ty1, tx2, ty2;

	tx1 = (sprite & 0x0F) * 0.0625f;
	ty1 = (sprite >> 4) * 0.0625f;
	tx2 = tx1 + 0.0625f;
	ty2 = ty1 + 0.0625f;

	glBindTexture(GL_TEXTURE_2D, fgsprites[m]);

	glPushMatrix();
	glTranslatef(x, y, 0);
	// Draw the sprite:
	glBegin(GL_QUADS);
	glTexCoord2f(tx1, ty1); glVertex2i(0 , 0 );
	glTexCoord2f(tx2, ty1); glVertex2i(16, 0 );
	glTexCoord2f(tx2, ty2); glVertex2i(16, 16);
	glTexCoord2f(tx1, ty2); glVertex2i(0 , 16);
	glEnd();
	glPopMatrix();

	return 0;
}

// Flip horizontally:
int put_sprite_hflip(fixed x,fixed y,int m,GLuint sprite)
{
	GLfloat	tx1, ty1, tx2, ty2;

	tx1 = (sprite & 0x0F) * 0.0625f;
	ty1 = (sprite >> 4) * 0.0625f;
	tx2 = tx1 + 0.0625f;
	ty2 = ty1 + 0.0625f;

	glBindTexture(GL_TEXTURE_2D, fgsprites[m]);

	glPushMatrix();
	glTranslatef(x, y, 0);
	// Draw the sprite:
	glBegin(GL_QUADS);
	glTexCoord2f(tx2, ty1); glVertex2i(0 , 0 );
	glTexCoord2f(tx1, ty1); glVertex2i(16, 0 );
	glTexCoord2f(tx1, ty2); glVertex2i(16, 16);
	glTexCoord2f(tx2, ty2); glVertex2i(0 , 16);
	glEnd();
	glPopMatrix();

	return 0;
}

// Flip vertically
int put_sprite_vflip(fixed x,fixed y,int m,GLuint sprite)
{
	GLfloat	tx1, ty1, tx2, ty2;

	tx1 = (sprite & 0x0F) * 0.0625f;
	ty1 = (sprite >> 4) * 0.0625f;
	tx2 = tx1 + 0.0625f;
	ty2 = ty1 + 0.0625f;

	glBindTexture(GL_TEXTURE_2D, fgsprites[m]);

	glPushMatrix();
	glTranslatef(x, y, 0);
	// Draw the sprite:
	glBegin(GL_QUADS);
	glTexCoord2f(tx1, ty2); glVertex2i(0 , 0 );
	glTexCoord2f(tx2, ty2); glVertex2i(16, 0 );
	glTexCoord2f(tx2, ty1); glVertex2i(16, 16);
	glTexCoord2f(tx1, ty1); glVertex2i(0 , 16);
	glEnd();
	glPopMatrix();

	return 0;
}

// Flip horizontally and vertically:
int put_sprite_hvflip(fixed x,fixed y,int m,GLuint sprite)
{
	GLfloat	tx1, ty1, tx2, ty2;

	tx1 = (sprite & 0x0F) * 0.0625f;
	ty1 = (sprite >> 4) * 0.0625f;
	tx2 = tx1 + 0.0625f;
	ty2 = ty1 + 0.0625f;

	glBindTexture(GL_TEXTURE_2D, fgsprites[m]);

	glPushMatrix();
	glTranslatef(x, y, 0);
	// Draw the sprite:
	glBegin(GL_QUADS);
	glTexCoord2f(tx2, ty2); glVertex2i(0 , 0 );
	glTexCoord2f(tx1, ty2); glVertex2i(16, 0 );
	glTexCoord2f(tx1, ty1); glVertex2i(16, 16);
	glTexCoord2f(tx2, ty1); glVertex2i(0 , 16);
	glEnd();
	glPopMatrix();

	return 0;
}

int put_bgtile(fixed x, fixed y, int m, unsigned char v, int bg) {
	GLfloat	tx1, ty1, tx2, ty2;
	unsigned char	t = ((v & 0x0F) << 4) | ((v >> 4) & 0x0F);

	tx1 = (t & 0x0F) * 0.0625f;
	ty1 = (t >> 4) * 0.0625f;
	tx2 = tx1 + 0.0625f;
	ty2 = ty1 + 0.0625f;

	glBindTexture(GL_TEXTURE_2D, bgsprites[m & 3]);

	glPushMatrix();
	glTranslatef(x, y, 0);
	// Draw the sprite:
	glBegin(GL_QUADS);
	glTexCoord2f(tx1, ty1); glVertex2i(0 , 0 );
	glTexCoord2f(tx2, ty1); glVertex2i(16, 0 );
	glTexCoord2f(tx2, ty2); glVertex2i(16, 16);
	glTexCoord2f(tx1, ty2); glVertex2i(0 , 16);
	glEnd();
	glPopMatrix();

	return 0;
}

void Draw2x2BGTile(fixed x, fixed y, int t, int bg) {
	int	m = map.mapflags[t];
	put_bgtile(x   , y   , m, map.map2x2[(t<<2)+0], bg);
	put_bgtile(x+16, y   , m, map.map2x2[(t<<2)+1], bg);
	put_bgtile(x   , y+16, m, map.map2x2[(t<<2)+2], bg);
	put_bgtile(x+16, y+16, m, map.map2x2[(t<<2)+3], bg);
}

int InitTextures() {
	int		i, j;
	SDL_Surface	*texture;
	char	filename[256];

#define LOAD_MAP_TEXTURES(filenames, dpylist, numtex) \
	glGenTextures(numtex, dpylist); \
	for (j=0; j<numtex; ++j) { \
		texture = IMG_Load(filenames[j]); \
		if (texture == NULL) { \
			fprintf(stderr, "Couldn't find '%s'\n", filenames[j]); \
			return -1; \
		} \
\
		glBindTexture(GL_TEXTURE_2D, dpylist[j]); \
\
		SDL_LockSurface(texture); \
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->pixels); \
		SDL_UnlockSurface(texture); \
\
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); \
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); \
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); \
		glEnable(GL_TEXTURE_2D); \
\
		SDL_FreeSurface(texture); \
	}

	// Load the background tileset for the level:
	LOAD_MAP_TEXTURES(map.texturefile, bgsprites, map.numtextures)

#undef LOAD_MAP_TEXTURES

	return 0;
}

void LoadTexture(int page, const char *filename) {
	int			i, j;
	SDL_Surface	*texture;

	if ((page < 0) || (page > 15)) return;
	
	// Free old texture:
	if (fgsprites[page] != 0) {
		glDeleteTextures(1, &(fgsprites[page]));
		fgsprites[page] = 0;
	}
	
	// Create new texture:
	glGenTextures(1, &fgsprites[page]);
	texture = IMG_Load(filename);
	if (texture == NULL) {
		fprintf(stderr, "Couldn't find '%s'\n", filename);
		exit(-1);
		return;
	}

	glBindTexture(GL_TEXTURE_2D, fgsprites[page]);

	SDL_LockSurface(texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->pixels);
	SDL_UnlockSurface(texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);

	SDL_FreeSurface(texture);
}

void FreeTextures() {
	int	i;
	for (i=0; i<16; ++i)
		if (fgsprites[i] != 0) {
			glDeleteTextures(1, &(fgsprites[i]));
			fgsprites[i] = 0;
		}
	for (i=0; i<map.numtextures; ++i) {
		glDeleteTextures(1, &(bgsprites[i]));
		bgsprites[i] = 0;
	}
}

void InitGL(int Width, int Height) {
	glViewport(0, 0, Width, Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, Width, Height, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			  // Black Background
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void sys_init(int argc, char **argv) {
	int i, w, h;
	unsigned long sdlflags;

	/* Initialize SDL for video output */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	// Enable the joystick subsystem:
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	// Default resolution:
	screen_w = 512; screen_h = 448;

	// Scan program arguments:
	sdlflags = SDL_OPENGL;
	for (i=1; i<argc; ++i) {
		if (strcmp(argv[i], "-fs") == 0) sdlflags |= SDL_FULLSCREEN;
		if (strcmp(argv[i], "-w") == 0) screen_w = atoi(argv[i+1]);
		if (strcmp(argv[i], "-h") == 0) screen_h = atoi(argv[i+1]);
	}

	/* Create an OpenGL full-screen context */
	sdlscreen = SDL_SetVideoMode(screen_w, screen_h, 24, sdlflags);
	if ( sdlscreen == NULL ) {
		fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}

	// Set the title bar in environments that support it:
	SDL_WM_SetCaption("Blaster Master", NULL);

	// Initialize OpenGL:
	InitGL(screen_w, screen_h);

	level_loaded = 0;

	// Scan for joysticks:
	printf("%i joysticks were found.\n\n", SDL_NumJoysticks() );
	if (SDL_NumJoysticks() > 0) {
		printf("The names of the joysticks are:\n");

		SDL_JoystickEventState(SDL_ENABLE);

		for( i=0; i < SDL_NumJoysticks(); i++ )
		{
			SDL_Joystick	*curjoy = SDL_JoystickOpen(i);
			printf("	%s (%d buttons, %d axes, %d hats, %d balls)\n", SDL_JoystickName(i),
				SDL_JoystickNumButtons(curjoy), SDL_JoystickNumAxes(curjoy),
				SDL_JoystickNumHats(curjoy), SDL_JoystickNumBalls(curjoy));
		}
	}

	// Clear texture indicies:	
	for (i=0; i<16; ++i)
		fgsprites[i] = bgsprites[i] = 0;

	// Don't show the mouse cursor in full screen:
	if (sdlflags & SDL_FULLSCREEN)
		SDL_ShowCursor(0);
		
	curticks = 0;
};

void sys_close() {
	if (level_loaded) FreeTextures();

	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(1);

	SDL_JoystickEventState(SDL_DISABLE);

	SDL_Quit();
}

void sys_togglefullscreen() {
	// Kinda buggy still:
/*	unsigned long sdlflags;

	fullscreen = !fullscreen;

	sdlflags = SDL_OPENGL;
	if (fullscreen) sdlflags |= SDL_FULLSCREEN;

	SDL_FreeSurface(sdlscreen);

	// Create an OpenGL full-screen context
	sdlscreen = SDL_SetVideoMode(640, 480, 24, sdlflags);
	if ( sdlscreen == NULL ) {
		fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}

	InitGL(640, 480);
	InitTextures();
*/
};

void sys_clearscreen() {
	// Clear the screen:
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sys_updatescreen() {
	static long numframes = 0;
	static clock_t	lastclock = 0;
	clock_t	curclock;
	
	// About 60 fps:	(60fps = 16.667 ms/frame)
	while (SDL_GetTicks() - curticks <= 16);
	curticks = SDL_GetTicks();
	
	time_dt = 1;
	
	SDL_GL_SwapBuffers();

#if 0
	numframes++;
	
	long	newticks = SDL_GetTicks();
	if (newticks - curticks >= 1000) {
		fprintf(stderr, "%3.2f fps = %d frames in %d ms\n", ((double)numframes / ((double)(newticks - curticks) / 1000.0)), numframes, newticks - curticks);
		numframes = 0;
		curticks = newticks;
	}
#endif
};

void sys_eventloop(Uint8 *control_keys) {
	// Poll SDL events:
	SDL_Event event;
	while ( SDL_PollEvent(&event) ) {
		if ( event.type == SDL_QUIT ) {
			quit = 1;
		}
		if ( event.type == SDL_KEYDOWN ) {
			switch ( event.key.keysym.sym ) {
				case SDLK_ESCAPE: quit = 1; break;
				case SDLK_LEFT:		*control_keys |= BUT_LEFT; accel_scale = 1.0; break;
				case SDLK_RIGHT:	*control_keys |= BUT_RIGHT; accel_scale = 1.0; break;
				case SDLK_UP:		*control_keys |= BUT_UP; break;
				case SDLK_DOWN:		*control_keys |= BUT_DOWN; break;
				case SDLK_x:		*control_keys |= BUT_SHOOT; break;
				case SDLK_z:		*control_keys |= BUT_JUMP; break;
				case SDLK_a:		*control_keys |= BUT_SWITCH; break;
				case SDLK_RETURN:
					if ( event.key.keysym.mod & KMOD_ALT )
						sys_togglefullscreen();
					else
						*control_keys |= BUT_PAUSE;
					break;
				case SDLK_F1:		LoadLevel("maps/map00.bma"); break;
				case SDLK_F2:		LoadLevel("maps/map01.bma"); break;
				case SDLK_F3:		LoadLevel("maps/map02.bma"); break;
				case SDLK_F4:		LoadLevel("maps/map03.bma"); break;
				case SDLK_F5:		LoadLevel("maps/map04.bma"); break;
				case SDLK_F6:		LoadLevel("maps/map05.bma"); break;
				case SDLK_F7:		LoadLevel("maps/map06.bma"); break;
				case SDLK_F8:		LoadLevel("maps/map07.bma"); break;
			}
		}
		if ( event.type == SDL_KEYUP ) {
			switch ( event.key.keysym.sym ) {
				case SDLK_LEFT:		*control_keys &= ~BUT_LEFT; break;
				case SDLK_RIGHT:	*control_keys &= ~BUT_RIGHT; break;
				case SDLK_UP:		*control_keys &= ~BUT_UP; break;
				case SDLK_DOWN:		*control_keys &= ~BUT_DOWN; break;
				case SDLK_x:		*control_keys &= ~BUT_SHOOT; break;
				case SDLK_z:		*control_keys &= ~BUT_JUMP; break;
				case SDLK_a:		*control_keys &= ~BUT_SWITCH; break;
				case SDLK_RETURN:
					if ( !(event.key.keysym.mod & KMOD_ALT) )
						*control_keys &= ~BUT_PAUSE; break;
			}
		}
		// Handle joystick input:
		if ( event.type == SDL_JOYAXISMOTION ) {
			// Left-right movement code:
			if (( event.jaxis.axis == 0 ) || ( event.jaxis.axis == 4 )) {
				//xaxis_scale = abs(event.jaxis.value);
				if ( event.jaxis.value <= -1024 ) {
					*control_keys |= BUT_LEFT;
					*control_keys &= ~BUT_RIGHT;
					accel_scale = -(event.jaxis.value / 32768.0);
				} else if ( event.jaxis.value >= 1024 ) {
					*control_keys &= ~BUT_LEFT;
					*control_keys |= BUT_RIGHT;
					accel_scale = (event.jaxis.value / 32768.0);
				} else {
					*control_keys &= ~BUT_LEFT;
					*control_keys &= ~BUT_RIGHT;
				}
			}
			// Up-Down movement code:
			if (( event.jaxis.axis == 1 ) || ( event.jaxis.axis == 5 )) {
				//yaxis_scale = abs(event.jaxis.value);
				if ( event.jaxis.value < -16384 ) {
					*control_keys |= BUT_UP;
					*control_keys &= ~BUT_DOWN;
				} else if ( event.jaxis.value > 16384 ) {
					*control_keys &= ~BUT_UP;
					*control_keys |= BUT_DOWN;
				} else {
					*control_keys &= ~BUT_UP;
					*control_keys &= ~BUT_DOWN;
				}
			}
		}
		if ( event.type == SDL_JOYBUTTONDOWN ) {
			if ( event.jbutton.button == 0) *control_keys |= BUT_JUMP;
			if ( event.jbutton.button == 1) *control_keys |= BUT_PAUSE;
			if ( event.jbutton.button == 2) *control_keys |= BUT_SHOOT;
			if ( event.jbutton.button == 3) *control_keys |= BUT_SWITCH;
		}
		if ( event.type == SDL_JOYBUTTONUP ) {
			if ( event.jbutton.button == 0) *control_keys &= ~BUT_JUMP;
			if ( event.jbutton.button == 1) *control_keys &= ~BUT_PAUSE;
			if ( event.jbutton.button == 2) *control_keys &= ~BUT_SHOOT;
			if ( event.jbutton.button == 3) *control_keys &= ~BUT_SWITCH;
		}

		// This should be updated, but I can't test it in Linux:
		if ( event.type == SDL_JOYHATMOTION ) {
			if ( event.jhat.hat & SDL_HAT_LEFT ) {
				*control_keys |= BUT_LEFT;
				*control_keys &= ~BUT_RIGHT;
				
			}
			if ( event.jhat.hat & SDL_HAT_RIGHT ) {
				*control_keys &= ~BUT_LEFT;
				*control_keys |= BUT_RIGHT;
			}
			if ( event.jhat.hat & SDL_HAT_UP ) {
				*control_keys |= BUT_UP;
				*control_keys &= ~BUT_DOWN;
			}
			if ( event.jhat.hat & SDL_HAT_DOWN ) {
				*control_keys &= ~BUT_UP;
				*control_keys |= BUT_DOWN;
			}
		}
	}
};
