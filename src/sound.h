#ifndef SOUND_H
#define SOUND_H

#define SOUND_EFFECT_NUM 1
#define SOUND_EFFECT_MENU_THUNDER 0

#define SOUND_MUSIC_NUM 1
#define SOUND_MUSIC_MENU_RAIN 0

void init_sound();
void cleanup_sound();

void stop_music();
void play_music(int sound);
void play_effect(int sound);

#endif
