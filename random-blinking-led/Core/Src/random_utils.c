/*
 * random_utils.c
 *
 *  Created on: 27 de mar. de 2026
 *      Author: Clovisf
 */
#include "random_utils.h"
#include <stdlib.h>

static ADC_HandleTypeDef *adc;
static TIM_HandleTypeDef *tim;
static uint32_t reseed_timer = 0;
static uint32_t seed = 0;

void init_random(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim)
{
	adc = hadc;
	tim = htim;

	HAL_ADC_Start(adc);
	HAL_ADC_PollForConversion(adc, 10);

	uint16_t noise = HAL_ADC_GetValue(adc);
	uint32_t t = HAL_GetTick();
	uint32_t c = tim->Instance->CNT;

	seed = noise ^ t ^ c;
	srand(seed);

}
int random_10_100(void)
	{
	    return (rand() % 91) + 10;
	}
void maybe_reseed(void)
{
    if (HAL_GetTick() - reseed_timer > 300000)   // 60 seconds
    {
        init_random(adc, tim);
        reseed_timer= HAL_GetTick();
    }
}

