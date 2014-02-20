#ifndef COMPONENT_H
#define COMPONENT_H

typedef enum {
	COMPONENT_NONE = 0,
	COMPONENT_MOUSE = 1 << 0,
	COMPONENT_TEXTFIELD = 1 << 1,
	COMPONENT_BUTTON = 1 << 2,
	COMPONENT_COMMAND = 1 << 3,
	COMPONENT_POSITION = 1 << 4
} Components;

typedef struct {
	
	float xPos, yPos;
	float width, height;
	
} PositionComponent;

typedef struct  {
	int xPos;
	int yPos;
	bool rightClick;
	bool leftClick;
	
} MouseComponent;

typedef struct  {
	
	char *text;
	bool focused;
	int length;
	bool number;
	
} TextFieldComponent;

//
typedef struct  {
	
	char *label;
	bool currentState;
	bool prevState;
	
} ButtonComponent;

//list of commands
//TODO: Add more commands
#define C_UP 		0
#define C_LEFT 		1
#define C_RIGHT 	2
#define C_DOWN 		3
#define C_ACTION 	4

typedef struct  {
	
	bool commands[5];
	
} CommandComponent;


#endif
