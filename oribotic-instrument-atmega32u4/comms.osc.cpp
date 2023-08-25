#include "ORICORD.h"
#include "comms.osc.h"
#include "music.scales.h"

#if ORIGAMI==YOSHIMURA
  #include "instrument_Y8.h"
#endif

#if ORIGAMI==KRESLING
  #include "instrument_K48.h"
#endif

#if ORIGAMI==SUKI
  #include "instrument_S12.h"
#endif

#ifdef BOARD_HAS_USB_SERIAL
    #include <SLIPEncodedUSBSerial.h>
    SLIPEncodedUSBSerial SLIPSerial(thisBoardsSerialUSB);
  #else
    #include <SLIPEncodedSerial.h>
    SLIPEncodedSerial SLIPSerial(Serial1);
#endif

#if OSC == 1 

// variant one for basic messaging to string based channels
void sendOSC(char msg_str[20], uint16_t arg1, uint16_t arg2 = 3333)
{
    OSCMessage msg(msg_str);
    // add key
    // add state
    msg.add(arg1);
    // optionally add midi note number
    if (arg2 != 3333)
    {
      msg.add(arg2);
    }
    SLIPSerial.beginPacket();
    msg.send(SLIPSerial);   // send the bytes to the SLIP stream
    SLIPSerial.endPacket(); // mark the end of the OSC Packet
    msg.empty();            // free space occupied by message
}

// variant two (NEW) for sending instruments messages 
void sendOSC(char msg_str[20], uint8_t key, uint16_t arg1, uint16_t arg2 = 3333)
{
    OSCMessage msg(msg_str);
    // add key
    msg.add(key);
    // add state
    msg.add(arg1);
    // optionally add midi note number
    if (arg2 != 3333)
    {
      msg.add(arg2);
    }
    SLIPSerial.beginPacket();
    msg.send(SLIPSerial);   // send the bytes to the SLIP stream
    SLIPSerial.endPacket(); // mark the end of the OSC Packet
    msg.empty();            // free space occupied by message
}

bool isNumber(OSCMessage &msg, int arg)
{
  return msg.isInt(arg) || msg.isFloat(arg);
}

int getNumber(OSCMessage &msg, int arg)
{
  return (msg.isFloat(arg) ? (int)msg.getFloat(arg) : msg.getInt(arg));
}

void dispatchGetNotes(OSCMessage &msg)
{
    getNotes();
}

void getNotes()
{
    int note;
    for (int i = 0; i < PINCOUNT; i++)
    {
      note = orikeys[i].note + rootNote;
      sendOSC("/n", (uint8_t)i, note);
      delay(20);
    }
}

void setRoot(OSCMessage &msg)
{
    if (isNumber(msg, 0))
    {
      int val = getNumber(msg, 0);
      if (val < 0 || val > 127)
      {
        return;
      }
      rootNote = val;
      // send feedback to PD
      sendOSC("/set/root", val);
    }
    getNotes();
}

void setScale(OSCMessage &msg)
{
  if (isNumber(msg, 0))
  {
    int val = getNumber(msg, 0);
    val = changeScale((uint8_t)val);
    // send feedback to PD
    sendOSC("/set/scale", val);
  }
}

void setMode(OSCMessage &msg)
{
    sendOSC("/set/mode", 0);
    if (isNumber(msg, 0))
    {
      int val = getNumber(msg, 0);
      if( val < 0 || val > MAXMODE)
      {
        return;
      }
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
    if (isNumber(msg, 0))
    {
      key = getNumber(msg, 0);
      if (isNumber(msg, 1))
      {
        val = getNumber(msg, 1);
      }
      else
      {
        val = orikeys[key].last;
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
    if (isNumber(msg, 0))
    {
      int val = getNumber(msg, 0);
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
    if (isNumber(msg, 0))
    {
      int val = getNumber(msg, 0);
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
    if (isNumber(msg, 0)) {
    size = getNumber(msg, 0);
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
    if (isNumber(msg, 0)) {
        size = getNumber(msg, 0);
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
