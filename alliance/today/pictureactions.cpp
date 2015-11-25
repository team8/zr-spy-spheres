void takePictureAction() { 
	game.takePic();
}

void uploadPictureAction() {
	game.uploadPics();
}

//writen by motion team
void faceEarthAction() {
	float facing_vec[3];
	mathVecSubtract(facing_vec, zr_pos(uploadDir), zr_pos(my_state), 3);
	mathVecNormalize(facing_vec, 3);
	api.setAttitudeTarget(facing_vec);
}

void faceOtherAction() {
	float facing_vec[3];
	mathVecSubtract(facing_vec, zr_pos(other_pos), zr_pos(my_state), 3);
	mathVecNormalize(facing_vec, 3);
	api.setAttitudeTarget(facing_vec);
}
