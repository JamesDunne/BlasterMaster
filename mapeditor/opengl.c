#include "common.h"
#include "bmedit.h"
#include "opengl.h"
#include "newmap.h"
#include "font.h"
#include "glwindows.h"

GLuint	fgsprites[16];
GLuint	bgsprites[16];

SDL_Surface	*sdlscreen;
Uint32	curticks;
int		fullscreen, level_loaded;

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
	glTexCoord2f(tx1, ty1); glVertex2i(0, 0);
	glTexCoord2f(tx2, ty1); glVertex2i(8, 0);
	glTexCoord2f(tx2, ty2); glVertex2i(8, 8);
	glTexCoord2f(tx1, ty2); glVertex2i(0, 8);
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
	glTexCoord2f(tx2, ty1); glVertex2i(0, 0);
	glTexCoord2f(tx1, ty1); glVertex2i(8, 0);
	glTexCoord2f(tx1, ty2); glVertex2i(8, 8);
	glTexCoord2f(tx2, ty2); glVertex2i(0, 8);
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
	glTexCoord2f(tx1, ty2); glVertex2i(0, 0);
	glTexCoord2f(tx2, ty2); glVertex2i(8, 0);
	glTexCoord2f(tx2, ty1); glVertex2i(8, 8);
	glTexCoord2f(tx1, ty1); glVertex2i(0, 8);
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
	glTexCoord2f(tx2, ty2); glVertex2i(0, 0);
	glTexCoord2f(tx1, ty2); glVertex2i(8, 0);
	glTexCoord2f(tx1, ty1); glVertex2i(8, 8);
	glTexCoord2f(tx2, ty1); glVertex2i(0, 8);
	glEnd();
	glPopMatrix();

	return 0;
}

int put_bgtile(int x, int y, int m, unsigned char v) {
	GLfloat	tx1, ty1, tx2, ty2;
	unsigned char	t = ((v & 0x0F) << 4) | ((v >> 4) & 0x0F);

	tx1 = (t & 0x0F) * 0.0625f;
	ty1 = (t >> 4) * 0.0625f;
	tx2 = tx1 + 0.0625f;
	ty2 = ty1 + 0.0625f;

	// Bind the appropriate texture:
	glBindTexture(GL_TEXTURE_2D, bgsprites[m & 3]);

	// Draw the sprite:
	glBegin(GL_QUADS);
	glTexCoord2f(tx1, ty1); glVertex2i(x  , y  );
	glTexCoord2f(tx2, ty1); glVertex2i(x+8, y  );
	glTexCoord2f(tx2, ty2); glVertex2i(x+8, y+8);
	glTexCoord2f(tx1, ty2); glVertex2i(x  , y+8);
	glEnd();

	return 0;
}

void Draw2x2BGTile(int x, int y, int t) {
	int	m = map.mapflags[t];
	put_bgtile(x  , y  , m, map.map2x2[(t<<2)+0]);
	put_bgtile(x+8, y  , m, map.map2x2[(t<<2)+1]);
	put_bgtile(x  , y+8, m, map.map2x2[(t<<2)+2]);
	put_bgtile(x+8, y+8, m, map.map2x2[(t<<2)+3]);
}

