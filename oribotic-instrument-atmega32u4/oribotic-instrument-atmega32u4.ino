#include "MPR121.h"
#include <Wire.h>

// BASIC INSTRUMENT CONFIG 
#include "instrument_config.h"
#include "music.scales.h"
// ORICORDION FUNCTIONS
#include "ORICORD.h"

// #include <EEPROM.h>
#define __AVR_ATmega32U4__

#ifndef _BV
  #define _BV(bit) (1 << (bit))
#endif

#if MIDI == 1
  #include "MIDIUSB.h"
  #include "MIDIUSB_Defs.h"
  #include "comms.midi.h"
#endif

#if OSC == 1
  #include "comms.osc.h"
#endif

#define REVERSED 0
#define ARR_LEN(x) (sizeof(x) / sizeof(x[0]))

// --------------------------------------------------------------------     SETUP
void setup()
{
  // put your setup code here, to run once:
  #if MIDI == 1
    Serial.begin(115200);
  #endif
  #if OSC == 1
    SLIPSerial.begin(115200);
    #if ARDUINO >= 100
      while (!Serial1)
        ; // Leonardo "feature"
    #endif
  #endif

  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, 1);
  pinMode(CAL_BTN, INPUT);   // button 1
  pinMode(HI_BTN, INPUT);    // button 2
  pinMode(LO_BTN, INPUT);    // button 3
  #if BTN_COUNT == 5
    pinMode(FN1_BTN, INPUT); // button 2
    pinMode(FN2_BTN, INPUT); // button 3
  #else
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
  #endif

  setupMPR();
  delay(500);
}

// --------------------------------------------------------------------     MAIN LOOP

void loop() {
  #if OSC == 1
    if (SLIPSerial.available() > 0)
      rxOSC();
  #endif

  #if MIDI == 1
    rxMIDI();
  #endif

  uint8_t i;

  #if MPRCOUNT > 0
    MPR121_0.updateAll();
  #endif
  #if MPRCOUNT > 1
    MPR121_1.updateAll();
  #endif
  #if MPRCOUNT > 2
    MPR121_2.updateAll();
  #endif
  #if MPRCOUNT > 3
    MPR121_3.updateAll();
  #endif

  checkbuttons();

  for (i = 0; i < PINCOUNT; i++)
  {
    switch (mode)
    {
      case MODE_SERIAL_DEBUG:         // 0
        serialDebug(i, 0);
        break;
      case MODE_SERIAL_DEBUG_RAW:     // 1
        serialDebug(i, 1);
        break;
      case MODE_RAW:                  // 2
        raw(i);
        break;
      case MODE_NORMALISED:           // 3
        touchPlay(i);
        break;
      case MODE_RAW_PLUS_NORMALISED:  // 4
        touchPlay(i);
        raw(i); 
        break;
      case MODE_TOUCH_PLAY:           // 5
        touchPlay(i);
        break;
      case 6:                         // 6
        soft(i);
        break;
    }
  }
  #ifdef PANELGROUPS
  if (usePanelGroups)
  {
    updatePanelGroups();
  }
  #endif  
  delay(intervaldelay);
}



