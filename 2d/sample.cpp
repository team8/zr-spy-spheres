//Begin page main
//Declare any variables shared between functions here
//#include <ZRGame.h>


float item1[3];
float item2[3];
float item3[3];

void init(){
	item1[0] = .4f;
	item1[1] = .6f;
	item1[2] = .0f;
	
	item2[0] = .0f;
	item2[1] = .6f;
	item2[2] = .0f;
	
	item3[0] = -.4f;
	item3[1] = .6f;
	item3[2] = .0f;
	
	
}


void loop() {
    
    
    if (game.getScore() <= 0.0f) {
        api.setPositionTarget(item1);
    } else if (game.getScore() == 1.5f) {
        api.setPositionTarget(item2);
    } else {
        api.setPositionTarget(item3);
    }

    
}
//End page main
