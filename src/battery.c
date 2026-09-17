/*
 *	(C) 2022 J. R. Sharp
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	battery.c : Battery-voltage sense via ADC3 (PORTC pin 3) against
 *	the internal 1.1V reference, through an external 10k/4.7k
 *	resistor divider enabled by PORTC pin 4.
 */

#include <avr/io.h>
#include <util/delay.h>

#include "battery.h"
#include "gpio.h"

#define BATTERY_ENABLE_PORT GPIO_PORTC
#define BATTERY_ENABLE_PIN  4

#define BATTERY_ADC_MUX 3 /* ADC3 = PORTC pin 3 */

/* atmega328pb internal band-gap reference, nominal. */
#define ADC_VREF_MV 1100UL

/* Sense divider: node = Vbat * 4700/(10000+4700). */
#define DIVIDER_LOW_OHMS  4700UL
#define DIVIDER_HIGH_OHMS 10000UL

void battery_enable(void)
{
	gpio_set_output(BATTERY_ENABLE_PORT, BATTERY_ENABLE_PIN);
	gpio_set(BATTERY_ENABLE_PORT, BATTERY_ENABLE_PIN);

	/* Let the divider node settle before it gets sampled. */
	_delay_ms(2);
}

void battery_disable(void)
{
	gpio_clear(BATTERY_ENABLE_PORT, BATTERY_ENABLE_PIN);
    gpio_set_input(BATTERY_ENABLE_PORT, BATTERY_ENABLE_PIN);
}

uint16_t battery_read_centivolts(void)
{
	uint16_t adc_value;
	uint32_t node_mv;
	uint32_t battery_mv;

	/* REFS1:REFS0 = 11 -> internal 1.1V reference; MUX3:0 -> ADC3. */
	ADMUX = (1 << REFS1) | (1 << REFS0) | BATTERY_ADC_MUX;

	/* Cut leakage through the digital input buffer on the pin while
	 * it's used as an analog input. */
	DIDR0 |= (1 << ADC3D);

	/* /8 prescaler: 1 MHz / 8 = 125 kHz, inside the 50-200 kHz range
	 * the datasheet specifies for full 10-bit accuracy. */
	ADCSRA = (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0);

	/* Discard one conversion: the datasheet notes the first result
	 * after switching the reference/mux can be inaccurate. */
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC)) {
	}

	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC)) {
	}
	adc_value = ADC;

	ADCSRA = 0; /* power the ADC back down until next use */

	node_mv = ((uint32_t) adc_value * ADC_VREF_MV) / 1024UL;
	battery_mv = node_mv * (DIVIDER_HIGH_OHMS + DIVIDER_LOW_OHMS) / DIVIDER_LOW_OHMS;

	return (uint16_t) ((battery_mv + 5) / 10); /* mV -> rounded centivolts */
}
