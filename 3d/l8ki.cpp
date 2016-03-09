//Begin page main
//Begin page main
//Begin page Picture Stuff
#define MIN_PIC_ENERGY 1.1f
#define MAX_UPLOAD_VEL 0.05f
#define MAX_UPLOAD_ANGLE 0.25f
#define MIN_UPLOAD_ENERGY 1.25f
#define UPLOAD_ANGLE_PID_TARGET 0.20f

bool inFacingEPID;
float earthFacingDirectional[3];
float angleToEarth;

void faceEarth()
{
    if (uploadConditionsMet) {
        inFacingEPID = false;
    }
    else {
        api.setAttitudeTarget(earthFacingDirectional);
    }

    if (!inFacingEPID && angleToEarth > MAX_UPLOAD_ANGLE) {
        for (int i = 0; i <= 2; i++) {
            earthFacingDirectional[i] = zr_att(my_state)[i];
        }
        dropInclinationToTargetAngle(zr_att(my_state), earthFacingDirectional);
        inFacingEPID = true;
    }
}

void face()
{
    if (!facingOther) {
        float facing_vec[3];
        mathVecSubtract(facing_vec, zr_pos(other_state), zr_pos(my_state), 3);
        mathVecNormalize(facing_vec, 3);
        api.setAttitudeTarget(facing_vec);
    }
}

