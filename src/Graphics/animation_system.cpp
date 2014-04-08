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
#include "../Input/menu.h"
#include "../triggered.h"

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
 * @designer Robin Hsieh
 *
 * @author Jordan Marling
 * @author Mat Siwoski
 * @author Robin Hsieh
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
				
				if (SDL_GetTicks() - animation->ms_last > animation->ms_to_skip) {
					
					animation->ms_last = SDL_GetTicks();
					
					animation->index++;
					if (animation->index >= animation->surface_count) {

						animation->index = 1;
						if (animation->loop == -1) {

							animationComponent->current_animation = -1;
							renderPlayer->playerSurface = animation->surfaces[0];
							//stop_effect(animation->sound_effect);

							animation_end(world, entity);
							continue;
						}
					}
					
					if (animation->index >= animation->surface_count) {
						animation->index = animation->surface_count - 1;
					}
					
					renderPlayer->playerSurface = animation->surfaces[animation->index];

				}
			}
			else { //check if random trigger has triggered

				if (animationComponent->rand_animation < 0) {
					continue;
				}
				
				if (SDL_GetTicks() > animationComponent->next_random_occurance) {
					
					animationComponent->current_animation = animationComponent->rand_animation;
					
					animationComponent->last_random_occurance = SDL_GetTicks();
					animationComponent->next_random_occurance = (rand() % (animationComponent->rand_occurance_max - animationComponent->rand_occurance_min)) + animationComponent->rand_occurance_min + SDL_GetTicks();
					
					if (animationComponent->animations[animationComponent->rand_animation].sound_effect != MAX_EFFECTS &&
						animationComponent->animations[animationComponent->rand_animation].sound_enabled == true) {
						play_effect(animationComponent->animations[animationComponent->rand_animation].sound_effect);
					}
				}
			}

		}
	}
}

/**
 * This loads in an animation text file to create an animated component.
 *
 * @param filename The filename of the animation text file
 * @param world Pointer to the world structure (contains "world" info, entities / components)
 * @param entity The entity to create the animation for
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 * @designer Damien Sathanielle
 *
 * @author Jordan Marling
 * @author Damien Sathanielle
 */
int load_animation(const char *filename, World *world, unsigned int entity) {
	AnimationComponent *animationComponent = &(world->animation[entity]);
	RenderPlayerComponent *renderComponent = &(world->renderPlayer[entity]);

	FILE *fp;

	char animation_name[128];
	int animation_frames, ms_to_skip, loop_animation;
	int frame_index, animation_index;
	int optional_features;
	int i;
	char feature_type[128];
	char triggered_sound[128];

	char animation_filename[128];

	if ((fp = fopen(filename, "r")) == 0) {
		printf("Error opening animation file: %s\n", filename);
		return -1;
	}

	if (fscanf(fp, "%d", &animationComponent->animation_count) != 1) {
		printf("Could not read in the animation count.\n");
		return -1;
	}

	animationComponent->animations = (Animation*)malloc(sizeof(Animation) * animationComponent->animation_count);
	animationComponent->current_animation = -1;
	animationComponent->id = -1;

	animationComponent->rand_animation = -1;
	animationComponent->hover_animation = -1;

	for(animation_index = 0; animation_index < animationComponent->animation_count; animation_index++) {
		if (fscanf(fp, "%s %d %d %s %d", animation_name, &animation_frames, &ms_to_skip, triggered_sound, &loop_animation) != 5) {
			printf("Expected more animations!\n");
			return -1;
		}

		animationComponent->animations[animation_index].surfaces = (SDL_Surface**)malloc(sizeof(SDL_Surface*) * animation_frames);

		animationComponent->animations[animation_index].surface_count = animation_frames;
		if (strcmp(triggered_sound, "-1") == 0) {
			animationComponent->animations[animation_index].sound_effect = MAX_EFFECTS;
		}
		else {
			animationComponent->animations[animation_index].sound_effect = load_effect(triggered_sound);
		}
		animationComponent->animations[animation_index].loop = loop_animation;
		animationComponent->animations[animation_index].ms_last = 0;
		animationComponent->animations[animation_index].ms_to_skip = ms_to_skip;
		animationComponent->animations[animation_index].index = 0;
		animationComponent->animations[animation_index].sound_enabled = true;
		
		animationComponent->animations[animation_index].name = (char*)malloc(sizeof(char) * strlen(animation_name) + 1);
		strcpy(animationComponent->animations[animation_index].name, animation_name);

		for (frame_index = 0; frame_index < animation_frames; frame_index++) {

			if (fscanf(fp, "%s", animation_filename) != 1) {
				printf("Error reading animation file\n");
				return -1;
			}

			animationComponent->animations[animation_index].surfaces[frame_index] = IMG_Load(animation_filename);

			if (animationComponent->animations[animation_index].surfaces[frame_index] == 0) {
				printf("Error loading file: %s, %s\n", animation_filename, IMG_GetError());
			}
		}
	}

	renderComponent->playerSurface = animationComponent->animations[0].surfaces[0];

	//load optional features
	if (fscanf(fp, "%d", &optional_features) == 1) {

		for(i = 0; i < optional_features; i++) {

			if (fscanf(fp, "%s", (char*)feature_type) != 1) {
				printf("Optional Feature type error: %s\n", filename);
				return -1;
			}

			if (strcmp(feature_type, "random") == 0) {
				
				if (fscanf(fp, "%d %d %d", &(animationComponent->rand_animation), &(animationComponent->rand_occurance_min), &(animationComponent->rand_occurance_max)) != 3) {
					
					printf("Wrong parameters for random. It should be 'random <animation index> <min delay in milliseconds> <max delay in milliseconds>\n");
					
					animationComponent->rand_animation = -1;
					
				}
				else {
					animationComponent->last_random_occurance = SDL_GetTicks();
					animationComponent->next_random_occurance = (rand() % (animationComponent->rand_occurance_max - animationComponent->rand_occurance_min)) + animationComponent->rand_occurance_min + SDL_GetTicks();
				}
			}
			else if (strcmp(feature_type, "hover") == 0) {

				if (fscanf(fp, "%d", &animationComponent->hover_animation) != 1) {
					animationComponent->hover_animation = -1;
				}

			}
			else {
				printf("Did not deal with the entity type: %s\n", feature_type);
				break;
			}
		}
	}

	return 0;
}

