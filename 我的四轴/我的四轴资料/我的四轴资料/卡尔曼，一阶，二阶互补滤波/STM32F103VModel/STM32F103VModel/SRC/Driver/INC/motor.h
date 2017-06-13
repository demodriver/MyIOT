
#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

//�������Ŷ���
#define DIRECTION 	GPIO_Pin_1

//��������꺯��
#define MOTOR_DIRECRION(x)		GPIO_WriteBit(GPIOB, DIRECTION, x);//PB1��DR
#define MOTOR_SPEED(x)				TIM_SetCompare3(TIM3,x);//PB0��Ve
#define MOTOR_FORWARD(x)			{MOTOR_DIRECRION(1);MOTOR_SPEED(x);}
#define MOTOR_REVERSE(x)			{MOTOR_DIRECRION(0);MOTOR_SPEED(x);}

void Motor_Init(uint16_t arr, uint16_t psc);

#endif
