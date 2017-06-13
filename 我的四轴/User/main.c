#include "stm32f10x.h"
#include "spi.h"
#include "attitude1_calculate.h"
#include "Control.h"
#include "time.h"
#include "usart.h"
#include "Datatansfer.h"
#include "Moto.h"
#include "Rc.h"
#include "led.h"
#include "Nrf24l01.h"

MPU_value Final_senser; //
MPU_value last_value;//��һ�β���ֵ	

u8 FLAG_ATT=0;//1ms��־λ

void System_Init(void)
{
		LED_INIT();
		spi_Init();
		spi2_Init();
		Init_MPU9250();
		NVIC_INIT();
		time3_Init(500);
	  Usart1_Init(115200);
		Moto_Init();		
		MPU9250_CalOff_Gyr();
		Nrf24l01_Init(1,40);
  	if(Nrf24l01_Check())
		Uart1_Put_String("NRF24L01 IS OK !\r\n");
		else 									
	 Uart1_Put_String("NRF24L01 IS NOT OK !\r\n");
		
		LED_FLASH(); //5����˸��ʾ��ʼ�����
		Time4_Init();
		Tim3_Control(1); //ʱ��3ʹ��
	}

int main()
{
	static u8 att_cnt=0,rc_cnt=0,senser_cnt=0,
		status_cnt=0,dt_rc_cnt=0,dt_moto_cnt=0;//��̬������������־λ
	
	System_Init();
	
	NRF_CE_L;
	NRF_Write_Reg(FLUSH_RX,0xff); 
	NRF_Write_Reg(FLUSH_TX,0xff); 
	NRF_CE_H;
	
	
 	while(1)
	{		
 		if(FLAG_ATT)
		{
			FLAG_ATT=0; 
			att_cnt++;
			rc_cnt++;
			
			LED1_ONOFF(); //led��˸��ʾ��������
			LED2_ONOFF(); 
			
			if(rc_cnt==20)    //ÿ20ms��ȡң������
			{
				rc_cnt = 0;
				#ifdef CONTROL_USE_RC
				Rc_GetValue(&Rc_Data); 
				#endif
				Rc_Fun(&Rc_Data,&Rc_Control);
			}
			
			if(att_cnt==1)           //ÿ2ms������̬
			{
				last_value=current_value;
			}	
			else
			{
				att_cnt=0;													
				attitude_calculate(current_value);              //��̬����	
				Control(&Q_ANGLE,&Rc_Data,Rc_Control,&Final_senser);          //����			
			}
			
				senser_cnt++;
				status_cnt++;
				dt_rc_cnt++;
				dt_moto_cnt++;

				if(senser_cnt==3)  //ÿ3ms�ϴ�һ�δ���������
				{
					senser_cnt = 0;
					Send_Senser = 1;
				}
				if(status_cnt==5) //ÿ5ms�ϴ�һ����̬����
				{
					status_cnt = 0;
					Send_Status = 1;
				}
				if(dt_rc_cnt==9)  //ÿ10ms�ϴ�һ��ң������
				{
					dt_rc_cnt=0;
					Send_RCData = 1;
				}
				if(dt_moto_cnt==7) //ÿ7ms�ϴ�һ�ε������
				{
					dt_moto_cnt=0;
					Send_MotoPwm = 1;
				}			
		}
	}
	
}
