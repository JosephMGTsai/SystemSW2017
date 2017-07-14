#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


uint64_t mult_uint32(uint32_t x, uint32_t y, uint32_t a, uint32_t b, uint64_t bit, uint64_t result)
{
    if (bit == 0)
        return result;

    uint32_t c = y & bit ? x : 0;
    uint32_t ab = a ^ b;

    uint32_t sum = ab ^ c;
    uint32_t carry = (a & b) | (ab & c);

    return mult_uint32(x, y, sum >> 1, carry, bit << 1, result | (sum & 1 ? bit : 0));
}


int main(void)
{
    for (int i = 0; i < 100; ++i) {
        uint32_t x = (rand() << 1) ^ rand();
        uint32_t y = (rand() << 1) ^ rand();
        uint64_t xy = mult_uint32(x, y, 0, 0, 1, 0);
        uint64_t expected = (uint64_t) x * (uint64_t) y;

        printf("0x%08x * 0x%08x = 0x%08x%08x (expected: 0x%08x%08x, equals: %s)\n", x, y, (uint32_t) (xy >> 32), (uint32_t) xy, (uint32_t) (expected >> 32), (uint32_t) expected, xy == expected ? "TRUE" : "FALSE");
    }

    return 0;
}
