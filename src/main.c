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

#define BLINK_DELAY_MS 1000

#include <util/delay.h>

#include "uart.h"
#include "debug.h"
#include "gpio.h"

#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

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
	uart_initialize();

    gpio_set_output(LED1_PORT, LED1_PIN);
    gpio_set_output(LED2_PORT, LED2_PIN);
    gpio_set_output(LED3_PORT, LED3_PIN);
    gpio_set_output(CTL1_PORT, CTL1_PIN);
    gpio_set_output(CTL2_PORT, CTL2_PIN);


	DEBUG("Hello world!\n");

    int cnt = 0;

    while(1)
    {
        DEBUG(".");

        switch (cnt)
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

        cnt ++;
        cnt &= 3;
   
        _delay_us(100000);
    }

}
