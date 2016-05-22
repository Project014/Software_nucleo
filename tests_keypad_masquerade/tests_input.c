
#include "clock.h"
#include "pwm.h"
#include "led.h"



void pwm_update_callback(uint16_t* buff) {
  static uint16_t cnt = 0;
  const uint16_t cnt_max = 124;
  cnt++;
  if ( cnt > cnt_max) {
    cnt = 0;
  }
  *buff = cnt;
}

int main(void)
{
  clock_init();
  time_init();
  pwm_init(0);
  led_green_init();

  while (1) {
    time_delay_ms(500);
    led_green_toggle();
  }

  return 0;
}

