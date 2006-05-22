// -- Main sprite routines --
extern int	level_loaded;
extern double	time_dt;
extern double	accel_scale;

int put_sprite(fixed x,fixed y,int m,GLuint sprite);
int put_sprite_hflip(fixed x,fixed y,int m,GLuint sprite);
int put_sprite_vflip(fixed x,fixed y,int m,GLuint sprite);
int put_sprite_hvflip(fixed x,fixed y,int m,GLuint sprite);

int put_bgtile(fixed x,fixed y,int m,unsigned char t, int bg);
void Draw2x2BGTile(fixed x, fixed y, int t, int bg);
void LoadTexture(int page, const char *filename);

void sys_init(int argc, char **argv);
void sys_clearscreen();
void sys_updatescreen();
void sys_eventloop(Uint8 *control_keys);
void sys_close();

#ifdef WIN32
#define	GL_BGRA	0x80E1
#endif

