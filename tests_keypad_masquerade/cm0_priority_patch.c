
#include <libopencm3/cm3/nvic.h>
#include <libopencmsis/core_cm3.h>

#include "cm0_priority_patch.h"

/* 32 bit regiters only */ 
#define NVIC_IPR_CM0(ipr_id) MMIO32(NVIC_BASE + 0x300 + (ipr_id)*4)

// priority on 2 bits, 0 to 3
void nvic_set_priority_cm0(uint8_t irqn, uint8_t priority) {
  priority = priority << 6;
  uint8_t ipr_index = irqn >> 2;
  uint8_t ipr_shift = irqn & 0b11;
  uint32_t ipr_mask = 0xff << (ipr_shift*8);
  NVIC_IPR_CM0(ipr_index) =
    (NVIC_IPR(ipr_index) & ~ipr_mask) | ( priority << (ipr_shift*8) );
}
