#include "communication.h"
#include "state.h"
#include "math.h"
#include "pm.h"

void initTasks(volatile FlightData* fd, enum StateCode* sc)
{
    void* fsParam[2] = {(void*)sc, (void*)fd};
    BaseType_t logTaskRet = xTaskCreate(sendFlightData, "LogTask", 2 * configMINIMAL_STACK_SIZE, (void*)fd,
                4, NULL);

    BaseType_t stateTaskRet = xTaskCreate(sendFlightState, "CurStateTask", 2 * configMINIMAL_STACK_SIZE, (void*)fsParam,
                 4, NULL);

  if (logTaskRet != pdPASS || stateTaskRet != pdPASS)
    consolePrintf("Start error\n");
}

int commandReceiver()
{
  uint8_t packetBuffer[APPCHANNEL_MTU] = {};
  if (appchannelReceiveDataPacket(&packetBuffer, sizeof(APPCHANNEL_MTU), 0))
  {
    // Instance a foor loop to compare the value of packetbuffer with contains of the table of commands.

    for (int i = 0; i < sizeof(commandList) / sizeof(TextCommand); i++)
    {
      if (!strcmp((char *)packetBuffer, commandList[i].cmdTxt))
      {
        return commandList[i].cmdId;
      }
    }
  }

  return NO_CMD_RECEIVED;
}

void sendFlightData(void* fd)
{
  struct LogData log;
  struct Vec3Tag pos;
  // ground station packet handler
  log.handler = LOG_DATA;

  while(true)
  {
    // log.x = ((FlightData*)fd)->position.x;
    // log.y = ((FlightData*)fd)->position.y;
    log.z = ((FlightData*)fd)->position.z;
    log.x = computedPos.x;
    log.y = computedPos.y;
    log.up = ((FlightData*)fd)->sensor.up;
    log.front = ((FlightData*)fd)->sensor.front;
    log.left = ((FlightData*)fd)->sensor.left;
    log.right = ((FlightData*)fd)->sensor.right;
    log.back = ((FlightData*)fd)->sensor.back;

    consolePrintf("COMM x=%f y=%f z=%f\n", (double)log.x, (double)log.y, (double)log.z);
  
    appchannelSendDataPacketBlock(&log, sizeof(log));
    // Send P2P
    pos.x = log.x;
    pos.y = log.y;
    pos.z = log.z;
    memcpy(&(p_reply.data[1]), &pos, sizeof(pos));
    radiolinkSendP2PPacketBroadcast(&p_reply);
    // Save my position
    myPreviousPos.x = log.x;
    myPreviousPos.y = log.y;
    myPreviousPos.z = log.z;

    vTaskDelay(M2T(1000));
  }
}

void p2pcallbackHandler(P2PPacket *p)
{
  if(p->data[0] != 0x12 && p->data[0] != 0xA2) return;

  if (receivedStartingPos == false)
  {
    // Get Data
    memcpy(&otherDronepreviousPos, &p->data[1], sizeof(otherDronepreviousPos));
    // Falsify
    receivedStartingPos = true;
    // No computation on received starting pos
    return;
  }

  Vec3 curPos;
  float curX = logGetFloat(idX);
  float curY = logGetFloat(idY);
  float curZ = logGetFloat(idZ);
  ledClearAll();

  // Get data
  memcpy(&curPos, &p->data[1], sizeof(curPos));

  // Comupte the other drone current pos with the previous
  otherDistance += sqrtf(pow(curPos.x - otherDronepreviousPos.x, 2) + pow(curPos.y - otherDronepreviousPos.y, 2) + pow(curPos.z - otherDronepreviousPos.z, 2));

  // Comupte my current pos with the previous
  myDistance += sqrtf(pow(curX - myPreviousPos.x, 2) + pow(curY - myPreviousPos.y, 2) + pow(curZ - myPreviousPos.z, 2));

  myDistance > otherDistance ? ledSetAll() : ledClearAll();
  consolePrintf("me previous x=%f, y=%f, z=%f\n", (double)myPreviousPos.x, (double)myPreviousPos.y, (double)myPreviousPos.z);
  consolePrintf("me now x=%f, y=%f, z=%f\n", (double)curX, (double)curY, (double)curZ);
  consolePrintf("other previous x=%f, y=%f, z=%f\n", (double)otherDronepreviousPos.x, (double)otherDronepreviousPos.y, (double)otherDronepreviousPos.z);
  consolePrintf("other now x=%f, y=%f, z=%f\n", (double)curPos.x, (double)curPos.y, (double)curPos.z);
  consolePrintf("myDistance=%f, otherDistance=%f \n", (double)myDistance, (double)otherDistance);
}

