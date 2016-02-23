#include "UltrasonicCtrol.h"
#include "interface.h"

volatile unsigned char status = 0;//����ǰ״̬��0,���� 1 ���ʹ����źţ�2 �ȴ��źŷ���,3 
unsigned int dis_count = 0;//������ʱ
volatile unsigned int distance_cm = 0;//��ǰ����
unsigned char t2_full_count = 0;//������������������
static unsigned int tick_5ms = 0;//5ms������

void UltraSoundInit()
{
	Trig = 0;
	TH2 = RCAP2H = 0;
	TL2 = RCAP2L = 0;  
	TR2 = 0;//�رն�ʱ��	
	ET2=1;             //����T0�ж�
	
	EX1=0;        //�ر��ⲿ�ж�1
	IT1=1;        //�ɸߵ�ƽ��͵�ƽ�������ⲿ�ж�
	PX1 = 1;//���ȼ��ϸ�
}

//�жϵķ�ʽ��ȡ����ֵ ÿ5msִ��һ��
void GetDistance()
{
		if(status == 1)
		{
			//���ʹ����ź�
			Trig = 1;
			status = 1;
			TH2 = 0;
			TL2 = 0;
			TR2 = 1;//�򿪶�ʱ��
			while(TL2 < 42);//��ʱ����10us
			status = 2;
			Trig = 0;
			TH2 = 0;
			TL2 = 0;
			TR2 = 0;
			while(Echo == 0);//�ȴ������ź���ʼλ��
			EX1 = 1;//���ⲿ�ж�
			TR2 = 1;
			status = 3;//��ʼ�������
	    }
		if(status == 4)//�ɹ����յ�����
		{
			distance_cm = (unsigned int)(((long)(dis_count) * 34)/8000);//����340m/s
			status = 0;//׼���´η���
		}
		if(status == 5)//��������ʧ��
		{
			status = 0;//׼���´η���
		}
}

//��ʼ��ȡ���룬ÿ100msִ��һ��
void StartGetDistance()
{
	status = 1;
}

void Distance()
{
	tick_5ms++;
	if(tick_5ms >= 15)
	{
		tick_5ms = 0;
//		StartGetDistance();
		GetDistanceDelay();
	}
	//GetDistance();
}

//��ʱ�ķ�ʽ��ȡ����ֵ
void GetDistanceDelay()
{
		//���ʹ����ź�
		Trig = 1;
		status = 1;
		TH2 = 0;
		TL2 = 0;
		TR2 = 1;//�򿪶�ʱ��
		while(TL2 < 42);//��ʱ����10us
		status = 2;
		Trig = 0;
		TH2 = 0;
		TL2 = 0;
		TR2 = 0;
		while(Echo == 0);//�ȴ������ź���ʼλ��
//			EX1 = 1;//���ⲿ�ж�
		TR2 = 1;
		while(Echo == 1)//��ʼ���㳤��
		{
			if(status == 5) 
			{
				status = 0;
				distance_cm = 0;//ʧ�ܺ�ͺ���
				EA = 1;		//���ж�
				return;//����ʧ��
			}
		}
		dis_count = TH2*256 + TL2;
		TR2 = 0;
		distance_cm = (unsigned int)(((long)(dis_count) * 34)/8000);//����340m/s
		status = 0;//׼���´η���		
}

/*******************************************************************************
* �� �� �� ��
* �������� ���ⲿ�ж�1������
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Exint1Int()  interrupt 2   // �ⲿ�ж���2��
 { 
		if(status == 3)
		{
			 dis_count = TH2*256 + TL2;
			 EX1=0;            //�ر��ⲿ�ж�
			 TR2 = 0;
			 status = 4;
		}
  }
 
/*******************************************************************************
* �� �� �� ��Timer0Int
* �������� ����ʱ��0�жϷ�����
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Timer2Int() interrupt 5 // ��ʱ��0�ж���1��
{
	TF2 = 0;
	EX1=0; 
	status = 5;//��ʱ
}

