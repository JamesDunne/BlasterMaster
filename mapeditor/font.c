#include <stdarg.h>
#include "common.h"
#include "font.h"

GLuint	fontTexture;
GLuint	fontBaseListId;
unsigned char	FontWidth[256];

int fontWidth(char *s) {
	char	*p;
	int		w;

	p = s; w = 0;
	while (*p != 0)
		w += FontWidth[*p++] + 1;

	return w;
}

void fontInitialise() {
	float	cx;
	float	cy;
	float	xs;
	int		loop;

	// Read font widths:
	FILE	*fontfile = fopen("font.fnt", "rb");
	for (loop=0; loop<256; ++loop)
		fread(&(FontWidth[loop]), 1, 1, fontfile);
	fclose(fontfile);

	fontBaseListId = glGenLists(NUMCHARS);	// Creating NUMCHARS Display Lists
	for (loop=0; loop<NUMCHARS; loop++) {
		cx=((float)(loop%16))/16.0f;				// X Position Of Current Character
		cy=((float)(loop/16))/16.0f;				// Y Position Of Current Character
		xs = (16.0-FontWidth[loop])/2.0/256.0;
		glNewList(fontBaseListId+loop, GL_COMPILE);	// Start Building A List
			glBegin(GL_QUADS);
				glTexCoord2f(cx+xs,cy);
				glVertex2f(0,0);
				glTexCoord2f(cx+xs+FontWidth[loop]/256.0 + 1.0/512.0,cy);
				glVertex2f(FontWidth[loop],0);
				glTexCoord2f(cx+xs+FontWidth[loop]/256.0 + 1.0/512.0,cy+0.0625);
				glVertex2f(FontWidth[loop],16);
				glTexCoord2f(cx+xs,cy+0.0625);
				glVertex2f(0,16);
			glEnd();								// Done Building Our Quad (Character)
			glTranslatef(FontWidth[loop] + 1.0,0,0);
		glEndList();								// Done Building The Display List
	}
}

void fontDestroy() {
	glDeleteLists(fontBaseListId, NUMCHARS);
}

/*
INFO From siggraph:

The GL_INTENSITY texture format is a good texture format for textures containing glyphs
because the texture format is compact. The intensity texture format replicates the single
intensity component value in the red, green, blue, and alpha channels. When rendering
colored glyphs, use the GL_MODULATE texture environment and set the current color to the
desired glyph color.
*/

int glPrint(GLfloat x, GLfloat y, const char *fmt, ...)
{
	char		text[1024];							// Holds Our String
	va_list		ap;									// Pointer To List Of Arguments

	if (fmt == NULL)
		return 0;

	// Parse The String For Variables
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);

	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fontTexture);

	glTranslatef(x, y, 0);							// Position The Text (0,0 - Top Left)

	glListBase(fontBaseListId);				// Font bitmaps starts at ' ' (space/32).
	glCallLists(strlen(text), GL_BYTE, text);		// Write The Text To The Screen

	glPopMatrix();

	return 0;
}

