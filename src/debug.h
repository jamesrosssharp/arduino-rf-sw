/*
 *	(C) 2022 J. R. Sharp	
 *
 *	Released under MIT License
 * 	See LICENSE.txt for License Terms
 *
 *	debug.h: Debug functions
 */
 
#pragma once

#include <stdio.h>

#ifndef RELEASE
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif
