# oribotic instrument

the arduino has too little memory to code for both OSC and MIDI, so you have to choose one, and one instrument configuration

## building for different modes

see instrument_config.h

The top section of this file has two defined variables to change for different builds

This will run in MIDI mode
```
#define MIDI 1
#define OSC 0
```


This will run in OSC mode
```
#define MIDI 0
#define OSC 1
```

A bit further down in the file is:

```
#define KRESLING 1
#define YOSHIMURA 2
#define SUKI 3
#define ORIGAMI YOSHIMURA
```
update the last line, to KRESLING, YOSHIMURA or SUKI to suit the build of your instrument



