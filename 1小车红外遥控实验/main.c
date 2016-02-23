/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� ��С������ң��ʵ��
* ʵ��˵�� ��ʹ�ú���ң��������С���˶�
* ʵ��ƽ̨ ������5�š�51��Ƭ����Сϵͳ
* ���ӷ�ʽ ����ο�interface.h�ļ�
* ע    �� ��ָ������������ͲŻ�ʹС����������ֹͣ����ָ���С����ֹͣ
* ��    �� ����̫���Ӳ�Ʒ�з���    QQ ��1909197536
* ��    �� ��http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "LCD1602.h"
#include "IRCtrol.h"
#include "interface.h"
#include "motor.h"

//ȫ�ֱ�������
unsigned int speed_count=0;//ռ�ձȼ����� 50��һ����
char front_left_speed_duty=SPEED_DUTY;
char front_right_speed_duty=SPEED_DUTY;

unsigned char tick_5ms = 0;//5ms����������Ϊ�������Ļ�������
unsigned char tick_1ms = 0;//1ms����������Ϊ����Ļ���������
unsigned char tick_200ms = 0;
unsigned char switch_flag=0;

char ctrl_comm;//����ָ��
unsigned char continue_time = 0;

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
	PT0 = 1;//����Ϊ�ϸߵ����ȼ�������������
	TMOD|=0x01; //���ö�ʱ��0������ʽΪ1
	TH0=(65536-(FOSC/12*TIME_US)/1000000)/256;
	TL0=(65536-(FOSC/12*TIME_US)/1000000)%256;
	ET0=1; //������ʱ��0�ж�
	TR0=1;	//������ʱ��	
	EA=1;  //�����ж�
}


/*******************************************************************************
* �� �� �� ��main
* �������� ��������
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void main()
{
	CarStop();	
	LCD1602Init();
	Timer0Init();
	Exit0Init();
	
	//5ms ִ��һ��
	while(1)
	{ 
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
				}else
				{
					LED_OFF;
					switch_flag = 1;				
				}
			}
			
			continue_time--;//200ms �޽���ָ���ͣ��
			if(continue_time == 0)
			{
				continue_time = 1;
				CarStop();
			}
			//do something
			if(ir_rec_flag == 1)//���յ������ź�
			{
				ir_rec_flag = 0;
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
* �������� ����ʱ��0�жϺ��� �� ÿ��TIME_MS ms���� 0.1ms
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Timer0Int() interrupt 1
{
	TH0=(65536-(FOSC/12*TIME_US)/1000000)/256;
	TL0=(65536-(FOSC/12*TIME_US)/1000000)%256;
	tick_1ms++;
	if(tick_1ms >= 5) //1ms
	{
		tick_1ms = 0;
		tick_5ms++;
	}
	speed_count++;
	if(speed_count >= 50)//5ms���� //modfied by LC 2015.09.12 9:56
	{
		speed_count = 0;
	}
	CarMove();
}



