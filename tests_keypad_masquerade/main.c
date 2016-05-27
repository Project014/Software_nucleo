
#include "clock.h"
#include "pwm.h"
#include "led.h"
#include "keypad_masquerade.h"
#include "rotary_dial.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/syscfg.h>

typedef struct {
  char lin, col;
}combi_t;

combi_t conv[10] = {
  {3, 1},                       /* 0 */
  {2, 4},                       /* 1 */
  {3, 4},                       /* 2 */
  {1, 4},                       /* 3 */
  {2, 3},                       /* 4 */
  {3, 3},                       /* 5 */
  {1, 3},                       /* 6 */
  {2, 2},                       /* 7 */
  {3, 2},                       /* 8 */
  {1, 2},                       /* 9 */
};

static void keypad_toggle_digit(char digit) {
  combi_t combi = conv[digit - '0'];
  keypad_masquerade_toggle_key(combi.lin, combi.col);
}

static void keypad_sequence(const char* seq) {
  while ( *seq ) {
    keypad_toggle_digit(*seq++);
  }
}

static void on_get_digit(char digit) {
  keypad_toggle_digit(digit);
  led_green_toggle();
}

int hp_enabled = 0;

int main(void) {
  clock_init();
  time_init();
  pwm_init(0);
  led_green_init();
  keypad_masquerade_init();
  rotary_dial_init(on_get_digit);

  /* hp key config */
  gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO4);

  while ( 1 ) {
    if ( !hp_enabled && !gpio_get(GPIOC, GPIO4)) {
      keypad_masquerade_toggle_key(0, 0); /* hp on */
      hp_enabled = 1;
    }
    if ( hp_enabled && gpio_get(GPIOC, GPIO4)) {
      keypad_masquerade_toggle_key(0, 0); /* hp off */
      hp_enabled = 0;
    }
  }

  return 0;
}
