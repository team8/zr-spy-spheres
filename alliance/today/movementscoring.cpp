float scores_move[5];
//In the order of the enum Movements
enum Movements{
	STAY = 0,
	MOVE_UP = 1,
	MOVE_FOLLOW_DARK = 2,
	MOVE_FOLLOW_LIGHT = 3,
	MOVE_NEAREST_ITEM_UP = 4,
	MOVE_NEAREST_ITEM_DOWN = 5
};

Movements move = STAY;

float scoreStay(){

}

float scoreMoveUp(){

}

float scoreMoveDarkFollow(){

}

float scoreMoveLightFollow(){

}

float scoreMoveItemUp(){

}

float scoreMoveItemDown(){

}

void updateScore(){
	scores_move[0] = scoreStay();
	scores_move[1] = scoreMoveUp();
	scores_move[2] = scoreMoveDarkFollow();
	scores_move[3] = scoreMoveLightFollow();
	scores_move[4] = scoreMoveItemUp();
	scores_move[5] = scoreMoveItemDown();
}












