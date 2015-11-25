#define FLOAT_SIZE 4

enum MoveState {
    MIRROR = 0,
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
} picture_state_;

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

void score() {
    float move_scores[7];
    move_scores[0] = mirrorScore();
    move_scores[1] = scoreUpperScore();
    move_scores[2] = scoreLowerAction();
    move_scores[3] = topAreaLazyScore();
    move_scores[4] = topAreaDarkLeadScore();
    move_scores[5] = topAreaDarkTrailScore();
    move_scores[6] = topAreaLight();
    for(int i = 0; i < sizeof(move_scores) / FLOAT_SIZE; i++) {
        if (move_scores[i] >= move_scores[MoveState]){
            move_scores = (MoveState) i;
        }
    }

    float picture_scores[5];
    picture_scores[0] = 0;
    picture_scores[1] = faceOtherScore();
    picture_scores[2] = pictureScore();
    picture_scores[3] = faceEarthScore();
    picture_scores[4] = uploadScore();
    for(int i = 0; i < sizeof(picture_scores) / FLOAT_SIZE; i++) {
        if (picture_scores[i] >= picture_scores[PictureState]){
            picture_scores = (PictureState) i;
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