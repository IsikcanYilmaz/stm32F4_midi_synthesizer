

#include "main.h"
#include "usbd_cdc_if.h"
#include "usart.h"
#include <stdbool.h>

#define USART_OUT_BUFFER_SIZE 256
#define USART_IN_BUFFER_SIZE 256

#define CMD_SHELL_USB_CDC 0
#define CMD_SHELL_UART    1

extern char input_buffer[USART_IN_BUFFER_SIZE];
extern char output_buffer[USART_OUT_BUFFER_SIZE];
extern uint16_t input_buffer_cursor;

#if CMD_SHELL_USB_CDC
#warning "USB CDC CMD UART"
#define print(...)      uint8_t _size = sprintf((char *) &output_buffer, __VA_ARGS__) ; CDC_Transmit_FS((uint8_t *) &output_buffer, _size);
#elif CMD_SHELL_UART
#warning "USART CMD UART"
#define print(...)      uint8_t _size = sprintf((char *) &output_buffer, __VA_ARGS__) ; HAL_UART_Transmit(&huart2, (uint8_t *) &output_buffer, _size, HAL_MAX_DELAY);
#else
#warning "CMD SHELL DISABLED"
#endif

void cmd_shell_init();
void cmd_shell_usart_interrupt_enable(bool en);
void cmd_shell_usart_isr();
uint8_t cmd_shell_cdc_receive(uint8_t *Buf, uint32_t *Len);
uint8_t cmd_shell_cdc_transmit(uint8_t *Buf, uint16_t Len);
