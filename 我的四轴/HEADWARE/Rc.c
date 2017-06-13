#include "Rc.h"
#include "Control.h"
#include "math.h"
extern T_RC_Data  Rc_send_Data;


void Rc_GetValue(T_RC_Data *temp)       //ң�����ݻ�ȡ
{
	temp->THROTTLE	= Rc_send_Data.THROTTLE;
	temp->YAW		    = Rc_send_Data.YAW;//<1000 ? -(1000-Rc_send_Data.YAW): (Rc_send_Data.YAW-1000))/12;
	temp->ROL		    = Rc_send_Data.ROL; //<1000 ? -(1000-Rc_send_Data.ROL): (Rc_send_Data.ROL-1000))/12;
	temp->PIT	    	= Rc_send_Data.PIT; //<1000 ? -(1000-Rc_send_Data.PIT): (Rc_send_Data.PIT-1000))/12;	
	temp->AUX1	    	= Rc_send_Data.AUX1; 
	temp->AUX2	    	= Rc_send_Data.AUX2; 
}


void Rc_Fun(T_RC_Data *rc_in,T_RC_Control *rc_ct)    //�ɿ�״̬�����ж�
{
	static u8 cnt_arm=0,cnt_fun=0;
	if(rc_in->THROTTLE<RC_FUN_MIN&&rc_in->YAW<RC_FUN_MIN)  //������С��yaw��Сʱ������
	{
		cnt_arm++;
		if(cnt_arm==75)
		{
			cnt_arm=0;
			rc_ct->ARMED = 1;
		}
	}
	else if(rc_in->THROTTLE<RC_FUN_MIN&&rc_in->YAW>RC_FUN_MAX) //������С��yaw���ʱ������
	{
		cnt_arm++;
		if(cnt_arm==75)
		{
			cnt_arm=0;
			rc_ct->ARMED = 0;
		}
	}
	else
		cnt_arm = 0;
		
	if(rc_ct->ARMED==1)
		return;
	
//	if(rc_in->THROTTLE<RC_FUN_MIN&&rc_in->ROL<RC_FUN_MIN)   //������С��roll��С��ACCELУ׼
//	{
//		cnt_fun++;
//		if(cnt_fun==75)
//		{
//			cnt_fun = 0;
//			MPU9250_CalOff_Acc();
//		}
//	}
	
	
//k1    0
//k3    2058 	
//k5    2734
//k7    3076
//k9    3283
//nokey 4095	
	if(rc_in->AUX1<=0)   //k1,GYROУ׼
	{
		cnt_fun++;
		if(cnt_fun==75)
		{
			cnt_fun = 0;
			MPU9250_CalOff_Gyr();
		}
	}
	else
		cnt_fun = 0;	
}	


