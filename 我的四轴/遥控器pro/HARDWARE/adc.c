#include "adc.h"

u16 ADC_Temp[10];
void ADC_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef ADC_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //����ʱ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //adcʱ�ӷ�Ƶ����  12M	
	
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ģ������
	GPIO_Init(GPIOA, &GPIO_InitStructure);         //GPIOA
 
	ADC_DeInit(ADC1); 
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC����ģʽ������ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //AD ɨ��ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //AD ����ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //�������ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC �����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 7; //˳�����adcת��ͨ����Ŀ 6
	ADC_Init(ADC1, &ADC_InitStructure); //��ʼ�� ADCx  
	
	ADC_Cmd(ADC1, ENABLE); //ʹ��ADC1 
	ADC_ResetCalibration(ADC1); //����adc1У׼
	while(ADC_GetResetCalibrationStatus(ADC1)); //�ȴ���λУ׼����
	ADC_StartCalibration(ADC1);             //����adУ׼
	while(ADC_GetCalibrationStatus(ADC1));  //�ȴ�adУ׼����
	
	//����ָ��adc�Ĺ���ͨ�����������ǵ�ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADC1, 1, 1, ADC_SampleTime_239Cycles5 ); //adc1��ͨ��1���������˳��ֵ1������ʱ��239.5����   
	ADC_RegularChannelConfig(ADC1, 2, 2, ADC_SampleTime_239Cycles5 ); //adc1��ͨ��1���������˳��ֵ1������ʱ��239.5����   
	ADC_RegularChannelConfig(ADC1, 3, 3, ADC_SampleTime_239Cycles5 ); //adc1��ͨ��1���������˳��ֵ1������ʱ��239.5����   
	ADC_RegularChannelConfig(ADC1, 4, 4, ADC_SampleTime_239Cycles5 ); //adc1��ͨ��1���������˳��ֵ1������ʱ��239.5����   
	ADC_RegularChannelConfig(ADC1, 5, 5, ADC_SampleTime_239Cycles5 ); //adc1��ͨ��1���������˳��ֵ1������ʱ��239.5����   
	ADC_RegularChannelConfig(ADC1, 6, 6, ADC_SampleTime_239Cycles5 ); //adc1��ͨ��1���������˳��ֵ1������ʱ��239.5����   
	ADC_RegularChannelConfig(ADC1, 7, 7, ADC_SampleTime_239Cycles5 ); //adc1��ͨ��1���������˳��ֵ1������ʱ��239.5����   
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��ָ����adc1�����ת������ 
}

void DMA_INIT(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //ʹ��DMAʱ��
	 
	DMA_InitStructure.DMA_PeripheralBaseAddr =((uint32_t)0x4001244C); 	//DMA����ADC����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_Temp;  //DMA �ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //�������ȡ���͵��ڴ�
	DMA_InitStructure.DMA_BufferSize = 7;  //DMA�����С ��һ�δ�����������  7*16
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //16λ���ִ��� 
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //������ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMAͨ��xӵ�������ȼ�  
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //���ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //��ʼ��
	ADC_DMACmd(ADC1, ENABLE);//ʹ��DMA
	DMA_Cmd(DMA1_Channel1, ENABLE);
}


void Get_Adc(void)    
{
	u8 i;	
	for(i=0;i<7;i++)
	{
	  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
	  //ADC_Temp[i]=ADC_GetConversionValue(ADC1); //�������һ��adc1������ת�����
	}
} 

