/********************************* �����к�̫�������޹�˾ *******************************
* ʵ �� �� ��С���Զ�ѭ��ʵ��ʵ��
* ʵ��˵�� ������KEY2��������С�����ݳ�ǰ�������Թܵķ�����ʼ�Զ��غ�����ʻ
* ʵ��ƽ̨ ������5�š�51��Ƭ����Сϵͳ
* ���ӷ�ʽ ����ο�interface.h�ļ�
* ע    �� �����ݻ�����ͬ����Ҫ���ڹ��Թܶ�Ӧ��������λ����ʹ���߼��ﵽ���״̬
* ��    �� ����̫���Ӳ�Ʒ�з���    QQ ��1909197536
* ��    �� ��http://shop120013844.taobao.com/
****************************************************************************************/

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#include "LCD1602.h"
#include "interface.h"
#include "motor.h"

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


//ѭ����ͨ���ж��������Թܵ�״̬������С���˶�
void SearchRun()
{
	//��·����⵽ ǰ��
	if(SEARCH_M_IO == BLACK_AREA && SEARCH_L_IO == BLACK_AREA && SEARCH_R_IO == BLACK_AREA)
	{
		ctrl_comm = COMM_UP;
		return;
	}
	
	if(SEARCH_R_IO == BLACK_AREA)//��ת
	{
		ctrl_comm = COMM_RIGHT;
	}
	else if(SEARCH_L_IO == BLACK_AREA)//��ת
	{
		ctrl_comm = COMM_LEFT;
	}
	else if(SEARCH_M_IO == BLACK_AREA)//ǰ��
	{
		ctrl_comm = COMM_UP;
	}
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
				}else
				{
					LED_OFF;
					switch_flag = 1;				
				}
			}
			if(sys_status == 1)
			{
				SearchRun();
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


