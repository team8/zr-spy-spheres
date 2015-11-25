#define TAKE_PIC_MIN_ENERGY 1.5f
#define ENERGY_CONST 1.0f
//motion team please write this
bool facingEarth() {
	
}

float picScore() {
	if (!facingOther || energy <= TAKE_PIC_MIN_ENERGY ) {
		//we're not facing them or were out of energy-> We dont need to take a pic
		return -1.0f;
	}	
	
	return ENERGY_CONST*(energy)+180-timeLeft;

		
	
	
}

float uploadScore() {

	if (!facingEarth()) {
		return -1.0f;
	} 
	
	return memFilled*(180-timeLeft);	
}

float faceEarthScore() {

}
float faceOtherScore() {
	
}