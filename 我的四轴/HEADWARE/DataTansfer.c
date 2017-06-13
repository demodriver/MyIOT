#include "Datatansfer.h"
#include "Control.h"
#include "usart.h"
#include "spi.h"
#include "Moto.h"
#include "Nrf24L01.h"

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))
	
extern Out_Angel Q_ANGLE; //������̬��
extern MPU_value Final_senser;
T_RC_Data  Rc_send_Data; //ң��ͨ������
u8 Data_Check,Send_Status,Send_Senser,Send_RCData,Send_Offset,Send_PID1,Send_PID2,Send_PID3,Send_MotoPwm;
u8 data_to_send[50];

//***********************************************************//
//NRF�¼����
//***********************************************************//
void Nrf_Check_Event(void)
{
	
//*************************************************************//
	u8 rx_l=0,sta=0;	 	
	sta = NRF_Read_Reg(NRF_READ_REG + NRFRegSTATUS); 
	NRF_Write_Reg(NRF_WRITE_REG + NRFRegSTATUS,sta);  	
	if(sta & (0x01<<RX_DR))                                  //�����ж�
	{
		rx_l = NRF_Read_Reg(R_RX_PL_WID);              //���ݳ��ȶ�ȡ
 		if(rx_l<33)
		{
			NRF_Read_Buf(RD_RX_PLOAD,NRF24L01_RXDATA,rx_l); // read receive payload from RX_FIFO buffer
			Data_Receive_Anl(NRF24L01_RXDATA,17);         //���ݷ���
		}
		else 
		{
			NRF_CE_L;
			NRF_Write_Reg(FLUSH_RX,0xff);                     //��ջ�����
			NRF_CE_H;
		}
	}
//*************************************************************//
	if(sta & (0x01<<TX_DS))                                   //�����ж�
	{
			
	}
	////////////////////////////////////////////////////////////////
	if(sta & (1<<MAX_RT))
	{
		if(sta & 0x01)	                                     //TX FIFO FULL
		{
			NRF_Write_Reg(FLUSH_TX,0xff);
		}
	}		
	////////////////////////////////////////////////////////////////
}
//*****************NRF FIFO status***************************//
u8 Nrf_Get_FIFOSta(void)
{
	return NRF_Read_Reg(NRF_READ_REG + FIFO_STATUS);
}
//***********************************************//
//���ݽ��շ���
//***********************************************//
void Data_Receive_Anl(u8 *data_buf,u8 len)
{
	u8 sum = 0,i;
	
	for(i=0;i<(len-1);i++)
	{
		sum += *(data_buf+i);
	}		
	if(!(sum==*(data_buf+len-1)))		return;		     //�ж�sum
	if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAF))		return;		//�ж�֡ͷ
//////////////////////////////////////////////////////////////////
	if(*(data_buf+2)==0X01)
	{
		if(*(data_buf+4)==0X01) 
			MPU9250_CalOff_Acc();  //accУ׼
		if(*(data_buf+4)==0X02)
			MPU9250_CalOff_Gyr();  //gyroУ׼
		if(*(data_buf+4)==0X03)
		{
			MPU9250_CalOff_Acc();
			MPU9250_CalOff_Gyr();
		}
//		if(*(data_buf+4)==0X04)
//		{
//			//Cal_Compass();       //magУ׼
//		}		
//		if(*(data_buf+4)==0X05)
//		{
//			//MS5611_CalOffset();  //��ѹУ׼
//		}
	}
	if(*(data_buf+2)==0X02)
	{
		if(*(data_buf+4)==0X01) //���󷵻�PID
		{
			Send_PID1 = 1;
			//Send_PID2 = 1;
		//	Send_PID3 = 1;
		}
//		if(*(data_buf+4)==0X02)
//			Send_Offset = 1;   //���󷵻�offset
	}
