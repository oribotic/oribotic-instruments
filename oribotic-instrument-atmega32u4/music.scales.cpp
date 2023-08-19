#include "music.scales.h"
#include "instrument_config.h"

#include <Arduino.h>

uint8_t rootNote = 60;
uint8_t scaleIndex = 0;
uint8_t scale[8] = {0, 0, 0, 0, 0, 0, 0, 0};

struct scaleTones availableScales[8] = {
    {.title = "majo", .tones = 0b00111011},
    {.title = "mino", .tones = 0b01101101},
    {.title = "dori", .tones = 0b01011101},
    {.title = "phry", .tones = 0b01101110},
    {.title = "lydi", .tones = 0b00110111},
    {.title = "mixo", .tones = 0b01011011},
    {.title = "aeol", .tones = 0b01101101},
    {.title = "locr", .tones = 0b01110110}
};

void generateMidiScale (uint8_t index, uint8_t root)
{
    // rootnote and scaleIndex are global
    uint8_t bit;
    uint8_t note = root;
    scale[0] = root;

    bool tonalstep;
    uint8_t scaleMap;

    if (index > MAXSCALE)
    {
      return;
    }
    scaleMap = availableScales[index].tones;
    // remember 0b00000001 (the ONE is bit 0)
    // so we skip bit 0

    for (bit=0; bit<=7; bit++)
    {
        tonalstep = bitRead(scaleMap, bit);
        if (tonalstep == false)
        {
          note += 1;
        } 
        if (tonalstep == true)
        {
          note += 2;
        }
        scale[bit+1] = note;
    }
    scale[7] = root + 12;
}

uint8_t changeScale(uint8_t value)
{
  if (value <= MAXSCALE)
  {
    // set global value
    scaleIndex = value;
    generateMidiScale(scaleIndex, rootNote);
  }
  return scaleIndex;
}

void changeRoot(boolean direction)
{
  if (direction)
  {
    rootNote++;
  } else
  {
    rootNote--;
  }
  generateMidiScale(scaleIndex, rootNote);
  #if OSC == 1
    //sendOSC("/set/root", rootNote);
    //getNotes();
  #endif
  #if MIDI == 1
    //
  #endif
}


// int main() 
int testScales()
{
    int i;
    int s;
    for (i=0; i<8; i++)
    {
        #if COMMANDLINE == 1
        cout << "NEW SCALE: " << availableScales[i].title << " id: " << i << endl;
        #endif
        generateMidiScale (i, rootNote);
        for (s=0; s<8; s++)
        {
            #if COMMANDLINE == 1
            cout << "scale note num: " << s << " scale note: " << scale[s] << endl;
            #endif
        }
    }
    return 0;
}



