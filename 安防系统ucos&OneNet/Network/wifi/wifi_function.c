#include "wifi_function.h"
#include "wifi_config.h"
#include "includes.h"
#include "led.h"
static wifiStatus_e wifiStatus;

/*
 * 函数名：ESP8266_Choose
 * 描述  ：使能/禁用WF-ESP8266模块
 * 输入  ：enumChoose = ENABLE，使能模块
 *         enumChoose = DISABLE，禁用模块
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_Choose ( FunctionalState enumChoose )
{
// 	if ( enumChoose == ENABLE )
// 		ESP8266_CH_HIGH_LEVEL();
// 	
// 	else
// 		ESP8266_CH_LOW_LEVEL();
}


/*
 * 函数名：ESP8266_Rst
 * 描述  ：重启WF-ESP8266模块
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被ESP8266_AT_Test调用
 */
bool ESP8266_Rst ( void )
{
    bool ret = false;
    u16 retry = 0;
    u8 err = 0;

    if(!ESP8266_Cmd ( "AT+RST\r\n", "OK", NULL, 20, 3 ))
        return false;
        
    while(ret==false && retry++ < 12)
    {
        OSSemPend(WiFiRecv_Sem, 200, &err);
        if(err == OS_ERR_NONE)
        {
            ret = ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "ready" );
            if(ret)
            {
                sys_log("wifi reply:\r\n%s", strEsp8266_Fram_Record .Data_RX_BUF);
            }
            else
            {
                sys_log("###ret=%d", ret);
            }                
        }
    }
    return ret;
}


/*
 * 函数名：ESP8266_AT_Test
 * 描述  ：对WF-ESP8266模块进行AT测试启动
 * 输入  ：无
 * 返回  : 无
 * 调用  ：被外部调用
 */
void ESP8266_AT_Test ( void )
{
	ESP8266_RST_HIGH_LEVEL();	
	while ( ! ESP8266_Cmd ( "AT", "OK", NULL, 1000, 3 ) ) ESP8266_Rst ();  	
}


/*
 * 函数名：ESP8266_Cmd
 * 描述  ：对WF-ESP8266模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 * 调用  ：被外部调用
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime, u8 retry )
{    
	u8 err = OS_ERR_NONE;
    bool ret = false;
    u8 cnt = 0;
	WiFiRecv_Sem->OSEventCnt = 0;
//	while(err == OS_ERR_NONE)
//	{
//		OSSemPend(WiFiRecv_Sem, 1, &err);
//		if(err == OS_ERR_NONE)
//		{
//			sys_log("%s", strEsp8266_Fram_Record .Data_RX_BUF);
//		}
//	}

	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包
    sys_log("send command: %s", cmd);
	ESP8266_Usart ( "%s\r\n", cmd );
	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return true;
    
    while(ret==false && cnt++ < retry)
    {
        OSSemPend(WiFiRecv_Sem, waittime, &err);
        if(err == OS_ERR_NONE)
        {
            sys_log("wifi reply:\r\n%s", strEsp8266_Fram_Record .Data_RX_BUF);
            if(strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ))
                ret = true;
            else if(strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "ERROR" ))
                ret = false;
        }
    }
    return ret;
}


/*
 * 函数名：ESP8266_Net_Mode_Choose
 * 描述  ：选择WF-ESP8266模块的工作模式
 * 输入  ：enumMode，工作模式
 * 返回  : 1，选择成功
 *         0，选择失败
 * 调用  ：被外部调用
 */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1\r\n", "OK", "no change", 50, 3 ); 
		
		case AP:
			return ESP8266_Cmd ( "AT+CWMODE=2\r\n", "OK", "no change", 50, 3 ); 
		
		case STA_AP:
			return ESP8266_Cmd ( "AT+CWMODE=3\r\n", "OK", "no change", 50, 3 ); 
		
		default:
			return false;
  }
}

/*
 * 函数名：ESP8266_JoinAP
 * 描述  ：WF-ESP8266模块连接外部WiFi
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 400, 10 );	
}


/*
 * 函数名：ESP8266_BuildAP
 * 描述  ：WF-ESP8266模块创建WiFi热点
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 *       ：enunPsdMode，WiFi加密方式代号字符串
 * 返回  : 1，创建成功
 *         0，创建失败
 * 调用  ：被外部调用
 */
bool ESP8266_BuildAP ( char * pSSID, char * pPassWord, char * enunPsdMode )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%s\r\n", pSSID, pPassWord, enunPsdMode );
	
	return ESP8266_Cmd ( cCmd, "OK", 0, 200 , 3 );
	
}


