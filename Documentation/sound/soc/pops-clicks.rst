=====================
Audio Pops and Clicks
=====================

Pops and clicks are unwanted audio artifacts caused by the powering up and down
of components within the audio subsystem. This is noticeable on PCs when an
audio module is either loaded or unloaded (at module load time the sound card is
powered up and causes a popping noise on the speakers).

Pops and clicks can be more frequent on portable systems with DAPM. This is
because the components within the subsystem are being dynamically powered
depending on the audio usage and this can subsequently cause a small pop or
click every time a component power state is changed.


Minimising Playback Pops and Clicks
===================================

Playback pops in portable audio subsystems cannot be completely eliminated
currently, however future audio codec hardware will have better pop and click
suppression.  Pops can be reduced within playback by powering the audio
components in a specific order. This order is different for startup and
shutdown and follows some basic rules:-
::

   Startup Order :- DAC --> Mixers --> Output PGA --> Digital Unmute
  
   Shutdown Order :- Digital Mute --> Output PGA --> Mixers --> DAC

This assumes that the codec PCM output path from the DAC is via a mixer and then
a PGA (programmable gain amplifier) before being output to the speakers.


Minimising Capture Pops and Clicks
==================================

Capture artifacts are somewhat easier to get rid as we can delay activating the
ADC until all the pops have occurred. This follows similar power rules to
playback in that components are powered in a sequence depending upon stream
startup or shutdown.
::

   Startup Order - Input PGA --> Mixers --> ADC
  
   Shutdown Order - ADC --> Mixers --> Input PGA


Zipper Noise
============
An unwanted zipper noise can occur within the audio playback or capture stream
when a volume control is changed near its maximum gain value. The zipper noise
is heard when the gain increase or decrease changes the mean audio signal
amplitude too quickly. It can be minimised by enabling the zero cross setting
for each volume control. The ZC forces the gain change to occur when the signal
crosses the zero amplitude line.
