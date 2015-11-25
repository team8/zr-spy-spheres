float fuel;
float energy;
bool cameraOn;
float score;
int mirrorsHeld;
bool isRed;
float data[6];
float my_state[12];
float uploadDir[3];
float itemLoc[9][3];

void initData() {
	uploadDir[0] = 0.0f;
	uploadDir[1] = 0.0f;
	uploadDir[2] = 1.0f;
	isRed = isSphereRed();

}
bool isSphereRed() {
	if (my_state[0] >0) return false;
	return true;
}

void setItemLocations() {
	if(isRed()) {

	}
}

void updateData() {
	energy = game.getEnergy();
	fuel = game.getFuelRemaining();
	cameraOn = game.isCameraOn();
	api.getMyZRState(my_state);
	mirrorsHeld = game.getNumMirrorsHeld();

}
