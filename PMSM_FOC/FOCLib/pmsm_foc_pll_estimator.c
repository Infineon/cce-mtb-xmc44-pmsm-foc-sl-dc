/**
 * @file pmsm_foc_pll_estimator.c
 * @Firmware PMSM_FOC_SL_XMC13_XMC14_V1_5
 * @Modified date: 2016-09-30
 *
 * @cond
 ****************************************
 * PMSM FOC Motor Control Library
 *
 * Copyright (c) 2015-2016, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this list of conditions and the  following
 *   disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *   following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes
 * with Infineon Technologies AG (dave@infineon.com).
 ******************************************
 *
 * @file pmsm_foc_pll_estimator.c
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * @endcond
 *
 */
/*********************************************************************************************************************
 *
 ***************************************/



#include "pmsm_foc_pll_estimator.h"


#define CORDIC_VECTORING_MODE		(0x62)			// CORDIC: Circular Vectoring Mode (default). MPS: Divide by 2 (default).
#define CORDIC_ROTATION_MODE		(0x6A)			// CORDIC: Circular Rotation Mode. MPS: Divide by 2 (default).

//extern CurrentType Current;											// Motor current and current space vector.
//extern ClarkeTransformType Clarke_Transform;
//extern ParkTransformType Park_Transform;
PLL_EstimatorType PLL_Estimator;
//
//extern FOCInputType FOCInput;							// Parameters input for FOC LIB.
//extern FOCOutputType FOCOutput;						// Output for FOC LIB.
//
//extern ADCType ADC;                          // ADC results, trigger positions.
//extern MotorControlType Motor;                      // Motor control information
//extern SVMType SVM;                          // SVM information, such as sector 0 ~ 5 (A ~ F) in SVM space vector hexagon.
//extern PI_Coefs_Type PI_Speed;                  // Speed PI controller.
//extern PI_Coefs_Type PI_Torque;                 // Torque / Iq PI controller.
//extern PI_Coefs_Type PI_Flux;                 // Flux /Id PI controller.
//extern uint32_t * NEW_SHS0_CALOC1;
//extern Car2PolType Car2Polar;

uint32_t Current_I_Mag;								// |I|, magnitude of current space vector
int32_t Delta_IV;
int32_t VrefxSinDelta;
extern FOCInputType FOCInput;							// Parameters input for FOC LIB.
extern PI_Coefs_Type PI_PLL;									// PLL rotor speed PI controller.
/**
 * @brief	I_Mag (i.e.: |I|) and angle Î³-Î¸ for PLL Observer
 * 			I_Mag = K/MPS * sqrt(I_Alpha^2+I_Beta^2)		* Xfinal = K/MPS * sqrt(X^2+Y^2), where K = 1.646760258121.
 * 			γ = atan(I_Beta/I_Alpha)						* Zfinal = Z + atan(Y/X)					(Yfinal = 0).
 *
 * @param	Current.I_Beta_1Q31
 * 			Current.I_Alpha_1Q31
 * 			FOCInput.Vref_AngleQ31
 *
 *@retval	FOCOutput.I_q
 * 			FOCOutput.I_d
 */
__RAM_FUNC void PLL_Imag(int32_t Vref_AngleQ31, int32_t I_Alpha_1Q31, int32_t I_Beta_1Q31, PLL_EstimatorType* const HandlePtr)
{
#if(UC_FAMILY == XMC1)
	/*  General control of CORDIC Control Register */
	MATH->CON = CORDIC_VECTORING_MODE;

	/* Z = -θ¸ of last PWM cycle, to get γ-θ¸ directly by CORDIC */
	MATH->CORDZ = -Vref_AngleQ31;

	/* Y = I_Beta */
	MATH->CORDY = I_Beta_1Q31;

	/* X = I_Alpha. Input CORDX data, and auto start of CORDIC calculation */
	MATH->CORDX = I_Alpha_1Q31;
#else
	fpu_cart2Polar(I_Alpha_1Q31, I_Beta_1Q31, &(HandlePtr->Current_I_Mag), &(HandlePtr->Delta_IV));
	HandlePtr->Delta_IV-=Vref_AngleQ31;
#endif
}


/**
 * @brief	 Results of CORDIC #2 - |I| and γ-θ
 *
 * @param	MATH2->CORRX
 * 			MATH2->CORRZ
 *
 *@retval	Current_I_Mag
 *			Delta_IV
 */
__RAM_FUNC void PLL_Imag_GetResult(PLL_EstimatorType* const HandlePtr)
{
#if(UC_FAMILY == XMC1)
	while (MATH->STATC & 0x01)
	{
	   continue;
	}
	/* Read CORDIC result: |I|- 32-bit unsigned */
	HandlePtr->Current_I_Mag = MATH->CORRX;

	/* γ-θ¸ by CORDIC directly */
	HandlePtr->Delta_IV = MATH->CORRZ;
#endif
}


