===============================================
How to conserve battery power using laptop-mode
===============================================

Document Author: Bart Samwel (bart@samwel.tk)

Date created: January 2, 2004

Last modified: December 06, 2004

Introduction
------------

Laptop mode is used to minimize the time that the hard disk needs to be spun up,
to conserve battery power on laptops. It has been reported to cause significant
power savings.

.. Contents

   * Introduction
   * Installation
   * Caveats
   * The Details
   * Tips & Tricks
   * Control script
   * ACPI integration
   * Monitoring tool


Installation
------------

To use laptop mode, you don't need to set any kernel configuration options
or anything. Simply install all the files included in this document, and
laptop mode will automatically be started when you're on battery. For
your convenience, a tarball containing an installer can be downloaded at:

	http://www.samwel.tk/laptop_mode/laptop_mode/

To configure laptop mode, you need to edit the configuration file, which is
located in /etc/default/laptop-mode on Debian-based systems, or in
/etc/sysconfig/laptop-mode on other systems.

Unfortunately, automatic enabling of laptop mode does not work for
laptops that don't have ACPI. On those laptops, you need to start laptop
mode manually. To start laptop mode, run "laptop_mode start", and to
stop it, run "laptop_mode stop". (Note: The laptop mode tools package now
has experimental support for APM, you might want to try that first.)


Caveats
-------

* The downside of laptop mode is that you have a chance of losing up to 10
  minutes of work. If you cannot afford this, don't use it! The supplied ACPI
  scripts automatically turn off laptop mode when the battery almost runs out,
  so that you won't lose any data at the end of your battery life.

