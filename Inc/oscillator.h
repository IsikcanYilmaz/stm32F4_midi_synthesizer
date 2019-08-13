
#include "main.h"

typedef struct Oscillator {
  float amp; // should be <= 1 for normal sounding output
  float last_amp;
  float noteFreq; // hertz (sample freq)
  float freq; // hertz (sample freq) (this gets used in the calculations)
  float phase; // radians
  float phi_0; // radians
  float modInd; // modulation index for fm
  float mul; // pitch frequency multiplier
  float out; // output sample
  float increment; // increment this much every update
} Oscillator_t;

enum Timbre {
  SINE = 0,
  SAWTOOTH,
  SQUARE,
  SAWTOOTH_TABLE,
  SINE_TABLE, 
  TRIANGLE_TABLE,
  TEST_PD_TABLE
};

float oscillatorSine(Oscillator_t *osc);
float oscillatorSineTable(Oscillator_t *osc);
float oscillatorSquare(Oscillator_t *osc);
float oscillatorSawtooth(Oscillator_t *osc);
float oscillatorSawtoothTable(Oscillator_t *osc);
float oscillatorTestPdTable(Oscillator_t *osc);

float waveCompute(Oscillator_t *osc, enum Timbre sound, float freq);


