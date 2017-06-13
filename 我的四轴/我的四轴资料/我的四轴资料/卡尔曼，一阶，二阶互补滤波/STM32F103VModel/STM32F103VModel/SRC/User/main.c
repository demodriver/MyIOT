/**
  ****************************************************
  *@file: 
  *@author: wszdxmh
  *@version: V1.0
  *@date: 2016.05.13
  *@brief:
  ****************************************************
	*@attention
	*
	*
	****************************************************
  */

/*����ͷ�ļ�*/
#include "stm32f10x.h"
#include "stm32_eval.h"
#include "led.h"
#include "tim.h"
#include "usart.h"
#include "delay.h"
#include "sys.h"
#include "spi.h"
#include "stdio.h"
#include "stdlib.h"
#include "motor.h"
#include "mpu6050.h"
#include "ANO-Tech.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "Kalman_filter.h"


/*����ȫ�ֱ���*/
uint16_t TIM5CH1_CAPTURE_VAL;//TIM5_IRQHeader
uint8_t TIM5CH1_CAPTURE_STA;//TIM5_IRQHearder
//�������
float pitch,roll,yaw; 		//ŷ����
short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
short gyrox,gyroy,gyroz;	//������ԭʼ����
extern float  Acc_angle,Gry_vivi;
extern float Angle,Gyro_x;         //С���˲�����б�Ƕ�/���ٶ�	
extern float Angle_x_temp;  //�ɼ��ٶȼ����x��б�Ƕ�
extern float Angle_y_temp;  //�ɼ��ٶȼ����y��б�Ƕ�
extern float Angle_z_temp;
extern float Angle_X_Final; //X������б�Ƕ�
extern float Angle_Y_Final; //Y������б�Ƕ�
extern float Angle_Z_Final; //Z������б�Ƕ�
extern float Gyro_x;		 //X�������������ݴ�
extern float Gyro_y;        //Y�������������ݴ�
extern float Gyro_z;		 //Z�������������ݴ�
extern float A_P,A_R,A2_P;


int main(void)
{
	//��ʼ��Ӳ��
	SystemInit();
	delay_init();
	LED_Init();
	LED_Open(0,0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	TIM1_Init(2499,7199);
	Motor_Init(999,71);
	Usart_Init(500000);
	MPU6050_Init();
	while(mpu_dmp_init());
	while(1)
	{
//		MOTOR_FORWARD(500);
//		delay_ms(1500);
//		MOTOR_REVERSE(500);
//		delay_ms(1500);
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{ 
//			temp=MPU_Get_Temperature();	//�õ��¶�ֵ
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
			Angle_Calcu();
			ANO_DT_Send_Senser(Angle_x_temp,Angle_X_Final,(Angle_y_temp),(Angle_Y_Final),(Angle_z_temp),(Angle_Z_Final));
		}
	}
}
