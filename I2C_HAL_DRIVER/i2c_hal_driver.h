#ifndef __I2C_HAL_DRIVER_H__
#define __I2C_HAL_DRIVER_H__

#include <stdint.h>
#include <stdio.h>

#include "stm32f1xx_hal.h"

typedef struct {
    GPIO_TypeDef *IIC_SDA_PORT;
    GPIO_TypeDef *IIC_SCL_PORT;
    uint16_t IIC_SDA_PIN;
    uint16_t IIC_SCL_PIN;
    // void (*CLK_ENABLE)(void);
} iic_bus_t;

void IICStart(iic_bus_t *bus);
void IICStop(iic_bus_t *bus);
uint8_t IICWaitAck(iic_bus_t *bus);
void IICSendAck(iic_bus_t *bus);
void IICSendNotAck(iic_bus_t *bus);
void IICSendByte(iic_bus_t *bus, uint8_tcSendByte);
uint8_t IICReceiveByte(iic_bus_t *bus);
void IICInit(iic_bus_t *bus);

uint8_t IIC_Write_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t data);
uint8_t IIC_Write_Multi_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg,uint8_t length,uint8_t buff[]);
uint8_t IIC_Read_One_Byte(iic_bus_t *bus, uint8_t daddr,uint8_t reg);
uint8_t A_READ_MULTI_BYTE(iic_bus_t *bus, uint8_t daddr, uint8_t reg, uint8_t length, uint8_t buff[]);

#endif
