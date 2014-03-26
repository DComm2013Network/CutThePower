#include <string.h>
#include <stdio.h>

#include "triggered.h"
#include "systems.h"
#include "world.h"
#include "components.h"
#include "sound.h"
#include "Input/menu.h"

#define SHOW_MENU_INTRO 0 //1 == load intro, 0 == load straight into map

extern bool running;
extern unsigned int player_entity;
extern int send_router_fd[];
extern int rcv_router_fd[];
extern int game_net_signalfd;
int character;
char *username;
char *serverip;

bool menu_click(World *world, unsigned int entity) {
	
	char* playerFileName;
	
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
		
		world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/menu/menu_button_soundon.png");
		world->button[entity].label = (char*)"options_sound_on";
		
		enable_sound(true);
		play_music(SOUND_MUSIC_MENU_RAIN);
		
	}
	else if (strcmp(world->button[entity].label, "options_sound_on") == 0) {
		
		world->renderPlayer[entity].playerSurface = IMG_Load("assets/Graphics/screen/menu/menu_button_soundoff.png");
		world->button[entity].label = (char*)"options_sound_off";
		
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
		character = ABHISHEK;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_aman") == 0) {
		
		destroy_menu(world);
		character = AMAN;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_andrew") == 0) {
		
		destroy_menu(world);
		character = ANDREW;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_chris") == 0) {
		
		destroy_menu(world);
		character = CHRIS;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_clark") == 0) {
		
		destroy_menu(world);
		character = CLARK;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_cory") == 0) {
		
		destroy_menu(world);
		character = CORY;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_damien") == 0) {
		
		destroy_menu(world);
		character = DAMIEN;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_german") == 0) {
		
		destroy_menu(world);
		character = GERMAN;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_ian") == 0) {
		
		destroy_menu(world);
		character = IAN;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_jordan") == 0) {
		
		destroy_menu(world);
		character = JORDAN;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_josh") == 0) {
		
		destroy_menu(world);
		character = JOSH;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_konst") == 0) {
		
		destroy_menu(world);
		character = KONST;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_mat") == 0) {
		
		destroy_menu(world);
		character = MAT;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_ramzi") == 0) {
		
		destroy_menu(world);
		character = RAMZI;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_robin") == 0) {
		
		destroy_menu(world);
		character = ROBIN;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_sam") == 0) {
		
		destroy_menu(world);
		character = SAM;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_shane") == 0) {
		
		destroy_menu(world);
		character = SHANE;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_tim") == 0) {
		
		destroy_menu(world);
		character = TIM;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_vincent") == 0) {
		
		destroy_menu(world);
		character = VINCENT;
		create_setup_menu(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_random") == 0) {
		
		destroy_menu(world);
		character = rand()%18;
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
		
		//map_init(world, "assets/Graphics/map/map_01/map01.txt", "assets/Graphics/map/map_01/map01_tiles.txt");
		//map_init(world, "assets/Graphics/lobby/lobby.txt", "assets/Graphics/lobby/lobby_tiles.txt");
		//map_init(world, "assets/Graphics/SampleFloor.txt", "assets/Graphics/tiles_lobby.txt");
		
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
		
		destroy_world(world);
		
		
		#if SHOW_MENU_INTRO 
		
		create_load_screen(world);
		//create_intro(world);
		
		#else
		
		//map_init(world, "assets/Graphics/map/map_01/map01.txt", "assets/Graphics/map/map_01/map01_tiles.txt");
		//player_entity = create_player(world, 600, 600, true, COLLISION_HACKER);
		
		//world->mask[player_entity] |= COMPONENT_ANIMATION;
		//load_animation("assets/Graphics/player/robber/rob_animation.txt", world, player_entity);
		
		//printf("Loaded player: %d\n", player_entity);
		
		animation_end(world, entity, 0);
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

void animation_end(World *world, unsigned int entity, int animation_id) {
	
	AnimationComponent *animationComponent = &(world->animation[entity]);
	
	//INTRO SCREEN ENDED
	if (animationComponent->id == 0) { //0 is the intro screen
					
		destroy_world(world);
		
		stop_music();
		stop_effect();
		
		//map_init(world, "assets/Graphics/map/map_01/map01.txt", "assets/Graphics/map/map_01/map01_tiles.txt");
		//map_init(world, "assets/Graphics/lobby/lobby.txt", "assets/Graphics/lobby/lobby_tiles.txt");
		//map_init(world, "assets/Graphics/SampleFloor.txt", "assets/Graphics/tiles_lobby.txt");
		
		map_init(world, (char*)"assets/Graphics/map/map_00/map00.txt", (char*)"assets/Graphics/map/map_00/tiles.txt");
		player_entity = create_player(world, 620, 420, true, COLLISION_HACKER, 0, character);
		
		load_animation((char*)"assets/Graphics/player/p0/rob_animation.txt", world, player_entity);
		
		////NETWORK CODE
		game_net_signalfd 	= eventfd(0, EFD_SEMAPHORE);

		init_client_network(send_router_fd, rcv_router_fd);
		send_intialization(world, send_router_fd[WRITE_END], username, serverip);
		//PUT CLIENT START CODE HERE!!!@!@!!!!
	}
	//LOADING SCREEN ENDED
	else if (animationComponent->id == 1) { //1 is the loading screen
		
		destroy_world(world);
		
		create_intro(world);
	}
}


