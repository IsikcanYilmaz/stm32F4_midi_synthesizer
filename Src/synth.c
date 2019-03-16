
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
const uint32_t pitchtbl[] = {16384,
  15464,14596,13777,13004,12274,11585,10935,10321,9742,9195,8679,
  8192,7732,7298,6889,6502,6137,5793,5468,5161,4871,4598,4340,
  4096,3866,3649,3444,3251,3069,2896,2734,2580,2435,2299,2170,
  2048,1933,1825,1722,1625,1534,1448,1367,1290,1218,1149,1085,
  1024,967,912,861,813,767,724,683,645,609,575,542,
  512,483,456,431,406,384,362,342,323,304,287,271,
  256,242,228,215,203,192,181,171,161,152,144,136,
  128,121,114,108 ,102,96,91,85,81,76,72,68,64};

int16_t signal[256];
int nsamples;

uint8_t test_on_note = 0;

Oscillator_t osc1;

void synth_init(){
  osc1.amp = 0.4f;
  osc1.last_amp = 0.9f;
  osc1.freq = 1;
  osc1.phase = 0;
  osc1.out = 0;
  osc1.modInd = 0;
  osc1.mul = 1;

  uint16_t j;

  // init i2s_buffer. 
  for (j = 0; j < (BUF_SIZE); j+=2){
    i2s_buffer[j] = (uint16_t)(1024 * (osc1.out + 1) * osc1.amp);
    i2s_buffer[j + 1] = (uint16_t)(1024 * (osc1.out + 1) * osc1.amp);
    update_oscillator(&osc1);
  }

  //hdma_spi3_tx.XferHalfCpltCallback = test_dma_half_transfer1;
  //hdma_spi3_tx.XferCpltCallback = test_dma_half_transfer2;
  //HAL_TIM_Base_Start_IT(&htim14);
  //synth_output();
  HAL_I2S_Transmit_DMA(&hi2s3, &i2s_buffer, BUF_SIZE);
}

void test_dma_half_transfer1(){

}

void test_dma_half_transfer2(){

}

void test_synth_output(){
  static uint32_t cursor = 0;
  if (hi2s3.State != HAL_I2S_STATE_BUSY_TX){
    volatile HAL_StatusTypeDef res = HAL_I2S_Transmit_DMA(&hi2s3, &i2s_buffer[0], BUF_SIZE);
    cursor += BUF_SIZE;
    if (cursor >= BUF_SIZE){
      cursor = 0;
    }
  }
}

void test_synth_output_2ndpart(){

}

void erase_i2s_buffer(){
  for (int i = 0; i < BUF_SIZE; i++){
    i2s_buffer[i] = 0;
  }
}

void test_bump_pitch(bool up){
  static int test_midi_num = 61 - 24;
  /*osc1.mul += MIDI_TO_FREQ(test_midi_num);
  test_midi_num++;*/
  if (up){
    osc1.freq += 12; 
  } else {
    osc1.freq -= 12;
  }
  /*for (int j = 0; j < (BUF_SIZE); j+=2){
    i2s_buffer[j] = (uint16_t)(1024 * (osc1.out + 1) * osc1.amp);
    i2s_buffer[j + 1] = (uint16_t)(1024 * (osc1.out + 1) * osc1.amp);
    update_oscillator(&osc1);
  }*/
  if (test_midi_num > 61 - 24 + 12){
    test_midi_num = 61;
  }

}

void play_note(uint8_t note, uint8_t velocity){
  TIM1_Config(pitchtbl[note]);
}

static volatile uint16_t out_test[2];

void synth_output(){

}

// fill buf[0]-buf[length] with outputs of the oscillator. 
void make_sound(uint16_t *buf, uint16_t length){
  static uint8_t ch = TIM_CHANNEL_1;
  static bool pressed = false;
  static int16_t last_led_speed;
  if (last_led_speed < led_speed /*|| led_speed == 500*/){
    test_bump_pitch(false);
    for (int j = 0; j < length; j+=2){
      buf[j] = (uint16_t)(1024 * (osc1.out + 1) * osc1.amp);
      buf[j + 1] = (uint16_t)(1024 * (osc1.out + 1) * osc1.amp);
      update_oscillator(&osc1);
    }
  } else if (last_led_speed > led_speed) {
    test_bump_pitch(true);
    for (int j = 0; j < length; j+=2){
      buf[j] = (uint16_t)(1024 * (osc1.out + 1) * osc1.amp);
      buf[j + 1] = (uint16_t)(1024 * (osc1.out + 1) * osc1.amp);
      update_oscillator(&osc1);
    }
  }
  last_led_speed = led_speed;
  //__HAL_TIM_SET_COMPARE(&htim4, ch, 9990);
  if (!HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)){
    //erase_i2s_buffer();
    pressed = true;
  } else {
    if (pressed){
      //test_bump_pitch();
      pressed = false;
    }
  }
  //__HAL_TIM_SET_COMPARE(&htim4, ch , 0);
}

void make_sound_osc(){
  update_oscillator(&osc1);
}

void update_lfos(){

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
  play_note(pitchtbl[3], NULL);

}

void mixer(){
  make_sound_osc();
  synth_output();
  //update_lfos();
}
