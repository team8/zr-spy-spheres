//Begin page main
//Begin page Definitions
// SPHERE speed values
#define DEFAULT_SPEED 1.4f
#define SLOW_N_STEADY_SPEED 0.8f
#define FINAL_RUSH 1.9f
#define ZR_MAX_FACING_ANGLE 0.968912f //Cosine of the angle at which pictures may be taken/uploaded.
#define MIN_ENERGY 0.5f //Minimum energy remains after the action (e.g. picture taking or uploading)
#define MOVE_ENERGY 1.0f //Reserve energy for SPHERE Movement
#define OPPONENT_ABOVE_US -0.8f // this is -1.0f if opponent is directly above us
//End page Definitions
//Begin page EaglesKineticLib
/*
 *  Move my SPHERE toward the target position at k speed.
 *  Return the current distance from the target
 */
/*
float moveTo(float *target, float *myPos, float k)
{
	float v[3], r;
	
	mathVecSubtract(v, target, myPos, 3); //find vector v from my position to the target position
	r = mathVecMagnitude(v, 3);   // compute the magnitude of the above vector v
	mathVecMult(v, v, k, false);  // scale the above vector v by a factor of k; the speed factor
	mathVecAdd(v, myPos, v ,3);   // add the k scaled vector v to my position to create a new vector v
	api.setPositionTarget(v);     // tell the SPHERE to go to position specified by the new vector v above
	return r;                     // return the magnitude of the original vector v
}
*/

// stop spinning
void stopSpinning()
{
    float vz[3];
    memset(vz, 0.0f, sizeof(vz)); // create a vector with value 0 in all three dimensions
    api.setAttRateTarget(vz); // set SPHERE spinning rate to 0
}

//End page EaglesKineticLib
//Begin page EaglesMathLib
/* 
 * This function scale a normalized or non-normalized vector by mag; depending on the value of norm (true=normalize)
 */
void mathVecMult(float res[3], float src[3], float mag, bool norm)
{
    memcpy(res, src, sizeof(float) * 3); // copy the source vector to the result vector
    if (norm)
        mathVecNormalize(res, 3); // normalize the vector first if norm is true
    // scale the result vector by mag
    res[0] *= mag;
    res[1] *= mag;
    res[2] *= mag;
}

/*
 *  Calculate the angle between two position vectors
 *  Last modified: 10-12
 */
/*
float calculateTheta(float targetPos[3], float myPos[3])
{
    float v1[3], v2[3];
    
    mathVecMult(v1, targetPos, 1.0f, true);  // compute the unit vector for the target position vector
    mathVecMult(v2, myPos, 1.0f,true);       // compute the unit vector for my position vector
    return acosf(mathVecInner(v1, v2, 3)) * RAD2DEG;  // the arc cosine of the inner product of the two vectors * RAD2DEG is Theta in degrees
}
*/
/*
 *  Calculate the cosine of the angle between two position vectors
 *  Last modified: 10-12
 */
float doCosineTheta(float targetPos[3], float myPos[3])
{
    float v1[3], v2[3];

    mathVecMult(v1, targetPos, 1.0f, true); // compute the unit vector for the target position vector
    mathVecMult(v2, myPos, 1.0f, true); // compute the unit vector for my position vector
    return mathVecInner(v1, v2, 3); // the inner product of the two vectors is Cosine Theta
}

// compute the distance between two points
float doPointToPointDistance(float fromPointLoc[3], float toPointLoc[3])
{
    float vecFT[3];

    mathVecSubtract(vecFT, fromPointLoc, toPointLoc, 3);
    return mathVecMagnitude(vecFT, 3);
}

// see if we have arrived at or near the destination coordinate off by not more that the threshold amount
/*
bool areWeThereYet(float fromPointLoc[3], float toPointLoc[3], float threshold){
	if ( doPointToPointDistance(fromPointLoc, toPointLoc) < threshold) {
	    return true;
	}
	return false;
}
*/

//End page EaglesMathLib
//Begin page main
//Declare any variables shared between functions here

