#ifndef __L298N_H
#define __L298N_H


#define MoterTIM                TIM4
#define MoterPeriod             10000
#define MoterPWMPort            GPIOB
#define MoterPWMPin             GPIO_Pin_8

#define MotorPort               GPIOD
#define MotorPin1               GPIO_Pin_2  //Left A
#define MotorPin2               GPIO_Pin_3 //Left B
#define MotorPin3               GPIO_Pin_4 //right A
#define MotorPin4               GPIO_Pin_7 //right B


//function
void MoterSetSpeed(double speed);
void TIM_Init();
static void InitPins();
void L298NInit();
void Moterforward();
void Moterback();
void Moterright();
void Moterleft();
void Moterstop();

#endif