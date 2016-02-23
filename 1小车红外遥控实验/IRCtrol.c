#include "IRCtrol.h"
#include "interface.h"

unsigned char ir_rec_flag=0;//�������ݱ�־λ 1 �������� 0 û��
unsigned char IRCOM[4];

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

/*******************************************************************************
* �� �� �� ��DelayIr
* �������� ��0.14MS ��ʱ
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void DelayIr(unsigned char x)  
{
 unsigned char i;
  while(x--)
 {
  for (i = 0; i<68; i++) {}
 }
}

/*******************************************************************************
* �� �� �� ��Exit0Init
* �������� �����ж�0��ʼ������
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Exit0Init()
{
	  EX0 = 1;	//ʹ�� INT1 �ⲿ�ж�
    IT0 = 1;	// ������ʽΪ���帺���ش���
    EA = 1;//���ж�
}  

/*******************************************************************************
* �� �� �� ��Exit0Int
* �������� ���ⲿ�ж�0 ISR
* ��    �� ����
* ��    �� ����
*******************************************************************************/
void Exit0Int() interrupt 0 
{
  unsigned char j,k,N=0;

     EX0 = 0;   
	 DelayIr(15);
	 if (IRIN==1) 
     { EX0 =1;
	   return;
	  } 
		 continue_time = 40;//�����źţ���ʾָ����� 40*5 = 200ms ��ָ��ͣ��
                           //ȷ��IR�źų���
  while (!IRIN)            //��IR��Ϊ�ߵ�ƽ������9ms��ǰ���͵�ƽ�źš�
    {DelayIr(1);}

 for (j=0;j<4;j++)         //�ռ���������
 { 
  for (k=0;k<8;k++)        //ÿ��������8λ
  {
   while (IRIN)            //�� IR ��Ϊ�͵�ƽ������4.5ms��ǰ���ߵ�ƽ�źš�
     {DelayIr(1);}
    while (!IRIN)          //�� IR ��Ϊ�ߵ�ƽ
     {DelayIr(1);}
     while (IRIN)           //����IR�ߵ�ƽʱ��
      {
    DelayIr(1);
    N++;           
    if (N>=30)
	 { EX0=1;
	 return;}                  //0.14ms���������Զ��뿪��
      }                        //�ߵ�ƽ�������                
     IRCOM[j]=IRCOM[j] >> 1;                  //�������λ����0��
     if (N>=8) {IRCOM[j] = IRCOM[j] | 0x80;}  //�������λ����1��
     N=0;
  }//end for k
 }//end for j
   
   if (IRCOM[2]!=~IRCOM[3])
   { EX0=1;
     return; }
	 
		 //ָ��ת��
		 
		 switch(IRCOM[2])
		 {
			 case 0x46: ctrl_comm = COMM_UP;break;
			 case 0x15: ctrl_comm = COMM_DOWN;break;
			 case 0x44: ctrl_comm = COMM_LEFT;break;
			 case 0x43: ctrl_comm = COMM_RIGHT;break;
			 case 0x40: ctrl_comm = COMM_STOP;break;
			 default : EX0 = 1; return;
		 }
		 ir_rec_flag = 1;

     EX0 = 1; 
} 

