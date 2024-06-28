/*** 
 * @Author: TommyRickWu
 * @Date: 2024-06-27 07:44:58
 * @LastEditors: TommyRickWu
 * @LastEditTime: 2024-06-27 22:07:59
 * @Description: 
 * @
 */
#ifndef __BSP_BL0942_REG_H__
#define __BSP_BL0942_REG_H__


#include <stdint.h>
/****************************************
BL0942 Uart通信方式，
指定寄存器地址读写方式；
全电参数数据包通信代码
电能保存需要根据采用的EEPROM进行存储，代码暂未实现

BL0942 SPI通信方式，
指定寄存器地址读写方式；
******************************************/
// 芯片系统级别参数
#define IC_Vol_Ref      1.218    // 芯片内部参考电压，单位V

// 芯片电参数寄存器地址
#define Addr_I_WAVE									0x01					 //电流通道波形
#define Addr_V_WAVE									0x02					//电压通道波形
#define Addr_I_RMS									0x03					 //电流有效值
#define Addr_V_RMS									0x04					//电压有效值
#define Addr_I_FAST_RMS							0x05					 //电流快速有效值*
#define Addr_WATT										0x06			 		 //有功功率
#define Addr_CF_CNT									0x07					 //有功电能脉冲计数
#define	Addr_FREQ										0x08					 //工频频率
#define Addr_STATUS									0x09					 //状态
#define Addr_VERSION								0x0F					//版本
//用户操作寄存器地址
#define Addr_I_CHOS									0x11						//电流通道直流偏置校正
#define Addr_I_RMSOS								0x12						//电流通道有效值小信号校正
#define Addr_WA_CREEP								0x14						//有功功率防潜阈值
#define Addr_FAST_RMS_TH						0x15						//
#define Addr_FAST_RMS_CYC						0x16
#define Addr_FREQ_CYC								0x17
#define Addr_MASK										0x18
#define Addr_MODE										0x19
#define Addr_GAIN_CR								0x1A
#define Addr_SOFT_RESET							0x1C						//软复位
#define Addr_WRPROT									0x1D						//用户写保护设置
#define Addr_ALL_ELEC_DATA          0xaa            // 读取全电数据包的地址

/*
    注意 BL0940的读命令字节固定为0x58+ICAddr，写命令字节固定为0xA8+ICAddr；SOP10封装芯片的IC_Addr地址固定为0
        BL0942 TSSOP14封装带地址选择管脚，需根据A1~A2地址选择管脚的电平配置命令字节，可以进行多机并联通信   
*/
#define BL0942_Addr_R 0x58
#define BL0942_Addr_w 0xA8

//出厂校准芯片，增益控制1%以内，外围器件精度控制1%以内，采用同一系数，不用 EEPROM保存参数					
// 功率转换系数=3537*1毫欧*0.51K*1000/(1.218*1.218)/(390K*5+0.51K)=623.39    ,整数运算考虑，放大10倍，*10=6234
// 电流转换系数=305978*1毫欧/1.218=251213，整数运算考虑，防止运算溢出，缩小10倍，/10=25121																				
// 电压转换系数=73989*0.51K*1000/1.218/(390K*5+0.51K)=15883										
// 电能系数=3600000*Power_K/1638.4/256=5350.6,对应于1度电的脉冲计数
// 电流采用1毫欧电阻采样，电压采用390K*5+0.51K进行分压，实际测试发现电阻存在偏差，进行微调
// BL0942评估版，立创直接贴片合金电阻( 台湾厚声MS121WF100NT4E  )，实际测量比1毫欧偏小，约0.93毫欧

/* wyw 2024_06_22 103310 适配新互感器  采样电阻2.2欧姆*/
#define Power_K						446		      //	1034					
#define Current_K					276335	    //	640594		,630216	 		
#define Voltage_K					10250 	    //	10325.33662		，10287,10300
#define Energy_K					3827		    //  公式中算出的值8870.99

#endif
