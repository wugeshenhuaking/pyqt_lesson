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
 *      @file       b1793watch_bsp_mpuxxxx.c
 *      @brief      An I2C-based driver for MPUxxxx .
 *      @details    This driver currently works for the following devices:
 *                  MPU6050
 *                  MPU6500
 *                  MPU9150 
 *                  MPU9250
 * 
 */
#include "mpu6050_regs.h"
#include "b1793mpuxxxx.h"
#include "stdint.h"
#define offset_of(type, field) ( (unsigned int)&(((type *)(0))->field) )
#define container_of(ptr, type, field) (type *)((char *)ptr - offset_of(type, field))

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
static uint8_t pfcheck_whoami(bsp_mpuxxxx_t * container)
{
    uint8_t uwhoami = 0U;
    uint8_t ret = 0U;

    container->psiicdriver_interface->pfiicstart(0);

    container->psiicdriver_interface->pfiicsendbyte(MPU_ADDR<<1|1);
    
    if(0 != container->psiicdriver_interface->pfiicwaitack(1))
    {
        container->psiicdriver_interface->pfiicstop(1);
        return -1;
    }

    container->psiicdriver_interface->pfiicsendbyte(MPU_DEVICE_ID_REG);

    if(0 != container->psiicdriver_interface->pfiicwaitack(1))
    {
        container->psiicdriver_interface->pfiicstop(1);
        return -2;
    }
    
    container->psiicdriver_interface->pfiicstop(1);
    
    container->psiicdriver_interface->pfiicstart(1);

    uwhoami = container->psiicdriver_interface->pfiicsendbyte((MPU_ADDR<<1)|0);

    container->psiicdriver_interface->pfiicstop(1);

    return uwhoami;
}


static uint8_t pfstart_working(bsp_mpuxxxx_t * container)
{
	container->psiicdriver_interface->pfiicstart(0);
}

static uint8_t pfstop_working(bsp_mpuxxxx_t * container)
{
	container->psiicdriver_interface->pfiicstop(0);
}

static uint8_t pfcheck_status(bsp_mpuxxxx_t * container)
{
	
	  return (uint8_t) (container->psiicdriver_interface->pfreadReg(MPU_INT_STA_REG));
	
}

static uint8_t pfset_gyro_fsr(bsp_mpuxxxx_t * container , uint8_t fsr)
{
	  container->psiicdriver_interface->pfwriteReg(MPU_GYRO_CFG_REG,fsr<<3);
}

static uint8_t pfset_accel_fsr(bsp_mpuxxxx_t * container , uint8_t fsr)
{
	  container->psiicdriver_interface->pfwriteReg(MPU_ACCEL_CFG_REG,fsr<<3);
}

static uint8_t pfset_rate(bsp_mpuxxxx_t * container , uint8_t data)
{
	  container->psiicdriver_interface->pfwriteReg(MPU_SAMPLE_RATE_REG,data);
}

static uint8_t pfget_temperature(bsp_mpuxxxx_t * container , uint8_t fsr)
{
	uint8_t buf[2]; 
	short raw;
	float temp;
	container->psiicdriver_interface->pfiic_readlen(MPU_ADDR,MPU_TEMP_OUTH_REG,2,&buf); 
	raw=((uint16_t)buf[0]<<8)|buf[1];  
	temp=36.53+((double)raw)/340;  
	return temp*100;;
	
	container->psiicdriver_interface->pfwriteReg(MPU_ACCEL_CFG_REG,fsr<<3);
}

static uint8_t pfget_gyroscope(bsp_mpuxxxx_t * container , uint8_t fsr)
{
	  container->psiicdriver_interface->pfwriteReg(MPU_ACCEL_CFG_REG,fsr<<3);
}

static uint8_t pfget_accelerometer(bsp_mpuxxxx_t * container , uint8_t *ax,uint8_t *ay,uint8_t *az)
{
	     uint8_t buf[6],res;  
		res=container->psiicdriver_interface->pfiic_readlen(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,&buf);
		if(res==0)
		{
			*ax=((uint16_t)buf[0]<<8)|buf[1];  
			*ay=((uint16_t)buf[2]<<8)|buf[3];  
			*az=((uint16_t)buf[4]<<8)|buf[5];
		} 	
    return res;;
}

