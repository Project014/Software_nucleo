
#include "keypad_masquerade.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/syscfg.h>

static uint8_t key_line_select_config;
static uint8_t key_line_data;
volatile static int key_catched, key_half_catched;

void keypad_masquerade_init(void) {
  /* enable select gpios as inputs */
  /* s0, s1 */
  rcc_periph_clock_enable(RCC_GPIOA);
  gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE,
                  GPIO0 | GPIO1);
  /* s2, s3 */
  rcc_periph_clock_enable(RCC_GPIOC);
  gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE,
                  GPIO2 | GPIO3);

  /* enable syscfg :], or else changing exti source is impossible */
  rcc_periph_clock_enable(RCC_SYSCFG);

  exti_select_source(EXTI0, GPIOA);
  exti_set_trigger(EXTI0, EXTI_TRIGGER_BOTH);
  exti_enable_request(EXTI0);

  exti_select_source(EXTI1, GPIOA);
  exti_set_trigger(EXTI1, EXTI_TRIGGER_BOTH);
  exti_enable_request(EXTI1);

  exti_select_source(EXTI2, GPIOC);
  /* ^  this should not work on cm0 ... */
  /* (no byte access, only word) */
  exti_set_trigger(EXTI2, EXTI_TRIGGER_BOTH);
  exti_enable_request(EXTI2);

  exti_select_source(EXTI3, GPIOC);
  exti_set_trigger(EXTI3, EXTI_TRIGGER_BOTH);
  exti_enable_request(EXTI3);

  /* enable data gpios as outputs, open-drain */
  rcc_periph_clock_enable(RCC_GPIOB);
  gpio_set(GPIOB, GPIO8 | GPIO9 | GPIO10 | GPIO11 | GPIO12);

  gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ,
                          GPIO8 | GPIO9 | GPIO10 | GPIO11 | GPIO12);

  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                  GPIO8 | GPIO9 | GPIO10 | GPIO11 | GPIO12);

  //gpio_clear(GPIOB, GPIO8 | GPIO9 | GPIO10 | GPIO11 | GPIO12);

  key_line_select_config = 0b0000; // line zero
  key_line_data = 0b00000; // no key pressed
  key_catched = 0; // not catched yet

  /* enable interrupts */
  nvic_enable_irq(NVIC_EXTI0_1_IRQ);
  nvic_enable_irq(NVIC_EXTI2_3_IRQ);
}

inline static uint8_t get_selected_lines(void) {
  uint8_t bit10 = gpio_port_read(GPIOA) & 0b11;
  uint8_t bit32 = gpio_port_read(GPIOC) & 0b1100;
  return ~(bit32 | bit10);
}

inline static void set_line_data(uint8_t data) {
  gpio_port_write(GPIOB, (~data) << 8); // PB8..PB12
}

static void update_keys(void) {
  nvic_disable_irq(NVIC_EXTI0_1_IRQ);
  nvic_disable_irq(NVIC_EXTI2_3_IRQ);

  uint8_t selected_lines = get_selected_lines() & 0b1111;
  if ( selected_lines & key_line_select_config ) {
    set_line_data(key_line_data);
    key_half_catched = 1;
    if ( selected_lines == key_line_select_config ) {
      key_catched = 1;
    }
  }
  else {
    set_line_data(0b00000);     /* no key pressed */
  }

  nvic_enable_irq(NVIC_EXTI0_1_IRQ);
  nvic_enable_irq(NVIC_EXTI2_3_IRQ);
}

void keypad_masquerade_press_key(uint8_t line, uint8_t column) {
  key_catched = 0;
  key_line_select_config = 1 << line;
  key_line_data = 1 << column;

  /* force refresh */
  update_keys();

  while ( key_catched != 1 );
}

void keypad_masquerade_release_key(void) {
  //key_line_select_config = 0;
  key_half_catched = 0;
  key_line_data = 0;

  /* force refresh */
  update_keys();

  while ( key_half_catched != 1);
  time_delay_ms(50);
}

void keypad_masquerade_toggle_key(uint8_t line, uint8_t column) {
  keypad_masquerade_press_key(line, column);
  time_delay_ms(80);
  keypad_masquerade_release_key();
  time_delay_ms(10);
  //while ( key_catched != 1 );
}

void exti0_1_isr() {
  exti_reset_request(EXTI0);
  exti_reset_request(EXTI1);
  update_keys();
}

void exti2_3_isr() {
  exti_reset_request(EXTI2);
  exti_reset_request(EXTI3);
  update_keys();
}
