==============================================================
Advanced Linux Sound Architecture - Driver Configuration guide
==============================================================


Kernel Configuration
====================

To enable ALSA support you need at least to build the kernel with
primary sound card support (``CONFIG_SOUND``).  Since ALSA can emulate
OSS, you don't have to choose any of the OSS modules.

Enable "OSS API emulation" (``CONFIG_SND_OSSEMUL``) and both OSS mixer
and PCM supports if you want to run OSS applications with ALSA.

If you want to support the WaveTable functionality on cards such as
SB Live! then you need to enable "Sequencer support"
(``CONFIG_SND_SEQUENCER``).

To make ALSA debug messages more verbose, enable the "Verbose printk"
and "Debug" options.  To check for memory leaks, turn on "Debug memory"
too.  "Debug detection" will add checks for the detection of cards.

Please note that all the ALSA ISA drivers support the Linux isapnp API
(if the card supports ISA PnP).  You don't need to configure the cards
using isapnptools.


Module parameters
=================

The user can load modules with options. If the module supports more than
one card and you have more than one card of the same type then you can
specify multiple values for the option separated by commas.


Module snd
----------

The core ALSA module.  It is used by all ALSA card drivers.
It takes the following options which have global effects.

major
    major number for sound driver;
    Default: 116
cards_limit
    limiting card index for auto-loading (1-8);
    Default: 1;
    For auto-loading more than one card, specify this option
    together with snd-card-X aliases.
slots
    Reserve the slot index for the given driver;
    This option takes multiple strings.
    See `Module Autoloading Support`_ section for details.
debug
    Specifies the debug message level;
    (0 = disable debug prints, 1 = normal debug messages,
    2 = verbose debug messages);
    This option appears only when ``CONFIG_SND_DEBUG=y``.
    This option can be dynamically changed via sysfs
    /sys/modules/snd/parameters/debug file.
  
Module snd-pcm-oss
------------------

The PCM OSS emulation module.
This module takes options which change the mapping of devices.

dsp_map
    PCM device number maps assigned to the 1st OSS device;
    Default: 0
adsp_map
    PCM device number maps assigned to the 2st OSS device;
    Default: 1
nonblock_open
    Don't block opening busy PCM devices;
    Default: 1

For example, when ``dsp_map=2``, /dev/dsp will be mapped to PCM #2 of
the card #0.  Similarly, when ``adsp_map=0``, /dev/adsp will be mapped
to PCM #0 of the card #0.
For changing the second or later card, specify the option with
commas, such like ``dsp_map=0,1``.

``nonblock_open`` option is used to change the behavior of the PCM
regarding opening the device.  When this option is non-zero,
opening a busy OSS PCM device won't be blocked but return
immediately with EAGAIN (just like O_NONBLOCK flag).
    
Module snd-rawmidi
------------------

This module takes options which change the mapping of devices.
similar to those of the snd-pcm-oss module.

midi_map
    MIDI device number maps assigned to the 1st OSS device;
    Default: 0
amidi_map
    MIDI device number maps assigned to the 2st OSS device;
    Default: 1

Common parameters for top sound card modules
--------------------------------------------

Each of top level sound card module takes the following options.