#ifdef CONTROL_USE_RC      //����ң��������
	
	if(*(data_buf+2)==0X03)
	{
		Rc_send_Data.THROTTLE = (vs16)(*(data_buf+4)<<8)|*(data_buf+5);
		Rc_send_Data.YAW = (vs16)(*(data_buf+6)<<8)|*(data_buf+7);
		Rc_send_Data.ROL = (vs16)(*(data_buf+8)<<8)|*(data_buf+9);
		Rc_send_Data.PIT = (vs16)(*(data_buf+10)<<8)|*(data_buf+11);
		Rc_send_Data.AUX1 = (vs16)(*(data_buf+12)<<8)|*(data_buf+13);
		Rc_send_Data.AUX2 = (vs16)(*(data_buf+14)<<8)|*(data_buf+15);
//		Rc_send_Data.AUX3 = (vs16)(*(data_buf+16)<<8)|*(data_buf+17);
//		Rc_send_Data.AUX4 = (vs16)(*(data_buf+18)<<8)|*(data_buf+19);
//		Rc_send_Data.AUX5 = (vs16)(*(data_buf+20)<<8)|*(data_buf+21);
//		Rc_send_Data.AUX6 = (vs16)(*(data_buf+21)<<8)|*(data_buf+22);
		//Rc_Fun(&Rc_Data,&Rc_Control);
	}
