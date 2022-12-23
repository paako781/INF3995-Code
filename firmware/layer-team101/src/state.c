#include "state.h"
#include "commander.h"
#include <math.h>

#define MIN(a,b) ((a<b)?a:b)

static setpoint_t setpoint;

static void setHoverSetpoint(setpoint_t *setpoint, float vx, float vy, float z, float yawrate)
{
  setpoint->mode.z = modeAbs;
  setpoint->position.z = z;


  setpoint->mode.yaw = modeVelocity;
  setpoint->attitudeRate.yaw = yawrate;


  setpoint->mode.x = modeVelocity;
  setpoint->mode.y = modeVelocity;
  setpoint->velocity.x = vx;
  setpoint->velocity.y = vy;

  setpoint->velocity_body = true;
}

int lookupTransitions(int curState, int returnedCode)
{
    for (int i = 0; i < sizeof(transitionTable) / sizeof(transitionTable[0]); i++)
    {
        if (transitionTable[i].srcState == curState && transitionTable[i].retCode == returnedCode)
        {
            return transitionTable[i].dstState;
        }
    }

    // Precaution, but should never happen
    return IDLE;
}

int idleState(volatile FlightData* flightData)
{
    memset(&setpoint, 0, sizeof(setpoint_t));
    return DONE;
}

int identify(volatile FlightData* flightData)
{
    ledSetAll();
    vTaskDelay(M2T(300));
    ledClearAll();
    // Keep the current state
    return REPEAT;
}

int takeOffState(volatile FlightData* flightData)
{
    // consolePrintf("Take OFF \n");
    setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, 0);
    commanderSetSetpoint(&setpoint, 3);

    if (flightData->position.z <= MAX_HEIGHT - 0.05f)
        return REPEAT;
    
    return DONE;
}

int landState(volatile FlightData* flightData)
{
    // TODO : const variable for MIN_HEIGHT_TO_IDLE
    const float MIN_HEIGHT_TO_IDLE = 0.08f;
    setHoverSetpoint(&setpoint, 0, 0, 0, 0);
    commanderSetSetpoint(&setpoint, 3);

    if (flightData->position.z <= MIN_HEIGHT_TO_IDLE)
        return IDLE;

    return DONE;
}

int moveForwardState(volatile FlightData* flightData)
{
    //consolePrintf("moving forward... \n");

    // Go straight until an obstacle is detected by the front sensor
    if(flightData->sensor.front > RADIUS)
    {
        float factor = VEL_MAX / SIDE_RADIUS;
        uint16_t left_o = SIDE_RADIUS - MIN(flightData->sensor.left, SIDE_RADIUS);
        uint16_t right_o = SIDE_RADIUS - MIN(flightData->sensor.right, SIDE_RADIUS);
        float l_comp = (-1) * left_o * factor;
        float r_comp = right_o * factor;
        float velSide = r_comp + l_comp;
        setHoverSetpoint(&setpoint, VEL_MAX, velSide, MAX_HEIGHT, 0);
        commanderSetSetpoint(&setpoint, 3);
        float deltapPosX = 300.0f * velSide / 1000.0f;
        float deltaPosY = (-1.0f) * 300.0f * VEL_MAX / 1000.0f;
        float length = sqrtf(powf(deltapPosX,2) + powf(deltaPosY,2));
        computedPos.x += length * computedPos.uVec.x; 
        computedPos.y += length * computedPos.uVec.y;
        computedPos.theta = getAngle(computedPos.x, computedPos.y);
        //consolePrintf("cF=%u ", (unsigned)flightData->sensor.front);
        return REPEAT;
    }

    float vx;
    while((vx = logGetFloat(idVx)) > 0.01f)
    {
        //consolePrintf("vx=%f ", (double)vx);
        setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, 0);
        commanderSetSetpoint(&setpoint, 3);
        vTaskDelay(M2T(20));
    }
 
    //consolePrintf("obstacle en face ");

    return flightData->sensor.right < flightData->sensor.left ? FAIL_RO : FAIL_LO;
}

