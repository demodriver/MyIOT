#include "Nrf24l01.h"
//#include "spi.h"

uint8_t NRF24L01_RXDATA[RX_PLOAD_WIDTH];//nrf24l01���յ�������
uint8_t NRF24L01_TXDATA[RX_PLOAD_WIDTH]={0xaa,0xaf,0x03,0x05,0x03,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0x64};//nrf24l01��Ҫ���͵�����
u8  TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//���ص�ַ
u8  RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//���յ�ַ
u8 flag=0;
extern u8 data_to_send[35];
//д�Ĵ���
void NRF_Write_Reg(u8 reg,u8 data)
{
	u8 status;
	NRF_ENABLE;//ʹ��
	status=SPI2_ReadWriteByte(reg); //д��ַ
	SPI2_ReadWriteByte(data); //д����
	NRF_DISENABLE;
}
//���Ĵ���
u8 NRF_Read_Reg(u8 reg)
{
	u8 reg_val;
	NRF_ENABLE;//ʹ��
	SPI2_ReadWriteByte(reg); //д��ַ
	reg_val=SPI2_ReadWriteByte(0xff); 
	NRF_DISENABLE;
	return reg_val;
}
//*****************************************************************//
// д������
//*****************************************************************//

uint8_t NRF_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	NRF_ENABLE;				        /* ѡͨ���� */
	status = SPI2_ReadWriteByte(reg);	/* д�Ĵ�����ַ */
	for(i=0; i<uchars; i++)
	{
		SPI2_ReadWriteByte(pBuf[i]);		/* д���� */
	}
	NRF_DISENABLE;						/* ��ֹ������ */
   return 	status;	
}
/*
*****************************************************************
* ��������
*****************************************************************
*/
uint8_t NRF_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	NRF_ENABLE;						/* ѡͨ���� */
	status = SPI2_ReadWriteByte(reg);	/* д�Ĵ�����ַ */
	for(i=0; i<uchars; i++)
	{
		pBuf[i] = SPI2_ReadWriteByte(0); /* ��ȡ�������� */ 	
	}
	NRF_DISENABLE;						/* ��ֹ������ */
    return 	status;
}
/*
*****************************************************************
* д���ݰ�
*****************************************************************
*/
void NRF_TxPacket(uint8_t * tx_buf, uint8_t len)
{
	u16 i=500;
	NRF_CE_L;		 //StandBy Iģʽ	
	
	NRF_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
	NRF_Write_Buf(WR_TX_PLOAD, tx_buf, len); 			 // װ������	
	NRF_CE_H;		 //�ø�CE���������ݷ���
	//while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0)!=RESET);
}
////////////////////////////////////////////////////////////////
u8 Nrf24l01_Check(void)
{ 
	u8 buf1[5]; 
	u8 i;
	NRF_CE_L;
	/*д��5���ֽڵĵ�ַ. */ 
	NRF_Write_Buf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,5);
	/*����д��ĵ�ַ */ 
	NRF_CE_L;
	NRF_Read_Buf(TX_ADDR,buf1,5); 
	/*�Ƚ�*/ 
	for(i=0;i<5;i++) 
	{ 
		if(buf1[i]!=TX_ADDRESS[i]) 
			break; 
	} 
	if(i==5)
		return SUCCESS ; //MCU��NRF�ɹ����� 
	else
		return ERROR ; //MCU��NRF���������� 
}

//******************************************************//
void Nrf24l01_Init(u8 model, u8 ch)
{
	NRF_CE_L;
	NRF_Write_Reg(NRF_WRITE_REG+CONFIG,0x0d);//����
	NRF_Write_Reg(NRF_WRITE_REG+SETUP_AW,0x03);//��ַ���
	NRF_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);	//дRX�ڵ��ַ 
	NRF_Write_Buf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH); 		//дTX�ڵ��ַ  
	NRF_Write_Reg(NRF_WRITE_REG+EN_AA,0x01); 													//ʹ��ͨ��0���Զ�Ӧ�� 
	NRF_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);											//ʹ��ͨ��0�Ľ��յ�ַ 
	NRF_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);											//�����Զ��ط����ʱ��:500us;����Զ��ط�����:10�� 
	NRF_Write_Reg(NRF_WRITE_REG+RF_CH,0);														//����RFͨ��ΪCHANAL
	NRF_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x0f); 												//����TX�������,0db����,2Mbps,���������濪��
