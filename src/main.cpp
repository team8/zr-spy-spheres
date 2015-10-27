//Begin page Picture Stuff
// TODO: Give 1/4 radian tolerance
void faceEarth(){
//   if(!game.isFacingOther()){
       float directional[3];
       float earth[3];
        earth[0] = 0.0f;
        earth[1] = 0.0f;
        earth[2] = 1.0f;
       mathVecSubtract(directional, zr_pos(earth), zr_pos(my_state), 3);
       mathVecNormalize(directional, 3);
       api.setAttitudeTarget(directional);
//   }
}

void face(){
   if(!game.isFacingOther()){
       float directional[3];
       mathVecSubtract(directional, zr_pos(other_state), zr_pos(my_state), 3);
       mathVecNormalize(directional, 3);
       api.setAttitudeTarget(directional);
   }
}
void attemptPic(){
    // we should not use isFacingOther as actual pictures have 0.25 radians of tolerance
    DEBUG(("Facing Other: %d", game.isFacingOther()));
	if(game.isFacingOther() == 1){
	    DEBUG(("FACING OTHER ROBOT"));
		if(game.getEnergy() >= 2.0f){
			if(game.posInLight(zr_pos(other_state))){
				game.takePic();
				DEBUG(("TOOK PIC"));
			}
			else{
			    DEBUG(("Opposing robot in dark"));
			}
		}
		else{
		    DEBUG(("Not enough energy"));
		}
		
	}
}

// TODO: Build in tolerance
void attemptUpload() {
    float earth[3];
    earth[0] = 0.0f;
    earth[1] = 0.0f;
    earth[2] = 1.0f;
    // faceEarth();
    api.setAttitudeTarget(earth);
    DEBUG(("zr_att(my_state)[2]: %f", zr_att(my_state)[2]));
    if(zr_att(my_state)[2] > 0.8 && rotVel(my_state) < 0.05) {
        DEBUG(("Uploading"));
        game.uploadPics();
    }
}

//End page Picture Stuff
//Begin page main
enum State {
	PHASE1_MIRROR, // starting game phase, the robot will head towards the closest mirror then transition to phase1_item
	PHASE12_ITEM, // moves towards the top scoring item while in phase1 or phase2. will transition to phase2_wait
	PHASE2_WAIT, // moves towards the target dark zone during phase 2
	PHASE2_ENERGY, // 24 seconds before the zone switch, activates a mirror and moves to the target dark zone in phase 2
	PHASE3_TRACKING, // sits in the target dark zone in phase3 and takes pictures 
	PHASE4_ENDGAME // uses all remaining energy for pictures and uploads
} state;
int phase;
char color;
int game_time;


void init() {
	setSphereData();
    game_time = 0;
	phase = 1;
	state = PHASE1_MIRROR;
	setColor();
	phase1_mirrorInit();
	setSwitchTimes();
	DEBUG(("Greetings from Paly Robotics, FRC Team #8"));
	DEBUG(("After the match check us out at palyrobotics.com"));
}

void loop() {
    game_time++;
    setItemPos();
	setSphereData();
	setGamePhase();
    DEBUG(("RotVel: %f", rotVel(my_state)));
	// remove this before submission
	DEBUG(("Phase: %d || State: %d", phase, state));

	switch(state) {
		case PHASE1_MIRROR:
		    phase1_mirrorLoop();
	    	break;
		case PHASE12_ITEM:
		    phase12_itemLoop();
	    	break;
		case PHASE2_WAIT:
	    	phase2_waitLoop();
	        break;
		case PHASE2_ENERGY:
	    	phase2_energyLoop();
	    	break;
		case PHASE3_TRACKING:
		    phase3_trackingLoop();
	    	break;
		case PHASE4_ENDGAME:
    		phase4_endgameLoop();
    		break;
		default:
	    	DEBUG(("NO STATE"));
	    	break;
	
	}
	face();
    attemptPic();
    if(game.getMemoryFilled() == 2) {
        attemptUpload();
    }
}
//End page main
//Begin page phase12_item
// picks up the top scoring object regardless of current position while in phase 1 or 2

void phase12_itemInit() {
    
}

void phase12_itemLoop() {
	moveToTopScoring();
	
	if(game.hasItem(6) != -1) {
		state = PHASE2_WAIT;
		phase2_waitInit();
	}
}

void moveToTopScoring() {
	float vector_between[3];
	mathVecSubtract(vector_between, items[6], zr_pos(my_state) , 3);
	float distance = mathVecMagnitude(vector_between, 3);

	if(distance > 0.4) {
		// looped PID calls :(
		api.setVelocityTarget(vector_between);
	} 

	if(distance < 0.3) {
		// looped PID calls :(
		api.setPositionTarget(items[6]);
	}
}
//End page phase12_item
//Begin page phase1_mirror
// phase 1 mirror currently only goes for the closest mirror as the tracking remains bugged
// no pictures are taken during this phase, that should be fixed as well



