#include "Moto.h"
#define Moto_PwmMax 1999
#define Moto_PwmMin 500
vs16 Moto_PWM_1,Moto_PWM_2,Moto_PWM_3,Moto_PWM_4; //������
void MotoReFlash(int16_t Moto_PWM_1,int16_t Moto_PWM_2,int16_t Moto_PWM_3,int16_t Moto_PWM_4)
{
	if(Moto_PWM_1>Moto_PwmMax)	Moto_PWM_1 = Moto_PwmMax;
	if(Moto_PWM_2>Moto_PwmMax)	Moto_PWM_2 = Moto_PwmMax;
	if(Moto_PWM_3>Moto_PwmMax)	Moto_PWM_3 = Moto_PwmMax;
	if(Moto_PWM_4>Moto_PwmMax)	Moto_PWM_4 = Moto_PwmMax;
	if(Moto_PWM_1<Moto_PwmMin)	Moto_PWM_1 =0;
	if(Moto_PWM_2<Moto_PwmMin)	Moto_PWM_2 =0;
	if(Moto_PWM_3<Moto_PwmMin)	Moto_PWM_3 =0;
	if(Moto_PWM_4<Moto_PwmMin)	Moto_PWM_4 =0;
	
	TIM4->CCR1 = Moto_PWM_4;
	TIM4->CCR2 = Moto_PWM_3;
	TIM4->CCR3 = Moto_PWM_2;
	TIM4->CCR4 = Moto_PWM_1;
}
//********************************************************//
//PWM
void Time4_Init()
{
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure; 
	TIM_OCInitTypeDef  TIM_OCInitStructure;  
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	//ʱ������
	TIM_TimeBaseStructure.TIM_Period = 1999;            //װ������
	TIM_TimeBaseStructure.TIM_Prescaler =72-1;          //Ԥ��Ƶֵ ������ʱ��1M
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //ʱ�ӷ�Ƶ:TDTS = Tck_tim 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);   //��ʼ��TIMx  
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //ѡ��PWMģʽ1 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //������Ը�
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);                 //ͨ��OC1��ʼ��  
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��Ԥװ�ؼĴ���
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);                 //ͨ��OC2��ʼ��  
  TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��Ԥװ�ؼĴ���
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);                 //ͨ��OC3��ʼ��  
  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��Ԥװ�ؼĴ���
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);                 //ͨ��OC4��ʼ��  
  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��Ԥװ�ؼĴ���
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);               //�Զ���װʹ��
  TIM_Cmd(TIM4, ENABLE);                          //ʹ��TIM4 
}

void Moto_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//ʹ�ܵ���õ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); 
	//���õ��ʹ�õ��ùܽ�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
