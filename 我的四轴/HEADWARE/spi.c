#include "spi.h"
u16 BUF[6];       //�������ݻ�����      
//u16 BUF_MAG[6];       
MPU_value current_value;          //9������
MPU_value offset_value;           //��Ư
u8 GYRO_OFFSET_OK,ACC_OFFSET_OK; 
/***************************************************************/
void spi_Init()
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	
	//GPIO����������//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
    /*Configure PA.4(NSS)--------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	 /* SPI1 configuration */ 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����ģʽ
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; //8λ����
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//SPI_CPOL_High=ģʽ3��ʱ�ӿ���Ϊ�� //SPI_CPOL_Low=ģʽ0��ʱ�ӿ���Ϊ��
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//SPI_CPHA_2Edge;//SPI_CPHA_1Edge, SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//SPI_NSS_Soft;//SPI_NSS_Hard
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//SPI_BaudRatePrescaler_2=32M;//SPI_BaudRatePrescaler_4=18MHz
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ݴӸ�λ��ʼ����
  SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
}
/***************************************************************/
//SPIx 
//TxData:����һ���ֽ�
//����ֵ:data
/***************************************************************/
u8 SPI1_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //�ȴ�SPI���ͱ�־λ��
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, TxData); //��������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //�ȴ�SPI���ձ�־λ��
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); //��������					    
}
/***************************************************************/
//SPI����
//reg: addr
//value:����
/***************************************************************/
u8 MPU9250_Write_Reg(u8 reg,u8 value)
{
	u8 status;
		MPU_9250_ENABLE;   //	MPU9250_CS=0;  //ƬѡMPU9250
	status=SPI1_ReadWriteByte(reg); //����reg��ַ
	SPI1_ReadWriteByte(value);//��������
	MPU_9250_DISENABLE;//	MPU9250_CS=1;  //ʧ��MPU9250
	return(status);//
}
//---------------------------------------------------------------//
//SPI��ȡ
//reg: addr
u8 MPU9250_Read_Reg(u8 reg)
{
	  u8 reg_val;
		MPU_9250_ENABLE;//	MPU9250_CS=0;  //ƬѡMPU9250
	  SPI1_ReadWriteByte(reg|0x80); //reg��ַ+������
	  reg_val=SPI1_ReadWriteByte(0xff);//��������
		MPU_9250_DISENABLE;//	MPU9250_CS=1;  //ʧ��MPU9250
	return(reg_val);
}
/***************************************************************/
// MPU�ڲ�i2c д��
//I2C_SLVx_ADDR:  MPU9250_AK8963_ADDR
//I2C_SLVx_REG:   reg
//I2C_SLVx_Data out:  value
/***************************************************************/
static void i2c_Mag_write(u8 reg,u8 value)
{
	MPU9250_Write_Reg(I2C_SLV0_REG ,reg);//set reg addr
	MPU9250_Write_Reg(I2C_SLV0_DO ,value);//send value	
}
/***************************************************************/
// MPU�ڲ�i2c ��ȡ
//I2C_SLVx_ADDR:  MPU9250_AK8963_ADDR
//I2C_SLVx_REG:   reg
//return value:   EXT_SENS_DATA_00 register value
/***************************************************************/
static u8 i2c_Mag_read(u8 reg)
{
	MPU9250_Write_Reg(I2C_SLV0_REG ,reg);// set reg addr
	MPU9250_Write_Reg(I2C_SLV0_DO ,0xff);//read
	//return MPU9250_Read_Reg(EXT_SENS_DATA_00);
}

