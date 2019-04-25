
#include "tim.h"
#include "synth.h"
#include "i2s.h"
#include "dac.h"
#include "gpio.h"
#include "led.h"
#include "adsr.h"
#include "frequency_table.h"
#include <math.h>
#include <string.h>

uint16_t i2s_buffer[BUF_SIZE]; // THE i2s buffer
uint16_t counters[NUM_OSCILLATORS], output;

Oscillator_t osc1;
Oscillator_t osc2;

Oscillator_t lfo1;
Oscillator_t lfo2;

ADSR_t voices[NUM_VOICES];
Oscillator_t *oscillators[NUM_VOICES];

uint8_t voice_cursor;

void synth_init(){
  // initialize "voice" structures; ADSR modules.
  voice_cursor = 0;
  for (int i = 0; i < NUM_VOICES; i++){
    adsr_init(&voices[i]);
    ADSR_t *a = &(voices[i]);
    adsr_set_attack(a, 4);
    adsr_set_decay(a, 4);
    adsr_set_sustain(a, 255);
    adsr_set_release(a, 128);
  }
#if ADSR_TEST
  ADSR_t *a = &(voices[0]);
  adsr_set_attack(a, 3);
  adsr_set_decay(a, 128);
  adsr_set_sustain(a, 128);
  adsr_set_release(a, 5);
#endif

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

  HAL_I2S_Transmit_DMA(&hi2s3, &i2s_buffer, BUF_SIZE * sizeof(uint16_t));
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
    // UPDATE ADSR MODULES
    float y_sum = 0;
    for (int i = 0; i < NUM_VOICES; i++){
      adsr_update(&(voices[i]));
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

    //waveCompute(&osc1, SINE_TABLE, osc1.freq);
    //waveCompute(oscillators[0], SINE_TABLE, oscillators[0]->freq);
    //i2s_buffer[pos] = (uint16_t)(40 * (oscillators[0]->out + 1) * oscillators[0]->amp);
    i2s_buffer[pos] = (int16_t) (50 * y_sum) / NUM_VOICES;
  }
}

void note_on(uint8_t key, uint8_t vel){
  ADSR_t *adsr = &voices[voice_cursor];
  adsr_excite(adsr, key);
  float freq = midi_frequency_table[key];
  oscillators[voice_cursor]->freq = freq;
  voice_cursor++;
  voice_cursor = voice_cursor % NUM_VOICES;
}

void note_off(uint8_t key){
  for (int i = 0; i < NUM_VOICES; i++){
    if (voices[i].key == key){
      adsr_release(&voices[i]);
      return;
    }
  }
}

void mixer(){
  make_sound_osc();
  synth_output();
  //update_lfo1s();
}
