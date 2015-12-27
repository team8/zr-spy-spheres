//Begin page data
float myPos[3];
float otherPos[3];
float myState[12];
float otherState[12];
float vecBetween[3];

float lzVel[3];
float lzDownVel[3];
float backLzVel[3];
float backLzDownVel[3];
float stoppedVel[3];

int mirrorIndex;
int otherMirrorIndex;
int upperPointIndex;
int lowerPointIndex;
int energyPackIndex;

float mirror[3];
float energyPack[3];
float upperPoint[3];
float lowerPoint[3];
float earth[3];

float leadingEdge;
float trailingEdge;

bool isRed;
bool facingOther;

bool picLive;
bool picActionValid;
bool faceOtherRotateValid;
bool uploadLive;
bool uploadActionValid;
bool uploadRotateValid;

bool movedThroughLight;
//End page data
//Begin page main
#define FLOAT_SIZE 4

enum GameState {
    POINT_BLOCK,
    MIRROR,
    PICTURE,
    DARK_1,
    MOVE_THROUGH_LIGHT,
    ENERGY_PACK,
    DARK_2
} game_state_;

void init(){
    DEBUG(("ZiRconiuM - Paly - Eagles: F2"));
    api.getMyZRState(myState);
    api.getOtherZRState(otherState);
    
    isRed = false;
	if(myState[0] < 0.0f) {
		isRed =  true;
	}
	
    earth[0] = 0.0f;
    earth[1] = 0.0f;
    earth[2] = 1.0f;
    
	lzVel[0] = 0.0f;
	lzVel[1] = 0.04f;
	lzVel[2] = 0.0f;
	
	backLzVel[0] = 0.0f;
	backLzVel[1] = -0.025f;
	backLzVel[2] = 0.0f;
    
    stoppedVel[0] = 0.0f;
    stoppedVel[1] = 0.0f;
    stoppedVel[2] = 0.0f;
    
    trailingEdge = -0.2f;
    leadingEdge = 0.6f;
    
    picLive = true;
    uploadLive = false;
    movedThroughLight = false;
    picActionValid = true;
    uploadActionValid = false;
    faceOtherRotateValid = false;
    uploadRotateValid = true;
    
    int redShift = 0;
    if(isRed) {
        redShift = 1;
    }
    mirrorIndex = 7 + redShift;
    otherMirrorIndex = 8 - redShift;
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
    
    energyPackIndex = redShift;
    game.getItemLoc(energyPack, energyPackIndex);
    
    float downVelComponent = (energyPack[2] - mirror[2]) / 62.0f; 
    lzDownVel[0] = 0.0f;
	lzDownVel[1] = 0.025f;
	lzDownVel[2] = downVelComponent;
    
    backLzDownVel[0] = 0.0f;
    backLzDownVel[1] = -0.025f;
    backLzDownVel[2] = downVelComponent;
	game_state_ = POINT_BLOCK;
}

