//Begin page main
#define FLOAT_SIZE 4

enum MoveState {
    MIRROR,
    SCORE_UPPER, 
    SCORE_LOWER,
    TOP_AREA_LAZY,
    TOP_AREA_DARK_LEAD,
    TOP_AREA_DARK_TRAIL,
    TOP_AREA_LIGHT,
    MOVE_BLANK
} move_state_;


enum PictureState {
	NONE,
	FACE_OTHER,
	PICTURE,
    FACE_EARTH,
	UPLOAD,
	PICTURE_BLANK
} picture_state_;

void init() {
	initData();
	move_state_ = MOVE_BLANK;
    picture_state_ = PICTURE_BLANK;
}

void loop() {
    updateData();
    score();
    act();
}

void score() {
    
    float move_scores[7];
    move_scores[0] = mirrorScore();
    move_scores[1] = scoreUpperScore();
    move_scores[2] = scoreLowerScore();
    move_scores[3] = topAreaLazyScore();
    move_scores[4] = topAreaDarkLeadScore();
    move_scores[5] = topAreaDarkTrailScore();
    move_scores[6] = topAreaLightScore();
    for(int i = 0; i < sizeof(move_scores) / FLOAT_SIZE; i++) {
        if (move_scores[i] >= move_scores[move_state_]){
            move_state_ = (MoveState) i;
        }
    }

    float picture_scores[5];
    picture_scores[0] = 0;
    picture_scores[1] = faceOtherScore();
    picture_scores[2] = pictureScore();
    picture_scores[3] = faceEarthScore();
    picture_scores[4] = uploadScore();
    for(int i = 0; i < sizeof(picture_scores) / FLOAT_SIZE; i++) {
        if (picture_scores[i] >= picture_scores[picture_state_]){
            picture_state_ = (PictureState) i;
        }
    }
    
}

void act() {
    switch(move_state_) {
        case MIRROR:
            mirrorAction();  
            break;
        case SCORE_UPPER:
            scoreUpperAction();
            break;
        case SCORE_LOWER:
            scoreLowerAction();
            break;
        case TOP_AREA_LAZY:
            topAreaLazyAction();
            break;
        case TOP_AREA_DARK_LEAD:
            topAreaDarkLeadAction();
            break;
        case TOP_AREA_DARK_TRAIL:
            topAreaDarkTrailAction();
            break;
        case TOP_AREA_LIGHT:
            topAreaLightAction();
            break;
        case MOVE_BLANK:
            DEBUG(("Blank move_state_"));
            break;
        default:
            DEBUG(("Invalid move_state_ with index: %d", move_state_));
    }

    switch(picture_state_) {
        case NONE:
            breakPictureAction();  
            break;
        case FACE_OTHER:
            faceOtherAction();
            break;
        case PICTURE:
            pictureAction();
            break;
        case FACE_EARTH:
            faceEarthAction();
            break;
        case UPLOAD:
            uploadAction();
            break;
        case PICTURE_BLANK:
            DEBUG(("Blank picture_state_"));
            break;
        default:
            DEBUG(("Invalid picture_state_ with index: %d", picture_state_));
    }
}

float myState[12];
float theirState[12];
float fuel;
float timeLeft;
float memoryUsed;
float memoryLeft;
float energy;
bool cameraOn;
float myScore;
float timeToSwitch;
float leadingEdge;
float trailingEdge;
int mirrorsHeld;
bool isRed;
bool isFacingOther;
float data[6];
float uploadDir[3];
float itemLoc[9][3];
float mirror[3];
float point1[3];
float point2[3];


void setItemLocations() {
    /*
	float testMirror[3];
	game.getItemLoc(testMirror, 7);
	float testMirror1[3];
	game.getItemLoc(testMirror1, 8);
	float testItem1[3];
	game.getItemLoc(testItem1, 3);
	float testItem2[3];
	game.getItemLoc(testItem2, 4);
	float testItem3[3];
	game.getItemLoc(testItem3, 5);
	float testItem4[3];
	game.getItemLoc(testItem4, 6);
	if(isRed) {
		if(testMirror[0] <0){
			mirror[3] = testMirror;
		}
		if(testItem1[0] < 0) {
			point1= testItem1;
			point2= testItem3;
		} else if(testItem2[0] < 0) {
			point1= testItem2;
			point2= testItem4;
		}
	} else {
		if(testMirror[0] >0){
			mirror[3] = testMirror;
		}
		if(testItem1[0] > 0) {
			point1= testItem1;
			point2= testItem3;
		} else if(testItem2[0] > 0) {
			point1= testItem2;
			point2= testItem4;
		}
	}
	*/
}

void initData() {
	api.getMyZRState(myState);
	isRed = isSphereRed();
	fuel = 60;
	energy = 5;
	cameraOn = true;
	myScore = 0;
	mirrorsHeld = 0;
}

void updateData() {
	api.getMyZRState(myState);
	api.getOtherZRState(theirState);
	energy = game.getEnergy();
	fuel = game.getFuelRemaining();
	cameraOn = game.isCameraOn();
	mirrorsHeld = game.getNumMirrorsHeld();
	leadingEdge = getLeading();
	trailingEdge = getTrailing();
	timeLeft = 180 - game.getCurrentTime();
	memoryUsed = game.getMemoryFilled();
	memoryLeft = game.getMemorySize() - game.getMemoryFilled();
	isFacingOther = game.isFacingOther();
}

bool isSphereRed() {
	if(myState[0] < 0) {
		return true;
	}
	return false;
}

float getLeading() {
	float leading = 1.05 + game.getLightInterfacePosition();
	return leading;
}

float getTrailing() {
	float trailing = 0.05 + game.getLightInterfacePosition();
	return trailing;
}

float mirrorScore() {
	return 0.0f;
}

float scoreUpperScore() {
 	return 0.0f;
}

float scoreLowerScore() {
	return 0.0f;
}

float topAreaLazyScore() {
	return 0.0f;
}

float topAreaDarkLeadScore() {
	return 0.0f;
}

float topAreaDarkTrailScore() {
	return 0.0f;
}

float topAreaLightScore() {
	return 0.0f;
}

float pictureScore() {
	return 0.0f;
}

float uploadScore() {
    return 0.0f	;
}

float faceEarthScore() {
	return 0.0f;
}
float faceOtherScore() {
	return 0.0f;
}

void mirrorAction() {
    DEBUG(("Mirror Action"));
}

void scoreUpperAction() {
    DEBUG(("Score Upper Action"));
}

void scoreLowerAction() {
    DEBUG(("Score Lower Action"));
}

void topAreaLazyAction() {
    DEBUG(("Top Area Lazy Action"));
}

void topAreaDarkLeadAction() {
    DEBUG(("Top Area Dark Lead Action"));
}

void topAreaDarkTrailAction() {
    DEBUG(("Top Area Dark Trail Action"));
}
void topAreaLightAction() {
	DEBUG(("Top Area Light Action"));
}

void breakPictureAction() {
    DEBUG(("Break Picture Action"));
}

void pictureAction() { 
	DEBUG(("Take Picture Action"));
}

void uploadAction() {
	DEBUG(("Upload Picture Action"));
}

void faceEarthAction() {
	DEBUG(("Face Earth Action"));
}

void faceOtherAction() {
	DEBUG(("Face Other Action"));
}


//End page main
