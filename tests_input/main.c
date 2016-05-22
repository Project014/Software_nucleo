
#include "clock.h"
#include "pwm.h"
#include "led.h"




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

