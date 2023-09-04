// used in single-number-ii

#include <stdio.h>
#include <assert.h>
typedef unsigned long long u64;
typedef unsigned int u32;
typedef u64 ternary32;

#define lo 0x5555555555555555ULL
#define hi (lo << 1ULL)
const u64 POW_3_IN_32 = 0x6954FE21E3E81ULL; // this is maximum representable value in ternary32

// ternary uses 2 bits for each digit
ternary32 binaryToTernary(u64 x) {
    assert(x <= POW_3_IN_32);
    ternary32 ret = 0;
    u64 pos = 0;
    while (x) {
        u64 digit = x % 3;
        x /= 3;
        ret |= (digit << pos);
        pos += 2;
    }
    return ret;
}

u64 ternaryToBinary(ternary32 x) {
    u64 ret = 0;
    u64 pow = 1;
    while (x) {
        u64 digit = x & 0b11;
        ret += digit * pow;
        x >>= 2ULL;
        pow *= 3ULL;
    }
    return ret;
}

// 10 -> 11; 00, 01 -> 00; 11 -> 11
u64 mask10(u64 x) {
    x &= (((x & hi) >> 1ULL) | hi);
    x |= ((x & hi) >> 1ULL);
    return x;
}

// 11 -> 11; others -> 00
u64 mask11(u64 x) {
    u64 zeroed_lo = x & ((x & hi) >> 1ULL); // 0# -> 00, 1# -> 0#; or in other words [x y] -> [0 (x & y)]
    u64 zeroed_hi = x & ((x & lo) << 1ULL); // #0 -> 00, #1 -> #0; or in other words [x y] -> [(x & y) 0]
    u64 ret = zeroed_hi | zeroed_lo;
    return ret;
}

// 11 -> 00; others to themself
u64 clear11(u64 x) {
    u64 mask = mask11(x);
    x &= ~mask;
    return x;
}

ternary32 ternaryDigitWiseSum(ternary32 x, ternary32 y) {
    u64 mask10x = mask10(x);
    u64 mask10y = mask10(y);
    u64 mask1010 = mask10x & mask10y;
    u64 sum_small = (x & ~mask1010) + (y & ~mask1010);
    u64 all01 = lo;
    u64 sum = mask1010 & all01;
    sum |= sum_small;
    ternary32 ret = clear11(sum);
    return ret;
}

int main() {
    for (u64 x = 0; x < 2; ++x) {
        for (u64 y = 0; y < 2; ++y) {
            u64 sum = ternaryDigitWiseSum(x, y);
            u64 expected = (x + y) % 3;
            if (sum != expected) {
                printf("%llu != %llu (%llu + %llu)\n", sum, expected, x, y);
                __debugbreak();
            }
        }
    }
    for (u32 i = 1; i != 0; ++i) {
        u64 ternary = binaryToTernary(i);
        u64 binary = ternaryToBinary(ternary);
        if (binary != i) {
            printf("%llu != %u\n", binary, i);
            __debugbreak();
        }
    }
    return 0;
}
