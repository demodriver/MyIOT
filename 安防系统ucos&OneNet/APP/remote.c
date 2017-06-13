#include "remote.h"
#include "remote_def.h"
#include "malloc.h"	
EdpPacket* send_pkg;
char send_buf[MAX_SEND_BUF_LEN]={"18"};
char strSendCmd[30] = {0};


//static bool Esp8266_Send(EdpPacket * dataPacket,char * reply1, char * reply2, u32 waittime, u8 retry)
//{
//	  u8 err = OS_ERR_NONE;
//    bool ret = false;
//		 u8 cnt = 0;
//    usart2_write(USART2,dataPacket->_data,dataPacket->_write_pos);	
//		if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
//		return true;
//    
//    while(ret==false && cnt++ < retry)
//    {
//        OSSemPend(WiFiRecv_Sem, waittime, &err);
//        if(err == OS_ERR_NONE)
//        {
//            sys_log("wifi reply:\r\n%s", strEsp8266_Fram_Record .Data_RX_BUF);
//            if(strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ))
//                ret = true;
//            else if(strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "ERROR" ))
//                ret = false;
//        }
//    }
//exit:
//	return ret;
//}
static bool Esp8266_Send(char * dataPacket)
{
    bool ret = false;
	
	sprintf(strSendCmd, "AT+CIPSEND=%d\r\n", strlen(dataPacket));
        
//    ret = ESP8266_Cmd("AT+CIPMUX=0\r\n", "OK", "CLOSED", 50, 3);
//    if(!ret)
//	{
//        sys_log("single link fail");
//		goto exit; 
//	}
	
	ret = ESP8266_Link_Server(enumTCP, Host, Port, Single_ID_0);
    if(!ret)
	{
        sys_log("yeelink server link fail");
		goto exit;
	}

    ret = ESP8266_Cmd(strSendCmd, "> ", 0, 50, 3);
    if(!ret)
	{
        sys_log("esp8266 SendCmd fail");
		goto exit;
	}
    
	sys_log("Http Send:\r\n%s", dataPacket);
    ESP8266_Usart("%s", dataPacket);

exit:
	return ret;
}

static bool _Onenet_httpSend(char * method, char * url, char * host, char * apikey, char *msg)
{
	bool ret = false;
	char * httpPacket;
    httpPacket = (char*)malloc(httpPacketSize);
    if(!httpPacket)
	{
        sys_log("malloc fail");
		return ret;
	}
    sprintf(httpPacket, HttpBuf_Format, 
            method,
            url,
            host,
			apikey,
			strlen(msg),
			msg);
	ret = Esp8266_Send(httpPacket);
	free(httpPacket);
	return ret;
}

bool _Onenet_CreateDataPoints(char * device_id, u8 datapoint0,u8 datapoint1)
{
	bool ret = false;
	u8 retry = 0;
	u8 err = 0;
	u8 is8266SendOK = 0;
	
	char url_DPCreate[50] = {0};
	char * msg = (char*)malloc(100);
	
	if(!msg)
	{
		sys_log("malloc fail");
		goto exit;
	}
	
	sprintf(url_DPCreate, URL_DevCreate, device_id);
	sprintf(msg, DataPoints_JsonFormat, datapoint0,datapoint1);
	ret = _Onenet_httpSend(POST, url_DPCreate, Host, APIKEY,msg);
	free(msg);
	if(!ret)
	{
		sys_log("http send fail");
		goto exit;
	}
	
	ret = false;
	
    while(retry++ < 5)
    {
        OSSemPend(WiFiRecv_Sem, 200, &err);
        if(err == OS_ERR_NONE)
        {
			if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, "SEND OK"))
			{
				sys_log("esp8266 send ok");
				is8266SendOK = 1;
			}
			else
			{
				if(is8266SendOK)
				{
					if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, "HTTP/1.1"))
					{
						if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, "200 OK"))
						{
							ret = true;
						}
						else
						{
							sys_log("Not 200 OK");
						}					
					}
					else
					{
						sys_log("Not start with \"HTTP/1.1\"");						
					}
					goto exit;
				}
				else
				{
					if(retry >= 2)
					{
						sys_log("esp8266 send fail");
						goto exit;
					}
				}
			}
        }
    }	
exit:
	ESP8266_ClsTCP();
	if(!ret)
	{
		sys_log("datapoints post fail ( retry=%d, ret=%d )\r\n", retry, ret);
	}
	else
	{
		sys_log("datapoints post success ( retry=%d, ret=%d )\r\n", retry, ret);
	}
	return ret;		
}



OS_STK REMOTE_TASK_STK[REMOTE_STK_SIZE];
OS_EVENT * postData_Sem;
void Remote_Task(void *pdata)
{
	u8 err = 0;
	//addNewDev_Sem = OSSemCreate(0);	
	postData_Sem = OSSemCreate(0);
	OSSemPend(WiFiStatusOK_Sem, 0, &err);
	while(1)
	{
		OSSemPend(postData_Sem, 0, &err);
		_Onenet_CreateDataPoints(devices1,temperature,humidity);
	}
}