* Most desktop hard drives have a very limited lifetime measured in spindown
  cycles, typically about 50.000 times (it's usually listed on the spec sheet).
  Check your drive's rating, and don't wear down your drive's lifetime if you
  don't need to.

* If you mount some of your ext3/reiserfs filesystems with the -n option, then
  the control script will not be able to remount them correctly. You must set
  DO_REMOUNTS=0 in the control script, otherwise it will remount them with the
  wrong options -- or it will fail because it cannot write to /etc/mtab.

* If you have your filesystems listed as type "auto" in fstab, like I did, then
  the control script will not recognize them as filesystems that need remounting.
  You must list the filesystems with their true type instead.

* It has been reported that some versions of the mutt mail client use file access
  times to determine whether a folder contains new mail. If you use mutt and
  experience this, you must disable the noatime remounting by setting the option
  DO_REMOUNT_NOATIME to 0 in the configuration file.


The Details
-----------

Laptop mode is controlled by the knob /proc/sys/vm/laptop_mode. This knob is
present for all kernels that have the laptop mode patch, regardless of any
configuration options. When the knob is set, any physical disk I/O (that might
have caused the hard disk to spin up) causes Linux to flush all dirty blocks. The
result of this is that after a disk has spun down, it will not be spun up
anymore to write dirty blocks, because those blocks had already been written
immediately after the most recent read operation. The value of the laptop_mode
knob determines the time between the occurrence of disk I/O and when the flush
is triggered. A sensible value for the knob is 5 seconds. Setting the knob to
0 disables laptop mode.

To increase the effectiveness of the laptop_mode strategy, the laptop_mode
control script increases dirty_expire_centisecs and dirty_writeback_centisecs in
/proc/sys/vm to about 10 minutes (by default), which means that pages that are
dirtied are not forced to be written to disk as often. The control script also
changes the dirty background ratio, so that background writeback of dirty pages
is not done anymore. Combined with a higher commit value (also 10 minutes) for
ext3 or ReiserFS filesystems (also done automatically by the control script),
this results in concentration of disk activity in a small time interval which
occurs only once every 10 minutes, or whenever the disk is forced to spin up by
a cache miss. The disk can then be spun down in the periods of inactivity.

If you want to find out which process caused the disk to spin up, you can
gather information by setting the flag /proc/sys/vm/block_dump. When this flag
is set, Linux reports all disk read and write operations that take place, and
all block dirtyings done to files. This makes it possible to debug why a disk
needs to spin up, and to increase battery life even more. The output of
block_dump is written to the kernel output, and it can be retrieved using
"dmesg". When you use block_dump and your kernel logging level also includes
kernel debugging messages, you probably want to turn off klogd, otherwise
the output of block_dump will be logged, causing disk activity that is not
normally there.


Configuration
-------------

The laptop mode configuration file is located in /etc/default/laptop-mode on
Debian-based systems, or in /etc/sysconfig/laptop-mode on other systems. It
contains the following options:

MAX_AGE:

Maximum time, in seconds, of hard drive spindown time that you are
comfortable with. Worst case, it's possible that you could lose this
amount of work if your battery fails while you're in laptop mode.

MINIMUM_BATTERY_MINUTES:

Automatically disable laptop mode if the remaining number of minutes of
battery power is less than this value. Default is 10 minutes.

AC_HD/BATT_HD:

The idle timeout that should be set on your hard drive when laptop mode
is active (BATT_HD) and when it is not active (AC_HD). The defaults are
20 seconds (value 4) for BATT_HD  and 2 hours (value 244) for AC_HD. The
possible values are those listed in the manual page for "hdparm" for the
"-S" option.

HD:

The devices for which the spindown timeout should be adjusted by laptop mode.
Default is /dev/hda. If you specify multiple devices, separate them by a space.

READAHEAD:

Disk readahead, in 512-byte sectors, while laptop mode is active. A large
readahead can prevent disk accesses for things like executable pages (which are
loaded on demand while the application executes) and sequentially accessed data
(MP3s).

DO_REMOUNTS:

The control script automatically remounts any mounted journaled filesystems
with appropriate commit interval options. When this option is set to 0, this
feature is disabled.

DO_REMOUNT_NOATIME:

When remounting, should the filesystems be remounted with the noatime option?
Normally, this is set to "1" (enabled), but there may be programs that require
access time recording.

DIRTY_RATIO:

The percentage of memory that is allowed to contain "dirty" or unsaved data
before a writeback is forced, while laptop mode is active. Corresponds to
the /proc/sys/vm/dirty_ratio sysctl.

DIRTY_BACKGROUND_RATIO:

The percentage of memory that is allowed to contain "dirty" or unsaved data
after a forced writeback is done due to an exceeding of DIRTY_RATIO. Set
this nice and low. This corresponds to the /proc/sys/vm/dirty_background_ratio
sysctl.

Note that the behaviour of dirty_background_ratio is quite different
when laptop mode is active and when it isn't. When laptop mode is inactive,
dirty_background_ratio is the threshold percentage at which background writeouts
start taking place. When laptop mode is active, however, background writeouts
are disabled, and the dirty_background_ratio only determines how much writeback
is done when dirty_ratio is reached.

DO_CPU:

Enable CPU frequency scaling when in laptop mode. (Requires CPUFreq to be setup.
See Documentation/admin-guide/pm/cpufreq.rst for more info. Disabled by default.)

CPU_MAXFREQ:

When on battery, what is the maximum CPU speed that the system should use? Legal
values are "slowest" for the slowest speed that your CPU is able to operate at,
or a value listed in /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies.


Tips & Tricks
-------------

* Bartek Kania reports getting up to 50 minutes of extra battery life (on top
  of his regular 3 to 3.5 hours) using a spindown time of 5 seconds (BATT_HD=1).

* You can spin down the disk while playing MP3, by setting disk readahead
  to 8MB (READAHEAD=16384). Effectively, the disk will read a complete MP3 at
  once, and will then spin down while the MP3 is playing. (Thanks to Bartek
  Kania.)

* Drew Scott Daniels observed: "I don't know why, but when I decrease the number
  of colours that my display uses it consumes less battery power. I've seen
  this on powerbooks too. I hope that this is a piece of information that
  might be useful to the Laptop Mode patch or its users."

* In syslog.conf, you can prefix entries with a dash `-` to omit syncing the
  file after every logging. When you're using laptop-mode and your disk doesn't
  spin down, this is a likely culprit.

* Richard Atterer observed that laptop mode does not work well with noflushd
  (http://noflushd.sourceforge.net/), it seems that noflushd prevents laptop-mode
  from doing its thing.

* If you're worried about your data, you might want to consider using a USB
  memory stick or something like that as a "working area". (Be aware though
  that flash memory can only handle a limited number of writes, and overuse
  may wear out your memory stick pretty quickly. Do _not_ use journalling
  filesystems on flash memory sticks.)


Configuration file for control and ACPI battery scripts
-------------------------------------------------------

This allows the tunables to be changed for the scripts via an external
configuration file

It should be installed as /etc/default/laptop-mode on Debian, and as
/etc/sysconfig/laptop-mode on Red Hat, SUSE, Mandrake, and other work-alikes.

Config file::

  # Maximum time, in seconds, of hard drive spindown time that you are
  # comfortable with. Worst case, it's possible that you could lose this
  # amount of work if your battery fails you while in laptop mode.
  #MAX_AGE=600

  # Automatically disable laptop mode when the number of minutes of battery
  # that you have left goes below this threshold.
  MINIMUM_BATTERY_MINUTES=10

  # Read-ahead, in 512-byte sectors. You can spin down the disk while playing MP3/OGG
  # by setting the disk readahead to 8MB (READAHEAD=16384). Effectively, the disk
  # will read a complete MP3 at once, and will then spin down while the MP3/OGG is
  # playing.
  #READAHEAD=4096

  # Shall we remount journaled fs. with appropriate commit interval? (1=yes)
  #DO_REMOUNTS=1

  # And shall we add the "noatime" option to that as well? (1=yes)
  #DO_REMOUNT_NOATIME=1

  # Dirty synchronous ratio.  At this percentage of dirty pages the process
  # which
  # calls write() does its own writeback
  #DIRTY_RATIO=40

  #
  # Allowed dirty background ratio, in percent.  Once DIRTY_RATIO has been
  # exceeded, the kernel will wake flusher threads which will then reduce the
  # amount of dirty memory to dirty_background_ratio.  Set this nice and low,
  # so once some writeout has commenced, we do a lot of it.
  #
  #DIRTY_BACKGROUND_RATIO=5

  # kernel default dirty buffer age
  #DEF_AGE=30
  #DEF_UPDATE=5
  #DEF_DIRTY_BACKGROUND_RATIO=10
  #DEF_DIRTY_RATIO=40
  #DEF_XFS_AGE_BUFFER=15
  #DEF_XFS_SYNC_INTERVAL=30
  #DEF_XFS_BUFD_INTERVAL=1

  # This must be adjusted manually to the value of HZ in the running kernel
  # on 2.4, until the XFS people change their 2.4 external interfaces to work in
  # centisecs. This can be automated, but it's a work in progress that still
  # needs# some fixes. On 2.6 kernels, XFS uses USER_HZ instead of HZ for
  # external interfaces, and that is currently always set to 100. So you don't
  # need to change this on 2.6.
  #XFS_HZ=100

  # Should the maximum CPU frequency be adjusted down while on battery?
  # Requires CPUFreq to be setup.
  # See Documentation/admin-guide/pm/cpufreq.rst for more info
  #DO_CPU=0

  # When on battery what is the maximum CPU speed that the system should
  # use? Legal values are "slowest" for the slowest speed that your
  # CPU is able to operate at, or a value listed in:
  # /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies
  # Only applicable if DO_CPU=1.
  #CPU_MAXFREQ=slowest

  # Idle timeout for your hard drive (man hdparm for valid values, -S option)
  # Default is 2 hours on AC (AC_HD=244) and 20 seconds for battery (BATT_HD=4).
  #AC_HD=244
  #BATT_HD=4

  # The drives for which to adjust the idle timeout. Separate them by a space,
  # e.g. HD="/dev/hda /dev/hdb".
  #HD="/dev/hda"

  # Set the spindown timeout on a hard drive?
  #DO_HD=1


Control script
--------------

Please note that this control script works for the Linux 2.4 and 2.6 series (thanks
to Kiko Piris).

Control script::

  #!/bin/bash

  # start or stop laptop_mode, best run by a power management daemon when
  # ac gets connected/disconnected from a laptop
  #
  # install as /sbin/laptop_mode
  #
  # Contributors to this script:   Kiko Piris
  #				 Bart Samwel
  #				 Micha Feigin
  #				 Andrew Morton
  #				 Herve Eychenne
  #				 Dax Kelson
  #
  # Original Linux 2.4 version by: Jens Axboe

  #############################################################################

  # Source config
  if [ -f /etc/default/laptop-mode ] ; then
	# Debian
	. /etc/default/laptop-mode
  elif [ -f /etc/sysconfig/laptop-mode ] ; then
	# Others
          . /etc/sysconfig/laptop-mode
  fi

  # Don't raise an error if the config file is incomplete
  # set defaults instead:

  # Maximum time, in seconds, of hard drive spindown time that you are
  # comfortable with. Worst case, it's possible that you could lose this
  # amount of work if your battery fails you while in laptop mode.
  MAX_AGE=${MAX_AGE:-'600'}

  # Read-ahead, in kilobytes
  READAHEAD=${READAHEAD:-'4096'}

  # Shall we remount journaled fs. with appropriate commit interval? (1=yes)
  DO_REMOUNTS=${DO_REMOUNTS:-'1'}

  # And shall we add the "noatime" option to that as well? (1=yes)
  DO_REMOUNT_NOATIME=${DO_REMOUNT_NOATIME:-'1'}

  # Shall we adjust the idle timeout on a hard drive?
  DO_HD=${DO_HD:-'1'}

  # Adjust idle timeout on which hard drive?
  HD="${HD:-'/dev/hda'}"

  # spindown time for HD (hdparm -S values)
  AC_HD=${AC_HD:-'244'}
  BATT_HD=${BATT_HD:-'4'}

  # Dirty synchronous ratio.  At this percentage of dirty pages the process which
  # calls write() does its own writeback
  DIRTY_RATIO=${DIRTY_RATIO:-'40'}

  # cpu frequency scaling
  # See Documentation/admin-guide/pm/cpufreq.rst for more info
  DO_CPU=${CPU_MANAGE:-'0'}
  CPU_MAXFREQ=${CPU_MAXFREQ:-'slowest'}

  #
  # Allowed dirty background ratio, in percent.  Once DIRTY_RATIO has been
  # exceeded, the kernel will wake flusher threads which will then reduce the
  # amount of dirty memory to dirty_background_ratio.  Set this nice and low,
  # so once some writeout has commenced, we do a lot of it.
  #
  DIRTY_BACKGROUND_RATIO=${DIRTY_BACKGROUND_RATIO:-'5'}

  # kernel default dirty buffer age
  DEF_AGE=${DEF_AGE:-'30'}
  DEF_UPDATE=${DEF_UPDATE:-'5'}
  DEF_DIRTY_BACKGROUND_RATIO=${DEF_DIRTY_BACKGROUND_RATIO:-'10'}
  DEF_DIRTY_RATIO=${DEF_DIRTY_RATIO:-'40'}
  DEF_XFS_AGE_BUFFER=${DEF_XFS_AGE_BUFFER:-'15'}
  DEF_XFS_SYNC_INTERVAL=${DEF_XFS_SYNC_INTERVAL:-'30'}
  DEF_XFS_BUFD_INTERVAL=${DEF_XFS_BUFD_INTERVAL:-'1'}

  # This must be adjusted manually to the value of HZ in the running kernel
  # on 2.4, until the XFS people change their 2.4 external interfaces to work in
  # centisecs. This can be automated, but it's a work in progress that still needs
  # some fixes. On 2.6 kernels, XFS uses USER_HZ instead of HZ for external
  # interfaces, and that is currently always set to 100. So you don't need to
  # change this on 2.6.
  XFS_HZ=${XFS_HZ:-'100'}

  #############################################################################

  KLEVEL="$(uname -r |
               {
	       IFS='.' read a b c
	       echo $a.$b
	     }
  )"
  case "$KLEVEL" in
	"2.4"|"2.6")
		;;
	*)
		echo "Unhandled kernel version: $KLEVEL ('uname -r' = '$(uname -r)')" >&2
		exit 1
		;;
  esac

  if [ ! -e /proc/sys/vm/laptop_mode ] ; then
	echo "Kernel is not patched with laptop_mode patch." >&2
	exit 1
  fi

  if [ ! -w /proc/sys/vm/laptop_mode ] ; then
	echo "You do not have enough privileges to enable laptop_mode." >&2
	exit 1
  fi

  # Remove an option (the first parameter) of the form option=<number> from
  # a mount options string (the rest of the parameters).
  parse_mount_opts () {
	OPT="$1"
	shift
	echo ",$*," | sed		\
	 -e 's/,'"$OPT"'=[0-9]*,/,/g'	\
	 -e 's/,,*/,/g'			\
	 -e 's/^,//'			\
	 -e 's/,$//'
  }

  # Remove an option (the first parameter) without any arguments from
  # a mount option string (the rest of the parameters).
  parse_nonumber_mount_opts () {
	OPT="$1"
	shift
	echo ",$*," | sed		\
	 -e 's/,'"$OPT"',/,/g'		\
	 -e 's/,,*/,/g'			\
	 -e 's/^,//'			\
	 -e 's/,$//'
  }

  # Find out the state of a yes/no option (e.g. "atime"/"noatime") in
  # fstab for a given filesystem, and use this state to replace the
  # value of the option in another mount options string. The device
  # is the first argument, the option name the second, and the default
  # value the third. The remainder is the mount options string.
  #
  # Example:
  # parse_yesno_opts_wfstab /dev/hda1 atime atime defaults,noatime
  #
  # If fstab contains, say, "rw" for this filesystem, then the result
  # will be "defaults,atime".
  parse_yesno_opts_wfstab () {
	L_DEV="$1"
	OPT="$2"
	DEF_OPT="$3"
	shift 3
	L_OPTS="$*"
	PARSEDOPTS1="$(parse_nonumber_mount_opts $OPT $L_OPTS)"
	PARSEDOPTS1="$(parse_nonumber_mount_opts no$OPT $PARSEDOPTS1)"
	# Watch for a default atime in fstab
	FSTAB_OPTS="$(awk '$1 == "'$L_DEV'" { print $4 }' /etc/fstab)"
	if echo "$FSTAB_OPTS" | grep "$OPT" > /dev/null ; then
		# option specified in fstab: extract the value and use it
		if echo "$FSTAB_OPTS" | grep "no$OPT" > /dev/null ; then
			echo "$PARSEDOPTS1,no$OPT"
		else
			# no$OPT not found -- so we must have $OPT.
			echo "$PARSEDOPTS1,$OPT"
		fi
	else
		# option not specified in fstab -- choose the default.
		echo "$PARSEDOPTS1,$DEF_OPT"
	fi
  }

  # Find out the state of a numbered option (e.g. "commit=NNN") in
  # fstab for a given filesystem, and use this state to replace the
  # value of the option in another mount options string. The device
  # is the first argument, and the option name the second. The
  # remainder is the mount options string in which the replacement
  # must be done.
  #
  # Example:
  # parse_mount_opts_wfstab /dev/hda1 commit defaults,commit=7
  #
  # If fstab contains, say, "commit=3,rw" for this filesystem, then the
  # result will be "rw,commit=3".
  parse_mount_opts_wfstab () {
	L_DEV="$1"
	OPT="$2"
	shift 2
	L_OPTS="$*"
	PARSEDOPTS1="$(parse_mount_opts $OPT $L_OPTS)"
	# Watch for a default commit in fstab
	FSTAB_OPTS="$(awk '$1 == "'$L_DEV'" { print $4 }' /etc/fstab)"
	if echo "$FSTAB_OPTS" | grep "$OPT=" > /dev/null ; then
		# option specified in fstab: extract the value, and use it
		echo -n "$PARSEDOPTS1,$OPT="
		echo ",$FSTAB_OPTS," | sed \
		 -e 's/.*,'"$OPT"'=//'	\
		 -e 's/,.*//'
	else
		# option not specified in fstab: set it to 0
		echo "$PARSEDOPTS1,$OPT=0"
	fi
  }

  deduce_fstype () {
	MP="$1"
	# My root filesystem unfortunately has
	# type "unknown" in /etc/mtab. If we encounter
	# "unknown", we try to get the type from fstab.
	cat /etc/fstab |
	grep -v '^#' |
	while read FSTAB_DEV FSTAB_MP FSTAB_FST FSTAB_OPTS FSTAB_DUMP FSTAB_DUMP ; do
		if [ "$FSTAB_MP" = "$MP" ]; then
			echo $FSTAB_FST
			exit 0
		fi
	done
  }

  if [ $DO_REMOUNT_NOATIME -eq 1 ] ; then
	NOATIME_OPT=",noatime"
  fi

  case "$1" in
	start)
		AGE=$((100*$MAX_AGE))
		XFS_AGE=$(($XFS_HZ*$MAX_AGE))
		echo -n "Starting laptop_mode"

		if [ -d /proc/sys/vm/pagebuf ] ; then
			# (For 2.4 and early 2.6.)
			# This only needs to be set, not reset -- it is only used when
			# laptop mode is enabled.
			echo $XFS_AGE > /proc/sys/vm/pagebuf/lm_flush_age
			echo $XFS_AGE > /proc/sys/fs/xfs/lm_sync_interval
		elif [ -f /proc/sys/fs/xfs/lm_age_buffer ] ; then
			# (A couple of early 2.6 laptop mode patches had these.)
			# The same goes for these.
			echo $XFS_AGE > /proc/sys/fs/xfs/lm_age_buffer
			echo $XFS_AGE > /proc/sys/fs/xfs/lm_sync_interval
		elif [ -f /proc/sys/fs/xfs/age_buffer ] ; then
			# (2.6.6)
			# But not for these -- they are also used in normal
			# operation.
			echo $XFS_AGE > /proc/sys/fs/xfs/age_buffer
			echo $XFS_AGE > /proc/sys/fs/xfs/sync_interval
		elif [ -f /proc/sys/fs/xfs/age_buffer_centisecs ] ; then
			# (2.6.7 upwards)
			# And not for these either. These are in centisecs,
			# not USER_HZ, so we have to use $AGE, not $XFS_AGE.
			echo $AGE > /proc/sys/fs/xfs/age_buffer_centisecs
			echo $AGE > /proc/sys/fs/xfs/xfssyncd_centisecs
			echo 3000 > /proc/sys/fs/xfs/xfsbufd_centisecs
		fi

		case "$KLEVEL" in
			"2.4")
				echo 1					> /proc/sys/vm/laptop_mode
				echo "30 500 0 0 $AGE $AGE 60 20 0"	> /proc/sys/vm/bdflush
				;;
			"2.6")
				echo 5					> /proc/sys/vm/laptop_mode
				echo "$AGE"				> /proc/sys/vm/dirty_writeback_centisecs
				echo "$AGE"				> /proc/sys/vm/dirty_expire_centisecs
				echo "$DIRTY_RATIO"			> /proc/sys/vm/dirty_ratio
				echo "$DIRTY_BACKGROUND_RATIO"		> /proc/sys/vm/dirty_background_ratio
				;;
		esac
		if [ $DO_REMOUNTS -eq 1 ]; then
			cat /etc/mtab | while read DEV MP FST OPTS DUMP PASS ; do
				PARSEDOPTS="$(parse_mount_opts "$OPTS")"
				if [ "$FST" = 'unknown' ]; then
					FST=$(deduce_fstype $MP)
				fi
				case "$FST" in
					"ext3"|"reiserfs")
						PARSEDOPTS="$(parse_mount_opts commit "$OPTS")"
						mount $DEV -t $FST $MP -o remount,$PARSEDOPTS,commit=$MAX_AGE$NOATIME_OPT
						;;
					"xfs")
						mount $DEV -t $FST $MP -o remount,$OPTS$NOATIME_OPT
						;;
				esac
				if [ -b $DEV ] ; then
					blockdev --setra $(($READAHEAD * 2)) $DEV
				fi
			done
		fi
		if [ $DO_HD -eq 1 ] ; then
			for THISHD in $HD ; do
				/sbin/hdparm -S $BATT_HD $THISHD > /dev/null 2>&1
				/sbin/hdparm -B 1 $THISHD > /dev/null 2>&1
			done
		fi
		if [ $DO_CPU -eq 1 -a -e /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq ]; then
			if [ $CPU_MAXFREQ = 'slowest' ]; then
				CPU_MAXFREQ=`cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq`
			fi
			echo $CPU_MAXFREQ > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
		fi
		echo "."
		;;
	stop)
		U_AGE=$((100*$DEF_UPDATE))
		B_AGE=$((100*$DEF_AGE))
		echo -n "Stopping laptop_mode"
		echo 0 > /proc/sys/vm/laptop_mode
		if [ -f /proc/sys/fs/xfs/age_buffer -a ! -f /proc/sys/fs/xfs/lm_age_buffer ] ; then
			# These need to be restored, if there are no lm_*.
			echo $(($XFS_HZ*$DEF_XFS_AGE_BUFFER))	 	> /proc/sys/fs/xfs/age_buffer
			echo $(($XFS_HZ*$DEF_XFS_SYNC_INTERVAL)) 	> /proc/sys/fs/xfs/sync_interval
		elif [ -f /proc/sys/fs/xfs/age_buffer_centisecs ] ; then
			# These need to be restored as well.
			echo $((100*$DEF_XFS_AGE_BUFFER))	> /proc/sys/fs/xfs/age_buffer_centisecs
			echo $((100*$DEF_XFS_SYNC_INTERVAL))	> /proc/sys/fs/xfs/xfssyncd_centisecs
			echo $((100*$DEF_XFS_BUFD_INTERVAL))	> /proc/sys/fs/xfs/xfsbufd_centisecs
		fi
		case "$KLEVEL" in
			"2.4")
				echo "30 500 0 0 $U_AGE $B_AGE 60 20 0"	> /proc/sys/vm/bdflush
				;;
			"2.6")
				echo "$U_AGE"				> /proc/sys/vm/dirty_writeback_centisecs
				echo "$B_AGE"				> /proc/sys/vm/dirty_expire_centisecs
				echo "$DEF_DIRTY_RATIO"			> /proc/sys/vm/dirty_ratio
				echo "$DEF_DIRTY_BACKGROUND_RATIO"	> /proc/sys/vm/dirty_background_ratio
				;;
		esac
		if [ $DO_REMOUNTS -eq 1 ] ; then
			cat /etc/mtab | while read DEV MP FST OPTS DUMP PASS ; do
				# Reset commit and atime options to defaults.
				if [ "$FST" = 'unknown' ]; then
					FST=$(deduce_fstype $MP)
				fi
				case "$FST" in
					"ext3"|"reiserfs")
						PARSEDOPTS="$(parse_mount_opts_wfstab $DEV commit $OPTS)"
						PARSEDOPTS="$(parse_yesno_opts_wfstab $DEV atime atime $PARSEDOPTS)"
						mount $DEV -t $FST $MP -o remount,$PARSEDOPTS
						;;
					"xfs")
						PARSEDOPTS="$(parse_yesno_opts_wfstab $DEV atime atime $OPTS)"
						mount $DEV -t $FST $MP -o remount,$PARSEDOPTS
						;;
				esac
				if [ -b $DEV ] ; then
					blockdev --setra 256 $DEV
				fi
			done
		fi
		if [ $DO_HD -eq 1 ] ; then
			for THISHD in $HD ; do
				/sbin/hdparm -S $AC_HD $THISHD > /dev/null 2>&1
				/sbin/hdparm -B 255 $THISHD > /dev/null 2>&1
			done
		fi
		if [ $DO_CPU -eq 1 -a -e /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq ]; then
			echo `cat /sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq` > /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq
		fi
		echo "."
		;;
	*)
		echo "Usage: $0 {start|stop}" 2>&1
		exit 1
		;;

  esac

  exit 0