/////////////////////////////////////////////////////////
	if(model==1)				//RX
	{
		NRF_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);								//ѡ��ͨ��0����Ч���ݿ�� 
		NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);   		 // IRQ�շ�����жϿ���,16λCRC,������
	}
	else if(model==2)		//TX
	{
		NRF_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);								//ѡ��ͨ��0����Ч���ݿ�� 
		NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����жϿ���,16λCRC,������
	}
	else if(model==3)		//RX2
	{
		NRF_Write_Reg(FLUSH_TX,0xff);
		NRF_Write_Reg(FLUSH_RX,0xff);
		NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);   		 // IRQ�շ�����жϿ���,16λCRC,������
		
		SPI2_ReadWriteByte(0x50);
		SPI2_ReadWriteByte(0x73);
		NRF_Write_Reg(NRF_WRITE_REG+0x1c,0x01);
		NRF_Write_Reg(NRF_WRITE_REG+0x1d,0x06);
	}
	else								//TX2
	{
		NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����жϿ���,16λCRC,������
		NRF_Write_Reg(FLUSH_TX,0xff);
		NRF_Write_Reg(FLUSH_RX,0xff);
		
		SPI2_ReadWriteByte(0x50);
		SPI2_ReadWriteByte(0x73);
		NRF_Write_Reg(NRF_WRITE_REG+0x1c,0x01);
		NRF_Write_Reg(NRF_WRITE_REG+0x1d,0x06);
	}
	NRF_CE_H;
}

void spi2_Init()
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	//GPIO����������//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    /*Configure PA.4(NSS)--------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);	
	 /* SPI1 configuration */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����ģʽ
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //8λ����
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//SPI_CPOL_High=ģʽ3��ʱ�ӿ���Ϊ�� //SPI_CPOL_Low=ģʽ0��ʱ�ӿ���Ϊ��
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//SPI_CPHA_1Edge, SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//SPI_NSS_Soft;//SPI_NSS_Hard
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//SPI_BaudRatePrescaler_2=32M;//SPI_BaudRatePrescaler_4=18MHz
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ݴӸ�λ��ʼ����
  SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPI2, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	SPI_Cmd(SPI2, ENABLE); //ʹ��SPI����
}

//void EXTIX_Init(void) //IRO
//{  
//	EXTI_InitTypeDef EXTI_InitStructure;   
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
//  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
// 
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);  //GPIOE.2 �ж����Լ��жϳ�ʼ�����ã��½��ش���     
//	EXTI_InitStructure.EXTI_Line=EXTI_Line1;   
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;   
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;    
//	EXTI_Init(&EXTI_InitStructure);                      //��ʼ��
//}

//void EXTI1_IRQHandler(void) 
//{ 
//	u8 sta;
//	EXTI_ClearITPendingBit(EXTI_Line1);  //����жϱ�־λ
//	 
//  //NRF_TxPacket(NRF24L01_TXDATA, RX_PLOAD_WIDTH);    //��������
//	sta = NRF_Read_Reg(NRF_READ_REG + NRFRegSTATUS);   //��ȡ״̬
//	NRF_Write_Reg(NRF_WRITE_REG + NRFRegSTATUS, sta);  //�����־λ
//		////////////////////////////////////////////////////////////////
//	  if(sta & (1<<TX_DS))                                   //�����ж�
//	  { 
//		 flag=1;                                               //���ͳɹ�
//	  }
//		if((sta & (1<<MAX_RT)))
//		{
//			if(sta & 0x01)	                                     //TX FIFO FULL
//			{
//				NRF_Write_Reg(FLUSH_TX,0xff);
//			}
//		} 	
//}


/***************************************************************/
//SPIx 
//TxData:����һ���ֽ�
//����ֵ:data
/***************************************************************/
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //�ȴ�SPI���ͱ�־λ��
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI2, TxData); //��������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //�ȴ�SPI���ձ�־λ��
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI2); //��������					    
}

u8 TX_Funcation()
{
		u8 sta,j=250;
		NRF_TxPacket(data_to_send, RX_PLOAD_WIDTH);    //��������s	
		sta = NRF_Read_Reg(NRF_READ_REG + NRFRegSTATUS);   //��ȡ״̬
		NRF_Write_Reg(NRF_WRITE_REG + NRFRegSTATUS, sta);  //�����־λ
		////////////////////////////////////////////////////////////////
		if(sta & (1<<TX_DS))
		{
			return 1;                                    //���ͳɹ�
		}
		if((sta & (1<<MAX_RT)))
		{
			if(sta & 0x01)	                                     //TX FIFO FULL
			{
				NRF_Write_Reg(FLUSH_TX,0xff);
			}
		}
		while(j--);
		return 0;
		///////////////////////////////////////////////////////////////	
}






