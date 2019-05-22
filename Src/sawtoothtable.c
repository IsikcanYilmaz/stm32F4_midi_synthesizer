
#include "sawtoothtable.h"

#if COMPUTE_SAWTOOTHTABLE
float_t sawtoothtable[SAWTOOTHTABLE_SIZE];
void sawtoothtable_init(){
  float timeDifference = 1.0/SAWTOOTHTABLE_SIZE;
  for (int i = 0; i < SAWTOOTHTABLE_SIZE; i++){
    sawtoothtable[i] = i * timeDifference;
  }
}
#else

#endif
