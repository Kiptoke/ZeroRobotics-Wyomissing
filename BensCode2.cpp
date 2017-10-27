/*
Wyomissing Robotics Team 3D Phase
Andrew's Final Code

Tasks:

Fix attitude problem after first base
Change base coordinates to its edge instead of center (remember to make two coordinates for different colored spheres)

*/
int inc;
int noInc;

float pos1[3];
float pos2[3];

float myState[12];
float myPosn[3];

float targetRate[3];
float stop[3];
float tolerance;
int step;
float base[3];
float baseAtt[3];
float att[3]; 

float distance;
float vectorBetween[3];

bool has2Samples;

int count;

void init(){
    
    has2Samples = false;
    
    att[0] = 0;
    att[1] = 0;
    att[2] = 0;
    
    baseAtt[0] = 0;
    baseAtt[1] = 0;
    baseAtt[2] = -1;
    
    myPosn[0] = 0;
    myPosn[1] = 0;
    myPosn[2] = 0;
    
    base[0] = 0;
    base[1] = 0;
    base[2] = 0;
    
    inc = 1;
    noInc = 0;
    tolerance = 0.02;

    targetRate[0] = 0.00;
    targetRate[1] = 0.00;
    targetRate[2] = -50 * PI/180;
    
    stop[0] = 0;
    stop[1] = 0;
    stop[2] = 0;
    
    count = 0;
    
    step = 1;
    
    pos1[0] = 0.2;
    pos1[1] = 0.2;
    pos1[2] = 0.37;
    
    pos2[0] = 0.25;
    pos2[1] = 0.25;
    pos2[2] = 0.37;
    
    myState[0] = 0;
    myState[1] = 0;
    myState[2] = 0;
    myState[3] = 0;
    myState[4] = 0;
    myState[5] = 0;
    myState[6] = 0;
    myState[7] = 0;
    myState[8] = 0;
    myState[9] = 0;
    myState[10] = 0;
    myState[11] = 0;

    vectorBetween[0] = 0.0;
    vectorBetween[1] = 0.0;
    vectorBetween[2] = 0.0;
    
    distance = 0;
}

//goes to specific position, takes into account tolerance, and set inc to 1 to increase step; set inc to 0 to not increase step
void goToPosition( float posn[] , float tolerance, int inc) {
    
    api.setAttitudeTarget(att);
    
    float err = fabsf(myState[0]-posn[0]) + fabsf(myState[1]-posn[1]) + fabsf(myState[2] - posn[2]);
    //DEBUG(("myState[0]=%f, myState[1]=%f",myState[0],myState[1]));
    //DEBUG(("posn[0]=%f, posn[1]=%f, err=%f",posn[0],posn[1],err));
    if (err > tolerance)
    {
        mathVecSubtract(vectorBetween,posn,myPosn,3);
        distance = mathVecMagnitude(vectorBetween,3);
        if (distance > 0.5)
        {
            api.setVelocityTarget(vectorBetween);
        }
        else
        {
            api.setPositionTarget(posn);
        }
    }
    else if (inc == 1)
    {
        step++;
    }
        
        
}

//rotate to attitudes (0, 0, 0)
void rotateToNormal() {
    
    
}

//checks if sphere is not rotating (required for starting drill and dropping samples)
int notRotating(float tol) 
{
    float err = fabsf(myState[9]) + fabsf(myState[10]) + fabsf(myState[11]);
    return err < tol ;
}

//entire function dedicated to going to base and dumping samples for points
void goToBase() 
{
    api.setAttRateTarget(stop);
    api.setAttitudeTarget(baseAtt);
    
    float err = fabsf(myState[0]-base[0]) + fabsf(myState[1]-base[1]) + fabsf(myState[2] - base[2]);
    //DEBUG(("myState[0]=%f, myState[1]=%f",myState[0],myState[1]));
    //DEBUG(("posn[0]=%f, posn[1]=%f, err=%f",posn[0],posn[1],err));
    if (err > tolerance)
    {
        api.setPositionTarget(base);
    }
    else
    {
        for (int i = 0; i < game.getNumSamplesHeld(); i++)
        {
            if (notRotating(0.01) || game.atBaseStation())
            {
                game.dropSample(i);
                has2Samples = false;
                step = 1;
            }
        }
    }
}

void loop() {

    //update ZRState
    api.getMyZRState(myState);

    //updates myPosition
    for(int i = 0; i < 3; i++)
    {
        myPosn[i] = myState[i];
    }

    //switches target values if sphere is red
    if (myPosn[0] < -0.18 && myPosn[0] > -0.22)
    {
        pos1[0] = -0.2;
        pos1[1] = -0.2;
  
        pos2[0] = -0.25;
        pos2[1] = -0.25;
    }

    //states the current step in the console
    DEBUG(("Step %d", step));

    switch(step){
    
    
        //if geyser spawns, stops drill and deposits current sample
        case 0:
    
            if (game.getDrillEnabled())
            {
                game.stopDrill();
            }
        
            goToBase();
            break;
    
        case 1:
        
            //sphere stops spinning
            //api.setAttRateTarget(stop);
            //api.setAttitudeTarget(stop);
            
        
            //goes to pos2 if sample at pos1 has been drilled
            //if (game.getDrills(pos1) > 0)
                //goToPosition(pos2, tolerance, inc);
           // else
            goToPosition(pos1, tolerance, inc);
            break;
        
        case 2:
        
            //starts drill
            game.startDrill();
            if (game.getDrillError())
            {
                game.stopDrill();
            }
                
            step++;
            break;
        
        case 3:
        
            if (game.getDrillError())
            {
                game.stopDrill();
            }
                
            //rotates the sphere and keeps sphere in correct position
            api.setAttRateTarget(targetRate);
            goToPosition(pos1, tolerance, noInc);
        
            //checks if sample is ready for pickup
            if (game.checkSample())
            {
                game.pickupSample();
                //game.stopDrill();
            }
            //checks if 2 samples have been collected
            //if there is 2 samples already, move spots (avoid geysers and improve efficiency)
            /*else if (game.getNumSamplesHeld() == 2 && has2Samples == false)
            {
                has2Samples = true;
                pos1[0] += 0.05;
                game.stopDrill(); 
                step = 1;
            }
            //returns to base if all samples collected or if a geyser spawns
            else if (game.getNumSamplesHeld() == 5)
            {
                step = 0;
            }*/
            if (game.isGeyserHere(myPosn))
                step = 0;
            break;
    }
}
