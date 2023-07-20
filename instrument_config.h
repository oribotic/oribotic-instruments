// only switch one on at a time
#ifndef INSTRUMENT_CONFIG_H
#define INSTRUMENT_CONFIG_H

#define MIDI 1
#define OSC 0

#if MIDI
#define INTERVALDELAY 50
#endif
#if OSC 
#define INTERVALDELAY 10
#endif 

// soft lo offset is the reading below bendLO that soft starts working
// for PCBS SOFT_LOW_OFFSET 50, LO_HARD_OFFSET 100

#define DEBUG_LEVEL 2       // 0 - 3 
#define KRESLING 1
#define YOSHIMURA 2
#define SUKI 3
#define ORIGAMI YOSHIMURA

#define ROOTNOTE 60

#define SETUP_FUNCTIONS 0

#define MODE_DEFAULT 5
#define MODE_SERIAL_DEBUG 0 // useful only for serial output checks
#define MODE_SERIAL_DEBUG_RAW 1 // useful only for serial output checks
#define MODE_RAW 2 // works for midi and osc
#define MODE_NORMALISED 3 // works for midi and osc
#define MODE_RAW_PLUS_NORMALISED 4 // only works on osc 
#define MODE_TOUCH_PLAY 5
// #define MODE_TOUCH_PLAY 6
//#define MODE_SERIAL_DEBUG_BYTES 5 // no longer used 

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


