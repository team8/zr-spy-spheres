//Begin page main
#define FLOAT_SIZE 4

#define FULL_FORCE_MOTION_LIMIT_MIRROR 1.0f
#define FULL_FORCE_MOTION_LIMIT_UPPER 0.85f
#define FULL_FORCE_MOTION_LIMIT_LOWER 0.85f
#define EXTRA_PUSH_LEAD_DISTANCE 0.1f
#define EXTRA_PUSH_TRAIL_DISTANCE 0.6f
#define EXTRA_PUSH_FUEL_TIME_RATIO_LIMIT 0.35f

#define TOP_TARGET_X 0.3f
#define TOP_TARGET_Z -0.4f

#define LIGHT_STATE_LEADING_EDGE_SWITCH -0.4f
#define LIGHT_STATE_LEADING_EDGE_TRAIL_DISTANCE 0.25f
#define LIGHT_STATE_TRAILING_EDGE_LEAD_DISTANCE 0.25f

#define DARK_STATE_LEADING_EDGE_SWITCH -0.6f
#define DARK_STATE_LEADING_EDGE_LEAD_DISTANCE 0.2f
#define DARK_STATE_TRAILING_EDGE_TRAIL_DISTANCE 0.125f

#define MIRROR_SCORE 1000.0f
#define UPPER_SCORE 500.0f
#define LOWER_SCORE 750.0f

#define DARK_DELTA_SCORE_SCALE 2.0f
#define DARK_BONUS 5.0f

#define LIGHT_LOW_ENERGY_SCALE 36.0f
#define LIGHT_LOW_ENERGY_BOUNDER 1.0f
#define LIGHT_MIRROR_PERCENT_LEFT_SCALE 18.0f
#define LIGHT_UPLOAD_BONUS 100.0f

#define PICTURE_DELTA_SCORE_WEIGHT 5.0f
#define PICTURE_DELTA_ENERGY_WEIGHT 7.0f
#define PICTURE_TIME_WEIGHT 1.0f
#define MIN_PIC_ENERGY 1.5f

#define UPLOAD_ENERGY_SCALE 45.0f
#define UPLOAD_BONUS 50.0f

enum MoveState {
    MIRROR,
    SCORE_UPPER, 
    SCORE_LOWER,
    TOP_AREA_DARK,
    TOP_AREA_LIGHT,
    MOVE_BLANK
} move_state_;

enum PictureState {
	FACE_OTHER,
	PICTURE,
    FACE_EARTH,
	UPLOAD,
	PICTURE_BLANK
} picture_state_;

void init() {
	initData();
	move_state_ = MOVE_BLANK;
    picture_state_ = FACE_OTHER;
}

void loop() {
    updateData();
    score();
    act();
}

void score() {
    float move_scores[5];
    move_scores[0] = mirrorScore();
    move_scores[1] = scoreUpperScore();
    move_scores[2] = scoreLowerScore();
    move_scores[3] = topAreaDarkScore();
    move_scores[4] = topAreaLightScore();
    for(int i = 0; i < sizeof(move_scores) / FLOAT_SIZE; i++) {
        if (move_scores[i] >= move_scores[move_state_]){
            move_state_ = (MoveState) i;
        }
    }
}

void act() {
    DEBUG(("Move State: %d", move_state_));
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
        case TOP_AREA_DARK:
            topAreaDarkAction();
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
        case FACE_OTHER:
            faceOtherAction();
            break;
        case FACE_EARTH:
            faceEarthAction();
            break;
        case PICTURE_BLANK:
           DEBUG(("Blank picture_state_"));
            break;
        default:
            DEBUG(("Invalid picture_state_ with index: %d", picture_state_));
    }
}

int mirrorIndex;
int upperPointIndex;
int lowerPointIndex;
float myPos[3];
float otherPos[3];
float myState[12];
float otherState[12];
float vecBetween[3];
float earth[3];
float fuel;
float time;
float timeLeft;
float memoryUsed;
float memoryLeft;
float energy;
bool cameraOn;
float myScore;
float otherScore;
float timeToSwitch;
float leadingEdge;
float trailingEdge;
int mirrorsHeld;
bool isRed;
float data[6];
float uploadDir[3];
float itemLoc[9][3];
float mirror[3];
float upperPoint[3];
float lowerPoint[3];
float topTarget[3];
bool facingOther;
bool rotSpeedGoodForUpload;


