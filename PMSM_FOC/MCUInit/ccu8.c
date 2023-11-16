/**
 * @file ccu8.c
 * @Firmware PMSM_FOC_SL_XMC13_XMC14_V1_5
 * @Modified date: 2019-01-10
 *
 * @cond
 ****************************************
 * PMSM FOC Motor Control Library
 *
 * Copyright (c) 2015-2019, Infineon Technologies AG
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
 * @file ccu8.c
 *
 * @brief ACMP low level driver API prototype definition for XMC1 <br>
 *
 *
 * APIs provided in this file mainly cover the following functionality:
 * ---- Filter, Hysterisis, Output inversion
 *
 *
 *
 * 16 Dec 2015 Version 1.0.0 <br>:
 *      Initial version
 * @endcond
 *
 */

/*********************************************************************************************************************
 * HEADER FILES
 ***************************************/

#include <PMSM_FOC/MCUInit/ccu8.h>
#include <xmc_common.h>

/*********************************************************************************************************************
 * DATA STRUCTURES
 ***************************************/
/**
 *  Data Structure initialization - CC8 Slices configuration for all three pwm phase.
 */
XMC_CCU8_SLICE_COMPARE_CONFIG_t CCU8_Phase_U_Init   =  //is the same for U,V,W phases
            {

			  .timer_mode           = (uint8_t)XMC_CCU8_SLICE_TIMER_COUNT_MODE_EA,  //edge-alligned
			  .asymmetric_pwm       = (uint8_t)true,

			  .monoshot              = XMC_CCU8_SLICE_TIMER_REPEAT_MODE_REPEAT,
              .shadow_xfer_clear     = 1U,    //check
              .dither_timer_period   = 0U,
              .dither_duty_cycle     = 0U,
              .prescaler_mode        = (uint8_t)XMC_CCU8_SLICE_PRESCALER_MODE_NORMAL,
              .mcm_ch1_enable        = 0U,
              .mcm_ch2_enable        = 0U,
              .slice_status          = (uint8_t)XMC_CCU8_SLICE_STATUS_CHANNEL_1,
              .passive_level_out0    = MOTOR_RUN_HIGH_SIDE,
              .passive_level_out1    = MOTOR_RUN_LOW_SIDE,
		     #if(UC_SERIES == XMC14 || defined(CCU8V3)) //XMC14 or XMC4-CCU8V3
				 .selector_out0         = PHASE_U_HS_OUTPUT0_SOURCE,
	  			 .selector_out1         = PHASE_U_LS_OUTPUT1_SOURCE,
	             .selector_out2         = PHASE_U_HS_OUTPUT2_SOURCE,
	             .selector_out3         = PHASE_U_LS_OUTPUT3_SOURCE,
			 #else                                     //XMC13 or XMC44-normal
	              .invert_out0           = 1U,
	              .invert_out1           = 0U,
			  #endif
              .prescaler_initval     = CCU8_MODULE_PRESCALER_VALUE,
              .float_limit           = 0U,
              .dither_limit          = 0U,

			                .timer_concatenation   = 0U
};

