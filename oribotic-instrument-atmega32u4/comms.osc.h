#ifndef COMMS_OSC_H
#define COMMS_OSC_H

#include <OSCMessage.h> 
#include "instrument_config.h"

#ifdef BOARD_HAS_USB_SERIAL
    #include <SLIPEncodedUSBSerial.h>
    extern SLIPEncodedUSBSerial SLIPSerial;
  #else
    #include <SLIPEncodedSerial.h>
    extern SLIPEncodedSerial SLIPSerial;
#endif

#if ORIGAMI==YOSHIMURA
  #include "instrument_Y8.h"
#endif

#if ORIGAMI==KRESLING
  #include "instrument_K48.h"
#endif

#if ORIGAMI==SUKI
  #include "instrument_S12.h"
#endif

// --------------------------------------------------------------------    FUNCTION PROTOTYPES

void rxOSC();
//void rawOSC(uint8_t key);
//void playOSC(uint8_t key);
//void touchedPlayOSC(uint8_t key);
void getNotes();
void sendOSC(char msg_str[20], uint16_t arg1, uint16_t arg2 = 3333);
void setRoot(OSCMessage &msg);
void setMode(OSCMessage &msg);
void setKeyProp(OSCMessage &msg, char param[4]);
void setKeyAutoHS(OSCMessage &msg);
void setKeyBendHI(OSCMessage &msg);
void calibrate(OSCMessage &msg);
void dispatchGetNotes(OSCMessage &msg);

#if SETUP_FUNCTIONS == 1
  void setSFIFilter(OSCMessage &msg);
  void setFFIFilter(OSCMessage &msg);
  void setCDT (OSCMessage &msg);
  void setInterval (OSCMessage &msg, int addressOffset);
#endif

#endif




  
    
    
    
    
    
    
    
      
      
      
  
    