void computeHeading(Position* pos, float speed)
{
    // float norme = sqrtf(powf(pos->x, 2) + powf(pos->y, 2));
    float newVecX = cosf((300.0f * speed / 1000.0f) + getAngle(pos->uVec.x, pos->uVec.y));
    float newVecY = sinf((300.0f * speed / 1000.0f) + getAngle(pos->uVec.x, pos->uVec.y));
    // float newVecX = norme * cosf((300.0f * speed / 1000.0f) + (pos->x != 0.0f ? atanf(pos->y / pos->x) : (pos->y > 0.0f ? 90.0f : 270.0f)));
    // float newVecY = norme * sinf((300.0f * speed / 1000.0f) + (pos->x != 0.0f ? atanf(pos->y / pos->x) : (pos->y > 0.0f ? 90.0f : 270.0f)));
    float normeNewVec = sqrtf(powf(newVecX, 2) + powf(newVecY, 2));
    pos->uVec.x = newVecX / normeNewVec;
    pos->uVec.y = newVecY / normeNewVec;

    // if(pos->uVec.x > 0.0f)
    // {
    //     pos->uVec.heading = pos->uVec.y > 0 ? fabs(atanf(newVecY / newVecX)) : fabs(atanf(newVecY / newVecX)) +  270.0f;
    // }
    // else{
    //     pos->uVec.heading = pos->uVec.y > 0 ? fabs(atanf(newVecY / newVecX)) + 90.0f : fabs(atanf(newVecY / newVecX)) +  180.0f;
    // }

    // float theta = (float)fabs(atanf(newVecY / newVecX));
    // if(pos->uVec.x > 0.0f)
    // {
    //     pos->uVec.heading = pos->uVec.y > 0 ? theta : theta +  270.0f;
    // }
    // else{
    //     pos->uVec.heading = pos->uVec.y > 0 ? theta + 90.0f : theta +  180.0f;
    // }

    // pos->uVec.heading = (float)((int)(pos->uVec.heading + getAngle(pos->uVec.x, pos->uVec.y)) % 360);
    pos->uVec.heading = getAngle(pos->uVec.x, pos->uVec.y);
    consolePrintf("heading=%f\n", (double)pos->uVec.heading);
}

float getAngle(float x, float y)
{
    if ( x == 0) return (y  > 0.0f ? 90.0f : 270.0f);

    if (x > 0.0f)
    {
        if(y > 0.0f)
        {
            return atanf(fabs(y)/fabs(x));
        }
        else{
            return atanf(fabs(y)/fabs(x)) + 270.0f;
        }
    }
    else{
        if(y > 0.0f)
        {
            return atanf(fabs(y)/fabs(x)) + 90.0f;
        }
        else{
            return atanf(fabs(y)/fabs(x)) + 180.0f;
        }
    }
}

int turnLeftState(volatile FlightData* flightData)
{
    //path[cpt++] = TURN_LEFT;
    //float curYaw = flightData.position.yaw;
    //consolePrintf(" TLT=%f curYaw=%f ", (double)turnLeftTarget, (double)flightData.position.yaw);
    setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, 0);
    commanderSetSetpoint(&setpoint, 3);
    // float factor = VEL_MAX / MIN_TOLERANCE_MV_FORWARD;;
    // uint16_t left_o = MIN_TOLERANCE_MV_FORWARD - MIN(flightData->sensor.left, MIN_TOLERANCE_MV_FORWARD);
    // uint16_t right_o = MIN_TOLERANCE_MV_FORWARD - MIN(flightData->sensor.right, MIN_TOLERANCE_MV_FORWARD);
    // float l_comp = (-1) * left_o * factor;
    // float r_comp = right_o * factor;
    // float velSide = r_comp + l_comp;

    uint16_t curFront = logGetUint(idFront);

    // consolePrintf("Turning left \n");
    if (curFront < 300)
    {
        setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, ANGULAR_SPEED);
        commanderSetSetpoint(&setpoint, 3);
        //vTaskDelay(M2T(10));
        //consolePrintf("yawing TL");
        computeHeading(&computedPos, ANGULAR_SPEED);
        return REPEAT;
    }

    // setHoverSetpoint(&setpoint, VEL_MAX, 0, MAX_HEIGHT, 0);
    // commanderSetSetpoint(&setpoint, 3);
    // vTaskDelay(M2T(10));

    return DONE;
}

