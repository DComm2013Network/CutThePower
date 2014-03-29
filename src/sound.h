#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL_mixer.h>

#define SOUND_EFFECT_NUM 			2
#define SOUND_EFFECT_MENU_THUNDER 	0
#define SOUND_EFFECT_LOGO			1

#define SOUND_MUSIC_NUM 			2
#define SOUND_MUSIC_MENU_RAIN 		0
#define SOUND_MAP_LOBBY_BACKGROUND 	1

typedef struct {
	int index;
	Mix_Chunk *chunk;
} SoundEffect;

void init_sound();
void cleanup_sound();

void enable_sound(bool enabled);
void stop_music();
void stop_effect();

void play_music(int sound);
void play_effect(int sound);

#endif
