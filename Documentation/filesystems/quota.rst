.. SPDX-License-Identifier: GPL-2.0

===============
Quota subsystem
===============

Quota subsystem allows system administrator to set limits on used space and
number of used inodes (inode is a filesystem structure which is associated with
each file or directory) for users and/or groups. For both used space and number
of used inodes there are actually two limits. The first one is called softlimit
and the second one hardlimit.  A user can never exceed a hardlimit for any
resource (unless he has CAP_SYS_RESOURCE capability). User is allowed to exceed
softlimit but only for limited period of time. This period is called "grace
period" or "grace time". When grace time is over, user is not able to allocate
more space/inodes until he frees enough of them to get below softlimit.

Quota limits (and amount of grace time) are set independently for each
filesystem.

For more details about quota design, see the documentation in quota-tools package
(https://sourceforge.net/projects/linuxquota).

Quota netlink interface
=======================
When user exceeds a softlimit, runs out of grace time or reaches hardlimit,
quota subsystem traditionally printed a message to the controlling terminal of
the process which caused the excess. This method has the disadvantage that
when user is using a graphical desktop he usually cannot see the message.
Thus quota netlink interface has been designed to pass information about
the above events to userspace. There they can be captured by an application
and processed accordingly.

The interface uses generic netlink framework (see
https://lwn.net/Articles/208755/ and http://www.infradead.org/~tgr/libnl/ for
more details about this layer). The name of the quota generic netlink interface
is "VFS_DQUOT". Definitions of constants below are in <linux/quota.h>.  Since
the quota netlink protocol is not namespace aware, quota netlink messages are
sent only in initial network namespace.

Currently, the interface supports only one message type QUOTA_NL_C_WARNING.
This command is used to send a notification about any of the above mentioned
events. Each message has six attributes. These are (type of the argument is
in parentheses):

        QUOTA_NL_A_QTYPE (u32)
	  - type of quota being exceeded (one of USRQUOTA, GRPQUOTA)
        QUOTA_NL_A_EXCESS_ID (u64)
	  - UID/GID (depends on quota type) of user / group whose limit
	    is being exceeded.
        QUOTA_NL_A_CAUSED_ID (u64)
	  - UID of a user who caused the event
        QUOTA_NL_A_WARNING (u32)
	  - what kind of limit is exceeded:

		QUOTA_NL_IHARDWARN
		    inode hardlimit
		QUOTA_NL_ISOFTLONGWARN
		    inode softlimit is exceeded longer
		    than given grace period
		QUOTA_NL_ISOFTWARN
		    inode softlimit
		QUOTA_NL_BHARDWARN
		    space (block) hardlimit
		QUOTA_NL_BSOFTLONGWARN
		    space (block) softlimit is exceeded
		    longer than given grace period.
		QUOTA_NL_BSOFTWARN
		    space (block) softlimit

	  - four warnings are also defined for the event when user stops
	    exceeding some limit:

		QUOTA_NL_IHARDBELOW
		    inode hardlimit
		QUOTA_NL_ISOFTBELOW
		    inode softlimit
		QUOTA_NL_BHARDBELOW
		    space (block) hardlimit
		QUOTA_NL_BSOFTBELOW
		    space (block) softlimit

        QUOTA_NL_A_DEV_MAJOR (u32)
	  - major number of a device with the affected filesystem
        QUOTA_NL_A_DEV_MINOR (u32)
	  - minor number of a device with the affected filesystem
