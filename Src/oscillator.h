
#include "main.h"

typedef struct Oscillator {
  float amp; // should be <= 1 for normal sounding output
  float last_amp;
  float freq; // hertz (sample freq)
  float phase; // radians
  float phi_0; // radians
  float modInd; // modulation index for fm
  float mul; // pitch frequency multiplier
  float out; // output sample
} Oscillator_t;

float update_oscillator(Oscillator_t *osc);
