#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


// Calculate x * y + a + b, with uint32_t inputs and results in uint64_t
uint64_t x_mul_y_add_a_add_b_uint32(uint32_t x, uint32_t y, uint32_t a, uint32_t b)
{
    if ((y | a | b) == 0)
        return 0;
    uint32_t c = y & 1 ? x : 0;
    uint32_t ab = a ^ b;
    uint32_t sum = ab ^ c;
    uint32_t carry = (a & b) | (ab & c);
    return (x_mul_y_add_a_add_b_uint32(x, y >> 1, sum >> 1, carry) << 1) | (sum & 1);
}


int main(void)
{
    for (int i = 0; i < 100; ++i) {
        uint32_t x = (rand() << 16) ^ rand();
        uint32_t y = (rand() << 16) ^ rand();
        uint64_t xy = x_mul_y_add_a_add_b_uint32(x, y, 0, 0);
        uint64_t expected = (uint64_t) x * (uint64_t) y;

        printf("0x%08x * 0x%08x = 0x%08x%08x (expected: 0x%08x%08x, equals: %s)\n", x, y, (uint32_t) (xy >> 32), (uint32_t) xy, (uint32_t) (expected >> 32), (uint32_t) expected, xy == expected ? "TRUE" : "FALSE");
    }

    return 0;
}
