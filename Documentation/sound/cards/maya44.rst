=================================
Notes on Maya44 USB Audio Support
=================================

.. note::
   The following is the original document of Rainer's patch that the
   current maya44 code based on.  Some contents might be obsoleted, but I
   keep here as reference -- tiwai

Feb 14, 2008

Rainer Zimmermann <mail@lightshed.de>
 
STATE OF DEVELOPMENT
====================

This driver is being developed on the initiative of Piotr Makowski (oponek@gmail.com) and financed by Lars Bergmann.
Development is carried out by Rainer Zimmermann (mail@lightshed.de).

ESI provided a sample Maya44 card for the development work.

However, unfortunately it has turned out difficult to get detailed programming information, so I (Rainer Zimmermann) had to find out some card-specific information by experiment and conjecture. Some information (in particular, several GPIO bits) is still missing.

This is the first testing version of the Maya44 driver released to the alsa-devel mailing list (Feb 5, 2008).


The following functions work, as tested by Rainer Zimmermann and Piotr Makowski:

- playback and capture at all sampling rates
- input/output level
- crossmixing
- line/mic switch
- phantom power switch
- analogue monitor a.k.a bypass


The following functions *should* work, but are not fully tested:

- Channel 3+4 analogue - S/PDIF input switching
- S/PDIF output
- all inputs/outputs on the M/IO/DIO extension card
- internal/external clock selection


*In particular, we would appreciate testing of these functions by anyone who has access to an M/IO/DIO extension card.*


Things that do not seem to work:

- The level meters ("multi track") in 'alsamixer' do not seem to react to signals in (if this is a bug, it would probably be in the existing ICE1724 code).

- Ardour 2.1 seems to work only via JACK, not using ALSA directly or via OSS. This still needs to be tracked down.


DRIVER DETAILS
==============

the following files were added:

* pci/ice1724/maya44.c - Maya44 specific code
* pci/ice1724/maya44.h
* pci/ice1724/ice1724.patch
* pci/ice1724/ice1724.h.patch - PROPOSED patch to ice1724.h (see SAMPLING RATES)
* i2c/other/wm8776.c - low-level access routines for Wolfson WM8776 codecs 
* include/wm8776.h


Note that the wm8776.c code is meant to be card-independent and does not actually register the codec with the ALSA infrastructure.
This is done in maya44.c, mainly because some of the WM8776 controls are used in Maya44-specific ways, and should be named appropriately.


the following files were created in pci/ice1724, simply #including the corresponding file from the alsa-kernel tree:

* wtm.h
* vt1720_mobo.h
* revo.h
* prodigy192.h
* pontis.h
* phase.h
* maya44.h
* juli.h
* aureon.h
* amp.h
* envy24ht.h
* se.h
* prodigy_hifi.h


*I hope this is the correct way to do things.*


SAMPLING RATES
==============

The Maya44 card (or more exactly, the Wolfson WM8776 codecs) allow a maximum sampling rate of 192 kHz for playback and 92 kHz for capture.

As the ICE1724 chip only allows one global sampling rate, this is handled as follows:

* setting the sampling rate on any open PCM device on the maya44 card will always set the *global* sampling rate for all playback and capture channels.

* In the current state of the driver, setting rates of up to 192 kHz is permitted even for capture devices.

*AVOID CAPTURING AT RATES ABOVE 96kHz*, even though it may appear to work. The codec cannot actually capture at such rates, meaning poor quality.


I propose some additional code for limiting the sampling rate when setting on a capture pcm device. However because of the global sampling rate, this logic would be somewhat problematic.

The proposed code (currently deactivated) is in ice1712.h.patch, ice1724.c and maya44.c (in pci/ice1712).


SOUND DEVICES
=============

PCM devices correspond to inputs/outputs as follows (assuming Maya44 is card #0):

* hw:0,0 input - stereo, analog input 1+2
* hw:0,0 output - stereo, analog output 1+2
* hw:0,1 input - stereo, analog input 3+4 OR S/PDIF input
* hw:0,1 output - stereo, analog output 3+4 (and SPDIF out)


NAMING OF MIXER CONTROLS
========================

(for more information about the signal flow, please refer to the block diagram on p.24 of the ESI Maya44 manual, or in the ESI windows software).


PCM
    (digital) output level for channel 1+2
PCM 1
    same for channel 3+4

Mic Phantom+48V
    switch for +48V phantom power for electrostatic microphones on input 1/2.

    Make sure this is not turned on while any other source is connected to input 1/2.
    It might damage the source and/or the maya44 card.

Mic/Line input
    if switch is on, input jack 1/2 is microphone input (mono), otherwise line input (stereo).

Bypass
    analogue bypass from ADC input to output for channel 1+2. Same as "Monitor" in the windows driver.
Bypass 1
    same for channel 3+4.

Crossmix
    cross-mixer from channels 1+2 to channels 3+4
Crossmix 1
    cross-mixer from channels 3+4 to channels 1+2

IEC958 Output
    switch for S/PDIF output.

    This is not supported by the ESI windows driver.
    S/PDIF should output the same signal as channel 3+4. [untested!]


Digitial output selectors
    These switches allow a direct digital routing from the ADCs to the DACs.
    Each switch determines where the digital input data to one of the DACs comes from.
    They are not supported by the ESI windows driver.
    For normal operation, they should all be set to "PCM out".

H/W
    Output source channel 1
H/W 1
    Output source channel 2
H/W 2
    Output source channel 3
H/W 3
    Output source channel 4

H/W 4 ... H/W 9
    unknown function, left in to enable testing.

    Possibly some of these control S/PDIF output(s).
    If these turn out to be unused, they will go away in later driver versions.

Selectable values for each of the digital output selectors are:

PCM out
	DAC output of the corresponding channel (default setting)
Input 1 ... Input 4
	direct routing from ADC output of the selected input channel

