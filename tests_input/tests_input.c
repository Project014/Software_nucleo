/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Copyright (C) 2012 Karl Palsson <karlp@tweak.net.au>
 * Copyright (C) 2016 Lucien Dos Santos Farret <luciendsf@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define LED_GREEN_PIN GPIO5
#define LED_GREEN_PORT GPIOA
#define RACCROCHE_PORT GPIO

static void gpio_setup(void)
{
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOC);

	/* set pins to output mode, push pull */
	gpio_mode_setup(LED_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GREEN_PIN);

	/* interrupteur raccroché/décroché */
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO12);
  	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO11);
  	gpio_clear(GPIOA, GPIO11);

	/* interrupteur cadran 1 */
	gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO6);
  	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
  	gpio_clear(GPIOC, GPIO5);

	/* interrupteur cadran 2 */
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO9);
  	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
  	gpio_clear(GPIOA, GPIO8);
}

int main(void)
{

	/* Enable HSE */
	rcc_osc_on(RCC_HSE);
	rcc_wait_for_osc_ready(RCC_HSE);

	/* setup PLL */
	// 8Mhz *8/2 -> 32MHz
	rcc_set_pll_source(RCC_HSE);
	rcc_set_pll_multiplier(RCC_CFGR_PLLMUL_MUL8);
	rcc_set_pll_divider(RCC_CFGR_PLLDIV_DIV2);
	rcc_osc_on(RCC_PLL);
	rcc_wait_for_osc_ready(RCC_PLL);

	/* switch to PLL */
	rcc_set_sysclk_source(RCC_PLL);

	/* setup AHB/APBx */
	rcc_set_hpre(RCC_CFGR_HPRE_NODIV);
	rcc_set_ppre1(RCC_CFGR_PPRE1_NODIV);
	rcc_set_ppre2(RCC_CFGR_PPRE2_NODIV);

	gpio_setup();

	while (1) {
		if(gpio_get(GPIOA, GPIO12))
			gpio_set(LED_GREEN_PORT, LED_GREEN_PIN);
		else
			gpio_clear(LED_GREEN_PORT, LED_GREEN_PIN);
	}

	return 0;
}
