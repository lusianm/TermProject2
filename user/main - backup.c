#include "stm32f10x_HC-SR04.h"
#include "lcd.h"
#include "QMC5883L.h"
#include "stm32f10x.h"
#include "L298N.h"
#include "FB755AC.h"

#define LCD_TEAM_NAME_X 20
#define LCD_TEAM_NAME_Y 50

#define LCD_DIST_VAL_X 20
#define LCD_DIST_VAL_Y 80

#define LCD_Cap1_VAL_X 20
#define LCD_Cap1_VAL_Y 110

#define LCD_Cap2_VAL_X 20
#define LCD_Cap2_VAL_Y 140

#define ObstacleDistance 100


int32_t distance1 = 0;
int32_t distance2 = 0;
int32_t heading = 0;
//void NVIC_Configure(void);
//void EnableHCSR04PeriphClock();

void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; // 5
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; // 5
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //@ TIM2 NVIC
    NVIC_EnableIRQ(TIM5_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // UART2
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EnableHCSR04PeriphClock() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}


void TIM5_Init() {
    RCC_ClocksTypeDef RCC_ClocksStatus;
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    uint16_t prescaler = RCC_ClocksStatus.SYSCLK_Frequency / 1000000 - 1; //1 tick = 1us (1 tick = 0.165mm resolution)
    TIM_DeInit(TIM5);

    TIM_TimeBaseInitTypeDef TIM_TimeBase;
    TIM_TimeBase.TIM_Period = (10000 - 1);
    TIM_TimeBase.TIM_Prescaler = prescaler;
    //Clock Division : /1
    TIM_TimeBase.TIM_ClockDivision = 0;
    //Clock Count When Rising Edge
    TIM_TimeBase.TIM_CounterMode = TIM_CounterMode_Up;

    //Init TIM5
    TIM_TimeBaseInit(TIM5, &TIM_TimeBase);
    //Set TIM5 prescaler
    TIM_PrescalerConfig(TIM5, prescaler, TIM_PSCReloadMode_Immediate);
    //Ready To use Timer5
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

    //Timer È°¼ºÈ­
    TIM_Cmd(TIM5, ENABLE);
}


void TIM5_IRQHandler() {
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        //heading = (int)QMC5883L_GetHeading();
        if ((distance1 < ObstacleDistance) && (distance2 < ObstacleDistance))
            Moterstop();
        else if (distance1 < ObstacleDistance)
            Moterright();
        else if (distance2 < ObstacleDistance)
            Moterleft();
        else
            Moterforward();
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    }
}


int main(void) {
    SystemInit();
    EnableHCSR04PeriphClock();
    NVIC_Configure();
    LCD_Init();
    LCD_Clear(RED);
    LCD_ShowString(LCD_TEAM_NAME_X, LCD_TEAM_NAME_Y, "HCSR04TEST", BLACK, WHITE);
    FB755ACInit();
    L298NInit();
    //QMC5883L_Initialize();
    InitHCSR04(&distance1, &distance2);
    TIM5_Init();

    //LCD_ShowNum(LCD_DIST_VAL_X, LCD_DIST_VAL_Y, 10, 4, BLACK, WHITE);
    while (1) {
        LCD_ShowNum(LCD_DIST_VAL_X, LCD_DIST_VAL_Y, GetBTState(), 4, BLACK, WHITE);
        //LCD_ShowNum(LCD_DIST_VAL_X, LCD_DIST_VAL_Y, heading, 4, BLACK, WHITE);
        LCD_ShowNum(LCD_Cap1_VAL_X, LCD_Cap1_VAL_Y, distance1, 4, BLACK, WHITE);
        LCD_ShowNum(LCD_Cap2_VAL_X, LCD_Cap2_VAL_Y, distance2, 4, BLACK, WHITE);
    }
}