/**
 * This cancels an entities animation and freezes it on the first frame specified by the animation_name
 *
 * @param world Pointer to the world structure (contains "world" info, entities / components)
 * @param entity The entity to cancel the animation for
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 * @designer Tim Kim
 *
 * @author Jordan Marling
 * @author Tim Kim
 */
void cancel_animation(World *world, unsigned int entity) {
	AnimationComponent *animation = &(world->animation[entity]);
	RenderPlayerComponent *render = &(world->renderPlayer[entity]);

	if (!IN_THIS_COMPONENT(world->mask[entity], COMPONENT_ANIMATION)) {
		return;
	}

	if (animation->current_animation == -1)
		return;
	
	
	render->playerSurface = animation->animations[animation->current_animation].surfaces[0];
	
	animation->current_animation = -1;
	
}

/**
 * This plays in an animation text file to create an animated component.
 *
 * @param world Pointer to the world structure (contains "world" info, entities / components)
 * @param entity The entity to cancel the animation for
 * @param animation_name The animation that gets played
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 *
 * @author Jordan Marling
 * @author Mat Siwoski
 */
void play_animation(World *world, unsigned int entity, const char *animation_name) {
	
	int i;
	AnimationComponent *animationComponent = &(world->animation[entity]);
	RenderPlayerComponent *renderComponent = &(world->renderPlayer[entity]);
	
	if (!IN_THIS_COMPONENT(world->mask[entity], COMPONENT_ANIMATION)) {
		return;
	}
	
	//Check if the current animation is already playing.
	if (animationComponent->current_animation > -1 && strcmp(animationComponent->animations[animationComponent->current_animation].name, animation_name) == 0) {
		return;
	}
	for(i = 0; i < animationComponent->animation_count; i++) {

		if (strcmp(animationComponent->animations[i].name, animation_name) == 0) {
			
			animationComponent->current_animation = i;
			animationComponent->animations[i].ms_last = SDL_GetTicks();
			animationComponent->animations[i].index = 0;
			
			renderComponent->playerSurface = animationComponent->animations[i].surfaces[0];
			
			if (animationComponent->animations[i].sound_effect != MAX_EFFECTS &&
				animationComponent->animations[i].sound_enabled == true) {
				play_effect(animationComponent->animations[i].sound_effect);
			}
			
			return;
		}

	}
	printf("I did not find the animation: %s\n", animation_name);
}
