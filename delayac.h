#ifndef delayac_H_
#define delayac_H_

#include "stm32f10x.h"

void Delay_Init(void);
void Delay_Ms(uint32_t u32DelayInMs);
void delay_us(uint32_t delay);

#endif
