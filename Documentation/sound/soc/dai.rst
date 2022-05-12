==================================
ASoC Digital Audio Interface (DAI)
==================================

ASoC currently supports the three main Digital Audio Interfaces (DAI) found on
SoC controllers and portable audio CODECs today, namely AC97, I2S and PCM.


AC97
====

AC97 is a five wire interface commonly found on many PC sound cards. It is
now also popular in many portable devices. This DAI has a reset line and time
multiplexes its data on its SDATA_OUT (playback) and SDATA_IN (capture) lines.
The bit clock (BCLK) is always driven by the CODEC (usually 12.288MHz) and the
frame (FRAME) (usually 48kHz) is always driven by the controller. Each AC97
frame is 21uS long and is divided into 13 time slots.

The AC97 specification can be found at :
https://www.intel.com/p/en_US/business/design


I2S
===

I2S is a common 4 wire DAI used in HiFi, STB and portable devices. The Tx and
Rx lines are used for audio transmission, while the bit clock (BCLK) and
left/right clock (LRC) synchronise the link. I2S is flexible in that either the
controller or CODEC can drive (master) the BCLK and LRC clock lines. Bit clock
usually varies depending on the sample rate and the master system clock
(SYSCLK). LRCLK is the same as the sample rate. A few devices support separate
ADC and DAC LRCLKs, this allows for simultaneous capture and playback at
different sample rates.

I2S has several different operating modes:-

I2S
  MSB is transmitted on the falling edge of the first BCLK after LRC
  transition.

Left Justified
  MSB is transmitted on transition of LRC.

Right Justified
  MSB is transmitted sample size BCLKs before LRC transition.

PCM
===

PCM is another 4 wire interface, very similar to I2S, which can support a more
flexible protocol. It has bit clock (BCLK) and sync (SYNC) lines that are used
to synchronise the link while the Tx and Rx lines are used to transmit and
receive the audio data. Bit clock usually varies depending on sample rate
while sync runs at the sample rate. PCM also supports Time Division
Multiplexing (TDM) in that several devices can use the bus simultaneously (this
is sometimes referred to as network mode).

Common PCM operating modes:-

Mode A
  MSB is transmitted on falling edge of first BCLK after FRAME/SYNC.

Mode B
  MSB is transmitted on rising edge of FRAME/SYNC.
