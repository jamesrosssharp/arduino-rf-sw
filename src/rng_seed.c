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

#include <stddef.h>

#include <avr/io.h>
#include <util/delay.h>

#include "rng_seed.h"

#define RNG_SEED_POOL_WORDS RNG_SEED_WORDS

#define RNG_SEED_ADC_MUX 5 /* ADC5 = PORTC pin 5, left floating */

/* Batch 2's inter-sample delay in rng_seed_mix_adc_entropy(), chosen
 * to be well clear of batch 1's back-to-back timing so the two
 * batches sample different transient noise on the floating pin. */
#define RNG_SEED_ADC_BATCH2_DELAY_US 200

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

uint32_t rng_seed_from_noinit(uint32_t raw_words_out[RNG_SEED_WORDS])
{
	uint32_t seed = 0;
	uint8_t i;

	for (i = 0; i < RNG_SEED_POOL_WORDS; i++) {
		if (raw_words_out != NULL) {
			raw_words_out[i] = rng_seed_pool[i];
		}
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

static uint16_t rng_seed_adc_sample(void)
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC)) {
	}
	return ADC;
}

uint32_t rng_seed_mix_adc_entropy(uint32_t seed, uint16_t raw_samples_out[RNG_SEED_ADC_TOTAL_SAMPLES])
{
	uint8_t i;

	/* REFS1:REFS0 = 00 -> external AREF reference; MUX3:0 -> ADC5. */
	ADMUX = RNG_SEED_ADC_MUX;

	/* Cut leakage through the digital input buffer on the pin while
	 * it's used as an analog input. */
	DIDR0 |= (1 << ADC5D);

	/* /8 prescaler: 1 MHz / 8 = 125 kHz, inside the 50-200 kHz range
	 * the datasheet specifies for full 10-bit accuracy. */
	ADCSRA = (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0);

	/* Discard one conversion: the datasheet notes the first result
	 * after switching the reference/mux can be inaccurate. */
	(void) rng_seed_adc_sample();

	/* Batch 1: back-to-back conversions, so consecutive samples
	 * differ only by whatever noise the floating pin picks up
	 * between one conversion finishing and the next starting. */
	for (i = 0; i < RNG_SEED_ADC_SAMPLES; i++) {
		uint16_t sample = rng_seed_adc_sample();

		if (raw_samples_out != NULL) {
			raw_samples_out[i] = sample;
		}
		seed ^= rng_seed_mix((uint32_t) sample ^ ((uint32_t) i << 24) ^ UINT32_C(0xa5a5a5a5));
	}

	/* Batch 2: same pin/reference, but with a settling delay between
	 * conversions so the floating node drifts further and picks up
	 * different transient noise than batch 1. */
	for (i = 0; i < RNG_SEED_ADC_SAMPLES; i++) {
		uint16_t sample;

		_delay_us(RNG_SEED_ADC_BATCH2_DELAY_US);
		sample = rng_seed_adc_sample();

		if (raw_samples_out != NULL) {
			raw_samples_out[RNG_SEED_ADC_SAMPLES + i] = sample;
		}
		seed ^= rng_seed_mix((uint32_t) sample ^ ((uint32_t) i << 24) ^ UINT32_C(0x5a5a5a5a));
	}

	ADCSRA = 0; /* power the ADC back down until next use */

	if (seed == 0) {
		seed = UINT32_C(0x2545f491);
	}

	return seed;
}
