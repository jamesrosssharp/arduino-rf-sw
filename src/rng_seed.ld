/*
 *	(C) 2022 J. R. Sharp
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	rng_seed.ld : Linker script fragment adding a dedicated, never
 *	zero-initialized RAM section used to seed the PRNG.
 *
 *	This does NOT replace avr-gcc's default atmega328pb ("avr5")
 *	linker script -- it is passed alongside the object files on the
 *	link line (see Makefile) and merged into the default script via
 *	INSERT AFTER, per the standard mechanism documented in the GNU ld
 *	manual for layering a small custom script on top of a target's
 *	built-in one.
 *
 *	avr-gcc's crt0 startup code zeroes .bss and copies .data from
 *	flash, but it deliberately leaves any NOLOAD section (including
 *	the generic ".noinit") untouched. That means whatever bit pattern
 *	SRAM happens to power up in survives into main() unmolested. On
 *	an AVR that pattern is not cryptographically random, but it is
 *	*not* a fixed value across the population or across resets
 *	either (die-to-die variation, temperature, supply ramp rate),
 *	which is exactly enough to stop every unit's TinyMT stream
 *	starting from the same fixed seed on first power-up.
 *
 *	A dedicated section (rather than avr-libc's shared ".noinit") is
 *	used so this pool can't silently collide with something else
 *	tagged __attribute__((section(".noinit"))) elsewhere in the
 *	program, and KEEP() protects it from -Wl,--gc-sections (which
 *	this project's Makefile enables).
 */

SECTIONS
{
	.rng_noinit (NOLOAD) : ALIGN(4)
	{
		KEEP(*(.rng_noinit))
	}
}
INSERT AFTER .noinit;
