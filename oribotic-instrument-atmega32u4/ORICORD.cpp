#include <OSCMessage.h> 
#include "MPR121.h"
#include "ORICORD.h"
#include "instrument_config.h"
#include "music.scales.h"

#if OSC == 1
  #include "comms.osc.h"
#endif

#if MIDI == 1
  #include "comms.midi.h"
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


#define ARR_LEN(x) (sizeof(x) / sizeof(x[0]))

uint8_t mode = MODE_DEFAULT;
// byte rootNote = 60; // major C // defined in music.scales.h
byte activebutton = 0;
bool continuousMode = false;

#if REVERSED
byte reverse_logical[12] = {11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
#endif

#if MPRCOUNT == 1
  MPR121_t MPR121_0 = MPR121_t();
  struct panels MPRpanels[MPRCOUNT] = {
      {.addr = 0x5A, .mpr = &MPR121_0, .reversed = 0, .electrodes = 12},
  };
#endif

#if MPRCOUNT == 2
MPR121_t MPR121_0 = MPR121_t();
MPR121_t MPR121_1 = MPR121_t();
struct panels MPRpanels[MPRCOUNT] = {
    {.addr = 0x5A, .mpr = &MPR121_0, .reversed = 0, .electrodes = 12},
    {.addr = 0x5B, .mpr = &MPR121_1, .reversed = 0, .electrodes = 12},
};
#endif
#if MPRCOUNT == 3
MPR121_t MPR121_0 = MPR121_t();
MPR121_t MPR121_1 = MPR121_t();
MPR121_t MPR121_2 = MPR121_t();
struct panels MPRpanels[MPRCOUNT] = {
    {.addr = 0x5A, .mpr = &MPR121_0, .reversed = 0, .electrodes = 12},
    {.addr = 0x5B, .mpr = &MPR121_1, .reversed = 0, .electrodes = 12},
    {.addr = 0x5C, .mpr = &MPR121_2, .reversed = 0, .electrodes = 12},
};
#endif
#if MPRCOUNT == 4
MPR121_t MPR121_0 = MPR121_t();
MPR121_t MPR121_1 = MPR121_t();
MPR121_t MPR121_2 = MPR121_t();
MPR121_t MPR121_3 = MPR121_t();
struct panels MPRpanels[MPRCOUNT] = {
    {.addr = 0x5A, .mpr = &MPR121_0, .reversed = 0, .electrodes = 12},
    {.addr = 0x5B, .mpr = &MPR121_1, .reversed = 0, .electrodes = 12},
    {.addr = 0x5C, .mpr = &MPR121_2, .reversed = 0, .electrodes = 12},
    {.addr = 0x5D, .mpr = &MPR121_3, .reversed = 0, .electrodes = 12},
};
#endif


void soft(uint8_t key)
{
  uint8_t note = orikeys[key].note + rootNote;
  uint8_t pin = orikeys[key].pin;
  uint8_t panel = orikeys[key].panel;
  uint8_t state = orikeys[key].state;
  uint16_t last = orikeys[key].last;
  uint16_t filtered;
  uint8_t baseline;
  long normalised;
  bool touched;
  bool released;
  bool touchdata;
  
  char msg_str[12];
  uint16_t arg_state;
  // filtered data
  filtered = MPRpanels[panel].mpr->getFilteredData(pin);
  touched = MPRpanels[panel].mpr->getTouchData(pin);
  baseline = MPRpanels[panel].mpr->getBaselineData(pin);
  byte action = 0; 
  
  if (touchdata)
  {
    //send("d", key, note, 1);      // used to be t
    action = 1;
    // send soft signal data
  }
  if (released)
  {
    //send("d", key, note, 0);      // used to be t
    action = 0;
  }

  // update state orikeys table
  orikeys[key].state = action;
  orikeys[key].last = filtered;

  if (touched)
  {
    arg_state = mapSoft(key, filtered);
  }
}

void raw(uint8_t key) 
{
    uint8_t note = orikeys[key].note + rootNote;
    uint8_t pin;
    uint8_t panel = orikeys[key].panel;
    uint16_t filtered;
    #if REVERSED
      pin = reverse_logical[orikeys[key].pin];
    #else
      pin = orikeys[key].pin;
    #endif
    filtered = MPRpanels[panel].mpr->getFilteredData(pin);
    note = MPRpanels[panel].mpr->getBaselineData(pin);
   
    send("r", key, note, filtered);    
}

void play(uint8_t key)
{
    uint8_t note = orikeys[key].note + rootNote;
    uint8_t pin;
    uint8_t panel = orikeys[key].panel;
    uint8_t state = orikeys[key].state;
    uint16_t last = orikeys[key].last;
    uint16_t filtered;
    long normalised;
    bool touched;
    bool released;

  #if REVERSED
    pin = reverse_logical[orikeys[key].pin];
  #else
    pin = orikeys[key].pin;
  #endif

    // filtered data
    filtered = MPRpanels[panel].mpr->getFilteredData(pin);
    //touched = MPRpanels[panel].mpr->isNewTouch(pin);
    //released = MPRpanels[panel].mpr->isNewRelease(pin);

    byte action; 
    // check the filtered data to decide which midiaction to take
    action = getActionState(key, filtered);

    // update state orikeys table
    orikeys[key].state = action;
    orikeys[key].last = filtered;

    char msg_str[12];
    uint16_t arg_state;
    uint16_t arg_note = 3333;

    switch (action)
    {
    case 0:
      // off
      if (state != 0)
      {
        send("d", key, note, 0);
        //sendOSC(msg_str, arg_state, arg_note);
      } // send only on toggle
      break;
    case 1:
      // hard
      if (state != 1)
      {
        send("d", key, note, 1);
      } // send only on toggle
      break;
    case 2:
      // soft
      if (state != 2)
      {
        // send digital note ON if this is the first setting of soft touch
        send("d", key, note, 1);
        //arg_note = 3333;
      }
      // soft touch analysis
      sprintf(msg_str, "/s/%d", key);
      arg_state = filtered;
      if (filtered < orikeys[key].hard)
      {
        filtered = orikeys[key].hard;
      }
      if (filtered > orikeys[key].bendLO)
      {
        filtered = orikeys[key].bendLO;
      }
      
      normalised = map(filtered, orikeys[key].hard, orikeys[key].bendLO, 0, 255);
      arg_state = bendLinear[normalised]; // convert normal value to linear value
      #if MIDI == 1
        arg_state = arg_state/2;
      #endif
      if (filtered != last)
      {
        send("s", key, note, arg_state);
        //sendOSC(msg_str, arg_state, arg_note); // send every time
      }
      break;
    case 3:
    case 4:
      if (state != 3)
      {
        // send digital note OFF if this is the first setting of BEND

        send("d", key, note, 0);
      }
      // bendHI <> bendLO

      // normalise the key output value between bendLO and bendHI
      if (filtered < orikeys[key].bendLO)
        filtered = orikeys[key].bendLO;
      if (filtered > orikeys[key].bendHI)
        filtered = orikeys[key].bendHI;

      normalised = map(filtered, orikeys[key].bendLO, orikeys[key].bendHI, 0, 255);
      arg_state = bendLinear[normalised]; // convert normal value to linear value
      #if MIDI == 1
        arg_state = arg_state/2;
      #endif
      if (filtered != last)
      {
        send("b", key, note, arg_state);
      }
      break;
    }
} 

uint16_t mapSoft(uint8_t key, uint16_t filtered)
{
  uint8_t pin = orikeys[key].pin;
  uint8_t panel = orikeys[key].panel;
  uint8_t touchThreshold = MPRpanels[panel].mpr->getTouchThreshold(pin);
  uint16_t softTouch;
  long normalised;
  uint8_t range = orikeys[key].soft - orikeys[key].hard;
  
  
  // record the new hard value
  if (filtered < orikeys[key].hard)
  {
    // limit soft filter SOFT_FILTER_LIMIT
    if (filtered > orikeys[key].soft - SOFT_FILTER_LIMIT)
    {
      orikeys[key].hard = filtered;
    }
  }
  // record the new hard value
  if (filtered > orikeys[key].soft)
  {
    orikeys[key].soft = filtered;
  }

  uint16_t constrained = filtered;
  // constrain the filtered between hard and soft
  if (constrained > orikeys[key].soft)
  {
    constrained = orikeys[key].soft;
    //Serial.print("XS ");
  }
  if (constrained < orikeys[key].hard)
  {
    constrained = orikeys[key].hard;
    //Serial.print("XH ");
  }

  // get a value for our 
  normalised = longmap(constrained, orikeys[key].hard, orikeys[key].soft, 0, ARR_LEN(bendLinear)-1);
  softTouch = (uint16_t) normalised;

  if (DEBUG_LEVEL > 2)
  {
    Serial.print(key);

    Serial.print(" \tAL: ");
    Serial.print(ARR_LEN(bendLinear)-1);

    Serial.print(" \tH: ");
    Serial.print(orikeys[key].hard);

    Serial.print(" \tS: ");
    Serial.print(orikeys[key].soft);

    Serial.print("\tR: ");
    Serial.print(range);

    Serial.print("\tF:");
    Serial.print(filtered);

    Serial.print("\tN:");
    Serial.print(normalised);

    Serial.print("\tN:");
    Serial.println(softTouch);
  }
  return softTouch;
}
#if OSC == 1

void send(char channel[1], uint8_t key, uint8_t note, uint16_t state )
{
  char msg_str[12];
  sprintf(msg_str, "/%s/%d", channel, key);
  // /d/1 0 68
  sendOSC(msg_str, state, note);
}

#endif

#if MIDI == 1

void debugMidi(char msg[12], uint8_t channel, uint8_t key, uint16_t value )
{
  Serial.print(msg);
  Serial.print(channel);
  Serial.print(" ");
  Serial.print(key);
  Serial.print(" ");
  Serial.println(value);
}

void send(char channel[1], uint8_t key, uint8_t note, uint16_t state)
{
  uint8_t midi_channel = 0;
  uint8_t attack_velocity = 127;
  uint8_t release_velocity = 0;

  switch (channel[0])
  {
    case 'd':
      if (state==1)
      {
        noteOn(midi_channel, key, attack_velocity);
        if (DEBUG_LEVEL > 1)
        {
          debugMidi("hard on:", midi_channel, key, attack_velocity );
        }
      } 
      if (state==0)
      {
        noteOff(midi_channel, key, release_velocity);
        if (DEBUG_LEVEL > 1)
        {
          debugMidi("hard off:", midi_channel, key, attack_velocity );
        }
      }
      break;
    case 'b':
      controlChange(midi_channel, key, state);
      if (DEBUG_LEVEL > 2)
      {
        debugMidi("bend ctl:", midi_channel, key, state );
      }
      break;
    case 't':
      uint8_t touchkey = key + PANELCOUNT;
      if (state==1)
      {
        if (DEBUG_LEVEL > 1)
        {
          debugMidi("touch on:", midi_channel, touchkey, attack_velocity );
        }
        noteOn(midi_channel, key + PANELCOUNT, attack_velocity);
      } 
      if (state==0)
      {
        if (DEBUG_LEVEL > 1)
        {
          debugMidi("touch off:", midi_channel, touchkey, release_velocity );
        }
        noteOff(midi_channel, key + PANELCOUNT, release_velocity);
      }
      break;
    case 's':
      if (DEBUG_LEVEL > 2)
      {
        debugMidi("bend ctl:", midi_channel, key, state );
      }
      controlChange(midi_channel, key, state);
      break;
  }
}

#endif 

void touchPlay(uint8_t key)
{
  uint8_t note = orikeys[key].note + rootNote;
  uint8_t pin;
  uint8_t panel = orikeys[key].panel;
  uint8_t state = orikeys[key].state;
  uint16_t last = orikeys[key].last;
  uint16_t filtered;
  long normalised;
  bool newtouch;
  bool touched;
  bool released;
  uint8_t touchThreshold;
  char msg_str[12];
  uint16_t arg_state;
  byte action; 
  
  #if REVERSED
    pin = reverse_logical[orikeys[key].pin];
  #else
    pin = orikeys[key].pin;
  #endif

  // filtered data
  filtered = MPRpanels[panel].mpr->getFilteredData(pin);
  newtouch = MPRpanels[panel].mpr->isNewTouch(pin);
  touched = MPRpanels[panel].mpr->getTouchData(pin);
  released = MPRpanels[panel].mpr->isNewRelease(pin);
  
  // check the filtered data to decide which midiaction to take
  // action = getActionState(key, filtered);

  // using touch data
  if (released)
  {
    send("d", key, note, 0);      // used to be t
    action = 0;
  }

  if (newtouch)
  {
    send("d", key, note, 1);      // used to be t
    action = 1;
  }

  // soft touch data
  if (touched)
  {
    arg_state = mapSoft(key, filtered);
    send("s", key, note, arg_state);
    action = 1;
  }

  // update state orikeys table
  orikeys[key].state = action;
  orikeys[key].last = filtered;
  
  // bend data
  if (filtered < orikeys[key].bendLO)
  {
    filtered = orikeys[key].bendLO;
  }
    
  if (filtered > orikeys[key].bendHI)
  {
    filtered = orikeys[key].bendHI;
  }
  long longnormal = map(filtered, orikeys[key].bendLO, orikeys[key].bendHI, 0, ARR_LEN(bendLinear)-1);
  normalised = map(filtered, orikeys[key].bendLO, orikeys[key].bendHI, 0, ARR_LEN(bendLinear)-1);
  arg_state = bendLinear[(int)longnormal]; // convert normal value to linear value
  if (filtered != last)
  {
    //sendOSC(msg_str, arg_state, arg_note); // send every time
    send("b", key, note, arg_state);
  }
  
} 

uint8_t getActionState (uint8_t key, uint16_t filtered)
{
  byte action = 0;
  if (filtered < orikeys[key].bendHI)
  {
    action = 3;
  }
  if (filtered > orikeys[key].bendLO)
  {
    action = 3;
  }
  if (filtered < (orikeys[key].bendLO - LO_HARD_OFFSET))
  {
    action = 2;
  }
  if (filtered < orikeys[key].hard)
  {
    action = 1;
  }
  if (filtered > orikeys[key].bendHI)
  {
    action = 0;
  }
  return action;
}

void resetHardSoft() 
{
  uint8_t key = 0;
  for (key=0; key<PINCOUNT; key++)
  {
    orikeys[key].hard = 1024;
    orikeys[key].soft = 0;
  }
}

void doCalibrate()
{
  resetHardSoft();
#if MPRCOUNT > 0
  MPR121_0.autoSetElectrodes(3300, false);
  if (MPR121_0.isInited())
  {
    reportMPRStatus("/mpr/A", 1);
  }
  else
  {
    reportMPRStatus("/mpr/A", 0);
  }
  delay(500);
#endif
#if MPRCOUNT > 1
  MPR121_1.autoSetElectrodes(3300, false);
  if (MPR121_1.isInited())
  {
    reportMPRStatus("/mpr/B", 1);
  }
  else
  {
    reportMPRStatus("/mpr/B", 0);
  }
  delay(500);
#endif
#if MPRCOUNT > 2
  MPR121_2.autoSetElectrodes(3300, false);
  if (MPR121_2.isInited())
  {
    reportMPRStatus("/mpr/C", 1);
  }
  else
  {
    reportMPRStatus("/mpr/C", 0);
  }
  delay(500);
#endif
#if MPRCOUNT > 3
  MPR121_3.autoSetElectrodes(3300, false);
  if (MPR121_3.isInited())
  {
    reportMPRStatus("/mpr/D", 1);
  }
  else
  {
    reportMPRStatus("/mpr/D", 0);
  }
  delay(500);
#endif
#if OSC == 1
  sendOSC("/set/calibrated", 1);
#endif // osc

}

void setBendAllKeys(char param[4])
{
  uint8_t key;
  //Serial.println("bend all key");
  for (key = 0; key < PINCOUNT; key++)
  {
    setBendSingleKey(key, param);
  }
  //delay(1000);
}

void setBendSingleKey(uint8_t key, char param[4])
{
  uint16_t val;
  char chan[20];
  //val = orikeys[key].last;

  val = MPRpanels[orikeys[key].panel].mpr->getFilteredData(orikeys[key].pin);
  
  if (param == "hard")
  {
    orikeys[key].hard = val;
  }
  if (param == "belo")
  {
    orikeys[key].bendLO = val;
  }
  if (param == "loHS")
  {
    orikeys[key].bendLO = val;
    if (orikeys[key].bendLO > 200)
    {
      orikeys[key].hard = val - LO_HARD_OFFSET;
    }
  }
  if (param == "behi")
  {
    orikeys[key].bendHI = val;
  }
  #if OSC == 1
    sprintf(chan, "/key/%s/%d", param, key);
    //Serial.println(chan);
    sendOSC(chan, val);
  #else
    if (mode==MODE_SERIAL_DEBUG || mode==MODE_SERIAL_DEBUG_RAW)
    {
      Serial.print("key: ");
      Serial.print(key);
      Serial.print(" val: ");
      Serial.println(val);
      if (param == "loHS")
      {
        Serial.print("key: hard val: ");
        Serial.println(val - LO_HARD_OFFSET);
      }
    }
  #endif
}

void reportMPRStatus(char channel[20], uint8_t value)
{
#if OSC == 1
  sendOSC(channel, value);
#endif
#if MIDI == 1
  if (channel == "/mpr/A")
  {
    controlChange(0, 107, value);
  }
  if (channel == "/mpr/B")
  {
    controlChange(0, 106, value);
  }
  if (channel == "/mpr/C")
  {
    controlChange(0, 105, value);
  }
  if (channel == "/mpr/D")
  {
    controlChange(0, 104, value);
  }
  Serial.print(channel);
  Serial.print(": ");
  Serial.println(value);
#endif
}

void setScale(uint8_t value) {
  // not yet implented
  //#if OSC == 1
    // add osc handlers
  //#endif
  #if MIDI == 1
    controlChange(0, 108, value);
  #endif
}

void serialDebug (byte key, byte raw) 
{
  uint8_t note  = orikeys[key].note;
  uint8_t pin   = orikeys[key].pin;
  #if REVERSED
    pin   = reverse_logical[orikeys[key].pin];
  #else
    pin   = orikeys[key].pin;
  #endif
  uint8_t panel = orikeys[key].panel;
  uint8_t state = orikeys[key].state;
  uint8_t short_filtered;
  uint16_t filtered = MPRpanels[panel].mpr->getFilteredData(pin);
  bool touchData = MPRpanels[panel].mpr->getTouchData(pin);
  orikeys[key].last = filtered;
  char output[4];
  char f;
   
  byte action = 0;

  if (filtered < orikeys[key].bendHI)
    action = 3;  // BEND 
  if (filtered > orikeys[key].bendLO)
    action = 3;  // BEND
  if (filtered < (orikeys[key].bendLO)-SOFT_LOW_OFFSET)
    action = 2;  // SOFT
  // if (filtered < orikeys[key].soft)   {    action = 2;    }
  if (filtered < orikeys[key].hard)
    action = 1;  // PRESS 
  if (filtered > orikeys[key].bendHI)
    action = 0;  // OFF 
  if (filtered < 30)
    action = 10; // BOTTOMED OUT  

  f = '.';
  switch (action)
  {
    case 0: // OFF 
      f = '.';
      break;
    case 1: // PRESS 
      f = 'x';
      break;
    case 2: // SOFT
      f = 'S';
      bendDebug(filtered, orikeys[key].hard, orikeys[key].bendLO);
      //f = '-';
      break;
    case 3: // BEND
      f = 'B';
      bendDebug(filtered, orikeys[key].bendLO, orikeys[key].bendHI);
      //f = '~';
      break;
    case 10:
      f = '#';
      break;
  }

  if (raw)
  {
    debug_raw(filtered);
  }  else 
  {
    debug_short_filtered (f);
  }
  if (key==PINCOUNT-1)
    Serial.println();
}

void debug_short(uint8_t n, uint8_t f)
{
  Serial.print(n);
  Serial.print(": ");
  Serial.print(f);
  Serial.print(" | ");
}

void debug_short_char(char f)
{
  Serial.print(f);
}

void debug_short_filtered(char f)
{
  Serial.print(f);
  Serial.print(" ");
}

void debug_raw(uint16_t f)
{
  Serial.print(f);
  Serial.print(" ");
}

void debug_raw_bytes(uint8_t f)
{
  Serial.write(f);
}

void checkbuttons()
{
  byte b = 0;
  byte active = 0;
  for (b = 0; b < BTN_COUNT; b++)
  {
    if (digitalRead(buttons[b].pin))
    {
      // on but check if its already been switched on
      // Serial.print(b);
      // Serial.print(" ");
      // Serial.println(digitalRead(buttons[b].pin));
      if (buttons[b].state == 0)
      {
        buttons[b].state = 2;
        #if ORIGAMI == KRESLING
          digitalWrite(buttons[b].led, LOW);
        #endif
      }
    }
    else
    {
      // off
      buttons[b].state = 0;
      #if ORIGAMI == KRESLING
        digitalWrite(buttons[b].led, HIGH);
      #endif
    }
  }
  // check for an active button
  for (b = 0; b < BTN_COUNT; b++)
  {
    if (buttons[b].state == 2)
    {
      buttons[b].state = 1;
      activebutton = b;
      active = 1;
    }
  }
  if (active)
  {
    switch (activebutton)
    {
    case 0:
      // Serial.println("CALIBRATE");
      doCalibrate();
      break;
    case 1:
      // Serial.println("bend HI");
      setBendAllKeys("behi"); // set bend hi
      break;
    case 2:
      // Serial.println("bend LO");
      setBendAllKeys("loHS"); // use loHS as default bendlo - sets hard press as well
      break;
    case 3:
      // Function 1 -- note up
      // Serial.println("FUNCTION 1");
      // changeRoot(1);
      // Serial.println("bend HI");
      setBendAllKeys("behi"); // set bend hi
      break;
    case 4:
      // Function 2 -- note down
      // Serial.println("FUNCTION 2");
      // changeRoot(0);
      // Serial.println("bend HI");
      mode ++;
      if (mode > 6) {
        mode = 0;
      }
      Serial.println(mode);
      #if MIDI == 1
        controlChange(0, 110, mode);
      #endif
      //setBendAllKeys("behi"); // set bend hi
      break;
    }
  }
}

uint8_t bendDebug(int filtered, int lo, int hi)
{
  uint8_t sf;
  sf = longmap(filtered, lo, hi, 0, 9);
  Serial.print(sf);
}

long longmap(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setupMPR()
{
  // char errStr[22] = { };
  // char txt[256] = { };
  #if MPRCOUNT > 0
    MPR121_0.begin(0x5A);
    MPR121_0.goFast();                             // enable fast I2C mode (set I2C clock to 400kHz)
    MPR121_0.setSamplePeriod(SAMPLE_INTERVAL_1MS); // time between capacitive readings (the faster, the more power it needs)
    MPR121_0.setFFI(FFI_6);                        // first filter iterations (number of samples taken)
    MPR121_0.setSFI(SFI_4);                        // second filter iterations (number of samples taken)
    #if ORIGAMI == YOSHIMURA
      #if TEXILE == 0
      MPR121_0.setNumEnabledElectrodes(8);         // version 2 has electrodes from 0-7 connected to the picoblade connector
      #endif
    #endif
  #endif
  #if MPRCOUNT > 1
    MPR121_1.begin(0x5B);
    MPR121_1.goFast();                             // enable fast I2C mode (set I2C clock to 400kHz)
    MPR121_1.setSamplePeriod(SAMPLE_INTERVAL_1MS); // time between capacitive readings (the faster, the more power it needs)
    MPR121_1.setFFI(FFI_6);                        // first filter iterations (number of samples taken)
    MPR121_1.setSFI(SFI_4);                        // second filter iterations (number of samples taken)
  #endif
  #if MPRCOUNT > 2
    MPR121_2.begin(0x5C);
    MPR121_2.goFast();                             // enable fast I2C mode (set I2C clock to 400kHz)
    MPR121_2.setSamplePeriod(SAMPLE_INTERVAL_1MS); // time between capacitive readings (the faster, the more power it needs)
    MPR121_2.setFFI(FFI_6);                        // first filter iterations (number of samples taken)
    MPR121_2.setSFI(SFI_4);                        // second filter iterations (number of samples taken)
  #endif
  #if MPRCOUNT > 3
    MPR121_3.begin(0x5D);
    MPR121_3.goFast();                             // enable fast I2C mode (set I2C clock to 400kHz)
    MPR121_3.setSamplePeriod(SAMPLE_INTERVAL_1MS); // time between capacitive readings (the faster, the more power it needs)
    MPR121_3.setFFI(FFI_6);                        // first filter iterations (number of samples taken)
    MPR121_3.setSFI(SFI_4);                        // second filter iterations (number of samples taken)
  #endif
}

// --------------------------------------------------------------------     FOLDING STRUCTURE

// link panels together into groups
#ifdef PANELGROUPS
// -------------------------------------------------------------------     LINEAR MAPPING
// takes input from the normalised value

// original version
// uint8_t bendLinear[255] = {0,0,1,2,3,4,4,5,6,6,7,7,8,8,9,9,9,10,10,10,11,11,11,11,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,15,15,15,16,16,16,16,16,16,16,17,17,17,17,17,18,18,18,18,18,19,19,19,19,20,20,20,20,21,21,21,22,22,22,22,23,23,23,24,24,24,25,25,25,26,26,26,27,27,27,28,28,28,29,29,29,30,30,30,31,31,31,32,32,32,33,33,33,34,34,34,35,35,35,36,36,36,37,37,37,38,38,38,39,39,39,40,40,40,41,41,41,42,42,42,43,43,43,44,44,45,45,45,46,46,47,47,48,48,48,49,49,50,51,51,52,52,53,53,54,55,55,56,57,58,58,59,60,61,62,63,64,65,66,67,68,69,70,72,73,74,76,77,79,80,82,84,85,87,89,91,93,95,97,99,102,104,107,109,112,115,118,121,124,127,130,133,137,140,144,148,152,156,160,164,169,174,178,183,188,193,199,204,210,216,222,228,234,241,248};

// from calculated function
// uint8_t bendLinear[255] = {0,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,4,4,4,4,4,4,5,5,5,5,5,5,6,6,6,6,6,6,7,7,7,7,7,8,8,8,8,8,9,9,9,9,9,10,10,10,10,10,11,11,11,11,11,12,12,12,12,12,13,13,13,13,13,14,14,14,14,15,15,15,15,15,16,16,16,16,17,17,17,17,18,18,18,18,19,19,19,19,20,20,20,20,21,21,21,21,22,22,22,22,23,23,23,24,24,24,24,25,25,25,26,26,26,27,27,27,27,28,28,28,29,29,29,30,30,30,31,31,31,32,32,32,33,33,34,34,34,35,35,35,36,36,37,37,37,38,38,39,39,40,40,40,41,41,42,42,43,43,44,44,45,45,46,46,47,47,48,48,49,49,50,50,51,52,52,53,53,54,55,55,56,57,57,58,59,59,60,61,62,62,63,64,65,66,67,68,68,69,70,71,72,73,74,75,77,78,79,80,81,83,84,85,87,88,90,92,93,95,97,99,101,103,106,108,111,114,117,120,124,128,133,138,145,152,161,173,191,225,255,200,179,165,155,147}
// from calculated function 0.8 second dwell time
// --------------------------------------------------------------------     FUNCTIONS

  void setPanelGroup(OSCMessage &msg)
  {
    if (msg.isInt(0)) {
      bool val = msg.getInt(0);
      // send feedback to PD
      if (val == 1)
      {
        usePanelGroups = true;
      } else {
        usePanelGroups = false;
      }
      sendOSC("/set/usePanelGroups", val);
    }
  }

  void updatePanelGroups()
  {
    uint8_t electrode = 0;
    uint16_t sum = 0;
    uint8_t activebits = 0;
    char route[10];
    uint8_t g;a
    uint8_t p;
    uint8_t bit;
    uint8_t last;
    uint8_t normalised;
    uint8_t last_pre_normal;
    // loop through panelgoups
    uint8_t panelCount = ARR_LEN(panelgroups);


    for (g = 0; g < panelCount; g++)
    {
      // loop through each byte in pattern
      // pattern bytes are a bit array set as a byte value
      activebits = 0;
      sum = 0;
      last = panelgroups[g].value;

      uint8_t patternsize = ARR_LEN(panelgroups[g].pattern);
      for (p = 0; p < patternsize; p++)
      {
        // char txt[16] = { '\0' };
        // snprintf(txt, sizeof(txt), "g=%d p=%d\n", g, p);
        // Serial.print(txt);
        if (panelgroups[g].pattern[p] == 0) continue;// skip if zero

        // each column in the oricordion has 8 electrodes
        // so the bit pattern of 6 x 8 = 48, describes the electrode values that make up the pattenrn
        for (bit = 0; bit < 8; bit++)
        {
          if (bitRead(panelgroups[g].pattern[p], bit))
          {
            activebits++;
            electrode = (p * 8) + bit;
            last_pre_normal = orikeys[electrode].last;
            if (orikeys[electrode].last < orikeys[electrode].bendLO)
            {
              last_pre_normal = orikeys[electrode].bendLO;
            }
            if (orikeys[electrode].last > orikeys[electrode].bendHI)
            {
              last_pre_normal = orikeys[electrode].bendHI;
            }
            normalised = map(last_pre_normal, orikeys[electrode].bendLO, orikeys[electrode].bendHI, 0, 254);
            normalised = bendLinear[(int)normalised];
            sum += normalised;
            // the bit array shows the panel/key number
          }
          //  Serial.println();
        }
      }

      float avg = (float)sum / (float)activebits;
      panelgroups[g].value = (uint8_t)(255.0f / 1024.0 * (float)avg);
      sprintf(route, "/g/%s", panelgroups[g].title);
      // Serial.print("value = ");
      // Serial.println(panelgroups[g].value);
      if (panelgroups[g].value != last)
      {
        sendOSC(route, panelgroups[g].value);
      }
    }
  }

#endif // END PANEL GROUP FUNCTIONS
