
#include "led.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define LED_GREEN_PIN GPIO5
#define LED_GREEN_PORT GPIOA

void led_green_init(void) {
  rcc_periph_clock_enable(RCC_GPIOA);
  led_green_clear();
  gpio_mode_setup(LED_GREEN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GREEN_PIN);
}

void led_green_set(void) {
  gpio_set(LED_GREEN_PORT, LED_GREEN_PIN);
}

void led_green_clear(void) {
  gpio_clear(LED_GREEN_PORT, LED_GREEN_PIN);
}

void led_green_toggle(void) {
  gpio_toggle(LED_GREEN_PORT, LED_GREEN_PIN);
}
