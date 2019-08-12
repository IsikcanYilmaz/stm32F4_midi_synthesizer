
#include "tim.h"
#include "synth.h"
#include "i2s.h"
#include "gpio.h"
#include "led.h"
#include "vca.h"
#include "frequency_table.h"
#include "cmd_shell.h"
#include <math.h>
#include <string.h>

int16_t i2s_buffer[BUF_SIZE]; // THE i2s buffer
uint16_t counters[NUM_OSCILLATORS], output;

Oscillator_t lfo1;
Oscillator_t lfo2;

VCA_t voices[NUM_VOICES];
Oscillator_t oscillators[NUM_VOICES];

uint8_t voice_cursor;

void synth_init(){
  // initialize "voice" structures; VCA modules.
  voice_cursor = 0;
  for (int i = 0; i < NUM_VOICES; i++){
    vca_init(&voices[i]);
    VCA_t *v = &(voices[i]);
    vca_set_attack(v, 1);
    vca_set_decay(v, 255);
    vca_set_sustain(v, 255);
    vca_set_release(v, 255);
  }

  // Initialize the oscillators
  for (int i = 0; i < NUM_OSCILLATORS; i++){
    Oscillator_t *o = &oscillators[i];
    o->amp = 0.5f;
    o->last_amp = 0.9f;
    o->freq = 0;
    o->phase = 0;
    o->out = 0;
    o->modInd = 0;
    o->mul = 1;
  }

  // lfo1 for sine sweep
  lfo1.amp = 0.5f;
  lfo1.last_amp = 0.9f;
  lfo1.freq = 1;
  lfo1.phase = 0;
  lfo1.out = 0;
  lfo1.modInd = 0;
  lfo1.mul = 1;

  // lfo2 for sine sweep sweep
  lfo2.amp = 0.5f;
  lfo2.last_amp = 0.9f;
  lfo2.freq = 0.1;
  lfo2.phase = 0;
  lfo2.out = 0;
  lfo2.modInd = 0;
  lfo2.mul = 1;

  HAL_I2S_Transmit_DMA(&hi2s3, &i2s_buffer, BUF_SIZE);
}

void erase_i2s_buffer(){
  for (int i = 0; i < BUF_SIZE; i++){
    i2s_buffer[i] = 0;
  }
}

// fill buf[begin]-buf[end] with outputs of the oscillator. 
void make_sound(uint16_t begin, uint16_t end){
  uint16_t pos;
  uint16_t *outp;
  float y[NUM_VOICES];

  // For each sample
  for (pos = begin; pos < end; pos++){ 
    float y_sum = 0;

    // go through all voices, oscillators, calculate the amplitude value of the sample
    for (int i = 0; i < NUM_VOICES; i++){
      // update the vca
      vca_update(&(voices[i]));
      if (voices[i].amp == 0){
        continue;
      }
      oscillators[i].amp = voices[i].amp;

      // update the oscillator, get its output for the current sample
      waveCompute(&oscillators[i], SINE_TABLE, oscillators[i].freq);
      y_sum += (oscillators[i].out) * oscillators[i].amp;
    }
    
    // scale the output, save to i2s_buffer.
    int16_t y_scaled = (int16_t) (y_sum * 0x800);
    i2s_buffer[pos] = (y_scaled);
  }
}

void note_on(uint8_t key, uint8_t vel){
  VCA_t *vca = NULL;
  static uint32_t noteNum = 0;

  // FIND A VOICE THATS UNUSED
  // IF ALL USED <DECIDE>
  uint32_t oldest = noteNum;
  uint8_t oldestVoiceIdx = 0;
  for (int i = 0; i < NUM_VOICES; i++){
    //print("VOICE %d STATE %d/%d\n", i, voices[i].state, NUM_VOICES);

    // RECORD THE OLDEST HIT NOTE/VCA OBJ
    if (voices[i].noteOnNum < oldest){
      oldest= voices[i].noteOnNum;
      oldestVoiceIdx = i;
    }

    // NOTE ALREADY ON IN A VOICE. RETRIGGER IT
    if (voices[i].key == key && voices[i].state < VCA_DONE){
      vca = &voices[i];
      voice_cursor = i;
      break;
    }

    // FREE VOICE SLOT FOUND
    if (voices[i].state == VCA_DONE){
      vca = &voices[i];
      voice_cursor = i;
      break;
    }
  }

  if (vca == NULL){ // ALL ARE USED
    vca = &voices[oldestVoiceIdx];
    voice_cursor = oldestVoiceIdx;
    //return; // COMMENT THIS LINE TO HAVE VOICE STEALING
  }
  vca_excite(vca, key);
  vca->noteOnNum = noteNum;
  noteNum++;
  float freq = midi_frequency_table[key];
  oscillators[voice_cursor].freq = freq;
  //print("NOTE ON %d. VOICE %d. VCA %x STATE %d FREQ %f TABLEFREQ %f\n", key, voice_cursor, vca, vca->state, oscillators[voice_cursor].freq, freq);

}

void note_off(uint8_t key){
  for (int i = 0; i < NUM_VOICES; i++){
    if (voices[i].key == key){
      print("NOTE %d OFF. WAS AT VOICE # %d\n", key, i);
      vca_release(&voices[i]);
      return;
    }
  }
  //print("NOTE %d OFF. VOICE NOT FOUND\n", key);
}

void mixer(){
  make_sound_osc();
  synth_output();
  //update_lfo1s();
}

/////////////
// On DMA Half transfer and Full transfer, the below two callback functions will be called.
// The trick is that we update the first half of the i2s_buffer when the first half's transfer
// is done, update the second half once the transfer of the second half is done and we are transfering
// the first half again.
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s){
  make_sound(0, BUF_SIZE_DIV2);
}


void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s){
  make_sound(BUF_SIZE_DIV2, BUF_SIZE);
}
//
