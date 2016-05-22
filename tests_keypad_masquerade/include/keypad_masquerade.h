
#include <stdint.h>

void keypad_masquerade_init(void);
void keypad_masquerade_press_key(uint8_t line, uint8_t column);
void keypad_masquerade_release_key(void);
void keypad_masquerade_toggle_key(uint8_t line, uint8_t column);

