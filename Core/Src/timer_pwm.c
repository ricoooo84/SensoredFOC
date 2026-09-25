/*
 * timer_pwm.c
 *
 *  Created on: Sep 23, 2026
 *      Author: rcall
 */

#include "timer_pwm.h"
#include "math.h"
#include "debug.h"

static float theta = 0.0f;
static float omega = 0.2f;
static float _120deg = 2.0944f;

static void cfg_gpio_regs() {
	// (assuming TIM1)
	// CH1: PA8		CH1N: PA7
	// CH2: PA9		CH2N: PB0
	// CH3: PA10	CH3N: PB1

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

	// PA12-15 have nonzero mode reset values
	// 0b10 = 0x2 = AFmode
	GPIOA->MODER |= (0x2 << GPIO_MODER_MODE7_Pos) | (0x2 << GPIO_MODER_MODE8_Pos) |
					(0x2 << GPIO_MODER_MODE9_Pos) | (0x2 << GPIO_MODER_MODE10_Pos);
	GPIOB->MODER |= (0x2 << GPIO_MODER_MODE0_Pos) | (0x2 << GPIO_MODER_MODE1_Pos);

	// 0b0001 =
	GPIOA->AFR[0] |= (0x1 << GPIO_AFRL_AFSEL7_Pos);
	GPIOA->AFR[1] |= (0x1 << GPIO_AFRH_AFSEL8_Pos) | (0x1 << GPIO_AFRH_AFSEL9_Pos) |
					 (0x1 << GPIO_AFRH_AFSEL10_Pos);
	GPIOB->AFR[0] |= (0x1 << GPIO_AFRL_AFSEL0_Pos) | (0x1 << GPIO_AFRL_AFSEL1_Pos);
}

static void cfg_tim_interrupts() {
	TIM_PWM->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

static void cfg_tim_pwm() {
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	// PWM mode
	TIM_PWM->CR1 |= (0x01 << TIM_CR1_CMS_Pos); // center aligned PWM mode
	TIM_PWM->CCMR1 |= (0x6 << TIM_CCMR1_OC1M_Pos);
	TIM_PWM->CCMR1 |= (0x6 << TIM_CCMR1_OC2M_Pos);
	TIM_PWM->CCMR2 |= (0x6 << TIM_CCMR2_OC3M_Pos);

	// time base units
	TIM_PWM->RCR = 1; // UE triggers every other event
	TIM_PWM->PSC = TIM_PSC;
	TIM_PWM->ARR = TIM_ARR;

	// preload
	TIM_PWM->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;
	TIM_PWM->CCMR2 |= TIM_CCMR2_OC3PE;

	// channel enables
	TIM_PWM->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E |
				     TIM_CCER_CC1NE | TIM_CCER_CC2NE | TIM_CCER_CC3NE;
	TIM_PWM->BDTR |= TIM_BDTR_MOE | (TIM_DTG << TIM_BDTR_DTG_Pos); // 100 clock cycles = 1us deadtime (CKD = 0)

	cfg_tim_interrupts();

	// generate update by software before starting counter
	TIM_PWM->EGR |= TIM_EGR_UG;
	TIM_PWM->CR1 |= TIM_CR1_CEN;
}




void timer_pwm_init() {
	cfg_gpio_regs();
	cfg_tim_pwm();
}

void timer_pwm_sin() {
	float duty_a = 0.5f + 0.5*sinf(theta);
	float duty_b = 0.5f + 0.5*sinf(theta - _120deg);
	float duty_c = 0.5f + 0.5*sinf(theta - 2*_120deg);

	TIM1->CCR1 = (uint16_t)(duty_a * TIM1->ARR);
	TIM1->CCR2 = (uint16_t)(duty_b * TIM1->ARR);
	TIM1->CCR3 = (uint16_t)(duty_c * TIM1->ARR);

	theta += omega;
	if (theta > 6.2831853f) {
		theta -= 6.2831853f;
	}
}

// USE CCR1/2/3 TO CHANGE DUTY

void timer_pwm_irq() {
	timer_pwm_sin();

	TIM_PWM->SR &= ~(TIM_SR_UIF);
}
