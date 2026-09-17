/*
 *	(C) 2022 J. R. Sharp
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	tinymt32.c : TinyMT32 pseudo-random number generator.
 *
 *	See tinymt32.h for provenance. This file fixes the generator to
 *	the reference "parameter set 0" (mat1/mat2/tmat below), which is
 *	the example set shipped with the original TinyMT distribution.
 */

#include "tinymt32.h"

#define TINYMT32_SH0  1
#define TINYMT32_SH1  10
#define TINYMT32_SH8  8
#define TINYMT32_MASK UINT32_C(0x7fffffff)

#define TINYMT32_MAT1 UINT32_C(0x8f7011ee)
#define TINYMT32_MAT2 UINT32_C(0xfc78ff1f)
#define TINYMT32_TMAT UINT32_C(0x3793fdff)

#define TINYMT32_MIN_LOOP 8
#define TINYMT32_PRE_LOOP 8

static void tinymt32_next_state(tinymt32_t *random)
{
	uint32_t x;
	uint32_t y;

	y = random->status[3];
	x = (random->status[0] & TINYMT32_MASK)
		^ random->status[1]
		^ random->status[2];
	x ^= (x << TINYMT32_SH0);
	y ^= (y >> TINYMT32_SH0) ^ x;

	random->status[0] = random->status[1];
	random->status[1] = random->status[2];
	random->status[2] = x ^ (y << TINYMT32_SH1);
	random->status[3] = y;

	random->status[1] ^= -((int32_t) (y & 1)) & TINYMT32_MAT1;
	random->status[2] ^= -((int32_t) (y & 1)) & TINYMT32_MAT2;
}

static uint32_t tinymt32_temper(tinymt32_t *random)
{
	uint32_t t0, t1;

	t0 = random->status[3];
	t1 = random->status[0] + (random->status[2] >> TINYMT32_SH8);
	t0 ^= t1;
	t0 ^= -((int32_t) (t1 & 1)) & TINYMT32_TMAT;

	return t0;
}

uint32_t tinymt32_generate_uint32(tinymt32_t *random)
{
	tinymt32_next_state(random);
	return tinymt32_temper(random);
}

/* Avoid the single all-zero state, which is a fixed point of the
 * recurrence and would produce an all-zero stream forever. */
static void tinymt32_period_certification(tinymt32_t *random)
{
	if ((random->status[0] & TINYMT32_MASK) == 0 &&
		random->status[1] == 0 &&
		random->status[2] == 0 &&
		random->status[3] == 0) {
		random->status[0] = 'T';
		random->status[1] = 'I';
		random->status[2] = 'N';
		random->status[3] = 'Y';
	}
}

void tinymt32_init(tinymt32_t *random, uint32_t seed)
{
	uint8_t i;

	random->status[0] = seed;
	random->status[1] = TINYMT32_MAT1;
	random->status[2] = TINYMT32_MAT2;
	random->status[3] = TINYMT32_TMAT;

	for (i = 1; i < TINYMT32_MIN_LOOP; i++) {
		random->status[i & 3] ^= i + UINT32_C(1812433253)
			* (random->status[(i - 1) & 3]
				^ (random->status[(i - 1) & 3] >> 30));
	}

	tinymt32_period_certification(random);

	for (i = 0; i < TINYMT32_PRE_LOOP; i++) {
		tinymt32_next_state(random);
	}
}
