/*
 * Copyright (c) 2023 Matthew Gardiner
 *
 * MIT License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/oribotic/oribotic-instruments for documentation
 *
 */
#ifndef INSTRUMENT_CONFIG_H
#define INSTRUMENT_CONFIG_H

#include <Arduino.h>

// only switch one on at a time
#define MIDI 1
#define OSC 0

#if MIDI
#define INTERVALDELAY 5
#endif
#if OSC 
#define INTERVALDELAY 10
#endif 

extern uint8_t intervaldelay;

// soft lo offset is the reading below bendLO that soft starts working
// for PCBS SOFT_LOW_OFFSET 50, LO_HARD_OFFSET 100

#define DEBUG_LEVEL 2   // 0 - 3 
#define TEXTILE 0
#define KRESLING 1
#define YOSHIMURA 2
#define SUKI 3
#define ORIGAMI YOSHIMURA

#define SOFT_FILTER_LIMIT 200

#define ROOTNOTE 60

#define SETUP_FUNCTIONS 1

#define MODE_DEFAULT 5
#define MODE_SERIAL_DEBUG 0 // useful only for serial output checks
#define MODE_SERIAL_DEBUG_RAW 1 // useful only for serial output checks
#define MODE_RAW 2 // works for midi and osc
#define MODE_NORMALISED 3 // works for midi and osc
#define MODE_RAW_PLUS_NORMALISED 4 // only works on osc 
#define MODE_TOUCH_PLAY 5 // works on midi and osc - default mode
#define MODE_SOFT_VELOCITY 6 // works on midi and osc
//#define MODE_SERIAL_DEBUG_BYTES 5 // no longer used 
#define MAXMODE 6 // max allowed mode index

#if ORIGAMI==YOSHIMURA
    #include "instrument_Y8.h"
#endif
#if ORIGAMI==KRESLING
    #include "instrument_K48.h"
#endif  
#if ORIGAMI==SUKI
    #include "instrument_S12.h"
#endif  

#endif
