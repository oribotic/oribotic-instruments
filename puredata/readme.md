Puredata
========

See intro video here:
https://youtu.be/8-rrHrH95Go

Very soon, we'll have desktop and iOS versions of each patch, these are all desktop versions

Puredata setup (Desktop)
------------------------

Developed with Puredata vanilla: http://puredata.info/downloads

Dependencies (external libraries and/or abstractions):
* comport: serial port communication (IEM / Martin Peach, et al)
* slip: SLIP byte protocol encoding/decoding (Martin Peach, et al)
* rjlib: vanilla-compatible abstractions (RjDj project, Frank Barknecht, et al)

Install the following via Pd's built-in `Help->Find externals...`: comport, slip, osc. By efault, they will be installed to `~/Pd/Documents/externals`.

Install rjlib manually:
1. Download as a ZIP from Github: https://github.com/rjdj/rjlib
2. Place the unzipped `rjlib` folder inside `~/Pd/Documents/externals`
3. Open Pd and add `~/Pd/Documents/externals/rjlib/rj` to your search path, either via `Settings->Path tab` (Pd 0.54+) or the `Path` window

