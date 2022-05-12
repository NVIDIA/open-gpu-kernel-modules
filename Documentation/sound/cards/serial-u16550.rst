===================================
Serial UART 16450/16550 MIDI driver
===================================

The adaptor module parameter allows you to select either:

* 0 - Roland Soundcanvas support (default)
* 1 - Midiator MS-124T support (1)
* 2 - Midiator MS-124W S/A mode (2)
* 3 - MS-124W M/B mode support (3)
* 4 - Generic device with multiple input support (4)

For the Midiator MS-124W, you must set the physical M-S and A-B
switches on the Midiator to match the driver mode you select.

In Roland Soundcanvas mode, multiple ALSA raw MIDI substreams are supported
(midiCnD0-midiCnD15).  Whenever you write to a different substream, the driver
sends the nonstandard MIDI command sequence F5 NN, where NN is the substream
number plus 1.  Roland modules use this command to switch between different
"parts", so this feature lets you treat each part as a distinct raw MIDI
substream. The driver provides no way to send F5 00 (no selection) or to not
send the F5 NN command sequence at all; perhaps it ought to.

Usage example for simple serial converter:
::

	/sbin/setserial /dev/ttyS0 uart none
	/sbin/modprobe snd-serial-u16550 port=0x3f8 irq=4 speed=115200

Usage example for Roland SoundCanvas with 4 MIDI ports:
::

	/sbin/setserial /dev/ttyS0 uart none
	/sbin/modprobe snd-serial-u16550 port=0x3f8 irq=4 outs=4

In MS-124T mode, one raw MIDI substream is supported (midiCnD0); the outs
module parameter is automatically set to 1. The driver sends the same data to
all four MIDI Out connectors.  Set the A-B switch and the speed module
parameter to match (A=19200, B=9600).

Usage example for MS-124T, with A-B switch in A position:
::

	/sbin/setserial /dev/ttyS0 uart none
	/sbin/modprobe snd-serial-u16550 port=0x3f8 irq=4 adaptor=1 \
			speed=19200

In MS-124W S/A mode, one raw MIDI substream is supported (midiCnD0);
the outs module parameter is automatically set to 1. The driver sends
the same data to all four MIDI Out connectors at full MIDI speed.

Usage example for S/A mode:
::

	/sbin/setserial /dev/ttyS0 uart none
	/sbin/modprobe snd-serial-u16550 port=0x3f8 irq=4 adaptor=2

In MS-124W M/B mode, the driver supports 16 ALSA raw MIDI substreams;
the outs module parameter is automatically set to 16.  The substream
number gives a bitmask of which MIDI Out connectors the data should be
sent to, with midiCnD1 sending to Out 1, midiCnD2 to Out 2, midiCnD4 to
Out 3, and midiCnD8 to Out 4.  Thus midiCnD15 sends the data to all 4 ports.
As a special case, midiCnD0 also sends to all ports, since it is not useful
to send the data to no ports.  M/B mode has extra overhead to select the MIDI
Out for each byte, so the aggregate data rate across all four MIDI Outs is
at most one byte every 520 us, as compared with the full MIDI data rate of
one byte every 320 us per port.

Usage example for M/B mode:
::

	/sbin/setserial /dev/ttyS0 uart none
	/sbin/modprobe snd-serial-u16550 port=0x3f8 irq=4 adaptor=3

The MS-124W hardware's M/A mode is currently not supported. This mode allows
the MIDI Outs to act independently at double the aggregate throughput of M/B,
but does not allow sending the same byte simultaneously to multiple MIDI Outs. 
The M/A protocol requires the driver to twiddle the modem control lines under
timing constraints, so it would be a bit more complicated to implement than
the other modes.

Midiator models other than MS-124W and MS-124T are currently not supported. 
Note that the suffix letter is significant; the MS-124 and MS-124B are not
compatible, nor are the other known models MS-101, MS-101B, MS-103, and MS-114.
I do have documentation (tim.mann@compaq.com) that partially covers these models,
but no units to experiment with.  The MS-124W support is tested with a real unit.
The MS-124T support is untested, but should work.

The Generic driver supports multiple input and output substreams over a single
serial port.  Similar to Roland Soundcanvas mode, F5 NN is used to select the
appropriate input or output stream (depending on the data direction).
Additionally, the CTS signal is used to regulate the data flow.  The number of
inputs is specified by the ins parameter.
