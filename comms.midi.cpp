#include "instrument_config.h"

#if MIDI == 1

#include "MIDIUSB.h"
#include "MIDIUSB_Defs.h"
#include "comms.midi.h"
#include "ORICORD.h"
#include "music.scales.h"

  //------------------------------------------------------------------- IF MIDI
  void rxMIDI() {
    // 0x0B is control
    // 0xB0

    // filter for header
    // if control then sendMIDIControl
    // if note
    uint8_t control;
    uint8_t value;
    midiEventPacket_t rx;
    do {
       rx = MidiUSB.read();
        if (rx.header != 0) {
          // echo back to device
          controlChange(rx.byte1, rx.byte2, rx.byte3);
          control = rx.byte2 & 0x7F;
          value   = rx.byte3 & 0x7F;
          //control = rx.byte2;
          //value   = rx.byte3;
          Serial.print("header DEC:");
          Serial.println(rx.header, DEC);
          Serial.print("1 DEC:");
          Serial.println(rx.byte1, DEC);
          Serial.print("2 DEC:");
          Serial.println(rx.byte2, DEC);
          Serial.print("3 DEC:");
          Serial.println(rx.byte3, DEC);
          switch (control) {
            case 111:               // calibrate command
            // set Hard	  [111] 0		x
            // set Soft   [111] 5		
            // set bendLO	[111] 10		
            // set bendHI	[111] 15			
            // set bendLO [111] 20
             switch(value) {
              case 100: 
                doCalibrate();
                break;
              case 0: 
                setBendAllKeys("hard");
                Serial.println("set hard");
                break;
              case 5: 
                setBendAllKeys("soft");
                Serial.println("set soft");
                break;
              case 10: 
                setBendAllKeys("belo");
                Serial.println("set belo");
                break;
              case 15: 
                setBendAllKeys("behi");
                Serial.println("set behi");
                break;
              case 20: 
                setBendAllKeys("loHS");  
                Serial.println("lohs");            
                break;
             }
             break;
            case 110:               // set mode
              mode = value;
            case 109:               // set root note
              setMIDIRoot(value);
            case 108:               // set scale
              setScale(value);
          }
        }
    } while (rx.header != 0);
  }

  void noteOn(byte channel, byte pitch, byte velocity) {
    midiEventPacket_t midinote = {0x09, 0x90 | channel, pitch, velocity};
    Serial.print ("midi note: ");
    Serial.println(pitch);
    MidiUSB.sendMIDI(midinote);
    MidiUSB.flush();
  }

  void noteOff(byte channel, byte pitch, byte velocity) {
    midiEventPacket_t midinote = {0x08, 0x80 | channel, pitch, velocity};
    MidiUSB.sendMIDI(midinote);
    MidiUSB.flush();
  }

  // First parameter is the event type (0x0B = control change).
  // Second parameter is the event type, combined with the channel.
  // Third parameter is the control number number (0-119).
  // Fourth parameter is the control value (0-127).

  void controlChange(byte channel, byte control, byte value) {
    midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
    MidiUSB.sendMIDI(event);
    MidiUSB.flush();
  }

  void debugMIDI(byte pin, byte action, byte note, int filtered)
  {
    Serial.print(pin);
    Serial.print(": ");
    Serial.print(action);
    Serial.print(", ");
    Serial.print(filtered);
    Serial.print(", ");
    Serial.println(note);
  }

  void setMIDIRoot(uint8_t newroot)
  {
  rootNote = newroot;
  }

 // void playMIDI(uint8_t key, boolean sendRaw)
  // {
  //   uint8_t note = orikeys[key].note + ROOTNOTE;
  //   uint8_t pin;
  //   uint8_t panel = orikeys[key].panel;
  //   uint8_t state = orikeys[key].state;
  //   uint16_t last = orikeys[key].last;
  //   uint16_t filtered;
  //   long linearised;
  //   long normalised;
  //   long map1;
  //   uint8_t attack_velocity = 127;
  //   uint8_t release_velocity = 0;     
  //   uint8_t midi_channel = 0;           // edit to change the channel number
  //   uint8_t control_value;
    
    
  // #if REVERSED
  //   pin = reverse_logical[orikeys[key].pin];
  // #else
  //   pin = orikeys[key].pin;
  // #endif

  //   // filtered data
  //   filtered = MPRpanels[panel].mpr->getFilteredData(pin);

  //   // check the filtered data to decide which midiaction to take
  //   byte action = 0;

  //   if (filtered < orikeys[key].bendHI)
  //     action = 3;
  //   if (filtered > orikeys[key].bendLO)
  //     action = 3;
  //   if (filtered < (orikeys[key].bendLO)-SOFT_LOW_OFFSET)
  //     action = 2;
  //   // if (filtered < orikeys[key].soft)   {    action = 2;    }
  //   if (filtered < orikeys[key].hard)
  //     action = 1;
  //   if (filtered > orikeys[key].bendHI)
  //     action = 0;
    
  //   // update state orikeys table
  //   orikeys[key].state = action;
  //   orikeys[key].last = filtered;
  //   // check if the action is different from the state
  //   switch (action)
  //   {
  //   case 0: // off
  //     // turn off every time
  //     noteOff(midi_channel, key, release_velocity);
  //     if (DEBUG_LEVEL > 2)
  //         debugMIDI(key, action, note, filtered);
      
  //     break;
  //   case 1:  // hard
  //     if (state != 1)
  //     {
  //       if (DEBUG_LEVEL > 2)
  //         debugMIDI(key, action, note, filtered);
  //       noteOn(midi_channel, key, attack_velocity);
  //     } // send only on toggle
  //     break;
  //   case 2:  // soft
  //     // soft touch limiting of value to between hard and bendLO
  //     if (filtered < orikeys[key].hard)
  //       filtered = orikeys[key].hard;
  //     if (filtered > orikeys[key].bendLO)
  //       filtered = orikeys[key].bendLO;
      
  //     // two processes, map between hard and bend low, as the value from the bendLinear array 
    
  //     // send out data that is raw or normalised 
  //     if (sendRaw) 
  //     {
  //       map1 = map(filtered, orikeys[key].hard, orikeys[key].bendLO, 0, 127);
  //       control_value = map1;
  //       //control_value = 127-(int)map1;
  //     } else {
  //       //map1 = map(filtered, orikeys[key].hard, orikeys[key].bendLO, 0, 255);
  //       //linearised = bendLinear[(int)map1];
  //       // the map this value to 0-127
  //       //normalised = map(linearised, 0, 255, 0, 127);
  //       normalised = map(filtered, orikeys[key].hard, orikeys[key].bendLO, 0, 255);
  //       linearised = bendLinear[(int)normalised];
  //       control_value = linearised/2;
  //       //control_value = 127-(int)normalised;
  //     }
  //     if (DEBUG_LEVEL > 2)
  //     {
  //       Serial.print("[SOFT] map1: ");
  //       Serial.print(map1);
  //       Serial.print(" linearised: ");
  //       Serial.print(linearised);
  //       Serial.print(" normalised: ");
  //       Serial.print(normalised);
  //       Serial.print(" control_value: ");
  //       Serial.println(control_value);
  //     }
      
  //     if (filtered != last)
  //     { // send digital note ON if this is the first setting of soft touch
  //       //debugMIDI(key, action, note, control_value);
  //       controlChange(midi_channel, key + PANELCOUNT, control_value);
  //       //noteOn(midi_channel, note, control_value);
  //     }
  //     break;
  //   case 3: // bend
  //   case 4: // bend
  //     if (state != 3)
  //     { // send digital note OFF if this is the first setting of BEND
  //       //debugMIDI(key, action, note, filtered);
  //       noteOff(midi_channel, key, release_velocity);
  //     }
  //     // bendHI <> bendLO
  //     // normalise the key output value between bendLO and bendHI
  //     if (filtered < orikeys[key].bendLO)
  //       filtered = orikeys[key].bendLO;
  //     if (filtered > orikeys[key].bendHI)
  //       filtered = orikeys[key].bendHI;
  //     if (sendRaw) 
  //     {
  //       control_value = map(filtered, orikeys[key].bendLO, orikeys[key].bendHI, 0, 127);
  //       //control_value = 127-(int)map1;
  //     } else {
  //       //map1 = map(filtered, orikeys[key].bendLO, orikeys[key].bendHI, 0, 255);
  //       normalised = map(filtered, orikeys[key].bendLO, orikeys[key].bendHI, 0, 255);
  //       linearised = bendLinear[(int)normalised];
  //       // the map this value to 0-127
  //       control_value = (int)linearised/2;
  //       //control_value = map(linearised, 0, 255, 0, 127);
  //       //control_value = 127-(int)normalised;
  //     }
  //     if (DEBUG_LEVEL > 2)
  //     {
  //       Serial.print(key);
  //       Serial.print(" [BEND] filtered: ");
  //       Serial.print(filtered);
  //       Serial.print(" map: ");
  //       Serial.print(map1);
  //       Serial.print(" linearised: ");
  //       Serial.print(linearised);
  //       Serial.print(" normalised: ");
  //       Serial.print(normalised);
  //       Serial.print(" control_value: ");
  //       Serial.println(control_value);
  //     }   
  //     if (filtered != last)
  //     {
  //       if (DEBUG_LEVEL > 2)
  //         debugMIDI(key, action, note, control_value);
  //       controlChange(midi_channel, key, control_value);
  //     }
  //     break;
  //   }
  // }


#endif