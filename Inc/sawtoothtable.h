#ifndef __sawtoothtable_h
#define __sawtoothtable_h
#include <math.h>
#include "main.h"

#define COMPUTE_SAWTOOTHTABLE 1
#define SAWTOOTHTABLE_SIZE (1024 * 2)
extern float_t sawtoothtable[SAWTOOTHTABLE_SIZE];

#define _2PI  6.283185307f
#define SAWTOOTH_ALPHA (SAWTOOTHTABLE_SIZE/_2PI)

void sawtoothtable_init();

#endif