int close_mirror;
int far_mirror;

void phase1_mirrorInit() {
	setMirrorValues();
}
void phase1_mirrorLoop() {
	moveToCloseMirror();
	if (game.getNumMirrorsHeld() > 0) {
	    state = PHASE12_ITEM;
	    phase12_itemInit();
	}
}

void setMirrorValues() {
	if(color == 'B') {
	    DEBUG(("I am a Blue robot, moving to item 8"));
		close_mirror = 8;
		far_mirror = 7;
	}
	else {
	    DEBUG(("I am a Red robot, moving to item 7"));
		close_mirror = 7;
		far_mirror = 8;
	}
}

void moveToCloseMirror() {
	float vector_between[3];
	mathVecSubtract(vector_between, items[close_mirror], zr_pos(my_state) , 3);
	float distance = mathVecMagnitude(vector_between, 3);

    DEBUG(("Distance between zr_pos and close_mirror: %f", distance));
    printPosition(items[close_mirror]);
	printPosition(zr_pos(my_state));
	
	if(distance > 0.6) {
		// looped PID calls :(
		api.setVelocityTarget(vector_between);
	} 

	if(distance < 0.35) {
		// looped PID calls :(
		api.setPositionTarget(items[close_mirror]);
	}
}

// not called yet
void moveToFarMirror() {
	float vector_between[3];
	mathVecSubtract(vector_between, items[far_mirror], zr_pos(my_state),3);
	float distance = mathVecMagnitude(vector_between, 3);

	if(distance > 0.5) {
		// looped PID calls :(
		api.setVelocityTarget(vector_between);
	} else {
		// looped PID calls :(
		api.setPositionTarget(items[far_mirror]);
	}	
}

//End page phase1_mirror
//Begin page phase2_energy
float rest_pos[3];
bool rest_pos_reached;

void phase2_energyInit() {
	// TODO: set actual values
	rest_pos[0] = 0.0f;
	rest_pos[1] = -0.2f;
	rest_pos[2] = -0.5f;

	rest_pos_reached = false;
	game.useMirror();
}

void phase2_energyLoop() {
	if(!rest_pos_reached) {
		moveToRestPos();
	}
	else {
	    state = PHASE3_TRACKING;
	    phase3_trackingInit();
	}
	if(phase == 3) {
		state = PHASE3_TRACKING;
		phase3_trackingInit();
	}
}

void moveToRestPos() {
	float vector_between[3];
	mathVecSubtract(vector_between,rest_pos, zr_pos(my_state) , 3);
	float distance = mathVecMagnitude(vector_between, 3);

	// looped PID calls :(
	DEBUG(("Moving to wait position soft PID"));
	api.setPositionTarget(rest_pos);
	if(stopped(0.01) && zr_pos(my_state)[1] < 0.11f && zr_pos(my_state)[1] > 0.09f) {
		DEBUG(("Rest position reached"));
		rest_pos_reached = true;
	}
}
//End page phase2_energy
//Begin page phase2_wait
float wait_pos[3];
bool wait_pos_reached;


void phase2_waitInit() {
	// TODO: set actual values
	wait_pos[0] = 0.0f;
	wait_pos[1] = 0.1f;
	wait_pos[2] = -0.5f;
	wait_pos_reached = false;
}

void phase2_waitLoop() {
	if(!wait_pos_reached) {
		conservativeMoveToWaitPos();
	}
	if(game_time > switch_times[1] - 24) {
		state = PHASE2_ENERGY;
		phase2_energyInit();
	}
}

// moves the wait_pos using a very conservative amount of fuel
void conservativeMoveToWaitPos() {
	float vector_between[3];
	mathVecSubtract(vector_between, wait_pos, zr_pos(my_state) , 3);
	float distance = mathVecMagnitude(vector_between, 3);

	// looped PID calls :(
	DEBUG(("Moving to wait position soft PID"));
	api.setPositionTarget(wait_pos);
	if(stopped(0.001) && zr_pos(my_state)[1] < 0.11f && zr_pos(my_state)[1] > 0.09f) {
		DEBUG(("Wait position reached"));
		wait_pos_reached = true;
	}
}
//End page phase2_wait
//Begin page phase3_tracking
float z_lock;
float y_lock;

 
void phase3_trackingInit() {
	z_lock = -0.5f;
	y_lock = -0.2f;
}
void phase3_trackingLoop(){
	follow(y_lock);
	face();
	attemptPic();
    if(game.getMemoryFilled() == 2) {
        attemptUpload();
    }
	if(phase == 4) {
	    state = PHASE4_ENDGAME;
		phase4_endgameInit();
	}
}

