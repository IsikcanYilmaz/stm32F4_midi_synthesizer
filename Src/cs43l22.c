
#include "cs43l22.h"
#include "i2c.h"
#include <stdio.h>
#include <string.h>

volatile CS43L22_codec_regs_t codec_regs;

void codec_init(){
  // Set up codec data structure
  memset((void*)&codec_regs, 0x0, sizeof(CS43L22_codec_regs_t));
  uint32_t counter = 0;

  // Reset 
  codec_reset();

  // Read all registers
  volatile uint32_t test = codec_read_register(0x1);
  codec_read_all_registers();

  // The default state of the “Power Ctl. 1” register (0x02) is 0x01. Load the desired register settings while keeping the “Power Ctl 1” register set to 0x01
  // Write 0x99 to 0x00
  codec_write_register(0x00, 0x99);

  // Write 0x80 to 0x47
  codec_write_register(0x80, 0x47);

  // Write 0b1 to bit 7 in register 0x32
  volatile uint8_t reg32 = codec_read_register(0x32);
  codec_write_register(0x32, reg32 | 1 << 6);

  // Write 0b0 to bit 7 in register 0x32
  reg32 = codec_read_register(0x32);
  codec_write_register(0x32, reg32 & 0x3f);

  // Write 0x0 to 0x0
  codec_write_register(0x00, 0x00);

  //Apply MCLK at the appropriate frequency, as discussed in Section 4.6. SCLK may be applied or set to master at any time; LRCK may only be applied or set to master while the PDN bit is set to 1. 
  // 1 0 x 0 01 11 => Interface Control 1 (06h) // no need to read its probably 0s
  //codec_write_register(0x06, 0x87);

  // 1 01 0 0 00 0 => Clocking Control 1 (05h)
  //codec_write_register(0x05, 0xa0);
  // Clock configuration auto detection
  codec_write_register(0x05, 0x81);

  // Set slave mod and audio standard
  codec_write_register(0x06, 0x04);

  // Set volume
  codec_volume_control(0xf6);
  
  // Set the “Power Ctl 1” register (0x02) to 0x9E
  codec_write_register(0x02, 0x9e);

}

void codec_generate_beep(){
  // TEST
  // 1 byte payload where MSB is the freq, LSB is the on time. refer to reference manual
  uint8_t payload = ((1 << 3) << 4) | (1 << 3);
  codec_write_register(0x1c, payload);

  // 1 byte where MSB is volume, LSB is off time
  payload = 0xff >> 3;
  codec_write_register(0x1d, payload);

  // beep and tone conf. 11 0 00 00 0 for continuous test beep
  payload = 0x3 << 6;
  codec_write_register(0x1e, payload);

  // set up master volume and headphone 
  payload = (0xff >> 1) << 1;
  codec_write_register(0x20, payload);
  codec_write_register(0x21, payload);
  codec_write_register(0x22, payload);
  codec_write_register(0x23, payload);

  // PCM channel (?) 0 111 1111
  payload = (0xff >> 1);
  codec_write_register(0x1a, payload);
}

void codec_stop_beep(){
  codec_init();
}

void codec_volume_control(uint8_t vol){
  if (vol > 0xe6){
    codec_write_register(0x20, vol - 0xe7);
    codec_write_register(0x21, vol - 0xe7);
  } else {
    codec_write_register(0x20, vol - 0x19);
    codec_write_register(0x21, vol - 0x19);
  }
}

void codec_reset(){
  HAL_GPIO_WritePin(Audio_RST_GPIO_Port, Audio_RST_Pin, 0);
  int i;
  for (i = 0; i < 10000; i++){}
  HAL_GPIO_WritePin(Audio_RST_GPIO_Port, Audio_RST_Pin, 1);
}

void codec_deinit(){
  // Mute the DAC’s and PWM outputs

  // Disable soft ramp and zero cross volume transitions

  // Set the “Power Ctl 1” register (0x02) to 0x9F
}

void codec_write_register(uint8_t register_addr, uint8_t data){
  volatile HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(&hi2c1, CODEC_I2C_ADDRESS, register_addr, 1, &data, 1, HAL_MAX_DELAY);
}

uint8_t codec_read_register(uint8_t register_addr){
  uint8_t buffer;
  volatile HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c1, CODEC_I2C_ADDRESS, register_addr, 1, &buffer, 1, HAL_MAX_DELAY);
  return buffer;
}

void codec_read_all_registers(){
  int i;
  uint8_t *regs = (uint8_t *) &codec_regs;
  for (i = 0; i < sizeof(CS43L22_codec_regs_t); i++){
    uint8_t tmp = codec_read_register(i);
    regs[i] = tmp;
  }
}

