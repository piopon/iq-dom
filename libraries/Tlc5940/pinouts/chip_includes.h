/*  
 * Copyright (c) 2014 
 * PONIK
 */

#ifndef TLC_CHIP_INCLUDES_H
#define TLC_CHIP_INCLUDES_H
/** \file
    Includes the chip-specfic defaults and pin definitions. */
#include <avr/io.h>
#ifndef PB0
#define PB0     PORTB0
#define PB1     PORTB1
#define PB2     PORTB2
#define PB3     PORTB3
#define PB4     PORTB4
#define PB5     PORTB5
#define PB6     PORTB6
#define PB7     PORTB7
#endif
/* Chip Specific Pinouts */
#if defined (__AVR_ATmega32U4__)
/* Leonardo */
#include "Arduino_Leonardo.h"
#else
#error "Unknown Chip!"
#endif
#endif