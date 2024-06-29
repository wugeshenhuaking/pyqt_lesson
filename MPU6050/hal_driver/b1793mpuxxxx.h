/*
 * Copyright (C) 2024 EternalChip Co.£¬Ltd. or its affiliates.
 * 
 * All Rights Reserved.
 * 
 * File name: b1793watch_bsp_mpuxxxx.h
 * 
 *  @addtogroup  BSP Layer
 *  @brief       Hardware drivers to communicate with MPUxxxx sensors via I2C.
 *
 *  @{
 *      @file       b1793watch_bsp_mpuxxxx.h
 *      @brief      An I2C-based driver for MPUxxxx .
 *      @details    This driver currently works for the following devices:
 *                  MPU6050
 *                  MPU6500
 *                  MPU9150 
 *                  MPU9250
 * 
 */
#ifndef __B1793WATCH_BSP_MPUXXXX_H__

#define __B1793WATCH_BSP_MPUXXXX_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "math.h"

// Define the related functions about common IIC driver
typedef struct {
    int8_t (*pfinit)(void);
    int8_t (*pfdeinit)(void);
    int8_t (*pfwriteReg)(uint8_t, uint8_t);
    int8_t (*pfreadReg)(uint8_t);
    int8_t (*pfwriteBuf)(uint8_t, uint8_t *, uint16_t);
	int8_t (*pfiicsendbyte)(uint8_t);
	int8_t (*pfiicwaitack)(uint8_t);
	int8_t (*pfiicstop)(uint8_t);
	int8_t (*pfiicstart)(uint8_t);
	int8_t (*pfiic_readlen)(uint8_t,uint8_t,uint8_t,uint8_t(*)[]);
} iic_driver_interface_t;

// Define the related functions about interuption communication
typedef struct {
    int8_t (*pfinit)(void);
    int8_t (*pfdeinit)(void);
    int8_t (*pfintcallback)(void *(pfcallback_after_data_available)(void));
} interuption_interface_t;


typedef struct 
{
	void (*delay_init)(void);
	void (*delay_us)(uint32_t nus);
	void (*delay_ms)(uint32_t nms);
}system_interface_t;

typedef enum {
    MPU6050,
    MPU6500,
    MPU9150,
    MPU9250,
}mpuxxxx_type_t;

// Define the related functions about MPUxxxx in the structure type
typedef struct {

    iic_driver_interface_t *psiicdriver_interface;
    interuption_interface_t *psinteruption_interface;
	system_interface_t		*pssystem_interface;
	
    uint8_t (*pfinit)(void * pbsp_mpuxxxx,
                        const mpuxxxx_type_t device_type,
                                                        // IIC Initialization function begin
                        int8_t (*pfiic_init)(void), //The iic initialize function
                        int8_t (*pfiic_deinit)(void),  //The iic deinit function
                        int8_t (*pfiic_writebyte)(uint8_t, uint8_t ), //The iic write function
                        int8_t (*pfiic_read)(uint8_t), //The iic read function
                        int8_t (*pfiic_writebuf)(uint8_t, uint8_t *, uint16_t), //The iic read function
						int8_t (*pfiicsendbyte)(uint8_t),
						int8_t (*pfiicwaitack)(uint8_t),
						int8_t (*pfiicstop)(uint8_t),
						int8_t (*pfiicstart)(uint8_t),
						int8_t (*pfiic_readlen)(uint8_t,uint8_t,uint8_t,uint8_t*), //The iic read function
                                                        // IIC Initialization function end

                                                        // interuption_interface function begin
                        int8_t (*pfint_init)(void),
                        int8_t (*pfint_deinit)(void),
                        int8_t (*pfint_callback)(void *(pfcallback_after_data_available)(void)),
                                                        // interuption_interface function end

														// system_interface function begin
						void (*delay_init)(void),
					    void (*delay_us)(uint32_t nus),
						void (*delay_ms)(uint32_t nms)									
														// system_interface function end
                                                       ); // IIC Initialization function); // Pointer to initialization function
    int8_t (*pfdeinit)(void); // Pointer to de-initialization function
    int8_t (*pfstart_working)(void*); // Pointer to start working function
    int8_t (*pfstop_working)(void*); // Pointer to stop working function
    uint8_t (*pfcheck_status)(void*); // Pointer to stop working function
    uint8_t (*pfcheck_whoami)(void* container); // Pointer to get id function
    uint8_t (*pfset_gyro_fsr)(void* container, uint8_t); // Pointer to set_gyro_fsr function
    uint8_t (*pfset_accel_fsr)(void* container ,uint8_t); // Pointer to set_accel_fsr function
    uint8_t (*pfset_lpf)(uint8_t); // Pointer to set_lpf function
    uint8_t (*pfset_rate)(uint8_t);// Pointer to set_rate function
	uint8_t (*pfget_temperature)(void);// Pointer to get_temperature function
	uint8_t (*pfget_gyroscope)(uint8_t *,uint8_t *,uint8_t *);// Pointer to get_gyroscope function
	uint8_t (*pfget_accelerometer)(uint8_t *,uint8_t *,uint8_t *);// Pointer to get_accelerometer function
	uint8_t (*pfget_angles)(float * ,float * );// Pointer to get_angles function
	uint8_t (*pfis_horizontal)(void);// Pointer to horizontal test function
	void 	(*pfmotion_init)(void);// Pointer to motion initialization function



} bsp_mpuxxxx_t;




/**
 * @brief Used for initialize MPUxxxx
 *
 * @param[in] pbsp_mpuxxxx : the instance of driver class.
 * 
 * 
 * @retvar 0                          init successfully
 * @retvar -1                         there is NULL pointer inside iic init functions
 * @retvar -2                         there is NULL pointer inside int init functions
 * @retvar -3                         there is problem inside iic init functions refered by user       
 * */
int8_t bsp_mpuxxxx_init(void * pbsp_mpuxxxx,
                        const mpuxxxx_type_t device_type,
                                                        // IIC Initialization function begin
                        int8_t (*pfiic_init)(void), //The iic initialize function
                        int8_t (*pfiic_deinit)(void),  //The iic deinit function
                        int8_t (*pfiic_writebyte)(uint8_t, uint8_t ), //The iic write function
                        int8_t (*pfiic_read)(uint8_t), //The iic read function
                        int8_t (*pfiic_writebuf)(uint8_t, uint8_t *, uint16_t), //The iic read function
						int8_t (*pfiicsendbyte)(uint8_t),
						int8_t (*pfiicwaitack)(uint8_t),
						int8_t (*pfiicstop)(uint8_t),
						int8_t (*pfiicstart)(uint8_t),
						int8_t (*pfiic_readlen)(uint8_t,uint8_t,uint8_t,uint8_t), //The iic read function
                                                        // IIC Initialization function end

                                                        // interuption_interface function begin
                        int8_t (*pfint_init)(void),
                        int8_t (*pfint_deinit)(void),
                        int8_t (*pfint_callback)(void *(pfcallback_after_data_available)(void)),
                                                        // interuption_interface function end

														// system_interface function begin
						void (*delay_init)(void),
					    void (*delay_us)(uint32_t nus),
						void (*delay_ms)(uint32_t nms)									
														// system_interface function end
                                                    );


static uint8_t pfcheck_whoami(bsp_mpuxxxx_t * container);

#endif /* __B1793WATCH_BSP_MPU6050_H */