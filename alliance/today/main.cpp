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
	FACE_OTHER = 1,
	PICTURE = 2,
    FACE_EARTH = 3;
	UPLOAD = 4,
	BLANK
}

void init() {
	initData();
	move_state_ = BLANK;
    picture_state_ = BLANK;
}

void loop() {
    updateData();
    score();
    act();
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
        case BLANK:
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
        case BLANK:
            DEBUG(("Blank picture_state_"));
            break;
        default:
            DEBUG(("Invalid picture_state_ with index: %d", picture_state_));
    }
}