# wrx - Schedule your weather receivers

#### The wrx program does:

* starting reception jobs at given time
* tuning the radio
* showing progress information
* showing waterfall diagramm
* decoding RTTY or NAVTEX directly, other types as postprocess
* starting postprocessing
* browsing thru results
* showing results

Tuning the radio, postprocessing and showing results is done by calling external tools. So you can 
freely choose your favorite picture viewer, audio player, decoder, radio controll program. 

You can controll any models and any count of radios. <br>
The minimal prerequisition for your radio is, that it's output can be read from a sound card.

For configuration details have a look into the doc directory.

The examples directory contains any tool scripts and a configuration for the Baltic sea. You can take this as hint to build your own configuration.

**wrx** is running on Linux systems, tested on PC and SBC's, Raspberry Pi and ODROID

### Building

* install development packages for qt5, fftw3, alsa, libsamplerate
* build libFSKDecoder as static
* edit mksymlinks pointing to your libFSKDecoder directory
* chmod u+x mksymlinks, run mksymlinks
* qmake-qt5 (or qmake)
* make
* write a configuration file ~/.wrx/wrx-config.ini
