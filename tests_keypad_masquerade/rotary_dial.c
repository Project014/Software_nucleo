
#include "rotary_dial.h"
#include "clock.h"
#include "cm0_priority_patch.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/syscfg.h>

#define GPIO_ENABLE GPIO4
#define GPIO_COUNT GPIO15

typedef enum {
  dial_state_idle,
  dial_state_counting
}dial_state_t;

static volatile dial_state_t dial_state;
static volatile unsigned int dial_count_num;

static rotary_dial_callback_t callback;

void rotary_dial_init(rotary_dial_callback_t on_get_digit_callback) {
  /* save callback  */
  callback = on_get_digit_callback;

  /* config gpios as pullup'd inputs */
  rcc_periph_clock_enable(RCC_GPIOB);
  gpio_mode_setup(GPIOB, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
                  GPIO_ENABLE | GPIO_COUNT);

  /* enable syscfg :], or else changing exti source from GPIOA is impossible */
  rcc_periph_clock_enable(RCC_SYSCFG);

  /* enable pin : active low */
  exti_select_source(EXTI4, GPIOB);
  exti_set_trigger(EXTI4, EXTI_TRIGGER_FALLING);
  exti_enable_request(EXTI4);

  /* count pin : active high */
  exti_select_source(EXTI15, GPIOB);
  exti_set_trigger(EXTI15, EXTI_TRIGGER_RISING);
  exti_enable_request(EXTI15);

  dial_state = dial_state_idle;
  dial_count_num = 0;

  /* enable irqs */
  nvic_set_priority_cm0(NVIC_EXTI4_15_IRQ, 3);
  nvic_enable_irq(NVIC_EXTI4_15_IRQ);
}

static void key_enable_on_event(void) {
  
  /* static int last_tick = 0; */
  /* int current_tick = time_get_tick_ms(); */
  /* int delta =  current_tick - last_tick; */
  /* if ( delta < 30 ) { */
  /*   return ; */
  /* } */
  /* last_tick = current_tick; */

  
  switch ( dial_state ) {
  case dial_state_idle: {
    dial_state = dial_state_counting;
    dial_count_num = 0;
    exti_set_trigger(EXTI4, EXTI_TRIGGER_RISING);
    break;
  }
  case dial_state_counting: {
    dial_state = dial_state_idle;
    if ( callback ) {
      callback(dial_count_num);
      exti_set_trigger(EXTI4, EXTI_TRIGGER_FALLING);
    }
    break;
  }
  default:
    break;
  }
}

static void key_count_on_event(void) {

  /* static int last_tick = 0; */
  /* int current_tick = time_get_tick_ms(); */
  /* int delta =  current_tick - last_tick; */
  /* if ( delta < 5 ) { */
  /*   return ; */
  /* } */
  /* last_tick = current_tick; */

  led_green_toggle();
  
  
  if ( dial_state == dial_state_counting ) {
    dial_count_num++;
  }
}

void exti4_15_isr() {

  if ( exti_get_flag_status(EXTI4) ) {
    key_enable_on_event();
    exti_reset_request(EXTI4);
  }
  if ( exti_get_flag_status(EXTI15) ) {
    key_count_on_event();
    exti_reset_request(EXTI15);
  }
}
