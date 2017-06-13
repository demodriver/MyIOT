#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"   
#include "includes.h"

 //数据IO口选择
#define GPIO             GPIOA
#define PIN                 GPIO_Pin_5
//IO方向设置
#define DHT11_IO_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=4<<12;}
#define DHT11_IO_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
////IO操作函数											   
#define	DHT11_DQ_OUT PBout(11) //数据端口	PA0 
#define	DHT11_DQ_IN  PBin(11)  //数据端口	PA0 

extern u8 isDht11Exist;
extern u8 temperature;
extern u8 humidity;

u8 DHT11_Init(void);//初始化DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//读取温湿度
u8 DHT11_Read_Byte(void);//读出一个字节
u8 DHT11_Read_Bit(void);//读出一个位
u8 DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11    
#endif















