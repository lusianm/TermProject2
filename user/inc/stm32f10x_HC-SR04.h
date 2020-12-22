#ifndef STM32F10X_HC_SR04_H_
#define STM32F10X_HC_SR04_H_

#include "stm32f10x.h"



#define US1_TIMER					TIM3

#define US1_TRIG_PORT				GPIOB
#define US1_TRIG_PIN					GPIO_Pin_0		//TIM Ch3 (trig output)

#define US1_ECHO_PORT				GPIOA
#define US1_ECHO_PIN					GPIO_Pin_6		//TIM Ch1 (echo input)
#define US1_TIMER_TRIG_SOURCE		TIM_TS_TI1FP1

#define US1_ECHO_Interrupt_PORT                  GPIO_PortSourceGPIOA
#define US1_ECHO_Interrupt_PIN                   GPIO_PinSource6
#define US1_ECHO_Interrupt_EXTI_LINE             EXTI_Line6

#define US2_TIMER					TIM2

#define US2_TRIG_PORT				GPIOA
#define US2_TRIG_PIN					GPIO_Pin_2		//TIM Ch3 (trig output)

#define US2_ECHO_PORT				GPIOA
#define US2_ECHO_PIN					GPIO_Pin_0		//TIM Ch1 (echo input)
#define US2_TIMER_TRIG_SOURCE		TIM_TS_TI1FP1

#define US2_ECHO_Interrupt_PORT                  GPIO_PortSourceGPIOA
#define US2_ECHO_Interrupt_PIN                   GPIO_PinSource0
#define US2_ECHO_Interrupt_EXTI_LINE             EXTI_Line0


/**
 *	How to use this driver:
 * 	1. Implement EnableHCSR04PeriphClock function and turn on clock for used peripherals
 * 		ex:
 * 		void EnableHCSR04PeriphClock() {
 *			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 *			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 *			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
 *		}
 *	2. Call InitHCSR04();
 *	3. Get distance (in mm) using HCSR04GetDistance function.
 *		Value lower than zero means no echo received (distance >3m)
 *		Measuring takes about 65ms
 *
 *	If necessary, change defines above, but be careful, US_ECHO_PIN must be tim ch1
 *	and US_TRIG_PIN must be ch3 or ch4. In case of ch4 change TIM_OC3Init into TIM_OC4Init in stm32f10x_HC-SR04.c file
 */



/**
 * Implement this function. See instruction at the top of this file.
 */
void EnableHCSR04PeriphClock();

/**
 * Initialization of HCSR04's peripherals
 */
void InitHCSR04(int32_t * d1, int32_t * d2);

/**
 * Measure distance and get value in mm. Lower than 0 means no echo signal: distance more than ~3m.
 */
int32_t HCSR04GetDistance1();
int32_t HCSR04GetDistance2();

#endif /* STM32F10X_HC_SR04_H_ */