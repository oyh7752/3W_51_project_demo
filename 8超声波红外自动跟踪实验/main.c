/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� ��С�������������Զ�����ʵ��
* ʵ��˵�� ������KEY2��������С����ʼͨ��������ģ���Լ��������ģ��۲�ǰ���ϰ��
*            ʵʱ����С�����ϰ���ľ��룬�������տ���С����Ч��
* ʵ��ƽ̨ ������5�š�51��Ƭ����Сϵͳ
* ���ӷ�ʽ ����ο�interface.h�ļ�
* ע    �� ��1����������ֱ�Ӱ�װ�ڶ���ϵģ�����ضϿ�����ӿ�CN9
*            2������ϸ�Ķ�������������ϣ�������ģ�����и������Ƕȵ�Լ30�㣬��ǰ���ϰ�����
                ��������ǹ��󣬻���ɳ�����ģ���಻׼
* ��    �� ����̫���Ӳ�Ʒ�з���    QQ ��1909197536
* ��    �� ��http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "LCD1602.h"
#include "interface.h"
#include "motor.h"
#include "UltrasonicCtrol.h"
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

char sys_status = 0;//ϵͳ״̬ 0 ֹͣ 1 ����
char ctrl_comm;//����ָ��
char ctrl_comm_last;

/*******************************************************************************
* �� �� �� ��Delayms
* �������� ��ʵ�� ms������ʱ
* ��    �� ��ms
* ��    �� ����
*******************************************************************************/
void Delayms(unsigned int ms)
{
	unsigned int i,j;
	for(i=0;i<ms;i++)
	#if FOSC == 11059200L
		for(j=0;j<114;j++);
	#elif FOSC == 12000000L
	  for(j=0;j<123;j++);
	#elif FOSC == 24000000L
		for(j=0;j<249;j++);
	#elif FOSC == 48000000L
		for(j=0;j<715;j++);
	#else
		for(j=0;j<114;j++);
	#endif
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

void BarrierProc()
{
	char status;
	// range 4cm -- 8cm
	if(distance_cm < 4 && distance_cm > 2) 
	{
		ctrl_comm = COMM_STOP;//����
		return;
	}
	
	//  < 2cm back
	if(distance_cm <= 2) 
	{
		ctrl_comm = COMM_DOWN;
		return;
	}

	// 4 cm - 8cm up and turn
	status = GetVoidStatus();
	if(status == VOID_BOTH) ctrl_comm = COMM_UP;
	else if(status == VOID_LEFT) ctrl_comm = COMM_LEFT;
	else if(status == VOID_RIGHT) ctrl_comm = COMM_RIGHT;
	else if(status == VOID_NONE) ctrl_comm = COMM_STOP;
}

//��ȡ��ֵ�����û�������� -1
int KeyScan(void)
{
	int value = -1;
	KEY1_IO = 1;
	KEY2_IO = 1;
	if(KEY_DOWN == KEY1_IO)
	{
		Delayms(5);//delay for a while
		if(KEY_DOWN == KEY1_IO)
		{
			value = KEY1;
		}
		while(KEY_DOWN == KEY1_IO);
	}

	if(KEY_DOWN == KEY2_IO)
	{
		Delayms(5);//delay for a while
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
	LCD1602Init();
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
			tick_200ms++;
			if(tick_200ms >= 40)
			{
				tick_200ms = 0;
				if(switch_flag)
				{
					LED_ON;
					switch_flag = 0;
					LCD1602WriteDistance(distance_cm);
				}else
				{
					LED_OFF;
					switch_flag = 1;				
				}
			}
			if(sys_status == 1)
			{
				Distance();
				BarrierProc();	
			}else
			{
				ctrl_comm = COMM_STOP;
			}
			
			//do something
			if(ctrl_comm_last != ctrl_comm)//���յ������ź�
			{
				ctrl_comm_last = ctrl_comm;
				switch(ctrl_comm)
				{
				case COMM_UP:    CarGo();break;
				case COMM_DOWN:  CarBack();break;
				case COMM_LEFT:  CarLeft();break;
				case COMM_RIGHT: CarRight();break;
				case COMM_STOP:  CarStop();break;
				default : break;
				}
				LCD1602WriteCommand(ctrl_comm);
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
	if(tick_1ms >= 5)
	{
		tick_1ms = 0;	
		tick_5ms++;
	}
	speed_count++;
	if(speed_count >= 50)
	{
		speed_count = 0;
	}
	CarMove();
}


