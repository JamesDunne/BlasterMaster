
#define NUMCHARS	256

extern	GLuint	fontTexture;
extern	GLuint	fontBaseListId;

void fontInitialise();
void fontDestroy();
int glPrint(GLfloat x, GLfloat y, const char *fmt, ...);

