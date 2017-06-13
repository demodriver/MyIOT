#include "includes.h"
#include "misc.h"
/////////////////////////UCOSII任务堆栈设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//创建任务堆栈空间	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数接口
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
	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();
	return 0;
}

//void SysTick_init()	 //系统滴答定时器初始化
//{
//	u32 reload;
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	// 72M/8 HCLK/8
//	 
//	reload=SystemCoreClock/8000000;		//	   
//	reload*=1000000/OS_TICKS_PER_SEC;//9M/OS_TICKS_PER_SEC
//	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//失能SYSTICK
//	SysTick->LOAD=reload; 	//定时周期为1/OS_TICKS_PER_SEC = 10ms	
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//使能SYSTICK    
//}			

//开始任务
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	
	//SysTick_init();
	
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    	
	OSTaskCreate(WiFi_Task,(void *)0,(OS_STK*)&WIFI_TASK_STK[WIFI_STK_SIZE-1],WIFI_TASK_PRIO);	
	OSTaskCreate(Device_Task, (void *)0, (OS_STK *)&DEVICE_TASK_STK[DEVICE_STK_SIZE-1],DEVICE_TASK_PRIO);
	OSTaskCreate(Remote_Task,(void *)0,(OS_STK*)&REMOTE_TASK_STK[REMOTE_STK_SIZE-1],REMOTE_TASK_PRIO);	
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

