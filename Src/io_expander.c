
/*
 * Driver for the MCP23S17 16-Bit SPI I/O Expander with Serial Interface Microchip IC.
 */

#include "io_expander.h"
#include "spi.h"
#include "gpio.h"
#include "usart.h"
#include "cmd_uart.h"
#include "midi.h"
#include <stdio.h>
#include <string.h>

/* Communicates thru SPI 2.
 * PE4  -> CS
 * PC2  -> MISO
 * PB15 -> MOSI
 * PB13 -> SCK
 */

uint8_t regs[2]; // 2 sets of regs.


void io_expander_init(){
  HAL_GPIO_WritePin(GPIOE, CS_IO_EXPANDER_Pin, GPIO_PIN_SET); // CS to remain high

  // Slew rate disabled
  io_expander_write_register(IO_EXP_IOCONA, 0x08);
  io_expander_write_register(IO_EXP_IOCONB, 0x08);
}

uint8_t io_expander_read_register(uint8_t register_addr){
  uint8_t buffer;
  uint8_t payload = register_addr;
  uint8_t ctrl_byte = 0x40 | IO_EXPANDER_READ; // 0b01000001 for read
  HAL_GPIO_WritePin(GPIOE, CS_IO_EXPANDER_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2, &ctrl_byte, 1, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi2, &payload, 1, HAL_MAX_DELAY);
  HAL_SPI_Receive(&hspi2, &buffer, 1, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(GPIOE, CS_IO_EXPANDER_Pin, GPIO_PIN_SET);
  return buffer;
}

bool io_expander_write_register(uint8_t register_addr, uint8_t data){
  bool ret = true;
  uint8_t ctrl_byte = 0x40 | IO_EXPANDER_WRITE; // 0b01000000 for write
  HAL_GPIO_WritePin(GPIOE, CS_IO_EXPANDER_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2, &ctrl_byte, 1, HAL_MAX_DELAY);
  HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(GPIOE, CS_IO_EXPANDER_Pin, GPIO_PIN_SET);
  return ret;
}

void poll_keybs(){
  // TODO make this a uint16 and merge the below if blocks
  uint8_t port_a = io_expander_read_register(IO_EXP_GPIOA);
  uint8_t port_b = io_expander_read_register(IO_EXP_GPIOB);
  uint8_t port_a_diff = port_a ^ regs[0];
  uint8_t port_b_diff = port_b ^ regs[1];

  // see if the state of the keybs is different than what we got in memory
  // if so, fire midi event
  int i;
  if (port_a_diff){ // if there has been a change in the state of the keybs
    for (i = 0; i < 8; i++){ // go through all the bits
      if (port_a_diff & (0x01 << i)){ // see which pins state has changed
        MIDIPacket_t p;
        if (port_a & (0x01 << i)) { // if key press
          p.status_byte = NOTE_ON;
          p.data_byte1 = pin_to_midi_value(port_a_diff & (0x01 << i), true);
          p.data_byte2 = 50; 
        } else { // if let go
          p.status_byte = NOTE_OFF;
          p.data_byte1 = 50;
          p.data_byte2 = pin_to_midi_value(port_a_diff & (0x01 << i), true);
        }
        enqueue_midi_packet(p);
      }
    }
    regs[0] = port_a;
  }
  if (port_b_diff){ // same as above
    for (i = 0; i < 8; i++){
      if (port_b_diff & (0x01 << i)){
        MIDIPacket_t p;
        if (port_b & (0x01 << i)) {
          p.status_byte = NOTE_ON;
          p.data_byte1 = pin_to_midi_value(port_b_diff & (0x01 << i), false);
          p.data_byte2 = 50; 
        } else { // if let go
          p.status_byte = NOTE_OFF;
          p.data_byte1 = 50;
          p.data_byte2 = pin_to_midi_value(port_b_diff & (0x01 << i), false);
        }
        enqueue_midi_packet(p);
      }
    }
    regs[1] = port_b;
  }
}

uint8_t pin_to_midi_value(uint8_t p, bool port_a){
  // true for port a, false for port b
  // gpio pin to switch on breadboard kinda fucky. this function will tell which note is being
  // pressed, and return its midi value for ease of use. Starting with C4=60. Doing the offsetting
  // is to be done outside of this function
  // note that this function will barf if the argument has more than 1 high bit. 
  uint8_t ret = 0;
  if (port_a){
    switch(p){
      case NOTE_Cs:
        ret = 61;
        break;
      case NOTE_Ds:
        ret = 63;
        break;
      case NOTE_Fs:
        ret = 66;
        break;
      case NOTE_Gs:
        ret = 68;
        break;
      case NOTE_As:
        ret = 70;
        break;
    }
  } else {
    switch(p){
      case NOTE_C:
        ret = 60;
        break;
      case NOTE_D:
        ret = 62;
        break;
      case NOTE_E:
        ret = 64;
        break;
      case NOTE_F:
        ret = 65;
        break;
      case NOTE_G:
        ret = 67;
        break;
      case NOTE_A:
        ret = 69;
        break;
      case NOTE_B:
        ret = 71;
        break;
    }
  }
  return ret;
}


