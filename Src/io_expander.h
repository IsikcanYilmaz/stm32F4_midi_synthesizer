

#include "main.h"
#define IO_EXPANDER_I2C      I2C3
#define IO_EXPANDER_I2C_ADDR 0x00

#define IO_EXPANDER_WRITE 0x0
#define IO_EXPANDER_READ  0x1

// IO EXPANDER REGISTERS
#define IO_EXP_IODIRA    0x00
#define IO_EXP_IODIRB    0x01
#define IO_EXP_IPOLA     0x02
#define IO_EXP_IPOLB     0x03
#define IO_EXP_GPINTENA  0x04
#define IO_EXP_GPINTENB  0x05
#define IO_EXP_DEFVALA   0x06
#define IO_EXP_DEFVALB   0x07
#define IO_EXP_INTCONA   0x08
#define IO_EXP_INTCONB   0x09
#define IO_EXP_IOCONA    0x0A
#define IO_EXP_IOCONB    0x0B
#define IO_EXP_GPPUA     0x0C
#define IO_EXP_GPPUB     0x0D
#define IO_EXP_INTFA     0x0E
#define IO_EXP_INTFB     0x0F
#define IO_EXP_INTCAPA   0x010
#define IO_EXP_INTCAPB   0x011
#define IO_EXP_GPIOA     0x012
#define IO_EXP_GPIOB     0x013
#define IO_EXP_OLATA     0x014
#define IO_EXP_OLATB     0x015

// NOTES OF THE SINGLE OCTAVE ON BREADBOARD
// MASKS CORRESPOND TO IO EXPANDER RESPONSE
#define NOTE_C    (0x1 << 0)
#define NOTE_D    (0x1 << 1)
#define NOTE_E    (0x1 << 2)
#define NOTE_F    (0x1 << 3)
#define NOTE_G    (0x1 << 4)
#define NOTE_A    (0x1 << 5)
#define NOTE_B    (0x1 << 6)
#define NOTE_Cs   (0x1 << 7)
#define NOTE_Ds   (0x1 << 6)
#define NOTE_Fs   (0x1 << 5)
#define NOTE_Gs   (0x1 << 4)
#define NOTE_As   (0x1 << 3)


void io_expander_init();
void io_expander_test();
uint8_t io_expander_read_register(uint8_t register_addr);
bool io_expander_write_register(uint8_t register_addr, uint8_t data);
void poll_keybs();
uint8_t pin_to_midi_value(uint8_t p, bool port_a);
