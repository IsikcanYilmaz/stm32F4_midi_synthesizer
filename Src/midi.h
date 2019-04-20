
#include <stdint.h>
#include "main.h"

#define MIDI_CNFG_USER_BUTTON_DEMO 0

#define MIDI_STATUS_BYTE_FLIP 1

#if MIDI_STATUS_BYTE_FLIP
#define NOTE_OFF          0x80
#define NOTE_ON           0x90
#else
#define NOTE_OFF          0x90
#define NOTE_ON           0x80
#endif
#define POLY_KEY_PRESSURE 0xA0
#define CONTROLLER_CHANGE 0xB0
#define PROGRAM_CHANGE    0xC0
#define CHANNEL_PRESSURE  0xD0
#define PITCH_BEND        0xE0

#define MIDI_PACKET_SIZE  3 // BYTES
#define MIDI_BUFFER_SIZE  16 // PACKETS
#define MIDI_BUFFER_SIZE_BYTES (MIDI_PACKET_SIZE * MIDI_BUFFER_SIZE)
#define MIDI_USART_BUFFER_SIZE (3*MIDI_BUFFER_SIZE)
#define MIDI_DMA_BUFFER_SIZE_BYTES (MIDI_PACKET_SIZE * 16) 

#define MIDI_USART_BAUD_RATE 31250

#define MIDI_BUFFER_IS_EMPTY (midi_packet_buffer_head == midi_packet_buffer_tail)

typedef struct MIDIPacket {
  uint8_t status_byte;
  uint8_t data_byte1;
  uint8_t data_byte2;
} MIDIPacket_t;

extern volatile uint16_t midi_usart_buffer_index;

extern volatile uint16_t midi_packet_buffer_head;
extern volatile uint16_t midi_packet_buffer_tail;

extern volatile uint8_t midi_dma_buffer[MIDI_DMA_BUFFER_SIZE_BYTES];

void init_midi();
void receive_midi_packet(uint8_t data); 
void process_midi_packet(MIDIPacket_t *p);
void enqueue_midi_packet(MIDIPacket_t *p);
MIDIPacket_t* dequeue_midi_packet();
void update_midi();
void inject_midi_packet(uint16_t midiNum, bool noteOn);
uint8_t midi_usart_buffer[MIDI_USART_BUFFER_SIZE];

