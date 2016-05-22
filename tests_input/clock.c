
#include "clock.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>

#define LED_GREEN_PIN GPIO5
#define LED_GREEN_PORT GPIOA

void clock_init(void) {
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
}

static volatile uint32_t system_ms;

void time_init(void) {
  system_ms = 0;
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  STK_CVR = 0;
  systick_set_reload(32000000/1000 -1);
  systick_counter_enable();
  systick_interrupt_enable();
}

void time_delay_ms(unsigned int delay) {
  uint32_t wake = system_ms + delay;
  uint32_t cnt;
  do {
    systick_counter_disable();
    cnt = system_ms;
    systick_counter_enable();
  }
  while (wake > cnt);
}

uint32_t time_get_tick_ms(void) {
  uint32_t tmp;
  systick_interrupt_disable();
  tmp = system_ms;
  systick_interrupt_enable();
  return tmp;
}

void sys_tick_handler(void) {
  system_ms++;
}
