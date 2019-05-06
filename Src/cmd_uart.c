

#include "cmd_uart.h"
#include "stm32f4xx_hal_usart.h"
#include "usbd_cdc_if.h"
#include "usart.h"

#include <string.h>

char input_buffer[USART_IN_BUFFER_SIZE];
char output_buffer[USART_OUT_BUFFER_SIZE];

uint16_t input_buffer_cursor = 0;

void cmd_uart_init(){
#if USB_CDC_UART

#else
  cmd_uart_interrupt_enable(true);
#endif
}


void cmd_uart_interrupt_enable(bool en){
  if (en){
    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
    //__HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
  } else {
    while((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET) || 
           __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) != RESET){
      __NOP();
    }
    NVIC_DisableIRQ(USART2_IRQn);
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
    //__HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
  }
}

void cmd_uart_isr(){
  // HANDLE POTENTIAL ERRORS
  uint32_t status = huart2.Instance->SR;
  if (huart2.Instance->CR1 & USART_IT_TXE & status){  // TRANSMIT DATA BUFFER EMPTY

  }

  if (status & USART_SR_FE) { // FRAMING ERROR

  }

  if (status & USART_SR_ORE) { // OVERRUN ERROR

  }

  if (status & USART_SR_RXNE) { // READ DATA BUFFER NOT EMPTY
    uint8_t data = huart2.Instance->DR;
    
    // IF NEW LINE THIS IS A COMMAND
    if (data == (uint8_t)'\n'){
      //print("COMMAND %s \n", input_buffer);
      memset(&input_buffer, 0, USART_IN_BUFFER_SIZE);
    } else {
      //print("char %c 0x%x\n", data, data);
    }

    // WRITE TO INPUT BUFFER 
    input_buffer[input_buffer_cursor] = data;
    input_buffer_cursor++;
    if(input_buffer_cursor >= USART_IN_BUFFER_SIZE){
      input_buffer_cursor = 0;
    }
  }
}

