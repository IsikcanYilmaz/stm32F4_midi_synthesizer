
#include "tim.h"
#include "synth.h"
#include "i2s.h"
#include "dac.h"
#include <math.h>

#define PI 3.14159265358979323846
#define TAU (2.0 * PI)

uint16_t counters[NUM_OSCILLATORS], output;
const uint32_t pitchtbl[] = {16384,
   15464,14596,13777,13004,12274,11585,10935,10321,9742,9195,8679,
   8192,7732,7298,6889,6502,6137,5793,5468,5161,4871,4598,4340,
   4096,3866,3649,3444,3251,3069,2896,2734,2580,2435,2299,2170,
   2048,1933,1825,1722,1625,1534,1448,1367,1290,1218,1149,1085,
   1024,967,912,861,813,767,724,683,645,609,575,542,
   512,483,456,431,406,384,362,342,323,304,287,271,
   256,242,228,215,203,192,181,171,161,152,144,136,
   128,121,114,108 ,102,96,91,85,81,76,72,68,64};

int16_t signal[46876];
int nsamples;

void synth_init(){
  volatile int sizeofsignal = sizeof(signal);
  volatile int sizeofsignal0 = sizeof(signal[0]);
  nsamples = sizeof(signal) / sizeof(signal[0]);
  int i = 0;
  while (i < nsamples){
    double t = ((double) i / 2.0) / ((double) nsamples);
    signal[i] = sin(100.0 * TAU * t);
    signal[i + 1] = signal[i]; // right
    i += 2;
  }

}

void play_note(uint8_t note, uint8_t velocity){
  TIM1_Config(pitchtbl[note]);
}

void synth_output(){
  uint16_t output;
  if (counters[0] % 2){
    output = 0xffff;
  } else {
    output = 0x0000;
  }
  //HAL_I2S_Transmit(&hi2s3, &output, 1, 1000);
  HAL_StatusTypeDef res = HAL_I2S_Transmit(&hi2s3, (uint16_t*)&output, 1, HAL_MAX_DELAY);
  HAL_StatusTypeDef dac_res = HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, output);
  if (res != HAL_OK || dac_res != HAL_OK){
    while(1){
      int error = 0;
      error++;
    }
  }
}

void make_sound(){
  counters[0] = __HAL_TIM_GetCounter(&htim1);
  counters[1] = __HAL_TIM_GetCounter(&htim2);
  counters[2] = __HAL_TIM_GetCounter(&htim3);
  counters[3] = __HAL_TIM_GetCounter(&htim4);
  counters[4] = __HAL_TIM_GetCounter(&htim5);
}

void update_lfos(){

}

void note_on(uint8_t key, uint8_t vel){
  uint32_t note = pitchtbl[key];
  play_note(key + 8, vel);
  //test_tone();
}

void note_off(uint8_t key){
  synth_init();
  
}

void test_tone(){
  play_note(pitchtbl[3], NULL);

}

void mixer(){
  make_sound();
  synth_output();
  //update_lfos();
}