void setItemLocations() {
    int redShift = 0;
    if(isRed) {
        redShift = 1;
    }
    mirrorIndex = 7 + redShift;
    game.getItemLoc(mirror, mirrorIndex);
    
    float point1[3];
    float point2[3];
    
    game.getItemLoc(point1, 3);
    game.getItemLoc(point2, 5);
    
    if(point1[2] < point2[2]) {
        upperPointIndex = 3 + redShift;
        lowerPointIndex = 5 + redShift;
    }
    else {
        upperPointIndex = 5 + redShift;
        lowerPointIndex = 3 + redShift;
    }
    game.getItemLoc(upperPoint, upperPointIndex);
    game.getItemLoc(lowerPoint, lowerPointIndex);
}

void initData() {
	api.getMyZRState(myState);
	isRed = isSphereRed();
	topTarget[0] = TOP_TARGET_X;
	if(isRed) {
	    topTarget[0] = -TOP_TARGET_X;
	}
	topTarget[1] = 0.0f;
	topTarget[2] = TOP_TARGET_Z;
	setItemLocations();
	energy = 5;
	cameraOn = true;
	myScore = 0;
	mirrorsHeld = 0;
	trailingEdge = -0.2f;
    leadingEdge = 0.6f;
    earth[0] = 0.0f;
    earth[1] = 0.0f;
    earth[2] = 1.0f;
    facingOther = game.isFacingOther();
}

void updateData() {
	api.getMyZRState(myState);
	api.getOtherZRState(otherState);
	myPos[0] = myState[0];
	myPos[1] = myState[1];
	myPos[2] = myState[2];
	otherPos[0] = otherState[0];
	otherPos[1] = otherState[1];
	otherPos[2] = otherState[2];
	energy = game.getEnergy();
	fuel = game.getFuelRemaining();
	cameraOn = game.isCameraOn();
	mirrorsHeld = game.getNumMirrorsHeld();
    leadingEdge += 0.025f; 
    if (leadingEdge >= 0.79f) leadingEdge = -0.8f;
    trailingEdge += 0.025f;
    if (trailingEdge >= 0.79f) trailingEdge = -0.8f;
    time = game.getCurrentTime();
    timeLeft = 180.0f - time;
	memoryUsed = game.getMemoryFilled();
	memoryLeft = 2 - memoryUsed;
	myScore = game.getScore();
	otherScore = game.getOtherScore();
	mathVecSubtract(vecBetween, otherPos, myPos, 3);
	facingOther = game.isFacingOther();
}

bool isSphereRed() {
	if(myState[0] < 0.0f) {
		return true;
	}
	return false;
}

float mirrorScore() {
    if(game.hasItem(mirrorIndex) == 0) {
        game.useMirror();
    }
    if(game.hasItem(mirrorIndex) == -1) {
        return MIRROR_SCORE;
    }
    return -1.0f;
}

float scoreUpperScore() {
  	if(game.hasItem(upperPointIndex) == -1) {
  	    return UPPER_SCORE;
  	}
 	return -1.0f;
}

float scoreLowerScore() {
 	 if(game.hasItem(lowerPointIndex) == -1) {
  	    return LOWER_SCORE;
  	}
 	return -1.0f;
}

float topAreaDarkScore() {
	return DARK_DELTA_SCORE_SCALE * (myScore - otherScore) + DARK_BONUS;
}

float topAreaLightScore() {
	return LIGHT_LOW_ENERGY_SCALE / (LIGHT_LOW_ENERGY_BOUNDER + energy * energy) 
	+ LIGHT_MIRROR_PERCENT_LEFT_SCALE * game.getMirrorTimeRemaining() / 24;
}

void mirrorAction() {
    moveToTarget(mirror, FULL_FORCE_MOTION_LIMIT_MIRROR);
}

void scoreUpperAction() {
    moveToTarget(upperPoint, FULL_FORCE_MOTION_LIMIT_UPPER);
}

void scoreLowerAction() {
    moveToTarget(lowerPoint, FULL_FORCE_MOTION_LIMIT_LOWER);
}

