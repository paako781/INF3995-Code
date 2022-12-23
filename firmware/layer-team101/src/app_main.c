/**
 * ,---------,       ____  _ __
 * |  ,-^-,  |      / __ )(_) /_______________ _____  ___
 * | (  O  ) |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * | / ,--´  |    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *    +------`   /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2019 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * app_main.c - Team101's App layer
 */
#include <stdio.h>
#include <stdlib.h>
#include "app.h"

#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "communication.c"
#include "console.h"
#include "sensor.c"
#include "state.c"
#include "radiolink.h"
#include "configblock.h"

void appMain()
{
  p_reply.port = 0x00;
  p_reply.size = sizeof(myPreviousPos) + 1;
  uint64_t address = configblockGetRadioAddress();
  uint8_t my_id =(uint8_t)((address) & 0x00000000ff);
  p_reply.data[0]=my_id;
  enum StateCode curStateCode = IDLE;
  enum ReturnCode returnCode;
  volatile FlightData fd;
  computedPos.x = 0.0f;
  computedPos.y = 0.0f;
  computedPos.uVec.x = 0.0f;
  computedPos.uVec.y = 1.0f;
  computedPos.uVec.heading = 90.0f;
  computedPos.theta = 90.0f;

  initFlightData();
  ledInit();
  initTasks(&fd, &curStateCode);
  p2pRegisterCB(p2pcallbackHandler);

  vTaskDelay(M2T(2500));

  while (true)
  {
    // Listen for commands from the dongle
    int cmd_id = commandReceiver();

    if (cmd_id != NO_CMD_RECEIVED)
      curStateCode = cmd_id;

    // Get the variables of sensors
    updateFlightData(&fd);

    // Battery level
    sendBatteryLevel();

    consolePrintf("main : x=%f y=%f ux=%f uy=%f head=%f\n", (double)computedPos.x, (double)computedPos.y, (double)computedPos.uVec.x, (double)computedPos.uVec.y, (double)computedPos.uVec.heading);

    // Compute current state
    returnCode = state[curStateCode](&fd);
    
    // Set next state
    curStateCode = lookupTransitions(curStateCode, returnCode);

    vTaskDelay(M2T(300));
  }
}
