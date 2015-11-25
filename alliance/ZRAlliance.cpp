//Begin page acitons
void mirrorAction() { 
    DEBUG(("mirror action called"));
}

void pointAction() {
    DEBUG(("point action called"));
}

void topAction() {
	//Top light
    DEBUG(("top action called"));
}

void darkAction(){
	//Top Dark
	DEBUG(("Top DARK"));
}
//End page acitons
//Begin page data
int time;
bool mirror_picked_up;
bool weAreRed;
float my_zrstate[12];
float other_zrstate[12];

void initData() {
    time = 0;
    mirror_picked_up = false;
    weAreRed = areWeRed();
}

bool areWeRed(){
    api.getMyZRState(my_zrstate);
    if (my_zrstate[0]>0) return false;
    //If X is pos, then starts as blue
    return true;
}

void updateData() {
    time++;
    api.getMyZRState(my_zrstate);
    api.getOtherZRState(other_zrstate);
    
    if(game.getNumMirrorsHeld() > 0) {
        mirror_picked_up = true;
    }
}
//End page data
//Begin page main
#define FLOAT_SIZE 4

enum State{
    MIRROR = 0,
    POINTS = 1,
    TOP_LIGHT = 2,
    TOP_DARK = 3,
    BLANK
} state;
/*
Mirror is go to mirror,
Points is go to points,
Top is go to top - 
Simple
*/

void init(){
	initData();
    //Initialize the values
	state = BLANK;
    //Init the state
}

void loop(){
    updateData();
    //Update zr state and stuff like that
    act();
    //Act on the enum
	score();
    //Update the score
}

void score() {
    float scores[4];
    scores[0] = mirrorScore();
    scores[1] = pointScore();
    scores[2] = topScore();
    scores[3] = topDarkScore();
    DEBUG(("%f", scores[3]));
    //Fill The Scores Array

    DEBUG(("Size of scores[0]: %d", sizeof(scores[0])));
    for(int i = 0; i < sizeof(scores) / FLOAT_SIZE; i++) {
        if (scores[i] >= scores[state]){
            state = (State) i;
        }
    }
}

void act() {
    switch(state){
        case MIRROR:
            mirrorAction();  
            break;
        case POINTS:
            pointAction();
            break;
        case TOP_LIGHT:
            topAction();
            break;
        case TOP_DARK:
            darkAction();
            break;
        case BLANK:
             DEBUG(("Blank state"));
             break;
        default:
            DEBUG(("No action with index: %d", state));
    }
}
//End page main
//Begin page scoring
#define scoreThreshold 75.0f

float mirrorScore() {
    float score = 0;
    if(mirror_picked_up) {
        score = 10000;
    }
    
    return score;
}

float pointScore() {
    float score;
    score = scoreThreshold;
    
    return score;
}

float topScore() {
    float score;
    score = 10.0f * (time - scoreThreshold);
    
    return score;   
}

bool topScoreYes(){
    if (10*(time-scoreThreshold) > scoreThreshold) return true;
    return false;
}

float topDarkScore(){
    float score;
    //score = 1;
    /*
    pseudo Code:
        If we have 3.5>x energy and they aren't in the dark, this score should be infinite
    */

    float oppPos[3];
    oppPos[0] = other_zrstate[0];
    oppPos[1] = other_zrstate[1];
    oppPos[2] = other_zrstate[2];
    DEBUG(("%d", game.getEnergy()>3.5f ));
    DEBUG(("%d",!game.posInDark(oppPos)));
    DEBUG(("%d",topScoreYes()));
    if (game.getEnergy()>3.5f && !game.posInDark(oppPos) && topScoreYes()){
        return -100000;
    }



    return 0;
}












//End page scoring
