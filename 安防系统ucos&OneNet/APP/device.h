#ifndef _DEVICE_H
#define _DEVICE_H

#include "stm32f10x.h"
#include "includes.h"

#define PARAM_E		0xFF	//param is empty
#define PARAM_NE	0x01	//param is not empty

#define StorageAreaSize  1024

#pragma pack(push, 1)
#pragma pack(pop)
extern u8 startDAQ;
extern OS_EVENT * devDAQ_Sem;

#define DEVICE_TASK_PRIO      			    		9 	//设置任务优先级
#define DEVICE_STK_SIZE  				        	128	//设置任务栈大小
extern OS_STK DEVICE_TASK_STK[DEVICE_STK_SIZE];		    //任务栈

#define DEVICE_TASK1_PRIO      			    		2 	//设置任务优先级
#define DEVICE_STK1_SIZE  				        	128	//设置任务栈大小
extern OS_STK DEVICE_TASK1_STK[DEVICE_STK1_SIZE];		    //任务栈

extern void Device_Task(void *pdata);

extern bool device_Init(void);


#endif
