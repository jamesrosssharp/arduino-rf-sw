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
#include "battery.h"

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

/* Random sleep-between-patterns bounds, in milliseconds. */
#define SLEEP_MIN_MS 200
#define SLEEP_MAX_MS 1000

/* Power-up battery status indicator thresholds, in centivolts, and
 * how long to hold the LEDs lit for. */
#define BATTERY_3_LED_CV     300
#define BATTERY_2_LED_CV     270
#define BATTERY_1_LED_CV     250
#define BATTERY_INDICATOR_MS 2000

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

    uint16_t adc_seed_samples[RNG_SEED_ADC_TOTAL_SAMPLES];
    seed = rng_seed_mix_adc_entropy(seed, adc_seed_samples);

	DEBUG("rng adc entropy (PORTC5, ext vref):");
	for (uint8_t i = 0; i < RNG_SEED_ADC_TOTAL_SAMPLES; i++) {
		DEBUG(" %03x", adc_seed_samples[i]);
	}
	DEBUG(" -> seed %08lx\n", seed);

    tinymt32_t rng;
    tinymt32_init(&rng, seed);

    /* Power-up battery check: sample the sense divider, print the
     * result, and show it as a 0-3 LED bar for BATTERY_INDICATOR_MS. */
    battery_enable();
    _delay_us(100000);
    // Hack because hardware is faulty
    //uint16_t battery_cv = battery_read_centivolts();

    uint16_t battery_cv = 330;

    DEBUG("battery: %u.%02uV\n", battery_cv / 100, battery_cv % 100);

    uint8_t battery_leds;
    if (battery_cv > BATTERY_3_LED_CV) {
        battery_leds = 3;
    } else if (battery_cv > BATTERY_2_LED_CV) {
        battery_leds = 2;
    } else if (battery_cv > BATTERY_1_LED_CV) {
        battery_leds = 1;
    } else {
        battery_leds = 0;
    }

    if (battery_leds >= 1) {
        gpio_set(LED1_PORT, LED1_PIN);
    }
    if (battery_leds >= 2) {
        gpio_set(LED2_PORT, LED2_PIN);
    }
    if (battery_leds >= 3) {
        gpio_set(LED3_PORT, LED3_PIN);
    }

    sleep_timer_sleep_ms(BATTERY_INDICATOR_MS);

    gpio_clear(LED1_PORT, LED1_PIN);
    gpio_clear(LED2_PORT, LED2_PIN);
    gpio_clear(LED3_PORT, LED3_PIN);
    battery_disable();
    gpio_set_input(LED1_PORT, LED1_PIN);
    gpio_set_input(LED2_PORT, LED2_PIN);
    gpio_set_input(LED3_PORT, LED3_PIN);
 
    while(1)
    {
        //uint8_t pattern = tinymt32_generate_uint32(&rng) & 3;

        uint32_t randm = tinymt32_generate_uint32(&rng);

        DEBUG("%08lx\n", randm);
        _delay_us(1000000);


        uint8_t loops = randm & 0x3;
        uint32_t r = randm >> 2;

        for (uint8_t i = 0; i < loops + 1; i++)
        {
            uint8_t pattern = r & 0x3;

            switch (pattern)
            {
                case 0:
            //    gpio_clear(LED1_PORT, LED1_PIN);
            //    gpio_clear(LED2_PORT, LED2_PIN);
            //    gpio_clear(LED3_PORT, LED3_PIN);
                gpio_clear(CTL1_PORT, CTL1_PIN);
                gpio_clear(CTL2_PORT, CTL2_PIN);
                break;
            case 1:
            //    gpio_set(LED1_PORT, LED1_PIN);
            //    gpio_clear(LED2_PORT, LED2_PIN);
            //    gpio_clear(LED3_PORT, LED3_PIN);
                gpio_set(CTL1_PORT, CTL1_PIN);
                gpio_clear(CTL2_PORT, CTL2_PIN);
                break;
            case 2:
            //    gpio_clear(LED1_PORT, LED1_PIN);
            //    gpio_set(LED2_PORT, LED2_PIN);
            //    gpio_clear(LED3_PORT, LED3_PIN);
                gpio_clear(CTL1_PORT, CTL1_PIN);
                gpio_set(CTL2_PORT, CTL2_PIN);
                break;
            case 3:
            //    gpio_clear(LED1_PORT, LED1_PIN);
            //    gpio_clear(LED2_PORT, LED2_PIN);
            //    gpio_set(LED3_PORT, LED3_PIN);
                gpio_set(CTL1_PORT, CTL1_PIN);
                gpio_set(CTL2_PORT, CTL2_PIN);
                break;
            }

            r >>= 2;

            sleep_timer_sleep_ms((r & 7) + 1);

            r >>= 3;

        }

        gpio_clear(CTL1_PORT, CTL1_PIN);
        gpio_clear(CTL2_PORT, CTL2_PIN);
            
        uint16_t sleep_range = SLEEP_MAX_MS - SLEEP_MIN_MS;
        uint16_t sleep_ms = SLEEP_MIN_MS
            + (uint16_t) (randm % sleep_range);

        sleep_timer_sleep_ms(sleep_ms);
    }

}
