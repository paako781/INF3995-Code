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
    float newVecX = cosf((300.0f * speed / 1000.0f) + getAngle(pos->uVec.x, pos->uVec.y));
    float newVecY = sinf((300.0f * speed / 1000.0f) + getAngle(pos->uVec.x, pos->uVec.y));
    float normeNewVec = sqrtf(powf(newVecX, 2) + powf(newVecY, 2));
    pos->uVec.x = newVecX / normeNewVec;
    pos->uVec.y = newVecY / normeNewVec;

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
    
    setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, 0);
    commanderSetSetpoint(&setpoint, 3);

    uint16_t curFront = logGetUint(idFront);

    if (curFront < 300)
    {
        setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, ANGULAR_SPEED);
        commanderSetSetpoint(&setpoint, 3);
        
        computeHeading(&computedPos, ANGULAR_SPEED);
        return REPEAT;
    }


    return DONE;
}

int turnRightState(volatile FlightData* flightData)
{
    setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, 0);
    commanderSetSetpoint(&setpoint, 3);

    uint16_t curFront = logGetUint(idFront);

    if (curFront < 300)
    {
        setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, (-1.0f) * ANGULAR_SPEED);
        commanderSetSetpoint(&setpoint, 3);
        vTaskDelay(M2T(10));
        computeHeading(&computedPos, (-1.0f) * ANGULAR_SPEED);
        return REPEAT;
    }

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

void stop()
{
    // Stop
    float vx;
    float vy;
    while((vx = logGetFloat(idVx)) > 0.02f || (vy = logGetFloat(idVy) > 0.02f))
    {
        setHoverSetpoint(&setpoint, 0, 0, MAX_HEIGHT, 0);
        commanderSetSetpoint(&setpoint, 3);
        vTaskDelay(M2T(50));
    }
}
