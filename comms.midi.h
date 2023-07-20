#include "instrument_config.h"

#if MIDI == 1

#include <Arduino.h>
#include <OSCMessage.h> 
#include "MIDIUSB.h"
#include "MIDIUSB_Defs.h"
#include "MPR121.h"
#include "ORICORD.h"
#include "music.scales.h"

void rxMIDI();
void playMIDI(uint8_t key, boolean sendRaw);
void noteOn(byte channel, byte pitch, byte velocity);
void noteOff(byte channel, byte pitch, byte velocity);
void controlChange(byte channel, byte control, byte value);
void debugMIDI(byte pin, byte action, byte note, int filtered);
void setMIDIRoot(uint8_t newroot);

#endif