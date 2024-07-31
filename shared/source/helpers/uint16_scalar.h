/*
 * Copyright (C) 2018-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "shared/source/helpers/aligned_memory.h"
#include "shared/source/helpers/debug_helpers.h"

#include <cstdint>

namespace NEO {

struct uint16x8_t { // NOLINT(readability-identifier-naming)
    enum { numChannels = 8 };

    uint16_t value[8];

    uint16x8_t() {
	for (int i = 0; i < 8; i++)
		value[i] = 0;
    }

    uint16x8_t(const uint16x8_t &rhs) {
	    for (int i = 0; i < 8; i++) {
		    value[i] = rhs.value[i];
	    }
    }

    uint16x8_t(uint16_t a) {
	    for (int i = 0; i < 8; i++)
		    value[i] = a;
    }

    explicit uint16x8_t(const void *alignedPtr) {
	for (int i = 0; i < 8; i++)
		value[i] = reinterpret_cast<const uint16_t*>(alignedPtr)[i];
    }

    void operator=(const uint16x8_t &rhs) {
	    for (int i = 0; i < 8; i++)
		    value[i] = rhs.value[i];
    }

    inline uint16_t get(unsigned int element) {
        DEBUG_BREAK_IF(element >= numChannels);
        return value[element];
    }

    static inline uint16x8_t zero() {
        return uint16x8_t(static_cast<uint16_t>(0u));
    }

    static inline uint16x8_t one() {
        return uint16x8_t(static_cast<uint16_t>(1u));
    }

    static inline uint16x8_t mask() {
        return uint16x8_t(static_cast<uint16_t>(0xffffu));
    }

    inline void load(const void *alignedPtr) {
        DEBUG_BREAK_IF(!isAligned<16>(alignedPtr));
	for (int i = 0; i < 8; i++)
		value[i] = reinterpret_cast<const uint16_t *>(alignedPtr)[i];
    }

    inline void loadUnaligned(const void *ptr) {
	for (int i = 0; i < 8; i++)
		value[i] = reinterpret_cast<const uint16_t *>(ptr)[i];
    }

    inline void store(void *alignedPtr) {
        DEBUG_BREAK_IF(!isAligned<16>(alignedPtr));
	for (int i = 0; i < 8; i++)
		reinterpret_cast<uint16_t *>(alignedPtr)[i] = value[i];
    }

    inline void storeUnaligned(void *ptr) {
	for (int i = 0; i < 8; i++)
		reinterpret_cast<uint16_t *>(ptr)[i] = value[i];
    }

    inline operator bool() const {
	    for (int i = 0; i < 8; i++)
		if ((value[i] & mask().value[i])) {
			// PRINT_DEBUG_STRING(NEO::debugManager.flags.PrintDebugMessages.get(), stderr, "##(%08x)", value[i]);
			return 1;
		}
	    return 0;
    }

    inline uint16x8_t &operator-=(const uint16x8_t &a) {
	    for (int i = 0; i < 8; i++)
		    value[i] -= a.value[i];
        return *this;
    }

    inline uint16x8_t &operator+=(const uint16x8_t &a) {
	    for (int i = 0; i < 8; i++)
		    value[i] += a.value[i];
        return *this;
    }

    inline friend uint16x8_t operator>=(const uint16x8_t &a, const uint16x8_t &b) {
        uint16x8_t result;
	for (int i = 0; i < 8; i++)
		if (a.value[i] > b.value[i])
			result.value[i] = 0xffffu;
		else
			result.value[i] = 0;
#if 0
        result.value =
            _mm_xor_si128(mask().value,
                          _mm_cmplt_epi16(a.value, b.value)); // SSE2
#endif
        return result;
    }

    inline friend uint16x8_t operator&&(const uint16x8_t &a, const uint16x8_t &b) {
        uint16x8_t result;
	for (int i = 0; i < 8; i++)
		result.value[i] = a.value[i] & b.value[i];
        // result.value = _mm_and_si128(a.value, b.value); // SSE2
        return result;
    }

    // NOTE: uint16x8_t::blend behaves like mask ? a : b
    inline friend uint16x8_t blend(const uint16x8_t &a, const uint16x8_t &b, const uint16x8_t &mask) {
        uint16x8_t result;

#if 0
        // Have to swap arguments to get intended calling semantics
        result.value =
            _mm_blendv_epi8(b.value, a.value, mask.value); // SSE4.1 alternatives?
#endif

#define HIGH_8BITS(x) ((x >> 8) & 0xff)
#define LOW_8BITS(x) (x & 0xff)
	
	for (int i = 0; i < 8; i++) {
		// we need to treat the uint16 to two uint8.
		uint8_t v1, v2;

		if (HIGH_8BITS(mask.value[i]) == 0xffu)
			v1 = HIGH_8BITS(a.value[i]);
		else
			v1 = HIGH_8BITS(b.value[i]);

		if (LOW_8BITS(mask.value[i]) == 0xffu)
			v2 = LOW_8BITS(a.value[i]);
		else
			v2 = LOW_8BITS(b.value[i]);

		result.value[i] = ((uint16_t)v1 << 8) | v2;
    // PRINT_DEBUG_STRING(NEO::debugManager.flags.PrintDebugMessages.get(), stderr, "%08x ", result.value[i]);
	}

  // PRINT_DEBUG_STRING(NEO::debugManager.flags.PrintDebugMessages.get(), stderr, "\n");
        return result;
    }
};
} // namespace NEO
