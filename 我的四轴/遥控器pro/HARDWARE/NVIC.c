#include "NVIC.h"

void NVIC_INIT(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_InitStruct.NVIC_IRQChannel= TIM3_IRQn;		    //ʱ��3
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;  //��ռ���ȼ�2
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;		  //��Ӧ���ȼ�1
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;			  //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStruct);						  //�жϳ�ʼ��	
	//����
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	//�ⲿ�ж�
	NVIC_InitStruct.NVIC_IRQChannel=EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}