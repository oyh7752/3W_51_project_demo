#ifndef __IRCTROL_H_
#define __IRCTROL_H_

#include <reg52.h>
#include <intrins.h>
#define FOSC 48000000L //�������ã�ʹ��24M Hz 6T˫�ٱ�ģʽ

extern unsigned char ir_rec_flag;//�������ݱ�־λ 1 �������� 0 û��
extern char ctrl_comm;//����ָ��
extern unsigned char continue_time;
void Exit0Init();

#endif
