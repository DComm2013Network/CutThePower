#include "sound.h"
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>

Mix_Music **music;
Mix_Chunk **effects;

bool soundon = true;

/**
 * Loads all sound files into memory.
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void init_sound() {
	
	//initialize audio mixer.
	if(Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1) {
        return;    
    }
	
	music = (Mix_Music**)malloc(sizeof(Mix_Music*) * SOUND_MUSIC_NUM);
	effects = (Mix_Chunk**)malloc(sizeof(Mix_Chunk*) * SOUND_EFFECT_NUM);
	
	music[0] = Mix_LoadMUS("assets/Sound/menu/sound_menu_bg.wav");
	music[1] = Mix_LoadMUS("assets/Sound/maps/lobby_bg.wav");
	
	effects[0] = Mix_LoadWAV("assets/Sound/menu/sound_menu_thunder.wav");
	
}

/**
 * Deletes all of the resources it loaded into memory.
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void cleanup_sound() {
	
	int i;
	
	for(i = 0; i < SOUND_MUSIC_NUM; i++) {
		Mix_FreeMusic(music[i]);
	}
	
	for(i = 0; i < SOUND_EFFECT_NUM; i++) {
		Mix_FreeChunk(effects[i]);
	}
	
	Mix_CloseAudio();
}

/**
 * Enables/disables playing sound
 *
 * @param enabled if the sound is turned on or off.
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void enable_sound(bool enabled) {
	soundon = enabled;
	if (!soundon) {
		stop_music();
		stop_effect();
	}
}

/**
 * Plays a music file and loops it.
 *
 * @param sound A defined sound that is loaded.
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void play_music(int sound) {
	
	if (!soundon)
		return;
	
	if (Mix_PlayingMusic() == 1) {
		stop_music();
	}
	
	Mix_PlayMusic(music[sound], -1);
	
}

/**
 * Stops the music.
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void stop_music() {
	Mix_HaltMusic();
}

/**
 * Stops a sound effect.
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void stop_effect() {
	Mix_HaltChannel(0);
	Mix_HaltChannel(1);
}

/**
 * Plays a sound effect.
 *
 * @param sound A defined sound that is loaded.
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void play_effect(int sound) {
	
	if (!soundon)
		return;
	
	Mix_PlayChannel(-1, effects[sound], 0);
	
}

