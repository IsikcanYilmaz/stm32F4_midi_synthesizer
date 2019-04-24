
#include "main.h"

#define MIDI_MAX 256.f // floating max value
#define ADSR_TEST 1
#define ADSR_OFF 0
#define ADSR_OFF_AMPLITUDE 1
#define ADSR_UPPER_LIMIT 10 // seconds
/*
 *    
 *   /\___
 *  /     \
 *  A D S R
 */

typedef enum ADSRState{
  // ATTACK is the time it takes to go to amplitude value 1 from 0
  ATTACK = 0,
  // DECAY is the time it takes to go to the SUSTAIN amplitude from as high as we got during ATTACK (<=1)
  DECAY,
  // SUSTAIN is the LEVEL of amplitude to sustain after DECAY is complete WHILE note is on
  SUSTAIN,
  // RELEASE is the time it takes to go to 0 amplitude from SUSTAIN level of amplitude after note off
  RELEASE,
  // DONE. ADSR module is idle
  DONE,
  // OFF. module will only output 1.0
  OFF
}ADSRState_e;

typedef struct ADSR{
  ADSRState_e state;
  uint8_t key;

  float ampCeiling; // this is the amplitude   
  float amp; // This is the value that will get written to the amp value of our oscillator structure
  float freq; // frequency that describes how fast the phase value goes from 0 to 1
  float phase; // this is the value that goes from 0 to 1. 1 is where we switch from A to D, and from R to Done
  float values[4]; // atk, dcy, sus, rel

  float attackVal;
  float decayVal;
  float sustainVal;
  float releaseVal;

  uint16_t attackRawVal;
  uint16_t decayRawVal;
  uint16_t sustainRawVal;
  uint16_t releaseRawVal;

  float attackTime; // denotes time to get from phase = 0 to phase = 1;
  float decayTime;
  float releaseTime; 

  float attackRate; // denotes increment value per sample assuming adsr update is called per sample
  float decayRate;
  float releaseRate;

  float decayAmpRate;
  float releaseAmpRate;

} ADSR_t;

void adsr_set_attack(ADSR_t *adsr, uint16_t atk);
void adsr_set_decay(ADSR_t *adsr, uint16_t dec);
void adsr_set_sustain(ADSR_t *adsr, uint16_t sus);
void adsr_set_release(ADSR_t *adsr, uint16_t rel);

void adsr_excite(ADSR_t *adsr, uint8_t key);
