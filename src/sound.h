#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL_mixer.h>

#define MAX_EFFECTS			16
#define MAX_MUSIC			2

void init_sound();
void cleanup_sound();

unsigned int load_effect(const char *file);
void cleanup_effect(unsigned int index);

unsigned int load_music(const char *file);
void cleanup_music(unsigned int index);

void enable_sound(bool enabled);
void stop_music();
void stop_all_effects();
void stop_effect(unsigned int sound);

void play_music(unsigned int sound);
void play_effect(unsigned int sound);

#endif
