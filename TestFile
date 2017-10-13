//Declare any variables shared between functions here
/*NOTES FOR 3D:

    DRILL ONLY WORKS IF:
    
    TRANSLATION SPEED < 0.01 M/S
    ROTATION RATE UNDER 0.04 RAD/S
    EDGE OF SPHERE IS WITHIN 0.04M OF THE SURFACE
    X AXIS ALIGNED WITHIN 11.25 DEGREES OF THE XY PLANE
    
    ROTATE Z AXIS 90 DEGREES AT SAME SQUARE WITH SPEED UNDER 0.01 M/S
    MUST MAINTAIN 11.25 DEGREE ALIGNMENT

    MICROORGANISMS ARE LOCATED ON THE "SURFACE" AT Z = 0.64m
    
    ANALYZER LOCATIONS:
    X = 0.30M
    Y = -0.48M
    Z = -0.36M
    
    X = -0.30M
    Y = 0.48M
    Z = -0.36M
    
    TO DEPOSIT: 
    MUST MEET THE DRILLING REQUIREMENTS STATED ABOVE*/
    
    
    float position[3];
    float analyzerPOS1[3];
    float analyzerPOS2[3];
    
    float myState[12];
    float velocity[3];
    float rotation[3];
    
    
void init(){
	//This function is called once when your code is first loaded.

    analyzerPOS1[0] = 0.30;
    analyzerPOS1[1] = -0.48;
    analyzerPOS1[2] = -0.36;
    
    analyzerPOS2[0] = -0.30;
    analyzerPOS2[1] = 0.48;
    analyzerPOS2[2] = -0.36;
    

    
    
	//IMPORTANT: make sure to set any variables that need an initial value.
	//Do not assume variables will be set to 0 automatically!
}

void loop(){
	//This function is called once per second.  Use it to control the satellite.
	
	velocity[0] = myState[3];
	velocity[1] = myState[4];
	velocity[2] = myState[5];
	
	rotation[0] = myState[9];
	rotation[1] = myState[10];
	rotation[2] = myState[11];
	
	getMyZRState(myState);
	if (velocity[0] < 0.01 && velocity[1] < 0.01 && velocity[2] < 0.01 && rotation[0] < 0.04 && rotation[1] < 0.04 && rotation[2] < 0.04)
	{
	      
	}
	
	
	
	
	
	
	
	
	
	
}
