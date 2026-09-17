/*
 *	(C) 2022 J. R. Sharp
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	battery.h : Battery-voltage sense via ADC3 (PORTC pin 3) against
 *	the internal 1.1V reference, through an external 10k/4.7k
 *	resistor divider enabled by PORTC pin 4.
 */

#pragma once

#include <stdint.h>

/* Drives PORTC pin 4 high, powering the sense divider, and waits
 * for the divider node to settle before returning. */
void battery_enable(void);

/* Drives PORTC pin 4 low, powering the sense divider back down. */
void battery_disable(void);

/*
 * Samples ADC3 (PORTC pin 3), inverts the board's
 * 4700/(10000+4700) divider ratio, and returns the battery voltage
 * in centivolts (hundredths of a volt), e.g. 312 for 3.12 V.
 * battery_enable() must be called first.
 */
uint16_t battery_read_centivolts(void);
