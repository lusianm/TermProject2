#include "L298N.h"
#include "stm32f10x.h"



volatile int MoterPulse = MoterPeriod;


void MoterSetSpeed(double speed){
  MoterPulse = (int)(MoterPeriod * speed);
  TIM_Init();
}

void TIM_Init(){
  RCC_ClocksTypeDef RCC_ClocksStatus;
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  uint16_t prescaler = RCC_ClocksStatus.SYSCLK_Frequency / 1000000 - 1; //1 tick = 1us (1 tick = 0.165mm resolution)
  TIM_DeInit(MoterTIM);
  
  TIM_TimeBaseInitTypeDef TIM_TimeBase;
  TIM_TimeBase.TIM_Period = (MoterPeriod -1);  //0.1ms
  TIM_TimeBase.TIM_Prescaler = prescaler;
  //Clock Division : /1
  TIM_TimeBase.TIM_ClockDivision = 0;  
  //Clock Count When Rising Edge
  TIM_TimeBase.TIM_CounterMode = TIM_CounterMode_Up;
  
  //Init TIM5
  TIM_TimeBaseInit(MoterTIM, &TIM_TimeBase);
  
  
  TIM_OCInitTypeDef TIM_OCInitStruct;
  TIM_OCStructInit(&TIM_OCInitStruct);
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_Pulse = MoterPulse; //us
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC3Init(MoterTIM, &TIM_OCInitStruct);
  
  //Set TIM5 prescaler
  TIM_PrescalerConfig(MoterTIM, prescaler, TIM_PSCReloadMode_Immediate);
  
  TIM_SelectMasterSlaveMode(MoterTIM, TIM_MasterSlaveMode_Enable);
  TIM_CtrlPWMOutputs(MoterTIM, ENABLE);
  TIM_ClearFlag(MoterTIM, TIM_FLAG_Update);
  
}


static void InitPins() {
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = MoterPWMPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       
  GPIO_Init(MoterPWMPort, &GPIO_InitStructure);
        
   GPIO_InitStructure.GPIO_Pin = MotorPin1;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
   GPIO_Init(MotorPort, &GPIO_InitStructure);
   
   GPIO_InitStructure.GPIO_Pin = MotorPin2;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
   GPIO_Init(MotorPort, &GPIO_InitStructure);
   
   GPIO_InitStructure.GPIO_Pin = MotorPin3;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
   GPIO_Init(MotorPort, &GPIO_InitStructure);
   
   GPIO_InitStructure.GPIO_Pin = MotorPin4;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
   GPIO_Init(MotorPort, &GPIO_InitStructure);
}


void MoterDelay(){
  for(int i = 0; i < 10000; i++) {}
}

void Moterforward()
{
  GPIO_SetBits(MotorPort, MotorPin1 | MotorPin3);
  MoterDelay();  
  GPIO_ResetBits(MotorPort, MotorPin2 | MotorPin4);
  MoterDelay();  
}

void Moterback()
{
  GPIO_SetBits(MotorPort, MotorPin2 | MotorPin4);
  MoterDelay();  
  GPIO_ResetBits(MotorPort, MotorPin1 | MotorPin3);
  MoterDelay();
}

void Moterright()
{
  GPIO_SetBits(MotorPort, MotorPin2 | MotorPin3);
  MoterDelay();  
  GPIO_ResetBits(MotorPort, MotorPin1 | MotorPin4);
  MoterDelay();  
}

void Moterleft()
{
  GPIO_SetBits(MotorPort, MotorPin1 | MotorPin4);
  MoterDelay();  
  GPIO_ResetBits(MotorPort, MotorPin2 | MotorPin3);
  MoterDelay();  
}

void Moterstop()
{
  GPIO_ResetBits(MotorPort, MotorPin1| MotorPin2 | MotorPin3 | MotorPin4);
  MoterDelay();  
}

void L298NInit(){
  InitPins();  
  TIM_Init();
  Moterstop();
}