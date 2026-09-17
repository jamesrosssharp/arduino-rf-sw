/*
 *	(C) 2022 J. R. Sharp
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	rng_seed.h : Derives a PRNG seed from the never-initialized
 *	.rng_noinit RAM pool (see rng_seed.ld).
 */

#pragma once

#include <stdint.h>

/*
 * Must be called exactly once, before anything else writes to the
 * .rng_noinit pool, i.e. as the first line of main(). Folds the
 * pool's power-on SRAM noise down to a single 32-bit seed, then
 * overwrites the pool with the derived seed so a warm reset (e.g.
 * watchdog) still perturbs the next seed rather than reusing raw,
 * by-then-stale RAM content.
 */
uint32_t rng_seed_from_noinit(void);
