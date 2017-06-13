#ifndef _NRF24L01_H
#define _NRF24L01_H
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"

//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define NRF_READ_REG        0x00  	// ���Ĵ���ָ��
#define NRF_WRITE_REG       0x20 	// д�Ĵ���ָ��
#define R_RX_PL_WID   	0x60
#define RD_RX_PLOAD     0x61  	// ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0  	// д��������ָ��
#define FLUSH_TX        0xE1 	// ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3  	// �����ظ�װ������ָ��
#define NOP             0xFF  	// ����
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define NRFRegSTATUS    0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // �ز�          
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��1�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��2�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��3�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��4�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��5�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������
//**************************************************************************************
//*********************************************NRF24L01*************************************
#define RX_DR				6		//�жϱ�־
#define TX_DS				5
#define MAX_RT			4

#define MODEL_RX				1			//��ͨ����
#define MODEL_TX				2			//��ͨ����
#define MODEL_RX2				3			//����ģʽ2,����˫����
#define MODEL_TX2				4			//����ģʽ2,����˫����

#define RX_PLOAD_WIDTH  32  	
#define TX_PLOAD_WIDTH  32  	
#define TX_ADR_WIDTH    5 	 	//���ݰ�����
#define RX_ADR_WIDTH    5   	

#define NRF_DISENABLE  GPIOB->BSRR = GPIO_Pin_12;//csƬѡ
#define NRF_ENABLE  GPIOB->BRR = GPIO_Pin_12;
#define NRF_CE_H  GPIOA->BSRR = GPIO_Pin_8;//ceʹ��
#define NRF_CE_L  GPIOA->BRR = GPIO_Pin_8;

extern uint8_t NRF24L01_RXDATA[RX_PLOAD_WIDTH];//nrf24l01���յ�������
extern uint8_t NRF24L01_TXDATA[RX_PLOAD_WIDTH];//

void Nrf24l01_Init(u8 model, u8 ch);
void NRF_Write_Reg(u8 reg,u8 data);
u8 NRF_Read_Reg(u8 reg);
uint8_t NRF_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);
uint8_t NRF_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars);
void NRF_TxPacket(uint8_t * tx_buf, uint8_t len);
u8 Nrf24l01_Check(void);
void spi2_Init(void);
u8 SPI2_ReadWriteByte(u8 TxData);
u8 TX_Funcation(void);
void EXTIX_Init(void);


#endif