int turnRightState(volatile FlightData* flightData)
{
    //path[cpt++] = TURN_RIGHT;
    setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, 0);
    commanderSetSetpoint(&setpoint, 3);

    // float factor = VEL_MAX / MIN_TOLERANCE_MV_FORWARD;
    // uint16_t left_o = MIN_TOLERANCE_MV_FORWARD - MIN(flightData->sensor.left, MIN_TOLERANCE_MV_FORWARD);
    // uint16_t right_o = MIN_TOLERANCE_MV_FORWARD - MIN(flightData->sensor.right, MIN_TOLERANCE_MV_FORWARD);
    // float l_comp = (-1) * left_o * factor;
    // float r_comp = right_o * factor;
    // float velSide = r_comp + l_comp;

    uint16_t curFront = logGetUint(idFront);
    

    //consolePrintf("Turning right \n");
    if (curFront < 300)
    {
        setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, (-1.0f) * ANGULAR_SPEED);
        commanderSetSetpoint(&setpoint, 3);
        vTaskDelay(M2T(10));
        computeHeading(&computedPos, (-1.0f) * ANGULAR_SPEED);
        return REPEAT;
    }

    // setHoverSetpoint(&setpoint, VEL_MAX, 0, MAX_HEIGHT, 0);
    // commanderSetSetpoint(&setpoint, 3);
    // vTaskDelay(M2T(10));

    return DONE;
}

// Turn back
int turnBackState(volatile FlightData* flightData) { return DONE; }

int endMissionState(volatile FlightData* flightData) 
{
    stop();

    // In my current axis
    float straightXToBase = (-1.0f) * computedPos.x;
    float straightYToBase = (-1.0f) * computedPos.y;
    float targetHeading = getAngle(straightXToBase, straightYToBase);
    //float theta = (float)fabs(atanf(straightYToBase / straightXToBase));
    //float theta = (straightXToBase != 0.0f ? atanf(straightYToBase / straightXToBase) : (straightYToBase > 0.0f ? 90.0f : 270.0f));

    // if(straightXToBase > 0.0f)
    // {
    //     targetHeading = straightYToBase > 0 ? theta : theta +  270.0f;
    // }
    // else{
    //     targetHeading = straightYToBase > 0 ? theta + 90.0f : theta +  180.0f;
    // }

    //theta = (straightXToBase != 0.0f ? atanf(straightYToBase / straightXToBase) : (straightYToBase > 0.0f ? 90.0f : 270.0f));

    //consolePrintf("targe");

    while(fabs(computedPos.uVec.heading - targetHeading) > (double)5.0f)
    {
        setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, 25.0f);
        commanderSetSetpoint(&setpoint, 3);
        computeHeading(&computedPos, 25.0f);    
        updateFlightData(flightData);
        vTaskDelay(M2T(300));
        consolePrintf("target=%f current=%f\n", (double)targetHeading, (double)computedPos.uVec.heading);
    }

    // Heading right
    setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, 0.0f);
    vTaskDelay(M2T(300));

    updateFlightData(flightData);

    moveForwardState(flightData);

    landState(flightData);

    return DONE;
}

// int endMissionState(volatile FlightData* flightData) 
// {
//     stop();
//     float vxBs = 0.0f;
//     float vyBs = 0.0f;
//     float vx = 0.0f;
//     float vy = 0.0f;

//     for (size_t i = cpt; i >= 0; i--)
//     {

//         while(path[i] == MV_FORWARD)
//         {
//             if(i == cpt)
//             {
//                 // On start
//                 vx = (-1.0f) * VEL_MAX;
//                 vy = 0.0f;
//             }
//             setHoverSetpoint(&setpoint, vx, vy, MAX_HEIGHT, 0);
//             commanderSetSetpoint(&setpoint, 3);
//             vTaskDelay(M2T(300));
//             i--;
//         }

//         vxBs = logGetFloat(idVx);
//         vyBs = logGetFloat(idVy);

//         stop();

//         while(path[i] == TURN_RIGHT)
//         {
//             vx = fabs(vyBs) > fabs(vxBs) ? VEL_MAX : 0.0f;
//             vy = fabs(vyBs) > fabs(vxBs) ? 0.0f : (-1.0f)*VEL_MAX;
//             i--;
//         }

