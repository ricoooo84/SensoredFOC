/*
 * debug.c
 *
 *  Created on: Sep 24, 2026
 *      Author: rcall
 */

#include "debug.h"
#include "stm32f4xx.h"

// hardcodes for PC4
void debug_gpio_init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	// 0b01 = output
	GPIOC->MODER |= (0x1 << GPIO_MODER_MODE4_Pos);
}

// hardcoded for PC4
void debug_gpio_toggle(void) {
	if (GPIOC->ODR & GPIO_ODR_OD4) {
		GPIOC->BSRR = GPIO_BSRR_BR4;
	} else {
		GPIOC->BSRR = GPIO_BSRR_BS4;
	}
}
