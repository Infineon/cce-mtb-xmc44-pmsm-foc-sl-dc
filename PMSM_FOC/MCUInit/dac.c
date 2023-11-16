/*
 * dac.c

 *
 *  Created on: Jun 24, 2019
 *      Author: trojan
 */

#include "xmc_device.h"
#if UC_SERIES == XMC44

#include <xmc_dac.h>

XMC_DAC_CH_CONFIG_t DAC_OFFSET_Channel_Conf =
{
  .data_type       = (uint8_t)XMC_DAC_CH_DATA_TYPE_UNSIGNED,
  .output_negation = (uint8_t)XMC_DAC_CH_OUTPUT_NEGATION_DISABLED,
  .output_scale    = (uint8_t)XMC_DAC_CH_OUTPUT_SCALE_NONE,
  .output_offset   = 0U
};

void pmsm_dac_inizialization(){
	  /* Enable DAC module */
	  XMC_DAC_CH_Init((XMC_DAC_t  *)(void *)DAC, 0, &DAC_OFFSET_Channel_Conf); //module, channel,

	  /* Configure DAC with user configured mode */
	  XMC_DAC_CH_StartDataMode((XMC_DAC_t  *)(void *)DAC, 0, XMC_DAC_CH_TRIGGER_INTERNAL, 2000U);
	    /*Initialized data register with zero */
	    XMC_DAC_CH_Write((XMC_DAC_t  *)(void *)DAC, 0, 1500U);
}

#endif