XMC_CCU8_SLICE_COMPARE_CONFIG_t CCU8_Phase_V_Init   =  //is the same for U,V,W phases
            {

			  .timer_mode           = (uint8_t)XMC_CCU8_SLICE_TIMER_COUNT_MODE_EA,  //edge-alligned
			  .asymmetric_pwm       = (uint8_t)true,

			  .monoshot              = XMC_CCU8_SLICE_TIMER_REPEAT_MODE_REPEAT,
              .shadow_xfer_clear     = 1U,    //check
              .dither_timer_period   = 0U,
              .dither_duty_cycle     = 0U,
              .prescaler_mode        = (uint8_t)XMC_CCU8_SLICE_PRESCALER_MODE_NORMAL,
              .mcm_ch1_enable        = 0U,
              .mcm_ch2_enable        = 0U,
              .slice_status          = (uint8_t)XMC_CCU8_SLICE_STATUS_CHANNEL_1,
              .passive_level_out0    = MOTOR_RUN_HIGH_SIDE,
              .passive_level_out1    = MOTOR_RUN_LOW_SIDE,
		     #if(UC_SERIES == XMC14 || defined(CCU8V3)) //XMC14 or XMC4-CCU8V3
				 .selector_out0         = PHASE_V_HS_OUTPUT0_SOURCE,
	  			 .selector_out1         = PHASE_V_LS_OUTPUT1_SOURCE,
	             .selector_out2         = PHASE_V_HS_OUTPUT2_SOURCE,
	             .selector_out3         = PHASE_V_LS_OUTPUT3_SOURCE,
			 #else                                     //XMC13 or XMC44-normal
	              .invert_out0           = 1U,
	              .invert_out1           = 0U,
			  #endif
              .prescaler_initval     = CCU8_MODULE_PRESCALER_VALUE,
              .float_limit           = 0U,
              .dither_limit          = 0U,
              .timer_concatenation   = 0U
};
XMC_CCU8_SLICE_COMPARE_CONFIG_t CCU8_Phase_W_Init   =  //is the same for U,V,W phases
            {

			  .timer_mode           = (uint8_t)XMC_CCU8_SLICE_TIMER_COUNT_MODE_EA,  //edge-alligned
			  .asymmetric_pwm       = (uint8_t)true,

			  .monoshot              = XMC_CCU8_SLICE_TIMER_REPEAT_MODE_REPEAT,
              .shadow_xfer_clear     = 1U,    //check
              .dither_timer_period   = 0U,
              .dither_duty_cycle     = 0U,
              .prescaler_mode        = (uint8_t)XMC_CCU8_SLICE_PRESCALER_MODE_NORMAL,
              .mcm_ch1_enable        = 0U,
              .mcm_ch2_enable        = 0U,
              .slice_status          = (uint8_t)XMC_CCU8_SLICE_STATUS_CHANNEL_1,
              .passive_level_out0    = MOTOR_RUN_HIGH_SIDE,
              .passive_level_out1    = MOTOR_RUN_LOW_SIDE,
		     #if(UC_SERIES == XMC14 || defined(CCU8V3)) //XMC14 or XMC4-CCU8V3
				 .selector_out0         = PHASE_W_HS_OUTPUT0_SOURCE,
	  			 .selector_out1         = PHASE_W_LS_OUTPUT1_SOURCE,
	             .selector_out2         = PHASE_W_HS_OUTPUT2_SOURCE,
	             .selector_out3         = PHASE_W_LS_OUTPUT3_SOURCE,
			 #else                                     //XMC13 or XMC44-normal
	              .invert_out0           = 1U,
	              .invert_out1           = 0U,
			  #endif
              .prescaler_initval     = CCU8_MODULE_PRESCALER_VALUE,
              .float_limit           = 0U,
              .dither_limit          = 0U,
              .timer_concatenation   = 0U
};
/**
 *  Data Structure initialization - CC8 Slice configuration to trigger ADC conversion start.
 */



const XMC_CCU8_SLICE_COMPARE_CONFIG_t  CCU8_ADC_TriggerInit     =
{
              .timer_mode            = XMC_CCU8_SLICE_TIMER_COUNT_MODE_EA,
              .monoshot              = XMC_CCU8_SLICE_TIMER_REPEAT_MODE_REPEAT,
              .shadow_xfer_clear     = 1U,
              .dither_timer_period   = 0U,
              .dither_duty_cycle     = 0U,
              .prescaler_mode        = XMC_CCU8_SLICE_PRESCALER_MODE_NORMAL,
              .mcm_ch1_enable        = 0U,
              .mcm_ch2_enable        = 0U,
              .slice_status          = XMC_CCU8_SLICE_STATUS_CHANNEL_1,
			  .passive_level_out0    = (uint32_t)XMC_CCU8_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
              .passive_level_out1    = (uint32_t)XMC_CCU8_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,

			  .asymmetric_pwm       = (uint8_t)true,

	        #if(UC_SERIES == XMC14 || defined(CCU8V3)) //XMC14 or XMC4-CCU8V3
              .selector_out0         = XMC_CCU8_SOURCE_OUT0_ST1,
              .selector_out1         = XMC_CCU8_SOURCE_OUT1_INV_ST1,
              .selector_out2         = XMC_CCU8_SOURCE_OUT2_ST2,
              .selector_out3         = XMC_CCU8_SOURCE_OUT3_INV_ST2,
			 #else                                     //XMC13 or XMC44-normal
              .invert_out0           = 0U,
              .invert_out1           = 1U,
			  #endif
              .prescaler_initval     = CCU8_MODULE_PRESCALER_VALUE,
              .float_limit           = 0U,
              .dither_limit          = 0U,
              .timer_concatenation   = 0U
};

