
#include "oscillator.h"
#include "tim.h"

float update_oscillator(Oscillator_t *osc){
  static float min = 0, max = 0;
  static uint16_t min16, max16;
  /* Sine
  */
  float z;

  while (osc->phase >= _2PI)
    osc->phase -= _2PI;

  z = sin(osc->phase);
  osc->out = osc->amp * z;

  osc->phase += osc->mul * _2PI * Ts * osc->freq; // increment phase
  //osc->phase +=  _2PI * Ts * osc->freq; // increment phase
  //__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3 , (uint16_t) (1000 * (osc->out + 0.5)));
  return osc->out;
}

float update_oscillator_sawtooth(Oscillator_t *osc){
  osc->out += osc->increment;
  return osc->out;
}
