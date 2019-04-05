
#include "tim.h"
#include "synth.h"
#include "i2s.h"
#include "dac.h"
#include "gpio.h"
#include "led.h"
#include <math.h>
#include <string.h>

uint16_t i2s_buffer[BUF_SIZE]; // THE i2s buffer
uint16_t test_buffer[BUF_SIZE];
uint16_t counters[NUM_OSCILLATORS], output;
uint16_t buffer_index;

int16_t signal[256];
int nsamples;


Oscillator_t indexOsc;
Oscillator_t sineOsc;

float sineWavetable[BUF_SIZE];
float indexWavetable[BUF_SIZE_DIV2];

uint8_t test_on_note = 0;

Oscillator_t osc1;

Oscillator_t lfo1;
Oscillator_t lfo2;

void synth_init(){
  // main oscillator 
  osc1.amp = 0.5f;
  osc1.last_amp = 0.9f;
  osc1.freq = 440;
  osc1.phase = 0;
  osc1.out = 0;
  osc1.modInd = 0;
  osc1.mul = 1;

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


  uint16_t j;

  HAL_I2S_Transmit_DMA(&hi2s3, &i2s_buffer, BUF_SIZE * 2);
}

void erase_i2s_buffer(){
  for (int i = 0; i < BUF_SIZE; i++){
    i2s_buffer[i] = 0;
  }
}

void test_bump_pitch(bool up){
  static int test_midi_num = 0 ; //61 - 24;
  /*osc1.mul += MIDI_TO_FREQ(test_midi_num);
    test_midi_num++;*/
  if (up){
    osc1.freq += 12 * 3; 
  } else {
    osc1.freq -= 12 * 3;
  }

  /*for (int j = 0; j < (BUF_SIZE); j+=2){
    i2s_buffer[j] = (uint16_t)(1024 * (osc1.out + 1) * osc1.amp);
    i2s_buffer[j + 1] = (uint16_t)(1024 * (osc1.out + 1) * osc1.amp);
    update_oscillator(&osc1);
    }*/
  if (test_midi_num > 61 * 3){
    test_midi_num = 61 * 3;
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
  float y;

  pressed = true;
  for (pos = begin; pos < end; pos++){ 
    //y = update_oscillator(&osc1);
    //i2s_buffer[j] = (uint16_t)(1024 * (y + 1) * osc1.amp);
    //i2s_buffer[j + 1] = (uint16_t)(1024 * (y + 1) * osc1.amp);
    //
    float lfo2_out =  (waveCompute(&lfo1, SINE_TABLE,  0.1) + 1);
    osc1.amp = lfo2_out;
    float lfo1_out = 8 * (waveCompute(&lfo1, SINE_TABLE,  lfo1.freq) + 1);
    osc1.freq = lfo1_out * lfo1_out * lfo1_out;
    y = waveCompute(&osc1, SINE_TABLE, osc1.freq);
    i2s_buffer[pos] = (uint16_t)(128 * (osc1.out + 1) * osc1.amp);
    //i2s_buffer[pos + 1] = (uint16_t)(128 * (osc1.out + 1) * osc1.amp);
    //i2s_buffer[pos] = pos;
  }
}

void make_sound_osc(){
  update_oscillator(&osc1);
}

void update_lfo1s(){

}

void note_on(uint8_t key, uint8_t vel){
  if (!test_on_note){
    test_on_note = key;
  }
}

void note_off(uint8_t key){
  test_on_note = 0;
}

void test_tone(){
  //play_note(pitchtbl[3], NULL);

}

void mixer(){
  make_sound_osc();
  synth_output();
  //update_lfo1s();
}
