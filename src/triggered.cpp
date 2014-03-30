#include <string.h>
#include <stdio.h>

#include "triggered.h"
#include "systems.h"
#include "world.h"
#include "components.h"
#include "sound.h"
#include "Input/menu.h"
#include "Graphics/text.h"

#define SHOW_MENU_INTRO 0 //1 == load intro, 0 == load straight into map

extern bool running;
extern unsigned int player_entity;

int player_number;


bool menu_click(World *world, unsigned int entity) {
	
	//printf("Clicked: %s\n", world->button[entity].label);
	
	//MAIN MENU
	if (strcmp(world->button[entity].label, "mainmenu_play") == 0) {
		
		destroy_menu(world);
		
		create_select_screen(world);
		
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
		
		world->position[entity].x = (WIDTH / 2);
		render_small_text(world, entity, "SOUND ON");
		
		world->button[entity].label = (char*)realloc(world->button[entity].label, sizeof(char) * strlen("options_sound_on"));
		strcpy(world->button[entity].label, "options_sound_on");
		
		enable_sound(true);
		play_music(SOUND_MUSIC_MENU_RAIN);
		
	}
	else if (strcmp(world->button[entity].label, "options_sound_on") == 0) {
		
		world->position[entity].x = (WIDTH / 2);
		render_small_text(world, entity, "SOUND OFF");
		
		world->button[entity].label = (char*)realloc(world->button[entity].label, sizeof(char) * strlen("options_sound_off"));
		strcpy(world->button[entity].label, "options_sound_off");
		
		enable_sound(false);
		
	}
	else if (strcmp(world->button[entity].label, "options_keymap") == 0) {
		
		destroy_menu(world);
		
		create_keymap_menu(world);
		
	}
	
	//KEYMAP
	else if (strcmp(world->button[entity].label, "keymap_back") == 0) {
		
		destroy_menu(world);
		
		KeyMapInit("assets/Input/keymap.txt");
		
		create_options_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "keymap_save") == 0) {

		FILE * keymapFile = fopen("assets/Input/keymap.txt", "w+");

		for(int i = 0; i < MAX_ENTITIES; i++) {
			
			if (IN_THIS_COMPONENT(world->mask[i], COMPONENT_TEXTFIELD)) {
								

				if (strcmp(world->text[i].name, "keymap_up") == 0) {
					fprintf(keymapFile, "C_UP %s\n", world->text[i].text);
				}
				else if (strcmp(world->text[i].name, "keymap_down") == 0) {
					fprintf(keymapFile, "C_DOWN %s\n", world->text[i].text);
				}
				else if (strcmp(world->text[i].name, "keymap_left") == 0) {
					fprintf(keymapFile, "C_LEFT %s\n", world->text[i].text);
				}
				else if (strcmp(world->text[i].name, "keymap_right") == 0) {
					fprintf(keymapFile, "C_RIGHT %s\n", world->text[i].text);
				}
				else if (strcmp(world->text[i].name, "keymap_action") == 0) {
					fprintf(keymapFile, "C_ACTION %s\n", world->text[i].text);
				}
				
			}
		}
		fclose(keymapFile);
		
	}

	else if (strcmp(world->button[entity].label, "keymap_default") == 0) {

		FILE * keymapFile = fopen("assets/Input/keymap.txt", "w+");

		for(int i = 0; i < MAX_ENTITIES; i++) {
			
			if (IN_THIS_COMPONENT(world->mask[i], COMPONENT_TEXTFIELD)) {
								

				if (strcmp(world->text[i].name, "keymap_up") == 0) {
					fprintf(keymapFile, "C_UP W\n");
				}
				else if (strcmp(world->text[i].name, "keymap_down") == 0) {
					fprintf(keymapFile, "C_DOWN S\n");
				}
				else if (strcmp(world->text[i].name, "keymap_left") == 0) {
					fprintf(keymapFile, "C_LEFT A\n");
				}
				else if (strcmp(world->text[i].name, "keymap_right") == 0) {
					fprintf(keymapFile, "C_RIGHT D\n");
				}
				else if (strcmp(world->text[i].name, "keymap_action") == 0) {
					fprintf(keymapFile, "C_ACTION SPACE\n");
				}
				
			}
		}
		fclose(keymapFile);

		destroy_menu(world);
		
		create_keymap_menu(world);
		
	}
	//SELECT SCREEN
	else if (strcmp(world->button[entity].label, "menu_select_abhishek") == 0) {
		
		destroy_menu(world);
		player_number = 0;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_aman") == 0) {
		
		destroy_menu(world);
		player_number = 1;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_andrew") == 0) {
		
		destroy_menu(world);
		player_number = 2;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_chris") == 0) {
		
		destroy_menu(world);
		player_number = 3;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_clark") == 0) {
		
		destroy_menu(world);
		player_number = 4;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_cory") == 0) {
		
		destroy_menu(world);
		player_number = 5;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_damien") == 0) {
		
		destroy_menu(world);
		player_number = 6;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_german") == 0) {
		
		destroy_menu(world);
		player_number = 7;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_ian") == 0) {
		
		destroy_menu(world);
		player_number = 8;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_jordan") == 0) {
		
		destroy_menu(world);
		player_number = 9;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_josh") == 0) {
		
		destroy_menu(world);
		player_number = 10;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_konst") == 0) {
		
		destroy_menu(world);
		player_number = 11;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_mat") == 0) {
		
		destroy_menu(world);
		player_number = 12;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_ramzi") == 0) {
		
		destroy_menu(world);
		player_number = 13;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_robin") == 0) {
		
		destroy_menu(world);
		player_number = 14;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_sam") == 0) {
		
		destroy_menu(world);
		player_number = 15;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_shane") == 0) {
		
		destroy_menu(world);
		player_number = 16;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_tim") == 0) {
		
		destroy_menu(world);
		player_number = 17;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_vincent") == 0) {
		
		destroy_menu(world);
		player_number = 18;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_random") == 0) {
		
		destroy_menu(world);
		player_number = (rand() % 18) + 1; 
		create_setup_menu(world);
		
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
		
		printf("Username: %s\n", username);
		printf("Server IP: %s\n", serverip);
		
		
		#if SHOW_MENU_INTRO 
		
		destroy_world(world);
		create_load_screen(world);
		
		#else
		
		world->animation[entity].id = 0;
		
		animation_end(world, entity);
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
		return false;
	}
	return true;
}