index
    index (slot #) of sound card;
    Values: 0 through 31 or negative;
    If nonnegative, assign that index number;
    if negative, interpret as a bitmask of permissible indices;
    the first free permitted index is assigned;
    Default: -1
id
    card ID (identifier or name);
    Can be up to 15 characters long;
    Default: the card type;
    A directory by this name is created under /proc/asound/
    containing information about the card;
    This ID can be used instead of the index number in
    identifying the card
enable
    enable card;
    Default: enabled, for PCI and ISA PnP cards

Module snd-adlib
----------------

Module for AdLib FM cards.

port
    port # for OPL chip

This module supports multiple cards. It does not support autoprobe, so
the port must be specified. For actual AdLib FM cards it will be 0x388.
Note that this card does not have PCM support and no mixer; only FM
synthesis.

Make sure you have ``sbiload`` from the alsa-tools package available and,
after loading the module, find out the assigned ALSA sequencer port
number through ``sbiload -l``.

Example output:
::

      Port     Client name                       Port name
      64:0     OPL2 FM synth                     OPL2 FM Port

Load the ``std.sb`` and ``drums.sb`` patches also supplied by ``sbiload``:
::

      sbiload -p 64:0 std.sb drums.sb

If you use this driver to drive an OPL3, you can use ``std.o3`` and ``drums.o3``
instead. To have the card produce sound, use ``aplaymidi`` from alsa-utils:
::

      aplaymidi -p 64:0 foo.mid

Module snd-ad1816a
------------------

Module for sound cards based on Analog Devices AD1816A/AD1815 ISA chips.

clockfreq
    Clock frequency for AD1816A chip (default = 0, 33000Hz)
    
This module supports multiple cards, autoprobe and PnP.
    
Module snd-ad1848
-----------------

Module for sound cards based on AD1848/AD1847/CS4248 ISA chips.

port
    port # for AD1848 chip
irq
    IRQ # for AD1848  chip
dma1
    DMA # for AD1848 chip (0,1,3)
    
This module supports multiple cards.  It does not support autoprobe
thus main port must be specified!!! Other ports are optional.
    
The power-management is supported.

Module snd-ad1889
-----------------

Module for Analog Devices AD1889 chips.

ac97_quirk
    AC'97 workaround for strange hardware;
    See the description of intel8x0 module for details.

This module supports multiple cards.

Module snd-ali5451
------------------

Module for ALi M5451 PCI chip.

pcm_channels
    Number of hardware channels assigned for PCM
spdif
    Support SPDIF I/O;
    Default: disabled

This module supports one chip and autoprobe.

The power-management is supported.

Module snd-als100
-----------------

Module for sound cards based on Avance Logic ALS100/ALS120 ISA chips.

This module supports multiple cards, autoprobe and PnP.

The power-management is supported.

Module snd-als300
-----------------

Module for Avance Logic ALS300 and ALS300+

This module supports multiple cards.

The power-management is supported.

Module snd-als4000
------------------

Module for sound cards based on Avance Logic ALS4000 PCI chip.

joystick_port
    port # for legacy joystick support;
    0 = disabled (default), 1 = auto-detect
    
This module supports multiple cards, autoprobe and PnP.

The power-management is supported.

Module snd-asihpi
-----------------

Module for AudioScience ASI soundcards

enable_hpi_hwdep
    enable HPI hwdep for AudioScience soundcard

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-atiixp
-----------------

Module for ATI IXP 150/200/250/400 AC97 controllers.

ac97_clock
    AC'97 clock (default = 48000)
ac97_quirk
    AC'97 workaround for strange hardware;
    See `AC97 Quirk Option`_ section below.
ac97_codec
    Workaround to specify which AC'97 codec instead of probing.
    If this works for you file a bug with your `lspci -vn` output.
    (-2 = Force probing, -1 = Default behavior, 0-2 = Use the
    specified codec.)
spdif_aclink
    S/PDIF transfer over AC-link (default = 1)

This module supports one card and autoprobe.

ATI IXP has two different methods to control SPDIF output.  One is
over AC-link and another is over the "direct" SPDIF output.  The
implementation depends on the motherboard, and you'll need to
choose the correct one via spdif_aclink module option.

The power-management is supported.

Module snd-atiixp-modem
-----------------------

Module for ATI IXP 150/200/250 AC97 modem controllers.

This module supports one card and autoprobe.

Note: The default index value of this module is -2, i.e. the first
slot is excluded.

The power-management is supported.

Module snd-au8810, snd-au8820, snd-au8830
-----------------------------------------

Module for Aureal Vortex, Vortex2 and Advantage device.

pcifix
    Control PCI workarounds;
    0 = Disable all workarounds,
    1 = Force the PCI latency of the Aureal card to 0xff,
    2 = Force the Extend PCI#2 Internal Master for Efficient
    Handling of Dummy Requests on the VIA KT133 AGP Bridge,
    3 = Force both settings,
    255 = Autodetect what is required (default)

This module supports all ADB PCM channels, ac97 mixer, SPDIF, hardware
EQ, mpu401, gameport. A3D and wavetable support are still in development.
Development and reverse engineering work is being coordinated at
https://savannah.nongnu.org/projects/openvortex/
SPDIF output has a copy of the AC97 codec output, unless you use the
``spdif`` pcm device, which allows raw data passthru.
The hardware EQ hardware and SPDIF is only present in the Vortex2 and 
Advantage.

Note: Some ALSA mixer applications don't handle the SPDIF sample rate 
control correctly. If you have problems regarding this, try
another ALSA compliant mixer (alsamixer works).

Module snd-azt1605
------------------

Module for Aztech Sound Galaxy soundcards based on the Aztech AZT1605
chipset.

port
    port # for BASE (0x220,0x240,0x260,0x280)
wss_port
    port # for WSS (0x530,0x604,0xe80,0xf40)
irq
    IRQ # for WSS (7,9,10,11)
dma1
    DMA # for WSS playback (0,1,3)
dma2
    DMA # for WSS capture (0,1), -1 = disabled (default)
mpu_port
    port # for MPU-401 UART (0x300,0x330), -1 = disabled (default)
mpu_irq
    IRQ # for MPU-401 UART (3,5,7,9), -1 = disabled (default)
fm_port
    port # for OPL3 (0x388), -1 = disabled (default)

This module supports multiple cards. It does not support autoprobe:
``port``, ``wss_port``, ``irq`` and ``dma1`` have to be specified.
The other values are optional.

``port`` needs to match the BASE ADDRESS jumper on the card (0x220 or 0x240)
or the value stored in the card's EEPROM for cards that have an EEPROM and
their "CONFIG MODE" jumper set to "EEPROM SETTING". The other values can
be chosen freely from the options enumerated above.

If ``dma2`` is specified and different from ``dma1``, the card will operate in
full-duplex mode. When ``dma1=3``, only ``dma2=0`` is valid and the only way to
enable capture since only channels 0 and 1 are available for capture.

Generic settings are ``port=0x220 wss_port=0x530 irq=10 dma1=1 dma2=0
mpu_port=0x330 mpu_irq=9 fm_port=0x388``.

Whatever IRQ and DMA channels you pick, be sure to reserve them for
legacy ISA in your BIOS.

Module snd-azt2316
------------------

Module for Aztech Sound Galaxy soundcards based on the Aztech AZT2316
chipset.

port
    port # for BASE (0x220,0x240,0x260,0x280)
wss_port
    port # for WSS (0x530,0x604,0xe80,0xf40)
irq
    IRQ # for WSS (7,9,10,11)
dma1
    DMA # for WSS playback (0,1,3)
dma2
    DMA # for WSS capture (0,1), -1 = disabled (default)
mpu_port
    port # for MPU-401 UART (0x300,0x330), -1 = disabled (default)
mpu_irq
    IRQ # for MPU-401 UART (5,7,9,10), -1 = disabled (default)
fm_port
    port # for OPL3 (0x388), -1 = disabled (default)

This module supports multiple cards. It does not support autoprobe:
``port``, ``wss_port``, ``irq`` and ``dma1`` have to be specified.
The other values are optional.

``port`` needs to match the BASE ADDRESS jumper on the card (0x220 or 0x240)
or the value stored in the card's EEPROM for cards that have an EEPROM and
their "CONFIG MODE" jumper set to "EEPROM SETTING". The other values can
be chosen freely from the options enumerated above.

If ``dma2`` is specified and different from ``dma1``, the card will operate in
full-duplex mode. When ``dma1=3``, only ``dma2=0`` is valid and the only way to
enable capture since only channels 0 and 1 are available for capture.

Generic settings are ``port=0x220 wss_port=0x530 irq=10 dma1=1 dma2=0
mpu_port=0x330 mpu_irq=9 fm_port=0x388``.

Whatever IRQ and DMA channels you pick, be sure to reserve them for
legacy ISA in your BIOS.

Module snd-aw2
--------------

Module for Audiowerk2 sound card

This module supports multiple cards.

Module snd-azt2320
------------------

Module for sound cards based on Aztech System AZT2320 ISA chip (PnP only).

This module supports multiple cards, PnP and autoprobe.

The power-management is supported.

Module snd-azt3328
------------------

Module for sound cards based on Aztech AZF3328 PCI chip.

joystick
    Enable joystick (default off)

This module supports multiple cards.

Module snd-bt87x
----------------

Module for video cards based on Bt87x chips.

digital_rate
    Override the default digital rate (Hz)
load_all
    Load the driver even if the card model isn't known

This module supports multiple cards.

Note: The default index value of this module is -2, i.e. the first
slot is excluded.

Module snd-ca0106
-----------------

Module for Creative Audigy LS and SB Live 24bit

This module supports multiple cards.


Module snd-cmi8330
------------------

Module for sound cards based on C-Media CMI8330 ISA chips.

isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

with ``isapnp=0``, the following options are available:

wssport
    port # for CMI8330 chip (WSS)
wssirq
    IRQ # for CMI8330 chip (WSS)
wssdma
    first DMA # for CMI8330 chip (WSS)
sbport
    port # for CMI8330 chip (SB16)
sbirq
    IRQ # for CMI8330 chip (SB16)
sbdma8
    8bit DMA # for CMI8330 chip (SB16)
sbdma16
    16bit DMA # for CMI8330 chip (SB16)
fmport
    (optional) OPL3 I/O port
mpuport
    (optional) MPU401 I/O port
mpuirq
    (optional) MPU401 irq #

This module supports multiple cards and autoprobe.

The power-management is supported.

Module snd-cmipci
-----------------

Module for C-Media CMI8338/8738/8768/8770 PCI sound cards.

mpu_port
    port address of MIDI interface (8338 only):
    0x300,0x310,0x320,0x330 = legacy port,
    1 = integrated PCI port (default on 8738),
    0 = disable
fm_port
    port address of OPL-3 FM synthesizer (8x38 only):
    0x388 = legacy port,
    1 = integrated PCI port (default on 8738),
    0 = disable
soft_ac3
    Software-conversion of raw SPDIF packets (model 033 only) (default = 1)
joystick_port
    Joystick port address (0 = disable, 1 = auto-detect)

This module supports autoprobe and multiple cards.

The power-management is supported.

Module snd-cs4231
-----------------

Module for sound cards based on CS4231 ISA chips.

port
    port # for CS4231 chip
mpu_port
    port # for MPU-401 UART (optional), -1 = disable
irq
    IRQ # for CS4231 chip
mpu_irq
    IRQ # for MPU-401 UART
dma1
    first DMA # for CS4231 chip
dma2
    second DMA # for CS4231 chip

This module supports multiple cards. This module does not support autoprobe
thus main port must be specified!!! Other ports are optional.

The power-management is supported.

Module snd-cs4236
-----------------

Module for sound cards based on CS4232/CS4232A,
CS4235/CS4236/CS4236B/CS4237B/CS4238B/CS4239 ISA chips.

isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

with ``isapnp=0``, the following options are available:

port
    port # for CS4236 chip (PnP setup - 0x534)
cport
    control port # for CS4236 chip (PnP setup - 0x120,0x210,0xf00)
mpu_port
    port # for MPU-401 UART (PnP setup - 0x300), -1 = disable
fm_port
    FM port # for CS4236 chip (PnP setup - 0x388), -1 = disable
irq
    IRQ # for CS4236 chip (5,7,9,11,12,15)
mpu_irq
    IRQ # for MPU-401 UART (9,11,12,15)
dma1
    first DMA # for CS4236 chip (0,1,3)
dma2
    second DMA # for CS4236 chip (0,1,3), -1 = disable

This module supports multiple cards. This module does not support autoprobe
(if ISA PnP is not used) thus main port and control port must be
specified!!! Other ports are optional.

The power-management is supported.

This module is aliased as snd-cs4232 since it provides the old
snd-cs4232 functionality, too.

Module snd-cs4281
-----------------

Module for Cirrus Logic CS4281 soundchip.

dual_codec
    Secondary codec ID (0 = disable, default)

This module supports multiple cards.

The power-management is supported.

Module snd-cs46xx
-----------------

Module for PCI sound cards based on CS4610/CS4612/CS4614/CS4615/CS4622/
CS4624/CS4630/CS4280 PCI chips.

external_amp
    Force to enable external amplifier.
thinkpad
    Force to enable Thinkpad's CLKRUN control.
mmap_valid
    Support OSS mmap mode (default = 0).

This module supports multiple cards and autoprobe.
Usually external amp and CLKRUN controls are detected automatically
from PCI sub vendor/device ids.  If they don't work, give the options
above explicitly.

The power-management is supported.

Module snd-cs5530
-----------------

Module for Cyrix/NatSemi Geode 5530 chip. 

Module snd-cs5535audio
----------------------

Module for multifunction CS5535 companion PCI device

The power-management is supported.

Module snd-ctxfi
----------------

Module for Creative Sound Blaster X-Fi boards (20k1 / 20k2 chips)

* Creative Sound Blaster X-Fi Titanium Fatal1ty Champion Series
* Creative Sound Blaster X-Fi Titanium Fatal1ty Professional Series
* Creative Sound Blaster X-Fi Titanium Professional Audio
* Creative Sound Blaster X-Fi Titanium
* Creative Sound Blaster X-Fi Elite Pro
* Creative Sound Blaster X-Fi Platinum
* Creative Sound Blaster X-Fi Fatal1ty
* Creative Sound Blaster X-Fi XtremeGamer
* Creative Sound Blaster X-Fi XtremeMusic
	
reference_rate
    reference sample rate, 44100 or 48000 (default)
multiple
    multiple to ref. sample rate, 1 or 2 (default)
subsystem
    override the PCI SSID for probing;
    the value consists of SSVID << 16 | SSDID.
    The default is zero, which means no override.

This module supports multiple cards.

Module snd-darla20
------------------

Module for Echoaudio Darla20

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-darla24
------------------

Module for Echoaudio Darla24

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-dt019x
-----------------

Module for Diamond Technologies DT-019X / Avance Logic ALS-007 (PnP
only)

This module supports multiple cards.  This module is enabled only with
ISA PnP support.

The power-management is supported.

Module snd-dummy
----------------

Module for the dummy sound card. This "card" doesn't do any output
or input, but you may use this module for any application which
requires a sound card (like RealPlayer).

pcm_devs
    Number of PCM devices assigned to each card (default = 1, up to 4)
pcm_substreams
    Number of PCM substreams assigned to each PCM (default = 8, up to 128)
hrtimer
    Use hrtimer (=1, default) or system timer (=0)
fake_buffer
    Fake buffer allocations (default = 1)

When multiple PCM devices are created, snd-dummy gives different
behavior to each PCM device:
* 0 = interleaved with mmap support
* 1 = non-interleaved with mmap support
* 2 = interleaved without mmap 
* 3 = non-interleaved without mmap

As default, snd-dummy drivers doesn't allocate the real buffers
but either ignores read/write or mmap a single dummy page to all
buffer pages, in order to save the resources.  If your apps need
the read/ written buffer data to be consistent, pass fake_buffer=0
option.

The power-management is supported.

Module snd-echo3g
-----------------

Module for Echoaudio 3G cards (Gina3G/Layla3G)

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-emu10k1
------------------

Module for EMU10K1/EMU10k2 based PCI sound cards.

* Sound Blaster Live!
* Sound Blaster PCI 512
* Emu APS (partially supported)
* Sound Blaster Audigy
	
extin
    bitmap of available external inputs for FX8010 (see bellow)
extout
    bitmap of available external outputs for FX8010 (see bellow)
seq_ports
    allocated sequencer ports (4 by default)
max_synth_voices
    limit of voices used for wavetable (64 by default)
max_buffer_size
    specifies the maximum size of wavetable/pcm buffers given in MB
    unit.  Default value is 128.
enable_ir
    enable IR

This module supports multiple cards and autoprobe.

Input & Output configurations 			[extin/extout]
* Creative Card wo/Digital out			[0x0003/0x1f03]
* Creative Card w/Digital out			[0x0003/0x1f0f]
* Creative Card w/Digital CD in			[0x000f/0x1f0f]
* Creative Card wo/Digital out + LiveDrive	[0x3fc3/0x1fc3]
* Creative Card w/Digital out + LiveDrive	[0x3fc3/0x1fcf]
* Creative Card w/Digital CD in + LiveDrive	[0x3fcf/0x1fcf]
* Creative Card wo/Digital out + Digital I/O 2  [0x0fc3/0x1f0f]
* Creative Card w/Digital out + Digital I/O 2	[0x0fc3/0x1f0f]
* Creative Card w/Digital CD in + Digital I/O 2	[0x0fcf/0x1f0f]
* Creative Card 5.1/w Digital out + LiveDrive	[0x3fc3/0x1fff]
* Creative Card 5.1 (c) 2003			[0x3fc3/0x7cff]
* Creative Card all ins and outs		[0x3fff/0x7fff]
  
The power-management is supported.
  
Module snd-emu10k1x
-------------------

Module for Creative Emu10k1X (SB Live Dell OEM version)

This module supports multiple cards.

Module snd-ens1370
------------------

Module for Ensoniq AudioPCI ES1370 PCI sound cards.

* SoundBlaster PCI 64
* SoundBlaster PCI 128
    
joystick
    Enable joystick (default off)
  
This module supports multiple cards and autoprobe.

The power-management is supported.

Module snd-ens1371
------------------

Module for Ensoniq AudioPCI ES1371 PCI sound cards.

* SoundBlaster PCI 64
* SoundBlaster PCI 128
* SoundBlaster Vibra PCI
      
joystick_port
    port # for joystick (0x200,0x208,0x210,0x218), 0 = disable
    (default), 1 = auto-detect
  
This module supports multiple cards and autoprobe.

The power-management is supported.

Module snd-es1688
-----------------

Module for ESS AudioDrive ES-1688 and ES-688 sound cards.

isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)
mpu_port
    port # for MPU-401 port (0x300,0x310,0x320,0x330), -1 = disable (default)
