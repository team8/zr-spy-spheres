
float offset;
float pos1[9];
float pos2[3];
//the offset of looking at us

float ourState[12];
//our robot state

//other robot state
float tState[12];

float distanceTo[];
float currentES;
//energy
float is;
float item1[3];
float item2[3];
float item3[3];
float item4[3];
//end energy items

//score items
float item5[3];
float item6[3];
float item7[3];
//end score items
float initPicked;
//mirror
float item8[3];
float item9[3];
//end mirror items

float distance;
void init()
{
    currentES = 0;
    getData();
    offset = .6;
    distance = .0f;
    initPicked = 0;
    is = 0;
}

void loop()
{
    if (game.getNumMirrorsHeld() < 1 && initPicked == 0) {
        api.setPositionTarget(item8);
        //start of the game action
    }
    if (game.getNumMirrorsHeld() == 1) {
        initPicked = 1;
    }

    if (initPicked == 1) {
        //api.setPositionTarget(item1);
        //if(game.hasItem(0)==-1){
        api.setPositionTarget(item5);
        // }
    }

    if (game.getScore() == 1.5) {
        if (game.hasItem(5) == -1) {
            api.setPositionTarget(item6);
            currentES = 1;
        }
        else if (game.hasItem(6) == -1) {
            api.setPositionTarget(item7);
            currentES = 1;
        }
        else {
            currentES = 1;
        }
    }
    if (currentES == 1) {
        float ret[3];
        float posT[3];
        api.getOtherZRState(tState);
        ret[0] = tState[0];
        ret[1] = tState[1];
        ret[2] = tState[2];
        api.setAttitudeTarget(ret);
        //  api.setPositionTarget(posT);
    }
    if (game.isFacingOther()) {
        game.takePic();
    }

    if (game.getMemoryFilled() >= 1) {
        game.uploadPics();
    }

    DEBUG(("%f", getDistanceSpheres()));
    //api.setPositionTarget(item4);
    if (isFacingYou()) {
        DEBUG(("IS FACING TRUE"));
        if (initPicked == 1) {
            game.useMirror();
        }
    }
    else {
        DEBUG(("IS FACING FALSE"));
    }
}
//get the distance between spheres
float getDistanceSpheres()
{
    api.getMyZRState(ourState);
    api.getOtherZRState(tState);
    distance = sqrt(pow(ourState[0] - tState[0], 2) + pow(ourState[1] - tState[1], 2) + pow(ourState[1] - tState[1], 2));
    // 3D distance formula
    return distance;
}

//get the distance between 2 objects
float getDistance(float pos2[3])
{
    api.getMyZRState(ourState);
    float dis = 0;
    dis = sqrt(pow(ourState[0] - pos2[0], 2) + pow(ourState[1] - pos2[1], 2) + pow(ourState[1] - pos2[1], 2));
    return dis;
}

bool isFacingYou()
{
    api.getMyZRState(ourState);
    api.getOtherZRState(tState);

    if (tState[6] - offset < ourState[0] && ourState[0] < tState[6] + offset) {
        if (tState[7] - offset < ourState[1] && ourState[1] < tState[7] + offset) {
            if (tState[8] - offset < ourState[2] && ourState[2] < tState[8] + offset) {
                return true;
            }
        }
    }
    else {
        return false;
    }
}
void getData()
{
    game.getItemLoc(item1, 0);
    game.getItemLoc(item2, 1);
    game.getItemLoc(item3, 2);
    game.getItemLoc(item4, 3);
    game.getItemLoc(item5, 4);
    game.getItemLoc(item6, 5);
    game.getItemLoc(item7, 6);
    game.getItemLoc(item8, 7);
    game.getItemLoc(item9, 8);
}