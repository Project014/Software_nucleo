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
 #include <libopencm3/cm3/nvic.h>
 #include <libopencm3/cm3/systick.h>

#define LED_GREEN_PIN GPIO5
#define LED_GREEN_PORT GPIOA

typedef struct FilteredInput FilteredInput;
static struct FilteredInput {
	int gpio_port,gpio_pin;
	int counter;
};

void filtered_input_init(FilteredInput* filtered_input, int gpio_port, int gpio_pin ) {
	filtered_input->gpio_port = gpio_port;
	filtered_input->gpio_pin = gpio_pin;
	filtered_input->counter = 0;
}

static void gpio_setup(void)
{
	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* set pins to output mode, push pull */
	gpio_mode_setup(LED_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GREEN_PIN);
}

static void delay(void)
{
	int i;
	for (i = 0; i < 3200000; i++) {	/* Wait a bit. */
		__asm__("nop");
	}
}

static void delay_short(void)
{
	int i;
	for (i = 0; i < 160000; i++) {	/* Wait a bit. */
		__asm__("nop");
	}
}

/* Set up timer to fire freq times per second */
static void systick_setup(int freq)
{
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	/* clear counter so it starts right away */
	STK_CVR = 0;
	systick_set_reload(32000000 / freq);
	systick_counter_enable();
	systick_interrupt_enable();
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

	systick_setup(2);

  /* init switch */
  gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO12);
  gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO11);
  gpio_clear(GPIOA, GPIO11);
  /* get value */
  gpio_get(GPIOA, GPIO12);

	while (1) {
		/*
		delay();
		gpio_toggle(LED_GREEN_PORT, LED_GREEN_PIN);
		delay_short();
		gpio_toggle(LED_GREEN_PORT, LED_GREEN_PIN);
		delay_short();
		gpio_toggle(LED_GREEN_PORT, LED_GREEN_PIN);
		delay_short();
		gpio_toggle(LED_GREEN_PORT, LED_GREEN_PIN);
		delay_short();
*/
	}

	return 0;
}

void sys_tick_handler(void) {
	gpio_toggle(LED_GREEN_PORT, LED_GREEN_PIN);
}
