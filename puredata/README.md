Puredata
========

See intro video here: <https://youtu.be/8-rrHrH95Go>

Abstraction library
-------------------

The `oi` abstraction library is provided to wrap communication with the instrument, either over OSC via a serial port (desktop only) or MIDI (desktop and mobile). See `oi/README.txt` for details.

Puredata setup (Desktop)
------------------------

Developed with Puredata vanilla: http://puredata.info/downloads

Dependencies (external libraries and/or abstractions):
* OSC serial communication:
  - comport: serial port communication (IEM / Martin Peach, et al)
  - slip: SLIP byte protocol encoding/decoding (Martin Peach, et al)
* MIDI communication: none

Install the following via Pd's built-in `Help->Find externals...`: comport, slip. By default, they will be installed to `~/Pd/Documents/externals`.

_Note: If only using MIDI, the externals are not required._

PdParty Scenes (mobile)
-----------------------

Developed for iOS/iPadOS to be run in PdParty: http://danomatika.com/code/pdparty

PdParty scene template and test to be run on an Apple iDevice. Connect the instrument using a Lightning to USB adapter which includes an additonal Lightning power port.

_Note: Copy the `puredata/oi` directory into each scene before copying the scene to the mobile device._