/**
 *  Data Structure initialization - CC8 Slice dead time configuration.
 */

XMC_CCU8_SLICE_DEAD_TIME_CONFIG_t  CCU8_PWM_Deadtime_config =
    {
	#if(UC_SERIES == XMC44)
      .enable_dead_time_channel1         = 1U,
      .enable_dead_time_channel2         = 0U,
      .channel1_st_path                  = 1U,
      .channel1_inv_st_path              = 1U,
      .div                               = (uint32_t)XMC_CCU8_SLICE_DTC_DIV_1,
	  .channel1_st_rising_edge_counter   = CCU8_DEADTIME_RISE,
      .channel1_st_falling_edge_counter  = CCU8_DEADTIME_FALL,
	  #else
	    .dtc = 0x3FU,
	  #endif
    };

/**
 *  Data Structure initialization - CC8 Slice event 0 set to sync start.
 */
/**
 *  Data Structure initialization - CC8 Slice event 0 set to sync start.
 */
XMC_CCU8_SLICE_EVENT_CONFIG_t CCU8_Input_event0_config =
{
#if(UC_SERIES == XMC14)										//XMC14
  .mapped_input = XMC_CCU8_SLICE_INPUT_AH,
#else
        .mapped_input        = XMC_CCU8_SLICE_INPUT_H,      //XMC13 and XMC44
#endif
        .edge                = XMC_CCU8_SLICE_EVENT_EDGE_SENSITIVITY_RISING_EDGE,
};

/**
 *  Data Structure initialization - CC8 Slice trap event configuration.
 */
XMC_CCU8_SLICE_EVENT_CONFIG_t CCU8_Input_trap_config =
     {
#if(UC_SERIES == XMC14)									//XMC14
	.mapped_input = XMC_CCU8_SLICE_INPUT_AA,
#else													//XMC13 and 44
	.mapped_input = XMC_CCU8_SLICE_INPUT_A,
#endif
  .edge = XMC_CCU8_SLICE_EVENT_EDGE_SENSITIVITY_NONE,
  .level = CCU8_INPUT_TRAP_LEVEL,
  .duration = XMC_CCU8_SLICE_EVENT_FILTER_7_CYCLES
};



/*********************************************************************************************************************
 * API IMPLEMENTATION
 ***************************************/

/* API to initialize CCU8 module for 3 phase pwm generation. Trap functionality enable */