mpu_irq
    IRQ # for MPU-401 port (5,7,9,10)
fm_port
    port # for OPL3 (option; share the same port as default)

with ``isapnp=0``, the following additional options are available:

port
    port # for ES-1688 chip (0x220,0x240,0x260)
irq
    IRQ # for ES-1688 chip (5,7,9,10)
dma8
    DMA # for ES-1688 chip (0,1,3)

This module supports multiple cards and autoprobe (without MPU-401 port)
and PnP with the ES968 chip.

Module snd-es18xx
-----------------

Module for ESS AudioDrive ES-18xx sound cards.

isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

with ``isapnp=0``, the following options are available:

port
    port # for ES-18xx chip (0x220,0x240,0x260)
mpu_port
    port # for MPU-401 port (0x300,0x310,0x320,0x330), -1 = disable (default)
fm_port
    port # for FM (optional, not used)
irq
    IRQ # for ES-18xx chip (5,7,9,10)
dma1
    first DMA # for ES-18xx chip (0,1,3)
dma2
    first DMA # for ES-18xx chip (0,1,3)

This module supports multiple cards, ISA PnP and autoprobe (without MPU-401
port if native ISA PnP routines are not used).
When ``dma2`` is equal with ``dma1``, the driver works as half-duplex.

The power-management is supported.

Module snd-es1938
-----------------

Module for sound cards based on ESS Solo-1 (ES1938,ES1946) chips.

This module supports multiple cards and autoprobe.

The power-management is supported.

Module snd-es1968
-----------------

Module for sound cards based on ESS Maestro-1/2/2E (ES1968/ES1978) chips.

total_bufsize
    total buffer size in kB (1-4096kB)
pcm_substreams_p
    playback channels (1-8, default=2)
pcm_substreams_c
    capture channels (1-8, default=0)
clock
    clock (0 = auto-detection)
use_pm
    support the power-management (0 = off, 1 = on, 2 = auto (default))
enable_mpu
    enable MPU401 (0 = off, 1 = on, 2 = auto (default))
joystick
    enable joystick (default off)       

This module supports multiple cards and autoprobe.

The power-management is supported.

Module snd-fm801
----------------

Module for ForteMedia FM801 based PCI sound cards.

tea575x_tuner
    Enable TEA575x tuner;
    1 = MediaForte 256-PCS,
    2 = MediaForte 256-PCPR,
    3 = MediaForte 64-PCR
    High 16-bits are video (radio) device number + 1;
    example: 0x10002 (MediaForte 256-PCPR, device 1)
	  
This module supports multiple cards and autoprobe.

The power-management is supported.

Module snd-gina20
-----------------

Module for Echoaudio Gina20

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-gina24
-----------------

Module for Echoaudio Gina24

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-gusclassic
---------------------

Module for Gravis UltraSound Classic sound card.

port
    port # for GF1 chip (0x220,0x230,0x240,0x250,0x260)
irq
    IRQ # for GF1 chip (3,5,9,11,12,15)
dma1
    DMA # for GF1 chip (1,3,5,6,7)
dma2
    DMA # for GF1 chip (1,3,5,6,7,-1=disable)
joystick_dac
    0 to 31, (0.59V-4.52V or 0.389V-2.98V)
voices
    GF1 voices limit (14-32)
pcm_voices
    reserved PCM voices

This module supports multiple cards and autoprobe.

Module snd-gusextreme
---------------------

Module for Gravis UltraSound Extreme (Synergy ViperMax) sound card.

port
    port # for ES-1688 chip (0x220,0x230,0x240,0x250,0x260)
gf1_port
    port # for GF1 chip (0x210,0x220,0x230,0x240,0x250,0x260,0x270)
mpu_port
    port # for MPU-401 port (0x300,0x310,0x320,0x330), -1 = disable
irq
    IRQ # for ES-1688 chip (5,7,9,10)
gf1_irq
    IRQ # for GF1 chip (3,5,9,11,12,15)
mpu_irq
    IRQ # for MPU-401 port (5,7,9,10)
dma8
    DMA # for ES-1688 chip (0,1,3)
dma1
    DMA # for GF1 chip (1,3,5,6,7)
joystick_dac
    0 to 31, (0.59V-4.52V or 0.389V-2.98V)
voices
    GF1 voices limit (14-32)
pcm_voices
    reserved PCM voices

This module supports multiple cards and autoprobe (without MPU-401 port).

Module snd-gusmax
-----------------

Module for Gravis UltraSound MAX sound card.

port
    port # for GF1 chip (0x220,0x230,0x240,0x250,0x260)
irq
    IRQ # for GF1 chip (3,5,9,11,12,15)
dma1
    DMA # for GF1 chip (1,3,5,6,7)
dma2
    DMA # for GF1 chip (1,3,5,6,7,-1=disable)
joystick_dac
    0 to 31, (0.59V-4.52V or 0.389V-2.98V)
voices
    GF1 voices limit (14-32)
pcm_voices
    reserved PCM voices

This module supports multiple cards and autoprobe.

Module snd-hda-intel
--------------------

Module for Intel HD Audio (ICH6, ICH6M, ESB2, ICH7, ICH8, ICH9, ICH10,
PCH, SCH), ATI SB450, SB600, R600, RS600, RS690, RS780, RV610, RV620,
RV630, RV635, RV670, RV770, VIA VT8251/VT8237A, SIS966, ULI M5461

[Multiple options for each card instance]

model
    force the model name
position_fix
    Fix DMA pointer;
    -1 = system default: choose appropriate one per controller hardware,
    0 = auto: falls back to LPIB when POSBUF doesn't work,
    1 = use LPIB,
    2 = POSBUF: use position buffer,
    3 = VIACOMBO: VIA-specific workaround for capture,
    4 = COMBO: use LPIB for playback, auto for capture stream
    5 = SKL+: apply the delay calculation available on recent Intel chips
    6 = FIFO: correct the position with the fixed FIFO size, for recent AMD chips
probe_mask
    Bitmask to probe codecs (default = -1, meaning all slots);
    When the bit 8 (0x100) is set, the lower 8 bits are used
    as the "fixed" codec slots; i.e. the driver probes the
    slots regardless what hardware reports back
probe_only
    Only probing and no codec initialization (default=off);
    Useful to check the initial codec status for debugging
bdl_pos_adj
    Specifies the DMA IRQ timing delay in samples.
    Passing -1 will make the driver to choose the appropriate
    value based on the controller chip.
patch
    Specifies the early "patch" files to modify the HD-audio setup
    before initializing the codecs.
    This option is available only when ``CONFIG_SND_HDA_PATCH_LOADER=y``
    is set.  See hd-audio/notes.rst for details.
beep_mode
    Selects the beep registration mode (0=off, 1=on);
    default value is set via ``CONFIG_SND_HDA_INPUT_BEEP_MODE`` kconfig.

[Single (global) options]

single_cmd
    Use single immediate commands to communicate with codecs
    (for debugging only)
enable_msi
    Enable Message Signaled Interrupt (MSI) (default = off)
power_save
    Automatic power-saving timeout (in second, 0 = disable)
power_save_controller
    Reset HD-audio controller in power-saving mode (default = on)
align_buffer_size
    Force rounding of buffer/period sizes to multiples of 128 bytes.
    This is more efficient in terms of memory access but isn't
    required by the HDA spec and prevents users from specifying
    exact period/buffer sizes. (default = on)
snoop
    Enable/disable snooping (default = on)

This module supports multiple cards and autoprobe.

See hd-audio/notes.rst for more details about HD-audio driver.

Each codec may have a model table for different configurations.
If your machine isn't listed there, the default (usually minimal)
configuration is set up.  You can pass ``model=<name>`` option to
specify a certain model in such a case.  There are different
models depending on the codec chip.  The list of available models
is found in hd-audio/models.rst.

