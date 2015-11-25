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

void initData() {
	uploadDir[0] = 0.0f;
	uploadDir[1] = 0.0f;
	uploadDir[2] = 1.0f;
	isRed = isSphereRed();

}

void setItemLocations() {
	if(isRed()) {

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

bool isRed() {
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
