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
              controlChange(0, 110, value); // send feedback on same channel
              // Serial.println(mode);
              break;
            case 109:               // set root note
              setMIDIRoot(value);
              controlChange(0, 109, value); // send feedback on same channel
              break;
            case 108:               // set scale
              setScale(value);
              controlChange(0, 108, value); // send feedback on same channel
              break;
            case 112:               // set FF1
              setFFIFilter(value);
              Serial.print("set FFI ");
              Serial.println(value);
              // controlChange(0, 112, value); // send feedback on same channel
              break;
            case 113:               // set SF1
              setSFIFilter(value);
              Serial.print("set SFI ");
              Serial.println(value);
              //controlChange(0, 113, value); // send feedback on same channel
              break;
            case 114:               // set Interval
              setInterval(value);
              Serial.print("set Interval ");
              Serial.println(value);
              //controlChange(0, 114, value); // send feedback on same channel
              break;
            case 115:               // set CDT
              setCDT(value);
              Serial.print("set CDT ");
              Serial.println(value);
              //controlChange(0, 115, value); // send feedback on same channel
              break;


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

#if SETUP_FUNCTIONS == 1

  void setFFIFilter(uint8_t size)
  {
      mpr121_FFI_t setting;
      setting = FFI_6;
      switch (size)
      {
        case 0:
          setting = FFI_6;
          break;
        case 1:
          setting = FFI_10;
          break;
        case 2:
          setting = FFI_18;
          break;
        case 3:
          setting = FFI_34;
          break;
      }
      #if MPRCOUNT > 0
          MPR121_0.setFFI(setting);
      #endif
      #if MPRCOUNT > 1
          MPR121_1.setFFI(setting);
      #endif
      #if MPRCOUNT > 2
          MPR121_2.setFFI(setting);
      #endif
      #if MPRCOUNT > 3
          MPR121_3.setFFI(setting);
      #endif
  }

  void setSFIFilter(uint8_t size)
  {
      mpr121_SFI_t setting;
      setting = SFI_4;
      switch (size)
      {
        case 0:
          setting = SFI_4;
          break;
        case 1:
          setting = SFI_6;
          break;
        case 2:
          setting = SFI_10;
          break;
        case 3:
          setting = SFI_18;
          break;
      }
      #if MPRCOUNT > 0
          MPR121_0.setSFI(setting);
      #endif
      #if MPRCOUNT > 1
          MPR121_1.setSFI(setting);
      #endif
      #if MPRCOUNT > 2
          MPR121_2.setSFI(setting);
      #endif
      #if MPRCOUNT > 3
          MPR121_3.setSFI(setting);
      #endif
  }

  

void setCDT (uint8_t size)
{
    mpr121_CDT_t cdt;
    cdt = CDT_500NS;
    if (size == 0)
    {
    cdt = CDT_500NS;
    }   
    if (size == 1)
    {
    cdt = CDT_1US;
    }
    if (size == 2)
    {
    cdt = CDT_2US;
    }
    if (size == 4)
    {
    cdt = CDT_4US;
    }
    if (size == 8)
    {
    cdt = CDT_8US;
    }
    if (size == 16)
    {
    cdt = CDT_16US;
    }
    if (size == 32)
    {
    cdt = CDT_32US;
    }

    #if MPRCOUNT > 0
        MPR121_0.setGlobalCDT(cdt);
    #endif
    #if MPRCOUNT > 1
        MPR121_1.setGlobalCDT(cdt);
    #endif
    #if MPRCOUNT > 2
        MPR121_2.setGlobalCDT(cdt);
    #endif
    #if MPRCOUNT > 3
        MPR121_3.setGlobalCDT(cdt);
    #endif

}

void setInterval (uint8_t size) 
{
    mpr121_sample_interval_t interval;
    interval = SAMPLE_INTERVAL_1MS;
    if (size == 1)
    {
    interval = SAMPLE_INTERVAL_1MS;
    }
    if (size == 2)
    {
    interval = SAMPLE_INTERVAL_2MS;
    }
    if (size == 4)
    {
    interval = SAMPLE_INTERVAL_4MS;
    }
    if (size == 8)
    {
    interval = SAMPLE_INTERVAL_8MS;
    }
    if (size == 16)
    {
    interval = SAMPLE_INTERVAL_16MS;
    }
    if (size == 32)
    {
    interval = SAMPLE_INTERVAL_32MS;
    }
    if (size == 64)
    {
    interval = SAMPLE_INTERVAL_64MS;
    }
    if (size == 127)
    {
    interval = SAMPLE_INTERVAL_128MS;
    }
    #if MPRCOUNT > 0
        MPR121_0.setSamplePeriod(interval);
    #endif
    #if MPRCOUNT > 1
        MPR121_1.setSamplePeriod(interval);
    #endif
    #if MPRCOUNT > 2
        MPR121_2.setSamplePeriod(interval);
    #endif
    #if MPRCOUNT > 3
        MPR121_3.setSamplePeriod(interval);
    #endif
}

#endif  // SETUP FUNCTIONS

#endif