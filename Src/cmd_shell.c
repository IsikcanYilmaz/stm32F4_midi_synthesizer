

#include "cmd_shell.h"
#include "stm32f4xx_hal_usart.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"
#include "usart.h"
#include "led.h"

#include <string.h>

char input_buffer[USART_IN_BUFFER_SIZE];
char output_buffer[USART_OUT_BUFFER_SIZE];

uint16_t input_buffer_cursor = 0;

void cmd_shell_init(){
#if USB_CDC_UART

#else
  cmd_shell_interrupt_enable(true);
#endif
}


void cmd_shell_usart_interrupt_enable(bool en){
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

// This gets called in CDC_Transmit_FS. transmits data over CDC
uint8_t cmd_shell_cdc_transmit(uint8_t *Buf, uint16_t Len){
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  uint8_t result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  return result;
}

// This gets called in CDC_Receive_FS. receives data over CDC
uint8_t cmd_shell_cdc_receive(uint8_t *Buf, uint32_t *Len){
#define USB_CDC_BUFFER_SIZE 128                                                
  static uint8_t bufferCursor = 0;                                            
  static bool commandBeingEntered = false;                                     

  uint8_t *rxBuffer = Buf;                                                     

  if (commandBeingEntered == false){                                           
    // Beginning of a command received. '['  
    for (int i = 0; i < *Len; i++){                                            
      if (rxBuffer[i] == '['){                                                 
        commandBeingEntered = true;                                            

        *Len -= i;                                                             
        rxBuffer = &Buf[i+1];                                                  
        bufferCursor++;                                                       
        break;                                                                 
      }                                                                        
    }                                                                          
  }                                                                            

  if (commandBeingEntered && *Len > 0){                                        

    // Go through each received byte                                           
    for (int i = 0; i < *Len; i++){                                            
      bufferCursor++;                                                         
      if (rxBuffer[i] == ']'){                                                 
        // Command finished                                                    
        // ROUTE COMMAND                                                       
        print("]\r\n");                                                        
        bufferCursor = 0;                                                     
        commandBeingEntered = false;                                           
        break;                                                                 
      }                                                                        
    }                                                                          
    CDC_Transmit_FS(Buf, *Len);                                                

    // Command buffer overflow. reset it                                       
    if (bufferCursor + *Len > USB_CDC_BUFFER_SIZE){                           
      bufferCursor = 0;                                                       
      commandBeingEntered = false;                                             
    }                                                                          

  }                                                                            

  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);                                
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);                                       
  return (USBD_OK);
}

void cmd_shell_isr(){
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
      print("COMMAND %s \n", input_buffer);
      memset(&input_buffer, 0, USART_IN_BUFFER_SIZE);
    } else {
      print("char %c 0x%x\n", data, data);
    }

    // WRITE TO INPUT BUFFER 
    input_buffer[input_buffer_cursor] = data;
    input_buffer_cursor++;
    if(input_buffer_cursor >= USART_IN_BUFFER_SIZE){
      input_buffer_cursor = 0;
    }
  }
}

