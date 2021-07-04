/*
 * user_timer.c
 *
 *  Created on: 13-Jun-2021
 *      Author: harsh
 */

#include "user_timer.h"

//system includes
#include "osapi.h"
#include "user_interface.h"

//Set-Up Debugging Macros
#ifndef ESP_TIMER_LOGGER
	#define TIMER_DEBUG(message)					do {} while(0)
	#define TIMER_DEBUG_ARGS(message, args...)		do {} while(0)
#else
	#define TIMER_DEBUG(message)					do {os_printf("[TIMER_DEBUG] %s", message); os_printf("\r\n");} while(0)
	#define TIMER_DEBUG_ARGS(message, args...)		do {os_printf("[TIMER_DEBUG] "); os_printf(message, args); os_printf("\r\n");} while(0)
#endif

// static variables
static os_timer_t osTimer;

/******** Function Definitions ********/

/*******************************************************************************************
 * FunctionName	:  _DeepSleep
 * Description	:  Timer function callback. It puts the processor in Deep Sleep.
 ******************************************************************************************/
void ICACHE_FLASH_ATTR _DeepSleep(void){
	system_deep_sleep(SLEEP_DURATION*1000*1000);	//in seconds
}

/*******************************************************************************************
 * FunctionName	:  ArmSleepTimer
 * Description	:  Arms timer that calls sleep timer.
 * Return		:  bool, true if successful,
 * 						 false if failed
 ******************************************************************************************/
bool ICACHE_FLASH_ATTR ArmSleepTimer(void){
	//arm sleep timer
	os_timer_arm(&osTimer, SLEEP_AFTER*1000, false);	//in seconds
	return true;
}

/*******************************************************************************************
 * FunctionName	:  DisarmSleepTimer
 * Description	:  Disarms timer that calls sleep timer.
 * Return		:  bool, true if successful,
 * 						 false if failed
 ******************************************************************************************/
bool ICACHE_FLASH_ATTR DisarmSleepTimer(void){
	//disarm sleep timer
	os_timer_disarm(&osTimer);
	return true;
}

/*******************************************************************************************
 * FunctionName	:  InitSleepTimer
 * Description	:  Initializes timer which calls Deep sleep function after x amount of time.
 * Return		:  bool, true if successful,
 * 						 false if failed
 ******************************************************************************************/
bool ICACHE_FLASH_ATTR InitSleepTimer(void){
	//register timer for deep sleep
	os_timer_setfn(&osTimer, (os_timer_func_t*) _DeepSleep, NULL);
	return true;
}
