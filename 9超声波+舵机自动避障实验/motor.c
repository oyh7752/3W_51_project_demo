#include "motor.h"
#include "interface.h"

//����ռ�ձ��������ת��
void CarMove()
{
	//��ǰ��
	if(front_left_speed_duty > 0)//��ǰ
	{
		if(speed_count < front_left_speed_duty)
		{
			FRONT_LEFT_GO;
		}else
		{
			FRONT_LEFT_STOP;
		}
	}
	else if(front_left_speed_duty < 0)//���
	{
		if(speed_count < (-1)*front_left_speed_duty)
		{
			FRONT_LEFT_BACK;
		}else
		{
			FRONT_LEFT_STOP;
		}
	}
	else                //ֹͣ
	{
		FRONT_LEFT_STOP;
	}
	
		//��ǰ��
	if(front_right_speed_duty > 0)//��ǰ
	{
		if(speed_count < front_right_speed_duty)
		{
			FRONT_RIGHT_GO;
		}else                //ֹͣ
		{
			FRONT_RIGHT_STOP;
		}
	}
	else if(front_right_speed_duty < 0)//���
	{
		if(speed_count < (-1)*front_right_speed_duty)
		{
			FRONT_RIGHT_BACK;
		}else                //ֹͣ
		{
			FRONT_RIGHT_STOP;
		}
	}
	else                //ֹͣ
	{
		FRONT_RIGHT_STOP;
	}
}

//��ǰ
void CarGo()
{
	front_left_speed_duty=SPEED_DUTY;
	front_right_speed_duty=SPEED_DUTY;
}

//����
void CarBack()
{
	front_left_speed_duty=-(SPEED_DUTY+20);
	front_right_speed_duty=-(SPEED_DUTY+20);
}

//����
void CarLeft()
{
	front_left_speed_duty=0;
	front_right_speed_duty=SPEED_DUTY;
}

//����
void CarRight()
{
	front_left_speed_duty=SPEED_DUTY;
	front_right_speed_duty=0;
}

//ֹͣ
void CarStop()
{
	front_left_speed_duty=0;
	front_right_speed_duty=0;
}

