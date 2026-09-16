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

#define LED_PORT GPIO_PORTB
#define LED_PIN	 5

#define BS_PORT GPIO_PORTB
#define BS_PIN	 0

int main (void)
{
	uart_initialize();

	DEBUG("Hello world!\n");

	// Set up adc

    PRR     = 0x00;
	ADMUX   = 0x00;
	ADCSRB  = 0x00;
	DIDR0   = 0x01;
	ADCSRA  = 0xe7;
	
    volatile uint32_t count = 0;
    uint32_t accu = 0;
    uint32_t accu2 = 0;

    gpio_set_output(LED_PORT, LED_PIN);
    gpio_set_output(BS_PORT, BS_PIN);
	
    bool dir = false;

	while(1) 
	{
        
        //ADCSRA |= 0x40;

        //while (ADCSRA & 0x40) {
        //    printf("Waiting...\n");
        //}


        if (dir)
        {
            if (accu > 50000UL)
            {
                accu += ((uint32_t)ADC) << 5;
            }
            if (accu > 10000UL)
            {
                accu += ((uint32_t)ADC) << 3;
            } 
            else
            {    
                accu += ADC;
            }

            accu += accu2 & 0xfff;

            if (accu > 125000L)
            {
                gpio_set(LED_PORT, LED_PIN);
                _delay_ms(100);
                gpio_clear(LED_PORT, LED_PIN);
                //printf('Flipping like gflip');
                accu2 += ADC;
                dir = !dir;
	            ADMUX   = !ADMUX;
            }
        }
        else
        {
            if (accu < 10)
            {
                
                accu2 += ADC;
                dir = !dir;
	            ADMUX   = !ADMUX;
            }
            else
            {
                uint32_t incr = ADC + (accu2 & 0xfff);
                if (incr > accu) accu = 0;
                else if (accu < 1000)
                    accu -= incr >> 3;
                else if (accu < 100)
                    accu -= incr >> 5;
                else accu -= incr;
            }
        }


	    count = accu >> 2 + 1;

//        printf("%lx %x\n", &ADC, ADC);
//		printf("count=%llx\n", count);

        for (int32_t i = 0; i < count; i++)
            _delay_us(1);

		gpio_set(BS_PORT, BS_PIN);
        gpio_clear(BS_PORT, BS_PIN);
       	}
}
