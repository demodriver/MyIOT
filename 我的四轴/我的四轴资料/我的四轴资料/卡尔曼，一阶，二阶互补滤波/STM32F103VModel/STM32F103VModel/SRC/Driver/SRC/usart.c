/**
  ************************************
  * @file    usart.c
  * @author  wszdxmh
  * @version V3.5.0
  * @date    2016.07.07
  * @brief   ��������
	************************************
*/
#include "sys.h"
#include "usart.h"	  

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x)
{
	x = x;
}
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{
	while ((USART1->SR & 0X40) == 0);//ѭ������,ֱ���������   
	USART1->DR = (u8)ch;
	return ch;
}

/**
  ************************************
  * @brief  ��ʼ������1
  * @param  baudrate��������
  * @retval None
	************************************
*/
void Usart_Init(u32 baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//ʹ��APB2ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//ʹ�ܴ���1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	//��ʼ��PA9 Tx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//��ʼ��PA10 Rx
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART1 ��ʼ��
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);
	//��ʼ���ж�
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_Init(&NVIC_InitStructure);
	//ʹ��
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//ʹ�ܴ����ж�
	USART_Cmd(USART1, ENABLE);//ʹ�ܴ���
}
