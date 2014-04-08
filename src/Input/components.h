/** @ingroup Input */
/*@{*/

/**
 * @todo Add more commands.
 * @file components.h 
 */
/*@}*/
#ifndef INPUT_COMPONENT_H
#define INPUT_COMPONENT_H

#include <SDL2/SDL.h>
#include "../components.h"

/**
 * Contains the cursor's position and the state of mouse buttons.
 *
 * @struct MouseComponent
 */
typedef struct  {
	int x;           /**< The x-coordinate of the mouse (in pixels). */
	int y;           /**< The y-coordinate of the mouse (in pixels). */
	bool rightClick;    /**< Whether the right mouse button was clicked. */
	bool leftClick;     /**< Whether the left mouse button was clicked. */
	
} MouseComponent;

/**
 * Contains a text field's text, allowable text length, and whether the field has focus.
 *
 * @struct TextFieldComponent
 */
typedef struct  {
	
	char *name; 	/**< The name of the text field. */
	char *text;    /**< The text contained within this text field. */
	bool focused;  /**< Whether the text field has focus. */
	int length;    /**< The current length of the text. */
	int max_length;/**< The maximum length of the field. */
	bool number;   /**< Whether the max number of characters has been exceeded. */
	
} TextFieldComponent;

/**
 * Contains a button's text and current and previous states.
 *
 * @struct ButtonComponent
 */
typedef struct  {
	
	char *label;        /**< The text to go on the button. */
	bool currentState;  /**< Whether the button is pressed or released. */
	bool prevState;     /**< The button's previous state. */
	bool hovered;		/**<If the mouse is hovered over the button. */
	
} ButtonComponent;

//list of commands
#define NUM_COMMANDS	6 /**< Total number of commands that an entity can do */

#define C_UP 			0 /**< Commands an entity to move up */
#define C_DOWN 			1 /**< Commands an entity to move down */
#define C_LEFT 			2 /**< Commands an entity to move left */
#define C_RIGHT 		3 /**< Commands an entity to move right */
#define C_ACTION 		4 /**< Commands an entity to perform an action */
#define C_TILE			5 /**< Commands an entity to place a special tile */

/**
 * Contains the commands that are currently active for an entity.
 *
 * @struct CommandComponent
 */
typedef struct  {
	
	bool commands[NUM_COMMANDS]; /**< The commands that are currently on. */
	
} CommandComponent;


#endif
/*@}*/
