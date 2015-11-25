
bool facingEarth() {
	
}

float picScore() {
	if (!facingOther || energy <= minPicEnergy ) {
		//we're not facing them or were out of energy-> We dont need to take a pic
		return -1.0f;
	}	
	
	return *(energy)+180-timeLeft;

		
	
	
}

float uploadScore() {

	if (!facingEarth()) {
		return -1.0f;
	} 
	
	return memFilled*(180-timeLeft);	
}

float faceEarthScore() {
	if (memFilled == 0) {
		return -1.0f;
	}

	return memFilled*180-timeLeft;
}
float faceOtherScore() {
	if (memFilled == 2) {
		return -1.0f;
	}

	return (90-timeLeft) - memFilled;
}