void attemptPic()
{
    if (facingOther && memoryFilled < 2 && energy >= MIN_PIC_ENERGY && game.isCameraOn() && game.getMirrorTimeRemaining() == 0 && !game.posInDark(zr_pos(other_state))
        && picCondition()) {
        game.takePic();
    }
}
bool picCondition()
{
    if (mirrorsHeld == 2 && game.getPicPoints() < 0.11) {
        return false;
    }
    return true;
}
void attemptUpload()
{
    faceEarth();
    if (uploadConditionsMet) {
        game.uploadPics();
    }
}
void tryPic()
{
    if (memoryFilled == 2 || ((phase == 4 || state == PHASE2_ENERGY) && memoryFilled > 0)) {
        angleToEarth = acosf((zr_att(my_state))[2]);
        attemptUpload();
    }
    else {
        face();
        attemptPic();
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
bool end_pos_reached;
float energy;

void init()
{
    setItems();
    inFacingEPID = false;
    end_pos_reached;
    setEarth();
    setSphereData();
    game_time = 0;
    phase = 1;
    state = PHASE1_MIRROR;
    setColor();
    setMirrorValues();
    setSwitchTimes();
    DEBUG(("Greetings from Paly Robotics, FRC Team #8"));
    DEBUG(("After the match visit us at palyrobotics.com"));
}

void loop()
{
    updateValues();
    game_time++;
    setSphereData();
    setGamePhase();

    switch (state) {
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
    }
    tryPic();
}
//End page main
//Begin page mathHelper
void dropInclinationToTargetAngle(float currentAtt[3], float upload[3])
{
    float c = sinf(UPLOAD_ANGLE_PID_TARGET) / sqrtf(mathSquare(currentAtt[0]) + mathSquare(currentAtt[1]));
    upload[0] = c * currentAtt[0];
    upload[1] = c * currentAtt[1];
    upload[2] = cosf(UPLOAD_ANGLE_PID_TARGET);
}
//End page mathHelper
//Begin page moveHelper
void endpointMove(float stopped_edge, float lower_edge, float upper_edge, float destination[])
{
    api.setPositionTarget(destination);
    if (stopped(stopped_edge) && zr_pos(my_state)[1] < upper_edge && zr_pos(my_state)[1] > lower_edge) {
        end_pos_reached = true;
    }
}

void moveToItem(int ID, float v_pid_edge, float p_pid_edge)
{
    float vector_between[3];
    mathVecSubtract(vector_between, items[ID], zr_pos(my_state), 3);
    float distance = mathVecMagnitude(vector_between, 3);

    if (distance > v_pid_edge) {
        api.setVelocityTarget(vector_between);
    }

    if (distance < p_pid_edge) {
        api.setPositionTarget(items[ID]);
    }
}
//End page moveHelper
//Begin page phase12_item
// picks up the top scoring object regardless of current position while in phase 1 or 2

#define TOP_SCORING_ID 6
#define P12_VELOCITY_PID_EDGE 0.3f
#define P12_POSITION_PID_EDGE 0.3f

void phase12_itemLoop()
{
    moveToItem(TOP_SCORING_ID, P12_VELOCITY_PID_EDGE, P12_POSITION_PID_EDGE);

    if (game.hasItem(TOP_SCORING_ID) != -1 || phase == 3) {
        state = PHASE2_WAIT;
        phase2_waitInit();
    }
}
//End page phase12_item
//Begin page phase1_mirror
// phase 1 mirror currently only goes for the closest mirror as the tracking remains bugged

#define P1_VELOCITY_PID_EDGE 0.53f
#define P1_POSITION_PID_EDGE 0.4f
#define BLUE_MIRROR 8
#define RED_MIRROR 7

int close_mirror;

void phase1_mirrorLoop()
{
    moveToItem(close_mirror, P1_VELOCITY_PID_EDGE, P1_POSITION_PID_EDGE);

    if (game.getNumMirrorsHeld() > 0) {
        state = PHASE12_ITEM;
    }
}

void setMirrorValues()
{
    close_mirror = RED_MIRROR;
    if (color == 'B') {
        close_mirror = BLUE_MIRROR;
    }
}
//End page phase1_mirror
//Begin page phase2_energy
void phase2_energyInit()
{
    end_pos_reached = false;
    destination_pos[1] = -0.2f;
    game.useMirror();
}

void phase2_energyLoop()
{
    if (!end_pos_reached) {
        endpointMove(0.04f, -0.25f, -0.15f, destination_pos);
    }
    else {
        state = PHASE3_TRACKING;
    }
    if (phase == 3) {
        state = PHASE3_TRACKING;
    }
}
//End page phase2_energy
//Begin page phase2_wait
void phase2_waitInit()
{
    end_pos_reached = false;
    destination_pos[0] = 0.0f;
    destination_pos[1] = 0.15f;
    destination_pos[2] = -0.5f;
}

void phase2_waitLoop()
{
    if (!end_pos_reached) {
        endpointMove(0.001f, 0.14f, 0.16f, destination_pos);
    }

    if (game_time > switch_times[1] - 24) {
        state = PHASE2_ENERGY;
        end_pos_reached = true;
        phase2_energyInit();
    }
}
//End page phase2_wait
//Begin page phase3_tracking
#define PHASE_3_TRACKING_Y -0.2f
#define PHASE_3_TRACKING_Z -0.5f
#define ENERGY_FOLLOW_LIMIT 1.0f

void phase3_trackingLoop()
{
    if (energy > ENERGY_FOLLOW_LIMIT) {
        follow(PHASE_3_TRACKING_Y);
    }
    if (phase == 4) {
        state = PHASE4_ENDGAME;
    }
}

void follow(float y)
{
    float target_pos[3];
    target_pos[0] = 0.0f;
    target_pos[1] = PHASE_3_TRACKING_Y;
    target_pos[2] = PHASE_3_TRACKING_Z;
    api.setPositionTarget(target_pos);
}
//End page phase3_tracking
//Begin page phase4_endgame
#define ZPOSITION -0.5f

void phase4_endgameLoop()
{
    float pos[3];
    pos[0] = 0.0f;
    pos[1] = 0.25f;
    pos[2] = -0.5f;
    api.setPositionTarget(pos);
}

//End page phase4_endgame
//Begin page stateHelper
float my_state[12];
float other_state[12];
float switch_times[4];
float earth[3];
float items[9][3];
float destination_pos[3];

bool facingOther;
int memoryFilled;
int mirrorsHeld;
float rotVel;
bool uploadConditionsMet;
int counter;

void updateValues()
{
    energy = game.getEnergy();
    facingOther = game.isFacingOther();
    memoryFilled = game.getMemoryFilled();
    mirrorsHeld = game.getNumMirrorsHeld();
    rotVel = getRotVel(my_state);
    uploadConditionsMet = getUploadConditionsMet();
}

void setSphereData()
{
    api.getMyZRState(my_state);
    api.getOtherZRState(other_state);
}

void setEarth()
{
    earth[0] = 0;
    earth[1] = 0;
    earth[2] = 1;
}

void setColor()
{
    color = 'R';
    if (zr_pos(my_state)[0] > 0) {
        color = 'B';
    }
}

void setSwitchTimes()
{
    switch_times[0] = game.getLightSwitchTime();
    switch_times[1] = switch_times[0] + 60;
    switch_times[2] = switch_times[1] + 60;
    switch_times[3] = 32000;
}

void setGamePhase()
{
    if (game_time > switch_times[phase - 1]) {
        phase++;
    }
}

bool getUploadConditionsMet()
{
    return (angleToEarth < MAX_UPLOAD_ANGLE && rotVel < MAX_UPLOAD_VEL);
}

/*
 * Provides easy access to different parts of the zr state
 * Example Use:
 *      zr_velocity(mystate)[3]; //gets the 3rd element of the velocity vector of the
 *                               //of 'mystate', where mystate is the zr state of, presumably, our robot
 */

//velocity vector
float* zr_velocity(float state[12])
{
    return &state[3];
}

//position vector
float* zr_pos(float state[12])
{
    return &state[0];
}

//attitude vector
float* zr_att(float state[12])
{
    return &state[6];
}

//rotation vector
float* zr_rot(float state[12])
{
    return &state[9];
}

float getRotVel(float state[12])
{
    float rot[3];
    for (int i = 0; i < 3; i++) {
        rot[i] = zr_rot(my_state)[i];
    }
    return mathVecMagnitude(rot, 3);
}

void setItems()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 3; j++) {
            items[i][j] = ITEM_LOC[i][j];
        }
    }
}

bool stopped(float limit)
{
    return mathVecMagnitude(zr_velocity(my_state), 3) > limit;
}
//End page stateHelper

//End page main

//End page main
