
#include "clock.h"
#include "pwm.h"
#include "led.h"
#include "keypad_masquerade.h"
#include "rotary_dial.h"

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

void on_get_digit(char digit) {
  if ( !digit ) {
    return;
  }

  if ( digit == 10 ) {
    digit = 0;
  }

  combi_t combi = conv[digit];
  keypad_masquerade_toggle_key(combi.lin, combi.col);
}

int main(void)
{
  clock_init();
  time_init();
  pwm_init(0);
  led_green_init();
  keypad_masquerade_init();
  rotary_dial_init(on_get_digit);

  keypad_masquerade_toggle_key(0, 0); /* hp on */
  

  //keypad_masquerade_press_key(uint8_t line, uint8_t column);

  /* while (1) { */
  /*   time_delay_ms(100); */
  /*   keypad_masquerade_toggle_key(0, 1); */
  /*   time_delay_ms(100); */
  /*   keypad_masquerade_toggle_key(1, 2); */
    
  /* } */

  return 0;
}
