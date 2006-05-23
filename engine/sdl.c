#include <time.h>
#include "common.h"
#include "bm.h"
#include "opengl.h"
#include "newmap.h"

SDL_Surface		*fgsprites[16];
SDL_Surface		*fgspritesh[16];
SDL_Surface		*fgspritesv[16];
SDL_Surface		*fgspriteshv[16];
SDL_Surface		*bgsprites[16];

SDL_Surface	*srfVideo;
Uint32		curticks;
int			fullscreen, level_loaded;
double		accel_scale;
double		time_dt;

// -- Main sprite routines --
int put_sprite(fixed x,fixed y,int m,GLuint sprite)
{
	SDL_Rect	src, dst;

	// Where to find the source sprite:
	src.x = (sprite & 0x0F) << 4;
	src.y = (sprite >> 4) << 4;
	src.w = 16;
	src.h = 16;

	// Where to blit the surface on the video:
	dst.x = (long)x;
	dst.y = (long)y;
	dst.w = 16;
	dst.h = 16;

	// Blit the surface:
	SDL_BlitSurface(fgsprites[m], &src, SDL_GetVideoSurface(), &dst);

	return 0;
}

// Flip horizontally:
int put_sprite_hflip(fixed x,fixed y,int m,GLuint sprite)
{
	SDL_Rect	src, dst;

	// Where to find the source sprite:
	src.x = (sprite & 0x0F) << 4;
	src.y = (sprite >> 4) << 4;
	src.w = 16;
	src.h = 16;

	// Where to blit the surface on the video:
	dst.x = (long)x;
	dst.y = (long)y;
	dst.w = 16;
	dst.h = 16;

	// Blit the surface:
	SDL_BlitSurface(fgspritesh[m], &src, SDL_GetVideoSurface(), &dst);

	return 0;
}

// Flip vertically
int put_sprite_vflip(fixed x,fixed y,int m,GLuint sprite)
{
	SDL_Rect	src, dst;

	// Where to find the source sprite:
	src.x = (sprite & 0x0F) << 4;
	src.y = (sprite >> 4) << 4;
	src.w = 16;
	src.h = 16;

	// Where to blit the surface on the video:
	dst.x = (long)x;
	dst.y = (long)y;
	dst.w = 16;
	dst.h = 16;

	// Blit the surface:
	SDL_BlitSurface(fgspritesv[m], &src, SDL_GetVideoSurface(), &dst);

	return 0;
}

// Flip horizontally and vertically:
int put_sprite_hvflip(fixed x,fixed y,int m,GLuint sprite)
{
	SDL_Rect	src, dst;

	// Where to find the source sprite:
	src.x = (sprite & 0x0F) << 4;
	src.y = (sprite >> 4) << 4;
	src.w = 16;
	src.h = 16;

	// Where to blit the surface on the video:
	dst.x = (long)x;
	dst.y = (long)y;
	dst.w = 16;
	dst.h = 16;

	// Blit the surface:
	SDL_BlitSurface(fgspriteshv[m], &src, SDL_GetVideoSurface(), &dst);

	return 0;
}

int put_bgtile(fixed x, fixed y, int m, unsigned char t, int bg) {
	SDL_Rect	src, dst;

	src.x = (t >> 4) << 4;
	src.y = (t & 0x0F) << 4;
	src.w = src.h = 16;

	dst.x = (long)x;
	dst.y = (long)y;
	dst.w = dst.h = 16;

	SDL_BlitSurface(bgsprites[m & 3], &src, SDL_GetVideoSurface(), &dst);

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
	char	filename[256];

	for (j=0; j<map.numtextures; ++j) {
		bgsprites[j] = IMG_Load(map.texturefile[j]);
		if (bgsprites[j] == NULL) {
			fprintf(stderr, "Couldn't find '%s'\n", map.texturefile[j]);
			return -1;
		}
	}

	return 0;
}

