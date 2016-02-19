
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include "time.h"

static volatile uint32_t system_millis;

void time_init() {
  system_millis = 0;

  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  /* clear counter so it starts right away */
  STK_CVR = 0;
  /* interrupt every 1ms */
  systick_set_reload(32000000 / 1000);
  systick_counter_enable();
  systick_interrupt_enable();
}

uint32_t time_delay_ms(unsigned int delay) {
  uint32_t wake = system_millis + delay;
  while (wake > system_millis);
}

uint32_t time_get_tick_ms(void) {
  return system_millis;
}

void sys_tick_handler(void) {
	system_millis++;
}
