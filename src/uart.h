/*
 *	(C) 2022 J. R. Sharp	
 *
 *	Released under MIT License
 * 	See LICENSE.txt for License Terms
 *
 *	uart.h: UART functions
 */
 
#pragma once

#include <stdio.h>

void	uart_initialize(void);
int	uart_write_char(char byte, FILE *stream); 
