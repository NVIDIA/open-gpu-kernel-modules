=====================================
Amiga Buddha and Catweasel IDE Driver
=====================================

The Amiga Buddha and Catweasel IDE Driver (part of ide.c) was written by
Geert Uytterhoeven based on the following specifications:

------------------------------------------------------------------------

Register map of the Buddha IDE controller and the
Buddha-part of the Catweasel Zorro-II version

The Autoconfiguration has been implemented just as Commodore
described  in  their  manuals, no tricks have been used (for
example leaving some address lines out of the equations...).
If you want to configure the board yourself (for example let
a  Linux  kernel  configure the card), look at the Commodore
Docs.  Reading the nibbles should give this information::

  Vendor number: 4626 ($1212)
  product number: 0 (42 for Catweasel Z-II)
  Serial number: 0
  Rom-vector: $1000

The  card  should be a Z-II board, size 64K, not for freemem
list, Rom-Vektor is valid, no second Autoconfig-board on the
same card, no space preference, supports "Shutup_forever".

Setting  the  base address should be done in two steps, just
as  the Amiga Kickstart does:  The lower nibble of the 8-Bit
address is written to $4a, then the whole Byte is written to
$48, while it doesn't matter how often you're writing to $4a
as  long as $48 is not touched.  After $48 has been written,
the  whole card disappears from $e8 and is mapped to the new
address just written.  Make sure $4a is written before $48,
otherwise your chance is only 1:16 to find the board :-).

The local memory-map is even active when mapped to $e8:

==============  ===========================================
$0-$7e		Autokonfig-space, see Z-II docs.

$80-$7fd	reserved

$7fe		Speed-select Register: Read & Write
		(description see further down)

$800-$8ff	IDE-Select 0 (Port 0, Register set 0)

$900-$9ff	IDE-Select 1 (Port 0, Register set 1)

$a00-$aff	IDE-Select 2 (Port 1, Register set 0)

$b00-$bff	IDE-Select 3 (Port 1, Register set 1)

$c00-$cff	IDE-Select 4 (Port 2, Register set 0,
                Catweasel only!)

$d00-$dff	IDE-Select 5 (Port 3, Register set 1,
		Catweasel only!)

$e00-$eff	local expansion port, on Catweasel Z-II the
		Catweasel registers are also mapped here.
		Never touch, use multidisk.device!

$f00		read only, Byte-access: Bit 7 shows the
		level of the IRQ-line of IDE port 0.

$f01-$f3f	mirror of $f00

$f40		read only, Byte-access: Bit 7 shows the
		level of the IRQ-line of IDE port 1.

$f41-$f7f	mirror of $f40

$f80		read only, Byte-access: Bit 7 shows the
		level of the IRQ-line of IDE port 2.
		(Catweasel only!)

$f81-$fbf	mirror of $f80

