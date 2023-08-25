# Oribotic instruments Arduino project

Arduino based firmware for Oribotic instruments

## Communication type

Communication to the host system can be either OSC serial or MIDI:

* OSC serial: Open Sound Control-style messaging using the SLIP protocol over a serial port
* MIDI: 

The communication type is set when building the firmware and cannot be changed at runtime.

See `comms-spec.md` for messaging details.

## Instrument type

Available instrument types are based on the number of touch panels. Naming is based on origami fold types.

* yoshimura: 8 panels
* suki: 12 panels
* kresling: 48 panels

The communication type is set when building the firmware and cannot be changed at runtime.

## Configuration

Basic configuration can be set in `instrument_config.h`. Important compile-time settings are:

* OSC: 0 - disable, 1 - build with OSC serial communication
* MIDI: 0 - disable, 1 - build with MIDI communication
* DEBUG_LEVEL: 0 - off, 1 - error, 2 - info, 3 - debug
* ORIGAMI: YOSHIMURA - 8 panel, SUKI - 12 panel, KRESLING - 48 panel 
* ROOTNOTE: root MIDI note

_Note: Only a single communication type should be enabled when building, ie. OSC 1 and MIDI 1 **not** OSC 1 and MIDI 1, etc._