void topAreaDarkAction() {
    bool leading = false;
    DEBUG(("Leading Edge: %f | Trailing Edge: %f", leadingEdge, trailingEdge));
    if(leadingEdge < DARK_STATE_LEADING_EDGE_SWITCH) {
        topTarget[1] = leadingEdge + DARK_STATE_LEADING_EDGE_LEAD_DISTANCE;
        DEBUG(("Dark State: Leading with: %f", topTarget[1]));
        leading = true;
    }
        
    else {
        topTarget[1] = trailingEdge - DARK_STATE_TRAILING_EDGE_TRAIL_DISTANCE;
        DEBUG(("Dark State: Trailing with: %f", topTarget[1]));
    }
    if(topTarget[1] >= .79f) {
        topTarget[1] -= 1.6f;
    }
    moveToTargetCatch(topTarget, leading);
}

void topAreaLightAction() {
     bool leading = false;
    DEBUG(("Leading Edge: %f | Trailing Edge: %f", leadingEdge, trailingEdge));
    if(leadingEdge < LIGHT_STATE_LEADING_EDGE_SWITCH) {
        topTarget[1] = leadingEdge - LIGHT_STATE_LEADING_EDGE_TRAIL_DISTANCE;
        DEBUG(("Light State: Trailing with: %f", topTarget[1]));
    }
        
    else {
        topTarget[1] = trailingEdge + LIGHT_STATE_TRAILING_EDGE_LEAD_DISTANCE;
        DEBUG(("Light State: Leading with: %f", topTarget[1]));
        leading = true;
    }
    if(topTarget[1] >= .79f) {
        topTarget[1] -= 1.6f;
    }
    moveToTargetCatch(topTarget, leading);
}

void faceEarthAction() {
   float stopped[3];
    stopped[0] = 0.0f;
    stopped[1] = 0.0f;
    stopped[2] = 0.0f;
    float squareRot = myState[9] * myState[9] + myState[10] * myState[10] + myState[11] * myState[11];
    float rotSpeedGoodForUpload = squareRot < 0.0025;
    if(acosf(myState[8]) > 0.25f && !rotSpeedGoodForUpload) {
        DEBUG(("SETTING ATTITUDE TO EARTH"));
        api.setAttitudeTarget(earth);
    }
    else {
        if(acosf(myState[8]) < 0.25f && rotSpeedGoodForUpload && UPLOAD_ENERGY_SCALE * (energy - 1) - timeLeft > -UPLOAD_BONUS) {
            DEBUG(("UPLOAD CALLED."));
            game.uploadPics();
        }
    }
    if(memoryUsed == 0) {
         picture_state_ = FACE_OTHER;
    }
}

void faceOtherAction() {
    if(!facingOther && (move_state_ == TOP_AREA_DARK || move_state_ == TOP_AREA_LIGHT)) {
        api.setAttitudeTarget(vecBetween);
    }
    else if(!game.posInDark(otherPos) && facingOther && (!checkMirror() && (PICTURE_DELTA_ENERGY_WEIGHT*(energy - MIN_PIC_ENERGY) + PICTURE_TIME_WEIGHT*(180 - timeLeft) + PICTURE_DELTA_SCORE_WEIGHT*(otherScore - myScore)) > 0)) {
        game.takePic();
    }
    if(memoryUsed == 2) {
         picture_state_ = FACE_EARTH;
    }
}

bool checkMirror(){
	if(game.hasItem(7) == 1 || game.hasItem(8) == 1){
	    return game.getPicPoints()>.01;
	}
	return false;
}

void moveToTarget(float target[3], float limit) {
    float vecToTarg[3];
    mathVecSubtract(vecToTarg, target, myPos, 3);
    
    float distance = mathVecMagnitude(vecToTarg, 3);
    if(distance > limit) {
        DEBUG(("Setting forces"));
        api.setForces(vecToTarg);
    } else {
        api.setPositionTarget(target);
    }
}

void moveToTargetCatch(float target[3], bool leading) {
    float vecToTarg[3];
    mathVecSubtract(vecToTarg, target, myPos, 3);
    
    bool fuelTimeRatioValid = fuel / timeLeft > EXTRA_PUSH_FUEL_TIME_RATIO_LIMIT;
    if(fuelTimeRatioValid && leading && myPos[1] < (target[1] + EXTRA_PUSH_LEAD_DISTANCE)) {
        DEBUG(("Extra leading push"));
        api.setForces(vecToTarg);
    } 
    else if(fuelTimeRatioValid && !leading && myPos[1] < (target[1] + EXTRA_PUSH_TRAIL_DISTANCE)) {
        DEBUG(("Extra trailing push"));
        api.setForces(vecToTarg);
    } else {
        api.setPositionTarget(target);
    }
}


//End page main