float myZRState[12]; // my SPHERE state
float otherZRState[12]; // other SPHERE state
float displacement[3]; // displacement vector between my SPHERE and opponent SPHERE
float direction[3]; // direction vector to point to opponent SPHERE
float uploadDir[3]; // direction vector to upload picture to earth (+z-axis unit vector = 0.0f,0.0f,1.0f)
//float myWayPoint[3];            // my waypoint to target
//float myMirror[3];              // location of closest mirror on my side
float closestPointsBlockList[4][3]; // 2D array to store PointsBlock locations
float myEnergyBlock[3]; // my closest energy block
float stepCloser[3]; // move closer to the target to score more photo points, must be >0.5f
float cosineTheta; // The cosine of the aiming angle
float picPoints; // picture points
float myEnergy; // my current energy
float otherEnergy; // opponent's current energy
float opponentDistance; // opponent's distance to my SPHERE
float opponentDirection; // if this value is close to -1.0f then opponent is directly above you.
float spinRate; // spinning rate of my SPHERE
float leadingEdge; // leading edge of light wave
float trailingEdge; // trailing edge of light wave
float d1, d2; // temp variables for distance comparison
int i; // temp loop count variable
int pointsBlockIDList[4]; // array of pointsBlockIDs
//int myMirrorID;                 // the item ID of my mirror
//int oppMirrorID;                // opponent mirror ID
int myEnergyID; // energy block closest to me initially
//int myMirrorStatus;             // Status of myMirror
int gameTime; // number of seconds passed since the beginning of the game (how many time game loop is called)
int availablePointBlocks; // PointBlocks still available to pick up
//bool myMirrorUsed;              // myMirror was used
//bool oppMirrorUsed;             // opponent mirror was used
//bool sideSwitchEnabled;         // switch side enabled
// bool safeToSkipPicPointsCheck;  // safe to skip picture point check
bool meInLight; // first time we see the light
bool opponentAboveMe; // opponent is above me
bool opponentTooClose; // opponent is too close for picture taking
bool opponentExposed; // opponent is exposed
bool pointsBlockAvailable; // at least one pointsBlock still available
bool myScoreIsHigher; // true if myScoreIsHigher;
int memFilled; // how much memory is filled

void init()
{
    //This function is called once when your code is first loaded.

    //IMPORTANT: make sure to set any variables that need an initial value.
    //Do not assume variables will be set to 0 automatically!
    gameTime = 0; // initialize game time to 0
    // initialize upload direction unit vector to point to the positive z-axis
    uploadDir[0] = 0.0f;
    uploadDir[1] = 0.0f;
    uploadDir[2] = 1.0f;
    //	myMirrorUsed = false; // initialize my mirror is used to false
    //	firstDawn = true;    // initialize to have not seen the light
    trailingEdge = -0.2f; // initialize trailing edge location to -0.2f in y axis
    leadingEdge = 0.5f; // initialize leading edge to -0.2+0.7 = 0.5f in y axis
    api.getMyZRState(myZRState); // read my SPHERE states
    // find closest mirror on my side
    /*    game.getItemLoc(myMirror, 7); // read first mirror location
	d1 = doPointToPointDistance(myZRState, myMirror);
	game.getItemLoc(myMirror, 8); // read second mirror location
	d2 = doPointToPointDistance(myZRState, myMirror);
	if (d1 > d2) { // save closest mirror location as myMirrorID
        myMirrorID = 8; // set closest Mirror item ID
        oppMirrorID = 7;
	}
	else {
	    myMirrorID = 7; // set closest Mirror item ID
	    oppMirrorID = 8;
	    game.getItemLoc(myMirror, 7); // reread mirror location
	}
*/
    // find my closest energy block
    myEnergyID = 0; // initialize ID of energy block closest to me to 0
    game.getItemLoc(myEnergyBlock, myEnergyID); // read closest energy block location into stepCloser
    if ((myEnergyBlock[0] * myZRState[0]) < 0.0f) {
        myEnergyID = 1; // revise ID of energy block closest to me to 1
        game.getItemLoc(myEnergyBlock, myEnergyID); // revise closest energy block location into stepCloser
    }
    pointsBlockAvailable = true; // initialize pointsBlock Available to true
    // find closest points block near my SPHERE
    d2 = 2.0f; // make d2 super large initially
    pointsBlockIDList[0] = 3; // initialize closest points block ID
    for (i = 0; i < 4; i++) { // find closest Points block
        game.getItemLoc(closestPointsBlockList[0], i + 3); // read first point block location
        d1 = doPointToPointDistance(myEnergyBlock, closestPointsBlockList[0]);
        if (d1 < d2) { // found a points block closer to my SPHERE
            d2 = d1; // replace d2 as the closest distance
            pointsBlockIDList[0] = i + 3;
            if (pointsBlockIDList[0] < 5) { // find the second closest points block on my mirror side
                pointsBlockIDList[1] = pointsBlockIDList[0] + 2;
                //	            pointsBlockIDList[3] = pointsBlockIDList[0] + 2;
            }
            else {
                pointsBlockIDList[1] = pointsBlockIDList[0] - 2;
                //	            pointsBlockIDList[3] = pointsBlockIDList[0] - 2;
            }
            if ((pointsBlockIDList[0] == 3) || (pointsBlockIDList[0] == 5)) {
                pointsBlockIDList[2] = pointsBlockIDList[1] + 1;
                pointsBlockIDList[3] = pointsBlockIDList[0] + 1;
                //	                pointsBlockIDList[1] = pointsBlockIDList[0] + 1;
                //	                pointsBlockIDList[2] = pointsBlockIDList[3] + 1;
            }
            else {
                pointsBlockIDList[2] = pointsBlockIDList[1] - 1;
                pointsBlockIDList[3] = pointsBlockIDList[0] - 1;
                //	                pointsBlockIDList[1] = pointsBlockIDList[0] - 1;
                //	                pointsBlockIDList[2] = pointsBlockIDList[3] - 1;
            }
        }
    }
    // DEBUG(("Closest Block distance: %f, PointsBlockID[1..4] %d, %d, %d, %d\n", d2, pointsBlockIDList[0],pointsBlockIDList[1],pointsBlockIDList[2],pointsBlockIDList[3]));
    for (i = 0; i < 4; i++) {
        game.getItemLoc(closestPointsBlockList[i], pointsBlockIDList[i]);
    }
    // set first way-point high up in -z
    //    memcpy(myWayPoint, myZRState, sizeof(float) * 3);  //copy initial coordinate from my position
    //    myWayPoint[2] = -0.5f;
    // DEBUG(("!!!!!ZiRconiuM_Paly_Eagles Strategy TE2!!!!!\n"));
}

