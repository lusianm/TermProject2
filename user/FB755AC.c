#include "FB755AC.h"
#include "stm32f10x.h"
#include "lcd.h"

uint16_t prevDataBuffer[100] = {0, };
int prevDataBufferIdx = 0;
uint16_t prevData = 0;
int BTState = 0;
//0 : Start
//1 : Paging 요청
//2 : Connect 대기
//3 : Connect 완료

uint16_t RCCommand = 's';

uint16_t FB755ACGetCommand(){
  uint16_t tempCommand = RCCommand;
  RCCommand = 's';
  return tempCommand;
}

int GetBTState(){
  return BTState;
}

int IsBTReady(){
  return (BTState == 3);
}

int IsBTOK(){
  if(prevDataBuffer[0] == 'O' && prevDataBuffer[1] == 'K' && prevDataBuffer[2] == 0x0D && prevDataBuffer[3] == 0x0A){
    return 1;
  }
  else
    return 0;
}

int IsBTCONNECT(){
  if(prevDataBuffer[0] == 'C' && prevDataBuffer[1] == 'O' && prevDataBuffer[2] == 'N' && prevDataBuffer[3] == 'N' && prevDataBuffer[4] == 'E' &&prevDataBuffer[5] == 'C' &&prevDataBuffer[6] == 'T'){
    return 1;
  }
  else
    return 0;
}

int IsBTDISCONNECT(){
  if(prevDataBuffer[0] == 'D' && prevDataBuffer[1] == 'I' && prevDataBuffer[2] == 'S' && prevDataBuffer[3] == 'C' && prevDataBuffer[4] == 'O' && prevDataBuffer[5] == 'N' && prevDataBuffer[6] == 'N' && prevDataBuffer[7] == 'E' &&prevDataBuffer[8] == 'C' &&prevDataBuffer[9] == 'T'){
    return 1;
  }
  else
    return 0;
}

void AutoConnectReady(){
  uint16_t data[10] = {'A', 'T', '+', 'B', 'T', 'S', 'C', 'A', 'N', 0x0D};
  for(int i = 0; i < 10; i++){
    while ((USART1->SR & USART_SR_TC) == 0);
    USART_SendData(BTUSART, data[i]);
  }
}

static void InitPins() {
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //TX
  GPIO_InitStructure.GPIO_Pin = USARTPinTX;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       
  GPIO_Init(USARTPort, &GPIO_InitStructure);
  
  //RX
  GPIO_InitStructure.GPIO_Pin = USARTPinRX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;       
  GPIO_Init(USARTPort, &GPIO_InitStructure);
}

void USART1_Init(void)
{
    USART_InitTypeDef USART1_InitStructure;

    // Enable the USART1 peripheral
    USART_Cmd(USART1, ENABLE);

    // TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
       //@ USART Word Length : 8 bit
    USART1_InitStructure.USART_WordLength = USART_WordLength_8b;
    //@ USART Stop Bit : 1
    USART1_InitStructure.USART_StopBits = USART_StopBits_1;
    //@ USART Parity bit : None
    USART1_InitStructure.USART_Parity = USART_Parity_No;
    //@ USART Mode : RX, TX
    USART1_InitStructure.USART_Mode = (USART_Mode_Rx | USART_Mode_Tx);
    //@ Hardware Flow Control : Node
    USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    //@ Serial BaudRate : 9600
    USART1_InitStructure.USART_BaudRate = 9600;
    USART_Init(USART1, &USART1_InitStructure);
    // TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
    //@ RXNE : Rx Not Empty
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}


void USART1_IRQHandler() {
    uint16_t word;
    if (USART_GetITStatus(BTUSART, USART_IT_RXNE) != RESET) {
        // the most recent received data by the USART1 peripheral
        //@ UART2의 입력을 word에 저장
        word = USART_ReceiveData(BTUSART);
        if(word == 'f' || word == 'b' || word =='r' || word =='l' || word == 's')
          RCCommand = word;
        prevDataBuffer[prevDataBufferIdx] = word;
        prevDataBufferIdx++;
        prevDataBufferIdx %= 100;
        if(prevData == 0x0D && word == 0x0A){
          if(prevDataBufferIdx > 3){
            switch(BTState){
            case 0:
              if(IsBTOK()){
                LCD_Clear(BRRED);
                BTState = 1;
                AutoConnectReady();
              }              
              break;
            
            case 1:
              if(!IsBTOK()){
                AutoConnectReady();
              }
              else{
                LCD_Clear(GREEN);
                BTState = 2;
              }
              break;
              
            case 2:
              if(IsBTCONNECT()){
                LCD_Clear(WHITE);
                BTState = 3;
              }
              break;
            case 3:
              if(IsBTDISCONNECT()){
                LCD_Clear(GREEN);
                BTState = 2;                
              }
              break;
            
            }
          }
          prevDataBufferIdx = 0;
          for(int i = 0; i < 100; i++){
            prevDataBuffer[i] = 0;
          }
        }
        prevData = word;      
        // clear 'Read data register not empty' flag
        USART_ClearITPendingBit(BTUSART, USART_IT_RXNE);        
    }
}


void FB755ACInit(){
  InitPins();
  USART1_Init();
}