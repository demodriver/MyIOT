#include "includes.h"
#include "misc.h"
/////////////////////////UCOSII�����ջ����///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//���������ջ�ռ�	
OS_STK START_TASK_STK[START_STK_SIZE];
//�������ӿ�
void start_task(void *pdata);	
 			   
/////////////////////////////////////////
void SysTick_init(void);
////////////////////////////////////////
int main()
{
	//RCC_Configuration();
	NVIC_Configuration();
	delay_init();
	usart1_init(115200);
	usart2_init(115200);
	Led_Init();
	tim2_init(199, 3599);
	delay_ms(2000);
	if(device_Init())
	{
		sys_log("Device Init OK");		
	}
	else
	{
		sys_log("Device Init Error");
	}
//	while(1)
//	{
//		DHT11_DQ_OUT=1;
//		delay_us(2000);	
//		DHT11_DQ_OUT=0;
//		delay_us(2000);	
//		//DHT11_Read_Data(&temperature, &humidity);
//		
//	}
	OSInit();
	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();
	return 0;
}

//void SysTick_init()	 //ϵͳ�δ�ʱ����ʼ��
//{
//	u32 reload;
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	// 72M/8 HCLK/8
//	 
//	reload=SystemCoreClock/8000000;		//	   
//	reload*=1000000/OS_TICKS_PER_SEC;//9M/OS_TICKS_PER_SEC
//	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//ʧ��SYSTICK
//	SysTick->LOAD=reload; 	//��ʱ����Ϊ1/OS_TICKS_PER_SEC = 10ms	
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//ʹ��SYSTICK    
//}			

//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	
	//SysTick_init();
	
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������	
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    	
	OSTaskCreate(WiFi_Task,(void *)0,(OS_STK*)&WIFI_TASK_STK[WIFI_STK_SIZE-1],WIFI_TASK_PRIO);	
	OSTaskCreate(Device_Task, (void *)0, (OS_STK *)&DEVICE_TASK_STK[DEVICE_STK_SIZE-1],DEVICE_TASK_PRIO);
	OSTaskCreate(Remote_Task,(void *)0,(OS_STK*)&REMOTE_TASK_STK[REMOTE_STK_SIZE-1],REMOTE_TASK_PRIO);	
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}

