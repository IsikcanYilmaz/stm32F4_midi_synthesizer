
#include "oscillator.h"
#include "sinetable.h"
#include "sawtoothtable.h"
//#include "test_pd_array_wavetable.h"
#include "tim.h"
#include "math.h"

////////////////////////
// credit to Dekrispator_v2
#define AMP_MULTIPLIER 0.8

// This function is to be called per sample of a wave. It will 
// update the phase of the oscillator as well as returning the 
// output value of it, which is between 0 and 1.
float waveCompute(Oscillator_t *osc, enum Timbre sound, float freq){

  float y;
  osc->freq = freq;
  switch(sound){
    case SINE: // accurate sine
      y = AMP_MULTIPLIER * oscillatorSine(osc);
      break;

    case SINE_TABLE: // sine from hardcoded sine wavetable
      y = AMP_MULTIPLIER * oscillatorSineTable(osc);
      break;

    case SAWTOOTH_TABLE:
      y = AMP_MULTIPLIER * oscillatorSawtoothTable(osc);
      break;

    case TEST_PD_TABLE:
      y = AMP_MULTIPLIER * oscillatorTestPdTable(osc);
      break;

    default:
      y = 0;
      break;
  }
  return y;
}

float oscillatorTriangleTable(Oscillator_t *osc){
  return 0;
}

float oscillatorTestPdTable(Oscillator_t *osc){ // Table sine
  while (osc->phase < 0){ // keep phase in [0, 2pi] // why tho?
    osc->phase += _2PI;
  } 
  while (osc->phase >= _2PI){
    osc->phase -= _2PI;
  }
  //int index = (int) round(ALPHA * osc->phase);
  //osc->out = osc->amp * test_pd_array_wavetable[index];
  osc->phase += _2PI * Ts * osc->freq; // increment phase
  return osc->out;
}

float oscillatorSawtoothTable(Oscillator_t *osc){
  while (osc->phase < 0){ // keep phase in [0, 2pi]
    osc->phase += _2PI;
  }
  while (osc->phase >= _2PI){
    osc->phase -= _2PI;
  }
  osc->out = osc->amp * sawtoothtable[(int)round(SAWTOOTH_ALPHA * osc->phase)];
  osc->phase += _2PI * Ts * osc->freq; // increment phase
  return osc->out;
} // TODO This could be done more elegantly. i.e. shorten the code

float oscillatorSineTable(Oscillator_t *osc){ // Table sine
  while (osc->phase < 0){ // keep phase in [0, 2pi] // why tho?
    osc->phase += _2PI;
  } 
  while (osc->phase >= _2PI){
    osc->phase -= _2PI;
  }
  osc->out = osc->amp * sinetable[(int)round(SINETABLE_ALPHA * osc->phase)];
  osc->phase += _2PI * Ts * osc->freq; // increment phase
  return osc->out;
}

float oscillatorSine(Oscillator_t *osc){ // Accurate sine
  float z;
  while(osc->phase >= _2PI){ // keep phase in [0, 2pi]
    osc->phase -= _2PI;
  }
  z = sin(osc->phase);
  osc->out = osc->amp * z;
  osc->phase += osc->mul * _2PI * Ts * osc->freq; // increment phase
  return osc->out;
}

float oscillatorSquare(Oscillator_t *osc){
  return 0;
}
float oscillatorSawtooth(Oscillator_t *osc){
  return 0;
}
