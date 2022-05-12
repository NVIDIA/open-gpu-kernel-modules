Kernel driver ftsteutates
=========================

Supported chips:

  * FTS Teutates

    Prefix: 'ftsteutates'

    Addresses scanned: I2C 0x73 (7-Bit)

Author: Thilo Cestonaro <thilo.cestonaro@ts.fujitsu.com>


Description
-----------

The BMC Teutates is the Eleventh generation of Superior System
monitoring and thermal management solution. It is builds on the basic
functionality of the BMC Theseus and contains several new features and
enhancements. It can monitor up to 4 voltages, 16 temperatures and
8 fans. It also contains an integrated watchdog which is currently
implemented in this driver.

To clear a temperature or fan alarm, execute the following command with the
correct path to the alarm file::

	echo 0 >XXXX_alarm

Specification of the chip can be found here:

- ftp://ftp.ts.fujitsu.com/pub/Mainboard-OEM-Sales/Services/Software&Tools/Linux_SystemMonitoring&Watchdog&GPIO/BMC-Teutates_Specification_V1.21.pdf
- ftp://ftp.ts.fujitsu.com/pub/Mainboard-OEM-Sales/Services/Software&Tools/Linux_SystemMonitoring&Watchdog&GPIO/Fujitsu_mainboards-1-Sensors_HowTo-en-US.pdf
