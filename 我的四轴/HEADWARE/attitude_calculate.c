#include "attitude_calculate.h"
#define  PItoAngel  180/3.14159      //���Ȼ��Ƕ�
//MPU_value current_value;//��ǰֵ
float kp1=10.0,ki1=0.08,kp2=10.0,ki2=0.08;//kp���������˲���ֹƵ�ʣ�ki����������̬ƫ��ʱ�� ki==kp��0.01~~0.1��
float last_attitude[3];//��̬��
float offset_Gyro[3];//�����ǲ���ֵ
double eabI[3],ecI[3];//������
float Gyro[3];
float Rxx=1,Rxy=0,Rxz=0,Ryx=0,Ryy=1,Ryz=0,Rzx=0,Rzy=0,Rzz=1;
float sinC,cosC;//CΪƫ����
/*****************�����˲�**********************/
void attitude_calculate(MPU_value current_value)//��ǰ9������
{
	u8 i,Acc_x,Acc_y,Acc_z,Mag_x,Mag_y;
  float tmp,tmp_e;//tmp_eΪ����ģ��//RxxΪ��������Ԫ�أ������ʼ����������ο�ϵ�����غ�
  float ax,ay,az;//��λ���ٶ�����ֵ
  float eab[3],ec[3];//pitch��roll��yow���
	float time=0.01;//����ʱ��1ms(�����е�dt)

  //����x����ٶ�
	if (current_value.Accel[0]<200) Acc_x = current_value.Accel[0];
	else              Acc_x = 1-(current_value.Accel[0] - 200);
	 //����y����ٶ�
	if (current_value.Accel[1]<200) Acc_y = current_value.Accel[1];
	else              Acc_y = 1-(current_value.Accel[1] - 200);
	 //����z����ٶ�
	if (current_value.Accel[2]<200) Acc_z = current_value.Accel[2];
	else              Acc_z = 1-(current_value.Accel[2] - 200);
	
	//����x��������
	if (current_value.Gyro[0]<2000) Gyro[0] = current_value.Gyro[0];
	else              Gyro[0] = 1-(current_value.Gyro[0] - 2000);
	 //����y��������
	if (current_value.Gyro[1]<2000) Gyro[1] = current_value.Gyro[1];
	else              Gyro[1] = 1-(current_value.Gyro[1] - 2000);
	 //����z��������
	if (current_value.Gyro[2]<2000) Gyro[2] = current_value.Gyro[2];
	else              Gyro[2] = 1-(current_value.Gyro[2] - 2000);

	//����x�������
	if (current_value.Mag[0]<4912) Mag_x = current_value.Mag[0];
	else              Mag_x = 1-(current_value.Mag[0] - 4912);
	//����y�������
	if (current_value.Mag[1]<4912) Mag_x = current_value.Mag[1];
	else              Mag_y = 1-(current_value.Mag[1] - 4912);
	
	
	
//**********************************������ز���*****************************//	
	tmp=fabs(Mag_x)*fabs(Mag_x)+fabs(Mag_y)*fabs(Mag_y);//ƫ����Ϊ�������̲���ֵ ��ȡ����ֵ���㣩
	tmp_e=sqrt(tmp);
	sinC=Mag_x/tmp_e;
	cosC=Mag_y/tmp_e;
//------------------------------------------------//	
	tmp=fabs(Acc_x)*fabs(Acc_x)+fabs(Acc_y)*fabs(Acc_y)+fabs(Acc_z)*fabs(Acc_z);
	tmp_e=sqrt(tmp);    //���ٶ�ģ
	ax=Acc_x/tmp_e;    //���ٶȵ�λ��
	ay=Acc_y/tmp_e;
	az=Acc_z/tmp_e;
//********************������̬�����***************************//	
	eab[0]=(ax*Rzy-ay*Rzz);//����pitch��roll���//����ֵȫΪ���ٶ�����ֵ
	eab[1]=(ax*Rzz-Rzx*az);
	eab[2]=(Rzx*ay-ax*Rzy);
	eabI[0]+=eab[0]*ki1;//x����ٶ���������
	eabI[1]+=eab[1]*ki1;//y����ٶ���������
	eabI[2]+=eab[2]*ki1;//z����ٶ���������
//-------------------------------------------------//	
	tmp=fabs(Rxx)*fabs(Rxx)+fabs(Ryx)*fabs(Ryx);//�ô����Ƶĺ������ݼ���yow���
	tmp_e=(Rxx*sinC-Ryx*cosC)/sqrt(tmp);
	ec[0]=Rxz*tmp_e;//ע��˴����ο�ϵ�����任������ϵ��ӦΪ  {x,y,z}*R������󣨻�R��ת�þ��� ����ΪR-1==RT�������������ʣ���
	ec[1]=Ryz*tmp_e;
	ec[2]=Rzz*tmp_e;
	ecI[0]+=ec[0]*ki2;//x���������������
	ecI[1]+=ec[1]*ki2;//y���������������
	ecI[2]+=ec[2]*ki2;//z���������������
	
	  for(i=0;i<3;i++)
		{
			offset_Gyro[i]=kp1*eab[i]+eabI[i]+kp2*ec[i]+ecI[i]+Gyro[i];//���ں�
		}
//*************************�������ҷ���****************************//			
			Rxx=Rxx+(Rxy*offset_Gyro[2]-Rxz*offset_Gyro[1])*time;
			Rxy=Rxy+(Rxz*offset_Gyro[0]-Rxx*offset_Gyro[2])*time;
			Rxz=Rxz+(Rxx*offset_Gyro[1]-Rxy*offset_Gyro[0])*time;
			
			Ryx=Ryx+(Ryy*offset_Gyro[2]-Ryz*offset_Gyro[1])*time;
			Ryy=Ryy+(Ryz*offset_Gyro[0]-Ryx*offset_Gyro[2])*time;
			Ryz=Ryz+(Ryx*offset_Gyro[1]-Ryy*offset_Gyro[0])*time;
			
			Rzx=Rzx+(Rzy*offset_Gyro[2]-Rzz*offset_Gyro[1])*time;
			Rzy=Rzy+(Rzz*offset_Gyro[0]-Rzx*offset_Gyro[2])*time;
			Rzz=Rzz+(Rzx*offset_Gyro[1]-Rzy*offset_Gyro[0])*time;
//**************************��̬��*********************************//	
	tmp=fabs(Rzx)*fabs(Rzx)+fabs(Rzy)*fabs(Rzy)+fabs(Rzz)*fabs(Rzz);
	tmp_e=sqrt(tmp);    //���ٶ�ģ
	Rzx=Rzx/tmp_e;    //���ٶȵ�λ��
	Rzy=Rzy/tmp_e;
	Rzz=Rzz/tmp_e;
	tmp=fabs(Ryx)*fabs(Ryx)+fabs(Ryy)*fabs(Ryy)+fabs(Ryz)*fabs(Ryz);
	tmp_e=sqrt(tmp);    //���ٶ�ģ
	Ryx=Rzx/tmp_e;    //���ٶȵ�λ��
	Ryy=Ryy/tmp_e;
	Ryz=Ryz/tmp_e;
	tmp=fabs(Rxx)*fabs(Rxx)+fabs(Rxy)*fabs(Rxy)+fabs(Rxz)*fabs(Rxz);
	tmp_e=sqrt(tmp);    //���ٶ�ģ
	Rxx=Rxx/tmp_e;    //���ٶȵ�λ��
	Rxy=Rxy/tmp_e;
	Rxz=Rxz/tmp_e;
		
			last_attitude[0]=asin(-Rzx)*PItoAngel;  //final pitch			 	  
			last_attitude[1]=atan(Rzy/Rzz)*PItoAngel; //final roll
			last_attitude[2]=atan(Ryx/Rxx)*PItoAngel; //final yow

}




