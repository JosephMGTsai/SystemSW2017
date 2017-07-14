#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


/* ==== Original function
uint32_t func(uint32_t x) {
    uint32_t n = x;
    n = ((n & 0xffff0000) >> 16) | ((n & 0x0000ffff) << 16);
    n = ((n & 0xff00ff00) >>  8) | ((n & 0x00ff00ff) <<  8);
    n = ((n & 0xf0f0f0f0) >>  4) | ((n & 0x0f0f0f0f) <<  4);
    n = ((n & 0xcccccccc) >>  2) | ((n & 0x33333333) <<  2);
    n = ((n & 0xaaaaaaaa) >>  1) | ((n & 0x55555555) <<  1);
    return n;
}
*/


uint32_t func(uint32_t x) {
    uint32_t r = 1, n = 0;
    for (; r; r <<= 1)
        n = (n << 1) | !!(x & r);
    return n;
}


int main(void) {
    for (int i = 0; i < 10; ++i) {
        uint32_t x = rand();
        uint32_t n = func(x);
        printf("%10u 0x%08x => %10u 0x%08x\n", x, x, n, n);
    }
    return 0;
}
