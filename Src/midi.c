
#include "midi.h"
#include "gpio.h"
#include "led.h"
#include "synth.h"

// https://www.nyu.edu/classes/bello/FMT_files/9_MIDI_code.pdf // 
// https://users.cs.cf.ac.uk/Dave.Marshall/Multimedia/node158.html //
//

MIDIPacket_t midi_packet_buffer[MIDI_BUFFER_SIZE]; // TODO make this better // implement general purpose fifo
uint8_t midi_packet_buffer_head = 0;
uint8_t midi_packet_buffer_tail = 0;

// puts packet in fifo. increments tail
void enqueue_midi_packet(MIDIPacket_t p){
  midi_packet_buffer[midi_packet_buffer_tail] = p;
  midi_packet_buffer_tail++;
  midi_packet_buffer_tail = midi_packet_buffer_tail % MIDI_BUFFER_SIZE;
}

// returns address to packet at head of the buffer. increments head.
MIDIPacket_t* dequeue_midi_packet(){
  MIDIPacket_t *ret = &midi_packet_buffer[midi_packet_buffer_head];
  midi_packet_buffer_head++;
  midi_packet_buffer_head = midi_packet_buffer_head % MIDI_BUFFER_SIZE;
  return ret;
}

void update_midi(){
#if MIDI_CNFG_USER_BUTTON_DEMO // user button acts as a midi button press
  static bool pressed = false;
  if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){
    if (pressed == false){ // BUTTON PRESS EVENT
      pressed = true;
      inject_midi_packet(61, true); 
    }
  } else {
    if (pressed == true){ // BUTTON RELEASE EVENT
      pressed = false;
      inject_midi_packet(61, false); 
    }
  }

#endif

  while(midi_packet_buffer_head != midi_packet_buffer_tail){
    process_midi_packet(dequeue_midi_packet());
  }
}

void process_midi_packet(MIDIPacket_t *p){
  uint8_t key, vel;
  switch(p->status_byte & 0xf0){
    case NOTE_OFF:
      key = p->data_byte2;
      vel = p->data_byte1;
      note_off(key);
      break;
    case NOTE_ON:
      key = p->data_byte1;
      vel = p->data_byte2;
      note_on(key, vel);
      break;
    case POLY_KEY_PRESSURE:

      break;
    case PITCH_BEND:

      break;
    default:
      break;
  }
}

void inject_midi_packet(uint16_t midiNum, bool noteOn){
  MIDIPacket_t p;
  uint8_t key = 49; // THIS MAY BE WRONG. MAY HAVE TO -20 FROM KEY VALUES
  uint8_t vel = 100;
  if (noteOn){
    p.data_byte1 = key;
    p.data_byte2 = vel;
    p.status_byte = NOTE_ON;
  } else {
    p.data_byte2 = key;
    p.data_byte1 = vel;
    p.status_byte = NOTE_OFF;
  }
  enqueue_midi_packet(p);
}
