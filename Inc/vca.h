
#include "main.h"

#define MIDI_MAX 256.f // floating max value
#define VCA_TEST 0
#define VCA_VCA_OFF 0
#define VCA_VCA_OFF_AMPLITUDE 1
#define VCA_UPPER_LIMIT 10 // seconds
/*
 *    
 *   /\___
 *  /     \
 *  A D S R
 */

typedef enum VCAState{
  // VCA_ATTACK is the time it takes to go to amplitude value 1 from 0
  VCA_ATTACK = 0,
  // VCA_DECAY is the time it takes to go to the VCA_SUSTAIN amplitude from as high as we got during VCA_ATTACK (<=1)
  VCA_DECAY,
  // VCA_SUSTAIN is the LEVEL of amplitude to sustain after VCA_DECAY is complete WHILE note is on
  VCA_SUSTAIN,
  // VCA_RELEASE is the time it takes to go to 0 amplitude from VCA_SUSTAIN level of amplitude after note off
  VCA_RELEASE,
  // VCA_DONE. VCA module is idle
  VCA_DONE,
  // VCA_OFF. module will only output 1.0
  VCA_OFF
}VCAState_e;

typedef struct VCA{
  VCAState_e state;
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

  float attackRate; // denotes increment value per sample assuming vca update is called per sample
  float decayRate;
  float releaseRate;

  float decayAmpRate;
  float releaseAmpRate;

  uint32_t noteOnNum;

} VCA_t;

void vca_set_attack(VCA_t *vca, uint16_t atk);
void vca_set_decay(VCA_t *vca, uint16_t dec);
void vca_set_sustain(VCA_t *vca, uint16_t sus);
void vca_set_release(VCA_t *vca, uint16_t rel);

void vca_excite(VCA_t *vca, uint8_t key);
void vca_release(VCA_t *vca);
