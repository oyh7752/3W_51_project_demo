/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� ��С��������+���+���Թ��Զ�����ʵ��
* ʵ��˵�� ����С��������+����Զ�����ʵ����������ӹ��Թܼ�⣬ʹ������׼ȷ
* ʵ��ƽ̨ ������5�š�51��Ƭ����Сϵͳ
* ���ӷ�ʽ ����ο�interface.h�ļ�
* ע    �� ��1���뽫����ӿ��߽���CN9
*            2������ϸ�Ķ�������������ϣ�������ģ�����и������Ƕȵ�Լ30�㣬��ǰ���ϰ�����
*            3����������Ҫ������ѡ��ʱ����׼ʱ��Ϊ0.1ms
             4����������ǹ��󣬻���ɳ�����ģ���಻׼
						 5����ʵ�鲻��Ҫ��ʾ����ע�⽫��Ļȡ�£�����Ӱ�쳬����
* ��    �� ����̫���Ӳ�Ʒ�з���    QQ ��1909197536
* ��    �� ��http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "interface.h"
#include "motor.h"
#include "UltrasonicCtrol.h"
#include "servo.h"
#include "redvoid.h"

//ȫ�ֱ�������
unsigned int speed_count=0;//ռ�ձȼ����� 50��һ����
char front_left_speed_duty=SPEED_DUTY;
char front_right_speed_duty=SPEED_DUTY;

unsigned char tick_5ms = 0;//5ms����������Ϊ�������Ļ�������
unsigned char tick_1ms = 0;//1ms����������Ϊ����Ļ���������
unsigned char tick_200ms = 0;
unsigned int tick_1s = 0;
unsigned char switch_flag=0;
volatile unsigned long system_tick=0;//ϵͳʱ�� 1ms�Լ�

char sys_status = 0;//ϵͳ״̬ 0 ֹͣ 1 ����
char ctrl_comm;//����ָ��
char ctrl_comm_last;

/*******************************************************************************
* �� �� �� ��Delayms
* �������� ��ʵ�� ms������ʱ
* ��    �� ��ms
* ��    �� ����
*******************************************************************************/
//void Delayms(unsigned int ms)
//{
//	unsigned int i,j;
//	for(i=0;i<ms;i++)
//	#if FOSC == 11059200L
//		for(j=0;j<114;j++);
//	#elif FOSC == 12000000L
//	  for(j=0;j<123;j++);
//	#elif FOSC == 24000000L
//		for(j=0;j<249;j++);
//	#elif FOSC == 48000000L
//		for(j=0;j<715;j++);
//	#else
//		for(j=0;j<114;j++);
//	#endif
//}
void Delayms(unsigned int ms)//�����ж�ռ���˽϶��ʱ�䣬���ﻻһ����ʱ����
{
	unsigned long start = system_tick;
	
	while(system_tick - start < ms);
}

/*******************************************************************************
* �� �� �� ��Timer0Init
* �������� ����ʱ��0��ʼ��
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Timer0Init()
{
	TMOD|=0x01; //���ö�ʱ��0������ʽΪ1
	TH0=(65536-(FOSC/12*TIME_US)/1000000)/256;
	TL0=(65536-(FOSC/12*TIME_US)/1000000)%256;
	ET0=1; //������ʱ��0�ж�
	TR0=1;	//������ʱ��	
	EA=1;  //�����ж�
}

///��ȡ��������ľ���,����ǰ�������Ϊ��ǰ
void GetAllDistance(unsigned int *dis_left,unsigned int *dis_right,unsigned int *dis_direct)
{
	CarStop();
	Delayms(100);
	GetDistance();
	*dis_direct = distance_cm;
	
	DuojiRight();
	Delayms(200);
	GetDistance();//��ȡ�ұ߾���
	*dis_right = distance_cm;
	
	DuojiMid();
	DuojiLeft();
	Delayms(200);
	GetDistance();//��ȡ��߾���
	*dis_left = distance_cm;
	
	DuojiMid();//��λ
}

void VoidRun()
{
	unsigned int dis_left;//��߾���
	unsigned int dis_right;//�ұ߾���
	unsigned int dis_direct;//�м����
	if(VOID_NONE != GetVoidStatus())
	{
		CarBack();
		Delayms(300);	
		GetAllDistance(&dis_left,&dis_right,&dis_direct);
		
		if(dis_direct >= dis_left && dis_direct >= dis_right)//ǰ��������Զ
		{
			CarGo();
			return;
		}
		else if(dis_left <= dis_right)//��ת
		{
			CarRight();
			DelayCheck(500);
		}
		else if(dis_right < dis_left)
		{
			CarLeft();
			DelayCheck(500);
		}				
	}
	else
	{
		CarGo();
	}
}

//��ȡ��ֵ�����û�������� -1
int KeyScan(void)
{
	int value = -1;
	KEY1_IO = 1;
	KEY2_IO = 1;
	if(KEY_DOWN == KEY1_IO)
	{
		Delayms(5);//Delayms for a while
		if(KEY_DOWN == KEY1_IO)
		{
			value = KEY1;
		}
		while(KEY_DOWN == KEY1_IO);
	}

	if(KEY_DOWN == KEY2_IO)
	{
		Delayms(5);//Delayms for a while
		if(KEY_DOWN == KEY2_IO)
		{
			value = KEY2;
		}
		while(KEY_DOWN == KEY2_IO);
	}
	return value;
}

/*******************************************************************************
* �� �� �� ��main
* �������� ��������
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void main()
{
	int key_value;
	CarStop();	
	LED_OFF;
	Timer0Init();
	UltraSoundInit();
	
	//5ms ִ��һ��
	while(1)
	{ 
		key_value = KeyScan();
		if(key_value == KEY1) 
		{
			sys_status = 0;
		}
			
		if(key_value == KEY2) 
		{
			sys_status = 1;
		}
		//ִ�в���
		if(tick_5ms >= 5)
		{
			tick_5ms = 0;
			if(sys_status == 1)
			{
				Distance();
				VoidRun();
			}else
			{
				CarStop();
			}  
		}   
	}
}

/*******************************************************************************
* �� �� �� ��Timer0Int
* �������� ����ʱ��0�жϺ��� �� ÿ��TIME_MS ms����
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Timer0Int() interrupt 1
{
	TH0=(65536-(FOSC/12*TIME_US)/1000000)/256;
	TL0=(65536-(FOSC/12*TIME_US)/1000000)%256;
	tick_1ms++;
	if(tick_1ms >= 10)
	{
		tick_1ms = 0;	
		tick_5ms++;
		system_tick++;
	}
	speed_count++;
	if(speed_count >= 50)//5ms
	{
		speed_count = 0;
		
		tick_200ms++;
		if(tick_200ms >= 40)
		{
			tick_200ms = 0;
			if(switch_flag)
			{
				LED_ON;
				switch_flag = 0;
			}else
			{
				LED_OFF;
				switch_flag = 1;				
			}
		}
	}
	CarMove();
	
	duoji_count++;
	if(duoji_count <= zhuanjiao)
	{
		DUOJI_IO = 0; //�������һ�������������Ҫ���� modfied by LC 2015.09.19 22:39
	}
	else
	{
		DUOJI_IO = 1;
	}
	if(duoji_count >= 200)//20ms
	{
		duoji_count = 0;
	}
	
}


