/*
 * user_main.c
 *
 *  Created on: 30-Mar-2021
 *      Author: harsh
 */

#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"

//driver libs
#include "driver/http.h"
#include "driver/uart.h"

//user includes
#include "user_config.h"
#include "user_espconn.h"
#include "user_wifi.h"
#include "user_timer.h"

//UART
#define UART_BAUD								115200

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
//Flash Size MACRO
#define SPI_FLASH_SIZE_MAP						4

/* user must define this partition */
/* mandatory */
#define SYSTEM_PARTITION_RF_CAL_SZ                0x1000	// 4KB
#define SYSTEM_PARTITION_PHY_DATA_SZ              0x1000	// 4KB
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_SZ      0x3000	// 12KB

#define SPI_FLASH_SIZE							0x400000	// 4MB

#define SYSTEM_PARTITION_RF_CAL_ADDR                SPI_FLASH_SIZE - SYSTEM_PARTITION_SYSTEM_PARAMETER_SZ - SYSTEM_PARTITION_PHY_DATA_SZ - SYSTEM_PARTITION_RF_CAL_SZ
#define SYSTEM_PARTITION_PHY_DATA_ADDR              SPI_FLASH_SIZE - SYSTEM_PARTITION_SYSTEM_PARAMETER_SZ - SYSTEM_PARTITION_PHY_DATA_SZ
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR      SPI_FLASH_SIZE - SYSTEM_PARTITION_SYSTEM_PARAMETER_SZ


static const partition_item_t at_partition_table[] = {
		{SYSTEM_PARTITION_RF_CAL, SYSTEM_PARTITION_RF_CAL_ADDR, SYSTEM_PARTITION_RF_CAL_SZ},
		{SYSTEM_PARTITION_PHY_DATA, SYSTEM_PARTITION_PHY_DATA_ADDR, SYSTEM_PARTITION_PHY_DATA_SZ},
		{SYSTEM_PARTITION_SYSTEM_PARAMETER, SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR, SYSTEM_PARTITION_SYSTEM_PARAMETER_SZ}
};
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/

void ICACHE_FLASH_ATTR InitUART(void){
	/**** Initializing UART BAUD ****/
	uart_init(UART_BAUD, UART_BAUD);
	os_delay_us(500);

	//clear screen and moves Cursor to left
	os_printf("\033[2J");
	ESP_DEBUG("ESP8266, User-Init");
}

void ICACHE_FLASH_ATTR espUserInit(void){
	/**** Initialize UART for logging ****/
	InitUART();

	/**** Init webserver ****/
	ESP_DEBUG("Initializing WebServer");
	InitWebServer();

	/**** Init Wifi ****/
	ESP_DEBUG("Initializing Wifi");
	InitWifi();
}

void ICACHE_FLASH_ATTR user_pre_init(void)
{
    if(!system_partition_table_regist(at_partition_table, sizeof(at_partition_table)/sizeof(at_partition_table[0]),SPI_FLASH_SIZE_MAP)) {
		os_printf("system_partition_table_regist fail\r\n");
		while(1);
	}
}

void ICACHE_FLASH_ATTR user_init(void)
{
	system_init_done_cb(espUserInit);

	//register sleep timer
	InitSleepTimer();
}

