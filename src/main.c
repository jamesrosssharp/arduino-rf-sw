/*
 *	(C) 2022 J. R. Sharp	
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	main.c : Program entry point
 *
 */

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "uart.h"
#include "debug.h"
#include "gpio.h"
#include "tinymt32.h"
#include "rng_seed.h"
#include "sleep_timer.h"

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

/* Random sleep-between-patterns bounds, in milliseconds. */
#define SLEEP_MIN_MS 200
#define SLEEP_MAX_MS 3000

#define LED1_PORT GPIO_PORTC
#define LED1_PIN	 0

#define LED2_PORT GPIO_PORTC
#define LED2_PIN	 1

#define LED3_PORT GPIO_PORTC
#define LED3_PIN	 2

#define CTL1_PORT GPIO_PORTE
#define CTL1_PIN	 3

#define CTL2_PORT GPIO_PORTE
#define CTL2_PIN	 2

int main (void)
{
    /* Must run before anything else touches the .rng_noinit pool
     * (see rng_seed.h). */
    uint32_t raw_seed_words[RNG_SEED_WORDS];
    uint32_t seed = rng_seed_from_noinit(raw_seed_words);
    tinymt32_t rng;
    tinymt32_init(&rng, seed);

	uart_initialize();

    gpio_set_output(LED1_PORT, LED1_PIN);
    gpio_set_output(LED2_PORT, LED2_PIN);
    gpio_set_output(LED3_PORT, LED3_PIN);
    gpio_set_output(CTL1_PORT, CTL1_PIN);
    gpio_set_output(CTL2_PORT, CTL2_PIN);


	DEBUG("Hello world!\n");
	DEBUG("rng seed words:");
	for (uint8_t i = 0; i < RNG_SEED_WORDS; i++) {
		DEBUG(" %08lx", raw_seed_words[i]);
	}
	DEBUG(" -> seed %08lx\n", seed);

    while(1)
    {
        uint8_t pattern = tinymt32_generate_uint32(&rng) & 3;

        DEBUG(".");

        switch (pattern)
        {
            case 0:
                gpio_clear(LED1_PORT, LED1_PIN);
                gpio_clear(LED2_PORT, LED2_PIN);
                gpio_clear(LED3_PORT, LED3_PIN);
                gpio_clear(CTL1_PORT, CTL1_PIN);
                gpio_clear(CTL2_PORT, CTL2_PIN);
                break;
            case 1:
                gpio_set(LED1_PORT, LED1_PIN);
                gpio_clear(LED2_PORT, LED2_PIN);
                gpio_clear(LED3_PORT, LED3_PIN);
                gpio_set(CTL1_PORT, CTL1_PIN);
                gpio_clear(CTL2_PORT, CTL2_PIN);
                break;
            case 2:
                gpio_clear(LED1_PORT, LED1_PIN);
                gpio_set(LED2_PORT, LED2_PIN);
                gpio_clear(LED3_PORT, LED3_PIN);
                gpio_clear(CTL1_PORT, CTL1_PIN);
                gpio_set(CTL2_PORT, CTL2_PIN);
                break;
            case 3:
                gpio_clear(LED1_PORT, LED1_PIN);
                gpio_clear(LED2_PORT, LED2_PIN);
                gpio_set(LED3_PORT, LED3_PIN);
                gpio_set(CTL1_PORT, CTL1_PIN);
                gpio_set(CTL2_PORT, CTL2_PIN);
                break;
        }

        uint16_t sleep_range = SLEEP_MAX_MS - SLEEP_MIN_MS;
        uint16_t sleep_ms = SLEEP_MIN_MS
            + (uint16_t) (tinymt32_generate_uint32(&rng) % sleep_range);

        sleep_timer_sleep_ms(sleep_ms);
    }

}
