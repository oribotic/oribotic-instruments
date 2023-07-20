#include <Arduino.h>
#include <OSCMessage.h> 
#include "MPR121.h"
#include "instrument_config.h"
#include "ORICORD.h"
#include "comms.osc.h"
#include "music.scales.h"

#if ORIGAMI==YOSHIMURA
  #include "instrument_Y8.h"
#endif

#if ORIGAMI==KRESLING
  #include "instrument_K48.h"
#endif

#ifdef BOARD_HAS_USB_SERIAL
    #include <SLIPEncodedUSBSerial.h>
    SLIPEncodedUSBSerial SLIPSerial(thisBoardsSerialUSB);
  #else
    #include <SLIPEncodedSerial.h>
    SLIPEncodedSerial SLIPSerial(Serial1);
#endif

#if OSC == 1 

void sendOSC(char msg_str[20], uint16_t arg1, uint16_t arg2 = 3333)
{
    OSCMessage msg(msg_str);
    msg.add(arg1);
    if (arg2 != 3333)
    {
      msg.add(arg2);
    }
    SLIPSerial.beginPacket();
    msg.send(SLIPSerial);   // send the bytes to the SLIP stream
    SLIPSerial.endPacket(); // mark the end of the OSC Packet
    msg.empty();            // free space occupied by message
}

// void rawOSC(uint8_t key) 
// {
//     uint8_t note = orikeys[key].note + rootNote;
//     uint8_t pin;
//     uint8_t panel = orikeys[key].panel;
//     uint16_t filtered;
//     #if REVERSED
//       pin = reverse_logical[orikeys[key].pin];
//     #else
//       pin = orikeys[key].pin;
//     #endif
//     filtered = MPRpanels[panel].mpr->getFilteredData(pin);
//     note = MPRpanels[panel].mpr->getBaselineData(pin);
//     char msg_str[12];
//     sprintf(msg_str, "/raw/%d", key);
//     sendOSC(msg_str, filtered, note);
// }

void dispatchGetNotes(OSCMessage &msg)
{
    getNotes();
}

void getNotes()
{
    int note;
    char chan[10];
    for (int i = 0; i < PINCOUNT; i++)
    {
      note = orikeys[i].note + rootNote;
      sprintf(chan, "/n/%d", i);
      sendOSC(chan, note);
      delay(20);
    }
}

void setRoot(OSCMessage &msg)
{
    if (msg.isInt(0))
    {
      int val = msg.getInt(0);
      rootNote = val;
      // send feedback to PD
      sendOSC("/set/root", val);
    }
    getNotes();
}

void setScale(OSCMessage &msg)
{
  if (msg.isInt(0))
  {
    uint8_t val = msg.getInt(0);
    changeScale(val);
  }
}

void setMode(OSCMessage &msg)
{
    sendOSC("/set/mode", 0);
    if (msg.isInt(0))
    {
      int val = msg.getInt(0);
      mode = val;
      // send feedback to PD
      sendOSC("/set/mode", val);
      int i;
      for (i=0; i<val; i++)
      {
        digitalWrite(LED1, 0);
        delay(100);
        digitalWrite(LED1, 1);
        delay(100);
      }
    }
}

void setKeyProp(OSCMessage &msg, char param[4])
{
    uint8_t key;
    uint16_t val;
    char chan[16];
    if (msg.isInt(0))
    {
      key = msg.getInt(0);
      if (msg.isInt(1))
      {
        val = msg.getInt(1);
      }
      else
      {
        val = orikeys[key].last;
        // sprintf(chan, "/key/last/%d", key);
        // sendOSC(chan, val);
      }
      setBendSingleKey(key, param);
    }
  }

void setKeyAutoHS(OSCMessage &msg)
{
    setKeyProp(msg, "loHS");
}

void setKeyHard(OSCMessage &msg)
{
    setKeyProp(msg, "hard");
}

void setKeyBendHI(OSCMessage &msg)
{
    setKeyProp(msg, "behi");
}

void setKeyBendLO(OSCMessage &msg)
{
    setKeyProp(msg, "belo");
}

void calibrate(OSCMessage &msg)
{
    doCalibrate();
}

  // void setContinuousMode(OSCMessage &msg)
  // {
  //   if (msg.isInt(0)) {
  //     bool val = msg.getInt(0);
  //     // send feedback to PD
  //     if (val == 1)
  //     {
  //       continuousMode = true;
  //     } else {
  //       continuousMode = false;
  //     }
  //     sendOSC("/set/continuousMode", val);
  //   }
  // }