#endif
//	if(*(data_buf+2)==0X10)								//����PID
//	{
//		  USART_ITConfig(USART1, USART_IT_TXE, DISABLE); //�رշ����ж�
////			PID_ROL.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/1000;
////			PID_ROL.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/1000;
////			PID_ROL.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/1000;
////		
////			PID_PIT.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/1000;
////			PID_PIT.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/1000;
////			PID_PIT.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/1000;
////		
////			PID_YAW.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/1000;
////			PID_YAW.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/1000;
////			PID_YAW.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/1000;
////			Data_Send_Check(sum);  //������ͬ����
//	}
//	if(*(data_buf+2)==0X11)								//PID2
//	{
//			PID_ALT.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
//			PID_ALT.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
//			PID_ALT.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
//			PID_POS.P = (float)((vs16)(*(data_buf+10)<<8)|*(data_buf+11))/100;
//			PID_POS.I = (float)((vs16)(*(data_buf+12)<<8)|*(data_buf+13))/100;
//			PID_POS.D = (float)((vs16)(*(data_buf+14)<<8)|*(data_buf+15))/100;
//			PID_PID_1.P = (float)((vs16)(*(data_buf+16)<<8)|*(data_buf+17))/100;
//			PID_PID_1.I = (float)((vs16)(*(data_buf+18)<<8)|*(data_buf+19))/100;
//			PID_PID_1.D = (float)((vs16)(*(data_buf+20)<<8)|*(data_buf+21))/100;
//			Data_Send_Check(sum);
//	}
//	if(*(data_buf+2)==0X12)								//PID3
//	{
//			PID_PID_2.P = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/100;
//			PID_PID_2.I = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/100;
//			PID_PID_2.D = (float)((vs16)(*(data_buf+8)<<8)|*(data_buf+9))/100;
//			Data_Send_Check(sum);
//			EE_SAVE_PID();
//	if(*(data_buf+2)==0X16)								//OFFSET
//	{
//			offset_value. = (float)((vs16)(*(data_buf+4)<<8)|*(data_buf+5))/1000;
//			offset_value = (float)((vs16)(*(data_buf+6)<<8)|*(data_buf+7))/1000;
//	}
//	
}
//***********************************************//
//���ݽ���
//***********************************************//
void Data_Exchange(void)
{
	
#ifdef DATA_TRANSFER_USE_SPI_NRF	
	u8 sta; 
	Nrf_Check_Event();             //NRF�¼���� 
	sta = Nrf_Get_FIFOSta();
	if((sta &(1<<MAX_RT))==0) return;
#endif
	if(Send_Status)
	{
		Send_Status = 0;
		Data_Send_Status(); //����״̬
	}
	else if(Send_Senser)
	{
		Send_Senser = 0;
		Data_Send_Senser(); //����������
	}
	else if(Send_PID1)
	{
		Send_PID1 = 0;
		//Data_Send_PID1();  //pid1����
	}
//	else if(Send_PID2)
//	{
//		Send_PID2 = 0;
//		Data_Send_PID2();
//	}
//	else if(Send_PID3)
//	{
//		Send_PID3 = 0;
//		Data_Send_PID3();
//	}
	else if(Send_RCData)
	{
		Send_RCData = 0;
		Data_Send_RCData(); //ң������
	}
//	else if(Send_Offset)
//	{
//		Send_Offset = 0;
//		Data_Send_OFFSET(); //��Ư
//	}
	else if(Send_MotoPwm)
	{
		Send_MotoPwm = 0;
		Data_Send_MotoPWM();   //PWM
	}
}
//****************************************************//
//�ɿ�to��λ��
//****************************************************//
void Data_Send_Status(void)
{
	u8 _cnt=0;
	u8 sum = 0,i;
	vs16 _temp;
	data_to_send[_cnt++]=0xAA;//֡ͷ
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x01;//������
	data_to_send[_cnt++]=0;

	_temp = (int)(Q_ANGLE.ROL*100); //�������
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = (int)(Q_ANGLE.PIT*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
	_temp = (int)(Q_ANGLE.YAW*100);
	data_to_send[_cnt++]=BYTE1(_temp);
	data_to_send[_cnt++]=BYTE0(_temp);
//	vs32 _temp2 = Alt;
//	data_to_send[_cnt++]=BYTE3(_temp2);
//	data_to_send[_cnt++]=BYTE2(_temp2);
//	data_to_send[_cnt++]=BYTE1(_temp2);
//	data_to_send[_cnt++]=BYTE0(_temp2);
/////////////////////////////////////////////////////////////////		
	if(Rc_Control.ARMED==0)			data_to_send[_cnt++]=0xA0;	//����״̬����
	else if(Rc_Control.ARMED==1)		data_to_send[_cnt++]=0xA1;
////////////////////////////////////////////////////////////////	
	data_to_send[3] = _cnt-4;//���ݳ���
	
	
	for(i=0;i<_cnt;i++) //У���
		sum += data_to_send[i];
	data_to_send[_cnt++]=sum;
//*******************************************//
//���ݷ���ѡ��(suart & NRF)
#ifdef DATA_TRANSFER_USE_USART
	Uart1_Put_Buf(data_to_send,_cnt); 
#else
	NRF_TxPacket(data_to_send,_cnt);
#endif
}
////////////////////////////////////////////
void Data_Send_Senser(void)
{
	u8 _cnt=0;
	u8 sum = 0,i;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x02;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(Final_senser.Accel[0]);
	data_to_send[_cnt++]=BYTE0(Final_senser.Accel[0]);
	data_to_send[_cnt++]=BYTE1(Final_senser.Accel[1]);
	data_to_send[_cnt++]=BYTE0(Final_senser.Accel[1]);
	data_to_send[_cnt++]=BYTE1(Final_senser.Accel[2]);
	data_to_send[_cnt++]=BYTE0(Final_senser.Accel[2]);
	data_to_send[_cnt++]=BYTE1(Final_senser.Gyro[0]);
	data_to_send[_cnt++]=BYTE0(Final_senser.Gyro[0]);
	data_to_send[_cnt++]=BYTE1(Final_senser.Gyro[1]);
	data_to_send[_cnt++]=BYTE0(Final_senser.Gyro[1]);
	data_to_send[_cnt++]=BYTE1(Final_senser.Gyro[2]);
	data_to_send[_cnt++]=BYTE0(Final_senser.Gyro[2]);
//	data_to_send[_cnt++]=BYTE1(Final_senser.Mag[0]);
//	data_to_send[_cnt++]=BYTE0(Final_senser.Mag[0]);
//	data_to_send[_cnt++]=BYTE1(Final_senser.Mag[1]);
//	data_to_send[_cnt++]=BYTE0(Final_senser.Mag[1]);
//	data_to_send[_cnt++]=BYTE1(Final_senser.Mag[2]);
//	data_to_send[_cnt++]=BYTE0(Final_senser.Mag[2]);
	
	data_to_send[3] = _cnt-4;
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	data_to_send[_cnt++] = sum;
	
#ifdef DATA_TRANSFER_USE_USART
	Uart1_Put_Buf(data_to_send,_cnt);
#else
	NRF_TxPacket(data_to_send,_cnt);
#endif
}
//////////////////////////////////////////
void Data_Send_RCData(void)
{
	u8 _cnt=0;
	u8 sum = 0,i;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x03;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(Rc_Data.THROTTLE);
	data_to_send[_cnt++]=BYTE0(Rc_Data.THROTTLE);
	data_to_send[_cnt++]=BYTE1(Rc_Data.YAW);
	data_to_send[_cnt++]=BYTE0(Rc_Data.YAW);
	data_to_send[_cnt++]=BYTE1(Rc_Data.ROL);
	data_to_send[_cnt++]=BYTE0(Rc_Data.ROL);
	data_to_send[_cnt++]=BYTE1(Rc_Data.PIT);
	data_to_send[_cnt++]=BYTE0(Rc_Data.PIT);
	data_to_send[_cnt++]=0;//BYTE1(Rc_Data.AUX1);
	data_to_send[_cnt++]=0;//BYTE0(Rc_Data.AUX1);
	data_to_send[_cnt++]=0;//BYTE1(Rc_Data.AUX2);
	data_to_send[_cnt++]=0;//BYTE0(Rc_Data.AUX2);
	data_to_send[_cnt++]=0;//BYTE1(Rc_Data.AUX3);
	data_to_send[_cnt++]=0;//BYTE0(Rc_Data.AUX3);
	data_to_send[_cnt++]=0;//BYTE1(Rc_Data.AUX4);
	data_to_send[_cnt++]=0;//BYTE0(Rc_Data.AUX4);
	data_to_send[_cnt++]=0;//BYTE1(Rc_Data.AUX5);
	data_to_send[_cnt++]=0;//BYTE0(Rc_Data.AUX5);
	data_to_send[_cnt++]=0;//BYTE1(Rc_Data.AUX6);
	data_to_send[_cnt++]=0;//BYTE0(Rc_Data.AUX6);
	
	data_to_send[3] = _cnt-4;
	
	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
#ifdef DATA_TRANSFER_USE_USART
	Uart1_Put_Buf(data_to_send,_cnt);
#else
	NRF_TxPacket(data_to_send,_cnt);
#endif
}
///////////////////////////////////////////////
void Data_Send_MotoPWM(void)
{
	u8 _cnt=0;
	u8 sum = 0,i;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0xAA;
	data_to_send[_cnt++]=0x06;
	data_to_send[_cnt++]=0;
	data_to_send[_cnt++]=BYTE1(Moto_PWM_1);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_1);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_2);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_2);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_3);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_3);
	data_to_send[_cnt++]=BYTE1(Moto_PWM_4);
	data_to_send[_cnt++]=BYTE0(Moto_PWM_4);
	data_to_send[_cnt++]=0;//BYTE1(Moto_PWM_5);
	data_to_send[_cnt++]=0;//BYTE0(Moto_PWM_5);
	data_to_send[_cnt++]=0;//BYTE1(Moto_PWM_6);
	data_to_send[_cnt++]=0;//BYTE0(Moto_PWM_6);
	data_to_send[_cnt++]=0;//BYTE1(Moto_PWM_7);
	data_to_send[_cnt++]=0;//BYTE0(Moto_PWM_7);
	data_to_send[_cnt++]=0;//BYTE1(Moto_PWM_8);
	data_to_send[_cnt++]=0;//BYTE0(Moto_PWM_8);
	
	data_to_send[3] = _cnt-4;

	for(i=0;i<_cnt;i++)
		sum += data_to_send[i];
	
	data_to_send[_cnt++]=sum;
	
