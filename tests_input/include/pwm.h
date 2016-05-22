
#ifndef PWM_H
#define PWM_H

#include <stdint.h>

typedef void (*pwm_callback)(uint16_t* next_value);

void pwm_init(pwm_callback update_callback);
void pwm_set(uint16_t value);

#endif /* PWM_H */
