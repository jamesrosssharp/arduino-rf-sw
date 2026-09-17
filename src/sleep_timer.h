/*
 *	(C) 2022 J. R. Sharp
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	sleep_timer.h : Sleep for a bounded duration in SLEEP_MODE_IDLE,
 *	woken by Timer1's output compare interrupt, with every other
 *	peripheral clock gated off via the Power Reduction Registers for
 *	the duration of the sleep.
 */

#pragma once

#include <stdint.h>

/*
 * Sleeps for approximately `ms` milliseconds (1..65535). Timer1 and
 * its interrupt are set up, used, and torn down internally -- do not
 * use Timer1 (OC1A/ICP1 etc.) for anything else in this program.
 */
void sleep_timer_sleep_ms(uint16_t ms);
