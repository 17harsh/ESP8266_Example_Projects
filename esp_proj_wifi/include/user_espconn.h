/*
 * user_espconn.h
 *
 *  Created on: 07-Jun-2021
 *      Author: harsh
 */

#ifndef INCLUDE_USER_ESPCONN_H_
#define INCLUDE_USER_ESPCONN_H_

#include "c_types.h"

//uncomment for log messages
#define ESP_ESPCONN_LOGGER

#define TCP_LOCAL_PORT		80

// APIs

/*******************************************************************************************
 * FunctionName	:  InitWebServer
 * Description	:  Initializes ESP Web Server
 * Return		:  0 if successful, else failed
 ******************************************************************************************/
sint8 ICACHE_FLASH_ATTR InitWebServer(void);

/*******************************************************************************************
 * FunctionName	:  StartWebServer
 * Description	:  Starts the Web Server
 * Return		:  0 if successful, else failed
 ******************************************************************************************/
sint8 ICACHE_FLASH_ATTR StartWebServer(void);

/*******************************************************************************************
 * FunctionName	:  StopWebServer
 * Description	:  Stops the Web Server
 * Return		:  0 if successful, else failed
 ******************************************************************************************/
sint8 ICACHE_FLASH_ATTR StopWebServer(void);


#endif /* INCLUDE_USER_ESPCONN_H_ */