The model name ``generic`` is treated as a special case.  When this
model is given, the driver uses the generic codec parser without
"codec-patch".  It's sometimes good for testing and debugging.

If the default configuration doesn't work and one of the above
matches with your device, report it together with alsa-info.sh
output (with ``--no-upload`` option) to kernel bugzilla or alsa-devel
ML (see the section `Links and Addresses`_).

``power_save`` and ``power_save_controller`` options are for power-saving
mode.  See powersave.rst for details.

Note 2: If you get click noises on output, try the module option
``position_fix=1`` or ``2``.  ``position_fix=1`` will use the SD_LPIB
register value without FIFO size correction as the current
DMA pointer.  ``position_fix=2`` will make the driver to use
the position buffer instead of reading SD_LPIB register.
(Usually SD_LPIB register is more accurate than the
position buffer.)

``position_fix=3`` is specific to VIA devices.  The position
of the capture stream is checked from both LPIB and POSBUF
values.  ``position_fix=4`` is a combination mode, using LPIB
for playback and POSBUF for capture.

NB: If you get many ``azx_get_response timeout`` messages at
loading, it's likely a problem of interrupts (e.g. ACPI irq
routing).  Try to boot with options like ``pci=noacpi``.  Also, you
can try ``single_cmd=1`` module option.  This will switch the
communication method between HDA controller and codecs to the
single immediate commands instead of CORB/RIRB.  Basically, the
single command mode is provided only for BIOS, and you won't get
unsolicited events, too.  But, at least, this works independently
from the irq.  Remember this is a last resort, and should be
avoided as much as possible...

MORE NOTES ON ``azx_get_response timeout`` PROBLEMS:
On some hardware, you may need to add a proper probe_mask option
to avoid the ``azx_get_response timeout`` problem above, instead.
This occurs when the access to non-existing or non-working codec slot
(likely a modem one) causes a stall of the communication via HD-audio
bus.  You can see which codec slots are probed by enabling
``CONFIG_SND_DEBUG_VERBOSE``, or simply from the file name of the codec
proc files.  Then limit the slots to probe by probe_mask option.
For example, ``probe_mask=1`` means to probe only the first slot, and
``probe_mask=4`` means only the third slot.

The power-management is supported.

Module snd-hdsp
---------------

Module for RME Hammerfall DSP audio interface(s)

This module supports multiple cards.

Note: The firmware data can be automatically loaded via hotplug
when ``CONFIG_FW_LOADER`` is set.  Otherwise, you need to load
the firmware via hdsploader utility included in alsa-tools
package.
The firmware data is found in alsa-firmware package.

Note: snd-page-alloc module does the job which snd-hammerfall-mem
module did formerly.  It will allocate the buffers in advance
when any HDSP cards are found.  To make the buffer
allocation sure, load snd-page-alloc module in the early
stage of boot sequence.  See `Early Buffer Allocation`_
section.

Module snd-hdspm
----------------

Module for RME HDSP MADI board.

precise_ptr
    Enable precise pointer, or disable.
line_outs_monitor
    Send playback streams to analog outs by default.
enable_monitor
    Enable Analog Out on Channel 63/64 by default.

See hdspm.rst for details.

Module snd-ice1712
------------------

Module for Envy24 (ICE1712) based PCI sound cards.

* MidiMan M Audio Delta 1010
* MidiMan M Audio Delta 1010LT
* MidiMan M Audio Delta DiO 2496
* MidiMan M Audio Delta 66
* MidiMan M Audio Delta 44
* MidiMan M Audio Delta 410
* MidiMan M Audio Audiophile 2496
* TerraTec EWS 88MT
* TerraTec EWS 88D
* TerraTec EWX 24/96
* TerraTec DMX 6Fire
* TerraTec Phase 88
* Hoontech SoundTrack DSP 24
* Hoontech SoundTrack DSP 24 Value
* Hoontech SoundTrack DSP 24 Media 7.1
* Event Electronics, EZ8
* Digigram VX442
* Lionstracs, Mediastaton
* Terrasoniq TS 88
			
model
    Use the given board model, one of the following:
    delta1010, dio2496, delta66, delta44, audiophile, delta410,
    delta1010lt, vx442, ewx2496, ews88mt, ews88mt_new, ews88d,
    dmx6fire, dsp24, dsp24_value, dsp24_71, ez8,
    phase88, mediastation
omni
    Omni I/O support for MidiMan M-Audio Delta44/66
cs8427_timeout
    reset timeout for the CS8427 chip (S/PDIF transceiver) in msec
    resolution, default value is 500 (0.5 sec)

This module supports multiple cards and autoprobe.
Note: The consumer part is not used with all Envy24 based cards (for
example in the MidiMan Delta siree).

