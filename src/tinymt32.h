/*
 *	(C) 2022 J. R. Sharp
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	tinymt32.h : TinyMT32 pseudo-random number generator.
 *
 *	Adapted for 8-bit AVR targets from the reference TinyMT32
 *	implementation by Mutsuo Saito and Makoto Matsumoto
 *	(MIT licensed, http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/TINYMT/).
 *
 *	TinyMT32 keeps only 16 bytes of state (vs. 2.5 KB for MT19937),
 *	which fits an ATmega328PB's 2 KB SRAM comfortably. This build
 *	fixes the generator parameters to a single, well-tested stream
 *	(no per-instance mat1/mat2/tmat) since only one PRNG instance
 *	runs on this MCU.
 */

#pragma once

#include <stdint.h>

typedef struct {
	uint32_t status[4];
} tinymt32_t;

void tinymt32_init(tinymt32_t *random, uint32_t seed);
uint32_t tinymt32_generate_uint32(tinymt32_t *random);
