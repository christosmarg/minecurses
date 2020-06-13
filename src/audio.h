#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define AUDIO_PATH "res/detective-8bit.wav"
#define VOL_STEP 10

void *play_audio(void *tid);
void volume     (char option);
void pause_audio(void);

#endif /* AUDIO_H */
