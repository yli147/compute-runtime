/*
 * Copyright (C) 2020-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/utilities/cpuintrinsics.h"

#if defined(_WIN32)
#include <immintrin.h>
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#elif defined(__riscv)
// do nothing
#else
#include <immintrin.h>
#include <x86intrin.h>
#endif

#if defined(__ARM_ARCH)
#include <sse2neon.h>
#elif defined(__riscv)
// do nothing
#else 
#include <emmintrin.h>
#endif

namespace NEO {
namespace CpuIntrinsics {

void clFlush(void const *ptr) {
	__asm__ __volatile__ ("fence.i");
	__asm__ __volatile__ ("sfence.vma %0" : : "r" (ptr));
}

void clFlushOpt(void *ptr) {
#ifdef SUPPORTS_CLFLUSHOPT
    _mm_clflushopt(ptr);
#else
    clFlush(ptr);
#endif
}

void sfence() {
    __asm__ __volatile__ ("fence w, w");
}

void pause() {
	__asm__ __volatile__ ("nop");
}

unsigned char umwait(unsigned int ctrl, uint64_t counter) {
#ifdef SUPPORTS_WAITPKG
    return _umwait(ctrl, counter);
#else
    return 0;
#endif
}

void umonitor(void *a) {
#ifdef SUPPORTS_WAITPKG
    _umonitor(a);
#endif
}

uint64_t rdtsc() {
	uint64_t result;
	__asm__ __volatile__ ("rdcycle %0" : "=r" (result));

	return result;
}

} // namespace CpuIntrinsics
} // namespace NEO