Note: The supported board is detected by reading EEPROM or PCI
SSID (if EEPROM isn't available).  You can override the
model by passing ``model`` module option in case that the
driver isn't configured properly or you want to try another
type for testing.

Module snd-ice1724
------------------

Module for Envy24HT (VT/ICE1724), Envy24PT (VT1720) based PCI sound cards.

* MidiMan M Audio Revolution 5.1
* MidiMan M Audio Revolution 7.1
* MidiMan M Audio Audiophile 192
* AMP Ltd AUDIO2000
* TerraTec Aureon 5.1 Sky
* TerraTec Aureon 7.1 Space
* TerraTec Aureon 7.1 Universe
* TerraTec Phase 22
* TerraTec Phase 28
* AudioTrak Prodigy 7.1
* AudioTrak Prodigy 7.1 LT
* AudioTrak Prodigy 7.1 XT
* AudioTrak Prodigy 7.1 HIFI
* AudioTrak Prodigy 7.1 HD2
* AudioTrak Prodigy 192
* Pontis MS300
* Albatron K8X800 Pro II 
* Chaintech ZNF3-150
* Chaintech ZNF3-250
* Chaintech 9CJS
* Chaintech AV-710
* Shuttle SN25P
* Onkyo SE-90PCI
* Onkyo SE-200PCI
* ESI Juli@
* ESI Maya44
* Hercules Fortissimo IV
* EGO-SYS WaveTerminal 192M
			
model
    Use the given board model, one of the following:
    revo51, revo71, amp2000, prodigy71, prodigy71lt,
    prodigy71xt, prodigy71hifi, prodigyhd2, prodigy192,
    juli, aureon51, aureon71, universe, ap192, k8x800,
    phase22, phase28, ms300, av710, se200pci, se90pci,
    fortissimo4, sn25p, WT192M, maya44
  
This module supports multiple cards and autoprobe.

Note: The supported board is detected by reading EEPROM or PCI
SSID (if EEPROM isn't available).  You can override the
model by passing ``model`` module option in case that the
driver isn't configured properly or you want to try another
type for testing.

Module snd-indigo
-----------------

Module for Echoaudio Indigo

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-indigodj
-------------------

Module for Echoaudio Indigo DJ

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-indigoio
-------------------

Module for Echoaudio Indigo IO

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-intel8x0
-------------------

Module for AC'97 motherboards from Intel and compatibles.

* Intel i810/810E, i815, i820, i830, i84x, MX440 ICH5, ICH6, ICH7,
  6300ESB, ESB2 
* SiS 7012 (SiS 735)
* NVidia NForce, NForce2, NForce3, MCP04, CK804 CK8, CK8S, MCP501
* AMD AMD768, AMD8111
* ALi m5455
	  
ac97_clock
    AC'97 codec clock base (0 = auto-detect)
ac97_quirk
    AC'97 workaround for strange hardware;
    See `AC97 Quirk Option`_ section below.
buggy_irq
    Enable workaround for buggy interrupts on some motherboards
    (default yes on nForce chips, otherwise off)
buggy_semaphore
    Enable workaround for hardware with buggy semaphores (e.g. on some
    ASUS laptops) (default off)
spdif_aclink
    Use S/PDIF over AC-link instead of direct connection from the
    controller chip (0 = off, 1 = on, -1 = default)

This module supports one chip and autoprobe.

Note: the latest driver supports auto-detection of chip clock.
if you still encounter too fast playback, specify the clock
explicitly via the module option ``ac97_clock=41194``.

Joystick/MIDI ports are not supported by this driver.  If your
motherboard has these devices, use the ns558 or snd-mpu401
modules, respectively.

The power-management is supported.

Module snd-intel8x0m
--------------------

Module for Intel ICH (i8x0) chipset MC97 modems.

* Intel i810/810E, i815, i820, i830, i84x, MX440 ICH5, ICH6, ICH7
* SiS 7013 (SiS 735)
* NVidia NForce, NForce2, NForce2s, NForce3
* AMD AMD8111
* ALi m5455
	  
ac97_clock
    AC'97 codec clock base (0 = auto-detect)
  
This module supports one card and autoprobe.

Note: The default index value of this module is -2, i.e. the first
slot is excluded.

The power-management is supported.

Module snd-interwave
--------------------

Module for Gravis UltraSound PnP, Dynasonic 3-D/Pro, STB Sound Rage 32
and other sound cards based on AMD InterWave (tm) chip.

joystick_dac
    0 to 31, (0.59V-4.52V or 0.389V-2.98V)
midi
    1 = MIDI UART enable, 0 = MIDI UART disable (default)
pcm_voices
    reserved PCM voices for the synthesizer (default 2)
effect
    1 = InterWave effects enable (default 0); requires 8 voices
isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

with ``isapnp=0``, the following options are available:

port
    port # for InterWave chip (0x210,0x220,0x230,0x240,0x250,0x260)
irq
    IRQ # for InterWave chip (3,5,9,11,12,15)
dma1
    DMA # for InterWave chip (0,1,3,5,6,7)
dma2
    DMA # for InterWave chip (0,1,3,5,6,7,-1=disable)

This module supports multiple cards, autoprobe and ISA PnP.

Module snd-interwave-stb
------------------------

Module for UltraSound 32-Pro (sound card from STB used by Compaq)
and other sound cards based on AMD InterWave (tm) chip with TEA6330T
circuit for extended control of bass, treble and master volume.

joystick_dac
    0 to 31, (0.59V-4.52V or 0.389V-2.98V)
midi
    1 = MIDI UART enable, 0 = MIDI UART disable (default)
pcm_voices
    reserved PCM voices for the synthesizer (default 2)
effect
    1 = InterWave effects enable (default 0); requires 8 voices
isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

with ``isapnp=0``, the following options are available:

port
    port # for InterWave chip (0x210,0x220,0x230,0x240,0x250,0x260)
port_tc
    tone control (i2c bus) port # for TEA6330T chip (0x350,0x360,0x370,0x380)
irq
    IRQ # for InterWave chip (3,5,9,11,12,15)
dma1
    DMA # for InterWave chip (0,1,3,5,6,7)
dma2
    DMA # for InterWave chip (0,1,3,5,6,7,-1=disable)

This module supports multiple cards, autoprobe and ISA PnP.

Module snd-jazz16
-------------------

Module for Media Vision Jazz16 chipset. The chipset consists of 3 chips:
MVD1216 + MVA416 + MVA514.

port
    port # for SB DSP chip (0x210,0x220,0x230,0x240,0x250,0x260)
irq
    IRQ # for SB DSP chip (3,5,7,9,10,15)
dma8
    DMA # for SB DSP chip (1,3)
dma16
    DMA # for SB DSP chip (5,7)
mpu_port
    MPU-401 port # (0x300,0x310,0x320,0x330)
mpu_irq
    MPU-401 irq # (2,3,5,7)

This module supports multiple cards.

Module snd-korg1212
-------------------

Module for Korg 1212 IO PCI card

This module supports multiple cards.

Module snd-layla20
------------------

Module for Echoaudio Layla20

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-layla24
------------------

Module for Echoaudio Layla24

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-lola
---------------

Module for Digigram Lola PCI-e boards

This module supports multiple cards.

Module snd-lx6464es
-------------------

Module for Digigram LX6464ES boards

This module supports multiple cards.

Module snd-maestro3
-------------------

Module for Allegro/Maestro3 chips

external_amp
    enable external amp (enabled by default)
amp_gpio
    GPIO pin number for external amp (0-15) or -1 for default pin (8
    for allegro, 1 for others) 

This module supports autoprobe and multiple chips.

Note: the binding of amplifier is dependent on hardware.
If there is no sound even though all channels are unmuted, try to
specify other gpio connection via amp_gpio option. 
For example, a Panasonic notebook might need ``amp_gpio=0x0d``
option.

The power-management is supported.

Module snd-mia
---------------

Module for Echoaudio Mia

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-miro
---------------

Module for Miro soundcards: miroSOUND PCM 1 pro, miroSOUND PCM 12,
miroSOUND PCM 20 Radio.

port
    Port # (0x530,0x604,0xe80,0xf40)
irq
    IRQ # (5,7,9,10,11)
dma1
    1st dma # (0,1,3)
dma2
    2nd dma # (0,1)
mpu_port
    MPU-401 port # (0x300,0x310,0x320,0x330)
mpu_irq
    MPU-401 irq # (5,7,9,10)
fm_port
    FM Port # (0x388)
wss
    enable WSS mode
ide
    enable onboard ide support

Module snd-mixart
-----------------

Module for Digigram miXart8 sound cards.

This module supports multiple cards.
Note: One miXart8 board will be represented as 4 alsa cards.
See Documentation/sound/cards/mixart.rst for details.

When the driver is compiled as a module and the hotplug firmware
is supported, the firmware data is loaded via hotplug automatically.
Install the necessary firmware files in alsa-firmware package.
When no hotplug fw loader is available, you need to load the
firmware via mixartloader utility in alsa-tools package.

Module snd-mona
---------------

Module for Echoaudio Mona

This module supports multiple cards.
The driver requires the firmware loader support on kernel.

Module snd-mpu401
-----------------

Module for MPU-401 UART devices.

port
    port number or -1 (disable)
irq
    IRQ number or -1 (disable)
pnp
    PnP detection - 0 = disable, 1 = enable (default)

This module supports multiple devices and PnP.

Module snd-msnd-classic
-----------------------

Module for Turtle Beach MultiSound Classic, Tahiti or Monterey
soundcards.

io
    Port # for msnd-classic card
irq
    IRQ # for msnd-classic card
mem
    Memory address (0xb0000, 0xc8000, 0xd0000, 0xd8000, 0xe0000 or 0xe8000)
write_ndelay
    enable write ndelay (default = 1)
calibrate_signal
    calibrate signal (default = 0)
isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)
digital
    Digital daughterboard present (default = 0)
cfg
    Config port (0x250, 0x260 or 0x270) default = PnP
reset
    Reset all devices
mpu_io
    MPU401 I/O port
mpu_irq
    MPU401 irq#
ide_io0
    IDE port #0
ide_io1
    IDE port #1
ide_irq
    IDE irq#
joystick_io
    Joystick I/O port

The driver requires firmware files ``turtlebeach/msndinit.bin`` and
``turtlebeach/msndperm.bin`` in the proper firmware directory.

See Documentation/sound/cards/multisound.sh for important information
about this driver.  Note that it has been discontinued, but the 
Voyetra Turtle Beach knowledge base entry for it is still available
at
https://www.turtlebeach.com

Module snd-msnd-pinnacle
------------------------

Module for Turtle Beach MultiSound Pinnacle/Fiji soundcards.

io
    Port # for pinnacle/fiji card
irq
    IRQ # for pinnalce/fiji card
mem
    Memory address (0xb0000, 0xc8000, 0xd0000, 0xd8000, 0xe0000 or 0xe8000)
write_ndelay
    enable write ndelay (default = 1)
calibrate_signal
    calibrate signal (default = 0)
isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

The driver requires firmware files ``turtlebeach/pndspini.bin`` and
``turtlebeach/pndsperm.bin`` in the proper firmware directory.

Module snd-mtpav
----------------

Module for MOTU MidiTimePiece AV multiport MIDI (on the parallel
port).

port
    I/O port # for MTPAV (0x378,0x278, default=0x378)
irq
    IRQ # for MTPAV (7,5, default=7)
hwports
    number of supported hardware ports, default=8.

Module supports only 1 card.  This module has no enable option.

Module snd-mts64
----------------

Module for Ego Systems (ESI) Miditerminal 4140

This module supports multiple devices.
Requires parport (``CONFIG_PARPORT``).

Module snd-nm256
----------------

Module for NeoMagic NM256AV/ZX chips

playback_bufsize
    max playback frame size in kB (4-128kB)
capture_bufsize
    max capture frame size in kB (4-128kB)
force_ac97
    0 or 1 (disabled by default)
buffer_top
    specify buffer top address
use_cache
    0 or 1 (disabled by default)
vaio_hack
    alias buffer_top=0x25a800
reset_workaround
    enable AC97 RESET workaround for some laptops
reset_workaround2
    enable extended AC97 RESET workaround for some other laptops

This module supports one chip and autoprobe.

The power-management is supported.

Note: on some notebooks the buffer address cannot be detected
automatically, or causes hang-up during initialization.
In such a case, specify the buffer top address explicitly via
the buffer_top option.
For example,
Sony F250: buffer_top=0x25a800
Sony F270: buffer_top=0x272800
The driver supports only ac97 codec.  It's possible to force
to initialize/use ac97 although it's not detected.  In such a
case, use ``force_ac97=1`` option - but *NO* guarantee whether it
works!

Note: The NM256 chip can be linked internally with non-AC97
codecs.  This driver supports only the AC97 codec, and won't work
with machines with other (most likely CS423x or OPL3SAx) chips,
even though the device is detected in lspci.  In such a case, try
other drivers, e.g. snd-cs4232 or snd-opl3sa2.  Some has ISA-PnP
but some doesn't have ISA PnP.  You'll need to specify ``isapnp=0``
and proper hardware parameters in the case without ISA PnP.

Note: some laptops need a workaround for AC97 RESET.  For the
known hardware like Dell Latitude LS and Sony PCG-F305, this
workaround is enabled automatically.  For other laptops with a
hard freeze, you can try ``reset_workaround=1`` option.

Note: Dell Latitude CSx laptops have another problem regarding
AC97 RESET.  On these laptops, reset_workaround2 option is
turned on as default.  This option is worth to try if the
previous reset_workaround option doesn't help.

Note: This driver is really crappy.  It's a porting from the
OSS driver, which is a result of black-magic reverse engineering.
The detection of codec will fail if the driver is loaded *after*
X-server as described above.  You might be able to force to load
the module, but it may result in hang-up.   Hence, make sure that
you load this module *before* X if you encounter this kind of
problem.

Module snd-opl3sa2
------------------

Module for Yamaha OPL3-SA2/SA3 sound cards.

isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

with ``isapnp=0``, the following options are available:

port
    control port # for OPL3-SA chip (0x370)
sb_port
    SB port # for OPL3-SA chip (0x220,0x240)
wss_port
    WSS port # for OPL3-SA chip (0x530,0xe80,0xf40,0x604)
midi_port
    port # for MPU-401 UART (0x300,0x330), -1 = disable
fm_port
    FM port # for OPL3-SA chip (0x388), -1 = disable
irq
    IRQ # for OPL3-SA chip (5,7,9,10)
dma1
    first DMA # for Yamaha OPL3-SA chip (0,1,3)
dma2
    second DMA # for Yamaha OPL3-SA chip (0,1,3), -1 = disable

This module supports multiple cards and ISA PnP.  It does not support
autoprobe (if ISA PnP is not used) thus all ports must be specified!!!

The power-management is supported.

Module snd-opti92x-ad1848
-------------------------

Module for sound cards based on OPTi 82c92x and Analog Devices AD1848 chips.
Module works with OAK Mozart cards as well.

isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

with ``isapnp=0``, the following options are available:

port
    port # for WSS chip (0x530,0xe80,0xf40,0x604)
mpu_port
    port # for MPU-401 UART (0x300,0x310,0x320,0x330)
fm_port
    port # for OPL3 device (0x388)
irq
    IRQ # for WSS chip (5,7,9,10,11)
mpu_irq
    IRQ # for MPU-401 UART (5,7,9,10)
dma1
    first DMA # for WSS chip (0,1,3)

This module supports only one card, autoprobe and PnP.

Module snd-opti92x-cs4231
-------------------------

Module for sound cards based on OPTi 82c92x and Crystal CS4231 chips.

isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

with ``isapnp=0``, the following options are available:

port
    port # for WSS chip (0x530,0xe80,0xf40,0x604)
mpu_port
    port # for MPU-401 UART (0x300,0x310,0x320,0x330)
fm_port
    port # for OPL3 device (0x388)
irq
    IRQ # for WSS chip (5,7,9,10,11)
mpu_irq
    IRQ # for MPU-401 UART (5,7,9,10)
dma1
    first DMA # for WSS chip (0,1,3)
dma2
    second DMA # for WSS chip (0,1,3)

This module supports only one card, autoprobe and PnP.

Module snd-opti93x
------------------

Module for sound cards based on OPTi 82c93x chips.

isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

with ``isapnp=0``, the following options are available:

port
    port # for WSS chip (0x530,0xe80,0xf40,0x604)
mpu_port
    port # for MPU-401 UART (0x300,0x310,0x320,0x330)
fm_port
    port # for OPL3 device (0x388)
irq
    IRQ # for WSS chip (5,7,9,10,11)
mpu_irq
    IRQ # for MPU-401 UART (5,7,9,10)
dma1
    first DMA # for WSS chip (0,1,3)
dma2
    second DMA # for WSS chip (0,1,3)

This module supports only one card, autoprobe and PnP.

Module snd-oxygen
-----------------

Module for sound cards based on the C-Media CMI8786/8787/8788 chip:

* Asound A-8788
* Asus Xonar DG/DGX
* AuzenTech X-Meridian
* AuzenTech X-Meridian 2G
* Bgears b-Enspirer
* Club3D Theatron DTS
* HT-Omega Claro (plus)
* HT-Omega Claro halo (XT)
* Kuroutoshikou CMI8787-HG2PCI
* Razer Barracuda AC-1
* Sondigo Inferno
* TempoTec HiFier Fantasia
* TempoTec HiFier Serenade
    
This module supports autoprobe and multiple cards.
  
Module snd-pcsp
---------------

Module for internal PC-Speaker.

nopcm
    Disable PC-Speaker PCM sound. Only beeps remain.
nforce_wa
    enable NForce chipset workaround. Expect bad sound.

This module supports system beeps, some kind of PCM playback and
even a few mixer controls.

Module snd-pcxhr
----------------

Module for Digigram PCXHR boards

This module supports multiple cards.

Module snd-portman2x4
---------------------

Module for Midiman Portman 2x4 parallel port MIDI interface

This module supports multiple cards.

Module snd-powermac (on ppc only)
---------------------------------

Module for PowerMac, iMac and iBook on-board soundchips

enable_beep
    enable beep using PCM (enabled as default)

Module supports autoprobe a chip.

Note: the driver may have problems regarding endianness.

The power-management is supported.

Module snd-pxa2xx-ac97 (on arm only)
------------------------------------

Module for AC97 driver for the Intel PXA2xx chip

For ARM architecture only.

The power-management is supported.

Module snd-riptide
------------------

Module for Conexant Riptide chip

joystick_port
    Joystick port # (default: 0x200)
mpu_port
    MPU401 port # (default: 0x330)
opl3_port
    OPL3 port # (default: 0x388)

This module supports multiple cards.
The driver requires the firmware loader support on kernel.
You need to install the firmware file ``riptide.hex`` to the standard
firmware path (e.g. /lib/firmware).

Module snd-rme32
----------------

Module for RME Digi32, Digi32 Pro and Digi32/8 (Sek'd Prodif32, 
Prodif96 and Prodif Gold) sound cards.

This module supports multiple cards.

Module snd-rme96
----------------

Module for RME Digi96, Digi96/8 and Digi96/8 PRO/PAD/PST sound cards.

This module supports multiple cards.

Module snd-rme9652
------------------

Module for RME Digi9652 (Hammerfall, Hammerfall-Light) sound cards.

precise_ptr
    Enable precise pointer (doesn't work reliably). (default = 0)

This module supports multiple cards.

Note: snd-page-alloc module does the job which snd-hammerfall-mem
module did formerly.  It will allocate the buffers in advance
when any RME9652 cards are found.  To make the buffer
allocation sure, load snd-page-alloc module in the early
stage of boot sequence.  See `Early Buffer Allocation`_
section.

Module snd-sa11xx-uda1341 (on arm only)
---------------------------------------

Module for Philips UDA1341TS on Compaq iPAQ H3600 sound card.

Module supports only one card.
Module has no enable and index options.

The power-management is supported.

Module snd-sb8
--------------

Module for 8-bit SoundBlaster cards: SoundBlaster 1.0, SoundBlaster 2.0,
SoundBlaster Pro

port
    port # for SB DSP chip (0x220,0x240,0x260)
irq
    IRQ # for SB DSP chip (5,7,9,10)
dma8
    DMA # for SB DSP chip (1,3)

This module supports multiple cards and autoprobe.

The power-management is supported.

Module snd-sb16 and snd-sbawe
-----------------------------

Module for 16-bit SoundBlaster cards: SoundBlaster 16 (PnP),
SoundBlaster AWE 32 (PnP), SoundBlaster AWE 64 PnP

mic_agc
    Mic Auto-Gain-Control - 0 = disable, 1 = enable (default)
csp
    ASP/CSP chip support - 0 = disable (default), 1 = enable
isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

with isapnp=0, the following options are available:

port
    port # for SB DSP 4.x chip (0x220,0x240,0x260)
mpu_port
    port # for MPU-401 UART (0x300,0x330), -1 = disable
awe_port
    base port # for EMU8000 synthesizer (0x620,0x640,0x660) (snd-sbawe
    module only)
irq
    IRQ # for SB DSP 4.x chip (5,7,9,10)
dma8
    8-bit DMA # for SB DSP 4.x chip (0,1,3)
dma16
    16-bit DMA # for SB DSP 4.x chip (5,6,7)

This module supports multiple cards, autoprobe and ISA PnP.

Note: To use Vibra16X cards in 16-bit half duplex mode, you must
disable 16bit DMA with dma16 = -1 module parameter.
Also, all Sound Blaster 16 type cards can operate in 16-bit
half duplex mode through 8-bit DMA channel by disabling their
16-bit DMA channel.

The power-management is supported.

Module snd-sc6000
-----------------

Module for Gallant SC-6000 soundcard and later models: SC-6600 and
SC-7000.

port
    Port # (0x220 or 0x240)
mss_port
    MSS Port # (0x530 or 0xe80)
irq
    IRQ # (5,7,9,10,11)
mpu_irq
    MPU-401 IRQ # (5,7,9,10) ,0 - no MPU-401 irq
dma
    DMA # (1,3,0)
joystick
    Enable gameport - 0 = disable (default), 1 = enable

This module supports multiple cards.

This card is also known as Audio Excel DSP 16 or Zoltrix AV302.

Module snd-sscape
-----------------

Module for ENSONIQ SoundScape cards.

port
    Port # (PnP setup)
wss_port
    WSS Port # (PnP setup)
irq
    IRQ # (PnP setup)
mpu_irq
    MPU-401 IRQ # (PnP setup)
dma
    DMA # (PnP setup)
dma2
    2nd DMA # (PnP setup, -1 to disable)
joystick
    Enable gameport - 0 = disable (default), 1 = enable

This module supports multiple cards.

The driver requires the firmware loader support on kernel.

Module snd-sun-amd7930 (on sparc only)
--------------------------------------

Module for AMD7930 sound chips found on Sparcs.

This module supports multiple cards.

Module snd-sun-cs4231 (on sparc only)
-------------------------------------

Module for CS4231 sound chips found on Sparcs.

This module supports multiple cards.

Module snd-sun-dbri (on sparc only)
-----------------------------------

Module for DBRI sound chips found on Sparcs.

This module supports multiple cards.

Module snd-wavefront
--------------------

Module for Turtle Beach Maui, Tropez and Tropez+ sound cards.

use_cs4232_midi
    Use CS4232 MPU-401 interface
    (inaccessibly located inside your computer)
isapnp
    ISA PnP detection - 0 = disable, 1 = enable (default)

with isapnp=0, the following options are available:

cs4232_pcm_port
    Port # for CS4232 PCM interface.
cs4232_pcm_irq
    IRQ # for CS4232 PCM interface (5,7,9,11,12,15).
cs4232_mpu_port
    Port # for CS4232 MPU-401 interface.
cs4232_mpu_irq
    IRQ # for CS4232 MPU-401 interface (9,11,12,15).
ics2115_port
    Port # for ICS2115
ics2115_irq
    IRQ # for ICS2115
fm_port
    FM OPL-3 Port #
dma1
    DMA1 # for CS4232 PCM interface.
dma2
    DMA2 # for CS4232 PCM interface.

The below are options for wavefront_synth features:

wf_raw
    Assume that we need to boot the OS (default:no);
    If yes, then during driver loading, the state of the board is
    ignored, and we reset the board and load the firmware anyway.
fx_raw
    Assume that the FX process needs help (default:yes);
    If false, we'll leave the FX processor in whatever state it is
    when the driver is loaded.  The default is to download the
    microprogram and associated coefficients to set it up for
    "default" operation, whatever that means.
debug_default
    Debug parameters for card initialization
wait_usecs
    How long to wait without sleeping, usecs (default:150);
    This magic number seems to give pretty optimal throughput
    based on my limited experimentation. 
    If you want to play around with it and find a better value, be
    my guest. Remember, the idea is to get a number that causes us
    to just busy wait for as many WaveFront commands as possible,
    without coming up with a number so large that we hog the whole
    CPU. 
    Specifically, with this number, out of about 134,000 status
    waits, only about 250 result in a sleep. 
sleep_interval
    How long to sleep when waiting for reply (default: 100)
sleep_tries
    How many times to try sleeping during a wait (default: 50)
ospath
    Pathname to processed ICS2115 OS firmware (default:wavefront.os);
    The path name of the ISC2115 OS firmware.  In the recent
    version, it's handled via firmware loader framework, so it
    must be installed in the proper path, typically,
    /lib/firmware.
reset_time
    How long to wait for a reset to take effect (default:2)
ramcheck_time
    How many seconds to wait for the RAM test (default:20)
osrun_time
    How many seconds to wait for the ICS2115 OS (default:10)

This module supports multiple cards and ISA PnP.

Note: the firmware file ``wavefront.os`` was located in the earlier
version in /etc.  Now it's loaded via firmware loader, and
must be in the proper firmware path, such as /lib/firmware.
Copy (or symlink) the file appropriately if you get an error
regarding firmware downloading after upgrading the kernel.

Module snd-sonicvibes
---------------------

Module for S3 SonicVibes PCI sound cards.
* PINE Schubert 32 PCI
  
reverb
    Reverb Enable - 1 = enable, 0 = disable (default);
    SoundCard must have onboard SRAM for this.
mge
    Mic Gain Enable - 1 = enable, 0 = disable (default)

This module supports multiple cards and autoprobe.

Module snd-serial-u16550
------------------------

Module for UART16550A serial MIDI ports.

port
    port # for UART16550A chip
irq
    IRQ # for UART16550A chip, -1 = poll mode
speed
    speed in bauds (9600,19200,38400,57600,115200)
    38400 = default
base
    base for divisor in bauds (57600,115200,230400,460800)
    115200 = default
outs
    number of MIDI ports in a serial port (1-4)
    1 = default
adaptor
    Type of adaptor.
	0 = Soundcanvas, 1 = MS-124T, 2 = MS-124W S/A,
	3 = MS-124W M/B, 4 = Generic

This module supports multiple cards. This module does not support autoprobe
thus the main port must be specified!!! Other options are optional.

Module snd-trident
------------------

Module for Trident 4DWave DX/NX sound cards.
* Best Union  Miss Melody 4DWave PCI
* HIS  4DWave PCI
* Warpspeed  ONSpeed 4DWave PCI
* AzTech  PCI 64-Q3D
* Addonics  SV 750
* CHIC  True Sound 4Dwave
* Shark  Predator4D-PCI
* Jaton  SonicWave 4D
* SiS SI7018 PCI Audio
* Hoontech SoundTrack Digital 4DWave NX
		    
pcm_channels
    max channels (voices) reserved for PCM
wavetable_size
    max wavetable size in kB (4-?kb)

This module supports multiple cards and autoprobe.

The power-management is supported.

Module snd-ua101
----------------

Module for the Edirol UA-101/UA-1000 audio/MIDI interfaces.

This module supports multiple devices, autoprobe and hotplugging.

Module snd-usb-audio
--------------------

Module for USB audio and USB MIDI devices.

vid
    Vendor ID for the device (optional)
pid
    Product ID for the device (optional)
nrpacks
    Max. number of packets per URB (default: 8)
device_setup
    Device specific magic number (optional);
    Influence depends on the device
    Default: 0x0000 
ignore_ctl_error
    Ignore any USB-controller regarding mixer interface (default: no)
autoclock
    Enable auto-clock selection for UAC2 devices (default: yes)
quirk_alias
    Quirk alias list, pass strings like ``0123abcd:5678beef``, which
    applies the existing quirk for the device 5678:beef to a new
    device 0123:abcd.
implicit_fb
    Apply the generic implicit feedback sync mode.  When this is set
    and the playback stream sync mode is ASYNC, the driver tries to
    tie an adjacent ASYNC capture stream as the implicit feedback
    source.
use_vmalloc
    Use vmalloc() for allocations of the PCM buffers (default: yes).
    For architectures with non-coherent memory like ARM or MIPS, the
    mmap access may give inconsistent results with vmalloc'ed
    buffers.  If mmap is used on such architectures, turn off this
    option, so that the DMA-coherent buffers are allocated and used
    instead.
delayed_register
    The option is needed for devices that have multiple streams
    defined in multiple USB interfaces.  The driver may invoke
    registrations multiple times (once per interface) and this may
    lead to the insufficient device enumeration.
    This option receives an array of strings, and you can pass
    ID:INTERFACE like ``0123abcd:4`` for performing the delayed
    registration to the given device.  In this example, when a USB
    device 0123:abcd is probed, the driver waits the registration
    until the USB interface 4 gets probed.
    The driver prints a message like "Found post-registration device
    assignment: 1234abcd:04" for such a device, so that user can
    notice the need.

This module supports multiple devices, autoprobe and hotplugging.

NB: ``nrpacks`` parameter can be modified dynamically via sysfs.
Don't put the value over 20.  Changing via sysfs has no sanity
check.

NB: ``ignore_ctl_error=1`` may help when you get an error at accessing
the mixer element such as URB error -22.  This happens on some
buggy USB device or the controller.

NB: quirk_alias option is provided only for testing / development.
If you want to have a proper support, contact to upstream for
adding the matching quirk in the driver code statically.

Module snd-usb-caiaq
--------------------

Module for caiaq UB audio interfaces,

* Native Instruments RigKontrol2
* Native Instruments Kore Controller
* Native Instruments Audio Kontrol 1
* Native Instruments Audio 8 DJ
	
This module supports multiple devices, autoprobe and hotplugging.
  
Module snd-usb-usx2y
--------------------

Module for Tascam USB US-122, US-224 and US-428 devices.

This module supports multiple devices, autoprobe and hotplugging.

Note: you need to load the firmware via ``usx2yloader`` utility included
in alsa-tools and alsa-firmware packages.

Module snd-via82xx
------------------

Module for AC'97 motherboards based on VIA 82C686A/686B, 8233, 8233A,
8233C, 8235, 8237 (south) bridge.

mpu_port
    0x300,0x310,0x320,0x330, otherwise obtain BIOS setup
    [VIA686A/686B only]
joystick
    Enable joystick (default off) [VIA686A/686B only]
ac97_clock
    AC'97 codec clock base (default 48000Hz)
dxs_support
    support DXS channels, 0 = auto (default), 1 = enable, 2 = disable,
    3 = 48k only, 4 = no VRA, 5 = enable any sample rate and different
    sample rates on different channels [VIA8233/C, 8235, 8237 only]
ac97_quirk
    AC'97 workaround for strange hardware;
    See `AC97 Quirk Option`_ section below.

This module supports one chip and autoprobe.

Note: on some SMP motherboards like MSI 694D the interrupts might
not be generated properly.  In such a case, please try to
set the SMP (or MPS) version on BIOS to 1.1 instead of
default value 1.4.  Then the interrupt number will be
assigned under 15. You might also upgrade your BIOS.

Note: VIA8233/5/7 (not VIA8233A) can support DXS (direct sound)
channels as the first PCM.  On these channels, up to 4
streams can be played at the same time, and the controller
can perform sample rate conversion with separate rates for
each channel.
As default (``dxs_support = 0``), 48k fixed rate is chosen
except for the known devices since the output is often
noisy except for 48k on some mother boards due to the
bug of BIOS.
Please try once ``dxs_support=5`` and if it works on other
sample rates (e.g. 44.1kHz of mp3 playback), please let us
know the PCI subsystem vendor/device id's (output of
``lspci -nv``).
If ``dxs_support=5`` does not work, try ``dxs_support=4``; if it
doesn't work too, try dxs_support=1.  (dxs_support=1 is
usually for old motherboards.  The correct implemented
board should work with 4 or 5.)  If it still doesn't
work and the default setting is ok, ``dxs_support=3`` is the
right choice.  If the default setting doesn't work at all,
try ``dxs_support=2`` to disable the DXS channels.
In any cases, please let us know the result and the
subsystem vendor/device ids.  See `Links and Addresses`_
below.