void rxOSC()
{
  OSCMessage messageIN;
  int sizeb = 0;
  while (!SLIPSerial.endofPacket())
  {
    if ((sizeb = SLIPSerial.available()) > 0)
    {
      while (sizeb--)
      {
        messageIN.fill(SLIPSerial.read());
      }
    }
  }
  if (!messageIN.hasError())
  {
    // registered ".dispatch" functions need signature -> void dispatchAddress(OSCMessage &msg){ }
    // registered ".route" functions need signature -> void routeAddress(OSCMessage &msg, int addressOffset){ }

    // eeprom 
    // messageIN.dispatch("/readConfig", dispatchEepromRead);
    // messageIN.dispatch("/writeConfig", eepromWrite);

    // panel config methods
    messageIN.dispatch("/calibrate", calibrate);
    messageIN.dispatch("/setKey/hard", setKeyHard);
    messageIN.dispatch("/setKey/belo", setKeyBendLO);
    messageIN.dispatch("/setKey/behi", setKeyBendHI);
    messageIN.dispatch("/setKey/loHS", setKeyAutoHS);
    // messageIN.dispatch("/setKey/soft", setKeySoft);

    // note config methods
    messageIN.dispatch("/getNotes", dispatchGetNotes);
    messageIN.dispatch("/setRoot", setRoot);
    messageIN.dispatch("/setMode", setMode);
    messageIN.dispatch("/setScale", setScale);
    // messageIN.dispatch("/setRoot", setPlay);
    // messageIN.dispatch("/setContinuous", setContinuousMode);
    // messageIN.dispatch("/setGroupMode", setPanelGroup);

    // mpr config methods
    #if SETUP_FUNCTIONS == 1
    messageIN.dispatch("/setCDT", setCDT);
    messageIN.dispatch("/setInterval", setInterval);
    messageIN.dispatch("/setSFIFilter", setSFIFilter);
    messageIN.dispatch("/setFFIFilter", setFFIFilter);
    #endif 
  }
}

#if SETUP_FUNCTIONS == 1

void setSFIFilter(OSCMessage &msg)
{
    if (msg.isInt(0))
    {
      int val = msg.getInt(0);
      switch (val)
      {
        case 0:
          MPR121_0.setSFI(SFI_4);
          break;
        case 1:
          MPR121_0.setSFI(SFI_6);
          break;
        case 2:
          MPR121_0.setSFI(SFI_10);
          break;
        case 3:
          MPR121_0.setSFI(SFI_18);
          break;
      }
      sendOSC("/set/SFI", val);
    }
}

void setFFIFilter(OSCMessage &msg)
{
    if (msg.isInt(0))
    {
      int val = msg.getInt(0);
      switch (val)
      {
        case 0:
          MPR121_0.setFFI(FFI_6);
          break;
        case 1:
          MPR121_0.setFFI(FFI_10);
          break;
        case 2:
          MPR121_0.setFFI(FFI_18);
          break;
        case 3:
          MPR121_0.setFFI(FFI_34);
          break;
      }
      sendOSC("/set/FFI", val);
    }
}