int LoadFontTexture() {
	SDL_Surface	*texture;

	// Load the font bitmap:
	texture = IMG_Load("font.tga");
	if (texture == NULL) {
		fprintf(stderr, "Couldn't find '%s'\n", "font.tga");
		return -1;
	}

	glGenTextures(1, &fontTexture);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	SDL_LockSurface(texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture->pixels);
	SDL_UnlockSurface(texture);

	glEnable(GL_TEXTURE_2D);

	SDL_FreeSurface(texture);

	// Load the GUI bitmap:
	texture = IMG_Load("GUI.tga");
	if (texture == NULL) {
		fprintf(stderr, "Couldn't find '%s'\n", "GUI.tga");
		return -1;
	}

	glGenTextures(1, &GUITexture);
	glBindTexture(GL_TEXTURE_2D, GUITexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	SDL_LockSurface(texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture->pixels);
	SDL_UnlockSurface(texture);

	glEnable(GL_TEXTURE_2D);

	SDL_FreeSurface(texture);

	return 0;
}

int DestroyFontTexture() {
	glDeleteTextures(1, &fontTexture);
	glDeleteTextures(1, &GUITexture);

	return 0;
}

int InitTextures() {
	int		i, j;

	SDL_Surface	*texture, *tempbitmap;
	char	filename[256], tempf[256];

#define LOAD_MAP_TEXTURES(filenames, dpylist, numtex) \
	glGenTextures(numtex, dpylist); \
	for (j=0; j<numtex; ++j) { \
		tempbitmap = IMG_Load(filenames[j]); \
		if (tempbitmap == NULL) { \
			fprintf(stderr, "Couldn't find '%s'\n", filenames[j]); \
			return -1; \
		} \
\
		SDL_SetColorKey(tempbitmap, SDL_SRCCOLORKEY, SDL_MapRGB(tempbitmap->format, 255, 0, 255)); \
		texture = SDL_DisplayFormatAlpha(tempbitmap); \
\
		glBindTexture(GL_TEXTURE_2D, dpylist[j]); \
\
		SDL_LockSurface(texture); \
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture->pixels); \
		SDL_UnlockSurface(texture); \
\
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); \
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); \
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); \
		glEnable(GL_TEXTURE_2D); \
\
		SDL_FreeSurface(texture); \
		SDL_FreeSurface(tempbitmap); \
	}

	LoadFontTexture();

	// Load the background tileset for the level:
	LOAD_MAP_TEXTURES(map.texturefile, bgsprites, map.numtextures)

	return 0;
}

void LoadTexture(int page, const char *filename) {
	int			i, j;
	SDL_Surface	*texture, *tempbitmap;

	//fprintf(stderr, "LoadTexture(%d, \"%s\")\n", page, filename);

	if ((page < 0) || (page > 15)) return;

	// Free old texture:
	if (fgsprites[page] != 0) {
		//fprintf(stderr, "  Freeing texture...\n");
		glDeleteTextures(1, &(fgsprites[page]));
		fgsprites[page] = 0;
	}

	// Create new texture:
	glGenTextures(1, &(fgsprites[page]));
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
		if (fgsprites[i] != 0)
			glDeleteTextures(1, &(fgsprites[i]));
	glDeleteTextures(map.numtextures, bgsprites);
}

void InitGL(int Width, int Height) {
	glViewport(0, 0, Width, Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, Width, Height, 0, -100, 100);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			  // Black Background
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);				 // Enables Smooth Color Shading
	glClearDepth(1.0);					   // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);				 // Enable Depth Buffer
	glDepthFunc(GL_LESS);				   // The Type Of Depth Test To Do

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int sys_init(int argc, char **argv) {
	int i;
	unsigned long sdlflags;

	/* Initialize SDL for video output */
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	sdlflags = SDL_OPENGL;
	if ((argc >= 2) && (strcmp(argv[1], "-fs") == 0)) sdlflags |= SDL_FULLSCREEN;

	/* Create an OpenGL full-screen context */
	sdlscreen = SDL_SetVideoMode(1024, 768, 32, sdlflags);
	if ( sdlscreen == NULL ) {
		fprintf(stderr, "Unable to create OpenGL screen: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}

	/* Set the title bar in environments that support it */
	SDL_WM_SetCaption("Blaster Master Map Editor", NULL);

	InitGL(1024, 768);

	fontInitialise();

	for (i=0; i<16; ++i) fgsprites[i] = 0;

	return 0;
};

void sys_close() {
	fontDestroy();
	DestroyFontTexture();

	FreeTextures();

	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(1);

	SDL_JoystickEventState(SDL_DISABLE);

	SDL_Quit();
}

void sys_togglefullscreen() {
	// Kinda buggy still:
/*
	unsigned long sdlflags;

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
	curticks = SDL_GetTicks();

	// Clear the screen:
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void sys_updatescreen() {
	// About 30 fps:	(60fps = 16.667 ms/frame)
	//while (SDL_GetTicks() - curticks <= 32);
	SDL_GL_SwapBuffers();
};
