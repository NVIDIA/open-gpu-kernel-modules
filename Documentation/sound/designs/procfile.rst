==========================
Proc Files of ALSA Drivers
==========================

Takashi Iwai <tiwai@suse.de>

General
=======

ALSA has its own proc tree, /proc/asound.  Many useful information are
found in this tree.  When you encounter a problem and need debugging,
check the files listed in the following sections.

Each card has its subtree cardX, where X is from 0 to 7. The
card-specific files are stored in the ``card*`` subdirectories.


Global Information
==================

cards
	Shows the list of currently configured ALSA drivers,
	index, the id string, short and long descriptions.

version
	Shows the version string and compile date.

modules
	Lists the module of each card

devices
	Lists the ALSA native device mappings.

meminfo
	Shows the status of allocated pages via ALSA drivers.
	Appears only when ``CONFIG_SND_DEBUG=y``.

hwdep
	Lists the currently available hwdep devices in format of
	``<card>-<device>: <name>``

pcm
	Lists the currently available PCM devices in format of
	``<card>-<device>: <id>: <name> : <sub-streams>``

timer
	Lists the currently available timer devices


oss/devices
	Lists the OSS device mappings.

oss/sndstat
	Provides the output compatible with /dev/sndstat.
	You can symlink this to /dev/sndstat.


Card Specific Files
===================

The card-specific files are found in ``/proc/asound/card*`` directories.
Some drivers (e.g. cmipci) have their own proc entries for the
register dump, etc (e.g. ``/proc/asound/card*/cmipci`` shows the register
dump).  These files would be really helpful for debugging.

When PCM devices are available on this card, you can see directories
like pcm0p or pcm1c.  They hold the PCM information for each PCM
stream.  The number after ``pcm`` is the PCM device number from 0, and
the last ``p`` or ``c`` means playback or capture direction.  The files in
this subtree is described later.

The status of MIDI I/O is found in ``midi*`` files.  It shows the device
name and the received/transmitted bytes through the MIDI device.

When the card is equipped with AC97 codecs, there are ``codec97#*``
subdirectories (described later).

When the OSS mixer emulation is enabled (and the module is loaded),
oss_mixer file appears here, too.  This shows the current mapping of
OSS mixer elements to the ALSA control elements.  You can change the
mapping by writing to this device.  Read OSS-Emulation.txt for
details.


PCM Proc Files
==============

``card*/pcm*/info``
	The general information of this PCM device: card #, device #,
	substreams, etc.

``card*/pcm*/xrun_debug``
	This file appears when ``CONFIG_SND_DEBUG=y`` and
	``CONFIG_SND_PCM_XRUN_DEBUG=y``.
	This shows the status of xrun (= buffer overrun/xrun) and
	invalid PCM position debug/check of ALSA PCM middle layer.
	It takes an integer value, can be changed by writing to this
	file, such as::

		 # echo 5 > /proc/asound/card0/pcm0p/xrun_debug

	The value consists of the following bit flags:

	* bit 0 = Enable XRUN/jiffies debug messages
	* bit 1 = Show stack trace at XRUN / jiffies check
	* bit 2 = Enable additional jiffies check

	When the bit 0 is set, the driver will show the messages to
	kernel log when an xrun is detected.  The debug message is
	shown also when the invalid H/W pointer is detected at the
	update of periods (usually called from the interrupt
	handler).

	When the bit 1 is set, the driver will show the stack trace
	additionally.  This may help the debugging.

	Since 2.6.30, this option can enable the hwptr check using
	jiffies.  This detects spontaneous invalid pointer callback
	values, but can be lead to too much corrections for a (mostly
	buggy) hardware that doesn't give smooth pointer updates.
	This feature is enabled via the bit 2.

``card*/pcm*/sub*/info``
	The general information of this PCM sub-stream.

``card*/pcm*/sub*/status``
	The current status of this PCM sub-stream, elapsed time,
	H/W position, etc.

``card*/pcm*/sub*/hw_params``
	The hardware parameters set for this sub-stream.

``card*/pcm*/sub*/sw_params``
	The soft parameters set for this sub-stream.

``card*/pcm*/sub*/prealloc``
	The buffer pre-allocation information.

``card*/pcm*/sub*/xrun_injection``
	Triggers an XRUN to the running stream when any value is
	written to this proc file.  Used for fault injection.
	This entry is write-only.

AC97 Codec Information
======================

``card*/codec97#*/ac97#?-?``
	Shows the general information of this AC97 codec chip, such as
	name, capabilities, set up.

``card*/codec97#0/ac97#?-?+regs``
	Shows the AC97 register dump.  Useful for debugging.

	When CONFIG_SND_DEBUG is enabled, you can write to this file for
	changing an AC97 register directly.  Pass two hex numbers.
	For example,

::

	# echo 02 9f1f > /proc/asound/card0/codec97#0/ac97#0-0+regs


USB Audio Streams
=================

``card*/stream*``
	Shows the assignment and the current status of each audio stream
	of the given card.  This information is very useful for debugging.


HD-Audio Codecs
===============

``card*/codec#*``
	Shows the general codec information and the attribute of each
	widget node.

``card*/eld#*``
	Available for HDMI or DisplayPort interfaces.
	Shows ELD(EDID Like Data) info retrieved from the attached HDMI sink,
	and describes its audio capabilities and configurations.

	Some ELD fields may be modified by doing ``echo name hex_value > eld#*``.
	Only do this if you are sure the HDMI sink provided value is wrong.
	And if that makes your HDMI audio work, please report to us so that we
	can fix it in future kernel releases.


Sequencer Information
=====================

seq/drivers
	Lists the currently available ALSA sequencer drivers.

seq/clients
	Shows the list of currently available sequencer clients and
	ports.  The connection status and the running status are shown
	in this file, too.

seq/queues
	Lists the currently allocated/running sequencer queues.

seq/timer
	Lists the currently allocated/running sequencer timers.

seq/oss
	Lists the OSS-compatible sequencer stuffs.


Help For Debugging?
===================

When the problem is related with PCM, first try to turn on xrun_debug
mode.  This will give you the kernel messages when and where xrun
happened.

If it's really a bug, report it with the following information:

- the name of the driver/card, show in ``/proc/asound/cards``
- the register dump, if available (e.g. ``card*/cmipci``)

when it's a PCM problem,

- set-up of PCM, shown in hw_parms, sw_params, and status in the PCM
  sub-stream directory

when it's a mixer problem,

- AC97 proc files, ``codec97#*/*`` files

for USB audio/midi,

- output of ``lsusb -v``
- ``stream*`` files in card directory


The ALSA bug-tracking system is found at:
https://bugtrack.alsa-project.org/alsa-bug/
