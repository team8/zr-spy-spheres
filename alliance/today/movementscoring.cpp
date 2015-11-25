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
	float posOpp[3];

	if(game.getOtherEnergy() == 0 && game.posInGrey(possOpp)){
		//If they have no energy and they are in the grey zone, we shouldn't be moving
		return 100.0f;
		//This should take precedence if possible
	}
	return 0.0f;
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

float updateScore(){
	scores_move[0] = scoreStay();
	scores_move[1] = scoreMoveUp();
	scores_move[2] = scoreMoveDarkFollow();
	scores_move[3] = scoreMoveLightFollow();
	scores_move[4] = scoreMoveItemUp();
	scores_move[5] = scoreMoveItemDown();
	int  max;
	for (int i = 0; i < 6; i++) if (max > scores_move[i]) max = i;
	return max;
}

void update(){
	move = (Movement) updateScore();
	switch (move){
		case STAY:
			stay();
			break;
		case MOVE_UP:
			moveUp();
			break;
		case MOVE_FOLLOW_DARK:
			moveToDark();
			break;
		case MOVE_FOLLOW_LIGHT:
			moveToLight();
			break;
		case MOVE_NEAREST_ITEM_UP:
			moveToTopItem();
			break;
		case MOVE_NEAREST_ITEM_DOWN:
			moveToBottomItem();
			break;
		break;
	};
}











