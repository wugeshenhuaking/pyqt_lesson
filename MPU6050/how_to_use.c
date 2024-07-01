#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include ".\hal_driver\b1793mpuxxxx.h"
#include ".\handler\bsp_mpu6050.h"
#include "..\I2C_HAL_DRIVER\i2c_hal_driver.h"

int main(void)
{
  iic_bus_t iic_bus = {
    .IIC_SDA_PORT = GPIOB,
    .IIC_SCL_PORT = GPIOB,
    .IIC_SDA_PIN = GPIO_PIN_7,
    .IIC_SCL_PIN = GPIO_PIN_6
  };

  iic_driver_interface_t iic_driver = {
    .pf_init = IICInit,
    .pf_deinit = NULL,
    .pf_read = IIC_Read_One_Byte,
    .pf_write = IIC_Write_One_Byte
  };

  IICInit(&iic_bus);

}