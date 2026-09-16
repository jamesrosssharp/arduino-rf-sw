/*
 *	(C) 2022 J. R. Sharp	
 *
 *	Released under MIT License
 *
 *	See LICENSE.txt for License Terms
 *
 *	gpio.c : GPIO driver
 *
 */

#include "gpio.h"

void gpio_set_output(uint8_t port, uint8_t pin)
{
	switch (port)
	{
		case GPIO_PORTB:
			DDRB |= (1<<pin);
			break;
		case GPIO_PORTC:
			DDRC |= (1<<pin);
			break;
		case GPIO_PORTD:
			DDRD |= (1<<pin);
			break;
	} 
}

void gpio_set_input(uint8_t port, uint8_t pin)
{
	switch (port)
	{
		case GPIO_PORTB:
			DDRB &= ~(1<<pin);
			break;
		case GPIO_PORTC:
			DDRC &= ~(1<<pin);
			break;
		case GPIO_PORTD:
			DDRD &= ~(1<<pin);
			break;
	} 
}

void gpio_set(uint8_t port, uint8_t pin)
{
	switch (port)
	{
		case GPIO_PORTB:
			PORTB |= (1<<pin);
			break;
		case GPIO_PORTC:
			PORTC |= (1<<pin);
			break;
		case GPIO_PORTD:
			PORTD |= (1<<pin);
			break;
	} 
}

void gpio_clear(uint8_t port, uint8_t pin)
{
	switch (port)
	{
		case GPIO_PORTB:
			PORTB &= ~(1<<pin);
			break;
		case GPIO_PORTC:
			PORTC &= ~(1<<pin);
			break;
		case GPIO_PORTD:
			PORTD &= ~(1<<pin);
			break;
	} 
}

int gpio_get(uint8_t port, uint8_t pin)
{
	switch (port)
	{
		case GPIO_PORTB:
			return (PINB & (1<<pin));
			break;
		case GPIO_PORTC:
			return (PORTC & (1<<pin));
			break;
		case GPIO_PORTD:
			return (PORTD & (1<<pin));
			break;
	} 
	return 0;
}