Note: for the MPU401 on VIA823x, use snd-mpu401 driver
additionally.  The mpu_port option is for VIA686 chips only.

The power-management is supported.

Module snd-via82xx-modem
------------------------

Module for VIA82xx AC97 modem

ac97_clock
    AC'97 codec clock base (default 48000Hz)

This module supports one card and autoprobe.

Note: The default index value of this module is -2, i.e. the first
slot is excluded.

The power-management is supported.

Module snd-virmidi
------------------

Module for virtual rawmidi devices.
This module creates virtual rawmidi devices which communicate
to the corresponding ALSA sequencer ports.

midi_devs
    MIDI devices # (1-4, default=4)

This module supports multiple cards.

Module snd-virtuoso
-------------------

Module for sound cards based on the Asus AV66/AV100/AV200 chips,
i.e., Xonar D1, DX, D2, D2X, DS, DSX, Essence ST (Deluxe),
Essence STX (II), HDAV1.3 (Deluxe), and HDAV1.3 Slim.

This module supports autoprobe and multiple cards.

Module snd-vx222
----------------

Module for Digigram VX-Pocket VX222, V222 v2 and Mic cards.

mic
    Enable Microphone on V222 Mic (NYI)
ibl
    Capture IBL size. (default = 0, minimum size)

This module supports multiple cards.

