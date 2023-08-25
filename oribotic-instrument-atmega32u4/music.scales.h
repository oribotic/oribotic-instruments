#include <Arduino.h>

#ifndef MUSIC_SCALES_H
#define MUSIC_SCALES_H

#define MAXSCALE 8 // max allowed scale index

struct scaleTones
{
  char title[5];
  byte tones;
};

extern uint8_t rootNote;
extern uint8_t scale[8];

extern struct scaleTones availableScales[8];
void generateMidiScale (uint8_t index, uint8_t root);
uint8_t changeScale (uint8_t value); // returns new value if set

#endif // MUSIC_SCALES_H