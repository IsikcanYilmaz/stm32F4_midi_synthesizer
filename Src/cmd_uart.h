

#include "main.h"
#define USART_OUT_BUFFER_SIZE 256
#define USART_IN_BUFFER_SIZE 256

extern char input_buffer[USART_IN_BUFFER_SIZE];
extern char output_buffer[USART_OUT_BUFFER_SIZE];

#define print(...) uint8_t _size = sprintf(output_buffer, __VA_ARGS__) ; HAL_UART_Transmit(&huart2, (uint8_t *) &output_buffer, _size, HAL_MAX_DELAY);
