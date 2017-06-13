#ifndef _REMOTE_H
#define _REMOTE_H
#include "includes.h"
#include "EdpKit.h"

/*empty string define*/
#define Msg_NULL	""

/*http packet size define*/
#define httpPacketSize 256

/*request method define*/
#define GET         "GET"
#define POST        "POST"
#define PUT         "PUT"
#define DELETE      "DELETE"


/*request host & port define*/
#define   DEVICEID   "3017351"
#define   APIKEY     "CGYhwTphnl6MDWgXuW1=vHyGpTc="
#define   Host       "jjfarfapi.heclouds.com"	//"192.168.1.104"//"www.baidu.com"
#define   Port       "80"


#define   MAX_SEND_BUF_LEN  1024

#define REMOTE_TASK_PRIO      			    		7 	//设置任务优先级
#define REMOTE_STK_SIZE  				        	256	//设置任务栈大小
extern OS_STK REMOTE_TASK_STK[REMOTE_STK_SIZE];		    //任务栈	
extern void Remote_Task(void *pdata);

extern OS_EVENT * postData_Sem;
extern OS_EVENT * addNewDev_Sem;

extern    EdpPacket* send_pkg;
extern    char send_buf[MAX_SEND_BUF_LEN];
extern  void SendCmd(char* cmd, char* result, int timeOut);
extern  void ESP8266_DevLink(const char* devid, const char* auth_key);

///*yeelink Http buf format define*/
//char * HttpHead_GetApiKey = 
//"GET /v1.0/user/apikey?username=%s&pass=%s HTTP/1.1\r\n"            //fill with username & pass
//"Host: %s\r\n"                                                      //fill with host
//"Accept: */*\r\n"
//"Content-Length: 0\r\n"
//"Connection: close\r\n"
//"\r\n";
#endif

