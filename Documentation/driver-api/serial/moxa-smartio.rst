=============================================================
MOXA Smartio/Industio Family Device Driver Installation Guide
=============================================================

.. note::

   This file is outdated. It needs some care in order to make it
   updated to Kernel 5.0 and upper

Copyright (C) 2008, Moxa Inc.

Date: 01/21/2008

.. Content

   1. Introduction
   2. System Requirement
   3. Installation
      3.1 Hardware installation
      3.2 Driver files
      3.3 Device naming convention
      3.4 Module driver configuration
      3.5 Static driver configuration for Linux kernel 2.4.x and 2.6.x.
      3.6 Custom configuration
      3.7 Verify driver installation
   4. Utilities
   5. Setserial
   6. Troubleshooting

1. Introduction
^^^^^^^^^^^^^^^

   The Smartio/Industio/UPCI family Linux driver supports following multiport
   boards.

    - 2 ports multiport board
	CP-102U, CP-102UL, CP-102UF
	CP-132U-I, CP-132UL,
	CP-132, CP-132I, CP132S, CP-132IS,
	CI-132, CI-132I, CI-132IS,
	(C102H, C102HI, C102HIS, C102P, CP-102, CP-102S)

    - 4 ports multiport board
	CP-104EL,
	CP-104UL, CP-104JU,
	CP-134U, CP-134U-I,
	C104H/PCI, C104HS/PCI,
	CP-114, CP-114I, CP-114S, CP-114IS, CP-114UL,
	C104H, C104HS,
	CI-104J, CI-104JS,
	CI-134, CI-134I, CI-134IS,
	(C114HI, CT-114I, C104P),
	POS-104UL,
	CB-114,
	CB-134I

    - 8 ports multiport board
	CP-118EL, CP-168EL,
	CP-118U, CP-168U,
	C168H/PCI,
	C168H, C168HS,
	(C168P),
	CB-108

   This driver and installation procedure have been developed upon Linux Kernel
   2.4.x and 2.6.x. This driver supports Intel x86 hardware platform. In order
   to maintain compatibility, this version has also been properly tested with
   RedHat, Mandrake, Fedora and S.u.S.E Linux. However, if compatibility problem
   occurs, please contact Moxa at support@moxa.com.tw.

   In addition to device driver, useful utilities are also provided in this
   version. They are:

    - msdiag
		 Diagnostic program for displaying installed Moxa
                 Smartio/Industio boards.
    - msmon
		 Monitor program to observe data count and line status signals.
    - msterm     A simple terminal program which is useful in testing serial
	         ports.
    - io-irq.exe
		 Configuration program to setup ISA boards. Please note that
                 this program can only be executed under DOS.

   All the drivers and utilities are published in form of source code under
   GNU General Public License in this version. Please refer to GNU General
   Public License announcement in each source code file for more detail.

   In Moxa's Web sites, you may always find latest driver at http://www.moxa.com/.

   This version of driver can be installed as Loadable Module (Module driver)
   or built-in into kernel (Static driver). You may refer to following
   installation procedure for suitable one. Before you install the driver,
   please refer to hardware installation procedure in the User's Manual.

   We assume the user should be familiar with following documents.

   - Serial-HOWTO
   - Kernel-HOWTO

2. System Requirement
^^^^^^^^^^^^^^^^^^^^^

   - Hardware platform: Intel x86 machine
   - Kernel version: 2.4.x or 2.6.x
   - gcc version 2.72 or later
   - Maximum 4 boards can be installed in combination

3. Installation
^^^^^^^^^^^^^^^

3.1 Hardware installation
=========================

   There are two types of buses, ISA and PCI, for Smartio/Industio
   family multiport board.

ISA board
---------

   You'll have to configure CAP address, I/O address, Interrupt Vector
   as well as IRQ before installing this driver. Please refer to hardware
   installation procedure in User's Manual before proceed any further.
   Please make sure the JP1 is open after the ISA board is set properly.

PCI/UPCI board
--------------

   You may need to adjust IRQ usage in BIOS to avoid from IRQ conflict
   with other ISA devices. Please refer to hardware installation
   procedure in User's Manual in advance.

PCI IRQ Sharing
---------------

   Each port within the same multiport board shares the same IRQ. Up to
   4 Moxa Smartio/Industio PCI Family multiport boards can be installed
   together on one system and they can share the same IRQ.


3.2 Driver files
================

   The driver file may be obtained from ftp, CD-ROM or floppy disk. The
   first step, anyway, is to copy driver file "mxser.tgz" into specified
   directory. e.g. /moxa. The execute commands as below::

       # cd /
       # mkdir moxa
       # cd /moxa
       # tar xvf /dev/fd0