void follow(float y){
    float target_pos[3];
    target_pos[0] = other_state[0];
    target_pos[1] = y_lock;
    target_pos[2] = z_lock;
    // open loop PID :(
    api.setPositionTarget(target_pos);
}
// void attemptPic(){
//     // we should not use isFacingOther as actual pictures have 0.25 radians of tolerance
//     DEBUG(("Facing Other: %d", game.isFacingOther()));
// 	if(game.isFacingOther()){
// 		if(game.getEnergy() >= 2.0f){
// 			if(game.posInLight(zr_pos(other_state))){
// 				game.takePic();
// 			}
// 			else{
// 			    DEBUG(("Opposing robot in dark"));
// 			}
// 		}
// 		else{
// 		    DEBUG(("Not enough energy"));
// 		}
		
// 	}
// }

// // TODO: Build in tolerance
// void attemptUpload() {
//     float earth[3];
//     earth[0] = 0.0f;
//     earth[1] = 0.0f;
//     earth[2] = 1.0f;
//     api.setAttitudeTarget(earth);
//     DEBUG(("zr_att(my_state)[2]: %f", zr_att(my_state)[2]));
//     if(zr_att(my_state)[2] > 0.8 && rotVel(my_state) < 0.05) {
//         DEBUG(("Uploading"));
//         game.uploadPics();
//     }
// }
//End page phase3_tracking
//Begin page phase4_endgame
#define ZPOSITION -0.5f

bool objectsLeft;
void phase4_endgameInit() {
	DEBUG(("In P4_Endgame, no code yet"));
    objectsLeft = false;
}

void phase4_endgameLoop() {
    // moveToBottomScoring();
    moveToDarkAndTakePics();
}

void moveToDarkAndTakePics() {
    float pos[3];
    pos[0] = other_state[0];
    pos[1] = 0.25f;
    pos[2] = 0.0f;
    
    if (game.posInDark(pos)) {
        api.setPositionTarget(pos);
    } else {
        pos[1] = -.25f;
        api.setPositionTarget(pos);
        
    }
}
// void moveToBottomScoring() {
// 	float vector_between[3];
// 	mathVecSubtract(vector_between, items[4], zr_pos(my_state) , 3);
// 	float distance = mathVecMagnitude(vector_between, 3);



// 	if(distance > 0.6) {
// 		// looped PID calls :(
// 		api.setVelocityTarget(vector_between);
// 	} 

// 	if(distance < 0.3) {
// 		// looped PID calls :(
// 		api.setPositionTarget(items[4]);
// 	}

// }
//End page phase4_endgame
//Begin page stateHelper
float my_state[12];
float other_state[12];
float items[9][3];
float switch_times[4];

void setItemPos() {
	for(int i = 0; i < 9; i++) {
	    for(int j = 0; j < 3; j++) {
	        items[i][j] = ITEM_LOC[i][j];
	    }
	}
}

void setSphereData() {
	api.getMyZRState(my_state);
	api.getOtherZRState(other_state);
}


void setColor() {
	if(zr_pos(my_state)[0] > 0) {
		color = 'B';
	}
	else {
		color = 'R';
	}
}

void setSwitchTimes() {
    switch_times[0] = game.getLightSwitchTime();
    switch_times[1] = switch_times[0] + 60;
    switch_times[2] = switch_times[1] + 60;
    switch_times[3] = 32000;
}

void setGamePhase() {
    if(game_time > switch_times[phase - 1]) {
        phase++;
    }
}

void printPosition(float pos[3]) {
    DEBUG(("Position (x, y, z): (%f, %f, %f)", pos[0], pos[1], pos[2]));
}

bool stopped(float limit) {
	return mathVecMagnitude(zr_velocity(my_state), 3) > limit;
}


/*
 * Provides easy access to different parts of the zr state
 * Example Use:
 *      zr_velocity(mystate)[3]; //gets the 3rd element of the velocity vector of the
 *                               //of 'mystate', where mystate is the zr state of, presumably, our robot
 */
 
 
 //velocity vector
 float* zr_velocity(float state[12]) { 
 	return &state[3];
 }

//position vector
 float* zr_pos(float state[12]) {
 	return &state[0];
 }

//attitude vector
 float* zr_att(float state[12]) {
 	return &state[6];
 }

//rotation vector
 float* zr_rot(float state[12]) {
 	return &state[9];
 }
 
 float rotVel(float state[12]) {
     float rot[3];
     rot[0] = zr_rot(my_state)[0];
     rot[1] = zr_rot(my_state)[1];
     rot[2] = zr_rot(my_state)[2];
     return mathVecMagnitude(rot, 3);
 }
//End page stateHelper
