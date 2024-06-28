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
BL0942 Uartͨ�ŷ�ʽ��
ָ���Ĵ�����ַ��д��ʽ��
ȫ��������ݰ�ͨ�Ŵ���
���ܱ�����Ҫ���ݲ��õ�EEPROM���д洢��������δʵ��

BL0942 SPIͨ�ŷ�ʽ��
ָ���Ĵ�����ַ��д��ʽ��
******************************************/
// оƬϵͳ�������
#define IC_Vol_Ref      1.218    // оƬ�ڲ��ο���ѹ����λV

// оƬ������Ĵ�����ַ
#define Addr_I_WAVE									0x01					 //����ͨ������
#define Addr_V_WAVE									0x02					//��ѹͨ������
#define Addr_I_RMS									0x03					 //������Чֵ
#define Addr_V_RMS									0x04					//��ѹ��Чֵ
#define Addr_I_FAST_RMS							0x05					 //����������Чֵ*
#define Addr_WATT										0x06			 		 //�й�����
#define Addr_CF_CNT									0x07					 //�й������������
#define	Addr_FREQ										0x08					 //��ƵƵ��
#define Addr_STATUS									0x09					 //״̬
#define Addr_VERSION								0x0F					//�汾
//�û������Ĵ�����ַ
#define Addr_I_CHOS									0x11						//����ͨ��ֱ��ƫ��У��
#define Addr_I_RMSOS								0x12						//����ͨ����ЧֵС�ź�У��
#define Addr_WA_CREEP								0x14						//�й����ʷ�Ǳ��ֵ
#define Addr_FAST_RMS_TH						0x15						//
#define Addr_FAST_RMS_CYC						0x16
#define Addr_FREQ_CYC								0x17
#define Addr_MASK										0x18
#define Addr_MODE										0x19
#define Addr_GAIN_CR								0x1A
#define Addr_SOFT_RESET							0x1C						//��λ
#define Addr_WRPROT									0x1D						//�û�д��������
#define Addr_ALL_ELEC_DATA          0xaa            // ��ȡȫ�����ݰ��ĵ�ַ

/*
    ע�� BL0940�Ķ������ֽڹ̶�Ϊ0x58+ICAddr��д�����ֽڹ̶�Ϊ0xA8+ICAddr��SOP10��װоƬ��IC_Addr��ַ�̶�Ϊ0
        BL0942 TSSOP14��װ����ַѡ��ܽţ������A1~A2��ַѡ��ܽŵĵ�ƽ���������ֽڣ����Խ��ж������ͨ��   
*/
#define BL0942_Addr_R 0x58
#define BL0942_Addr_w 0xA8

//����У׼оƬ���������1%���ڣ���Χ�������ȿ���1%���ڣ�����ͬһϵ�������� EEPROM�������					
// ����ת��ϵ��=3537*1��ŷ*0.51K*1000/(1.218*1.218)/(390K*5+0.51K)=623.39    ,�������㿼�ǣ��Ŵ�10����*10=6234
// ����ת��ϵ��=305978*1��ŷ/1.218=251213���������㿼�ǣ���ֹ�����������С10����/10=25121																				
// ��ѹת��ϵ��=73989*0.51K*1000/1.218/(390K*5+0.51K)=15883										
// ����ϵ��=3600000*Power_K/1638.4/256=5350.6,��Ӧ��1�ȵ���������
// ��������1��ŷ�����������ѹ����390K*5+0.51K���з�ѹ��ʵ�ʲ��Է��ֵ������ƫ�����΢��
// BL0942�����棬����ֱ����Ƭ�Ͻ����( ̨�����MS121WF100NT4E  )��ʵ�ʲ�����1��ŷƫС��Լ0.93��ŷ

/* wyw 2024_06_22 103310 �����»�����  ��������2.2ŷķ*/
#define Power_K						446		      //	1034					
#define Current_K					276335	    //	640594		,630216	 		
#define Voltage_K					10250 	    //	10325.33662		��10287,10300
#define Energy_K					3827		    //  ��ʽ�������ֵ8870.99

#endif
