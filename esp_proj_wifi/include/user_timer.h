/*
 * user_timer.h
 *
 *  Created on: 13-Jun-2021
 *      Author: harsh
 */

#ifndef INCLUDE_USER_TIMER_H_
#define INCLUDE_USER_TIMER_H_

#include "c_types.h"

//uncomment for Debug Log
#define ESP_TIMER_LOGGER

#define SLEEP_DURATION				60 //300	//in seconds
#define SLEEP_AFTER					30	//in seconds

// API's

/*******************************************************************************************
 * FunctionName	:  InitSleepTimer
 * Description	:  Initializes timer which calls Deep sleep function after x amount of time
 * Return		:  bool, true if successful,
 * 						 false if failed
 ******************************************************************************************/
bool ICACHE_FLASH_ATTR InitSleepTimer(void);

/*******************************************************************************************
 * FunctionName	:  ArmSleepTimer
 * Description	:  Arms timer that calls sleep timer.
 * Return		:  bool, true if successful,
 * 						 false if failed
 ******************************************************************************************/
bool ICACHE_FLASH_ATTR ArmSleepTimer(void);

/*******************************************************************************************
 * FunctionName	:  DisarmSleepTimer
 * Description	:  Disarms timer that calls sleep timer.
 * Return		:  bool, true if successful,
 * 						 false if failed
 ******************************************************************************************/
bool ICACHE_FLASH_ATTR DisarmSleepTimer(void);

#endif /* INCLUDE_USER_TIMER_H_ */