or::

       # cd /
       # mkdir moxa
       # cd /moxa
       # cp /mnt/cdrom/<driver directory>/mxser.tgz .
       # tar xvfz mxser.tgz


3.3 Device naming convention
============================

   You may find all the driver and utilities files in /moxa/mxser.
   Following installation procedure depends on the model you'd like to
   run the driver. If you prefer module driver, please refer to 3.4.
   If static driver is required, please refer to 3.5.

Dialin and callout port
-----------------------

   This driver remains traditional serial device properties. There are
   two special file name for each serial port. One is dial-in port
   which is named "ttyMxx". For callout port, the naming convention
   is "cumxx".

Device naming when more than 2 boards installed
-----------------------------------------------

   Naming convention for each Smartio/Industio multiport board is
   pre-defined as below.

   ============ ===============       ==============
   Board Num.	 Dial-in Port	      Callout port
   1st board	ttyM0  - ttyM7	      cum0  - cum7
   2nd board	ttyM8  - ttyM15       cum8  - cum15
   3rd board	ttyM16 - ttyM23       cum16 - cum23
   4th board	ttyM24 - ttym31       cum24 - cum31
   ============ ===============       ==============

.. note::

   Under Kernel 2.6 and upper, the cum Device is Obsolete. So use ttyM*
   device instead.

Board sequence
--------------

   This driver will activate ISA boards according to the parameter set
   in the driver. After all specified ISA board activated, PCI board
   will be installed in the system automatically driven.
   Therefore the board number is sorted by the CAP address of ISA boards.
   For PCI boards, their sequence will be after ISA boards and C168H/PCI
   has higher priority than C104H/PCI boards.

3.4 Module driver configuration
===============================

   Module driver is easiest way to install. If you prefer static driver
   installation, please skip this paragraph.


   ------------- Prepare to use the MOXA driver --------------------

3.4.1 Create tty device with correct major number
-------------------------------------------------

   Before using MOXA driver, your system must have the tty devices
   which are created with driver's major number. We offer one shell
   script "msmknod" to simplify the procedure.
   This step is only needed to be executed once. But you still
   need to do this procedure when:

   a. You change the driver's major number. Please refer the "3.7"
      section.
   b. Your total installed MOXA boards number is changed. Maybe you
      add/delete one MOXA board.
   c. You want to change the tty name. This needs to modify the
      shell script "msmknod"

   The procedure is::

	 # cd /moxa/mxser/driver
	 # ./msmknod

   This shell script will require the major number for dial-in
   device and callout device to create tty device. You also need
   to specify the total installed MOXA board number. Default major
   numbers for dial-in device and callout device are 30, 35. If
   you need to change to other number, please refer section "3.7"
   for more detailed procedure.
   Msmknod will delete any special files occupying the same device
   naming.

3.4.2 Build the MOXA driver and utilities
-----------------------------------------

   Before using the MOXA driver and utilities, you need compile the
   all the source code. This step is only need to be executed once.
   But you still re-compile the source code if you modify the source
   code. For example, if you change the driver's major number (see
   "3.7" section), then you need to do this step again.

   Find "Makefile" in /moxa/mxser, then run

	 # make clean; make install

   ..note::

	 For Red Hat 9, Red Hat Enterprise Linux AS3/ES3/WS3 & Fedora Core1:
	 # make clean; make installsp1

	 For Red Hat Enterprise Linux AS4/ES4/WS4:
	 # make clean; make installsp2

   The driver files "mxser.o" and utilities will be properly compiled
   and copied to system directories respectively.

------------- Load MOXA driver--------------------

3.4.3 Load the MOXA driver
--------------------------

   ::

	 # modprobe mxser <argument>

   will activate the module driver. You may run "lsmod" to check
   if "mxser" is activated. If the MOXA board is ISA board, the
   <argument> is needed. Please refer to section "3.4.5" for more
   information.

------------- Load MOXA driver on boot --------------------

3.4.4 Load the mxser driver
---------------------------


   For the above description, you may manually execute
   "modprobe mxser" to activate this driver and run
   "rmmod mxser" to remove it.

   However, it's better to have a boot time configuration to
   eliminate manual operation. Boot time configuration can be
   achieved by rc file. We offer one "rc.mxser" file to simplify
   the procedure under "moxa/mxser/driver".

   But if you use ISA board, please modify the "modprobe ..." command
   to add the argument (see "3.4.5" section). After modifying the
   rc.mxser, please try to execute "/moxa/mxser/driver/rc.mxser"
   manually to make sure the modification is ok. If any error
   encountered, please try to modify again. If the modification is
   completed, follow the below step.

   Run following command for setting rc files::

	 # cd /moxa/mxser/driver
	 # cp ./rc.mxser /etc/rc.d
	 # cd /etc/rc.d

   Check "rc.serial" is existed or not. If "rc.serial" doesn't exist,
   create it by vi, run "chmod 755 rc.serial" to change the permission.

   Add "/etc/rc.d/rc.mxser" in last line.

   Reboot and check if moxa.o activated by "lsmod" command.

