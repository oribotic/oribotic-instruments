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

#endif