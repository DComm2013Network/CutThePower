#ifndef SOUND_H
#define SOUND_H

#define SOUND_EFFECT_NUM 			1
#define SOUND_EFFECT_MENU_THUNDER 	0

#define SOUND_MUSIC_NUM 			2
#define SOUND_MUSIC_MENU_RAIN 		0
#define SOUND_MAP_LOBBY_BACKGROUND 	1

void init_sound();
void cleanup_sound();

void enable_sound(bool enabled);
void stop_music();
void stop_effect();

void play_music(int sound);
void play_effect(int sound);

#endif
