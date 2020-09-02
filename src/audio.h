#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define AUDIO_PATH "res/detective-8bit.wav"
#define VOL_STEP 10

extern void *audio_play(void *);
extern void  audio_change_volume(char);
extern void  audio_pause(void);

#endif /* AUDIO_H */