void setCDT (OSCMessage &msg)
{
    mpr121_CDT_t cdt;
    uint8_t size;
    if (msg.isInt(0)) {
    size = msg.getInt(0);
    sendOSC("/filter/CDT", size);
    }
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

void setInterval (OSCMessage &msg) 
{
    mpr121_sample_interval_t interval;
    uint8_t size;
    if (msg.isInt(0)) {
        size = msg.getInt(0);
        sendOSC("/set/interval", size);
    }
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
    if (size == 128)
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

#endif

#endif // OSC

// void playOSC(uint8_t key)
// {
//     uint8_t note = orikeys[key].note + rootNote;
//     uint8_t pin;
//     uint8_t panel = orikeys[key].panel;
//     uint8_t state = orikeys[key].state;
//     uint16_t last = orikeys[key].last;
//     uint16_t filtered;
//     long normalised;

//   #if REVERSED
//     pin = reverse_logical[orikeys[key].pin];
//   #else
//     pin = orikeys[key].pin;
//   #endif

//     // filtered data
//     filtered = MPRpanels[panel].mpr->getFilteredData(pin);

//     // check the filtered data to decide which midiaction to take
//     byte action = 0;

//     if (filtered < orikeys[key].bendHI)
//     {
//       action = 3;
//     }
//     if (filtered > orikeys[key].bendLO)
//     {
//       action = 3;
//     }
//     if (filtered < (orikeys[key].bendLO - 50))
//     {
//       action = 2;
//     }
//     // if (filtered < orikeys[key].soft)   {    action = 2;    }
//     if (filtered < orikeys[key].hard)
//     {
//       action = 1;
//     }
//     if (filtered > orikeys[key].bendHI)
//     {
//       action = 0;
//     }

//     // update state orikeys table
//     orikeys[key].state = action;
//     orikeys[key].last = filtered;

//     char msg_str[12];
//     uint16_t arg_state;
//     uint16_t arg_note = 3333;
//     // uint16_t arg_role = 3333;

//     if (continuousMode)
//     {
//       if (filtered != last)
//       {
//         sprintf(msg_str, "/c/%d", key);
//         arg_state = filtered;
//         sendOSC(msg_str, arg_state, arg_note);
//       }
//     }
//     // check if the action is different from the state
//     switch (action)
//     {
//     case 0:
//       // off
//       sprintf(msg_str, "/d/%d", key);
//       arg_state = 0;
//       arg_note = note;
//       // arg_role = role;
//       if (state != 0)
//       {
//         sendOSC(msg_str, arg_state, arg_note);
//       } // send only on toggle
//       break;
//     case 1:
//       // hard
//       sprintf(msg_str, "/d/%d", key);
//       arg_state = 1;
//       arg_note = note;
//       // arg_role = role;
//       if (state != 1)
//       {
//         sendOSC(msg_str, arg_state, arg_note);
//       } // send only on toggle
//       break;
//     case 2:
//       // soft
//       if (state != 2)
//       {
//         // send digital note ON if this is the first setting of soft touch
//         sprintf(msg_str, "/d/%d", key);
//         arg_state = 1;
//         arg_note = note;
//         // arg_role = role;
//         sendOSC(msg_str, arg_state, arg_note);
//         arg_note = 3333;
//       }
//       // soft touch analysis
//       sprintf(msg_str, "/s/%d", key);
//       arg_state = filtered;
//       if (filtered < orikeys[key].hard)
//       {
//         filtered = orikeys[key].hard;
//       }
//       if (filtered > orikeys[key].bendLO)
//       {
//         filtered = orikeys[key].bendLO;
//       }
//       normalised = map(filtered, orikeys[key].hard, orikeys[key].bendLO, 0, 255);

//       arg_state = bendLinear[normalised]; // convert normal value to linear value
//       if (filtered != last)
//       {
//         sendOSC(msg_str, arg_state, arg_note); // send every time
//       }
//       break;
//     case 3:
//     case 4:
//       if (state != 3)
//       {
//         // send digital note OFF if this is the first setting of BEND
//         sprintf(msg_str, "/d/%d", key);
//         arg_state = 0;
//         arg_note = note;
//         // arg_role = role;
//         sendOSC(msg_str, arg_state, arg_note);
//         arg_note = 3333;
//       }
//       // bendHI <> bendLO
//       sprintf(msg_str, "/b/%d", key);
//       // normalise the key output value between bendLO and bendHI
//       if (filtered < orikeys[key].bendLO)
//       {
//         filtered = orikeys[key].bendLO;
//       }
//       if (filtered > orikeys[key].bendHI)
//       {
//         filtered = orikeys[key].bendHI;
//       }

//       normalised = map(filtered, orikeys[key].bendLO, orikeys[key].bendHI, 0, 255);

//       arg_state = bendLinear[normalised]; // convert normal value to linear value

//       if (filtered != last)
//       {
//         sendOSC(msg_str, arg_state, arg_note); // send every time
//       }
//       break;
//     }
// }

// void touchedPlayOSC(uint8_t key)
// {
//     uint8_t note = orikeys[key].note + rootNote;
//     uint8_t pin;
//     uint8_t panel = orikeys[key].panel;
//     uint8_t state = orikeys[key].state;
//     uint16_t last = orikeys[key].last;
//     uint16_t filtered;
//     long normalised;
//     bool touched;
//     bool released;

//   #if REVERSED
//     pin = reverse_logical[orikeys[key].pin];
//   #else
//     pin = orikeys[key].pin;
//   #endif

//     // filtered data
//     filtered = MPRpanels[panel].mpr->getFilteredData(pin);
//     touched = MPRpanels[panel].mpr->isNewTouch(pin);
//     released = MPRpanels[panel].mpr->isNewRelease(pin);

 
//     // check the filtered data to decide which midiaction to take
//     byte action = 0;

//     if (filtered < orikeys[key].bendHI)
//     {
//       action = 3;
//     }
//     if (filtered > orikeys[key].bendLO)
//     {
//       action = 3;
//     }
//     if (filtered < (orikeys[key].bendLO - 50))
//     {
//       action = 2;
//     }
//     // if (filtered < orikeys[key].soft)   {    action = 2;    }
//     if (filtered < orikeys[key].hard)
//     {
//       action = 1;
//     }
//     if (filtered > orikeys[key].bendHI)
//     {
//       action = 0;
//     }

//     // update state orikeys table
//     orikeys[key].state = action;
//     orikeys[key].last = filtered;

//     char msg_str[12];
//     uint16_t arg_state;
//     uint16_t arg_note = 3333;
//     // uint16_t arg_role = 3333;
    
//     // using touch data
//     if (touched)
//     {
//       sprintf(msg_str, "/t/%d", key);
//       arg_state = 1;
//       arg_note = note;
//       sendOSC(msg_str, arg_state, arg_note);
//     }
//     if (released)
//     {
//       sprintf(msg_str, "/t/%d", key);
//       arg_state = 0;
//       arg_note = note;
//       sendOSC(msg_str, arg_state, arg_note);
//     }

//     if (continuousMode)
//     {
//       if (filtered != last)
//       {
//         sprintf(msg_str, "/c/%d", key);
//         arg_state = filtered;
//         sendOSC(msg_str, arg_state, arg_note);
//       }
//     }
//     // check if the action is different from the state
//     switch (action)
//     {
//     case 0:
//       // off
//       sprintf(msg_str, "/d/%d", key);
//       arg_state = 0;
//       arg_note = note;
//       // arg_role = role;
//       if (state != 0)
//       {
//         sendOSC(msg_str, arg_state, arg_note);
//       } // send only on toggle
//       break;
//     case 1:
//       // hard
//       sprintf(msg_str, "/d/%d", key);
//       arg_state = 1;
//       arg_note = note;
//       // arg_role = role;
//       if (state != 1)
//       {
//         sendOSC(msg_str, arg_state, arg_note);
//       } // send only on toggle
//       break;
//     case 2:
//       // soft
//       if (state != 2)
//       {
//         // send digital note ON if this is the first setting of soft touch
//         sprintf(msg_str, "/d/%d", key);
//         arg_state = 1;
//         arg_note = note;
//         // arg_role = role;
//         sendOSC(msg_str, arg_state, arg_note);
//         arg_note = 3333;
//       }
//       // soft touch analysis
//       sprintf(msg_str, "/s/%d", key);
//       arg_state = filtered;
//       if (filtered < orikeys[key].hard)
//       {
//         filtered = orikeys[key].hard;
//       }
//       if (filtered > orikeys[key].bendLO)
//       {
//         filtered = orikeys[key].bendLO;
//       }
//       normalised = map(filtered, orikeys[key].hard, orikeys[key].bendLO, 0, 255);

//       arg_state = bendLinear[normalised]; // convert normal value to linear value
//       if (filtered != last)
//       {
//         sendOSC(msg_str, arg_state, arg_note); // send every time
//       }
//       break;
//     case 3:
//     case 4:
//       if (state != 3)
//       {
//         // send digital note OFF if this is the first setting of BEND
//         sprintf(msg_str, "/d/%d", key);
//         arg_state = 0;
//         arg_note = note;
//         // arg_role = role;
//         sendOSC(msg_str, arg_state, arg_note);
//         arg_note = 3333;
//       }
//       // bendHI <> bendLO
//       sprintf(msg_str, "/b/%d", key);
//       // normalise the key output value between bendLO and bendHI
//       if (filtered < orikeys[key].bendLO)
//       {
//         filtered = orikeys[key].bendLO;
//       }
//       if (filtered > orikeys[key].bendHI)
//       {
//         filtered = orikeys[key].bendHI;
//       }

//       normalised = map(filtered, orikeys[key].bendLO, orikeys[key].bendHI, 0, 255);
//       arg_state = bendLinear[normalised]; // convert normal value to linear value

//       if (filtered != last)
//       {
//         sendOSC(msg_str, arg_state, arg_note); // send every time
//       }
//       break;
//     }
// } 