When the driver is compiled as a module and the hotplug firmware
is supported, the firmware data is loaded via hotplug automatically.
Install the necessary firmware files in alsa-firmware package.
When no hotplug fw loader is available, you need to load the
firmware via vxloader utility in alsa-tools package.  To invoke
vxloader automatically, add the following to /etc/modprobe.d/alsa.conf

::

  install snd-vx222 /sbin/modprobe --first-time -i snd-vx222\
    && /usr/bin/vxloader

(for 2.2/2.4 kernels, add ``post-install /usr/bin/vxloader`` to
/etc/modules.conf, instead.)
IBL size defines the interrupts period for PCM.  The smaller size
gives smaller latency but leads to more CPU consumption, too.
The size is usually aligned to 126.  As default (=0), the smallest
size is chosen.  The possible IBL values can be found in
/proc/asound/cardX/vx-status proc file.

The power-management is supported.

Module snd-vxpocket
-------------------

Module for Digigram VX-Pocket VX2 and 440 PCMCIA cards.

ibl
    Capture IBL size. (default = 0, minimum size)

This module supports multiple cards.  The module is compiled only when
PCMCIA is supported on kernel.

With the older 2.6.x kernel, to activate the driver via the card
manager, you'll need to set up /etc/pcmcia/vxpocket.conf.  See the
sound/pcmcia/vx/vxpocket.c.  2.6.13 or later kernel requires no
longer require a config file.