#ifdef DATA_TRANSFER_USE_USART
	Uart1_Put_Buf(data_to_send,_cnt);
#else
	NRF_TxPacket(data_to_send,_cnt);
#endif
}
//void Data_Send_OFFSET(void)
//{
//	u8 _cnt=0;
//	data_to_send[_cnt++]=0xAA;
//	data_to_send[_cnt++]=0xAA;
//	data_to_send[_cnt++]=0x16;
//	data_to_send[_cnt++]=0;
//	vs16 _temp = AngleOffset_Rol*1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = AngleOffset_Pit*1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	
//	data_to_send[3] = _cnt-4;
//	
//	u8 sum = 0;
//	for(u8 i=0;i<_cnt;i++)
//		sum += data_to_send[i];
//	
//	data_to_send[_cnt++]=sum;
//	
//#ifdef DATA_TRANSFER_USE_USART
//	Uart1_Put_Buf(data_to_send,_cnt);
//#else
//	NRF_TxPacket(data_to_send,_cnt);
//#endif
//}
//void Data_Send_PID1(void)
//{
//	u8 _cnt=0;
//	vs16 _temp;
//	u8 sum = 0,i;
//	data_to_send[_cnt++]=0xAA;
//	data_to_send[_cnt++]=0xAA;
//	data_to_send[_cnt++]=0x10;
//	data_to_send[_cnt++]=0;
//	
//	_temp = PID_ROL.P * 1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_ROL.I * 1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_ROL.D * 1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_PIT.P * 1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_PIT.I * 1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_PIT.D * 1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_YAW.P * 1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_YAW.I * 1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_YAW.D * 1000;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	
//	data_to_send[3] = _cnt-4;
//	

