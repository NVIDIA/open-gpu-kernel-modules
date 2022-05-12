// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/ide.h>
#include <linux/hdreg.h>
#include <linux/mutex.h>

#include "ide-disk.h"

static DEFINE_MUTEX(ide_disk_ioctl_mutex);
static const struct ide_ioctl_devset ide_disk_ioctl_settings[] = {
{ HDIO_GET_ADDRESS,	HDIO_SET_ADDRESS,   &ide_devset_address   },
{ HDIO_GET_MULTCOUNT,	HDIO_SET_MULTCOUNT, &ide_devset_multcount },
{ HDIO_GET_NOWERR,	HDIO_SET_NOWERR,    &ide_devset_nowerr	  },
{ HDIO_GET_WCACHE,	HDIO_SET_WCACHE,    &ide_devset_wcache	  },
{ HDIO_GET_ACOUSTIC,	HDIO_SET_ACOUSTIC,  &ide_devset_acoustic  },
{ 0 }
};

int ide_disk_ioctl(ide_drive_t *drive, struct block_device *bdev, fmode_t mode,
		   unsigned int cmd, unsigned long arg)
{
	int err;

	mutex_lock(&ide_disk_ioctl_mutex);
	err = ide_setting_ioctl(drive, bdev, cmd, arg, ide_disk_ioctl_settings);
	if (err != -EOPNOTSUPP)
		goto out;

	err = generic_ide_ioctl(drive, bdev, cmd, arg);
out:
	mutex_unlock(&ide_disk_ioctl_mutex);
	return err;
}
