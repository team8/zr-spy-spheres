float myState[12];
float theirState[12];
float fuel;
float memoryUsed;
float memoryLeft;
float energy;
bool cameraOn;
float score;
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

void initData() {
	uploadDir[0] = 0.0f;
	uploadDir[1] = 0.0f;
	uploadDir[2] = 1.0f;
	isRed = isSphereRed();

}

void setItemLocations() {
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
}

void initData() {
	api.getMyZRState(myState);
	isRed = isRed();
	fuel = 60;
	energy = 5;
	cameraOn = true;
	score = 0;
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
	else return false;
}

float getLeading() {
	float leading = 1.05 + game.getLightInterfacePosition();
}

float getTrailing() {
	float trailing = 0.05 + game.getLightInterfacePosition();
}