ACPI integration
----------------

Dax Kelson submitted this so that the ACPI acpid daemon will
kick off the laptop_mode script and run hdparm. The part that
automatically disables laptop mode when the battery is low was
written by Jan Topinski.

/etc/acpi/events/ac_adapter::

	event=ac_adapter
	action=/etc/acpi/actions/ac.sh %e

/etc/acpi/events/battery::

	event=battery.*
	action=/etc/acpi/actions/battery.sh %e

/etc/acpi/actions/ac.sh::

  #!/bin/bash

  # ac on/offline event handler

  status=`awk '/^state: / { print $2 }' /proc/acpi/ac_adapter/$2/state`

  case $status in
          "on-line")
                  /sbin/laptop_mode stop
                  exit 0
          ;;
          "off-line")
                  /sbin/laptop_mode start
                  exit 0
          ;;
  esac


/etc/acpi/actions/battery.sh::

  #! /bin/bash

  # Automatically disable laptop mode when the battery almost runs out.

  BATT_INFO=/proc/acpi/battery/$2/state

  if [[ -f /proc/sys/vm/laptop_mode ]]
  then
     LM=`cat /proc/sys/vm/laptop_mode`
     if [[ $LM -gt 0 ]]
     then
       if [[ -f $BATT_INFO ]]
       then
          # Source the config file only now that we know we need
          if [ -f /etc/default/laptop-mode ] ; then
                  # Debian
                  . /etc/default/laptop-mode
          elif [ -f /etc/sysconfig/laptop-mode ] ; then
                  # Others
                  . /etc/sysconfig/laptop-mode
          fi
          MINIMUM_BATTERY_MINUTES=${MINIMUM_BATTERY_MINUTES:-'10'}

          ACTION="`cat $BATT_INFO | grep charging | cut -c 26-`"
          if [[ ACTION -eq "discharging" ]]
          then
             PRESENT_RATE=`cat $BATT_INFO | grep "present rate:" | sed  "s/.* \([0-9][0-9]* \).*/\1/" `
             REMAINING=`cat $BATT_INFO | grep "remaining capacity:" | sed  "s/.* \([0-9][0-9]* \).*/\1/" `
          fi
          if (($REMAINING * 60 / $PRESENT_RATE < $MINIMUM_BATTERY_MINUTES))
          then
             /sbin/laptop_mode stop
          fi
       else
         logger -p daemon.warning "You are using laptop mode and your battery interface $BATT_INFO is missing. This may lead to loss of data when the battery runs out. Check kernel ACPI support and /proc/acpi/battery folder, and edit /etc/acpi/battery.sh to set BATT_INFO to the correct path."
       fi
     fi
  fi


Monitoring tool
---------------

Bartek Kania submitted this, it can be used to measure how much time your disk
spends spun up/down.  See tools/laptop/dslm/dslm.c
