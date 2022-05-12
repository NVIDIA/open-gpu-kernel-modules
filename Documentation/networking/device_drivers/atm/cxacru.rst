.. SPDX-License-Identifier: GPL-2.0

========================
ATM cxacru device driver
========================

Firmware is required for this device: http://accessrunner.sourceforge.net/

While it is capable of managing/maintaining the ADSL connection without the
module loaded, the device will sometimes stop responding after unloading the
driver and it is necessary to unplug/remove power to the device to fix this.

Note: support for cxacru-cf.bin has been removed. It was not loaded correctly
so it had no effect on the device configuration. Fixing it could have stopped
existing devices working when an invalid configuration is supplied.

There is a script cxacru-cf.py to convert an existing file to the sysfs form.

Detected devices will appear as ATM devices named "cxacru". In /sys/class/atm/
these are directories named cxacruN where N is the device number. A symlink
named device points to the USB interface device's directory which contains
several sysfs attribute files for retrieving device statistics:

* adsl_controller_version

* adsl_headend
* adsl_headend_environment

	- Information about the remote headend.

* adsl_config

	- Configuration writing interface.
	- Write parameters in hexadecimal format <index>=<value>,
	  separated by whitespace, e.g.:

		"1=0 a=5"

	- Up to 7 parameters at a time will be sent and the modem will restart
	  the ADSL connection when any value is set. These are logged for future
	  reference.

* downstream_attenuation (dB)
* downstream_bits_per_frame
* downstream_rate (kbps)
* downstream_snr_margin (dB)

	- Downstream stats.

* upstream_attenuation (dB)
* upstream_bits_per_frame
* upstream_rate (kbps)
* upstream_snr_margin (dB)
* transmitter_power (dBm/Hz)

	- Upstream stats.

* downstream_crc_errors
* downstream_fec_errors
* downstream_hec_errors
* upstream_crc_errors
* upstream_fec_errors
* upstream_hec_errors

	- Error counts.

* line_startable

	- Indicates that ADSL support on the device
	  is/can be enabled, see adsl_start.

* line_status

	 - "initialising"
	 - "down"
	 - "attempting to activate"
	 - "training"
	 - "channel analysis"
	 - "exchange"
	 - "waiting"
	 - "up"

	Changes between "down" and "attempting to activate"
	if there is no signal.

* link_status

	 - "not connected"
	 - "connected"
	 - "lost"

* mac_address

* modulation

	 - "" (when not connected)
	 - "ANSI T1.413"
	 - "ITU-T G.992.1 (G.DMT)"
	 - "ITU-T G.992.2 (G.LITE)"

* startup_attempts

	- Count of total attempts to initialise ADSL.

To enable/disable ADSL, the following can be written to the adsl_state file:

	 - "start"
	 - "stop
	 - "restart" (stops, waits 1.5s, then starts)
	 - "poll" (used to resume status polling if it was disabled due to failure)

Changes in adsl/line state are reported via kernel log messages::

	[4942145.150704] ATM dev 0: ADSL state: running
	[4942243.663766] ATM dev 0: ADSL line: down
	[4942249.665075] ATM dev 0: ADSL line: attempting to activate
	[4942253.654954] ATM dev 0: ADSL line: training
	[4942255.666387] ATM dev 0: ADSL line: channel analysis
	[4942259.656262] ATM dev 0: ADSL line: exchange
	[2635357.696901] ATM dev 0: ADSL line: up (8128 kb/s down | 832 kb/s up)
