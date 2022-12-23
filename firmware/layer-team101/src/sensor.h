#ifndef SENSOR_H
#define SENSOR_H

#include "log.h"
#include "param.h"
#include "app_channel.h"
#include "communication.h"

// sensors
logVarId_t idFront;
logVarId_t idBack;
logVarId_t idLeft;
logVarId_t idRight;
logVarId_t idUp;

// position
logVarId_t idX;
logVarId_t idY;
logVarId_t idZ;
logVarId_t idVx;
logVarId_t idVy;
logVarId_t idVz;

// gyro
logVarId_t idYaw;
logVarId_t idPitch;
logVarId_t idRoll;

// State estimate
typedef struct PositionEstimateTag
{
    float x;
    float y;
    float z;
    float vx;
    float vy;
    float vz;
    float yaw;
    float pitch;
    float roll;
} PositionEstimate;

typedef struct SensorDataTag
{
    uint16_t front;
    uint16_t back;
    uint16_t left;
    uint16_t right;
    uint16_t up;
} SensorData;

typedef struct FlightDataTag
{
    PositionEstimate position;
    SensorData sensor;
} FlightData;

void initFlightData();
void updateFlightData(volatile FlightData*);

#endif