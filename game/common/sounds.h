// sounds.h  -  defines structure for holding precached sounds

enum soundchannels {
	SOUNDCHANNEL_AMBIENT,
	SOUNDCHANNEL_PLAYER,
	SOUNDCHANNEL_PAIN,
	SOUNDCHANNEL_MONSTERS,
	SOUNDCHANNEL_HOVER
};

typedef struct {
	struct {
		int		explode;
		int		cannon;
		int		hover;
		int		hyper;
		int		jump;
		int		land;
		int		warp;
	} tank;
	struct {
		int		enter_exit;
		int		shoot;
	} jason;
	struct {
		int		beep;
	} mine;
	struct {
		int		regular;
		int		flashy;
		int		weapon;
	} powerup;
	struct {
		int		flyby;
	} ring;
	struct {
		int		menu;
	} ui;
	struct {
		int		explode1;
		int		explode2;
		int		explode3;
		int		drop;
		int		monster_hit;
		int		damage;
	} generic;
} sounds_t;

extern	sounds_t	sounds;


