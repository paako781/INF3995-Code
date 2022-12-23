#include "sensor.h"
#include "console.h"

void initFlightData()
{
    // sensors
    idFront = logGetVarId("range", "front");
    idBack = logGetVarId("range", "back");
    idLeft = logGetVarId("range", "left");
    idRight = logGetVarId("range", "right");
    idUp = logGetVarId("range", "up");

    // position
    idX = logGetVarId("stateEstimate", "x");
    idY = logGetVarId("stateEstimate", "y");
    idZ = logGetVarId("stateEstimate", "z");
    idVx = logGetVarId("stateEstimate", "vx");
    idVy = logGetVarId("stateEstimate", "vy");
    idVz = logGetVarId("stateEstimate", "vz");
    idYaw = logGetVarId("stateEstimate", "yaw");
    idPitch = logGetVarId("stateEstimate", "pitch");
    idRoll = logGetVarId("stateEstimate", "roll");

    myPreviousPos.x = logGetFloat(idX);
    myPreviousPos.y = logGetFloat(idY);
    myPreviousPos.z = logGetFloat(idZ);
}

void updateFlightData(volatile FlightData* fd)
{
    fd->sensor.front = logGetUint(idFront);
    fd->sensor.back = logGetUint(idBack);
    fd->sensor.left = logGetUint(idLeft);
    fd->sensor.right = logGetUint(idRight);
    fd->sensor.up = logGetUint(idUp);

    fd->position.x = logGetFloat(idX);
    fd->position.y = logGetFloat(idY);
    fd->position.z = logGetFloat(idZ);
    fd->position.vx = logGetFloat(idVx);
    fd->position.vy = logGetFloat(idVy);
    fd->position.vz = logGetFloat(idVz);
    fd->position.yaw = logGetFloat(idYaw);
    fd->position.pitch = logGetFloat(idPitch);
    fd->position.roll = logGetFloat(idRoll);
}