void pmsm_foc_ccu8_init(void)
{
	// Enable GLOBAL CCU8
	XMC_CCU8_Init(CCU8_MODULE, XMC_CCU8_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
	// Start the prescaler
	XMC_CCU8_StartPrescaler(CCU8_MODULE);
	//Init Slice
	XMC_CCU8_SLICE_CompareInit(CCU8_MODULE_PHASE_U, &CCU8_Phase_U_Init);
	XMC_CCU8_SLICE_CompareInit(CCU8_MODULE_PHASE_V, &CCU8_Phase_V_Init);
	XMC_CCU8_SLICE_CompareInit(CCU8_MODULE_PHASE_W, &CCU8_Phase_W_Init);
	XMC_CCU8_SLICE_CompareInit(CCU8_MODULE_ADC_TR, &CCU8_ADC_TriggerInit);
	//Set period match value of the timer
	XMC_CCU8_SLICE_SetTimerPeriodMatch(CCU8_MODULE_PHASE_U, (uint16_t)CCU8_PERIOD_REG);
	XMC_CCU8_SLICE_SetTimerPeriodMatch(CCU8_MODULE_PHASE_V,(uint16_t)CCU8_PERIOD_REG);
	XMC_CCU8_SLICE_SetTimerPeriodMatch(CCU8_MODULE_PHASE_W,(uint16_t)CCU8_PERIOD_REG);
	XMC_CCU8_SLICE_SetTimerPeriodMatch(CCU8_MODULE_ADC_TR,(uint16_t)CCU8_PERIOD_REG);
	//Set timer compare match value for channel 1
	XMC_CCU8_SLICE_SetTimerCompareMatchChannel1(CCU8_MODULE_PHASE_U, 0);
	XMC_CCU8_SLICE_SetTimerCompareMatchChannel2(CCU8_MODULE_PHASE_U, CCU8_PERIOD_REG+1);
	XMC_CCU8_SLICE_SetTimerCompareMatchChannel1(CCU8_MODULE_PHASE_V, 0);
	XMC_CCU8_SLICE_SetTimerCompareMatchChannel2(CCU8_MODULE_PHASE_V, CCU8_PERIOD_REG+1);
	XMC_CCU8_SLICE_SetTimerCompareMatchChannel1(CCU8_MODULE_PHASE_W, 0);
	XMC_CCU8_SLICE_SetTimerCompareMatchChannel2(CCU8_MODULE_PHASE_W, CCU8_PERIOD_REG+1);

#if(CURRENT_SENSING == USER_SINGLE_SHUNT_CONV)
  /* First CCU83 period is a constant, e.g.: 2Tz. */
  XMC_CCU8_SLICE_SetTimerPeriodMatch(CCU8_MODULE_ADC_TR, PERIOD_REG);
  /* For ADCTz1 trigger. */
  XMC_CCU8_SLICE_SetTimerCompareMatchChannel1(CCU8_MODULE_ADC_TR, TRIGGER_POINT);
  /* For ADCTz2 trigger. */
  XMC_CCU8_SLICE_SetTimerCompareMatchChannel2(CCU8_MODULE_ADC_TR, (TZ_PZV + TRIGGER_POINT));

#else
  XMC_CCU8_SLICE_SetTimerPeriodMatch(CCU8_MODULE_ADC_TR, CCU8_PERIOD_REG);
  /* For ADC trigger after CR1 */
	XMC_CCU8_SLICE_SetTimerCompareMatchChannel1(CCU8_MODULE_ADC_TR, (CCU8_PERIOD_REG /2));
	XMC_CCU8_SLICE_SetTimerCompareMatchChannel2(CCU8_MODULE_ADC_TR, ((CCU8_PERIOD_REG) /2) + 1);
#endif




	XMC_CCU8_EnableShadowTransfer(CCU8_MODULE, ((uint32_t)XMC_CCU8_SHADOW_TRANSFER_SLICE_0 | (uint32_t)XMC_CCU8_SHADOW_TRANSFER_SLICE_1 | (uint32_t)XMC_CCU8_SHADOW_TRANSFER_SLICE_2 | (uint32_t)XMC_CCU8_SHADOW_TRANSFER_SLICE_3));
	//DeadTime
#if(UC_FAMILY == XMC1)   //calls only for xmc1
	// Dead time enabled, No prescaler for the dead time counters
	// Dead time could cause motor phase current distortion, especially at low motor speed
	CCU8_PWM_Deadtime_config.dc1r = DEAD_TIME;
	CCU8_PWM_Deadtime_config.dc2r = DEAD_TIME;
#endif
	XMC_CCU8_SLICE_DeadTimeInit((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_U, &CCU8_PWM_Deadtime_config);
	XMC_CCU8_SLICE_DeadTimeInit((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_V, &CCU8_PWM_Deadtime_config);
	XMC_CCU8_SLICE_DeadTimeInit((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_W, &CCU8_PWM_Deadtime_config);


	// Configure events
	// Configure slice to a external event 0
	XMC_CCU8_SLICE_ConfigureEvent((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_U, XMC_CCU8_SLICE_EVENT_0, &CCU8_Input_event0_config);
	XMC_CCU8_SLICE_ConfigureEvent((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_V, XMC_CCU8_SLICE_EVENT_0, &CCU8_Input_event0_config);
	XMC_CCU8_SLICE_ConfigureEvent((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_W, XMC_CCU8_SLICE_EVENT_0, &CCU8_Input_event0_config);
	XMC_CCU8_SLICE_ConfigureEvent((XMC_CCU8_SLICE_t*) CCU8_MODULE_ADC_TR, XMC_CCU8_SLICE_EVENT_0, &CCU8_Input_event0_config);
	// Configure slice to a external event 2
	XMC_CCU8_SLICE_ConfigureEvent((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_U, XMC_CCU8_SLICE_EVENT_2, &CCU8_Input_trap_config);
	XMC_CCU8_SLICE_ConfigureEvent((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_V, XMC_CCU8_SLICE_EVENT_2, &CCU8_Input_trap_config);
	XMC_CCU8_SLICE_ConfigureEvent((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_W, XMC_CCU8_SLICE_EVENT_2, &CCU8_Input_trap_config);
#if(UC_SERIES == XMC44)
	// External signal controls start of the timer
	XMC_CCU8_SLICE_StartConfig((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_U, XMC_CCU8_SLICE_EVENT_0, XMC_CCU8_SLICE_START_MODE_TIMER_START);
	XMC_CCU8_SLICE_StartConfig((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_V, XMC_CCU8_SLICE_EVENT_0, XMC_CCU8_SLICE_START_MODE_TIMER_START);
	XMC_CCU8_SLICE_StartConfig((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_W, XMC_CCU8_SLICE_EVENT_0, XMC_CCU8_SLICE_START_MODE_TIMER_START);
	XMC_CCU8_SLICE_StartConfig((XMC_CCU8_SLICE_t*) CCU8_MODULE_ADC_TR, XMC_CCU8_SLICE_EVENT_0, XMC_CCU8_SLICE_START_MODE_TIMER_START);
	XMC_CCU8_SLICE_SetInterruptNode((XMC_CCU8_SLICE_t*) CCU8_MODULE_ADC_TR, XMC_CCU8_SLICE_IRQ_ID_COMPARE_MATCH_UP_CH_1, XMC_CCU8_SLICE_SR_ID_2);
	XMC_CCU8_SLICE_EnableEvent((XMC_CCU8_SLICE_t*) CCU8_MODULE_ADC_TR, XMC_CCU8_SLICE_IRQ_ID_COMPARE_MATCH_UP_CH_1);
	#if(CURRENT_SENSING == USER_SINGLE_SHUNT_CONV)
	XMC_CCU8_SLICE_SetInterruptNode((XMC_CCU8_SLICE_t*) CCU8_MODULE_ADC_TR, XMC_CCU8_SLICE_IRQ_ID_COMPARE_MATCH_UP_CH_2, XMC_CCU8_SLICE_SR_ID_2);
	XMC_CCU8_SLICE_EnableEvent((XMC_CCU8_SLICE_t*) CCU8_MODULE_ADC_TR, XMC_CCU8_SLICE_IRQ_ID_COMPARE_MATCH_UP_CH_2);
	#endif
	// PWM signal (ST bit) output gets modulated by external signal trap
#endif

	// Disable Global Start Control CCU80
	XMC_SCU_SetCcuTriggerLow(SCU_GENERAL_CCUCON_GSC80_Msk);
	// PWM signal (ST bit) output gets modulated by external signal trap
	XMC_CCU8_SLICE_TrapConfig((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_U, XMC_CCU8_SLICE_TRAP_EXIT_MODE_SW, true);
	XMC_CCU8_SLICE_TrapConfig((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_V, XMC_CCU8_SLICE_TRAP_EXIT_MODE_SW, true);
	XMC_CCU8_SLICE_TrapConfig((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_W, XMC_CCU8_SLICE_TRAP_EXIT_MODE_SW, true);
	// Clear idle bit slice 0,1,2,3 for CCU80 Kernel
	XMC_CCU8_EnableMultipleClocks(CCU8_MODULE, XMC_CCU8_GIDLC_CLOCK_MASK);
	XMC_CCU8_SLICE_SetInterruptNode((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_U, XMC_CCU8_SLICE_IRQ_ID_PERIOD_MATCH, XMC_CCU8_SLICE_SR_ID_0);
	XMC_CCU8_SLICE_EnableEvent((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_U, XMC_CCU8_SLICE_IRQ_ID_PERIOD_MATCH);
	XMC_CCU8_SLICE_EnableEvent((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_U, XMC_CCU8_SLICE_IRQ_ID_EVENT2);

#if(UC_SERIES == XMC44)
	// Enable the interrupts
	// Enable events. Bind event to corresponding service request node.
	XMC_CCU8_SLICE_SetInterruptNode((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_U, XMC_CCU8_SLICE_IRQ_ID_EVENT2, XMC_CCU8_SLICE_SR_ID_3);
	//Primary loop interrupt
	NVIC_SetPriority((IRQn_Type)CCU80_0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
	NVIC_EnableIRQ((IRQn_Type)CCU80_0_IRQn);
	//Trap interrupt
	NVIC_SetPriority((IRQn_Type)CCU80_3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	NVIC_EnableIRQ((IRQn_Type)CCU80_3_IRQn);
#elif (UC_SERIES == XMC14)
	//Primary loop interrupt
	NVIC_SetPriority (IRQ25_IRQn, 2U);
    XMC_SCU_SetInterruptControl(IRQ25_IRQn, XMC_SCU_IRQCTRL_CCU80_SR0_IRQ25);
    NVIC_EnableIRQ(IRQ25_IRQn);
	//Trap interrupt
	XMC_CCU8_SLICE_SetInterruptNode((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_U, XMC_CCU8_SLICE_IRQ_ID_EVENT2, XMC_CCU8_SLICE_SR_ID_1);
	NVIC_SetPriority (IRQ26_IRQn, 0U);
    XMC_SCU_SetInterruptControl(IRQ26_IRQn, XMC_SCU_IRQCTRL_CCU80_SR1_IRQ26);
	NVIC_EnableIRQ(IRQ26_IRQn);
#else       //XMC13
	//Primary loop interrupt
	NVIC_SetPriority (CCU80_0_IRQn, 2U);
	NVIC_EnableIRQ(CCU80_0_IRQn);
	//Trap interrupt
	XMC_CCU8_SLICE_SetInterruptNode((XMC_CCU8_SLICE_t*) CCU8_MODULE_PHASE_U, XMC_CCU8_SLICE_IRQ_ID_EVENT2, XMC_CCU8_SLICE_SR_ID_1);
	NVIC_SetPriority (CCU80_1_IRQn, 0U);
	NVIC_EnableIRQ(CCU80_1_IRQn);
#endif
    XMC_CCU8_SLICE_EnableTrap(CCU8_MODULE_PHASE_U, (uint32_t)(XMC_CCU8_SLICE_OUTPUT_0 | XMC_CCU8_SLICE_OUTPUT_1 | XMC_CCU8_SLICE_OUTPUT_2 | XMC_CCU8_SLICE_OUTPUT_3));
    XMC_CCU8_SLICE_EnableTrap(CCU8_MODULE_PHASE_V, (uint32_t)(XMC_CCU8_SLICE_OUTPUT_0 | XMC_CCU8_SLICE_OUTPUT_1 | XMC_CCU8_SLICE_OUTPUT_2 | XMC_CCU8_SLICE_OUTPUT_3));
    XMC_CCU8_SLICE_EnableTrap(CCU8_MODULE_PHASE_W, (uint32_t)(XMC_CCU8_SLICE_OUTPUT_0 | XMC_CCU8_SLICE_OUTPUT_1 | XMC_CCU8_SLICE_OUTPUT_2 | XMC_CCU8_SLICE_OUTPUT_3));
	/* Interrupt Status Clear, for interrupts of Period Match, Trap Flag, and Event2 */
	CCU8_MODULE_PHASE_U->SWR |= 0x00000C01U;
}





