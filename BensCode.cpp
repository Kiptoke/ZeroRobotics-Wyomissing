/*
Different strategies to explore:
1. Pick up analyzer first, then go to assigned square and drill.
2. Multiple drills at the same location
3. Use analyzer data to find high concentation drill sites.
4. Pick up 5 samples, then go to base station
5. Search for high concentration sites.
*/
///////////////////////////////////////////////////////////////////////////////////

//Option passed to functions indicating if they can increment the step counter
#define STEP_INC    1
#define STEP_NO_INC 0

//Current State
float myState[12];

//Current Attitude, index 6,7,8 of myState
float currAtt[3];
float myPos[3];
//Target attitude
float attitude[3];
float store_att[3];
float quat[4];
float vectorbetween[3];
float torques[3];

//Target position
float posn[3];
float origin[3];
float attitude_target[3];
//Position tolerance
float tolerance ;

//Target square
int square[3];
float velocity_zero_target[3];

//step counter
int step ;
int cycle;

//This function is called once when your code is first loaded.
void init() {
    step = 1;
    cycle = 0;
    velocity_zero_target[0] = 0.0;
    velocity_zero_target[1] = 0.0;
    velocity_zero_target[2] = 0.0;

    attitude[0] = -1;
    attitude[1] = 0;
    attitude[2] = 0;
    store_att[0] = -1;
    store_att[1] = 0;
    store_att[2] = 0;
    attitude_target[0] = 0.001;
    attitude_target[1] = 0.001;
    attitude_target[2] = 0.001;
    quat[0] = 0.0;
    quat[1] = 0.0;
    quat[2] = 0.0;
    quat[3] = 0.0;
    
    tolerance = 0.04;
    
    posn[0]=0;
    posn[1]=0;
    posn[2]=0;
}

//go to position in X,Y coord
void goToPosition( float posn[] , float tolerance , int inc ) {
    float err = fabsf(myState[0]-posn[0]) + fabsf(myState[1]-posn[1]) + fabsf(myState[2] - posn[2]);
    //DEBUG(("myState[0]=%f, myState[1]=%f",myState[0],myState[1]));
    //DEBUG(("posn[0]=%f, posn[1]=%f, err=%f",posn[0],posn[1],err));
    if (err > tolerance)
        api.setPositionTarget(posn);
    else if (inc)
            step++;
}

//rotate to target attitude 
void rotatePosition( float attitude[], float tolerance, int inc) {
    float err = fabsf(myState[6]-attitude[0]) + fabsf(myState[7]-attitude[1]) + fabsf(myState[8] - attitude[2]);
    
    //DEBUG(("myState[6]=%f, myState[7]=%f",myState[6],myState[7]));
    //DEBUG(("attitude[0]=%f, attitude[1]=%f, err=%f",attitude[0],attitude[1],err));
    if (err > tolerance)
       api.setAttitudeTarget(attitude);
        
    else if (inc)
            step++;
}

//Returns true if sum of absolute values of angular velocity
//components is less than tol
int notRotating(float tol) {
    float err = fabsf(myState[9]) + fabsf(myState[10]) + fabsf(myState[11]);
    return err < tol ;
}

//Main loop , called every second.
void loop() {
    
    //Update myState array
    api.getMyZRState(myState);
    
    //Current attitude is in elements 6,7,8 of myState
    for(int i=0; i<3; i++) {
        currAtt[i] = myState[i+6];
        myPos[i] = myState[i];
    }
    
    //api.setControlMode(CTRL_PD,CTRL_PID);
	//P, I and D values for position controller
	//High P value will increase speed of movement
	//resulting in overshoot, counteract with a higher
	//D value.
    //api.setPosGains(0.5,0.01,3.0);
    
    DEBUG(("Step %d", step));
    switch(step) {
        
        //Goto position
        case 1: 
        if (cycle >= 1)
        {
            square[0] = 4;
        square[1] = -6;
        square[2] = 5;
        //Convert square grid to XY coordinates (posn)
        game.square2pos(square,posn);
        
        origin[0] = 0;
        origin[1] = 0;
        origin[2] = 0;
        attitude[0] = 1.0;
        attitude[1] = 0.010;
        attitude[2] = 0.004;
        //mathVecSubtract(vectorbetween,origin,myPos, 3);
        //mathVecNormalize(vectorbetween, 3); 
        
       
        
        goToPosition(posn,tolerance,STEP_INC);
        rotatePosition(attitude,0.01,STEP_NO_INC);
            
        }
        else {
            square[0] = 4;
        square[1] = -6;
        square[2] = 5;
        //Convert square grid to XY coordinates (posn)
        game.square2pos(square,posn);
        
        origin[0] = 0;
        origin[1] = 0;
        origin[2] = 0;
        //mathVecSubtract(vectorbetween,origin,myPos, 3);
        //mathVecNormalize(vectorbetween, 3); 
        
       
        
        goToPosition(posn,tolerance,STEP_INC);
        }
        
        //rotatePosition(attitude,0.01,STEP_NO_INC);
        break;
        
        //Start drill
        case 2:
        goToPosition(posn,tolerance,STEP_NO_INC);
        
        if (  game.getDrillEnabled() ) {
            game.stopDrill();
        }
        else {
            
             store_att[0] = currAtt[0];
             store_att[1] = currAtt[1];
             store_att[2] = currAtt[2];
             attitude[0] = -currAtt[1]; //Nx
            attitude[1] =  currAtt[0]; //Ny
            attitude[2] = currAtt[2];
            api.setAttRateTarget(attitude_target);
            game.startDrill();
            
            //Compute target attitude
            //For 180 degree rotation, just invert sign
            //Nz
            step++;
        }
        break;
        
        //Rotate by 180 around Z axis for sample pickup
        case 3:
        goToPosition(posn,tolerance,STEP_NO_INC);//To hold position
        rotatePosition(attitude, 0.01,STEP_NO_INC);
       //Rotate
        if ( game.checkSample() )
          step++;
        break;
        
        //Pick up sample
        case 4:
        //These 2 statements are to hold current position
        //and rotation, otherwise we will get a drill error
        goToPosition(posn,tolerance,STEP_NO_INC);
        rotatePosition(attitude,0.01,STEP_NO_INC);
        
        game.pickupSample();
        game.stopDrill();
        step++;
        break;
        
        //Go to base station
        case 5:
        posn[0] =0;
        posn[1] =0;
        posn[2] =0;
        
        
        store_att[0] = currAtt[0];
        store_att[1] = currAtt[1];
        store_att[2] = currAtt[2];
        // Reorient in such a way that x face of sphere rotates to face the -Z direction
        attitude[0] = 0; 
        attitude[1] = 0;
        attitude[2] = -1;
        goToPosition(posn,tolerance,STEP_INC);
        rotatePosition(attitude,0.01,STEP_NO_INC);
        
        break;
        
        //Drop sample
        case 6:
        if ( notRotating(0.01) ) {
            game.dropSample(0);
            step++;
        }
        else {
            //Hold position
            goToPosition(posn,tolerance,STEP_NO_INC);
            //Stop rotating
            api.setAttitudeTarget(attitude);
        }
        break;
        
        //Done
        case 7:
       cycle++;
       step = 1;
       break;
    }
    
}
