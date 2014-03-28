#include <string.h>
#include <stdio.h>

#include "triggered.h"
#include "systems.h"
#include "world.h"
#include "components.h"
#include "sound.h"
#include "Input/menu.h"

extern bool running;
extern unsigned int player_entity;

void menu_click(World *world, unsigned int entity) {
	
	printf("Clicked: %s\n", world->button[entity].label);
	
	//MAIN MENU
	if (strcmp(world->button[entity].label, "mainmenu_play") == 0) {
		
		destroy_menu(world);
		
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "mainmenu_options") == 0) {
		
		destroy_menu(world);
		
		create_options_menu(world);
	}
	else if (strcmp(world->button[entity].label, "mainmenu_credits") == 0) {
		
		destroy_menu(world);
		
		create_credits_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "mainmenu_exit") == 0) {
		destroy_world(world);

		running = false;
	}
	
	//OPTIONS
	else if (strcmp(world->button[entity].label, "options_back") == 0) {
		
		destroy_menu(world);
		
		create_main_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "options_sound_off") == 0) {
		
		world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/menu/menu_button_soundon.png");
		world->button[entity].label = "options_sound_on";
		
		enable_sound(true);
		play_music(SOUND_MUSIC_MENU_RAIN);
		
	}
	else if (strcmp(world->button[entity].label, "options_sound_on") == 0) {
		
		world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/menu/menu_button_soundoff.png");
		world->button[entity].label = "options_sound_off";
		
		enable_sound(false);
		
	}
	else if (strcmp(world->button[entity].label, "options_keymap") == 0) {
		
		destroy_menu(world);
		
		create_keymap_menu(world);
		
	}
	
	//KEYMAP
	else if (strcmp(world->button[entity].label, "keymap_back") == 0) {
		
		destroy_menu(world);
		
		create_options_menu(world);
		
	}
	
	//CREDITS
	else if (strcmp(world->button[entity].label, "credits_back") == 0) {
		
		destroy_menu(world);
		
		create_main_menu(world);
		
	}
	
	//SETUP
	else if (strcmp(world->button[entity].label, "setup_back") == 0) {
		
		destroy_menu(world);
		
		create_main_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "setup_play") == 0) {
		
		//map_init(world, "assets/Graphics/map/map_01/map01.txt", "assets/Graphics/map/map_01/map01_tiles.txt");
		//map_init(world, "assets/Graphics/lobby/lobby.txt", "assets/Graphics/lobby/lobby_tiles.txt");
		//map_init(world, "assets/Graphics/SampleFloor.txt", "assets/Graphics/tiles_lobby.txt");
		
		char *username;
		char *serverip;
		
		unsigned int i;
		
		stop_music();
		stop_effect();
		
		for(i = 0; i < MAX_ENTITIES; i++) {
			
			if (IN_THIS_COMPONENT(world->mask[i], COMPONENT_TEXTFIELD)) {
				
				if (strcmp(world->text[i].name, "setup_username") == 0) {
					username = world->text[i].text;
				}
				else if (strcmp(world->text[i].name, "setup_serverip") == 0) {
					serverip = world->text[i].text;
				}
				
			}
		}
		
		
		destroy_world(world);
		
		
		#if 1//1 == load into map, 0 == load into intro.
		
		map_init(world, "assets/Graphics/map/map_01/map01.txt", "assets/Graphics/map/map_01/map01_tiles.txt");
		player_entity = create_player(world, 600, 600, true, COLLISION_HACKER);
		
		world->mask[player_entity] |= COMPONENT_ANIMATION;
		
		load_animation("assets/Graphics/player/player_josh/josh_animation.txt", world, player_entity);

		/*player_entity = create_player(world, 700, 600, false, COLLISION_GUARD);

		world->mask[player_entity] |= COMPONENT_ANIMATION;
		
		load_animation("assets/Graphics/player/robber/rob_animation.txt", world, player_entity);*/
		
		animation_end(world, entity, 0);
		#else
		//create_load_screen(world);
		create_intro(world);
		#endif
		
	}
	
	//BSOD
	else if (strcmp(world->button[entity].label, "bsod_exit") == 0) {
		
		destroy_world(world);
		
		exit(0);
	}
	else if (strcmp(world->button[entity].label, "bsod_continue") == 0) {
		
		destroy_menu(world);
		
		create_main_menu(world);
	}
	
	else {
		printf("DID NOT HANDLE BUTTON: %s\n", world->button[entity].label);
	}
}

void animation_end(World *world, unsigned int entity, int ANIMATION_ID) {
	
	AnimationComponent *animationComponent = &(world->animation[entity]);
	
	//INTRO SCREEN ENDED
	if (animationComponent->id == 0) { //0 is the intro screen!
									
		destroy_world(world);

		stop_music();
		stop_effect();
		
		//map_init(world, "assets/Graphics/map/map_01/map01.txt", "assets/Graphics/map/map_01/map01_tiles.txt");
		//map_init(world, "assets/Graphics/lobby/lobby.txt", "assets/Graphics/lobby/lobby_tiles.txt");
		//map_init(world, "assets/Graphics/SampleFloor.txt", "assets/Graphics/tiles_lobby.txt");
		
		map_init(world, "assets/Graphics/map/map_01/map01.txt", "assets/Graphics/map/map_01/map01_tiles.txt");
		player_entity = create_player(world, 600, 600, true, COLLISION_HACKER);
							
		world->mask[player_entity] |= COMPONENT_ANIMATION;
		
		load_animation("assets/Graphics/player/robber/rob_animation.txt", world, player_entity);

		
		//PUT CLIENT START CODE HERE!!!@!@!!!!
		
		
		
	}
	//LOADING SCREEN ENDED
	else if (animationComponent->id == 1) { //1 is the loading screen!
		
		destroy_world(world);
		
		create_intro(world);
	}
}

void change_level(World * world, int targetLevel) {
	/*int e = 0;
	for (e = 0; e < MAX_ENTITIES; e++) {
		if (!(world->mask[e] & COMPONENT_PLAYER)) {
			world->mask[e] = COMPONENT_EMPTY;
		}
	}*/
	map_init(world, "assets/Graphics/map/map_02/map02.txt", "assets/Graphics/map/map_02/map02_tiles.txt");
}