3.4.5. specify CAP address
--------------------------

   If you'd like to drive Smartio/Industio ISA boards in the system,
   you'll have to add parameter to specify CAP address of given
   board while activating "mxser.o". The format for parameters are
   as follows.::

	   modprobe mxser ioaddr=0x???,0x???,0x???,0x???
				  |  |  |    |
				  |  |  |    +- 4th ISA board
				  |  |  +------ 3rd ISA board
				  |  +------------ 2nd ISA board
				  +-------------------1st ISA board

3.5 Static driver configuration for Linux kernel 2.4.x and 2.6.x
================================================================

    Note:
          To use static driver, you must install the linux kernel
          source package.

3.5.1 Backup the built-in driver in the kernel
----------------------------------------------

    ::

       # cd /usr/src/linux/drivers/char
       # mv mxser.c mxser.c.old

       For Red Hat 7.x user, you need to create link:
       # cd /usr/src
       # ln -s linux-2.4 linux

3.5.2 Create link
-----------------
    ::

	  # cd /usr/src/linux/drivers/char
	  # ln -s /moxa/mxser/driver/mxser.c mxser.c

3.5.3 Add CAP address list for ISA boards.
------------------------------------------

    For PCI boards user, please skip this step.

    In module mode, the CAP address for ISA board is given by
    parameter. In static driver configuration, you'll have to
    assign it within driver's source code. If you will not
    install any ISA boards, you may skip to next portion.
    The instructions to modify driver source code are as
    below.

    a. run::

	# cd /moxa/mxser/driver
	# vi mxser.c

    b. Find the array mxserBoardCAP[] as below::

	  static int mxserBoardCAP[] = {0x00, 0x00, 0x00, 0x00};

    c. Change the address within this array using vi. For
       example, to driver 2 ISA boards with CAP address
       0x280 and 0x180 as 1st and 2nd board. Just to change
       the source code as follows::

	  static int mxserBoardCAP[] = {0x280, 0x180, 0x00, 0x00};

3.5.4 Setup kernel configuration
--------------------------------

    Configure the kernel::

      # cd /usr/src/linux
      # make menuconfig

    You will go into a menu-driven system. Please select [Character
    devices][Non-standard serial port support], enable the [Moxa
    SmartIO support] driver with "[*]" for built-in (not "[M]"), then
    select [Exit] to exit this program.

3.5.5 Rebuild kernel
--------------------

    The following are for Linux kernel rebuilding, for your
    reference only.

    For appropriate details, please refer to the Linux document:

        a. Run the following commands::

	     cd /usr/src/linux
	     make clean		     # take a few minutes
	     make dep		     # take a few minutes
	     make bzImage	     # take probably 10-20 minutes
	     make install	     # copy boot image to correct position

	f. Please make sure the boot kernel (vmlinuz) is in the
	   correct position.
	g. If you use 'lilo' utility, you should check /etc/lilo.conf
	   'image' item specified the path which is the 'vmlinuz' path,
	   or you will load wrong (or old) boot kernel image (vmlinuz).
	   After checking /etc/lilo.conf, please run "lilo".

	  Note that if the result of "make bzImage" is ERROR, then you have to
	  go back to Linux configuration Setup. Type "make menuconfig" in
          directory /usr/src/linux.


3.5.6 Make tty device and special file
--------------------------------------

    ::
       # cd /moxa/mxser/driver
       # ./msmknod

3.5.7 Make utility
------------------

    ::

	  # cd /moxa/mxser/utility
	  # make clean; make install

3.5.8 Reboot
------------



3.6 Custom configuration
========================

    Although this driver already provides you default configuration, you
    still can change the device name and major number. The instruction to
    change these parameters are shown as below.

a. Change Device name

    If you'd like to use other device names instead of default naming
    convention, all you have to do is to modify the internal code
    within the shell script "msmknod". First, you have to open "msmknod"
    by vi. Locate each line contains "ttyM" and "cum" and change them
    to the device name you desired. "msmknod" creates the device names
    you need next time executed.

b. Change Major number

    If major number 30 and 35 had been occupied, you may have to select
    2 free major numbers for this driver. There are 3 steps to change
    major numbers.

3.6.1 Find free major numbers
-----------------------------

    In /proc/devices, you may find all the major numbers occupied
    in the system. Please select 2 major numbers that are available.
    e.g. 40, 45.