void loop()
{
    //This function is called once per second.  Use it to control the satellite.
    api.getMyZRState(myZRState); // read my SPHERE states
    api.getOtherZRState(otherZRState); // read other SPHERE states
    meInLight = game.posInLight(myZRState);
    myEnergy = game.getEnergy(); // my current energy level
    otherEnergy = game.getOtherEnergy(); // opponent current energy level
    opponentExposed = !game.posInDark(otherZRState);
    memFilled = game.getMemoryFilled();
    /*     myScoreIsHigher = false;
     if (game.getOtherScore() < game.getScore()){
//     if ((game.getScore() - game.getOtherScore()) > 0.5f){
         myScoreIsHigher = true;
     }
*/
    mathVecSubtract(displacement, otherZRState, myZRState, 3); //find the direction vector to aim at opponent
    mathVecMult(direction, displacement, 1.0f, true); // normalize the direction vector
    opponentDistance = mathVecMagnitude(displacement, 3);
    //	opponentDirection = doCosineTheta(direction, uploadDir);  // if this value is close to -1.0f, then opponent is above us
    //	opponentAboveMe = false;
    //	if (opponentDirection < OPPONENT_ABOVE_US) {  // opponent is above my SPHERE
    if (otherZRState[2] < -0.45f) { // opponent above us
        opponentAboveMe = true;
    }
    else {
        opponentAboveMe = false;
    }

    //	DEBUG(("opponent direction = %f\n", opponentDirection));
    /*    if (fabsf(doCosineTheta(direction, &otherZRState[6])) >= ZR_MAX_FACING_ANGLE) {
        opponentCameraAimed = true;
    }
    else {
        opponentCameraAimed = false;
    }
*/
    //    opponentTooClose = false;
    if (opponentDistance < 0.45f) { // opponent is too close
        opponentTooClose = true;
        //        if (((meInLight) || (!pointsBlockAvailable) || (gameTime > 160) || (myEnergy > 3.5f) || (myEnergy < 1.0f)) && (game.getMemoryFilled() < 1.0f))  {
        if (((meInLight) || (gameTime > 160) || (myEnergy > 3.5f) || (myEnergy < 1.0f)) && (game.getMemoryFilled() < 1.0f)) {
            game.takePic(); // take picture anyway for 0.01 pts
        }
    }
    else {
        opponentTooClose = false;
    }

    // check if opponent is out of bound and weak in energy, if so, move to the center and take picture if we have energy
    if ((myEnergy > MOVE_ENERGY) && ((otherZRState[0] > 0.64f) || (otherZRState[0] < -0.64f) || (otherZRState[1] > 0.8f) || (otherZRState[1] < -0.8f) || (otherZRState[2] > 0.64f) || (otherZRState[2] < -0.64f))) {
        stepCloser[0] = 0.0f;
        stepCloser[1] = 0.0f;
        stepCloser[2] = 0.0f;
        api.setPositionTarget(stepCloser); // move farther from opponent SPHERE to score more
        // 	 DEBUG(("In opponent out of bound handling...\n"));
    }
    //    else if ((opponentAboveMe) || (myScoreIsHigher)) {  // chase opponent closer so it cannot take your picture
    else if (!pointsBlockAvailable) {
        // move back so pictures can be taken
        if ((opponentExposed) && (otherZRState[1] > trailingEdge) && (myEnergy > 3.5f)) {
            stepCloser[0] = myZRState[0];
            stepCloser[1] = otherZRState[1] - 0.55f; // move to the dark to take picture
        }

        else {
            bool driftingInGray = false;

            // move to trail behind opponent
            stepCloser[0] = 0.333f * myZRState[0] + 0.667f * otherZRState[0];
            stepCloser[1] = 0.333f * myZRState[1] + 0.667f * otherZRState[1];

            // set the y-velocity but make sure to not be stuck in gray
            if (game.posInGrey(myZRState) && myZRState[4] < 0.03f && myZRState[4] > 0.2f) {
                driftingInGray = true;
            }
            if (driftingInGray) {
                // if it will be drifting in the gray, speed up
                DEBUG(("Speeding up to not sit in gray"));
                stepCloser[1] = -1 * myZRState[1] + 2 * otherZRState[1];
            }
        }

        // always be on top
        stepCloser[2] = -0.45f;

        api.setPositionTarget(stepCloser); // move closer to opponent SPHERE to score more photo points
    }
    // if (game.getMemoryFilled() < 2)      // save energy for upload pictures if you got two already
    else if ((game.hasItem(myEnergyID) < 0) && (myEnergy > MOVE_ENERGY)) { // my mirror is still available for pickup
        api.setPositionTarget(myEnergyBlock); // move to energy block to pick it up
        //     DEBUG(("In energy capturing...\n"));
    }
    //	    else if ((pointsBlockAvailable) && (myEnergy > MOVE_ENERGY) && (myMirrorStatus >= 0)) { // there are still points block(s) to capture

    else if ((pointsBlockAvailable) && (myEnergy > MOVE_ENERGY)) { // there are still points block(s) to capture
        //     DEBUG (("In Points Block capturing...\n"));
        availablePointBlocks = 0;
        for (i = 0; i < 4; i++) {
            if ((game.hasItem(pointsBlockIDList[i]) < 0) && ((closestPointsBlockList[i][2] - otherZRState[2]) < 0.48f)) { // points block is still available
                availablePointBlocks += 1;
                //	                d1 = doPointToPointDistance(closestPointsBlockList[i], myZRState);
                //	                if ((game.posInLight(myZRState) || (myEnergy > MOVE_ENERGY)) && (d1 < 1.0f) && (availablePointBlocks == 1)) // wait for the light and check distance before move to points block
                //	                if ((game.posInLight(myZRState) || (myEnergy > MOVE_ENERGY)) && (availablePointBlocks == 1)) // wait for the light and check distance before move to points block
                if ((myEnergy > MOVE_ENERGY) && (availablePointBlocks == 1)) // check energy level before move to points block
                    api.setPositionTarget(closestPointsBlockList[i]); // move to capture closest points block
            }
        }
        if (availablePointBlocks == 0)
            pointsBlockAvailable = false;
    }
    else { // stay where you are
        //	        DEBUG(("Stay where you are...\n"));
        // memcpy(stepCloser, myZRState, sizeof(float)*3); // copy the current position of our SPHERE

        //            else if ((opponentAboveMe) || ((!opponentTooClose) && (!pointsBlockAvailable))) {  // chase opponent closer so it cannot take your picture

        api.setPositionTarget(myZRState);
        //	  DEBUG(("In stay where we are...\n"));
    }

    // decide where should the camera be aiming
    if ((game.getMemoryFilled() > 1.0f) || ((game.getMemoryFilled() > 0.0f) && ((gameTime > 160) || (opponentTooClose)))) { // camera film slots full
        api.setAttitudeTarget(uploadDir); //point camera at earth for uploading picture
        //	    DEBUG(("Aiming for upload direction to Earth"));
    }
    else { // point camera at opponent SPHERE
        api.setAttitudeTarget(direction); //point camera at Opponent SPHERE
    }
    // check if we should use the mirror: when our opponent is in the dark but we are not.
    /*
	if (!myMirrorUsed && (otherEnergy > 1.0f) && opponentCameraAimed && (game.posInDark(otherZRState)) && (!game.posInDark(myZRState)) && (myMirrorStatus==0)){
	    game.useMirror(); //use the mirror
	    myMirrorUsed = true;  // mark that we already used the mirror
	}
*/
    // check if we are in range to slow down the spinning for uploading
    cosineTheta = doCosineTheta(uploadDir, &myZRState[6]); // compute cosine theta
    if (cosineTheta >= ZR_MAX_FACING_ANGLE) { // in range to upload
        stopSpinning(); // slow down the SPHERE spinning and speed
        //        DEBUG(("Stop Spinning!"));
    }
    //    DEBUG(("CosineTheta of the angle to earth = %f", cosineTheta ));
    //check if we can score and it is safe to take a picture
    //    if ((!opponentTooClose) && (myMirrorStatus >= 0) && game.isFacingOther() && (game.getMirrorTimeRemaining()==0) && (!game.posInDark(otherZRState)) && (game.getEnergy() >= (1.0f+MIN_ENERGY))) {  // opponent SPHERE is in view for picture taking
    if ((!opponentTooClose) && game.isFacingOther() && (opponentExposed) && (myEnergy >= 1.8f)) { // opponent SPHERE is in view for picture taking
        picPoints = game.getPicPoints(); // check how many points the picture worths
        //      DEBUG(("This picture is worth = %f pts", picPoints));
        if (game.isCameraOn() && (myEnergy >= 1.7f) && (picPoints > 0.0f) && opponentExposed) { // check conditions for picture taking
            picPoints = game.takePic(); // take picture
            //          DEBUG(("This picture taken is worth = %f pts", picPoints));
        }
    }
    // check if camera is filled and need to upload pictures to earth
    if ((memFilled > 1) && (cosineTheta >= ZR_MAX_FACING_ANGLE) && (mathVecMagnitude(&myZRState[9], 3) <= 0.05f) && (myEnergy >= 1.7f)) { //check if we have more than one pictures to upload
        game.uploadPics(); // upload picture if we have pictures
        //        DEBUG(("Uploading pics\n"));
    }
    //    DEBUG(("Time : , %d", gameTime));
    if ((memFilled > 0) && ((gameTime > 160) || (opponentTooClose))) { //check if we have pictures to upload near end game
        if ((cosineTheta >= ZR_MAX_FACING_ANGLE) && (mathVecMagnitude(&myZRState[9], 3) <= 0.05f) && (myEnergy >= 1.3f || gameTime > 160) && !(pointsBlockAvailable && myEnergy <= 1.6f)) {
            game.uploadPics(); // upload picture if we have pictures
            //            DEBUG(("Uploading one pic\n"));
        }
    }
    /*    
    if (gameTime == 180){
        if (game.getOtherScore() < game.getScore()) {
            DEBUG(("!!!!!ZiRconiuM_Paly_Eagles strategy TE5 rock!!!!!\n"));
        }
        else {
            DEBUG(("!!!!!ZiRconiuM_Paly_Eagles strategy TE5 failed!!!!!\n"));
        } 
    }
*/
    if (gameTime == 180)
        DEBUG(("<=== Color of ZiRconiuM_Paly_Eagles strategy TE6B ===\n"));
    //  if (gameTime == 180) DEBUG(("!!!!!ZiRconiuM_Paly_Eagles top strategy C1!!!!!\n"));

    gameTime++; // increase game time value by one
    // compute leading and trailing edges of the light wave block moving at 0.025f per second
    leadingEdge += 0.025f;
    if (leadingEdge >= 0.8f)
        leadingEdge = -0.8f; // wrap around y boundary from 0.8 to -0.8
    trailingEdge += 0.025f;
    if (trailingEdge >= 0.8f)
        trailingEdge = -0.8f; // wrap around y boundary from 0.8 to -0.8
}

//End page main

//End page main
