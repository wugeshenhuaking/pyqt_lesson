/**
 * @file ec_bsp_bl0942_driver.h
 * @brief BL0942驱动程序接口声明
 *
 * 本文件提供了BL0942电能计量芯片的驱动程序接口声明，包括初始化函数、数据读取函数等。
 * 使用这些接口可以方便地集成BL0942到电能计量相关的项目中。
 *
 * @version 1.0.0
 * @author TommyRickWu
 */
#ifndef __BSP_BL0942_DRIVER_H__
#define __BSP_BL0942_DRIVER_H__

/*************************** includes begin ***************************/
#include <stdio.h>
#include <stdint.h>

#include "ec_bsp_bl0942_reg.h"
/*************************** includes end ***************************/



/*************************** macros begin ***************************/
#define OS_SUPPORTING
/*************************** macros end ***************************/



/*************************** typdefs begin ***************************/

/*         interface from core library        */
// i2c interface
typedef struct{
  int8_t (*pf_init)(void *);
  int8_t (*pf_deinit)(void *);
  int8_t (*pf_read)(uint8_t addr, uint8_t *data, uint8_t len);
  int8_t (*pf_write)(uint8_t addr, uint8_t *data, uint8_t len);
}iic_driver_interface_t;

// spi interface
typedef struct{
  int8_t (*pf_init)(void);
  int8_t (*pf_deinit)(void);
  int8_t (*pf_read)(uint8_t addr, uint8_t *data, uint8_t len);
  int8_t (*pf_write)(uint8_t addr, uint8_t *data, uint8_t len);
}spi_driver_interface_t;

// uart interface
typedef struct{
  int8_t (*pf_init)(void);
  int8_t (*pf_deinit)(void);
  int8_t (*pf_read)(uint8_t addr, uint8_t *data, uint8_t len);
  int8_t (*pf_write)(uint8_t addr, uint8_t *data, uint8_t len);
}uart_driver_interface_t;

// 2.timebase interface
typedef struct{
  int32_t (*pf_get_tick)(void);
}timebase_interface_t;

/*         interface from rtos         */
#ifdef OS_SUPPORTING
typedef struct{
  void (*pf_rtos_yield)(void);
}yield_interface_t;
#endif

/*         interface for rtos              */
typedef struct{
  // 外部提供的接口
  iic_driver_interface_t *p_iic_driver_interface;
  spi_driver_interface_t *p_spi_driver_interface;
  uart_driver_interface_t *p_uart_driver_interface;
  timebase_interface_t *p_timebase_interface;
  yield_interface_t *p_yield_interface;

  // 通过构造函数来挂在外部需要的接口给bl0942_instance
  int8_t (*pf_instance)(
      void *bl0942_instance,
      iic_driver_interface_t *iic_instance,
      spi_driver_interface_t *spi_instance,
      uart_driver_interface_t *uart_instance,
      timebase_interface_t *timebase_instance,
      yield_interface_t *yield_instance
  );
  // bl0942_hal 实现的HAL接口
  int8_t (*pf_init)(void *bl0942_instance);
  int8_t (*pf_deinit)(void *bl0942_instance);
  int8_t (*pf_read_all_elec_data)(void *bl0942_instance, uint8_t *data, uint8_t len);
}bsp_bl0942_driver_t;
/*************************** typdefs end ***************************/

/*************************** function declarations begin ***************************/
int8_t bl0942_inst(
    bsp_bl0942_driver_t *bl0942_instance,
    iic_driver_interface_t *iic_instance,
    spi_driver_interface_t *spi_instance,
    uart_driver_interface_t *uart_instance,
    timebase_interface_t *timebase_instance,
    yield_interface_t *yield_instance);

int8_t bl0942_init(bsp_bl0942_driver_t *bl0942_instance);     // 初始化函数
int8_t bl0942_deinit(bsp_bl0942_driver_t *bl0942_instance);   // 去初始化函数
int8_t bl0942_read_all_elec_data(bsp_bl0942_driver_t *bl0942_instance, uint8_t *data, uint8_t len); // 读取全电参数数据包

/*************************** function declarations end ***************************/
#endif
