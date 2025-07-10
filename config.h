#pragma once
/*
 * Global configuration file for the program
 */

#include <bitset>
#include <cstdint>

// Bitsize is defined as a constant because we only want one bitsize per
// compilation, since it is optimized for the number of bits of the
// architecture.
//
// If one wants to speed up computations, setting a maximun region depth is
// better than changing the bitsize, since it makes things cleaner and
// performance isn't affected.

#if INTPTR_MAX == INT32_MAX
static const int NUMBITS = 32;
#elif INTPTR_MAX == INT64_MAX
static const int NUMBITS = 64;
#else
#error The program wont support this ISA
#endif

static const std::bitset<NUMBITS> ALLONES = (uint64_t)-1 >> (64 - NUMBITS);
static const std::bitset<NUMBITS> BIGONE = 1LL << (NUMBITS - 1);
static const std::bitset<NUMBITS> SMALLONE = 1LL;
