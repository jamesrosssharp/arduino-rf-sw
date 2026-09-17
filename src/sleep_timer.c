/*
 *	(C) 2022 J. R. Sharp
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	sleep_timer.c : Sleep for a bounded duration in SLEEP_MODE_IDLE,
 *	woken by Timer1's output compare interrupt, with every other
 *	peripheral clock gated off via the Power Reduction Registers for
 *	the duration of the sleep.
 *
 *	SLEEP_MODE_IDLE (rather than a deeper mode) is used because it is
 *	the only sleep mode that keeps the I/O clock running, which
 *	Timer1 needs in order to keep counting and raise the wake-up
 *	interrupt; deeper modes would stop Timer1 too.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "sleep_timer.h"

/* Empty on purpose: the compare-match interrupt firing is what wakes
 * the CPU out of SLEEP_MODE_IDLE, nothing further needs to happen. */
ISR(TIMER1_COMPA_vect)
{
}

void sleep_timer_sleep_ms(uint16_t ms)
{
	uint8_t saved_prr0, saved_prr1;
	uint32_t ticks = ((uint32_t) ms * (F_CPU / 1024UL)) / 1000UL;

	if (ticks == 0) {
		ticks = 1;
	} else if (ticks > 0xFFFFUL) {
		ticks = 0xFFFFUL;
	}

	/* Timer1: CTC mode (TOP = OCR1A), /1024 prescaler. */
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);
	OCR1A = (uint16_t) ticks;
	TCNT1 = 0;
	TIFR1 = (1 << OCF1A);
	TIMSK1 = (1 << OCIE1A);

	saved_prr0 = PRR0;
	saved_prr1 = PRR1;

	/* Disable every peripheral clock except Timer1's. */
	PRR0 = (1 << PRADC) | (1 << PRUSART0) | (1 << PRSPI0)
		| (1 << PRUSART1) | (1 << PRTIM0) | (1 << PRTIM2)
		| (1 << PRTWI0);
	PRR1 = (1 << PRTIM3) | (1 << PRSPI1) | (1 << PRTIM4)
		| (1 << PRPTC) | (1 << PRTWI1);

	TCCR1B |= (1 << CS12) | (1 << CS10); /* starts the count */

	set_sleep_mode(SLEEP_MODE_IDLE);

	cli();
	sleep_enable();
	sei();
	sleep_cpu();
	sleep_disable();
	sei();

	/* Restore peripheral clocks and stop Timer1. */
	PRR0 = saved_prr0;
	PRR1 = saved_prr1;
	TCCR1B = 0;
	TIMSK1 = 0;
}
