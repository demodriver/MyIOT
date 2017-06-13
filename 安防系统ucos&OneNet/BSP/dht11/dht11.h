#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   
#include "includes.h"

 //����IO��ѡ��
#define GPIO             GPIOA
#define PIN                 GPIO_Pin_5
//IO��������
#define DHT11_IO_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=4<<12;}
#define DHT11_IO_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
////IO��������											   
#define	DHT11_DQ_OUT PBout(11) //���ݶ˿�	PA0 
#define	DHT11_DQ_IN  PBin(11)  //���ݶ˿�	PA0 

extern u8 isDht11Exist;
extern u8 temperature;
extern u8 humidity;

u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11    
#endif















