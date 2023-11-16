/*
 * math2.h
 *
 *  Created on: Jun 4, 2019
 *      Author: trojan
 */

#ifndef PMSM_FOC_CONTROLMODULES_MATH2_H_
#define PMSM_FOC_CONTROLMODULES_MATH2_H_


#include "stdint.h"

/*---Look up table generation---*/
void fpu_tangent_lookup_table_generation();
void fpu_sin_cos_lookup_table_generation();

/*---Math Functions---*/
__RAM_FUNC void fpu_park_q31( int32_t Ialpha, int32_t Ibeta, int32_t * pId, int32_t * pIq,  int32_t delta);
__RAM_FUNC void fpu_cart2Polar(int32_t Vd, int32_t Vq, uint32_t *magnitude, int32_t *phase);
__RAM_FUNC void fpu_sin_cos_q31(int32_t angle, float* sinEstimated, float* cosEstimated);
__RAM_FUNC float fpu_sin_q31(int32_t angle);

/*---Settings---*/
//Arctangent
#define AT_VARIABLE_STEP DISABLED //+2bit precision
#define AT_ERROR_CORRECTION ENABLED //+2bit precision
//Sin-Cosin
#define SC_ERROR_CORRECTION DISABLED //+6bit precision

#endif /* PMSM_FOC_CONTROLMODULES_MATH2_H_ */
