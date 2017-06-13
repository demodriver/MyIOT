/*
*********************************************************************************************************
*
*	模块名称 : 定时器配置模块
*	文件名称 : timer.c
*	版    本 : V1.0
*	说    明 : 定时器初始化设置
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2016-07-30  陈驰  	 正式发布
*
*********************************************************************************************************
*/

#include "timer.h"

/*
*********************************************************************************************************
*	函 数 名: Tim2_Init
*	功能说明: 定时器2初始化配置
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void tim2_init(uint16_t Period, uint16_t Prescaler)
{
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//开启TIM2时钟
	TIM_DeInit(TIM2);
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseStructure.TIM_Period = Period;//72M时钟36000分频后，变成2000Hz，预装载值设为200，则100ms中断一次
	TIM_TimeBaseStructure.TIM_Prescaler = Prescaler;//36000分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure);
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_PrescalerConfig(TIM2, Prescaler, TIM_PSCReloadMode_Immediate);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE );
	
	TIM_Cmd(TIM2, ENABLE);
}
