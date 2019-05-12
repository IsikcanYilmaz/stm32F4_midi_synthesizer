
#include "adsr.h"
#include "synth.h"


adsr_init(ADSR_t *adsr){
  adsr->state = ADSR_DONE;
  adsr->phase = 0;
  adsr->key = 0;
}

adsr_excite(ADSR_t *adsr, uint8_t key){
  adsr->state = ADSR_ATTACK;
  adsr->phase = 0;
  adsr->key = key;
  adsr->freq = adsr->values[ADSR_ATTACK];
  //adsr->amp = ADSR_ADSR_OFF_AMPLITUDE;
  //adsr->amp = 0;
}

adsr_release(ADSR_t *adsr){
  adsr->state = ADSR_RELEASE;
}

adsr_update(ADSR_t *adsr){
  if (adsr->state == ADSR_DONE){
    return;
  }
#if ADSR_ADSR_OFF
  switch(adsr->state){
    case ADSR_ATTACK:
    case ADSR_DECAY:
    case ADSR_SUSTAIN:
      adsr->amp = ADSR_ADSR_OFF_AMPLITUDE;
      break;
    case ADSR_RELEASE:
      adsr->amp = 0;
      adsr->state++;
    case ADSR_DONE:
      break;
    case ADSR_OFF:
      break;
    default:
      break;
  }

#else 
  switch(adsr->state){
    case ADSR_ATTACK:
      adsr->amp += adsr->attackRate;
      if (adsr->amp >= 1.0){
        adsr->state = ADSR_DECAY;
        adsr->amp = 1.0;
        adsr->phase = 0;
      }
      break;
    case ADSR_DECAY:
      adsr->phase = 0;
      adsr->amp -= adsr->decayRate;
      if (adsr->amp <= adsr->sustainVal){
        adsr->state = ADSR_SUSTAIN;
        adsr->amp = adsr->sustainVal;
        adsr->phase = 0;
      }
      break;
    case ADSR_SUSTAIN:
      adsr->amp = adsr->sustainVal;
      break;
    case ADSR_RELEASE:
      adsr->phase = 0;
      adsr->amp -= adsr->releaseRate;
      if (adsr->amp <= 0){
        adsr->state = ADSR_DONE;
        adsr->amp = 0;
      }
      break;
    case ADSR_DONE:
      break;
    case ADSR_OFF:
      break;
  }
#endif
}

void adsr_set_attack(ADSR_t *adsr, uint16_t atk){
  adsr->attackVal = atk;
  adsr->attackRate = MIDI_MAX / (ADSR_UPPER_LIMIT * SAMPLERATE * atk);
  //adsr->attackTime = ADSR_UPPER_LIMIT - (ADSR_UPPER_LIMIT * (MIDI_MAX - atk) / MIDI_MAX);
  //adsr->attackRate = 1 / (adsr->attackTime * SAMPLERATE);
}
void adsr_set_decay(ADSR_t *adsr, uint16_t dec){
  adsr->decayVal = dec;
  //adsr->decayTime = ADSR_UPPER_LIMIT - (ADSR_UPPER_LIMIT * (MIDI_MAX - dec) / MIDI_MAX);
  adsr->decayRate = (1 - adsr->sustainVal) * MIDI_MAX / (ADSR_UPPER_LIMIT * SAMPLERATE * dec);
}
void adsr_set_sustain(ADSR_t *adsr, uint16_t sus){
  adsr->sustainVal = sus / MIDI_MAX;
  adsr_set_decay(adsr, adsr->decayVal);
  adsr_set_release(adsr, adsr->releaseVal);
}
void adsr_set_release(ADSR_t *adsr, uint16_t rel){
  adsr->releaseVal = rel;
  //adsr->releaseTime = ADSR_UPPER_LIMIT - (ADSR_UPPER_LIMIT * (MIDI_MAX - rel) / MIDI_MAX);
  adsr->releaseRate = adsr->sustainVal * MIDI_MAX / (ADSR_UPPER_LIMIT * SAMPLERATE * rel);
}
