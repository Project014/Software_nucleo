
#ifndef ROTARY_DIAL_H
#define ROTARY_DIAL_H

typedef void (*rotary_dial_callback_t)(char digit);

void rotary_dial_init(rotary_dial_callback_t on_get_digit_callback);

#endif /* ROTARY_DIAL_H */
