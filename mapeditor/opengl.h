// -- Main sprite routines --


int put_sprite(fixed x,fixed y,int m,GLuint sprite);
int put_sprite_hflip(fixed x,fixed y,int m,GLuint sprite);
int put_sprite_vflip(fixed x,fixed y,int m,GLuint sprite);
int put_sprite_hvflip(fixed x,fixed y,int m,GLuint sprite);

int put_bgtile(int x,int y,int m,unsigned char t);
void Draw2x2BGTile(int x, int y, int t);
void LoadTexture(int page, const char *filename);

int sys_init(int argc, char **argv);
void sys_clearscreen();
void sys_updatescreen();
void sys_close();

#ifdef WIN32
#define	GL_BGRA	0x80E1
#endif
