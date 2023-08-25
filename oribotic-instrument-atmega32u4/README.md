Oribotic Instruments Arduino project
=====================================

Arduino based firmware for Oribotic instruments

Dependencies
------------

The project uses the following open source libraries:
* [MPR121](https://github.com/BareConductive/mpr121): MPR121 capacitive touch sense IC (modified, included in this repo)
* [OSC](https://github.com/CNMAT/OSC): (Open Sound Control) communication
* [MIDIUSB](https://www.arduino.cc/reference/en/libraries/midiusb/): MIDI communication over USB

Install the following libraries via the Arduino IDE Library Manager: OSC, MIDIUSB

Configuration
-------------

Configuration is set via defines at the top of `instrument_config.h`

Important compile-time settings are:

* `OSC`: 0 - disable, 1 - build with OSC serial communication
* `MIDI`: 0 - disable, 1 - build with MIDI communication
* `DEBUG_LEVEL`: 0 - off, 1 - error, 2 - info, 3 - debug
* `ORIGAMI`: YOSHIMURA - 8 panel, SUKI - 12 panel, KRESLING - 48 panel 
* `ROOTNOTE`: root MIDI note

The Arduino has too little memory for both OSC and MIDI communication, so you have to choose one and one only.

For example, this will run in MIDI mode:

~~~C
#define MIDI 1
#define OSC 0
~~~

This will run in OSC mode:

~~~C
#define MIDI 0
#define OSC 1
~~~

A bit further down in the file is:

~~~C
#define KRESLING 1
#define YOSHIMURA 2
#define SUKI 3
#define ORIGAMI YOSHIMURA
~~~

Update the last line, to KRESLING, YOSHIMURA or SUKI to suit the build of your instrument.