void sendFlightState(void* param)
{
    enum StateCode* curStateCode = ((enum StateCode*)((void**)param)[0]);
    FlightData* fd = ((FlightData*)((void**)param)[1]);
    float* angleRates[] = {&fd->position.yaw, &fd->position.pitch, &fd->position.roll};
    struct LogState ls;
    // ground station packet handler
    ls.handler = LOG_STATE;
    bool isCrashed = false;
    
    while(true)
    {
      isCrashed = strcmp(ls.curStateText,CRASHED_TEXT) == 0;
    
      for(int i=0; i < sizeof(StateTextList) / sizeof(StateTextList[0]) && !isCrashed; i++)
      {
        if(StateTextList[i].stateCode == *curStateCode)
        {
            strcpy(ls.curStateText, StateTextList[i].text);
            // found
            break;
        }
        strcpy(ls.curStateText, ON_MISSION_TEXT);
      }

      for (int i=0; i < sizeof(angleRates) / sizeof(angleRates[0]) && !isCrashed; i++)
      {
        if (*angleRates[i] > MAX_INCLINATION || *angleRates[i] < (-1.0f) * MAX_INCLINATION)
        {
          strcpy(ls.curStateText, CRASHED_TEXT);
          // consolePrintf("ACCIDENT");
          // if(i == 0)
          //   consolePrintf("y=");
          // else if(i == 1)
          //   consolePrintf("p="); 
          // else if(i == 2)
          //   consolePrintf("r=");

          consolePrintf("%f\n", (double)*angleRates[i]);  
        }

      }

      consolePrintf("state=%s\n", ls.curStateText);
      appchannelSendDataPacketBlock(&ls, sizeof(ls));
      vTaskDelay(M2T(1000));
    }
}

void sendBatteryLevel()
{
    struct LogBattery lb;
    // ground station packet handler
    lb.handler = BATTERY_LEVEL;
    lb.batteryLevel = getBatteryStatus();

    appchannelSendDataPacketBlock(&lb, sizeof(lb));
}

const static float myBatteryVoltage[10] =
{
  3.00, // 00%
  3.78, // 10%
  3.83, // 20%
  3.87, // 30%
  3.89, // 40%
  3.92, // 50%
  3.96, // 60%
  4.00, // 70%
  4.04, // 80%
  4.10  // 90%
};


const static float load[10] =
{
  3.00, // 00%
  3.78, // 10%
  3.83, // 20%
  3.87, // 30%
  3.89, // 40%
  3.92, // 50%
  3.96, // 60%
  4.00, // 70%
  4.04, // 80%
  4.10  // 90%
};


/**
 * Returns a number from 0 to 9 where 0 is completely discharged
 * and 9 is 90% charged.
 */
static int32_t pmBatteryChargeIdle(float voltage)
{
  int charge = 0;

  if (voltage < myBatteryVoltage[0])
  {
    return 0;
  }
  if (voltage > myBatteryVoltage[9])
  {
    return 9;
  }
  while (voltage >  myBatteryVoltage[charge])
  {
    charge++;
  }

  return charge;
}


static int32_t pmBatteryChargeLoad(float voltage)
{
  int charge = 0;

  if (voltage < load[0])
  {
    return 0;
  }
  if (voltage > load[9])
  {
    return 9;
  }
  while (voltage >  load[charge])
  {
    charge++;
  }

  return charge;
}


void batteryDropOn(void){
    isBatteryDrop = true;
}

void batteryDropOff(void){
    isBatteryDrop = false;
}

uint8_t getBatteryStatus(void){
  if(isBatteryDrop)
    return pmBatteryChargeIdle(pmGetBatteryVoltage()) *10;

  return pmBatteryChargeLoad(pmGetBatteryVoltage()) * 10;
}