//	for(i=0;i<_cnt;i++)
//		sum += data_to_send[i];
//	
//	data_to_send[_cnt++]=sum;
//	
//#ifdef DATA_TRANSFER_USE_USART
//	Uart1_Put_Buf(data_to_send,_cnt);
//#else
//	NRF_TxPacket(data_to_send,_cnt);
//#endif
//}
//void Data_Send_PID2(void)
//{
//	u8 _cnt=0;
//	data_to_send[_cnt++]=0xAA;
//	data_to_send[_cnt++]=0xAA;
//	data_to_send[_cnt++]=0x11;
//	data_to_send[_cnt++]=0;
//	
//	vs16 _temp;
//	_temp = PID_ALT.P * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_ALT.I * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_ALT.D * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_POS.P * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_POS.I * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_POS.D * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_PID_1.P * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_PID_1.I * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_PID_1.D * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	
//	data_to_send[3] = _cnt-4;
//	
//	u8 sum = 0;
//	for(u8 i=0;i<_cnt;i++)
//		sum += data_to_send[i];
//	
//	data_to_send[_cnt++]=sum;
//	
//#ifdef DATA_TRANSFER_USE_USART
//	Uart1_Put_Buf(data_to_send,_cnt);
//#else
//	NRF_TxPacket(data_to_send,_cnt);
//#endif
//}
//void Data_Send_PID3(void)
//{
//	u8 _cnt=0;
//	data_to_send[_cnt++]=0xAA;
//	data_to_send[_cnt++]=0xAA;
//	data_to_send[_cnt++]=0x12;
//	data_to_send[_cnt++]=0;
//	
//	vs16 _temp;
//	_temp = PID_PID_2.P * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_PID_2.I * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	_temp = PID_PID_2.D * 100;
//	data_to_send[_cnt++]=BYTE1(_temp);
//	data_to_send[_cnt++]=BYTE0(_temp);
//	
//	data_to_send[3] = _cnt-4;
//	
//	u8 sum = 0;
//	for(u8 i=0;i<_cnt;i++)
//		sum += data_to_send[i];
//	
//	data_to_send[_cnt++]=sum;
//	
//#ifdef DATA_TRANSFER_USE_USART
//	Uart1_Put_Buf(data_to_send,_cnt);
//#else
//	NRF_TxPacket(data_to_send,_cnt);
//#endif
//}

void Data_Send_Check(u16 check)  
{
	u8 sum = 0,i;
	data_to_send[0]=0xAA;
	data_to_send[1]=0xAA;
	data_to_send[2]=0xF0;
	data_to_send[3]=3;
	data_to_send[4]=0xBA;
	
	data_to_send[5]=BYTE1(check);
	data_to_send[6]=BYTE0(check);
	
	for(i=0;i<7;i++)
		sum += data_to_send[i];
	
	data_to_send[7]=sum;
#ifdef DATA_TRANSFER_USE_USART
	Uart1_Put_Buf(data_to_send,8);
#else
	NRF_TxPacket(data_to_send,8);
#endif
}


