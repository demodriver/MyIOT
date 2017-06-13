/*
************************************************************************************************
主要的包含文件

文 件: INCLUDES.C ucos包含文件
作 者: Jean J. Labrosse
************************************************************************************************
*/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
//#include "malloc.h"	

#include "ucos_ii.h"
#include "os_cpu.h"
#include "os_cfg.h"

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "sys.h"
#include "EdpKit.h"
#include "wifi_config.h"
#include "wifi_function.h"
#include "usart.h"
#include "led.h"
#include "dht11.h"
#include "remote.h"
#include "device.h"
#include "timer.h"
#include "delay.h"

#define _DEBUG(msg...)		do	\
							{	\
								printf("[ %s, %s, %04d ] ",__FILE__, __FUNCTION__, __LINE__);	\
								printf(msg);	\
								printf("\r\n");	\
							} while(0)
							
#define _ERROR(msg...)     	do	\
							{	\
								printf("[ ERROR: %s, %04d] ", __FILE__,  __LINE__);	\
								printf(msg);	\
								printf("\r\n");	\
							} while(0)
							
#define _ASSERT(exp)        do	\
							{	\
                                if (!(exp)) {\
                                    printf( "[ %s ]  ",#exp);printf("\r\n");\
                                    assert(exp);\
                                }\
                            } while (0)
#define sys_log             _DEBUG  

 
#endif































