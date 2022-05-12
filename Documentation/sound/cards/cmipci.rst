=================================================
Brief Notes on C-Media 8338/8738/8768/8770 Driver
=================================================

Takashi Iwai <tiwai@suse.de>


Front/Rear Multi-channel Playback
---------------------------------

CM8x38 chip can use ADC as the second DAC so that two different stereo
channels can be used for front/rear playbacks.  Since there are two
DACs, both streams are handled independently unlike the 4/6ch multi-
channel playbacks in the section below.

As default, ALSA driver assigns the first PCM device (i.e. hw:0,0 for
card#0) for front and 4/6ch playbacks, while the second PCM device
(hw:0,1) is assigned to the second DAC for rear playback.

There are slight differences between the two DACs:

- The first DAC supports U8 and S16LE formats, while the second DAC
  supports only S16LE.
- The second DAC supports only two channel stereo.

Please note that the CM8x38 DAC doesn't support continuous playback
rate but only fixed rates: 5512, 8000, 11025, 16000, 22050, 32000,
44100 and 48000 Hz.

The rear output can be heard only when "Four Channel Mode" switch is
disabled.  Otherwise no signal will be routed to the rear speakers.
As default it's turned on.

.. WARNING::
  When "Four Channel Mode" switch is off, the output from rear speakers
  will be FULL VOLUME regardless of Master and PCM volumes [#]_.
  This might damage your audio equipment.  Please disconnect speakers
  before your turn off this switch.


.. [#]
  Well.. I once got the output with correct volume (i.e. same with the
  front one) and was so excited.  It was even with "Four Channel" bit
  on and "double DAC" mode.  Actually I could hear separate 4 channels
  from front and rear speakers!  But.. after reboot, all was gone.
  It's a very pity that I didn't save the register dump at that
  time..  Maybe there is an unknown register to achieve this...

If your card has an extra output jack for the rear output, the rear
playback should be routed there as default.  If not, there is a
control switch in the driver "Line-In As Rear", which you can change
via alsamixer or somewhat else.  When this switch is on, line-in jack
is used as rear output.

There are two more controls regarding to the rear output.
The "Exchange DAC" switch is used to exchange front and rear playback
routes, i.e. the 2nd DAC is output from front output.


4/6 Multi-Channel Playback
--------------------------

The recent CM8738 chips support for the 4/6 multi-channel playback
function.  This is useful especially for AC3 decoding.

When the multi-channel is supported, the driver name has a suffix
"-MC" such like "CMI8738-MC6".  You can check this name from
/proc/asound/cards.

When the 4/6-ch output is enabled, the second DAC accepts up to 6 (or
4) channels.  While the dual DAC supports two different rates or
formats, the 4/6-ch playback supports only the same condition for all
channels.  Since the multi-channel playback mode uses both DACs, you
cannot operate with full-duplex.

The 4.0 and 5.1 modes are defined as the pcm "surround40" and "surround51"
in alsa-lib.  For example, you can play a WAV file with 6 channels like
::

	% aplay -Dsurround51 sixchannels.wav

For programming the 4/6 channel playback, you need to specify the PCM
channels as you like and set the format S16LE.  For example, for playback
with 4 channels,
::

	snd_pcm_hw_params_set_access(pcm, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
	    // or mmap if you like
	snd_pcm_hw_params_set_format(pcm, hw, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(pcm, hw, 4);

and use the interleaved 4 channel data.

There are some control switches affecting to the speaker connections:

Line-In Mode
	an enum control to change the behavior of line-in
	jack.  Either "Line-In", "Rear Output" or "Bass Output" can
	be selected.  The last item is available only with model 039
	or newer. 
	When "Rear Output" is chosen, the surround channels 3 and 4
	are output to line-in jack.
Mic-In Mode
	an enum control to change the behavior of mic-in
	jack.  Either "Mic-In" or "Center/LFE Output" can be
	selected. 
	When "Center/LFE Output" is chosen, the center and bass
	channels (channels 5 and 6) are output to mic-in jack. 

Digital I/O
-----------

The CM8x38 provides the excellent SPDIF capability with very cheap
price (yes, that's the reason I bought the card :)

The SPDIF playback and capture are done via the third PCM device
(hw:0,2).  Usually this is assigned to the PCM device "spdif".
The available rates are 44100 and 48000 Hz.
For playback with aplay, you can run like below:
::

	% aplay -Dhw:0,2 foo.wav

or

::

	% aplay -Dspdif foo.wav

24bit format is also supported experimentally.

The playback and capture over SPDIF use normal DAC and ADC,
respectively, so you cannot playback both analog and digital streams
simultaneously.

To enable SPDIF output, you need to turn on "IEC958 Output Switch"
control via mixer or alsactl ("IEC958" is the official name of
so-called S/PDIF).  Then you'll see the red light on from the card so
you know that's working obviously :)
The SPDIF input is always enabled, so you can hear SPDIF input data
from line-out with "IEC958 In Monitor" switch at any time (see
below).

You can play via SPDIF even with the first device (hw:0,0),
but SPDIF is enabled only when the proper format (S16LE), sample rate
(441100 or 48000) and channels (2) are used.  Otherwise it's turned
off.  (Also don't forget to turn on "IEC958 Output Switch", too.)


Additionally there are relevant control switches:

IEC958 Mix Analog
	Mix analog PCM playback and FM-OPL/3 streams and
	output through SPDIF.  This switch appears only on old chip
	models (CM8738 033 and 037).

	Note: without this control you can output PCM to SPDIF.
	This is "mixing" of streams, so e.g. it's not for AC3 output
	(see the next section).

IEC958 In Select
	Select SPDIF input, the internal CD-in (false)
	and the external input (true).

IEC958 Loop
	SPDIF input data is loop back into SPDIF
	output (aka bypass)

IEC958 Copyright
	Set the copyright bit.

IEC958 5V
	Select 0.5V (coax) or 5V (optical) interface.
	On some cards this doesn't work and you need to change the
	configuration with hardware dip-switch.

IEC958 In Monitor
	SPDIF input is routed to DAC.

IEC958 In Phase Inverse
	Set SPDIF input format as inverse.
	[FIXME: this doesn't work on all chips..]

IEC958 In Valid
	Set input validity flag detection.

Note: When "PCM Playback Switch" is on, you'll hear the digital output
stream through analog line-out.


The AC3 (RAW DIGITAL) OUTPUT
----------------------------

The driver supports raw digital (typically AC3) i/o over SPDIF.  This
can be toggled via IEC958 playback control, but usually you need to
access it via alsa-lib.  See alsa-lib documents for more details.

On the raw digital mode, the "PCM Playback Switch" is automatically
turned off so that non-audio data is heard from the analog line-out.
Similarly the following switches are off: "IEC958 Mix Analog" and
"IEC958 Loop".  The switches are resumed after closing the SPDIF PCM
device automatically to the previous state.

On the model 033, AC3 is implemented by the software conversion in
the alsa-lib.  If you need to bypass the software conversion of IEC958
subframes, pass the "soft_ac3=0" module option.  This doesn't matter
on the newer models.


ANALOG MIXER INTERFACE
----------------------

The mixer interface on CM8x38 is similar to SB16.
There are Master, PCM, Synth, CD, Line, Mic and PC Speaker playback
volumes.  Synth, CD, Line and Mic have playback and capture switches,
too, as well as SB16.

In addition to the standard SB mixer, CM8x38 provides more functions.
- PCM playback switch
- PCM capture switch (to capture the data sent to DAC)
- Mic Boost switch
- Mic capture volume
- Aux playback volume/switch and capture switch
- 3D control switch


MIDI CONTROLLER
---------------

With CMI8338 chips, the MPU401-UART interface is disabled as default.
You need to set the module option "mpu_port" to a valid I/O port address
to enable MIDI support.  Valid I/O ports are 0x300, 0x310, 0x320 and
0x330.  Choose a value that doesn't conflict with other cards.

With CMI8738 and newer chips, the MIDI interface is enabled by default
and the driver automatically chooses a port address.

There is *no* hardware wavetable function on this chip (except for
OPL3 synth below).
What's said as MIDI synth on Windows is a software synthesizer
emulation.  On Linux use TiMidity or other softsynth program for
playing MIDI music.


FM OPL/3 Synth
--------------

The FM OPL/3 is also enabled as default only for the first card.
Set "fm_port" module option for more cards.

The output quality of FM OPL/3 is, however, very weird.
I don't know why..

CMI8768 and newer chips do not have the FM synth.


Joystick and Modem
------------------

The legacy joystick is supported.  To enable the joystick support, pass
joystick_port=1 module option.  The value 1 means the auto-detection.
If the auto-detection fails, try to pass the exact I/O address.

The modem is enabled dynamically via a card control switch "Modem".


Debugging Information
---------------------

The registers are shown in /proc/asound/cardX/cmipci.  If you have any
problem (especially unexpected behavior of mixer), please attach the
output of this proc file together with the bug report.
