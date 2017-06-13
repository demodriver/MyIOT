/*
*********************************************************************************************************
*
*	ģ������ : ��ʱ������ģ��
*	�ļ����� : timer.c
*	��    �� : V1.0
*	˵    �� : ��ʱ����ʼ������
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2016-07-30  �³�  	 ��ʽ����
*
*********************************************************************************************************
*/

#include "timer.h"

/*
*********************************************************************************************************
*	�� �� ��: Tim2_Init
*	����˵��: ��ʱ��2��ʼ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void tim2_init(uint16_t Period, uint16_t Prescaler)
{
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//����TIM2ʱ��
	TIM_DeInit(TIM2);
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseStructure.TIM_Period = Period;//72Mʱ��36000��Ƶ�󣬱��2000Hz��Ԥװ��ֵ��Ϊ200����100ms�ж�һ��
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler;//36000��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure);
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_PrescalerConfig(TIM2, Prescaler, TIM_PSCReloadMode_Immediate);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );
	
	TIM_Cmd(TIM2, ENABLE);
}
