oi abstraction library
======================

Oribotic foldable instrument Puredata abstractions

Copyright (C) 2023 Dan Wilcox, Matthew Gardiner, Oribotic Instruments team

GNU GPL Version 3

Description
-----------

The oi abstraction library is provided to wrap communication with Oribotic instruments, either over OSC via a serial port (desktop only) or MIDI (desktop and mobile). The main high-level abstractions are:

* oi.o.instrument: N-panel instrument serial OSC receiver, output range 0-255
* oi.m.instrument: N-panel instrument MIDI receiver, output range 0-127

The naming scheme follows: "oi.o." for OSC serial-specific and "oi.m." for MIDI-specific abstractions. Usage between either communication type is designed to be as similar as possible.

Additionally, the lower-level abstractions can be utilized individually, if needed. For example, oi.m.panel can be used to receive control input for a single panel over MIDI.

Puredata setup (Desktop)
------------------------

Developed with Puredata vanilla: http://puredata.info/downloads

Dependencies (external libraries and/or abstractions):
* OSC serial communication:
  - comport: serial port communication (IEM / Martin Peach, et al)
  - slip: SLIP byte protocol encoding/decoding (Martin Peach, et al)
* MIDI communication: none

Install the following via Pd's built-in Help->Find externals...: comport, slip

By default, they will be installed to ~/Pd/Documents/externals

Note: If only using MIDI, the externals are not required.

Mobile
------

Developed for PdParty: http://danomatika.com/code/pdparty

For using an Oribotic instrument with PdParty (iOS) the instrument firmware should be compiled for MIDI communication. Receive from the instrument using the "oi.m" abstractions.
