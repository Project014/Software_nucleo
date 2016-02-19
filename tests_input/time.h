
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

void time_init();
uint32_t time_delay_ms(unsigned int delay);
uint32_t time_get_tick_ms(void);
