void mirrorAction() {

}

void scoreUpperAction() {
float[3] topCorrect;
	//Copy the position of the bottom item for the correct side.  whichItemIdIsBottom should return the id on the left for us (dependent on if we are left or right)
	api.getItemLoc(topCorrect, whichItemIdIsTop());
	api.setPositionTarget(topCorrect);
}

void scoreLowerAction() {
float[3] bottomCorrect;
	//Copy the position of the bottom item for the correct side.  whichItemIdIsBottom should return the id on the left for us (dependent on if we are left or right)
	api.getItemLoc(bottomCorrect, whichItemIdIsBottom());
	api.setPositionTarget(bottomCorrect);
}

void topAreaLazyAction() {

}

void topAreaDarkLeadAction() {

}

void topAreaDarkTrailAction() {

}
void topAreaLightAction() {
	
}

void moveWithHorizontalVector(){
	float h_vector[3];
	h_vector[0] = 0;
	h_vector[1] = 1;
	h_vector[2] = 0;
	api.setVelocityTarget(h_vector);
	//Set the horizontal vector to keep moving
}

void move(float pos[3]){
	api.setPositionTarget(pos); //Move there
	//Takes an x,y,z and moves the sphere there
}

void moveToMirror(){
	float mirrorLoc[3];
	api.getItemLoc(mirrorLoc,getCloserMirrorNumber());
	api.setPositionTarget(mirrorLoc);

}



