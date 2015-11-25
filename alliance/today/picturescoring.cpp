#define TAKE_PIC_MIN_ENERGY 1.5f

//motion team please write this
bool facingEarth() {
	
}

float picScore() {
	if (!facingOther || energy <= TAKE_PIC_MIN_ENERGY ) {
		//we're not facing them or were out of energy-> We dont need to take a pic
		return -1.0f;
	}	
	
	return 
		
	
	
}

float uploadScore() {
	if (!facingEarth()) {
		return -1.0f;
	} 
}


