/*
 * user_main.c
 *
 *  Created on: 30-Mar-2021
 *      Author: harsh
 */

//system includes
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"

//driver libs
#include "driver/uart.h"

//UART
#define UART_BAUD								115200

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

static os_timer_t osTimer;

void blinky(){
	if(GPIO_INPUT_GET(GPIO_ID_PIN(2)) == 1){
		GPIO_OUTPUT_SET(GPIO_ID_PIN(2), 0);
	}
	else{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(2), 1);
	}
}

void myapp(){
	uart_init(UART_BAUD, UART_BAUD);
	os_printf("\033[2J");
	os_printf("Hallo \r\n");
	gpio_init();
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U,FUNC_GPIO2);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(2), 0);
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
	system_init_done_cb(myapp);
	os_timer_setfn(&osTimer, (os_timer_func_t*) blinky, NULL);
	os_timer_arm(&osTimer, 1000, true);
}

