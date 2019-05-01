/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __sinetable_h_
#define __sinetable_h_
/*-----------------------------------------------------------------------------*/

#include <math.h>
#include "main.h"

/*-------------------------------------------------------------------------------*/

#define SINETABLE_SIZE 1024 * 2
#define _2PI    		6.283185307f
#define ALPHA			(SINETABLE_SIZE/_2PI)

/*-------------------------------------------------------------------------------*/

#define COMPUTE_SINETABLE 1

#if COMPUTE_SINETABLE
extern float_t sinetable[SINETABLE_SIZE];
#else
extern const  float_t sinetable[1025];
#endif

void sinetable_init();
/*----------------------------------------------------------------------------*/

#endif // __sinetable_h_