//****************��ʼ��MPU9250��������Ҫ��ο�pdf�����޸�************************
void Init_MPU9250(void)
{	
	MPU9250_Write_Reg(PWR_MGMT_1, 0x00);	//�������״̬	
/*******************Init GYRO and ACCEL******************************/	
	MPU9250_Write_Reg(SMPLRT_DIV, 0x07);  // (SAMPLE_RATE= Internal_Sample_Rate / (1 + SMPLRT_DIV) )
	MPU9250_Write_Reg(SPI_CONFIG, 0x06);      //��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)�˼Ĵ����ھ���Internal_Sample_Rate==8K
	MPU9250_Write_Reg(GYRO_CONFIG, 0x08); //�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)
	MPU9250_Write_Reg(ACCEL_CONFIG, 0x00);//���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x00(���Լ죬2G)
	MPU9250_Write_Reg(ACCEL_CONFIG_2, 0x06);//���ټƸ�ͨ�˲�Ƶ�� ����ֵ ��0x06  ��5Hz��	

/**********************Init SLV0 i2c**********************************/	
//Use SPI-bus read slave0
//	MPU9250_Write_Reg(INT_PIN_CFG ,0x30);// INT Pin / Bypass Enable Configuration  
//	MPU9250_Write_Reg(I2C_MST_CTRL,0x4d);//I2C MAster mode and Speed 400 kHz
//	MPU9250_Write_Reg(USER_CTRL ,0x20); // I2C_MST _EN 
//	MPU9250_Write_Reg(I2C_MST_DELAY_CTRL ,0x01);//��ʱʹ��I2C_SLV0 _DLY_ enable 	
//	MPU9250_Write_Reg(I2C_SLV0_CTRL ,0x86); //enable IIC	and EXT_SENS_DATA==1 Byte
	
/**********************Init MAG **********************************/
//	MPU9250_Write_Reg(I2C_SLV0_ADDR ,MPU9250_AK8963_ADDR);//���ô����Ƶ�ַ,mode: write
//	
//	i2c_Mag_write(AK8963_CNTL2_REG,AK8963_CNTL2_SRST); // Reset AK8963
//	i2c_Mag_write(AK8963_CNTL1_REG,0x12); // use i2c to set AK8963 working on single measurement mode1 & 16-bit output
//	
//	MPU9250_Write_Reg(I2C_SLV0_ADDR ,MPU9250_AK8963_ADDR|0x80); //���ô����� mode��read
	
/*******************************************************************/
   BUF[0]=MPU9250_Read_Reg(XA_OFFSET_L);  //��ƯУ׼
   BUF[1]=MPU9250_Read_Reg(XA_OFFSET_H);
	 offset_value.Accel[0]=   ((BUF[1]<<8)|BUF[0]);
	 
   BUF[2]=MPU9250_Read_Reg(YA_OFFSET_L); 
   BUF[3]=MPU9250_Read_Reg(YA_OFFSET_H);
	 offset_value.Accel[1]=   ((BUF[3]<<8)|BUF[2]);
		
	 BUF[4]=MPU9250_Read_Reg(ZA_OFFSET_L); 
   BUF[5]=MPU9250_Read_Reg(ZA_OFFSET_H);
	 offset_value.Accel[2]=   ((BUF[5]<<8)|BUF[4]);
	 /////////////////////////////////////////////////
}

//************************���ٶȶ�ȡ**************************/
void READ_MPU9250_ACCEL(void)//
{ 
	
   BUF[0]=MPU9250_Read_Reg(ACCEL_XOUT_L); 
   BUF[1]=MPU9250_Read_Reg(ACCEL_XOUT_H);
   current_value.Accel[0]=	((BUF[1]<<8)|BUF[0]);
	 current_value.Accel[0]-=(short)offset_value.Accel[0]/8;
   					   //��ȡ����X������  16384=2^16/4  4=2*2  4Ϊ����   16384Ϊ��С�ֱ���
   BUF[2]=MPU9250_Read_Reg(ACCEL_YOUT_L);
   BUF[3]=MPU9250_Read_Reg(ACCEL_YOUT_H);
   current_value.Accel[1]=	((BUF[3]<<8)|BUF[2]);
	 current_value.Accel[1]-=(short)offset_value.Accel[1]/8;
 					   //��ȡ����Y������
   BUF[4]=MPU9250_Read_Reg(ACCEL_ZOUT_L); 
   BUF[5]=MPU9250_Read_Reg(ACCEL_ZOUT_H);
   current_value.Accel[2]=  ((BUF[5]<<8)|BUF[4]);
	 current_value.Accel[2]-=(short)offset_value.Accel[2]/8;
	 	      //��ȡ����Z������ 
}
/**********************�����Ƕ�ȡ*****************************/
void READ_MPU9250_GYRO(void)
{ 

   BUF[0]=MPU9250_Read_Reg(GYRO_XOUT_L); 
   BUF[1]=MPU9250_Read_Reg(GYRO_XOUT_H);
   current_value.Gyro[0]=	((BUF[1]<<8)|BUF[0]);
	 current_value.Gyro[0]-=offset_value.Gyro[0];
   				   //��ȡ����X������

   BUF[2]=MPU9250_Read_Reg(GYRO_YOUT_L);
   BUF[3]=MPU9250_Read_Reg(GYRO_YOUT_H);
   current_value.Gyro[1]=	((BUF[3]<<8)|BUF[2]);
	 current_value.Gyro[1]-=offset_value.Gyro[1];
 					   //��ȡ����Y������
   BUF[4]=MPU9250_Read_Reg(GYRO_ZOUT_L);
   BUF[5]=MPU9250_Read_Reg(GYRO_ZOUT_H);
   current_value.Gyro[2]=	((BUF[5]<<8)|BUF[4]);
	 current_value.Gyro[2]-=offset_value.Gyro[2];
   			       //��ȡ����Z������
}
/**********************************************************/
//void GET_MPU9250_MAG(void)
//{
//	u8 str,i;
//	for(str=EXT_SENS_DATA_00,i=0;str<(EXT_SENS_DATA_00+1);str++,i++)
//	{
//		BUF_MAG[i]=MPU9250_Read_Reg(str);
//	}	
//	current_value.Mag[0]=((BUF_MAG[1]<<8)|BUF_MAG[0]);
//	current_value.Mag[1]=((BUF_MAG[3]<<8)|BUF_MAG[2]);
//	current_value.Mag[2]=((BUF_MAG[5]<<8)|BUF_MAG[4]);
//}

