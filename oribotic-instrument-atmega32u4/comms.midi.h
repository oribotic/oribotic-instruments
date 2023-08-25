#include "instrument_config.h"

#if MIDI == 1

#include <Arduino.h>
#include <OSCMessage.h> 
#include "MIDIUSB.h"
#include "MIDIUSB_Defs.h"
#include "src/MPR121/MPR121.h"
#include "ORICORD.h"
#include "music.scales.h"

void rxMIDI();
void playMIDI(uint8_t key, boolean sendRaw);
void noteOn(byte channel, byte pitch, byte velocity);
void noteOff(byte channel, byte pitch, byte velocity);
void controlChange(byte channel, byte control, byte value);
void debugMIDI(byte pin, byte action, byte note, int filtered);
void setMIDIRoot(uint8_t newroot);

#if SETUP_FUNCTIONS == 1
  void setSFIFilter(uint8_t size);
  void setFFIFilter(uint8_t size);
  void setCDT (uint8_t size);
  void setInterval (uint8_t size);
#endif

#endif