#include <stm32f10x_HC-SR04.h>

#define LCD_DIST_VAL_X 20
#define LCD_DIST_VAL_Y 70

#define LCD_Cap1_VAL_X 20
#define LCD_Cap1_VAL_Y 90

#define LCD_Cap2_VAL_X 20
#define LCD_Cap2_VAL_Y 110

int32_t * EchoDistance1;
int32_t * EchoDistance2;




extern void EnableHCSR04PeriphClock();

/*
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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
*/
static void initMeasureTimer() {
	RCC_ClocksTypeDef RCC_ClocksStatus;
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	uint16_t prescaler = RCC_ClocksStatus.SYSCLK_Frequency / 1000000 - 1; //1 tick = 1us (1 tick = 0.165mm resolution)

	TIM_DeInit(US1_TIMER);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(US1_TIMER, &TIM_TimeBaseInitStruct);
        
	TIM_DeInit(US2_TIMER);
	TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFF;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(US2_TIMER, &TIM_TimeBaseInitStruct);

	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 15; //us
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(US1_TIMER, &TIM_OCInitStruct);

	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 15; //us
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(US2_TIMER, &TIM_OCInitStruct);
        
	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStruct.TIM_ICFilter = 0;      

	TIM_PWMIConfig(US1_TIMER, &TIM_ICInitStruct);
	TIM_PWMIConfig(US2_TIMER, &TIM_ICInitStruct);
        
	TIM_SelectInputTrigger(US1_TIMER, US1_TIMER_TRIG_SOURCE);
	TIM_SelectMasterSlaveMode(US1_TIMER, TIM_MasterSlaveMode_Enable);
	TIM_CtrlPWMOutputs(US1_TIMER, ENABLE);
	TIM_ClearFlag(US1_TIMER, TIM_FLAG_Update);
        
	TIM_SelectInputTrigger(US2_TIMER, US2_TIMER_TRIG_SOURCE);
	TIM_SelectMasterSlaveMode(US2_TIMER, TIM_MasterSlaveMode_Enable);
	TIM_CtrlPWMOutputs(US2_TIMER, ENABLE);
	TIM_ClearFlag(US2_TIMER, TIM_FLAG_Update);
}

void EXTI_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    // TODO: Select the GPIO pin (Joystick, button) used as EXTI Line using function 'GPIO_EXTILineConfig'
    // TODO: Initialize the EXTI using the structure 'EXTI_InitTypeDef' and the function 'EXTI_Init'

    /* Button */
    GPIO_EXTILineConfig(US1_ECHO_Interrupt_PORT, US1_ECHO_Interrupt_PIN);
    //@  EXTI Line : 6
    EXTI_InitStructure.EXTI_Line = US1_ECHO_Interrupt_EXTI_LINE;
    //@  EXTI Mode : Interrupt
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //@ Interrupt Trigger : Falling
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    // NOTE: do not select the UART GPIO pin used as EXTI Line here
    
    /* Button */
    GPIO_EXTILineConfig(US2_ECHO_Interrupt_PORT, US2_ECHO_Interrupt_PIN);
    //@  EXTI Line : 6
    EXTI_InitStructure.EXTI_Line = US2_ECHO_Interrupt_EXTI_LINE;
    //@  EXTI Mode : Interrupt
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    //@ Interrupt Trigger : Falling
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    // NOTE: do not select the UART GPIO pin used as EXTI Line here
}

static void initPins() {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = US1_TRIG_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(US1_TRIG_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = US1_ECHO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(US1_ECHO_PORT, &GPIO_InitStructure);
        
        
	GPIO_InitStructure.GPIO_Pin = US2_TRIG_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(US2_TRIG_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = US2_ECHO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(US2_ECHO_PORT, &GPIO_InitStructure);
}

void InitHCSR04(int32_t * d1, int32_t * d2) {
        EchoDistance1 = d1;
        EchoDistance2 = d2;
	EnableHCSR04PeriphClock();
	initPins();
	initMeasureTimer();
        EXTI_Configure();
        //NVIC_Configure();
}

//@ Joystick Up Interrupt
void EXTI9_5_IRQHandler() { // Joystick Up

    if (EXTI_GetITStatus(US1_ECHO_Interrupt_EXTI_LINE) != RESET) {
        if (GPIO_ReadInputDataBit(US1_ECHO_PORT, US1_ECHO_PIN) == Bit_RESET) {
          *EchoDistance1 = HCSR04GetDistance1();
        }
        EXTI_ClearITPendingBit(US1_ECHO_Interrupt_EXTI_LINE);
    }
}

//@ Joystick Up Interrupt
void EXTI0_IRQHandler() { // Joystick Up

    if (EXTI_GetITStatus(US2_ECHO_Interrupt_EXTI_LINE) != RESET) {
        if (GPIO_ReadInputDataBit(US2_ECHO_PORT, US2_ECHO_PIN) == Bit_RESET) {
          *EchoDistance2 = HCSR04GetDistance2();
        }
        EXTI_ClearITPendingBit(US2_ECHO_Interrupt_EXTI_LINE);
    }
}

int32_t HCSR04GetDistance1() {
	(US1_TIMER)->CNT = 0;
	TIM_Cmd(US1_TIMER, ENABLE);
	while(!TIM_GetFlagStatus(US1_TIMER, TIM_FLAG_Update));
	// TIM_Cmd(US1_TIMER, DISABLE);
	TIM_ClearFlag(US1_TIMER, TIM_FLAG_Update);
        int32_t Cap1 = TIM_GetCapture1(US1_TIMER);
        int32_t Cap2 = TIM_GetCapture2(US1_TIMER);
        int32_t dist = (Cap2-Cap1)*165/1000;
        /*
        LCD_ShowNum(LCD_DIST_VAL_X, LCD_DIST_VAL_Y, dist, 4, BLACK, WHITE);
        LCD_ShowNum(LCD_Cap1_VAL_X, LCD_Cap1_VAL_Y, Cap1, 4, BLACK, WHITE);
        LCD_ShowNum(LCD_Cap2_VAL_X, LCD_Cap2_VAL_Y, Cap2, 4, BLACK, WHITE);
        */
	return dist;
}

int32_t HCSR04GetDistance2() {
	(US2_TIMER)->CNT = 0;
	TIM_Cmd(US2_TIMER, ENABLE);
	while(!TIM_GetFlagStatus(US2_TIMER, TIM_FLAG_Update));
	// TIM_Cmd(US1_TIMER, DISABLE);
	TIM_ClearFlag(US2_TIMER, TIM_FLAG_Update);
        int32_t Cap1 = TIM_GetCapture1(US2_TIMER);
        int32_t Cap2 = TIM_GetCapture2(US2_TIMER);
        int32_t dist = (Cap2-Cap1)*165/1000;
        /*
        LCD_ShowNum(LCD_DIST_VAL_X, LCD_DIST_VAL_Y, dist, 4, BLACK, WHITE);
        LCD_ShowNum(LCD_Cap1_VAL_X, LCD_Cap1_VAL_Y, Cap1, 4, BLACK, WHITE);
        LCD_ShowNum(LCD_Cap2_VAL_X, LCD_Cap2_VAL_Y, Cap2, 4, BLACK, WHITE);
        */
	return dist;
}