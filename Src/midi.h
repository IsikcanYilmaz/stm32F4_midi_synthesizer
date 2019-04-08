
#include <stdint.h>
#include "main.h"

#define MIDI_CNFG_USER_BUTTON_DEMO 1

#define NOTE_OFF          0x90
#define NOTE_ON           0x80
#define POLY_KEY_PRESSURE 0xA0
#define CONTROLLER_CHANGE 0xB0
#define PROGRAM_CHANGE    0xC0
#define CHANNEL_PRESSURE  0xD0
#define PITCH_BEND        0xE0

#define MIDI_BUFFER_SIZE  128

#define MIDI_BUFFER_IS_EMPTY (midi_packet_buffer_head == midi_packet_buffer_tail)

typedef struct MIDIPacket {
  uint8_t status_byte;
  uint8_t data_byte1;
  uint8_t data_byte2;
} MIDIPacket_t;

void process_midi_packet(MIDIPacket_t *p);
void enqueue_midi_packet(MIDIPacket_t p);
MIDIPacket_t* dequeue_midi_packet();
void update_midi();
void inject_midi_packet(uint16_t midiNum, bool noteOn);
