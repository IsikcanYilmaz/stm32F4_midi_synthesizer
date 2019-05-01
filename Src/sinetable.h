/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __sinetable_h_
#define __sinetable_h_
/*-----------------------------------------------------------------------------*/

#include <math.h>
#include "main.h"

/*-------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------*/

#define COMPUTE_SINETABLE 0

#if COMPUTE_SINETABLE
#define SINETABLE_SIZE (1024 * 2)
extern float_t sinetable[SINETABLE_SIZE];
#else
#define SINETABLE_SIZE 1025
extern const  float_t sinetable[1025];
#endif

#define _2PI    		6.283185307f
#define ALPHA			(SINETABLE_SIZE/_2PI)


void sinetable_init();
/*----------------------------------------------------------------------------*/

#endif // __sinetable_h_

