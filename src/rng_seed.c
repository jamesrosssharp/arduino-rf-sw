/*
 *	(C) 2022 J. R. Sharp
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	rng_seed.c : Derives a PRNG seed from the never-initialized
 *	.rng_noinit RAM pool (see rng_seed.ld).
 */

#include "rng_seed.h"

#define RNG_SEED_POOL_WORDS 4

/* Placed by src/rng_seed.ld into a dedicated NOLOAD section, so
 * crt0 never zeroes or otherwise writes to it before main() runs. */
static uint32_t rng_seed_pool[RNG_SEED_POOL_WORDS] __attribute__((section(".rng_noinit")));

/* 32-bit bit-mixer (Chris Wellons' "lowbias32"); just needs to
 * decorrelate whatever raw pattern SRAM powered up in, not be
 * cryptographically strong. */
static uint32_t rng_seed_mix(uint32_t x)
{
	x ^= x >> 16;
	x *= UINT32_C(0x7feb352d);
	x ^= x >> 15;
	x *= UINT32_C(0x846ca68b);
	x ^= x >> 16;
	return x;
}

uint32_t rng_seed_from_noinit(void)
{
	uint32_t seed = 0;
	uint8_t i;

	for (i = 0; i < RNG_SEED_POOL_WORDS; i++) {
		seed ^= rng_seed_mix(rng_seed_pool[i] ^ ((uint32_t) i << 24));
	}

	if (seed == 0) {
		/* Vanishingly unlikely, but TinyMT's own zero-guard is
		 * internal state, not the seed -- guard here too. */
		seed = UINT32_C(0x2545f491);
	}

	/* Overwrite so a watchdog/soft reset perturbs next boot's seed
	 * instead of replaying stale RAM content indefinitely. */
	for (i = 0; i < RNG_SEED_POOL_WORDS; i++) {
		rng_seed_pool[i] = seed + i;
	}

	return seed;
}
