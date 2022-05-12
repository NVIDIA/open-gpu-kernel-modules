.. SPDX-License-Identifier: GPL-2.0

======================
SCSI Kernel Parameters
======================

See Documentation/admin-guide/kernel-parameters.rst for general information on
specifying module parameters.

This document may not be entirely up to date and comprehensive. The command
``modinfo -p ${modulename}`` shows a current list of all parameters of a loadable
module. Loadable modules, after being loaded into the running kernel, also
reveal their parameters in /sys/module/${modulename}/parameters/. Some of these
parameters may be changed at runtime by the command
``echo -n ${value} > /sys/module/${modulename}/parameters/${parm}``.

::

	advansys=	[HW,SCSI]
			See header of drivers/scsi/advansys.c.

	aha152x=	[HW,SCSI]
			See Documentation/scsi/aha152x.rst.

	aha1542=	[HW,SCSI]
			Format: <portbase>[,<buson>,<busoff>[,<dmaspeed>]]

	aic7xxx=	[HW,SCSI]
			See Documentation/scsi/aic7xxx.rst.

	aic79xx=	[HW,SCSI]
			See Documentation/scsi/aic79xx.rst.

	atascsi=	[HW,SCSI]
			See drivers/scsi/atari_scsi.c.

	BusLogic=	[HW,SCSI]
			See drivers/scsi/BusLogic.c, comment before function
			BusLogic_ParseDriverOptions().

	gvp11=		[HW,SCSI]

	ips=		[HW,SCSI] Adaptec / IBM ServeRAID controller
			See header of drivers/scsi/ips.c.

	mac5380=	[HW,SCSI]
			See drivers/scsi/mac_scsi.c.

	scsi_mod.max_luns=
			[SCSI] Maximum number of LUNs to probe.
			Should be between 1 and 2^32-1.

	scsi_mod.max_report_luns=
			[SCSI] Maximum number of LUNs received.
			Should be between 1 and 16384.

	NCR_D700=	[HW,SCSI]
			See header of drivers/scsi/NCR_D700.c.

	ncr5380=	[HW,SCSI]
			See Documentation/scsi/g_NCR5380.rst.

	ncr53c400=	[HW,SCSI]
			See Documentation/scsi/g_NCR5380.rst.

	ncr53c400a=	[HW,SCSI]
			See Documentation/scsi/g_NCR5380.rst.

	ncr53c8xx=	[HW,SCSI]

	osst=		[HW,SCSI] SCSI Tape Driver
			Format: <buffer_size>,<write_threshold>
			See also Documentation/scsi/st.rst.

	scsi_debug_*=	[SCSI]
			See drivers/scsi/scsi_debug.c.

	scsi_mod.default_dev_flags=
			[SCSI] SCSI default device flags
			Format: <integer>

	scsi_mod.dev_flags=
			[SCSI] Black/white list entry for vendor and model
			Format: <vendor>:<model>:<flags>
			(flags are integer value)

	scsi_mod.scsi_logging_level=
			[SCSI] a bit mask of logging levels
			See drivers/scsi/scsi_logging.h for bits.  Also
			settable via sysctl at dev.scsi.logging_level
			(/proc/sys/dev/scsi/logging_level).
			There is also a nice 'scsi_logging_level' script in the
			S390-tools package, available for download at
			https://github.com/ibm-s390-linux/s390-tools/blob/master/scripts/scsi_logging_level

	scsi_mod.scan=	[SCSI] sync (default) scans SCSI busses as they are
			discovered.  async scans them in kernel threads,
			allowing boot to proceed.  none ignores them, expecting
			user space to do the scan.

	sim710=		[SCSI,HW]
			See header of drivers/scsi/sim710.c.

	st=		[HW,SCSI] SCSI tape parameters (buffers, etc.)
			See Documentation/scsi/st.rst.

	wd33c93=	[HW,SCSI]
			See header of drivers/scsi/wd33c93.c.