When the driver is compiled as a module and the hotplug firmware
is supported, the firmware data is loaded via hotplug automatically.
Install the necessary firmware files in alsa-firmware package.
When no hotplug fw loader is available, you need to load the
firmware via vxloader utility in alsa-tools package.

About capture IBL, see the description of snd-vx222 module.

Note: snd-vxp440 driver is merged to snd-vxpocket driver since
ALSA 1.0.10.

The power-management is supported.

Module snd-ymfpci
-----------------

Module for Yamaha PCI chips (YMF72x, YMF74x & YMF75x).

mpu_port
    0x300,0x330,0x332,0x334, 0 (disable) by default,
    1 (auto-detect for YMF744/754 only)
fm_port
    0x388,0x398,0x3a0,0x3a8, 0 (disable) by default
    1 (auto-detect for YMF744/754 only)
joystick_port
    0x201,0x202,0x204,0x205, 0 (disable) by default,
    1 (auto-detect)
rear_switch
    enable shared rear/line-in switch (bool)

This module supports autoprobe and multiple chips.

The power-management is supported.

Module snd-pdaudiocf
--------------------

Module for Sound Core PDAudioCF sound card.

The power-management is supported.


AC97 Quirk Option
=================

The ac97_quirk option is used to enable/override the workaround for
specific devices on drivers for on-board AC'97 controllers like
snd-intel8x0.  Some hardware have swapped output pins between Master
and Headphone, or Surround (thanks to confusion of AC'97
specifications from version to version :-)

The driver provides the auto-detection of known problematic devices,
but some might be unknown or wrongly detected.  In such a case, pass
the proper value with this option.

The following strings are accepted:

default
    Don't override the default setting
none
    Disable the quirk
hp_only
    Bind Master and Headphone controls as a single control
swap_hp
    Swap headphone and master controls
swap_surround
    Swap master and surround controls
ad_sharing
    For AD1985, turn on OMS bit and use headphone
alc_jack
    For ALC65x, turn on the jack sense mode
inv_eapd
    Inverted EAPD implementation
mute_led
    Bind EAPD bit for turning on/off mute LED

For backward compatibility, the corresponding integer value -1, 0, ...
are  accepted, too.

For example, if ``Master`` volume control has no effect on your device
but only ``Headphone`` does, pass ac97_quirk=hp_only module option.


Configuring Non-ISAPNP Cards
============================

When the kernel is configured with ISA-PnP support, the modules
supporting the isapnp cards will have module options ``isapnp``.
If this option is set, *only* the ISA-PnP devices will be probed.
For probing the non ISA-PnP cards, you have to pass ``isapnp=0`` option
together with the proper i/o and irq configuration.

When the kernel is configured without ISA-PnP support, isapnp option
will be not built in.


Module Autoloading Support
==========================

The ALSA drivers can be loaded automatically on demand by defining
module aliases.  The string ``snd-card-%1`` is requested for ALSA native
devices where ``%i`` is sound card number from zero to seven.

To auto-load an ALSA driver for OSS services, define the string
``sound-slot-%i`` where ``%i`` means the slot number for OSS, which
corresponds to the card index of ALSA.  Usually, define this
as the same card module.

An example configuration for a single emu10k1 card is like below:
::

    ----- /etc/modprobe.d/alsa.conf
    alias snd-card-0 snd-emu10k1
    alias sound-slot-0 snd-emu10k1
    ----- /etc/modprobe.d/alsa.conf

The available number of auto-loaded sound cards depends on the module
option ``cards_limit`` of snd module.  As default it's set to 1.
To enable the auto-loading of multiple cards, specify the number of
sound cards in that option.

When multiple cards are available, it'd better to specify the index
number for each card via module option, too, so that the order of
cards is kept consistent.

An example configuration for two sound cards is like below:
::

    ----- /etc/modprobe.d/alsa.conf
    # ALSA portion
    options snd cards_limit=2
    alias snd-card-0 snd-interwave
    alias snd-card-1 snd-ens1371
    options snd-interwave index=0
    options snd-ens1371 index=1
    # OSS/Free portion
    alias sound-slot-0 snd-interwave
    alias sound-slot-1 snd-ens1371
    ----- /etc/modprobe.d/alsa.conf

In this example, the interwave card is always loaded as the first card
(index 0) and ens1371 as the second (index 1).

Alternative (and new) way to fixate the slot assignment is to use
``slots`` option of snd module.  In the case above, specify like the
following: 
::

    options snd slots=snd-interwave,snd-ens1371

Then, the first slot (#0) is reserved for snd-interwave driver, and
the second (#1) for snd-ens1371.  You can omit index option in each
driver if slots option is used (although you can still have them at
the same time as long as they don't conflict).

The slots option is especially useful for avoiding the possible
hot-plugging and the resultant slot conflict.  For example, in the
case above again, the first two slots are already reserved.  If any
other driver (e.g. snd-usb-audio) is loaded before snd-interwave or
snd-ens1371, it will be assigned to the third or later slot.

When a module name is given with '!', the slot will be given for any
modules but that name.  For example, ``slots=!snd-pcsp`` will reserve
the first slot for any modules but snd-pcsp. 


ALSA PCM devices to OSS devices mapping
=======================================
::

    /dev/snd/pcmC0D0[c|p]  -> /dev/audio0 (/dev/audio) -> minor 4
    /dev/snd/pcmC0D0[c|p]  -> /dev/dsp0 (/dev/dsp)     -> minor 3
    /dev/snd/pcmC0D1[c|p]  -> /dev/adsp0 (/dev/adsp)   -> minor 12
    /dev/snd/pcmC1D0[c|p]  -> /dev/audio1              -> minor 4+16 = 20
    /dev/snd/pcmC1D0[c|p]  -> /dev/dsp1                -> minor 3+16 = 19
    /dev/snd/pcmC1D1[c|p]  -> /dev/adsp1               -> minor 12+16 = 28
    /dev/snd/pcmC2D0[c|p]  -> /dev/audio2              -> minor 4+32 = 36
    /dev/snd/pcmC2D0[c|p]  -> /dev/dsp2                -> minor 3+32 = 39
    /dev/snd/pcmC2D1[c|p]  -> /dev/adsp2               -> minor 12+32 = 44

The first number from ``/dev/snd/pcmC{X}D{Y}[c|p]`` expression means
sound card number and second means device number.  The ALSA devices
have either ``c`` or ``p`` suffix indicating the direction, capture and
playback, respectively.

Please note that the device mapping above may be varied via the module
options of snd-pcm-oss module.


Proc interfaces (/proc/asound)
==============================

/proc/asound/card#/pcm#[cp]/oss
-------------------------------
erase
    erase all additional information about OSS applications

<app_name> <fragments> <fragment_size> [<options>]
    <app_name>
	name of application with (higher priority) or without path
    <fragments>
	 number of fragments or zero if auto
    <fragment_size>
	 size of fragment in bytes or zero if auto
    <options>
	optional parameters

	disable
	    the application tries to open a pcm device for
	    this channel but does not want to use it.
	    (Cause a bug or mmap needs)
	    It's good for Quake etc...
	direct
	    don't use plugins
	block
	     force block mode (rvplayer)
	non-block
	    force non-block mode
	whole-frag
	    write only whole fragments (optimization affecting
	    playback only)
	no-silence
	    do not fill silence ahead to avoid clicks
	buggy-ptr
	    Returns the whitespace blocks in GETOPTR ioctl
	    instead of filled blocks

Example:
::

    echo "x11amp 128 16384" > /proc/asound/card0/pcm0p/oss
    echo "squake 0 0 disable" > /proc/asound/card0/pcm0c/oss
    echo "rvplayer 0 0 block" > /proc/asound/card0/pcm0p/oss


Early Buffer Allocation
=======================

Some drivers (e.g. hdsp) require the large contiguous buffers, and
sometimes it's too late to find such spaces when the driver module is
actually loaded due to memory fragmentation.  You can pre-allocate the
PCM buffers by loading snd-page-alloc module and write commands to its
proc file in prior, for example, in the early boot stage like
``/etc/init.d/*.local`` scripts.

Reading the proc file /proc/drivers/snd-page-alloc shows the current
usage of page allocation.  In writing, you can send the following
commands to the snd-page-alloc driver:

* add VENDOR DEVICE MASK SIZE BUFFERS

VENDOR and DEVICE are PCI vendor and device IDs.  They take
integer numbers (0x prefix is needed for the hex).
MASK is the PCI DMA mask.  Pass 0 if not restricted.
SIZE is the size of each buffer to allocate.  You can pass
k and m suffix for KB and MB.  The max number is 16MB.
BUFFERS is the number of buffers to allocate.  It must be greater
than 0.  The max number is 4.

* erase

This will erase the all pre-allocated buffers which are not in
use.


Links and Addresses
===================

ALSA project homepage
    http://www.alsa-project.org
Kernel Bugzilla
    http://bugzilla.kernel.org/
ALSA Developers ML
    mailto:alsa-devel@alsa-project.org
alsa-info.sh script
    https://www.alsa-project.org/alsa-info.sh
