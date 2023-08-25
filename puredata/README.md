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
* oi abstraction library:
  - comport: serial port communication (IEM / Martin Peach, et al)
  - slip: SLIP byte protocol encoding/decoding (Martin Peach, et al)
* template patches (in additional to the above)
  - osc: original OSC object library (CNMAT, et al.)
  - rjlib: vanilla-compatible abstractions (RjDj project, Frank Barknecht, et al)

Install the following via Pd's built-in `Help->Find externals...`: comport, slip, osc. By default, they will be installed to `~/Pd/Documents/externals`.

Install rjlib manually:
1. Download as a ZIP from Github: https://github.com/rjdj/rjlib
2. Place the unzipped `rjlib` folder inside `~/Pd/Documents/externals`
3. Open Pd and add `~/Pd/Documents/externals/rjlib/rj` to your search path, either via `Settings->Path tab` (Pd 0.54+) or the `Path` window
