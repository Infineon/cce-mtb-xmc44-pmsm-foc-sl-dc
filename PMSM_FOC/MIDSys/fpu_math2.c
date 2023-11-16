/*
 * math2.c
 *
 *  Created on: Jun 4, 2019
 *      Author: trojan
 */
#include "xmc_device.h"
#if UC_SERIES == XMC44
#include <xmc_common.h>
#include <PMSM_FOC/MIDSys/fpu_math2.h>
#include "math.h"
#include "xmc_gpio.h"

//Look up table
float tableAtan[300];
float tableSin[301];


__RAM_FUNC void fpu_park_q31(int32_t Ialpha, int32_t Ibeta, int32_t * pId, int32_t * pIq,  int32_t delta){
	/* Temporary variables used to store intermediate results */
	float sinVal,cosVal;
	fpu_sin_cos_q31(delta, &sinVal,&cosVal);
	/* Calculate pId by adding the two intermediate products 1 and 2 */
	*pId = Ialpha*cosVal+Ibeta*sinVal; //cast to int has intrinsical saturation
	/* Calculate pIq by subtracting the two intermediate products 3 from 4 */
	*pIq = Ibeta*cosVal-Ialpha*sinVal;
}

//Macros for arctangent
#define AT_X_SCALING 2
#define AT_X_OFFSET 0.675f
#define AT_Y_SCALING 500.467269f
#define AT_Y_OFFSET 156.6511975f
#if AT_VARIABLE_STEP==ENABLED
#define AT_LAST_LUT_TANGENT 5.02719008f
#define AT_CC_ERROR 0.000000261111f
#else
#define AT_LAST_LUT_TANGENT 4.5f
#define AT_CC_ERROR 0.000002973f
#endif
#define FPU_HALF_PI 1.57079632679f

#define FPU_PI 3.1415926535f

void fpu_tangent_lookup_table_generation(){
	double x;
	for(int i=0;i<300;i++){
		//Inverse of the map
		x=i;
		#if AT_VARIABLE_STEP==ENABLED
		x=(x+AT_Y_OFFSET)/AT_Y_SCALING;
		x=sqrtf(x/(1-x));
		x=(x-AT_X_OFFSET)*AT_X_SCALING;
		tableAtan[i]=atan(x)/FPU_PI;
		#else
		x=x/299*AT_LAST_LUT_TANGENT;
		tableAtan[i]=atan(x)/FPU_PI; //continuos component added to minimize the error
		#endif
		#if AT_ERROR_CORRECTION == 0
		tableAtan[i]+=AT_CC_ERROR; //continuos component added to minimize the error
		#endif

	}
}