/*
 * 函数名：ESP8266_Enable_MultipleId
 * 描述  ：WF-ESP8266模块启动多连接
 * 输入  ：enumEnUnvarnishTx，配置是否多连接
 * 返回  : 1，配置成功
 *         0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d\r\n", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 80, 3 );
	
}


/*
 * 函数名：ESP8266_Link_Server
 * 描述  ：WF-ESP8266模块连接外部服务器
 * 输入  ：enumE，网络协议
 *       ：ip，服务器IP字符串
 *       ：ComNum，服务器端口字符串
 *       ：id，模块连接服务器的ID
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch (  enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s\r\n", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s\r\n", "UDP", ip, ComNum );
		  break;
		
		default:
			break;
  }
  if ( id < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s\r\n", id, cStr);

  else
	  sprintf ( cCmd, "AT+CIPSTART=%s\r\n", cStr );
	
	return ESP8266_Cmd ( cCmd, "CONNECT\r\n\r\nOK", "ALREADY CONNECTED", 200, 3 );	
}


/*
 * 函数名：ESP8266_StartOrShutServer
 * 描述  ：WF-ESP8266模块开启或关闭服务器模式
 * 输入  ：enumMode，开启/关闭
 *       ：pPortNum，服务器端口号字符串
 *       ：pTimeOver，服务器超时时间字符串，单位：秒
 * 返回  : 1，操作成功
 *         0，操作失败
 * 调用  ：被外部调用
 */
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
	char cCmd1 [120], cCmd2 [120];

	if ( enumMode )
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s\r\n", 1, pPortNum );
		
		sprintf ( cCmd2, "AT+CIPSTO=%s\r\n", pTimeOver );

		return ( ESP8266_Cmd ( cCmd1, "OK", 0, 100, 3 ) &&
						 ESP8266_Cmd ( cCmd2, "OK", 0, 100, 3 ) );
	}
	
	else
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s\r\n", 0, pPortNum );

		return ESP8266_Cmd ( cCmd1, "OK", 0, 100, 3 );
	}
}


/*
 * 函数名：ESP8266_SMARTConfig
 * 描述  ：WF-ESP8266模块配置SMART连接
 * 输入  ：enumSmartMode
 * 返回  : 1，配置成功
		   0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_SMARTConfig(ENUM_SMART_TypeDef enumSmartMode)
{
	bool ret = false;
	u16 retry = 0;
    u8 err = 0;

	switch(enumSmartMode)
	{
		case AI_LINK:
			ESP8266_Cmd ( "AT+CWSMARTSTART=0\r\n", "OK", 0, 30, 3 );break;
		case ESP_TOUCH:
			ESP8266_Cmd ( "AT+CWSMARTSTART=1\r\n", "OK", 0, 30, 3 );break;
		case AirKiss:
			ESP8266_Cmd ( "AT+CWSMARTSTART=2\r\n", "OK", 0, 30, 3 );break;
		default:
			return false;
	}
    
	while(ret==false && retry++ < 2)
    {
        OSSemPend(WiFiRecv_Sem, 6000, &err);
        
        if(err == OS_ERR_NONE)
        {
            sys_log("wifi reply:\r\n%s", strEsp8266_Fram_Record .Data_RX_BUF);
            ret = ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "WIFI GOT IP" );
        }
    }
    
    if(err != OS_ERR_NONE)
    {
        sys_log("airkiss timeout");
    }
    return ret;
}

/*
 * 函数名：ESP8266_CheckLinkStatus
 * 描述  ：WF-ESP8266检查TCP连接状态
 * 输入  ：无
 * 返回  : 1，TCP已连接
		   0，TCP未连接
 * 调用  ：被外部调用
 */
bool ESP8266_CheckLinkStatus()
{
	return ESP8266_Cmd ( "AT+CIPSTATUS", " ", 0, 20, 3 );
}

/*
 * 函数名：ESP8266_AutoConnect
 * 描述  ：WF-ESP8266自动连接
 * 输入  ：无
 * 返回  : 1，配置成功
		   0，配置失败
 * 调用  ：被外部调用
 */
bool ESP8266_AutoConnect()
{
	return ESP8266_Cmd ( "AT+CWAUTOCONN=1", "OK", 0, 20, 3 );
}

/*
 * 函数名：ESP8266_ClsTCP
 * 描述  ：WF-ESP8266关闭TCP连接
 * 输入  ：无
 * 返回  : 1，关闭成功
		   0，关闭失败
 * 调用  ：被外部调用
 */
