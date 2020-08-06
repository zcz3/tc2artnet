TC2ArtNet
=========

A cross platform tool which converts Linear timecode (LTC) and MIDI timecode
(MTC) to Art-Net timecode.

LTC can be received using any soundcard, such as a PC's built in audio input.

MTC is currently only supported on Windows, and can be received using any USB
MIDI or virtual device (e.g. loopback, rtpMIDI).

Art-Net timecode can be transmitted to other applications on the same PC and/or
over the network.

Building
--------

Requires Qt and [libltc](https://github.com/x42/libltc). Uses qmake.

License
-------

GPLv3. See LICENSE.txt.
