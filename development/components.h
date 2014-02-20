
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
