#ifndef SOUND_H
#define SOUND_H

#include "common.h"
#include <SDL/SDL_mixer.h>

#define TOTAL_SOUNDS	64
#define	MAX_CHANNELS	16

extern	int			volume;
extern	Mix_Music	*music;

int	sndPrecache(const char *filename);
int	sndFree(int sound);
int	sndPlay(int sound, int channel, int loops);
int	sndSetVolume(int channel, int volume);
int sndIsPlaying(int channel);
int	sndStop(int channel);
int sndInit();
int sndClose();

#endif