/**********************�����ƶ�ȡ***************************/
//void READ_MPU9250_MAG(void)
//{ 
//			//i2c_Mag_read(AK8963_WHOAMI_REG);
//	   // i2c_Mag_read(0x0a);
//			//GET_MPU9250_MAG();
//			//BUF_MAG[0]=MPU9250_Read_Reg(EXT_SENS_DATA_00);
////	  if(AK8963_ST1_DRDY&i2c_Mag_read(AK8963_ST1_REG)==1) 
////		{
//			i2c_Mag_read(MAG_XOUT_L);
//			i2c_Mag_read(MAG_XOUT_H);
//			i2c_Mag_read(MAG_YOUT_L);
//			i2c_Mag_read(MAG_YOUT_H);
//			i2c_Mag_read(MAG_ZOUT_L);
//			i2c_Mag_read(MAG_ZOUT_H);
//			i2c_Mag_read(AK8963_ST2_REG);
//			GET_MPU9250_MAG();
//		//}
//		//else{}
//}

/**********************************************************/

void MPU_DATANGEL(void)
{
		if(!GYRO_OFFSET_OK)//������ ��ƫ����
	{
		static int32_t tempgx=0,tempgy=0,tempgz=0;
		static uint8_t cnt_g=0;
		if(cnt_g==0)//�ս��� �Ĵ�������
		{
			offset_value.Gyro[0]=0;
			offset_value.Gyro[1]=0;
			offset_value.Gyro[2]=0;
			tempgx = 0;
			tempgy = 0;
			tempgz = 0;
			cnt_g = 1;
			return ;
		}
		//6050�����ۼ�
		tempgx+= current_value.Gyro[0];
		tempgy+= current_value.Gyro[1];
		tempgz+= current_value.Gyro[2];
		if(cnt_g==200)//�����ٴ� ��ƽ��
		{
			offset_value.Gyro[0]=tempgx/cnt_g;
			offset_value.Gyro[1]=tempgy/cnt_g;
			offset_value.Gyro[2]=tempgz/cnt_g;
			cnt_g = 0;
			GYRO_OFFSET_OK = 1;//������ɱ�־
			return ;
		}
		cnt_g++;
	}
//	if(!ACC_OFFSET_OK)//���ٶȴ��������� ��ƫ���ݼ���
//	{
//		static int32_t	tempax=0,tempay=0,tempaz=0;
//		static uint8_t cnt_a=0;
//		if(cnt_a==0)
//		{
//			offset_value.Accel[0] = 0;
//			offset_value.Accel[1] = 0;
//			offset_value.Accel[2] = 0;
//			tempax = 0;
//			tempay = 0;
//			tempaz = 0;
//			cnt_a = 1;
//			return ;
//		}
//		tempax+= current_value.Accel[0];
//		tempay+= current_value.Accel[1];
//		//tempaz+= MPU6050_ACC_LAST.Z;
//		if(cnt_a==200)
//		{
//			offset_value.Accel[0]=tempax/cnt_a;
//			offset_value.Accel[1]=tempay/cnt_a;
//			offset_value.Accel[2]=tempaz/cnt_a;
//			cnt_a = 0;
//			ACC_OFFSET_OK = 1;
//			return ;
//		}
//		cnt_a++;		
//	}
}

void MPU9250_CalOff_Acc()
{
	ACC_OFFSET_OK=0;
}
void MPU9250_CalOff_Gyr()
{
	GYRO_OFFSET_OK=0;
}




