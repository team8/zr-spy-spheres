//Begin page main
//Begin page main
#define ZR_MAX_FACING_ANGLE 0.968912f //Cosine of the angle at which pictures may be taken/uploaded.
#define MOVE_ENERGY 1.0f //Reserve energy for SPHERE Movement

// This function scale a normalized or non-normalized vector by mag; depending on the value of norm (true=normalize)
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

//  Calculate the cosine of the angle between two position vectors
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

float myZRState[12]; // my SPHERE state
float otherZRState[12]; // other SPHERE state
float displacement[3]; // displacement vector between my SPHERE and opponent SPHERE
float direction[3]; // direction vector to point to opponent SPHERE
float uploadDir[3]; // direction vector to upload picture to earth (+z-axis unit vector = 0.0f,0.0f,1.0f)
float closestPointsBlockList[4][3]; // 2D array to store PointsBlock locations
float myEnergyBlock[3]; // my closest energy block
float stepCloser[3]; // move closer to the target to score more photo points, must be >0.5f
float cosineTheta; // The cosine of the aiming angle
float picPoints; // picture points
float myEnergy; // my current energy
float opponentDistance; // opponent's distance to my SPHERE
float opponentDirection; // if this value is close to -1.0f then opponent is directly above you.
float leadingEdge; // leading edge of light wave
float trailingEdge; // trailing edge of light wave
float d1, d2; // temp variables for distance comparison
int i; // temp loop count variable
int pointsBlockIDList[4]; // array of pointsBlockIDs
int myEnergyID; // energy block closest to me initially
int gameTime; // number of seconds passed since the beginning of the game (how many time game loop is called)
int availablePointBlocks; // PointBlocks still available to pick up
bool meInLight; // first time we see the light
bool opponentTooClose; // opponent is too close for picture taking
bool opponentExposed; // opponent is exposed
bool pointsBlockAvailable; // at least one pointsBlock still available
int memFilled; // how much memory is filled
bool finalUpload;

void init()
{
    gameTime = 0; // initialize game time to 0

    // initialize upload direction unit vector to point to the positive z-axis
    uploadDir[0] = 0.0f;
    uploadDir[1] = 0.0f;
    uploadDir[2] = 1.0f;
    trailingEdge = -0.2f; // initialize trailing edge location to -0.2f in y axis
    leadingEdge = 0.5f; // initialize leading edge to -0.2+0.7 = 0.5f in y axis
    api.getMyZRState(myZRState); // read my SPHERE states
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
            }
            else {
                pointsBlockIDList[1] = pointsBlockIDList[0] - 2;
            }
            if ((pointsBlockIDList[0] == 3) || (pointsBlockIDList[0] == 5)) {
                pointsBlockIDList[2] = pointsBlockIDList[1] + 1;
                pointsBlockIDList[3] = pointsBlockIDList[0] + 1;
            }
            else {
                pointsBlockIDList[2] = pointsBlockIDList[1] - 1;
                pointsBlockIDList[3] = pointsBlockIDList[0] - 1;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        game.getItemLoc(closestPointsBlockList[i], pointsBlockIDList[i]);
    }
    // set first way-point high up in -z
    finalUpload = false;
}

