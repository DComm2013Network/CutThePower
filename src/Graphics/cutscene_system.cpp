/** @ingroup Graphics
 * @{ */
/** @file cutscene_system.cpp */
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

#define SYSTEM_MASK (COMPONENT_POSITION | COMPONENT_ANIMATION | COMPONENT_CUTSCENE) /**< The entity must have a animation and render component */

void start_cutscene_section(int id, World *world, unsigned int entity);


/**
 * Creates the Cutscene Animation system
 *
 * Used to draw animations. This component determines which stage the animation
 * is at and updates the render player component accordingly so no special system
 * is needed for animations vs. static images. As well, it will place the animation in
 * the correct position.
 * 
 * The animation can also be triggered at a random time, and can also trigger a sound effect.
 *
 * @param world Pointer to the world structure (contains "world" info, entities / components)
 *
 * @designer Jordan Marling
 * @designer Mat Siwoski
 * @designer Damien Sathanielle
 *
 * @author Jordan Marling
 * @author Mat Siwoski
 * @author Damien Sathanielle
 */
void cutscene_system(World *world) {
	
	unsigned int entity;
	
	PositionComponent *position;
	CutsceneComponent *cutscene;
	CutsceneSection *section;
	
	//used to interpolate the position.
	float percent_time;
	float x_start, y_start;
	float x_diff, y_diff;
	
	for(entity = 0; entity < MAX_ENTITIES; entity++) {
		
		if (IN_THIS_COMPONENT(world->mask[entity], SYSTEM_MASK)) {
			
			position = &world->position[entity];
			cutscene = &world->cutscene[entity];
			
			section = &cutscene->sections[cutscene->current_section];
			
			//printf("[%u] total ms: %u section: %d\n", entity, section->total_ms, cutscene->current_section);
			
			//If the animation has gone too long, switch it
			if (section->total_ms <= SDL_GetTicks() - section->start_ms) {
				
				cutscene->current_section++;
				//check to see if the cutscene is over
				if (cutscene->current_section >= cutscene->num_sections) {
					
					cutscene_end(world, entity);
					destroy_entity(world, entity);
					//printf("Destroyed entity\n");
					
					continue;
				}
				
				//printf("Playing cutscene %s\n", cutscene->sections[cutscene->current_section].animation_name);
				
				//If the animation name is 0, don't render
				if (strcmp(cutscene->sections[cutscene->current_section].animation_name, "0") == 0) {
					disable_component(world, entity, COMPONENT_RENDER_PLAYER);
				}
				else {
					enable_component(world, entity, COMPONENT_RENDER_PLAYER);
					play_animation(world, entity, cutscene->sections[cutscene->current_section].animation_name);
				}
				cutscene->sections[cutscene->current_section].start_ms = SDL_GetTicks();
			}
			else {
				
				//get the percent of the position.
				percent_time = (float)(SDL_GetTicks() - section->start_ms) / section->total_ms;
				
				x_start = cutscene->xpos;
				y_start = cutscene->ypos;
				
				//if the section is not the first, get the last x and y positions.
				if (cutscene->current_section > 0) {
					x_start = cutscene->sections[cutscene->current_section - 1].x_end;
					y_start = cutscene->sections[cutscene->current_section - 1].y_end;
				}
				
				x_diff = section->x_end - x_start;
				y_diff = section->y_end - y_start;
				
				position->x = (percent_time * x_diff) + x_start;
				position->y = (percent_time * y_diff) + y_start;
				
				//printf("New position: %f %f [%f]\n", position->x, position->y, percent_time);
			}
		}
		
		
	}
	
}


/**
 * Loads cut scene animations
 *
 * Used to draw the cut scene animations. This component determines which stage the animation
 * is at and updates the render player component accordingly so no special system
 * is needed for animations vs. static images.
 * 
 * @param filename The name of the file
 * @param id The id of the file
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
unsigned int load_cutscene(const char *filename, World *world, int id) {
	
	unsigned int entity = create_entity(world, COMPONENT_POSITION | COMPONENT_RENDER_PLAYER | COMPONENT_ANIMATION | COMPONENT_CUTSCENE);
	CutsceneComponent *cutscene = &world->cutscene[entity];
	
	FILE *fp;
	
	float xpos, ypos;
	int width, height;
	char animation_filename[128] = {0};
	char animation_name[128] = {0};
	
	int i;
	
	if ((fp = fopen(filename, "r")) == 0) {
		printf("Error opening cutscene: %s\n", filename);
		return MAX_ENTITIES;
	}
	
	if (fscanf(fp, "%d %f %f %d %d %s", &cutscene->num_sections, &xpos, &ypos, &width, &height, animation_filename) != 6) {
		printf("Error loading number of sections and initial position.\n");
		return MAX_ENTITIES;
	}
	
	//printf("num_sections: %d\n", cutscene->num_sections);
	
	cutscene->sections = (CutsceneSection*)malloc(sizeof(CutsceneSection) * cutscene->num_sections);
	
	for(i = 0; i < cutscene->num_sections; i++) {
		
		if (fscanf(fp, "%u %f %f %s", &cutscene->sections[i].total_ms, &cutscene->sections[i].x_end, &cutscene->sections[i].y_end, animation_name) != 4) {
			printf("Error loading cutscene section!\n");
			break;
		}
		
		//printf("Loaded animation %s go for %u to (%f, %f)\n", animation_name, cutscene->sections[i].total_ms, cutscene->sections[i].x_end, cutscene->sections[i].y_end);
		
		cutscene->sections[i].animation_name = (char*)malloc((sizeof(char) * strlen(animation_name)) + 1);
		strcpy(cutscene->sections[i].animation_name, animation_name);
		
	}
	
	cutscene->current_section = 0;
	cutscene->sections[0].start_ms = SDL_GetTicks();
	
	cutscene->xpos = xpos;
	cutscene->ypos = ypos;
	
	cutscene->id = id;
	
	world->position[entity].x = xpos;
	world->position[entity].y = ypos;
	
	world->position[entity].width = width;
	world->position[entity].height = height;
	
	world->renderPlayer[entity].width = width;
	world->renderPlayer[entity].height = height;
	
	if (strcmp(animation_filename, "0") != 0) {
		load_animation(animation_filename, world, entity);
	}
	
	//If the animation name is 0, don't render
	if (strcmp(cutscene->sections[cutscene->current_section].animation_name, "0") == 0) {
		disable_component(world, entity, COMPONENT_RENDER_PLAYER);
	}
	else {
		play_animation(world, entity, cutscene->sections[cutscene->current_section].animation_name);
	}
	
	return entity;
}
