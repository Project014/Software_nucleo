
#include "pwm.h"

#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/timer.h"
#include "libopencm3/cm3/nvic.h"


static void pwm_timer_init(pwm_callback update_callback) {
  /** timer config **/
  rcc_periph_clock_enable(RCC_TIM2);
  timer_reset(TIM2);

  timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
  timer_set_prescaler(TIM2, 0);

  timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);
  timer_set_oc_mode(TIM2, TIM_OC2, TIM_OCM_PWM2);

  timer_enable_oc_output(TIM2, TIM_OC1);
  timer_enable_oc_output(TIM2, TIM_OC2);

  timer_set_period(TIM2, 1024-1);

  timer_set_oc_value(TIM2, TIM_OC1, 0);
  timer_set_oc_value(TIM2, TIM_OC2, 0);

  timer_enable_counter(TIM2);

  if ( update_callback != 0 ) {
    pwm_update_callback = update_callback;
    nvic_enable_irq(NVIC_TIM2_IRQ);
    timer_enable_irq(TIM2, TIM_DIER_UIE);
  }
}

static void pwm_gpio_init(void) {
  rcc_periph_clock_enable(RCC_GPIOA);
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO0 | GPIO1 );
  gpio_set_af(GPIOA, GPIO_AF2, GPIO0);
  gpio_set_af(GPIOA, GPIO_AF2, GPIO1);
}

void pwm_init(pwm_callback update_callback) {
  pwm_timer_init(update_callback);
  pwm_gpio_init();
}

void pwm_set(uint16_t value) {
  timer_set_oc_value(TIM2, TIM_OC1, value);
  timer_set_oc_value(TIM2, TIM_OC2, value);
}

void tim2_isr(void)
{
  /* reset interrupt flag */
  if (timer_get_flag(TIM2, TIM_DIER_UIE)) {
    timer_clear_flag(TIM2, TIM_DIER_UIE);
  }

  /* call user callback */
  uint16_t next_value;
  pwm_update_callback(&next_value);
  pwm_set(next_value);
}
