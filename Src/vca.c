
#include "vca.h"
#include "synth.h"


vca_init(VCA_t *vca){
  vca->state = VCA_DONE;
  vca->phase = 0;
  vca->key = 0;
}

vca_excite(VCA_t *vca, uint8_t key){
  vca->state = VCA_ATTACK;
  vca->key = key;
}

vca_release(VCA_t *vca){
  vca->state = VCA_RELEASE;
}

vca_update(VCA_t *vca){
  if (vca->state == VCA_DONE){
    return;
  }
#if VCA_VCA_OFF
  switch(vca->state){
    case VCA_ATTACK:
    case VCA_DECAY:
    case VCA_SUSTAIN:
      vca->amp = VCA_VCA_OFF_AMPLITUDE;
      break;
    case VCA_RELEASE:
      vca->amp = 0;
      vca->state++;
    case VCA_DONE:
      break;
    case VCA_OFF:
      break;
    default:
      break;
  }

#else 
  switch(vca->state){
    case VCA_ATTACK:
      vca->amp += vca->attackRate;
      if (vca->amp >= 1.0){
        vca->state = VCA_DECAY;
        vca->amp = 1.0;
        vca->phase = 0;
      }
      break;
    case VCA_DECAY:
      vca->phase = 0;
      vca->amp -= vca->decayRate;
      if (vca->amp <= vca->sustainVal){
        vca->state = VCA_SUSTAIN;
        vca->amp = vca->sustainVal;
        vca->phase = 0;
      }
      break;
    case VCA_SUSTAIN:
      vca->amp = vca->sustainVal;
      break;
    case VCA_RELEASE:
      vca->phase = 0;
      vca->amp -= vca->releaseRate;
      if (vca->amp <= 0){
        vca->state = VCA_DONE;
        vca->amp = 0;
      }
      break;
    case VCA_DONE:
      break;
    case VCA_OFF:
      break;
  }
#endif
}

void vca_set_attack(VCA_t *vca, uint16_t atk){
  vca->attackVal = atk;
  vca->attackRate = MIDI_MAX / (VCA_UPPER_LIMIT * SAMPLERATE * atk);
}
void vca_set_decay(VCA_t *vca, uint16_t dec){
  vca->decayVal = dec;
  vca->decayRate = (1 - vca->sustainVal) * MIDI_MAX / (VCA_UPPER_LIMIT * SAMPLERATE * dec);
}
void vca_set_sustain(VCA_t *vca, uint16_t sus){
  vca->sustainVal = sus / MIDI_MAX;
  vca_set_decay(vca, vca->decayVal);
  vca_set_release(vca, vca->releaseVal);
}
void vca_set_release(VCA_t *vca, uint16_t rel){
  vca->releaseVal = rel;
  vca->releaseRate = vca->sustainVal * MIDI_MAX / (VCA_UPPER_LIMIT * SAMPLERATE * rel);
}