void loop(){
    api.getMyZRState(myState);
    api.getOtherZRState(otherState);
    
    myPos[0] = myState[0];
	myPos[1] = myState[1];
	myPos[2] = myState[2];
	otherPos[0] = otherState[0];
	otherPos[1] = otherState[1];
	otherPos[2] = otherState[2];
    
    leadingEdge += 0.025f; 
    if (leadingEdge >= 0.79f) leadingEdge = -0.8f;
    trailingEdge += 0.025f;
    if (trailingEdge >= 0.79f) trailingEdge = -0.8f;
    facingOther = game.isFacingOther();
    mathVecSubtract(vecBetween, otherPos, myPos, 3);
	    switch(game_state_) {
        case POINT_BLOCK:{
            float vecBetweenPoints[3];
            mathVecSubtract(vecBetweenPoints, upperPoint, lowerPoint, 3);
            float distance = fabsf(mathVecMagnitude(vecBetweenPoints, 3));
            bool getOtherPoint = false;
            if(distance < 0.4f) {
                getOtherPoint = true;
            }
            if(myPos[1] < 0.2f) {
                api.setVelocityTarget(lzVel);
            }
            else {
                api.setPositionTarget(upperPoint);
            }
            if(getOtherPoint && game.hasItem(upperPointIndex) ==0) {
                api.setPositionTarget(lowerPoint);
                if(game.hasItem(lowerPointIndex) ==0) {
                    faceOtherRotateValid = true;
                    picLive = true;
                    game_state_ = MIRROR;
                    uploadActionValid = true;
                }
            } else if(game.hasItem(upperPointIndex) == 0) {
                faceOtherRotateValid = true;
                picLive = true;
                game_state_ = MIRROR;
                uploadActionValid = true;
            }
            break;
        }
        case MIRROR:{
            api.setPositionTarget(mirror);
            if(game.hasItem(mirrorIndex) == 0) {
                game_state_ = PICTURE;
                uploadActionValid = false;
            }
            break;
        case PICTURE:
            if(game.posInDark(myPos)) {
                game_state_ = DARK_1;
            }
            break;
        case DARK_1:
            api.setVelocityTarget(lzDownVel);
            if(game.getEnergy() > 2.5f) {
                uploadActionValid = true;
            }
            else {
                uploadActionValid = false;
            }
            if(game.getEnergy() < 1.65f) {
                picActionValid = false;
            }
            if(myPos[1] > 0.55f || game.getEnergy() < 1.65f) {
                game_state_ = MOVE_THROUGH_LIGHT;
                picActionValid = false;
                uploadRotateValid = false;
            }
            break;
        }
        case MOVE_THROUGH_LIGHT:{
            api.setVelocityTarget(backLzDownVel);
            if(game.getEnergy() > 0.65 || myState[4] < 0.0f) {
                 uploadRotateValid = true;
            }
            
            if(!uploadActionValid && myState[4] < 0.0f) {
                uploadActionValid = true;
                picActionValid = true;
            }
            
            if(!uploadActionValid && !movedThroughLight) {
                picLive = true;
            }
            if(myPos[1] + myState[4] < leadingEdge + 0.1f) {
                game.useMirror();
                picLive = false;
            }
            if(!game.posInDark(myPos)){
                movedThroughLight = true;   
            }
            if(movedThroughLight && game.posInDark(myPos)) {
                game_state_ = ENERGY_PACK;
                picLive = true;
            }
            break;
        case ENERGY_PACK:
            if(game.hasItem(energyPackIndex) != -1) {
                game_state_ = DARK_2;
            }
            else {
                api.setPositionTarget(energyPack);
                if(game.hasItem(energyPackIndex) == 0) {
                    if(game.getEnergy() > 1.5f) {
                        uploads();
                    }
                }
            }
            break;
        }
        case DARK_2:{
            bool breakMotion = (game.getMemoryFilled() == 2 && game.getEnergy() < 1.8f);
            
            if(myPos[1] < 0.6f) {
                if(!breakMotion) {
                    api.setVelocityTarget(lzVel);
                }
            }
            else {
                if(!breakMotion) {
                    api.setVelocityTarget(stoppedVel);
                }
            }
            
            if(game.getMemoryFilled() != 0 && (game.getEnergy() < 2.8f || (game.getCurrentTime() >= 160))) {
                picLive = false;
                uploadLive = true;
                uploadActionValid = true;
                uploadRotateValid = true;
            }
            break;
        }
    }
	if(picLive) {
	   bool beyondMinDistance = mathVecMagnitude(vecBetween, 3) > 0.5f; 
    
    if(faceOtherRotateValid && !facingOther) {
        api.setAttitudeTarget(vecBetween);
    }
    else if(picActionValid && !game.posInDark(otherPos) && game.isFacingOther() && beyondMinDistance) {
        if(game.hasItem(otherMirrorIndex) == -1) {
            game.takePic();
        }
        else if (game.getPicPoints() > 1.0f) {
            game.takePic();
        }
    }
    if(game.getMemoryFilled() == 2) {
        picLive = false;
        uploadLive = true;
    }
	}
	if(uploadLive) {
        uploads();
	}
	bool leavingFullEnergy = game.getEnergy() + 0.5f * (1 + (myPos[1] - trailingEdge) / myState[4]) >= 6.0f;
    if(game.posInLight(myPos) && game.getMirrorTimeRemaining() != 0 && leavingFullEnergy) {
        game.takePic();
    }
}
//End page main
//Begin page pictures
void uploads() {
    float rotationSquared = myState[9] * myState[9] + myState[10] * myState[10] + myState[11] * myState[11];
    bool validRotationSpeed = rotationSquared < 0.0025f;
    float angle = acosf(myState[8]);
    bool validAngle = angle < 0.25f;
    bool narrowAngle = angle < 0.15f;
    
    if(uploadRotateValid && !narrowAngle) {
        api.setAttitudeTarget(earth);
    }
    if(uploadActionValid && validRotationSpeed && validAngle) {
        game.uploadPics();
    }
    
    if(game.getMemoryFilled() == 0) {
        uploadLive = false;
        uploadActionValid = false;
        picLive = true;
    }
}
//End page pictures
