#ifndef __WIFI_FUNCTION_H
#define	__WIFI_FUNCTION_H 

#include "stm32f10x.h"
#include "wifi_config.h"
#include "usart.h"
#include "includes.h"
#include <stdbool.h>
#define SLAVE_NODE //从机模式

#define     ESP8266_Usart( fmt, ... )           USART_OUT ( USART2, fmt, ##__VA_ARGS__ )

#define     ESP8266_CH_HIGH_LEVEL()         {}//    GPIO_SetBits( GPIOA, GPIO_Pin_0 )
#define     ESP8266_CH_LOW_LEVEL()          {}//    GPIO_ResetBits( GPIOA, GPIO_Pin_0 )

#define     ESP8266_RST_HIGH_LEVEL()        {} //  GPIO_SetBits( GPIOA, GPIO_Pin_1 )
#define     ESP8266_RST_LOW_LEVEL()         {} //  GPIO_ResetBits( GPIOA, GPIO_Pin_1 )

#define WIFI_TASK_PRIO      			    3 	//设置任务优先级
#define WIFI_STK_SIZE  				        128	//设置任务栈大小
extern OS_STK WIFI_TASK_STK[WIFI_STK_SIZE];		    //任务栈	
void WiFi_Task(void *pdata);

extern OS_EVENT * WiFiRecv_Sem;
extern OS_EVENT * AirKiss_Sem;
extern OS_EVENT * WiFiStatusOK_Sem;
typedef enum
{
	LINKING,
	OK,
	FAIL
}wifiStatus_e;

void        ESP8266_Choose                      ( FunctionalState enumChoose );
bool        ESP8266_Rst                         ( void );
void        ESP8266_AT_Test                     ( void );
bool        ESP8266_Cmd                         ( char * cmd, char * reply1, char * reply2, u32 waittime, u8 retry );
bool        ESP8266_Net_Mode_Choose             ( ENUM_Net_ModeTypeDef enumMode );
bool        ESP8266_JoinAP                      ( char * pSSID, char * pPassWord );
bool        ESP8266_BuildAP                     ( char * pSSID, char * pPassWord, char * enunPsdMode );
bool        ESP8266_Enable_MultipleId           ( FunctionalState enumEnUnvarnishTx );
bool        ESP8266_Link_Server                 ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
bool        ESP8266_StartOrShutServer           ( FunctionalState enumMode, char * pPortNum, char * pTimeOver );
bool 		ESP8266_SMARTConfig					( ENUM_SMART_TypeDef enumSmartMode );
bool 		ESP8266_CheckLinkStatus				( void );
bool 		ESP8266_AutoConnect					( void );
bool 		ESP8266_ClsTCP						( void );
bool    ESP8266_GetIP             (void);
bool    ESP8266_SetMode           (void);

#endif    /* __WIFI_FUNCTION_H */





