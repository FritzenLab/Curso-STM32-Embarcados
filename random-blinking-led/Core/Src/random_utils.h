#ifndef RANDOM_UTILS_H
#define RANDOM_UTILS_H

#include "stm32g0xx_hal.h"

void init_random(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim);
void maybe_reseed(void);
int random_10_100(void);

#endif
