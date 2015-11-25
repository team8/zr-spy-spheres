#define FLOAT_SIZE 4

enum MoveState{
    MIRROR = 0, //Move to Mirror State
    SCORE_UPPER = 1, 
    SCORE_LOWER = 2,
    TOP_AREA_LAZY = 3,
    TOP_AREA_DARK_LEAD = 4,
    TOP_AREA_DARK_TRAIL = 5,
    TOP_AREA_LIGHT = 6,
    BLANK
} move_state_;

enum PictureState {
	NONE = 0,
	FACE = 1,
	PICTURE = 2,
	UPLOAD = 3,
	BLANK
}

void init(){
	initData();
    //Initialize the values
	state = BLANK;
    //Init the state
    picLoop();
}