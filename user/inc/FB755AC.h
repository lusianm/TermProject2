#ifndef __FB755AC_H
#define __FB755AC_H
#include "stm32f10x.h"


#define BTUSART                 USART1

#define USARTPort               GPIOA
#define USARTPinRX              GPIO_Pin_10
#define USARTPinTX              GPIO_Pin_9


//function
int GetBTState();
int IsBTReady();
int IsBTOK();
int IsBTCONNECT();
int IsBTDISCONNECT();
void AutoConnectReady();
static void InitPins();
void USART2_Init(void);
void USART2_IRQHandler();
void sendDataUART2(uint16_t data);
void FB755ACInit();
uint16_t FB755ACGetCommand();
#endif