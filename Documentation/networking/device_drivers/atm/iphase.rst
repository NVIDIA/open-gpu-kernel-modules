.. SPDX-License-Identifier: GPL-2.0

==================================
ATM (i)Chip IA Linux Driver Source
==================================

			      READ ME FISRT

--------------------------------------------------------------------------------

		     Read This Before You Begin!

--------------------------------------------------------------------------------

Description
===========

This is the README file for the Interphase PCI ATM (i)Chip IA Linux driver
source release.

The features and limitations of this driver are as follows:

    - A single VPI (VPI value of 0) is supported.
    - Supports 4K VCs for the server board (with 512K control memory) and 1K
      VCs for the client board (with 128K control memory).
    - UBR, ABR and CBR service categories are supported.
    - Only AAL5 is supported.
    - Supports setting of PCR on the VCs.
    - Multiple adapters in a system are supported.
    - All variants of Interphase ATM PCI (i)Chip adapter cards are supported,
      including x575 (OC3, control memory 128K , 512K and packet memory 128K,
      512K and 1M), x525 (UTP25) and x531 (DS3 and E3). See
      http://www.iphase.com/
      for details.
    - Only x86 platforms are supported.
    - SMP is supported.


Before You Start
================


Installation
------------

1. Installing the adapters in the system

   To install the ATM adapters in the system, follow the steps below.

       a. Login as root.
       b. Shut down the system and power off the system.
       c. Install one or more ATM adapters in the system.
       d. Connect each adapter to a port on an ATM switch. The green 'Link'
	  LED on the front panel of the adapter will be on if the adapter is
	  connected to the switch properly when the system is powered up.
       e. Power on and boot the system.

2. [ Removed ]

3. Rebuild kernel with ABR support

   [ a. and b. removed ]

    c. Reconfigure the kernel, choose the Interphase ia driver through "make
       menuconfig" or "make xconfig".
    d. Rebuild the kernel, loadable modules and the atm tools.
    e. Install the new built kernel and modules and reboot.

4. Load the adapter hardware driver (ia driver) if it is built as a module

       a. Login as root.
       b. Change directory to /lib/modules/<kernel-version>/atm.
       c. Run "insmod suni.o;insmod iphase.o"
	  The yellow 'status' LED on the front panel of the adapter will blink
	  while the driver is loaded in the system.
       d. To verify that the 'ia' driver is loaded successfully, run the
	  following command::

	      cat /proc/atm/devices

	  If the driver is loaded successfully, the output of the command will
	  be similar to the following lines::

	      Itf Type    ESI/"MAC"addr AAL(TX,err,RX,err,drop) ...
	      0   ia      xxxxxxxxx  0 ( 0 0 0 0 0 )  5 ( 0 0 0 0 0 )

	  You can also check the system log file /var/log/messages for messages
	  related to the ATM driver.

5. Ia Driver Configuration

5.1 Configuration of adapter buffers
    The (i)Chip boards have 3 different packet RAM size variants: 128K, 512K and
    1M. The RAM size decides the number of buffers and buffer size. The default
    size and number of buffers are set as following:

	=========  =======  ======   ======   ======   ======   ======
	 Total     Rx RAM   Tx RAM   Rx Buf   Tx Buf   Rx buf   Tx buf
	 RAM size  size     size     size     size     cnt      cnt
	=========  =======  ======   ======   ======   ======   ======
	   128K      64K      64K      10K      10K       6        6
	   512K     256K     256K      10K      10K      25       25
	     1M     512K     512K      10K      10K      51       51
	=========  =======  ======   ======   ======   ======   ======

       These setting should work well in most environments, but can be
       changed by typing the following command::

	   insmod <IA_DIR>/ia.o IA_RX_BUF=<RX_CNT> IA_RX_BUF_SZ=<RX_SIZE> \
		   IA_TX_BUF=<TX_CNT> IA_TX_BUF_SZ=<TX_SIZE>

       Where:

	    - RX_CNT = number of receive buffers in the range (1-128)
	    - RX_SIZE = size of receive buffers in the range (48-64K)
	    - TX_CNT = number of transmit buffers in the range (1-128)
	    - TX_SIZE = size of transmit buffers in the range (48-64K)

	    1. Transmit and receive buffer size must be a multiple of 4.
	    2. Care should be taken so that the memory required for the
	       transmit and receive buffers is less than or equal to the
	       total adapter packet memory.

5.2 Turn on ia debug trace

    When the ia driver is built with the CONFIG_ATM_IA_DEBUG flag, the driver
    can provide more debug trace if needed. There is a bit mask variable,
    IADebugFlag, which controls the output of the traces. You can find the bit
    map of the IADebugFlag in iphase.h.
    The debug trace can be turn on through the insmod command line option, for
    example, "insmod iphase.o IADebugFlag=0xffffffff" can turn on all the debug
    traces together with loading the driver.

6. Ia Driver Test Using ttcp_atm and PVC

   For the PVC setup, the test machines can either be connected back-to-back or
   through a switch. If connected through the switch, the switch must be
   configured for the PVC(s).

   a. For UBR test:

      At the test machine intended to receive data, type::

	 ttcp_atm -r -a -s 0.100

      At the other test machine, type::

	 ttcp_atm -t -a -s 0.100 -n 10000

      Run "ttcp_atm -h" to display more options of the ttcp_atm tool.
   b. For ABR test:

      It is the same as the UBR testing, but with an extra command option::

	 -Pabr:max_pcr=<xxx>

      where:

	     xxx = the maximum peak cell rate, from 170 - 353207.

      This option must be set on both the machines.

   c. For CBR test:

      It is the same as the UBR testing, but with an extra command option::

	 -Pcbr:max_pcr=<xxx>

      where:

	     xxx = the maximum peak cell rate, from 170 - 353207.

      This option may only be set on the transmit machine.


Outstanding Issues
==================



Contact Information
-------------------

::

     Customer Support:
	 United States:	Telephone:	(214) 654-5555
			Fax:		(214) 654-5500
			E-Mail:		intouch@iphase.com
	 Europe:	Telephone:	33 (0)1 41 15 44 00
			Fax:		33 (0)1 41 15 12 13
     World Wide Web:	http://www.iphase.com
     Anonymous FTP:	ftp.iphase.com