void LoadTexture(int page, const char *filename) {
	int	i, j;
	int	x, y;
	unsigned long	tmp;
	unsigned long	*px;
	int		base1, base2;
	Uint16	pitch;
	Uint8	bpp;

	if ((page < 0) || (page > 15)) return;

	// Free old texture:
	if (fgsprites[page] != NULL) {
		SDL_FreeSurface(fgsprites[page]);
		fgsprites[page] = NULL;
	}
	if (fgspritesh[page] != NULL) {
		SDL_FreeSurface(fgspritesh[page]);
		fgspritesh[page] = NULL;
	}
	if (fgspritesv[page] != NULL) {
		SDL_FreeSurface(fgspritesv[page]);
		fgspritesv[page] = NULL;
	}
	if (fgspriteshv[page] != NULL) {
		SDL_FreeSurface(fgspriteshv[page]);
		fgspriteshv[page] = NULL;
	}

	// Create new texture:
	fgsprites[page] = IMG_Load(filename);
	if (fgsprites[page] == NULL) {
		fprintf(stderr, "Couldn't find '%s'\n", filename);
		exit(-1);
		return;
	}

	// Create a h-flipped version:
	fgspritesh[page] = SDL_ConvertSurface(
		fgsprites[page],
		fgsprites[page]->format,
		0
	);
	SDL_LockSurface(fgspritesh[page]);
	px = (unsigned long *)fgspritesh[page]->pixels;
	bpp = fgspritesh[page]->format->BytesPerPixel;
	pitch = fgspritesh[page]->pitch;
	for (y = 0; y < 256; ++y) {
		base1 = (y * pitch / bpp);
		for (x = 0; x < 16; ++x) {
			base2 = (x * 16);
			for (j = 0; j < 8; ++j) {
				tmp = px[base1 + base2 + j];
				px[base1 + base2 + j] = px[base1 + base2 + (15-j)];
				px[base1 + base2 + (15-j)] = tmp;
			}
		}
	}
	SDL_UnlockSurface(fgspritesh[page]);

	// Create a v-flipped version:
	fgspritesv[page] = SDL_ConvertSurface(
		fgsprites[page],
		fgsprites[page]->format,
		0
	);
	SDL_LockSurface(fgspritesv[page]);
	px = (unsigned long *)fgspritesv[page]->pixels;
	bpp = fgspritesv[page]->format->BytesPerPixel;
	pitch = fgspritesv[page]->pitch;
	for (x = 0; x < 256; ++x) {
		for (y = 0; y < 16; ++y) {
			base1 = y*16;
			for (j = 0; j < 8; ++j) {
				tmp = px[x + (base1 + j)*pitch/bpp];
				px[x + (base1 + j)*pitch/bpp] = px[x + (base1 + (15-j))*pitch/bpp];
				px[x + (base1 + (15-j))*pitch/bpp] = tmp;
			}
		}
	}
	SDL_UnlockSurface(fgspritesv[page]);

	// Create a hv-flipped version:
	fgspriteshv[page] = SDL_ConvertSurface(
		fgsprites[page],
		fgsprites[page]->format,
		0
	);
	SDL_LockSurface(fgspriteshv[page]);
	px = (unsigned long *)fgspriteshv[page]->pixels;
	bpp = fgspriteshv[page]->format->BytesPerPixel;
	pitch = fgspriteshv[page]->pitch;
	// First, h-flip:
	for (y = 0; y < 256; ++y) {
		base1 = (y * pitch / bpp);
		for (x = 0; x < 16; ++x) {
			base2 = (x * 16);
			for (j = 0; j < 8; ++j) {
				tmp = px[base1 + base2 + j];
				px[base1 + base2 + j] = px[base1 + base2 + (15-j)];
				px[base1 + base2 + (15-j)] = tmp;
			}
		}
	}
	// Then, v-flip:
	for (x = 0; x < 256; ++x) {
		for (y = 0; y < 16; ++y) {
			base1 = y*16;
			for (j = 0; j < 8; ++j) {
				tmp = px[x + (base1 + j)*pitch/bpp];
				px[x + (base1 + j)*pitch/bpp] = px[x + (base1 + (15-j))*pitch/bpp];
				px[x + (base1 + (15-j))*pitch/bpp] = tmp;
			}
		}
	}
	SDL_UnlockSurface(fgspriteshv[page]);
}

void FreeTextures() {
	int	i;
	for (i=0; i<16; ++i) {
		if (fgsprites[i] != NULL) {
			SDL_FreeSurface(fgsprites[i]);
			fgsprites[i] = 0;
		}
		if (fgspritesh[i] != NULL) {
			SDL_FreeSurface(fgspritesh[i]);
			fgspritesh[i] = 0;
		}
		if (fgspritesv[i] != NULL) {
			SDL_FreeSurface(fgspritesv[i]);
			fgspritesv[i] = 0;
		}
		if (fgspriteshv[i] != NULL) {
			SDL_FreeSurface(fgspriteshv[i]);
			fgspriteshv[i] = 0;
		}
	}

	for (i=0; i<4; ++i) {
		SDL_FreeSurface(bgsprites[i]);
		bgsprites[i] = 0;
	}
}

void InitGL(int Width, int Height) {
/*	glViewport(0, 0, Width, Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, Width, Height, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			  // Black Background
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
*/
}

void sys_init(int argc, char **argv) {
	int i, w, h;
	unsigned long sdlflags;
	Uint32 rmask, gmask, bmask, amask;

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
	sdlflags = SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_HWACCEL;
	for (i=1; i<argc; ++i) {
		if (strcmp(argv[i], "-fs") == 0) sdlflags |= SDL_FULLSCREEN;
		if (strcmp(argv[i], "-w") == 0) screen_w = atoi(argv[i+1]);
		if (strcmp(argv[i], "-h") == 0) screen_h = atoi(argv[i+1]);
	}

	/* Create an SDL full-screen context */
	srfVideo = SDL_SetVideoMode(screen_w, screen_h, 24, sdlflags);
	if ( srfVideo == NULL ) {
		fprintf(stderr, "Unable to create SDL screen: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}

	// Set the title bar in environments that support it:
	SDL_WM_SetCaption("Blaster Master Engine Test", NULL);

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
		fgsprites[i] = fgspritesh[i] = fgspritesv[i] = fgspriteshv[i] = bgsprites[i] = NULL;

	// Don't show the mouse cursor in full screen:
	if (sdlflags & SDL_FULLSCREEN)
		SDL_ShowCursor(0);
};

void sys_close() {
	if (level_loaded) FreeTextures();

	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(1);

	SDL_JoystickEventState(SDL_DISABLE);

	SDL_Quit();
}

void sys_togglefullscreen() {

};

void sys_clearscreen() {
	curticks = SDL_GetTicks();
}

void sys_updatescreen() {
	static long numframes = 0;
	static clock_t	lastclock = 0;
	clock_t	curclock;

	// About 60 fps:	(60fps = 16.667 ms/frame)
	while (SDL_GetTicks() - curticks <= 16);
	curticks = SDL_GetTicks();

	time_dt = 1;

	// Flip buffers:
	SDL_Flip(srfVideo);
	//SDL_UpdateRect(srfVideo, 0, 0, 0, 0);
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
