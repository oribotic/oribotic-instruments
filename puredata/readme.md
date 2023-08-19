Puredata
========

See intro video here:
https://youtu.be/8-rrHrH95Go

Very soon, we'll have desktop and iOS versions of each patch, these are all desktop versions

Abstraction library
-------------------

The `oi` abstraction library is provided to wrap communication with the instrument, either over OSC via a serial port (desktop only) or MIDI (desktop and mobile). The main high-level abstractions are:

* `oi.o.y8`: 8 panel yoshimura instrument serial OSC receiver
* `oi.m.y8`: 8 panel yoshimura instrument MIDI receiver

The naming scheme follows: `oi.o.*` for OSC serial-specific and `oi.m.*` for MIDI-specific abstractions. Usage between either communication type is designed to be as similar as possible.

Additionally, the lower-level abstractions can be utilized individually, if needed. For example, `oi.m.panel` can be used to receive control input for a single panel over MIDI.

Puredata setup (Desktop)
------------------------

Developed with Puredata vanilla: http://puredata.info/downloads

Dependencies (external libraries and/or abstractions):
* oi abstraction library:
  - comport: serial port communication (IEM / Martin Peach, et al)
  - slip: SLIP byte protocol encoding/decoding (Martin Peach, et al)
* template patches (in additional to the above)
* rjlib: vanilla-compatible abstractions (RjDj project, Frank Barknecht, et al)

Install the following via Pd's built-in `Help->Find externals...`: comport, slip, osc. By default, they will be installed to `~/Pd/Documents/externals`.

Install rjlib manually:
1. Download as a ZIP from Github: https://github.com/rjdj/rjlib
2. Place the unzipped `rjlib` folder inside `~/Pd/Documents/externals`
3. Open Pd and add `~/Pd/Documents/externals/rjlib/rj` to your search path, either via `Settings->Path tab` (Pd 0.54+) or the `Path` window

