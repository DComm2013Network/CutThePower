#include <string.h>
#include <stdio.h>

#include "triggered.h"
#include "systems.h"
#include "world.h"
#include "components.h"
#include "sound.h"
#include "Input/menu.h"
#include "Graphics/text.h"
#include "Network/Packets.h"
#include "Graphics/map.h"

#define DEBUG_SKINS     1 //1 = on, 0 = off
#define ALT_SKIN_CHANCE 3 //chance to roll an alternate skin

extern bool running;
extern SDL_Surface *map_surface;
extern unsigned int player_entity;
extern int send_router_fd[];
extern int rcv_router_fd[];
extern int game_net_signalfd;
static int character;
static char username[MAX_NAME];
static char serverip[MAXIP];
extern FowComponent *fow;
extern SDL_Window *window;
static unsigned int altSong = 0;
extern FowComponent *fow;


/**
 * Processes button presses from the main menu.
 * 
 * @param[in,out] world    A pointer to the world structure.
 * @param[in] entity       The current button entity.
 *
 * @designer Jordan Marling
 * @author   Jordan Marling, Joshua Campbell, Ian Davidson
 */
bool menu_click(World *world, unsigned int entity) {
	//printf("Clicked: %s\n", world->button[entity].label);
	unsigned int alternateSkin = 0;
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
		render_text(world, entity, "SOUND ON", MENU_FONT);
		
		world->button[entity].label = (char*)realloc(world->button[entity].label, sizeof(char) * strlen("options_sound_on"));
		strcpy(world->button[entity].label, "options_sound_on");
		
		enable_sound(true);

	}
	else if (strcmp(world->button[entity].label, "options_sound_on") == 0) {
		
		world->position[entity].x = (WIDTH / 2);
		render_text(world, entity, "SOUND OFF", MENU_FONT);
		
		world->button[entity].label = (char*)realloc(world->button[entity].label, sizeof(char) * strlen("options_sound_off"));
		strcpy(world->button[entity].label, "options_sound_off");
		
		enable_sound(false);

	}
	else if (strcmp(world->button[entity].label, "options_keymap") == 0) {

		destroy_menu(world);

		create_keymap_menu(world);

	}
	else if (strcmp(world->button[entity].label, "options_fullscreen_off") == 0) {

		world->position[entity].x = (WIDTH / 2);
		render_text(world, entity, "FULLSCREEN ON", MENU_FONT);
		
		world->button[entity].label = (char*)realloc(world->button[entity].label, sizeof(char) * strlen("options_fullscreen_on"));
		strcpy(world->button[entity].label, "options_fullscreen_on");
		
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		
	}
	else if (strcmp(world->button[entity].label, "options_fullscreen_on") == 0) {

		world->position[entity].x = (WIDTH / 2);
		render_text(world, entity, "FULLSCREEN OFF", MENU_FONT);
		
		world->button[entity].label = (char*)realloc(world->button[entity].label, sizeof(char) * strlen("options_fullscreen_off"));
		strcpy(world->button[entity].label, "options_fullscreen_off");
		
		SDL_SetWindowFullscreen(window, 0);
		
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
		character = ABHISHEK;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			int subroll = 0;
			subroll = rand() % 5;
			switch(subroll) {
				case 0:
					character = ABHISHEK_ALT1;
					stop_music();
					altSong = load_music("assets/Sound/players/abhishek_ranger/rangerTrack.wav");
					if (altSong != 0) {
						play_music(altSong);
					}
				break;
				case 1:
					character = ABHISHEK_ALT2;
					stop_music();
					altSong = load_music("assets/Sound/players/abhishek_ranger/rangerTrack.wav");
					if (altSong != 0) {
						play_music(altSong);
					}
				break;
				case 2:
					character = ABHISHEK_ALT3;
					stop_music();
					altSong = load_music("assets/Sound/players/abhishek_ranger/rangerTrack.wav");
					if (altSong != 0) {
						play_music(altSong);
					}
				break;
				case 3:
					character = ABHISHEK_ALT4;
					stop_music();
					altSong = load_music("assets/Sound/players/abhishek_ranger/rangerTrack.wav");
					if (altSong != 0) {
						play_music(altSong);
					}
				break;
				case 4:
					character = ABHISHEK_ALT5;
					stop_music();
					altSong = load_music("assets/Sound/players/abhishek_ranger/rangerTrack.wav");
					if (altSong != 0) {
						play_music(altSong);
					}
				break;
			}
			disable_background_sound(world);
		}
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_aman") == 0) {
		
		destroy_menu(world);
		character = AMAN;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = AMAN_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/aman_vacation/beachTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_andrew") == 0) {
		
		destroy_menu(world);
		character = ANDREW;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = ANDREW_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/andrew_terminator/terminatorTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_chris") == 0) {
		
		destroy_menu(world);
		character = CHRIS;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = CHRIS_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/chris_niko/nikoTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_clark") == 0) {
		
		destroy_menu(world);
		character = CLARK;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = CLARK_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/clark_halo/haloTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_cory") == 0) {
		
		destroy_menu(world);
		character = CORY;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = CORY_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/cory_megaman/megamanTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_damien") == 0) {
		
		destroy_menu(world);
		character = DAMIEN;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = DAMIEN_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/damien_ninja/ninjaTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_german") == 0) {
		
		destroy_menu(world);
		character = GERMAN;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = GERMAN_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/german_fisherman/fishermanTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_ian") == 0) {
		
		destroy_menu(world);
		character = IAN;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = IAN_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/ian_dovakiin/dovakiinTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_jordan") == 0) {
		
		destroy_menu(world);
		character = JORDAN;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = JORDAN_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/jordan_bling/blingTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_josh") == 0) {
		
		destroy_menu(world);
		character = JOSH;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = JOSH_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/josh_link/linkTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_konst") == 0) {
		
		destroy_menu(world);
		character = KONST;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = KONST_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/konst_box/boxTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_mat") == 0) {
		
		destroy_menu(world);
		character = MAT;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			int subroll = 0;
			subroll = rand() % 2;
			switch(subroll) {
				case 0:
					character = MAT_ALT1;
					stop_music();
					altSong = load_music("assets/Sound/players/mat_turtle/turtleTrack.wav");
					if (altSong != 0) {
						play_music(altSong);
					}
				break;
				case 1:
					character = MAT_ALT2;
					stop_music();
					altSong = load_music("assets/Sound/players/mat_stache/moustacheTrack.wav");
					if (altSong != 0) {
						play_music(altSong);
					}
				break;
				default:
					character = MAT_ALT1;
					stop_music();
					altSong = load_music("assets/Sound/players/mat_turtle/turtleTrack.wav");
					if (altSong != 0) {
						play_music(altSong);
					}
				break;
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_ramzi") == 0) {
		destroy_menu(world);
		character = RAMZI;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = RAMZI_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/ramzi_fish/underwaterTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_robin") == 0) {
		
		destroy_menu(world);
		character = ROBIN;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = ROBIN_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/robin_robin/robinTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_sam") == 0) {
		
		destroy_menu(world);
		character = SAM;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = SAM_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/sam_glitch/glitchTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_shane") == 0) {
		
		destroy_menu(world);
		character = SHANE;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = SHANE_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/shane_pirate/pirateTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_tim") == 0) {
		
		destroy_menu(world);
		character = TIM;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = TIM_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/tim_yoshi/yoshiTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_vincent") == 0) {
		
		destroy_menu(world);
		character = VINCENT;
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = VINCENT_ALT1;
			stop_music();
			altSong = load_music("assets/Sound/players/vincent_wizard/wizardTrack.wav");
			if (altSong != 0) {
				play_music(altSong);
			}
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_albert") == 0) {
		
		destroy_menu(world);
		character = ALBERT;
		stop_music();
		altSong = load_music("assets/Sound/players/albert_wei/albertTrack.wav");
		if (altSong != 0) {
			play_music(altSong);
		}
		alternateSkin = rand() % (ALT_SKIN_CHANCE + 1);
		#if DEBUG_SKINS
		printf("Roll: %u\n", alternateSkin);
		#endif
		create_setup_menu(world);
		if (alternateSkin == ALT_SKIN_CHANCE) {
			character = ALBERT_ALT1;
			
		}
		disable_background_sound(world);
		
	}
	else if (strcmp(world->button[entity].label, "menu_select_random") == 0) {
		
		altSong = 0;
		destroy_menu(world);
		character = rand() % 45;
		create_setup_menu(world);
		switch(character) {
			case JOSH_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/josh_link/LoZ_MainThemeShort.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case IAN_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/josh_dovakiin/Dovak-Ian.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case AMAN_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/aman_vacation/beachTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case ANDREW_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/andrew_terminator/terminatorTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case CORY_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/cory_megaman/megamanTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case DAMIEN_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/damien_ninja/ninjaTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case JORDAN_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/jordan_bling/blingTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case MAT_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/mat_turtle/turtleTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case RAMZI_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/ramzi_fish/underwaterTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case SAM_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/sam_glitch/glitchTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case TIM_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/tim_yoshi/yoshiTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case CHRIS_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/chris_niko/nikoTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case SHANE_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/shane_pirate/pirateTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case ROBIN_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/robin_robin/robinTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case VINCENT_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/vincent_wizard/wizardTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case KONST_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/konst_box/boxTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case ABHISHEK_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/abhishek_ranger/rangerTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case GERMAN_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/german_fisherman/fishermanTrack.wav");
				if (altSong != 0) { 
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case ALBERT:
				stop_music();
				altSong = load_music("assets/Sound/players/albert_wei/albertTrack.wav");
				if (altSong != 0) {
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case ALBERT_ALT1:
				stop_music();
				altSong = load_music("assets/Sound/players/albert_wei/albertTrack.wav");
				if (altSong != 0) {
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case MAT_ALT2:
				character = MAT_ALT2;
				stop_music();
				altSong = load_music("assets/Sound/players/mat_stache/moustacheTrack.wav");
				if (altSong != 0) {
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case ABHISHEK_ALT2:
				character = ABHISHEK_ALT2;
				stop_music();
				altSong = load_music("assets/Sound/players/abhishek_ranger/rangerTrack.wav");
				if (altSong != 0) {
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case ABHISHEK_ALT3:
				character = ABHISHEK_ALT2;
				stop_music();
				altSong = load_music("assets/Sound/players/abhishek_ranger/rangerTrack.wav");
				if (altSong != 0) {
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case ABHISHEK_ALT4:
				character = ABHISHEK_ALT2;
				stop_music();
				altSong = load_music("assets/Sound/players/abhishek_ranger/rangerTrack.wav");
				if (altSong != 0) {
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case ABHISHEK_ALT5:
				character = ABHISHEK_ALT2;
				stop_music();
				altSong = load_music("assets/Sound/players/abhishek_ranger/rangerTrack.wav");
				if (altSong != 0) {
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
			case CLARK_ALT1:
				character = CLARK_ALT1;
				stop_music();
				altSong = load_music("assets/Sound/players/clark_halo/haloTrack.wav");
				if (altSong != 0) {
					play_music(altSong);
				}
				disable_background_sound(world);
			break;
		}

	}

	//CREDITS
	else if (strcmp(world->button[entity].label, "credits_back") == 0) {

		destroy_menu(world);

		create_main_menu(world);

	}

	//SETUP
	else if (strcmp(world->button[entity].label, "setup_back") == 0) {
		
		if (altSong != 0) {
			stop_music();
			cleanup_music(altSong);
			altSong = 0;
		}
		
		destroy_menu(world);

		create_main_menu(world);

	}
	else if (strcmp(world->button[entity].label, "setup_play") == 0) {

		unsigned int i;

		stop_music();
		
		if (altSong != 0) {
			cleanup_music(altSong);
			altSong = 0;
		}
		stop_all_effects();

		for(i = 0; i < MAX_ENTITIES; i++) {

			if (IN_THIS_COMPONENT(world->mask[i], COMPONENT_TEXTFIELD)) {

				if (strcmp(world->text[i].name, "setup_username") == 0) {
					memcpy(username, world->text[i].text, MAX_NAME);
				}
				else if (strcmp(world->text[i].name, "setup_serverip") == 0) {
					memcpy(serverip, world->text[i].text, MAXIP);
				}

			}
		}

		//printf("Username: %s\n", username);
		//printf("Server IP: %s\n", serverip);
		
		#if DISPLAY_CUTSCENES 
		
		destroy_world(world);
		stop_music();
		create_van_intro(world, character);
		
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
	
	//IN GAME PAUSE MENU
	else if (strcmp(world->button[entity].label, "ingame_sound_off") == 0) {
		
		world->position[entity].x = (WIDTH / 2);
		render_text(world, entity, "SOUND ON", MENU_FONT);
		
		world->button[entity].label = (char*)realloc(world->button[entity].label, sizeof(char) * strlen("ingame_sound_on"));
		strcpy(world->button[entity].label, "ingame_sound_on");
		
		enable_sound(true);
	}
	else if (strcmp(world->button[entity].label, "ingame_sound_on") == 0) {
		
		world->position[entity].x = (WIDTH / 2);
		render_text(world, entity, "SOUND OFF", MENU_FONT);
		
		world->button[entity].label = (char*)realloc(world->button[entity].label, sizeof(char) * strlen("ingame_sound_off"));
		strcpy(world->button[entity].label, "ingame_sound_off");
		
		enable_sound(false);

	}
	else if (strcmp(world->button[entity].label, "ingame_fullscreen_off") == 0) {

		world->position[entity].x = (WIDTH / 2);
		render_text(world, entity, "FULLSCREEN ON", MENU_FONT);
		
		world->button[entity].label = (char*)realloc(world->button[entity].label, sizeof(char) * strlen("ingame_fullscreen_on"));
		strcpy(world->button[entity].label, "ingame_fullscreen_on");
		
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		
	}
	else if (strcmp(world->button[entity].label, "ingame_fullscreen_on") == 0) {

		world->position[entity].x = (WIDTH / 2);
		render_text(world, entity, "FULLSCREEN OFF", MENU_FONT);
		
		world->button[entity].label = (char*)realloc(world->button[entity].label, sizeof(char) * strlen("ingame_fullscreen_off"));
		strcpy(world->button[entity].label, "ingame_fullscreen_off");
		
		SDL_SetWindowFullscreen(window, 0);
		
	}
	else if (strcmp(world->button[entity].label, "ingame_back") == 0) {
		
		destroy_menu(world);
		
		world->mask[player_entity] ^= COMPONENT_COMMAND;
		
	}
	else if (strcmp(world->button[entity].label, "ingame_exit") == 0) {
		
		uint32_t err = NUM_PACKETS + 1;
        write_pipe(send_router_fd[WRITE], &err, sizeof(err));
        reset_fog_of_war(fow);
		destroy_world(world);
		player_entity = MAX_ENTITIES;
		map_surface = 0;
		cleanup_map();
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
		stop_all_effects();
		
		PKT_GAME_STATUS pkt;
		memcpy(pkt.otherPlayers_name[0], username, MAX_NAME);
		pkt.characters[0] = character;
		pkt.readystatus[0] = 0;
		pkt.otherPlayers_teams[0] = 0;

		map_init(world, "assets/Graphics/map/map_00/map00.txt", "assets/Graphics/map/map_00/tiles.txt");
		player_entity = create_player(world, 620, 420, true, COLLISION_HACKER, 0, &pkt);
		setup_character_animation(world, character, player_entity);
		////NETWORK CODE
		game_net_signalfd 	= eventfd(0, EFD_SEMAPHORE);



		init_client_network(send_router_fd, rcv_router_fd, serverip);
		init_client_update(world);
		send_intialization(world, send_router_fd[WRITE], username);
		create_objective(world, 620, 380, 40, 40, 0, 0);
	}
	//LOADING SCREEN ENDED
	else if (animationComponent->id == 1) { //1 is the loading screen

		destroy_world(world);

		create_intro(world);
	}
	else if (animationComponent->id == 2) { //2 is the logo loading screen
		
		destroy_world(world);
		
		create_main_menu(world);
	}
	else if (animationComponent->id == ANIMATION_FADE_TO_BLACK) {
		
		destroy_world(world);
		
		create_load_screen(world);
		
		//world->animation[entity].id = CUTSCENE_GAME_INTRO;
		//animation_end(world, entity);
		
	}
}

void cutscene_end(World *world, unsigned int entity) {
	
	CutsceneComponent *cutscene = &world->cutscene[entity];
	
	if (cutscene->id == CUTSCENE_GAME_INTRO) {
		
		destroy_world(world);

		stop_music();
		stop_all_effects();
		
		PKT_GAME_STATUS pkt;
		memcpy(pkt.otherPlayers_name[0], username, MAX_NAME);
		pkt.characters[0] = character;
		pkt.readystatus[0] = 0;
		pkt.otherPlayers_teams[0] = 0;

		map_init(world, "assets/Graphics/map/map_00/map00.txt", "assets/Graphics/map/map_00/tiles.txt");
		player_entity = create_player(world, 620, 420, true, COLLISION_HACKER, 0, &pkt);
		setup_character_animation(world, character, player_entity);
		////NETWORK CODE
		game_net_signalfd 	= eventfd(0, EFD_SEMAPHORE);



		init_client_network(send_router_fd, rcv_router_fd, serverip);
		init_client_update(world);
		send_intialization(world, send_router_fd[WRITE], username);
		
	}
	else if (cutscene->id == CUTSCENE_VAN_INTRO) {
		
		unsigned int e = create_entity(world, COMPONENT_RENDER_PLAYER | COMPONENT_POSITION | COMPONENT_ANIMATION);
		
		world->position[e].x = 0;
		world->position[e].y = 0;
		
		world->position[e].width = WIDTH;
		world->position[e].height = HEIGHT;
		
		world->renderPlayer[e].width = WIDTH;
		world->renderPlayer[e].height = HEIGHT;
		
		load_animation("assets/Graphics/cutscene/fade_to_black/animation.txt", world, e);
		play_animation(world, e, "fade");
		
		world->animation[e].id = ANIMATION_FADE_TO_BLACK;
	}
	
}