static uint8_t pfget_angles(bsp_mpuxxxx_t * container ,float * roll,float * pitch )
{
	uint8_t  ax,ay,az;
	container->pfget_accelerometer(&ax,&ay,&az);
	*pitch = -atanf(ax/sqrtf(ay*ay+az*az));
	*roll = atanf((float)ay/(float)az);
}
	
static uint8_t pfis_horizontal(bsp_mpuxxxx_t * container)
{
	  	float roll,pitch;
	container->pfget_angles(&roll,&pitch);
	if(roll<=0.50 && roll>=-0.50 && pitch<=0.50 && pitch>=-0.50)
	{return 1;}
	return 0;
}

static uint8_t pfmotion_init(bsp_mpuxxxx_t * container )
{
	container->psiicdriver_interface->pfwriteReg(MPU_MOTION_DET_REG,0x01);
	container->psiicdriver_interface->pfwriteReg(MPU_MOTION_DUR_REG,0x01);
	container->psiicdriver_interface->pfwriteReg(MPU_INTBP_CFG_REG,0X90);
	container->psiicdriver_interface->pfwriteReg(MPU_INT_EN_REG,0x40);
}


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
                                                    )
{
    bsp_mpuxxxx_t * ptemp = pbsp_mpuxxxx;
    uint8_t derive_id ;


    /*#part-1 System related functions initialization*/

    // IIC Initialization function begin
    iic_driver_interface_t iic_driver_interface = {NULL, NULL, NULL, NULL, NULL};
    
    iic_driver_interface.pfinit        = pfiic_init;
    iic_driver_interface.pfdeinit      = pfiic_deinit;
    iic_driver_interface.pfwriteReg    = pfiic_writebyte;
    iic_driver_interface.pfreadReg     = pfiic_read;
    iic_driver_interface.pfwriteBuf    = pfiic_writebuf;
	iic_driver_interface.pfiicsendbyte = pfiicsendbyte;
	iic_driver_interface.pfiicwaitack  = pfiicwaitack;
	iic_driver_interface.pfiicstop     = pfiicstop;
	iic_driver_interface.pfiic_readlen = pfiic_readlen;
	
    if (    iic_driver_interface.pfinit         != NULL &&
            iic_driver_interface.pfdeinit       != NULL &&
            iic_driver_interface.pfwriteReg     != NULL &&
            iic_driver_interface.pfreadReg      != NULL &&
            iic_driver_interface.pfwriteBuf     != NULL &&    
			iic_driver_interface.pfiicsendbyte	!= NULL && 								
            iic_driver_interface.pfiicwaitack   != NULL &&
            iic_driver_interface.pfiicstop      != NULL &&
			iic_driver_interface.pfiicstart     != NULL &&   
			iic_driver_interface.pfiic_readlen  != NULL      )
	
	{      
           return -1;                                   
        }                                     


    ptemp->psiicdriver_interface->pfinit = iic_driver_interface.pfinit;
    ptemp->psiicdriver_interface->pfdeinit = iic_driver_interface.pfdeinit;
    ptemp->psiicdriver_interface->pfwriteReg = iic_driver_interface.pfwriteReg;
    ptemp->psiicdriver_interface->pfreadReg = iic_driver_interface.pfreadReg;
    ptemp->psiicdriver_interface->pfwriteBuf = iic_driver_interface.pfwriteBuf;
	ptemp->psiicdriver_interface->pfiicsendbyte = iic_driver_interface.pfiicsendbyte;
	ptemp->psiicdriver_interface->pfiicwaitack = iic_driver_interface.pfiicwaitack;
	ptemp->psiicdriver_interface->pfiicstop = iic_driver_interface.pfiicstop;
	ptemp->psiicdriver_interface->pfiicstart = iic_driver_interface.pfiicstart;
	ptemp->psiicdriver_interface->pfiic_readlen = iic_driver_interface.pfiic_readlen;	
    // IIC Initialization function end


    interuption_interface_t interuption_interface = {NULL,NULL,NULL};
    
    interuption_interface.pfinit = pfint_init;
    interuption_interface.pfdeinit = pfint_deinit;
    interuption_interface.pfintcallback = pfint_callback;

    if (    interuption_interface.pfinit!= NULL         &&
            interuption_interface.pfdeinit!= NULL       &&
            interuption_interface.pfintcallback!= NULL     )
    {
        return -2;
    }

    ptemp->psinteruption_interface->pfinit = interuption_interface.pfinit;
    ptemp->psinteruption_interface->pfdeinit = interuption_interface.pfdeinit;
    ptemp->psinteruption_interface->pfintcallback = interuption_interface.pfintcallback;

    // Delay initialization functions
	system_interface_t system_interface = {NULL,NULL,NULL};

	system_interface.delay_init = delay_init;
	system_interface.delay_us 	= delay_us;
	system_interface.delay_ms 	= delay_ms;
	
	if (    system_interface.delay_init != NULL &&
			system_interface.delay_us   != NULL &&
			system_interface.delay_ms   != NULL       )
    {
        return -3;
    }
	
	ptemp->pssystem_interface->delay_init = system_interface.delay_init;
	ptemp->pssystem_interface->delay_us = system_interface.delay_us;
	ptemp->pssystem_interface->delay_ms = system_interface.delay_ms;

    if ( 0 != ptemp->psiicdriver_interface->pfinit())
    {
        return -4;
    }

    if ( MPU6050 == device_type)
    {
        if( 0 != ptemp->psiicdriver_interface->pfwriteReg(MPU_PWR_MGMT1_REG,0x80))
        {
            return -5; // reset mpu6050 failed
        }
		
		ptemp->pssystem_interface->delay_ms(1000);
		
		if( 0 != ptemp->psiicdriver_interface->pfwriteReg(MPU_PWR_MGMT1_REG,0x00))
        {
            return -6; 
        }
		
		if( 0 != ptemp->pfset_gyro_fsr(&ptemp,3))
        {
            return -7; 
        }
		
		if( 0 != ptemp->pfset_accel_fsr(&ptemp,2))
        {
            return -8; 
        }
		
		if( 0 != ptemp->pfset_rate(50))
        {
            return -9; 
        }
		
		if( 0 != ptemp->psiicdriver_interface->pfwriteReg(MPU_INT_EN_REG,0X00))
        {
            return -10; // reset mpu6050 failed
        }
		
		if( 0 != ptemp->psiicdriver_interface->pfwriteReg(MPU_USER_CTRL_REG,0X00))
        {
            return -11; // reset mpu6050 failed
        }
		
		if( 0 != ptemp->psiicdriver_interface->pfwriteReg(MPU_FIFO_EN_REG,0X00))
        {
            return -12; // reset mpu6050 failed
        }
		
		if( 0 != ptemp->psiicdriver_interface->pfwriteReg(MPU_INTBP_CFG_REG,0X80))
        {
            return -13; // reset mpu6050 failed
        }
		
		derive_id = ptemp->psiicdriver_interface->pfreadReg(MPU_DEVICE_ID_REG);
		
		if(MPU_ADDR == derive_id)
		{
			if( 0 != ptemp->psiicdriver_interface->pfwriteReg(MPU_PWR_MGMT1_REG,0X28))
			{
				return -14; // reset mpu6050 failed
			}
			
			if( 0 != ptemp->psiicdriver_interface->pfwriteReg(MPU_PWR_MGMT2_REG,0X87))
			{
				return -15; // reset mpu6050 failed
			}
			
			if( 0 != ptemp->pfset_rate(50))
			{
				return -16; 
			}
		}
		else
		{
			return -17;
		}
		
		ptemp->pfmotion_init();
		
		
		ptemp->psinteruption_interface->pfinit();
		
		
    }



    /*#part-2 internal functions initialization*/
    ptemp->pfcheck_whoami = pfcheck_whoami;

	ptemp->pfstart_working = pfstart_working;
	
	ptemp->pfstop_working = pfstop_working;
	
	ptemp->pfcheck_status = pfcheck_status;

	ptemp->pfset_gyro_fsr = pfset_gyro_fsr;
	
	ptemp->pfset_accel_fsr = pfset_accel_fsr;

	
	ptemp->pfset_rate = pfset_rate;
	
	ptemp->pfget_temperature = pfget_temperature;
	
	ptemp->pfget_gyroscope = pfget_gyroscope;
	
	ptemp->pfget_accelerometer = pfget_accelerometer;
	
	ptemp->pfget_angles = pfget_angles;
	
	ptemp->pfis_horizontal = pfis_horizontal;
	
	ptemp->pfmotion_init = pfmotion_init;
	
	
   return 0; // successfully initialized
    
}

