#include "includes.h"
#include "device.h"

u8 sensor_num = 0;
u8 startDAQ = 0;

user_info_t * user_info;

user_info_t * mallocStorageArea(void)
{
	user_info = (user_info_t *)malloc(StorageAreaSize);
	memset(user_info, 0, StorageAreaSize);
	return user_info;
}

#ifdef SLAVE_NODE   //从节点代码

bool device_Init(void)
{
	if(!DHT11_Init())
	{
		isDht11Exist = 1;
		return true;
	}
	else
	{
		return false;		
	}
}

static bool device_DataAcquisition(void)
{
	bool ret = false;
	OS_CPU_SR  cpu_sr = 0u;
	if(isDht11Exist)
	{
		OS_ENTER_CRITICAL();	
		ret = (bool)DHT11_Read_Data(&temperature, &humidity);
		OS_EXIT_CRITICAL();
		sys_log("temperature = %d", temperature);
		sys_log("humidity = %d", humidity);
	}
	return ret;
}

OS_STK DEVICE_TASK_STK[DEVICE_STK_SIZE];		    //任务栈
OS_EVENT * devDAQ_Sem;
void Device_Task(void *pdata)
{
	  u8 err = 0;
  	devDAQ_Sem = OSSemCreate(0);
	  startDAQ = 1;
	  DHT11_DQ_OUT=1;
	while(1)
	{
		OSSemPend(devDAQ_Sem, 0, &err);
		if(!device_DataAcquisition())
		{
			OSSemPost(postData_Sem);
		}
		else
		{
			sys_log("DAQ Fail");
		}
	}
}
#endif

OS_STK DEVICE_TASK1_STK[DEVICE_STK1_SIZE];		    //任务栈
OS_EVENT * devMQ2_Sem;
void Device_Task1(void *pdata)
{
	  u8 err = 0;
  	devMQ2_Sem = OSSemCreate(0);
	while(1)
	{
		OSSemPend(devMQ2_Sem, 0, &err);
		while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
		 if(ADC_GetConversionValue(ADC1))
		 {
			 OSSemPost(postData_Sem);
		 }
			
	}

}

