#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ϵͳ�жϷ������û�		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/10
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************  
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

//void RCC_Configuration()
//{
//	RCC_DeInit();	
//	RCC_HSEConfig(RCC_HSE_ON);
//	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);	

//	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);	
//	FLASH_SetLatency(FLASH_Latency_2);

//	RCC_HCLKConfig(RCC_SYSCLK_Div1);
//	RCC_PCLK2Config(RCC_HCLK_Div1);
//	RCC_PCLK1Config(RCC_HCLK_Div2);	
//	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
//	RCC_PLLCmd(ENABLE);	//??PLL
//	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);	
//	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);	
//	while(RCC_GetSYSCLKSource() != 0x08);	
//}

void NVIC_Configuration()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
    
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
