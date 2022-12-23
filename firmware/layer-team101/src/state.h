#ifndef STATE_H
#define STATE_H

#include "sensor.h"

static const float MAX_HEIGHT = 0.3f; // m
static const float VEL_MAX = 0.2f; // m/s;
static const uint16_t OBSTACLE_TOLERANCE = 300; // mm
static const uint16_t MIN_TOLERANCE_MV_FORWARD = 150; // mm
static const uint16_t RADIUS = 450; // mm
static const uint16_t SIDE_RADIUS = 150; // mm
static const float ANGULAR_SPEED = 180.0f; // deg/s
static const float MAX_INCLINATION = 80.0f;  // deg


int identify(volatile FlightData*);
int idleState(volatile FlightData*);
int takeOffState(volatile FlightData*);
int landState(volatile FlightData*);
int moveForwardState(volatile FlightData*);
int turnBackState(volatile FlightData*);
int turnRightState(volatile FlightData*);
int turnLeftState(volatile FlightData*);
int endMissionState(volatile FlightData*);
void stop(void);
float getAngle(float, float);

// TODO : end mission and idle state
int (*state[])(volatile FlightData*) = {&idleState, &takeOffState, &landState, &moveForwardState,
                        &turnBackState, &turnRightState, &turnLeftState, &endMissionState, &identify};

enum StateCode
{
    IDLE,
    TAKEOFF,
    LAND,
    MV_FORWARD,
    TURN_BACK,
    TURN_RIGHT,
    TURN_LEFT,
    END_MISSION,
    IDENTIFY
};

enum ReturnCode
{
    DONE,
    REPEAT,
    FAIL_LO,
    FAIL_RO,
    FAIL_LRO
};

int lookupTransitions(int curState, int returnedCode);

typedef struct TransitionTag
{
    enum StateCode srcState;
    enum ReturnCode retCode;
    enum StateCode dstState;
} Transition;

typedef struct PositionTag
{
    float theta;
    float x;
    float y;
    struct Orientation
    {
        float x;
        float y;
        float heading;
    } uVec;
} Position;

void computeHeading(Position* pos, float speed);

Transition transitionTable[] = {
    {TAKEOFF, REPEAT, TAKEOFF},
    {TAKEOFF, DONE, MV_FORWARD},
    {MV_FORWARD, REPEAT, MV_FORWARD},
    {MV_FORWARD, FAIL_LO, TURN_RIGHT},
    {MV_FORWARD, FAIL_RO, TURN_LEFT},
    {MV_FORWARD, FAIL_LRO, TURN_BACK},
    {TURN_LEFT, REPEAT, TURN_LEFT},
    {TURN_LEFT, DONE, MV_FORWARD},
    {TURN_RIGHT, REPEAT, TURN_RIGHT},
    {TURN_RIGHT, DONE, MV_FORWARD},
    {TURN_BACK, REPEAT, TURN_BACK},
    {TURN_BACK, DONE, MV_FORWARD},
    {IDENTIFY, REPEAT, IDENTIFY},
    {END_MISSION, DONE, IDLE},
    {END_MISSION, REPEAT, END_MISSION}};

enum StateCode path[3700] = {};
int cpt = -1;

Position computedPos;

#endif