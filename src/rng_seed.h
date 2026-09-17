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

#define RNG_SEED_WORDS 4

/* Samples per settling-delay batch in rng_seed_mix_adc_entropy(), and
 * the total across both batches (see there for why there are two). */
#define RNG_SEED_ADC_SAMPLES 4
#define RNG_SEED_ADC_TOTAL_SAMPLES (RNG_SEED_ADC_SAMPLES * 2)

/*
 * Must be called exactly once, before anything else writes to the
 * .rng_noinit pool, i.e. as the first line of main(). Folds the
 * pool's power-on SRAM noise down to a single 32-bit seed, then
 * overwrites the pool with the derived seed so a warm reset (e.g.
 * watchdog) still perturbs the next seed rather than reusing raw,
 * by-then-stale RAM content.
 *
 * If raw_words_out is non-NULL, the pool's raw (pre-mix) words are
 * copied there first -- e.g. for a caller that wants to log them for
 * debugging. Pass NULL to skip that.
 */
uint32_t rng_seed_from_noinit(uint32_t raw_words_out[RNG_SEED_WORDS]);

/*
 * Folds extra entropy into `seed` from PORTC pin 5 (ADC5), left
 * floating, sampled against the external voltage reference (AREF
 * pin, REFS1:REFS0 = 00): first RNG_SEED_ADC_SAMPLES back-to-back
 * conversions, then RNG_SEED_ADC_SAMPLES more spaced out by a
 * settling delay, so the second batch picks up different transient
 * noise on the floating pin than the first. Returns the updated
 * seed.
 *
 * If raw_samples_out is non-NULL, the raw ADC readings (batch 1
 * followed by batch 2) are copied there -- e.g. for a caller that
 * wants to log them for debugging. Pass NULL to skip that.
 */
uint32_t rng_seed_mix_adc_entropy(uint32_t seed, uint16_t raw_samples_out[RNG_SEED_ADC_TOTAL_SAMPLES]);