__RAM_FUNC void fpu_cart2Polar(int32_t Vd, int32_t Vq, uint32_t *magnitude, int32_t *phase){
	float y, coef1, coef2, angleApprox, tan2, tan3, tan5, tan;
	#if AT_ERROR_CORRECTION == 1
	float error;
	#endif
	int32_t yI;
	//Magnitude
	//Cast to float because sqrt(long) is not available
	*magnitude = (int32_t)__builtin_sqrtf(((float)Vd*Vd)+((float)Vq*Vq));
	//Phase
	  //XMC_GPIO_SetOutputHigh(P3_0);
    tan=(float)Vq/Vd;
    if(tan<0){ //use odd property
    	tan=-tan;
    }
	y=tan; //y will be the float vector index after the application of the map
	if(y<AT_LAST_LUT_TANGENT){ //last tangent mapped in LUT
	   //map from tangent to index vector
	   #if AT_VARIABLE_STEP==ENABLED
	   y=(y/AT_X_SCALING+AT_X_OFFSET);
	   y=y*y;
	   y=(y/(1+y))*AT_Y_SCALING-AT_Y_OFFSET;
	   #else
	   y=y/AT_LAST_LUT_TANGENT*299;
	   #endif
	   yI=(int32_t)y;
	   coef1=y-yI;
	   coef2=1-coef1;
	   angleApprox=(tableAtan[yI]*coef2+tableAtan[yI+1]*coef1);
	   //Error correction - don't change the values
	   #if AT_ERROR_CORRECTION == 1
		#if AT_VARIABLE_STEP==ENABLED
	   if(tan<1.3091917149296f){
		  error=-0.0000014437f*tan*tan + 0.0000025789f*tan - 0.0000003306f;
	   }else{
		  error=-0.0000002736f*tan + 0.0000010797f;
	   }
	   #else
	   if(angleApprox<0.2548f){
		   error=-0.00020892f*angleApprox*angleApprox + 0.000071053f*angleApprox + 0.00000022513f;
	   }else{
		   error=0.000086184f*angleApprox*angleApprox-0.000084129f*angleApprox+0.000020389f;
	   }
	   #endif
	   angleApprox+=error*4*(-(coef1*coef1)+coef1);
	   #endif
	}else{
	   //Math approximation
	   tan2=tan*tan;
	   tan3=tan*tan2;
	   tan5=tan3*tan2;
	   angleApprox=(-1/tan+1/(3*tan3)-1/(5*tan5)+FPU_HALF_PI)/FPU_PI;
	   //Error correction - don't change the values
	   #if AT_ERROR_CORRECTION == 1
	   if(tan<7){
		   angleApprox-=-0.00000014809f*tan2+ 0.0000020005f*tan - 0.0000068263f;
	   }
	   #endif
	}
	*phase=(int32_t)(angleApprox*2147483648);
	//Collocation in the proper quadrant
    if(Vd<0){
		if(Vq>0){ //2nd quadrant
			*phase=2147483648-*phase;
		}else{ //3th quadrant
			*phase-=2147483648;
		}
    }else{
		if(Vq<0){ //4th quadrant
			*phase=-*phase;
		}
	}
   // XMC_GPIO_SetOutputLow(P3_0);
}


void fpu_sin_cos_lookup_table_generation(){
	float x;
	for(int i=0;i<301;i++){
		x=i-150;
		x=x/300*2*FPU_PI;
		tableSin[i]=sin(x);
	}
}

//Macros for sine and cosine
#define SC_ANGLE_TO_VECTOR_INDEX 0.000000069849193096160888671875f

void fpu_sin_cos_q31(int32_t angle, float* sinEstimated, float* cosEstimated){
	float x,coef1,coef2;
	#if SC_ERROR_CORRECTION == ENABLED
	float errorCoef;
	#endif
	int32_t index;
	x=(float)angle*SC_ANGLE_TO_VECTOR_INDEX;
	x=x+150;
	index=(int32_t)x;
	coef1=x-index;
	coef2=1-coef1;
	*sinEstimated=tableSin[index]*coef2+tableSin[index+1]*coef1;
	#if SC_ERROR_CORRECTION == ENABLED
		errorCoef=4*(-(coef1*coef1)+coef1)*0.000055f;
		*sinEstimated+=(*sinEstimated)*errorCoef;
	#endif
	index=(index+75)%300;
	*cosEstimated=tableSin[index]*coef2+tableSin[index+1]*coef1;
	#if SC_ERROR_CORRECTION == ENABLED
		*cosEstimated+=(*cosEstimated)*errorCoef;
	#endif
}

float fpu_sin_q31(int32_t angle){
	float x,coef1,coef2, sinEstimated;
	#if SC_ERROR_CORRECTION == ENABLED
	float errorCoef;
	#endif
	int32_t index;
	x=(float)angle*SC_ANGLE_TO_VECTOR_INDEX;
	x=x+150;
	index=(int32_t)x;
	coef1=x-index;
	coef2=1-coef1;
	sinEstimated=tableSin[index]*coef2+tableSin[index+1]*coef1;
	#if SC_ERROR_CORRECTION == ENABLED
		errorCoef=4*(-(coef1*coef1)+coef1)*0.000055f;
		sinEstimated+=(sinEstimated)*errorCoef;
	#endif
	return sinEstimated;
}



#endif
