/*
 * timer_pwm.h
 *
 *  Created on: Sep 23, 2026
 *      Author: rcall
 */

#ifndef INC_TIMER_PWM_H_
#define INC_TIMER_PWM_H_

#include "stm32f4xx.h"

#define CLK_HZ			100000
#define PWM_FREQ_HZ		20000
#define TIM_PWM			TIM1

// time-base unit values for 20kHz PWM at 100 MHz clock
#define TIM_ARR		2500
#define TIM_PSC		0
#define TIM_DTG		100	// 100 MHz needs 100 cycles for 1us

void timer_pwm_init(void);
void timer_pwm_start_temp(void);
void timer_pwm_irq(void);


#endif /* INC_TIMER_PWM_H_ */
