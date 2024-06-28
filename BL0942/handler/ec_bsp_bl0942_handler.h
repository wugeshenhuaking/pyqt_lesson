#ifndef __BSP_BL0942_HANDLER_H__
#define __BSP_BL0942_HANDLER_H__

#include <stdint.h>
#include <stdio.h>

#include "..\hal_driver\ec_bsp_bl0942_driver.h"



/********************* external API functions declearion begin*********************/
/*** 
 * @brief 
 * @param {void} *args
 * @return {*}
 * @note
 */
void elec_meter_sensor_handler_thread(void *args);    // 被操作系统调用

/*** 
 * @brief 
 * @param {uint8_t} event
 * @param {void} *args
 * @return {*}
 */
int8_t elec_meter_event_handler_send(uint8_t event, void *args);    // 被操作系统调用

/********************* external API functions declearion end*********************/

#endif
