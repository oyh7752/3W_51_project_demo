#include "redvoid.h"
#include "interface.h"
#include "motor.h"

extern char ctrl_comm;

/*******************************************************************************
* �� �� �� ��Delayms
* �������� ��ʵ�� ms������ʱ
* ��    �� ��ms
* ��    �� ����
*******************************************************************************/
static void Delayms(unsigned int ms)
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

//��ȡ�������ģ��״̬
char GetVoidStatus(void)
{
	char left=0,right=0;
	char count;
	if(VOID_L_IO == BARRIER_Y)
	{
		count = 2;
		while(--count)//10ms �ɼ�10�ξ�Ҫ�ɼ���ǰ���ϰ�����Ϣ���˲�
		{
			if(VOID_L_IO == BARRIER_N)
				break;
			Delayms(1);
		}
		if(count == 0) left = 1;
	}
	
	if(VOID_R_IO == BARRIER_Y)
	{
		count = 2;
		while(--count)//10ms �ɼ�10�ξ�Ҫ�ɼ���ǰ���ϰ�����Ϣ���˲�
		{
			if(VOID_R_IO == BARRIER_N)
				break;
			Delayms(1);
		}
		if(count == 0) right = 2;
	}
	
	return left + right;
}

//��ʱ��ͬʱ�����⣬һ�������ϰ����ֹͣ��������ʱ
void DelayCheck(int ms)
{
	while(ms--)
	{
		Delayms(1);
		if(VOID_NONE != GetVoidStatus())
		{
			CarStop();
			return;
		}
	}
}

//������ϴ���
//����ʽ����߼�⵽  ����500ms ��ת500ms
//			�ұ߼�⵽  ����500ms ��ת500ms
//			���߼�⵽  ����1000ms ��ת500ms
//          û��⵽    ֱ��
void VoidRun(void)
{
	char status;
	status = GetVoidStatus();
	
	switch(status)
	{
		case VOID_LEFT: 
			ctrl_comm = COMM_RIGHT;CarBack(); Delayms(500); CarRight(); DelayCheck(500);
			break;
		case VOID_RIGHT:
			ctrl_comm = COMM_LEFT;CarBack(); Delayms(500); CarLeft(); DelayCheck(500);	
			break;
		case VOID_BOTH:
			ctrl_comm = COMM_RIGHT;CarBack(); Delayms(1000); CarRight(); DelayCheck(500);
			break;
		case VOID_NONE:
			ctrl_comm = COMM_UP;CarGo();
			break;
		default: break;
	}
}

