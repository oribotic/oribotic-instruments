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
#ifndef ORICORD_H
#define ORICORD_H

#include <stdint.h>
#include <Arduino.h>
#include "src/MPR121/MPR121.h"
#include <Wire.h>
#include "instrument_config.h"

// --------------------------------------------------------------------    BUTTONS

struct pressbuttons
{
  byte pin;
  byte led;
  byte state;
};

extern struct pressbuttons buttons[];

// --------------------------------------------------------------------    PANELS

struct panels
{
  uint8_t addr;
  MPR121_t *mpr;
  uint8_t reversed;
  uint8_t electrodes;
};

extern struct panels MPRpanels[];

// --------------------------------------------------------------------    ORIKEY

struct orikey
{
  uint8_t pin;        // identify the pin
  uint8_t panel;      // idenfity the panel
  uint8_t note;       // note to assoticate
  uint16_t hard;      // hard level
  uint16_t soft;      // hard level
  uint16_t bendLO;
  uint16_t bendHI;
  uint8_t state;
  uint16_t last;
};

extern struct orikey orikeys[];

// class oricord_t
// {
//     public:
//         oricord_t();
    
//         bool begin ();
//         void write ();
//         void read ();
    
//     // oricordion data
//         // split into seperate files
//         //
//     // oricordion functions
//         // read
//         // write    - midi or osc
//         // comms    - midi or osc
// };

extern uint8_t mode;  // = 6;
extern byte activebutton;  // = 0;
extern bool continuousMode;

#if OSC == 1
  extern uint8_t bendLinear[256];
#endif
#if MIDI == 1
  extern uint8_t bendLinear[128]; 
#endif

extern byte reverse_logical[];

extern MPR121_t MPR121_0;
extern MPR121_t MPR121_1;
extern MPR121_t MPR121_2;
extern MPR121_t MPR121_3;
#if MPR_COUNT > 1
  extern MPR121_t MPR121_1;
#endif
#if MPR_COUNT > 2
  extern MPR121_t MPR121_2;
#endif
#if MPR_COUNT > 3
  extern MPR121_t MPR121_3;
#endif

void doCalibrate();

// main read write functions
void raw (uint8_t key);
void play(uint8_t key);
void soft(uint8_t key);
void touchPlay(uint8_t key);
uint8_t getActionState (uint8_t key, uint16_t filtered);
uint16_t mapSoft(uint8_t key, uint16_t filtered);

#if OSC == 1
void send(char channel[1], uint8_t key, uint8_t note, uint16_t state );
#endif

#if MIDI == 1
void send(char channel[1], uint8_t key, uint8_t note, uint16_t state);
#endif 

void setBendAllKeys(char param[4]);
void setBendSingleKey(uint8_t key, char param[4]);
void changeRoot(boolean direction);
void reportMPRStatus(char channel[20], uint8_t value);
void setScale(uint8_t value);
void serialDebug (byte key, byte raw);
void debug_short(uint8_t n, uint8_t f);
void debug_short_char(char f);
void debug_short_filtered(char f);
void debug_raw(uint16_t f);
void debug_raw_bytes(uint8_t f);
void checkbuttons();
void setupMPR();
uint8_t bendDebug(int filtered, int lo, int hi);
long longmap(long x, long in_min, long in_max, long out_min, long out_max);

#endif ORICORD_H