/**
 * @brief	Vref x sin(γ-θ) for PLL Observer
 *			Vref x cos(γ-θ)=K[|Vref| cos(γ-θ)- 0 X sin(γ-θ)]/MPS		* Xfinal = K[X cos(Z) - Y sin(Z)] / MPS, where K = 1.646760258121.
 * 			Vref x sin(γ-θ)=K[0 X cos(γ-θ) + |Vref|sin(γ-θ)]/MPS		* Yfinal = K[Y cos(Z) + X sin(Z)] / MPS. Select Y= 0. (Zfinal = 0).
 *
 * @param	Delta_IV
 * 			FOCInput.Vref32
 *
 *@retval	MATH2->CORDZ
 *			MATH2->CORDX
 *			MATH2->CORDY
 */
__RAM_FUNC void PLL_Vref (int32_t Delta_IV, uint32_t Vref32, int32_t PLL_Uk, int32_t Phase_L,PLL_EstimatorType* const HandlePtr)
{
#if(UC_FAMILY == XMC1)
	/* General control of CORDIC Control Register */
	MATH->CON = CORDIC_ROTATION_MODE;

	/* Z = Angle difference of I and Vref */
	MATH->CORDZ = Delta_IV;

	/* Y = zero */
	MATH->CORDY = 0;

	/* X = |Vref| of last PWM cycle. Input CORDX data, and auto start of CORDIC calculation */
	MATH->CORDX = Vref32;

	HandlePtr->Current_I_Mag >>= CORDIC_SHIFT;				// Shift to get real result |I| (1Q15) of last CORDIC calculation.
	HandlePtr->Current_I_Mag = (HandlePtr->Current_I_Mag * 311) >> 8; // x MPS/K.
#else
	//fpu_park_q31(0, Vref32, &(HandlePtr->VrefxSinDelta), &(HandlePtr->VrefxCosDelta), Delta_IV);
	HandlePtr->VrefxSinDelta=Vref32*fpu_sin_q31(Delta_IV);

#endif
	FOCInput.BEMF2 = (FOCInput.BEMF2* ((1<<FOCInput.LPF_N_BEMF)-1) + ((((PLL_Uk >> RES_INC) * (uint32_t)(FOCInput.Phase_L)) * HandlePtr->Current_I_Mag)>>FOCInput.Phase_L_Scale))>>SHIFT_MET_PLL;	// wLI
}


/**
 * @brief	Results of CORDIC #3 - Vrefxsin(γ-θ) and Vrefxcos(γ-θ)
 *
 *@param	MATH2->CORRY
 *
 *@retval	VrefxSinDelta
 */
__RAM_FUNC void PLL_Vref_GetResult(PLL_EstimatorType* const HandlePtr)
{
#if(UC_FAMILY == XMC1)
	while (MATH->STATC & 0x01)
	{
	   continue;
	}
	/*  Read CORDIC result |Vref|sin(γ-θ) - 32-bit. FOC doesn't need above information of |Vref|sin(γ-θ) */
	HandlePtr->VrefxSinDelta = MATH->CORRY;

	/*  Read CORDIC result |Vref|cost(γ-θ) - 32-bit. FOC doesn't need above information of |Vref|cos(γ-θ) */
	HandlePtr->VrefxCosDelta = MATH->CORRX;
#endif
}


/**
 * @brief	Results of CORDIC #3 - Vrefxsin(γ-θ) and Vrefxcos(γ-θ)
 *
 *@param
 *
 *@retval	FOCInput.RotorSpeed_In
 *			FOCInput.RotorAngleQ31
 */
#define LPF_FACTOR_PLL		1
__RAM_FUNC void PLL_GetPosSpd(PLL_EstimatorType* const HandlePtr)
{
#if(UC_FAMILY == XMC1)
	HandlePtr->VrefxSinDelta >>= CORDIC_SHIFT;							// Shift to get real result (16-bit).
	HandlePtr->VrefxCosDelta >>= CORDIC_SHIFT;
	// Shift to get real result (16-bit).
	HandlePtr->VrefxSinDelta = (HandlePtr->VrefxSinDelta * 311)>>8;
	HandlePtr->VrefxCosDelta = (HandlePtr->VrefxCosDelta * 311)>>8;
#endif

	FOCInput.BEMF1 = (FOCInput.BEMF1 * ((1<<SHIFT_MET_PLL)-1) + HandlePtr->VrefxSinDelta) >> SHIFT_MET_PLL;		// |Vref|sin(Î³-Î¸) with LPF.

	/*&&&& PI Controller #4 - PLL Observer PI controller &&&&*/
	pmsm_foc_pi_controller(0, (FOCInput.BEMF2 + FOCInput.BEMF1), &PI_PLL);
	HandlePtr->RotorAngleQ31 += (PI_PLL.Uk << (16U - RES_INC));			// Estimate latest rotor angle (1Q31). φ[k] = φ[k-1] + ωr[k].


	HandlePtr->RotorSpeed_In = (HandlePtr->RotorSpeed_In * ((1U << LPF_FACTOR_PLL) - 1U) + PI_PLL.Uk) >> LPF_FACTOR_PLL;	// PLL Observer rotor speed with LPF.

}






































