$fc0		write-only: Writing any value to this
		register enables IRQs to be passed from the
		IDE ports to the Zorro bus. This mechanism
		has been implemented to be compatible with
		harddisks that are either defective or have
		a buggy firmware and pull the IRQ line up
		while starting up. If interrupts would
		always be passed to the bus, the computer
		might not start up. Once enabled, this flag
		can not be disabled again. The level of the
		flag can not be determined by software
		(what for? Write to me if it's necessary!).

$fc1-$fff	mirror of $fc0

$1000-$ffff	Buddha-Rom with offset $1000 in the rom
		chip. The addresses $0 to $fff of the rom
		chip cannot be read. Rom is Byte-wide and
		mapped to even addresses.
==============  ===========================================

The  IDE ports issue an INT2.  You can read the level of the
IRQ-lines  of  the  IDE-ports by reading from the three (two
for  Buddha-only)  registers  $f00, $f40 and $f80.  This way
more  than one I/O request can be handled and you can easily
determine  what  driver  has  to serve the INT2.  Buddha and
Catweasel  expansion  boards  can issue an INT6.  A separate
memory  map  is available for the I/O module and the sysop's
I/O module.

The IDE ports are fed by the address lines A2 to A4, just as
the  Amiga  1200  and  Amiga  4000  IDE ports are.  This way
existing  drivers  can be easily ported to Buddha.  A move.l
polls  two  words  out of the same address of IDE port since
every  word  is  mirrored  once.  movem is not possible, but
it's  not  necessary  either,  because  you can only speedup
68000  systems  with  this  technique.   A 68020 system with
fastmem is faster with move.l.

If you're using the mirrored registers of the IDE-ports with
A6=1,  the Buddha doesn't care about the speed that you have
selected  in  the  speed  register (see further down).  With
A6=1  (for example $840 for port 0, register set 0), a 780ns
access  is being made.  These registers should be used for a
command   access   to  the  harddisk/CD-Rom,  since  command
accesses  are Byte-wide and have to be made slower according
to the ATA-X3T9 manual.

Now  for the speed-register:  The register is byte-wide, and
only  the  upper  three  bits are used (Bits 7 to 5).  Bit 4
must  always  be set to 1 to be compatible with later Buddha
versions  (if  I'll  ever  update this one).  I presume that
I'll  never use the lower four bits, but they have to be set
to 1 by definition.

The  values in this table have to be shifted 5 bits to the
left and or'd with $1f (this sets the lower 5 bits).

All  the timings have in common:  Select and IOR/IOW rise at
the  same  time.   IOR  and  IOW have a propagation delay of
about  30ns  to  the clocks on the Zorro bus, that's why the
values  are no multiple of 71.  One clock-cycle is 71ns long
(exactly 70,5 at 14,18 Mhz on PAL systems).

value 0 (Default after reset)
  497ns Select (7 clock cycles) , IOR/IOW after 172ns (2 clock cycles)
  (same timing as the Amiga 1200 does on it's IDE port without
  accelerator card)

value 1
  639ns Select (9 clock cycles), IOR/IOW after 243ns (3 clock cycles)

value 2
  781ns Select (11 clock cycles), IOR/IOW after 314ns (4 clock cycles)

value 3
  355ns Select (5 clock cycles), IOR/IOW after 101ns (1 clock cycle)

value 4
  355ns Select (5 clock cycles), IOR/IOW after 172ns (2 clock cycles)

value 5
  355ns Select (5 clock cycles), IOR/IOW after 243ns (3 clock cycles)

value 6
  1065ns Select (15 clock cycles), IOR/IOW after 314ns (4 clock cycles)

value 7
  355ns Select, (5 clock cycles), IOR/IOW after 101ns (1 clock cycle)

When accessing IDE registers with A6=1 (for example $84x),
the timing will always be mode 0 8-bit compatible, no matter
what you have selected in the speed register:

781ns select, IOR/IOW after 4 clock cycles (=314ns) aktive.

All  the  timings with a very short select-signal (the 355ns
fast  accesses)  depend  on the accelerator card used in the
system:  Sometimes two more clock cycles are inserted by the
bus  interface,  making  the  whole access 497ns long.  This
doesn't  affect  the  reliability  of the controller nor the
performance  of  the  card,  since  this doesn't happen very
often.

All  the  timings  are  calculated  and  only  confirmed  by
measurements  that allowed me to count the clock cycles.  If
the  system  is clocked by an oscillator other than 28,37516
Mhz  (for  example  the  NTSC-frequency  28,63636 Mhz), each
clock  cycle is shortened to a bit less than 70ns (not worth
mentioning).   You  could think of a small performance boost
by  overclocking  the  system,  but  you would either need a
multisync  monitor,  or  a  graphics card, and your internal
diskdrive would go crazy, that's why you shouldn't tune your
Amiga this way.

Giving  you  the  possibility  to  write  software  that  is
compatible  with both the Buddha and the Catweasel Z-II, The
Buddha  acts  just  like  a  Catweasel  Z-II  with no device
connected  to  the  third  IDE-port.   The IRQ-register $f80
always  shows a "no IRQ here" on the Buddha, and accesses to
the  third  IDE  port  are  going into data's Nirwana on the
Buddha.

Jens Schönfeld february 19th, 1997

updated may 27th, 1997

eMail: sysop@nostlgic.tng.oche.de
