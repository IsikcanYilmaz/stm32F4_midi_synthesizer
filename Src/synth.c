
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

Oscillator_t osc1;
Oscillator_t osc2;
Oscillator_t osc3;
Oscillator_t osc4;
Oscillator_t osc5;
Oscillator_t osc6;

Oscillator_t lfo1;
Oscillator_t lfo2;

VCA_t voices[NUM_VOICES];
Oscillator_t *oscillators[NUM_VOICES];

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

  // main oscillator 
  osc1.amp = 0.5f;
  osc1.last_amp = 0.9f;
  osc1.freq = 0;
  osc1.phase = 0;
  osc1.out = 0;
  osc1.modInd = 0;
  osc1.mul = 1;

  // 2. oscillator 
  osc2.amp = 0.5f;
  osc2.last_amp = 0.9f;
  osc2.freq = 0;
  osc2.phase = 0;
  osc2.out = 0;
  osc2.modInd = 0;
  osc2.mul = 1;

  // 3. oscillator 
  osc3.amp = 0.5f;
  osc3.last_amp = 0.9f;
  osc3.freq = 0;
  osc3.phase = 0;
  osc3.out = 0;
  osc3.modInd = 0;
  osc3.mul = 1;

  // 4. oscillator 
  osc4.amp = 0.5f;
  osc4.last_amp = 0.9f;
  osc4.freq = 0;
  osc4.phase = 0;
  osc4.out = 0;
  osc4.modInd = 0;
  osc4.mul = 1;

  // 5. oscillator 
  osc5.amp = 0.5f;
  osc5.last_amp = 0.9f;
  osc5.freq = 0;
  osc5.phase = 0;
  osc5.out = 0;
  osc5.modInd = 0;
  osc5.mul = 1;

  // 6. oscillator 
  osc6.amp = 0.5f;
  osc6.last_amp = 0.9f;
  osc6.freq = 0;
  osc6.phase = 0;
  osc6.out = 0;
  osc6.modInd = 0;
  osc6.mul = 1;

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

  oscillators[0] = &osc1; // TODO make these more general
  oscillators[1] = &osc2;
  oscillators[2] = &osc3;
  oscillators[3] = &osc4;
  oscillators[4] = &osc5;
  oscillators[5] = &osc6;

  HAL_I2S_Transmit_DMA(&hi2s3, &i2s_buffer, BUF_SIZE);
}

void erase_i2s_buffer(){
  for (int i = 0; i < BUF_SIZE; i++){
    i2s_buffer[i] = 0;
  }
}


void play_note(uint8_t note, uint8_t velocity){
  //TIM1_Config(pitchtbl[note]);
}

static volatile uint16_t out_test[2];

// fill buf[0]-buf[length] with outputs of the oscillator. 
void make_sound(uint16_t begin, uint16_t end){
  static uint8_t ch = TIM_CHANNEL_1;
  static bool pressed = false;
  static int16_t last_led_speed;
  static bool playing = false;

  uint32_t test_sum = 0;

  uint16_t pos;
  uint16_t *outp;
  float y[NUM_VOICES];

  for (pos = begin; pos < end; pos++){ 
    // UPDATE VCA MODULES
    float y_sum = 0;
    for (int i = 0; i < NUM_VOICES; i++){
      vca_update(&(voices[i]));
      if (voices[i].amp == 0){
        continue;
      }
      oscillators[i]->amp = voices[i].amp;
      waveCompute(oscillators[i], SINE_TABLE, oscillators[i]->freq);
      y_sum += (oscillators[i]->out) * oscillators[i]->amp;
    }

#ifdef SINE_AMP_FREQ_SWEEP
    // SINE AMPLITUDE AND FREQ SWEEP
    float lfo2_out = 50 * (waveCompute(&lfo2, SINE_TABLE,  0.01) - 0.1);
    lfo1.amp = (lfo2_out < 1) ? lfo2_out : 1;
    lfo1.amp = (osc1.amp > 0.3) ? osc1.amp : 0.3;
    lfo1.freq = lfo2_out;
    //float lfo1_out = 8 * (waveCompute(&lfo1, SINE_TABLE,  lfo1.freq) + 1);
    float lfo1_out = led_signal[led_cursor];
    osc1.freq = lfo1_out / 8;// * lfo1_out * lfo1_out;
#endif

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
  
  //vca seems to be broken with NUM_VOICES == 4.
  //like when i set somethings state to attack
  //it freezes. bad. 
  //
  if (vca == NULL){ // ALL ARE USED
    vca = &voices[oldestVoiceIdx];
    voice_cursor = oldestVoiceIdx;
    //return; // COMMENT THIS LINE TO HAVE VOICE STEALING
  }
  vca_excite(vca, key);
  vca->noteOnNum = noteNum;
  noteNum++;
  float freq = midi_frequency_table[key];
  oscillators[voice_cursor]->freq = freq;
  //print("NOTE ON %d FREQ %f. ASSIGNING TO VOICE # %d\n", key, freq, voice_cursor);
  
  print("NOTE ON %d. VOICE %d. VCA %x STATE %d FREQ %f TABLEFREQ %f\n", key, voice_cursor, vca, vca->state, oscillators[voice_cursor]->freq, freq);

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
