=======================
ALSA SoC Layer Overview
=======================

The overall project goal of the ALSA System on Chip (ASoC) layer is to
provide better ALSA support for embedded system-on-chip processors (e.g.
pxa2xx, au1x00, iMX, etc) and portable audio codecs.  Prior to the ASoC
subsystem there was some support in the kernel for SoC audio, however it
had some limitations:-

  * Codec drivers were often tightly coupled to the underlying SoC
    CPU. This is not ideal and leads to code duplication - for example,
    Linux had different wm8731 drivers for 4 different SoC platforms.

  * There was no standard method to signal user initiated audio events (e.g.
    Headphone/Mic insertion, Headphone/Mic detection after an insertion
    event). These are quite common events on portable devices and often require
    machine specific code to re-route audio, enable amps, etc., after such an
    event.

  * Drivers tended to power up the entire codec when playing (or
    recording) audio. This is fine for a PC, but tends to waste a lot of
    power on portable devices. There was also no support for saving
    power via changing codec oversampling rates, bias currents, etc.


ASoC Design
===========

The ASoC layer is designed to address these issues and provide the following
features :-

  * Codec independence. Allows reuse of codec drivers on other platforms
    and machines.

  * Easy I2S/PCM audio interface setup between codec and SoC. Each SoC
    interface and codec registers its audio interface capabilities with the
    core and are subsequently matched and configured when the application
    hardware parameters are known.

  * Dynamic Audio Power Management (DAPM). DAPM automatically sets the codec to
    its minimum power state at all times. This includes powering up/down
    internal power blocks depending on the internal codec audio routing and any
    active streams.

  * Pop and click reduction. Pops and clicks can be reduced by powering the
    codec up/down in the correct sequence (including using digital mute). ASoC
    signals the codec when to change power states.

  * Machine specific controls: Allow machines to add controls to the sound card
    (e.g. volume control for speaker amplifier).

To achieve all this, ASoC basically splits an embedded audio system into
multiple re-usable component drivers :-

  * Codec class drivers: The codec class driver is platform independent and
    contains audio controls, audio interface capabilities, codec DAPM
    definition and codec IO functions. This class extends to BT, FM and MODEM
    ICs if required. Codec class drivers should be generic code that can run
    on any architecture and machine.

  * Platform class drivers: The platform class driver includes the audio DMA
    engine driver, digital audio interface (DAI) drivers (e.g. I2S, AC97, PCM)
    and any audio DSP drivers for that platform.

  * Machine class driver: The machine driver class acts as the glue that
    describes and binds the other component drivers together to form an ALSA
    "sound card device". It handles any machine specific controls and
    machine level audio events (e.g. turning on an amp at start of playback).
