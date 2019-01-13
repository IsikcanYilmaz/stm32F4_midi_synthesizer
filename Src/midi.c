
#include "midi.h"
#include "synth.h"

// https://www.nyu.edu/classes/bello/FMT_files/9_MIDI_code.pdf // 
// https://users.cs.cf.ac.uk/Dave.Marshall/Multimedia/node158.html //
//

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
