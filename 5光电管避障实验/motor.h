#ifndef __MOTOR_H_
#define __MOTOR_H_

extern unsigned int speed_count;//ռ�ձȼ����� 50��һ����
extern char front_left_speed_duty;
extern char front_right_speed_duty;
extern char behind_left_speed_duty;
extern char behind_right_speed_duty;

void CarMove();
void CarGo();
void CarBack();
void CarLeft();
void CarRight();
void CarStop();

#endif