bool ESP8266_ClsTCP()
{
	return ESP8266_Cmd ( "AT+CIPCLOSE\r\n", "OK", "ERROR", 20, 3 );
}
/*
 * 函数名：ESP8266_GetIP
 * 描述  ：WF-ESP8266获取IP
 * 输入  ：无
 * 返回  : 1，关闭成功
		   0，关闭失败
 * 调用  ：被外部调用
 */
bool ESP8266_GetIP()
{
	return ESP8266_Cmd ( "AT+CIFSR\r\n", "OK", "ERROR", 20, 3 );
}
/*
 * 函数名：ESP8266_SetMode
 * 描述  ：WF-ESP8266设置透传模式
 * 输入  ：无
 * 返回  : 1，关闭成功
		   0，关闭失败
 * 调用  ：被外部调用
 */
bool ESP8266_SetMode()
{
	return ESP8266_Cmd ( "AT+CIPMODE=0\r\n", "OK", "ERROR", 20, 3 );
}
//#define AIRKISS_TASK_PRIO      			    6 	//设置任务优先级
//#define AIRKISS_STK_SIZE  				    256	//设置任务栈大小
//OS_STK AIRKISS_TASK_STK[AIRKISS_STK_SIZE];		//任务栈	
//OS_EVENT * AirKiss_Sem;
//static void AirKiss_Task(void *pdata)
//{
//    u8 err = 0;
//    bool ret = false;
//    
//    while(1)
//    {
//				//OSSemPend(AirKiss_Sem, 0, &err);
//        sys_log("enter wifi smartconfig");
//        sys_log("wifi reseting...");
//        ret = ESP8266_Rst();
//        if(!ret)
//            goto exit;
//        sys_log("reset ok");
//        sys_log("set station mode");
//        ret = ESP8266_Net_Mode_Choose ( STA );
//        if(!ret)
//            goto exit;
//        sys_log("airkiss executing...");
//        ret = ESP8266_SMARTConfig(AirKiss);

//    exit:
//        sys_log("airkiss config, ret=%d", ret);
//    }
//}


#define ACCESSROUTER_TASK_PRIO      			    6 	//设置任务优先级
#define ACCESSROUTER_STK_SIZE  				        128	//设置任务栈大小
OS_STK ACCESSROUTER_TASK_STK[ACCESSROUTER_STK_SIZE];	//任务栈	
OS_EVENT * AccessRouter_Sem;
extern OS_EVENT * WiFiStatus_Sem;
static void AccessRouter_Task(void *pdata)
{
    u8 err = 0;
    bool ret = false;
	
    while(1)
    {
			OSSemPend(AccessRouter_Sem, 0, &err);
        sys_log("router access");
		
				wifiStatus = LINKING;
				OSSemPost(WiFiStatus_Sem);
        
        sys_log("set station mode");
#ifdef SLAVE_NODE   //从节点
        ret = ESP8266_Net_Mode_Choose ( STA_AP ); //STA & AP
        if(!ret)
            goto exit;
				sys_log("wifi reseting...");
        ret = ESP8266_Rst();
				
				sys_log("Disable_MultipleId...");
				ret = ESP8266_Enable_MultipleId (DISABLE);//使能多链接
				if(!ret)
           goto exit;
				sys_log("MultipleId ok");	
				
		    sys_log("join AP...");
        ret = ESP8266_JoinAP("MI12138", "12345678");
					if(!ret)
				{
					goto exit;
				} 
				
				ret = ESP8266_GetIP();
				if(!ret)
				{
					goto exit;
				} 
				sys_log("join AP ok"); 
				
//				ret = ESP8266_Link_Server(enumTCP, Host, Port,Single_ID_0);
//        if(!ret)
//	      {
//        sys_log("Onenet server link fail");
//		    goto exit;
//	      }	
				ret = ESP8266_SetMode();
        if(!ret)
	      {
        sys_log("SetMode fail");
		    goto exit;
	      }
//				ret = ESP8266_Cmd ( "AT+CIPSEND\r\n", ">", "OK", 50, 3 );
//        if(!ret)
//	      {
//        sys_log("SetMode fail");
//		    goto exit;
//	      }							
#else
				ret = ESP8266_Net_Mode_Choose ( AP ); //STA & AP
        if(!ret)
            goto exit;
				sys_log("wifi reseting...");
        ret = ESP8266_Rst();
        if(!ret)
            goto exit;
        sys_log("reset ok");
				
				ret = ESP8266_BuildAP ( "ESP_tast","0123456789", "4");//pwd至少8位
				if(!ret)
           goto exit;
				sys_log("ESP8266_BuildAP ok");
				
				sys_log("Enable_MultipleId...");
				ret = ESP8266_Enable_MultipleId (ENABLE);//使能多链接
				if(!ret)
           goto exit;
				sys_log("MultipleId ok");		
				
				sys_log("Start_Server...");
				ret = ESP8266_StartOrShutServer ( ENABLE, "8080", "300" );//创建服务器
				if(!ret)
           goto exit;
				sys_log("Start_Server ok");
				
#endif       
			exit:
        sys_log("AccessRouter, ret=%d", ret);	
				
		   if(ret)
		   {
			  wifiStatus = OK;
		   }
		   else
		   {
		   	wifiStatus = FAIL;
		   }
		   OSSemPost(WiFiStatus_Sem);
			 AccessRouter_Sem->OSEventCnt = 0;
	  }
}

