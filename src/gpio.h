/*
 *	(C) 2022 J. R. Sharp	
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	gpio.h : GPIO driver
 *
 */

#pragma once

#include <avr/io.h>
#include <stdint.h>

enum {
	GPIO_PORTB = 0,
	GPIO_PORTC = 1,
	GPIO_PORTD = 2,
};

void gpio_set_output(uint8_t port, uint8_t pin);
void gpio_set_input(uint8_t port, uint8_t pin);
void gpio_set(uint8_t port, uint8_t pin);
int  gpio_get(uint8_t port, uint8_t pin);
void gpio_clear(uint8_t port, uint8_t pin);

