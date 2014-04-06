#include "sound.h"
#include <SDL2/SDL_mixer.h>
#include <stdlib.h>
#include <stdio.h>

Mix_Chunk *effects[MAX_EFFECTS];
Mix_Music *music[MAX_MUSIC];

int chunk_channels[MAX_EFFECTS];

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
	
	unsigned int i;
	
	Mix_Init(MIX_INIT_MP3);
	
	//initialize audio mixer.
	if(Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        return; 
    }
	
	for(i = 0; i < MAX_EFFECTS; i++) {
		effects[i] = 0;
	}
	
	for(i = 0; i < MAX_MUSIC; i++) {
		music[i] = 0;
	}
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
	
	unsigned int i;
	
	for(i = 0; i < MAX_MUSIC; i++) {
		cleanup_music(i);
	}
	
	for(i = 0; i < MAX_EFFECTS; i++) {
		cleanup_effect(i);
	}
	
	Mix_CloseAudio();
}

unsigned int load_effect(const char *file) {
	
	unsigned int i;
	
	for(i = 0; i < MAX_EFFECTS; i++) {
		if (effects[i] == 0) {
			effects[i] = Mix_LoadWAV(file);
			if (effects[i] == 0) {
				printf("Error loading effect file: %s\n", file);
			}
			break;
		}
	}
	
	if (i == MAX_EFFECTS) {
		printf("Loading too many effects\n");
	}
	
	return i;
}

void cleanup_effect(unsigned int index) {
	if (index < 0 || index > MAX_EFFECTS)
		return;
	
	Mix_FreeChunk(effects[index]);
	effects[index] = 0;
}

unsigned int load_music(const char *file) {
	unsigned int i;
	
	for(i = 0; i < MAX_MUSIC; i++) {
		if (music[i] == 0) {
			music[i] = Mix_LoadMUS(file);
			if (music[i] == 0) {
				printf("Error loading music file: %s: %s\n", file, Mix_GetError());
			}
			break;
		}
	}
	
	if (i == MAX_EFFECTS) {
		printf("Loading too much music\n");
	}
	
	return i;
}

void cleanup_music(unsigned int index) {
	if (index < 0 || index > MAX_MUSIC)
		return;
	
	Mix_FreeMusic(music[index]);
	music[index] = 0;
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
		Mix_VolumeMusic(0);
	}
	else {
		Mix_VolumeMusic(MIX_MAX_VOLUME);
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
void play_music(unsigned int sound) {
	
	if (!soundon)
		return;
	
	if (Mix_PlayingMusic() == 1) {
		stop_music();
	}
	
	
	Mix_PlayMusic(music[sound], -1);
	//printf("Playing music %u\n", sound);
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
void stop_effect(unsigned int sound) {
	Mix_HaltChannel(chunk_channels[sound]);
}

void stop_all_effects() {
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
void play_effect(unsigned int sound) {
	
	if (soundon) {
		chunk_channels[sound] = Mix_PlayChannel(-1, effects[sound], 0);
		//printf("Playing effect %u\n", sound);
	}
	
}