void animation_end(World *world, unsigned int entity) {
	
	AnimationComponent *animationComponent = &(world->animation[entity]);
	
	//INTRO SCREEN ENDED
	if (animationComponent->id == 0) { //0 is the intro screen
		
		destroy_world(world);
		
		stop_music();
		stop_effect();
		
		map_init(world, "assets/Graphics/map/map_00/map00.txt", "assets/Graphics/map/map_00/tiles.txt");
		player_entity = create_player(world, 600, 400, true, COLLISION_HACKER);
		
		world->mask[player_entity] |= COMPONENT_ANIMATION;
		if ( player_number == 0){
			load_animation("assets/Graphics/player/abhishek/animation.txt", world, player_entity);
		}else if (player_number == 1) {
			load_animation("assets/Graphics/player/aman/animation.txt", world, player_entity);
		}else if (player_number == 2) {
			load_animation("assets/Graphics/player/andrew/animation.txt", world, player_entity);
		}else if (player_number == 3) {
			load_animation("assets/Graphics/player/chris/animation.txt", world, player_entity);
		}else if (player_number == 4) {
			load_animation("assets/Graphics/player/clark/animation.txt", world, player_entity);
		}else if (player_number == 5) {
			load_animation("assets/Graphics/player/cory/animation.txt", world, player_entity);
		}else if (player_number == 6) {
			load_animation("assets/Graphics/player/damien/animation.txt", world, player_entity);
		}else if (player_number == 7) {
			load_animation("assets/Graphics/player/german/animation.txt", world, player_entity);
		}else if (player_number == 8) {
			load_animation("assets/Graphics/player/ian/animation.txt", world, player_entity);
		}else if (player_number == 9) {
			load_animation("assets/Graphics/player/jordan/animation.txt", world, player_entity);
		}else if (player_number == 10) {
			load_animation("assets/Graphics/player/josh/animation.txt", world, player_entity);
		}else if (player_number == 11) {
			load_animation("assets/Graphics/player/konst/animation.txt", world, player_entity);
		}else if (player_number == 12) {
			load_animation("assets/Graphics/player/mat/animation.txt", world, player_entity);
		}else if (player_number == 13) {
			load_animation("assets/Graphics/player/ramzi/animation.txt", world, player_entity);
		}else if (player_number == 14) {
			load_animation("assets/Graphics/player/robin/animation.txt", world, player_entity);
		}else if (player_number == 15) {
			load_animation("assets/Graphics/player/sam/animation.txt", world, player_entity);
		}else if (player_number == 16) {
			load_animation("assets/Graphics/player/shane/animation.txt", world, player_entity);
		}else if (player_number == 17) {
			load_animation("assets/Graphics/player/tim/animation.txt", world, player_entity);
		}else if (player_number == 18) {
			load_animation("assets/Graphics/player/vincent/animation.txt", world, player_entity);
		}
		else{
			load_animation("assets/Graphics/player/p0/rob_animation.txt", world, player_entity);
		}
		
		
		
		//PUT CLIENT START CODE HERE!!!@!@!!!!
		
		
	}
	//LOADING SCREEN ENDED
	else if (animationComponent->id == 1) { //1 is the loading screen
		
		destroy_world(world);
		
		create_intro(world);
	}
}


