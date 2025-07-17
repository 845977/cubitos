#pragma once
/*
 * file: config.h
 * description: Global configuration file for the program
 */

#include <bitset>
#include <cstdint>

// Bitsize is defined as a constant because we only want one bitsize per
// compilation, since it is optimized for the number of bits of the
// architecture. Only 32 and 64 bits are supported

#if INTPTR_MAX == INT32_MAX
static const int NUMBITS = 32;
#elif INTPTR_MAX == INT64_MAX
static const int NUMBITS = 64;
#else
#error The program wont support this ISA
#endif

// Bitmasks are defined according to NUMBITS
static const std::bitset<NUMBITS> ALLONES = (uint64_t)-1 >> (64 - NUMBITS);
static const std::bitset<NUMBITS> BIGONE = 1LL << (NUMBITS - 1);
static const std::bitset<NUMBITS> SMALLONE = 1LL;
