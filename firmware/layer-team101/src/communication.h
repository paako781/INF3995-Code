#ifndef COMMAND_H
#define COMMAND_H

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "app.h"

#include "FreeRTOS.h"
#include "task.h"
#include "app_channel.h"
#include "state.h"
#include "semphr.h"
#include "radiolink.h"


static const int NO_CMD_RECEIVED = -1;
static char CRASHED_TEXT[25] = "Accident";
static char ON_MISSION_TEXT[25] = "En mission";

static P2PPacket p_reply;

typedef struct
{
  char *cmdTxt;
  enum StateCode cmdId;
} TextCommand;

TextCommand commandList[] = {{"ID", IDENTIFY}, {"SM", TAKEOFF}, {"TM", LAND}, {"RB", END_MISSION}};

typedef struct 
{
  enum StateCode stateCode;
  char text[25];
} StateText;

StateText StateTextList[] = {{IDLE, "En attente"}, {TAKEOFF, "Debut de la mission"}, {LAND, "Fin de la mission"}};

enum Handler
{
  LOG_DATA,
  LOG_STATE,
  BATTERY_LEVEL
};

struct LogData
{
  enum Handler handler;
  uint16_t front;
  uint16_t back;
  uint16_t left;
  uint16_t right;
  uint16_t up;
  float x;
  float y;
  float z;
} __attribute__((packed));

struct LogState
{
  enum Handler handler;
  char curStateText[25];
} __attribute__((packed));

struct LogBattery
{
  enum Handler handler;
  uint8_t batteryLevel;
} __attribute__((packed));

typedef struct Vec3Tag 
{
  float x;
  float y;
  float z;
} Vec3;

float myDistance = 0.0f;
float otherDistance = 0.0f;
Vec3 otherDronepreviousPos;
Vec3 myPreviousPos;
bool receivedStartingPos = false;

void initTasks(volatile FlightData*, enum StateCode*);
int commandReceiver(void);
void sendFlightData(void*);
void sendFlightState(void*);

bool isBatteryDrop = false;
void batteryDropOn(void);
void batteryDropOff(void);
static int32_t pmBatteryChargeIdle(float voltage);
static int32_t pmBatteryChargeLoad(float voltage);
uint8_t getBatteryStatus(void);

#endif