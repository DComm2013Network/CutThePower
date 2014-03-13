/** @ingroup Graphics
 * @{ */
/** @file animation_system.cpp */
/** @} */
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../world.h"
#include "../components.h"
#include "../systems.h"
#include "../sound.h"

#include <stdlib.h>

#define SYSTEM_MASK (COMPONENT_RENDER_PLAYER | COMPONENT_ANIMATION) /**< The entity must have a animation and render component */


/**
 * Updates animations
 *
 * Used to draw animations. This component determines which stage the animation
 * is at and updates the render player component accordingly so no special system
 * is needed for animations vs. static images.
 * 
 * The animation can also be triggered at a random time, and can also trigger a sound effect.
 *
 * @param world Pointer to the world structure (contains "world" info, entities / components)
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 */
void animation_system(World *world) {
	
	unsigned int entity;
	AnimationComponent 		*animationComponent;
	RenderPlayerComponent 	*renderPlayer;
	
	Animation *animation;
	
	for(entity = 0; entity < MAX_ENTITIES; entity++){
		
		if (IN_THIS_COMPONENT(world->mask[entity], SYSTEM_MASK)){
			
			animationComponent = &(world->animation[entity]);
			renderPlayer = &(world->renderPlayer[entity]);
			
			if (animationComponent->current_animation > -1) {
				
				animation = &(animationComponent->animations[animationComponent->current_animation]);
				
				if (animation->index == 0 && animation->frame_count == 0 && animation->sound_effect > -1) {
					
					if (animation->sound_effect >= 0) {
						play_effect(animation->sound_effect);
					}
				}
				
				animation->frame_count++;
				
				if (animation->frame_count > animation->frames_to_skip) {
					
					animation->frame_count = 0;
					
					if (animation->index >= animation->surface_count) {
						
						animation->index = 0;
						animation->frame_count = 0;
						if (animation->loop == -1) {
							animationComponent->current_animation = -1;
							renderPlayer->playerSurface = animation->surfaces[0];
							continue;
						}
					}
					
					renderPlayer->playerSurface = animation->surfaces[animation->index];
					
					animation->index++;
				}
			}
			else { //check if random trigger has triggered
				
				if (animationComponent->rand_animation < 0) {
					continue;
				}
				
				if (rand() % animationComponent->rand_occurance == 0) {
					animationComponent->current_animation = animationComponent->rand_animation;
				}
			}
			
		}
	}
}

int load_animation(char *filename, World *world, unsigned int entity) {
	
	AnimationComponent *animationComponent = &(world->animation[entity]);
	RenderPlayerComponent *renderComponent = &(world->renderPlayer[entity]);
	
	FILE *fp;
	
	char animation_name[64];
	int animation_frames, frames_to_skip, triggered_sound, loop_animation;
	int frame_index, animation_index;
	
	char animation_filename[64];
	
	if ((fp = fopen(filename, "r")) == 0) {
		printf("Error opening animation file: %s\n", filename);
		return -1;
	}
	
	if (fscanf(fp, "%d", &animationComponent->animation_count) != 1) {
		printf("Could not read in the animation count.\n");
		return -1;
	}
	
	//printf("Animation count: %d\n", animationComponent->animation_count);
	
	animationComponent->animations = (Animation*)malloc(sizeof(Animation) * animationComponent->animation_count);
	animationComponent->current_animation = -1;
	
	for(animation_index = 0; animation_index < animationComponent->animation_count; animation_index++) {
		
		if (fscanf(fp, "%s %d %d %d %d", animation_name, &animation_frames, &frames_to_skip, &triggered_sound, &loop_animation) != 5) {
			printf("Expected more animations!\n");
			return -1;
		}
		
		animationComponent->animations[animation_index].surfaces = (SDL_Surface**)malloc(sizeof(SDL_Surface*) * animation_frames);
		
		animationComponent->animations[animation_index].surface_count = animation_frames;
		animationComponent->animations[animation_index].frames_to_skip = frames_to_skip;
		animationComponent->animations[animation_index].sound_effect = triggered_sound;
		animationComponent->animations[animation_index].loop = loop_animation;
		
		animationComponent->animations[animation_index].name = (char*)malloc(sizeof(char) * strlen(animation_name));
		strcpy(animationComponent->animations[animation_index].name, animation_name);
		
		for (frame_index = 0; frame_index < animation_frames; frame_index++) {
			
			if (fscanf(fp, "%s", animation_filename) != 1) {
				printf("Error loading animation file.\n");
				return -1;
			}
			
			//printf("File: %s\n", animation_filename);
			
			animationComponent->animations[animation_index].surfaces[frame_index] = IMG_Load(animation_filename);
			
			if (animationComponent->animations[animation_index].surfaces[frame_index] == 0) {
				printf("Error loading file: %s\n", animation_filename);
			}
		}
		
		//printf("Loaded animation: %s\n", animationComponent->animations[animation_index].name);
		
	}
	
	renderComponent->playerSurface = animationComponent->animations[0].surfaces[0];
	
	//load random animation
	if (fscanf(fp, "%d %d", &(animationComponent->rand_animation), &(animationComponent->rand_occurance)) != 2) {
		
		animationComponent->rand_animation = -1;
		animationComponent->rand_occurance = -1;
		
	}
	
	return 0;
}

void play_animation(AnimationComponent *animationComponent, char *animation_name) {
	
	int i;
	
	for(i = 0; i < animationComponent->animation_count; i++) {
		
		//printf("Looking at animation: %s\n", animationComponent->animations[i].name);
		
		if (strcmp(animationComponent->animations[i].name, animation_name) == 0) {
			
			if (animationComponent->current_animation == i)
				return;
			
			animationComponent->current_animation = i;
			animationComponent->animations[i].frame_count = 0;
			animationComponent->animations[i].index = 0;
			
			//printf("Playing animation: %s\n", animationComponent->animations[i].name);
			
			return;
		}
		
	}
	printf("I did not find the animation: %s\n", animation_name);
}