void loop()
{
    //This function is called once per second.  Use it to control the satellite.
    api.getMyZRState(myZRState); // read my SPHERE states
    api.getOtherZRState(otherZRState); // read other SPHERE states
    meInLight = game.posInLight(myZRState);
    myEnergy = game.getEnergy(); // my current energy level
    opponentExposed = !game.posInDark(otherZRState);
    memFilled = game.getMemoryFilled();
    mathVecSubtract(displacement, otherZRState, myZRState, 3); //find the direction vector to aim at opponent
    mathVecMult(direction, displacement, 1.0f, true); // normalize the direction vector
    opponentDistance = mathVecMagnitude(displacement, 3);

    if (gameTime >= 165 && memFilled < 1) {
        game.takePic();
    }
    if (opponentDistance < 0.45f) { // opponent is too close
        opponentTooClose = true;
        if ((meInLight || gameTime >= 160 || myEnergy > 3.5f || myEnergy < 1.0f) && memFilled < 1) {
            game.takePic(); // take picture anyway for 0.01 pts
        }
    }
    else {
        opponentTooClose = false;
    }

    if (gameTime >= 160 && memFilled > 0) {
        finalUpload = true;
    }

    // check if opponent is out of bound and weak in energy, if so, move to the center and take picture if we have energy
    if ((myEnergy > MOVE_ENERGY) && ((otherZRState[0] > 0.64f) || (otherZRState[0] < -0.64f) || (otherZRState[1] > 0.8f) || (otherZRState[1] < -0.8f) || (otherZRState[2] > 0.64f) || (otherZRState[2] < -0.64f))) {
        memset(stepCloser, 0.0f, 12);
        api.setPositionTarget(stepCloser); // move farther from opponent SPHERE to score more
    }
    else if (!pointsBlockAvailable) {
        // move back so pictures can be taken
        if ((opponentExposed) && (otherZRState[1] > trailingEdge) && (myEnergy > 3.5f) && ((otherZRState[1] - 0.55f) > -0.75)) {
            stepCloser[0] = myZRState[0];
            stepCloser[1] = otherZRState[1] - 0.55f; // move to the dark to take picture
        }

        else {
            bool driftingInGray = false;

            // move to trail behind opponent
            stepCloser[0] = 0.333f * myZRState[0] + 0.667f * otherZRState[0];
            stepCloser[1] = 0.333f * myZRState[1] + 0.667f * otherZRState[1];

            // set the y-velocity but make sure to not be stuck in gray
            if (myZRState[1] < leadingEdge + 0.1f && myZRState[4] < 0.03f && myZRState[4] > 0.02f) {
                driftingInGray = true;
            }
            if (driftingInGray) {
                // if it will be drifting in the gray, speed up
                stepCloser[1] = -1 * myZRState[1] + 2 * otherZRState[1];
            }
        }

        // always be on top
        stepCloser[2] = -0.45f;

        api.setPositionTarget(stepCloser); // move closer to opponent SPHERE to score more photo points
    }
    else if ((game.hasItem(myEnergyID) < 0) && (myEnergy > MOVE_ENERGY)) { // my mirror is still available for pickup
        api.setPositionTarget(myEnergyBlock); // move to energy block to pick it up
    }

    else if ((pointsBlockAvailable) && (myEnergy > MOVE_ENERGY)) { // there are still points block(s) to capture
        availablePointBlocks = 0;
        for (i = 0; i < 4; i++) {
            if ((game.hasItem(pointsBlockIDList[i]) < 0) && ((closestPointsBlockList[i][2] - otherZRState[2]) < 0.48f)) { // points block is still available
                availablePointBlocks += 1;
                if ((myEnergy > MOVE_ENERGY) && (availablePointBlocks == 1)) // check energy level before move to points block
                    api.setPositionTarget(closestPointsBlockList[i]); // move to capture closest points block
            }
        }
        if (availablePointBlocks == 0)
            pointsBlockAvailable = false;
    }
    else { // stay where you are
        api.setPositionTarget(myZRState);
    }
    if (finalUpload) {
        float noForce[3];
        memset(noForce, 0.0f, sizeof(float) * 3);
        api.setForces(noForce);
    }

    // decide where should the camera be aiming
    if ((memFilled > 1) || ((memFilled > 0) && ((gameTime > 160) || (opponentTooClose)))) { // camera film slots full
        api.setAttitudeTarget(uploadDir); //point camera at earth for uploading picture
    }
    else {
        api.setAttitudeTarget(direction); //point camera at Opponent SPHERE
    }

    // check if we are in range to slow down the spinning for uploading
    cosineTheta = doCosineTheta(uploadDir, &myZRState[6]); // compute cosine theta

    //check if we can score and it is safe to take a picture
    if ((!opponentTooClose) && game.isFacingOther() && (opponentExposed) && (myEnergy >= 1.8f)) { // opponent SPHERE is in view for picture taking
        picPoints = game.getPicPoints(); // check how many points the picture worths
        if (game.isCameraOn() && (myEnergy >= 2.0f || (myEnergy >= 1.7f && !pointsBlockAvailable)) && (picPoints > 0.0f) && opponentExposed) { // check conditions for picture taking
            picPoints = game.takePic(); // take picture
        }
    }

    // check if camera is filled and need to upload pictures to earth
    if (cosineTheta >= ZR_MAX_FACING_ANGLE && mathVecMagnitude(&myZRState[9], 3) <= 0.05f) {
        if (memFilled > 1 && myEnergy >= 1.7f) {
            game.uploadPics();
        }
        else if (memFilled > 0 && (gameTime >= 160 || (opponentTooClose && myEnergy >= 1.5f)) && (!pointsBlockAvailable || myEnergy >= 2.0f)) {
            game.uploadPics();
        }
    }

    if (gameTime == 180)
        DEBUG(("ZiRconiuM_Paly_Eagles TE6C"));
    gameTime++; // increase game time value by one
    // compute leading and trailing edges of the light wave block moving at 0.025f per second
    leadingEdge += 0.025f;
    trailingEdge += 0.025f;

    if (leadingEdge >= 0.8f)
        leadingEdge = -0.8f; // wrap around y boundary from 0.8 to -0.8
    if (trailingEdge >= 0.8f)
        trailingEdge = -0.8f; // wrap around y boundary from 0.8 to -0.8
}

//End page main

//End page main
