
#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

void clock_init(void);
void time_init(void);
uint32_t time_delay_ms(unsigned int delay);
uint32_t time_get_tick_ms(void);

#endif /* CLOCK_H */