3.6.2 Create special files
--------------------------

   Run /moxa/mxser/driver/msmknod to create special files with
   specified major numbers.

3.6.3 Modify driver with new major number
-----------------------------------------

   Run vi to open /moxa/mxser/driver/mxser.c. Locate the line
   contains "MXSERMAJOR". Change the content as below::

	  #define	  MXSERMAJOR		  40
	  #define	  MXSERCUMAJOR		  45

    3.6.4 Run "make clean; make install" in /moxa/mxser/driver.

3.7 Verify driver installation
==============================

    You may refer to /var/log/messages to check the latest status
    log reported by this driver whenever it's activated.

4. Utilities
^^^^^^^^^^^^

   There are 3 utilities contained in this driver. They are msdiag, msmon and
   msterm. These 3 utilities are released in form of source code. They should
   be compiled into executable file and copied into /usr/bin.

   Before using these utilities, please load driver (refer 3.4 & 3.5) and
   make sure you had run the "msmknod" utility.

msdiag - Diagnostic
===================

   This utility provides the function to display what Moxa Smartio/Industio
   board found by driver in the system.

msmon - Port Monitoring
=======================

   This utility gives the user a quick view about all the MOXA ports'
   activities. One can easily learn each port's total received/transmitted
   (Rx/Tx) character count since the time when the monitoring is started.

   Rx/Tx throughputs per second are also reported in interval basis (e.g.
   the last 5 seconds) and in average basis (since the time the monitoring
   is started). You can reset all ports' count by <HOME> key. <+> <->
   (plus/minus) keys to change the displaying time interval. Press <ENTER>
   on the port, that cursor stay, to view the port's communication
   parameters, signal status, and input/output queue.

msterm - Terminal Emulation
===========================

   This utility provides data sending and receiving ability of all tty ports,
   especially for MOXA ports. It is quite useful for testing simple
   application, for example, sending AT command to a modem connected to the
   port or used as a terminal for login purpose. Note that this is only a
   dumb terminal emulation without handling full screen operation.

5. Setserial
^^^^^^^^^^^^

   Supported Setserial parameters are listed as below.

   ============== =========================================================
   uart		  set UART type(16450-->disable FIFO, 16550A-->enable FIFO)
   close_delay	  set the amount of time(in 1/100 of a second) that DTR
		  should be kept low while being closed.
   closing_wait   set the amount of time(in 1/100 of a second) that the
		  serial port should wait for data to be drained while
		  being closed, before the receiver is disable.
   spd_hi	  Use  57.6kb  when  the application requests 38.4kb.
   spd_vhi	  Use  115.2kb	when  the application requests 38.4kb.
   spd_shi	  Use  230.4kb	when  the application requests 38.4kb.
   spd_warp	  Use  460.8kb	when  the application requests 38.4kb.
   spd_normal	  Use  38.4kb  when  the application requests 38.4kb.
   spd_cust	  Use  the custom divisor to set the speed when  the
		  application requests 38.4kb.
   divisor	  This option set the custom division.
   baud_base	  This option set the base baud rate.
   ============== =========================================================

6. Troubleshooting
^^^^^^^^^^^^^^^^^^

   The boot time error messages and solutions are stated as clearly as
   possible. If all the possible solutions fail, please contact our technical
   support team to get more help.


   Error msg:
	      More than 4 Moxa Smartio/Industio family boards found. Fifth board
              and after are ignored.

   Solution:
   To avoid this problem, please unplug fifth and after board, because Moxa
   driver supports up to 4 boards.

   Error msg:
	      Request_irq fail, IRQ(?) may be conflict with another device.

   Solution:
   Other PCI or ISA devices occupy the assigned IRQ. If you are not sure
   which device causes the situation, please check /proc/interrupts to find
   free IRQ and simply change another free IRQ for Moxa board.

   Error msg:
	      Board #: C1xx Series(CAP=xxx) interrupt number invalid.

   Solution:
   Each port within the same multiport board shares the same IRQ. Please set
   one IRQ (IRQ doesn't equal to zero) for one Moxa board.

   Error msg:
	      No interrupt vector be set for Moxa ISA board(CAP=xxx).

   Solution:
   Moxa ISA board needs an interrupt vector.Please refer to user's manual
   "Hardware Installation" chapter to set interrupt vector.

   Error msg:
              Couldn't install MOXA Smartio/Industio family driver!

   Solution:
   Load Moxa driver fail, the major number may conflict with other devices.
   Please refer to previous section 3.7 to change a free major number for
   Moxa driver.

   Error msg:
              Couldn't install MOXA Smartio/Industio family callout driver!

   Solution:
   Load Moxa callout driver fail, the callout device major number may
   conflict with other devices. Please refer to previous section 3.7 to
   change a free callout device major number for Moxa driver.
