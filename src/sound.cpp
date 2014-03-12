#include "sound.h"
#include "SDL/SDL_mixer.h"
#include <stdlib.h>
#include <stdio.h>

Mix_Music **music;
Mix_Chunk **effects;

void init_sound() {
	
	//initialize audio mixer.
	if(Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1) {
        return;    
    }
	
	music = (Mix_Music**)malloc(sizeof(Mix_Music*) * SOUND_MUSIC_NUM);
	effects = (Mix_Chunk**)malloc(sizeof(Mix_Chunk*) * SOUND_EFFECT_NUM);
	
	music[0] = Mix_LoadMUS("assets/Sound/menu/sound_menu_bg.wav");
	
	effects[0] = Mix_LoadWAV("assets/Sound/menu/sound_menu_thunder.wav");
	
}

void cleanup_sound() {
	
	
	
}

void play_music(int sound) {
	
	if (Mix_PlayingMusic() == 1) {
		stop_music();
	}
	
	Mix_PlayMusic(music[sound], -1);
	
}

void stop_music() {
	Mix_HaltMusic();
}

void play_effect(int sound) {
	
	Mix_PlayChannel(-1, effects[sound], 0);
	
}
