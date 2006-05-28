#include "common.h"
#include "sound.h"

// TOTAL_SOUNDS is defined in sound.h
typedef struct {
	Mix_Chunk	*sound;
	char		*filename;
} sound_precache_t;

sound_precache_t	*precached_sounds[TOTAL_SOUNDS];
int		last_free = 0;
int		sound_available;

int		audio_rate,audio_channels,
	// set this to any of 512,1024,2048,4096
	// the higher it is, the more FPS shown and CPU needed
	audio_buffers=2048;
Uint16	audio_format;
int		bits;

// Music:
int			volume = SDL_MIX_MAXVOLUME;
Mix_Music	*music;
char		old_music_filename[1024];

int	sndPrecache(const char *filename) {
	int	sound, i;

	if (last_free < 0) return -1;
	if (!sound_available) return -1;

	// Find a previous slot:
	for (i=0; i<TOTAL_SOUNDS; ++i) if (precached_sounds[i] != NULL) {
		if (strcmp(precached_sounds[i]->filename, filename) == 0)
			return i;
	}

	sound = last_free;
	precached_sounds[sound] = calloc(sizeof(sound_precache_t), 1);
	precached_sounds[sound]->sound = Mix_LoadWAV(filename);
	if (precached_sounds[sound]->sound == NULL) {
		fprintf(stderr, "Mix_LoadWAV(\"%s\"): %s\n", filename, Mix_GetError());
	}
	precached_sounds[sound]->filename = calloc(strlen(filename)+1, 1);
	strcpy(precached_sounds[sound]->filename, filename);

	// Find the next free slot:
	for (i=0; i<TOTAL_SOUNDS; ++i)
		if (precached_sounds[i] == NULL) {
			last_free = i;
			break;
		}

	return sound;
}

int	sndFree(int sound) {
	if (!sound_available) return 0;
	if ((sound < 0) || (sound >= TOTAL_SOUNDS)) return 0;
	if (precached_sounds[sound] != NULL) {
		free(precached_sounds[sound]->sound);
		free(precached_sounds[sound]->filename);
		precached_sounds[sound]->sound = NULL;
		precached_sounds[sound]->filename = NULL;
		// Now, this is the last freed sound slot:
		last_free = sound;
		return -1;
	}

	return 0;
}

int	sndPlay(int sound, int channel, int loops) {
	if (!sound_available) return 0;
	if (precached_sounds[sound] == NULL) return;
	if (channel >= MAX_CHANNELS) return 0;
	if (channel < 0) return 0;
	return Mix_PlayChannel(channel, precached_sounds[sound]->sound, loops);
}

int	sndSetVolume(int channel, int volume) {
	if (!sound_available) return 0;
	if (channel >= MAX_CHANNELS) return 0;
	if (channel < 0) return 0;
	return Mix_Volume(channel, volume);
}

int sndIsPlaying(int channel) {
	if (!sound_available) return 0;
	if (channel >= MAX_CHANNELS) return 0;
	if (channel < 0) return 0;
	return Mix_Playing(channel);
}

int	sndStop(int channel) {
	if (!sound_available) return 0;
	if (channel >= MAX_CHANNELS) return 0;
	if (channel < 0) return 0;
	return Mix_HaltChannel(channel);
}

int sndInit() {
	int	i;

	SDL_InitSubSystem(SDL_INIT_AUDIO);

	// initialize sdl mixer, open up the audio device
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, audio_buffers) < 0) {
		fprintf(stderr, "Mix_OpenAudio failed!\n");
		sound_available = 0;
		return 0;
	}

	// print out some info on the audio device and stream
	Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
	bits = audio_format & 0xFF;
	fprintf(stderr, "Opened audio at %d Hz %d bit %s, %d bytes audio buffer\n", audio_rate,
			bits, audio_channels>1?"stereo":"mono", audio_buffers );

	// allocate 16 mixing channels for sound effects
	Mix_AllocateChannels(MAX_CHANNELS);

	music = NULL;
	memset(old_music_filename, 0, 1024);

	// Set all sound-slots to NULL (available):
	for (i=0; i<TOTAL_SOUNDS; ++i)
		precached_sounds[i] = NULL;

	last_free = 0;

	sound_available = -1;
	return -1;
}

int sndClose() {
	Mix_CloseAudio();
    return  0;
}

// Music functions:

int musPlay(const char *music_filename) {
	// load the song
	if ( music_filename != NULL ) {
		
		// If the filenames are different, then load the new tune,
		// otherwise continue playing the old one, since it's the same.
		if ( strcmp(music_filename, old_music_filename) ) {
			
			if ( music != NULL ) {
				Mix_FreeMusic(music);
				music = NULL;
			}
			
			if ( !(music = Mix_LoadMUS(music_filename)) ) {
				music = NULL;
				return 0;
			}
			
			// Loop level music forever:
			if ( Mix_PlayMusic(music, -1) == -1 ) {
				Mix_FreeMusic(music);
				music = NULL;
				return 0;
			}
		}
		
		Mix_VolumeMusic(volume);
		
		// Save the current music filename:
		strcpy(old_music_filename, music_filename);
		
	} else {
		
		// No music filename given, play silence.		
		if ( music != NULL ) {
			Mix_FreeMusic(music);
			music = NULL;
		}
		
	}
	
	return 0;
}
