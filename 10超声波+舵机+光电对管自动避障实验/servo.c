//�������ɵ��ڷ�Χ 0.5ms -- 2.5ms
//��Ӧ��zhuanjiao���� 5 -- 25
#include "servo.h"
#include "interface.h"
unsigned char duoji_count=0;
unsigned char zhuanjiao = 15;

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

void DuojiMid()
{
	zhuanjiao = 15;
	Delayms(300);//��ʱ
}

void DuojiRight()
{
	zhuanjiao = 8;
	Delayms(300);//��ʱ
}

void DuojiLeft()
{
	zhuanjiao = 22;
	Delayms(300);//��ʱ
}