//         stop();

//         while(path[i] == TURN_LEFT)
//         {
//             vx = fabs(vyBs) > fabs(vxBs) ? VEL_MAX : 0.0f;
//             vy = fabs(vyBs) > fabs(vxBs) ? 0.0f : VEL_MAX;
//             i--;
//         }

//     }
    

// }


// int endMissionState(volatile FlightData* flightData) 
// {
//     // stop(flightData);
//     float newVx = 0.0f;
//     float newVy = 0.0f;
//     int previous = cpt + 1;

//     // for (size_t i = cpt - 1; i >= 0; i--)
//     // {
//     //     cpt--;
//         if(path[cpt] == MV_FORWARD)
//         {
//             if (path[previous] != MV_FORWARD) stop(flightData);
//             setHoverSetpoint(&setpoint, (-1.0f) * VEL_MAX, 0, MAX_HEIGHT, 0);
//             commanderSetSetpoint(&setpoint, 3);
//             //vTaskDelay(M2T(300));
//         }
//         else if(path[cpt] == TURN_RIGHT)
//         {
//             if(fabs((double)logGetFloat(idVy)) > fabs((double)logGetFloat(idVx)))
//             {
//                 newVx = logGetFloat(idVy) > 0.0f ? (-1.0f) * VEL_MAX : VEL_MAX;
//                 if (path[previous] != TURN_RIGHT) stop(flightData);
//                 setHoverSetpoint(&setpoint, newVx, 0, MAX_HEIGHT, 0);
//                 commanderSetSetpoint(&setpoint, 3);
//                 //vTaskDelay(M2T(300));
//                 consolePrintf("TR-1");
//             }
//             else if(fabs((double)logGetFloat(idVx)) > fabs((double)logGetFloat(idVy)))
//             {
//                 newVy = logGetFloat(idVx) > 0.0f ? VEL_MAX : (-1.0f) * VEL_MAX;
//                 if (path[previous] != TURN_RIGHT) stop(flightData);
//                 setHoverSetpoint(&setpoint, 0.0f, newVy, MAX_HEIGHT, 0);
//                 commanderSetSetpoint(&setpoint, 3);
//                 //vTaskDelay(M2T(300));
//                 consolePrintf("TR-2");
//             }
//         }
//         else if(path[cpt] == TURN_LEFT)
//         {
//             if(fabs((double)logGetFloat(idVy)) > fabs((double)logGetFloat(idVx)))
//             {
//                 newVx = logGetFloat(idVy) > 0.0f ? VEL_MAX : (-1.0f) * VEL_MAX;
//                 if (path[previous] != TURN_LEFT) stop(flightData);
//                 setHoverSetpoint(&setpoint, newVx, 0, MAX_HEIGHT, 0);
//                 commanderSetSetpoint(&setpoint, 3);
//                 //vTaskDelay(M2T(300));
//                 consolePrintf("TF-1");
//             }
//             else if(fabs((double)logGetFloat(idVx)) > fabs((double)logGetFloat(idVy)))
//             {
//                 newVy = logGetFloat(idVx) > 0.0f ? (-1.0f) * VEL_MAX : VEL_MAX;
//                 if (path[previous] != TURN_LEFT) stop(flightData);
//                 setHoverSetpoint(&setpoint, 0.0f, newVy, MAX_HEIGHT, 0);
//                 commanderSetSetpoint(&setpoint, 3);
//                 //vTaskDelay(M2T(300));
//                 consolePrintf("TF-2");
//             }
//         }

//         //updateFlightData(flightData);
//     //}
//     cpt--;
//     if(cpt == 0)
//     {
//         stop(flightData);
//         while(landState(flightData) != DONE) updateFlightData(flightData);
//         return DONE;
//     }
    
//     return REPEAT;
// }

void stop()
{
    // Stop
    float vx;
    float vy;
    while((vx = logGetFloat(idVx)) > 0.02f || (vy = logGetFloat(idVy) > 0.02f))
    {
        //consolePrintf("vx=%f ", (double)vx);
        setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, 0);
        commanderSetSetpoint(&setpoint, 3);
        vTaskDelay(M2T(50));
    }
}
