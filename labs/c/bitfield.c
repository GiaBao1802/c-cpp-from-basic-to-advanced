#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

static bool set_field(uint32_t *reg, uint32_t mask, uint8_t shift, uint32_t value)
{
    if ((reg == 0) || (shift >= 32u) || (((value << shift) & ~mask) != 0u)) return false;
    *reg = (*reg & ~mask) | ((value << shift) & mask);
    return true;
}

int main(void)
{
    uint32_t reg = 0xA5A50000u;
    assert(set_field(&reg, 0x00000070u, 4u, 5u));
    assert(reg == 0xA5A50050u);
    assert(!set_field(&reg, 0x70u, 4u, 8u));
    return 0;
}