//#define WIFISTATUSQUERY_TASK_PRIO      			    			5 	//设置任务优先级
//#define WIFISTATUSQUERY_STK_SIZE  				        		64	//设置任务栈大小
//OS_STK WIFISTATUSQUERY_TASK_STK[WIFISTATUSQUERY_STK_SIZE];			//任务栈	
//OS_EVENT * WiFiStatusQuery_Sem;
//static void WiFiStatusQuery_Task(void *pdata)
//{
//	  u8 err = 0;
//    bool ret = false;
//	while(1)
//	{
//		
//	}
//}

static void LedBling_TmrCallBack(void *pdata)
{
	static u8 ledVal = ON;
	LED1 = ledVal;
	ledVal = !ledVal;
}

#define WIFISTATUS_TASK_PRIO      			    	4 	//设置任务优先级
#define WIFISTATUS_STK_SIZE  				        256	//设置任务栈大小
OS_STK WIFISTATUS_TASK_STK[WIFISTATUS_STK_SIZE];		//任务栈	
OS_EVENT * WiFiStatus_Sem;
OS_EVENT * WiFiStatusOK_Sem;
static void WiFiStatus_Task(void *pdata)
{
    u8 err = 0;
	static OS_TMR * pTmr = NULL;
	
	while(1)
	{
		OSSemPend(WiFiStatus_Sem, 0, &err);
		switch(wifiStatus)
		{
			case LINKING:
				sys_log("wifiStatus LINKING");
				LED0 = OFF;
				//参数period以定时器的节拍数OS_TMR_CFG_TICKS_PER_SEC为单位，比如节拍数为10，则参数为1，就是100ms
				pTmr = OSTmrCreate(0, 1, OS_TMR_OPT_PERIODIC, (OS_TMR_CALLBACK)LedBling_TmrCallBack, (void*)0, (u8*)"LedBling", &err);
				if(pTmr)
				{
					OSTmrStart(pTmr, &err);
				}
			break;
			
			case OK:
				sys_log("wifiStatus OK");
				if(pTmr)
				{
					OSTmrDel(pTmr, &err);
					LED1 = ON;
				}
			  OSSemPost(WiFiStatusOK_Sem);		
				OSTaskSuspend(ACCESSROUTER_TASK_PRIO);	//挂起链接任务.
			break;

			case FAIL:
				sys_log("wifiStatus FAIL");
				if(pTmr)
				{
					OSTmrDel(pTmr, &err);
					LED1 = OFF;
					LED0 = ON;
				}
			OSSemPost(AccessRouter_Sem);
			break;
		}
	}
}


OS_STK WIFI_TASK_STK[WIFI_STK_SIZE];
OS_EVENT * WiFiRecv_Sem;
void WiFi_Task(void *pdata)
{
	OS_CPU_SR  cpu_sr = 0u;
	OS_ENTER_CRITICAL();
	AccessRouter_Sem = OSSemCreate(1);
	WiFiStatus_Sem = OSSemCreate(0);
	WiFiStatusOK_Sem = OSSemCreate(0);
  WiFiRecv_Sem = OSSemCreate(0);	
	//WiFiStatusQuery_Sem = OSSemCreate(0);
	
  //OSTaskCreate(WiFiStatusQuery_Task, (void *)0, (OS_STK *)&WIFISTATUSQUERY_TASK_STK[WIFISTATUSQUERY_STK_SIZE-1],WIFISTATUSQUERY_TASK_PRIO);
  OSTaskCreate(AccessRouter_Task, (void *)0, (OS_STK *)&ACCESSROUTER_TASK_STK[ACCESSROUTER_STK_SIZE-1],ACCESSROUTER_TASK_PRIO);
	OSTaskCreate(WiFiStatus_Task, (void *)0, (OS_STK *)&WIFISTATUS_TASK_STK[WIFISTATUS_STK_SIZE-1], WIFISTATUS_TASK_PRIO);
	
	OSSemPost(AccessRouter_Sem);
	OS_EXIT_CRITICAL();
	OSTaskDel(OS_PRIO_SELF);
}
