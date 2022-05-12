// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/genhd.h>
#include <linux/mutex.h>
#include <linux/ide.h>
#include <linux/hdreg.h>
#include <linux/dmi.h>
#include <linux/slab.h>

#if !defined(CONFIG_DEBUG_BLOCK_EXT_DEVT)
#define IDE_DISK_MINORS		(1 << PARTN_BITS)
#else
#define IDE_DISK_MINORS		0
#endif

#include "ide-disk.h"
#include "ide-floppy.h"

#define IDE_GD_VERSION	"1.18"

/* module parameters */
static DEFINE_MUTEX(ide_gd_mutex);
static unsigned long debug_mask;
module_param(debug_mask, ulong, 0644);

static DEFINE_MUTEX(ide_disk_ref_mutex);

static void ide_disk_release(struct device *);

static struct ide_disk_obj *ide_disk_get(struct gendisk *disk)
{
	struct ide_disk_obj *idkp = NULL;

	mutex_lock(&ide_disk_ref_mutex);
	idkp = ide_drv_g(disk, ide_disk_obj);
	if (idkp) {
		if (ide_device_get(idkp->drive))
			idkp = NULL;
		else
			get_device(&idkp->dev);
	}
	mutex_unlock(&ide_disk_ref_mutex);
	return idkp;
}

static void ide_disk_put(struct ide_disk_obj *idkp)
{
	ide_drive_t *drive = idkp->drive;

	mutex_lock(&ide_disk_ref_mutex);
	put_device(&idkp->dev);
	ide_device_put(drive);
	mutex_unlock(&ide_disk_ref_mutex);
}

sector_t ide_gd_capacity(ide_drive_t *drive)
{
	return drive->capacity64;
}

static int ide_gd_probe(ide_drive_t *);

static void ide_gd_remove(ide_drive_t *drive)
{
	struct ide_disk_obj *idkp = drive->driver_data;
	struct gendisk *g = idkp->disk;

	ide_proc_unregister_driver(drive, idkp->driver);
	device_del(&idkp->dev);
	del_gendisk(g);
	drive->disk_ops->flush(drive);

	mutex_lock(&ide_disk_ref_mutex);
	put_device(&idkp->dev);
	mutex_unlock(&ide_disk_ref_mutex);
}

static void ide_disk_release(struct device *dev)
{
	struct ide_disk_obj *idkp = to_ide_drv(dev, ide_disk_obj);
	ide_drive_t *drive = idkp->drive;
	struct gendisk *g = idkp->disk;

	drive->disk_ops = NULL;
	drive->driver_data = NULL;
	g->private_data = NULL;
	put_disk(g);
	kfree(idkp);
}

/*
 * On HPA drives the capacity needs to be
 * reinitialized on resume otherwise the disk
 * can not be used and a hard reset is required
 */
static void ide_gd_resume(ide_drive_t *drive)
{
	if (ata_id_hpa_enabled(drive->id))
		(void)drive->disk_ops->get_capacity(drive);
}

static const struct dmi_system_id ide_coldreboot_table[] = {
	{
		/* Acer TravelMate 66x cuts power during reboot */
		.ident   = "Acer TravelMate 660",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "Acer"),
			DMI_MATCH(DMI_PRODUCT_NAME, "TravelMate 660"),
		},
	},

	{ }	/* terminate list */
};

static void ide_gd_shutdown(ide_drive_t *drive)
{
#ifdef	CONFIG_ALPHA
	/* On Alpha, halt(8) doesn't actually turn the machine off,
	   it puts you into the sort of firmware monitor. Typically,
	   it's used to boot another kernel image, so it's not much
	   different from reboot(8). Therefore, we don't need to
	   spin down the disk in this case, especially since Alpha
	   firmware doesn't handle disks in standby mode properly.
	   On the other hand, it's reasonably safe to turn the power
	   off when the shutdown process reaches the firmware prompt,
	   as the firmware initialization takes rather long time -
	   at least 10 seconds, which should be sufficient for
	   the disk to expire its write cache. */
	if (system_state != SYSTEM_POWER_OFF) {
#else
	if (system_state == SYSTEM_RESTART &&
		!dmi_check_system(ide_coldreboot_table)) {
#endif
		drive->disk_ops->flush(drive);
		return;
	}

	printk(KERN_INFO "Shutdown: %s\n", drive->name);

	drive->gendev.bus->suspend(&drive->gendev, PMSG_SUSPEND);
}

#ifdef CONFIG_IDE_PROC_FS
static ide_proc_entry_t *ide_disk_proc_entries(ide_drive_t *drive)
{
	return (drive->media == ide_disk) ? ide_disk_proc : ide_floppy_proc;
}

static const struct ide_proc_devset *ide_disk_proc_devsets(ide_drive_t *drive)
{
	return (drive->media == ide_disk) ? ide_disk_settings
					  : ide_floppy_settings;
}
#endif

static ide_startstop_t ide_gd_do_request(ide_drive_t *drive,
					 struct request *rq, sector_t sector)
{
	return drive->disk_ops->do_request(drive, rq, sector);
}

static struct ide_driver ide_gd_driver = {
	.gen_driver = {
		.owner		= THIS_MODULE,
		.name		= "ide-gd",
		.bus		= &ide_bus_type,
	},
	.probe			= ide_gd_probe,
	.remove			= ide_gd_remove,
	.resume			= ide_gd_resume,
	.shutdown		= ide_gd_shutdown,
	.version		= IDE_GD_VERSION,
	.do_request		= ide_gd_do_request,
#ifdef CONFIG_IDE_PROC_FS
	.proc_entries		= ide_disk_proc_entries,
	.proc_devsets		= ide_disk_proc_devsets,
#endif
};

static int ide_gd_open(struct block_device *bdev, fmode_t mode)
{
	struct gendisk *disk = bdev->bd_disk;
	struct ide_disk_obj *idkp;
	ide_drive_t *drive;
	int ret = 0;

	idkp = ide_disk_get(disk);
	if (idkp == NULL)
		return -ENXIO;

	drive = idkp->drive;

	ide_debug_log(IDE_DBG_FUNC, "enter");

	idkp->openers++;

	if ((drive->dev_flags & IDE_DFLAG_REMOVABLE) && idkp->openers == 1) {
		drive->dev_flags &= ~IDE_DFLAG_FORMAT_IN_PROGRESS;
		/* Just in case */

		ret = drive->disk_ops->init_media(drive, disk);

		/*
		 * Allow O_NDELAY to open a drive without a disk, or with an
		 * unreadable disk, so that we can get the format capacity
		 * of the drive or begin the format - Sam
		 */
		if (ret && (mode & FMODE_NDELAY) == 0) {
			ret = -EIO;
			goto out_put_idkp;
		}

		if ((drive->dev_flags & IDE_DFLAG_WP) && (mode & FMODE_WRITE)) {
			ret = -EROFS;
			goto out_put_idkp;
		}

		/*
		 * Ignore the return code from door_lock,
		 * since the open() has already succeeded,
		 * and the door_lock is irrelevant at this point.
		 */
		drive->disk_ops->set_doorlock(drive, disk, 1);
		if (__invalidate_device(bdev, true))
			pr_warn("VFS: busy inodes on changed media %s\n",
				bdev->bd_disk->disk_name);
		drive->disk_ops->get_capacity(drive);
		set_capacity(disk, ide_gd_capacity(drive));
		set_bit(GD_NEED_PART_SCAN, &disk->state);
	} else if (drive->dev_flags & IDE_DFLAG_FORMAT_IN_PROGRESS) {
		ret = -EBUSY;
		goto out_put_idkp;
	}
	return 0;

out_put_idkp:
	idkp->openers--;
	ide_disk_put(idkp);
	return ret;
}

static int ide_gd_unlocked_open(struct block_device *bdev, fmode_t mode)
{
	int ret;

	mutex_lock(&ide_gd_mutex);
	ret = ide_gd_open(bdev, mode);
	mutex_unlock(&ide_gd_mutex);

	return ret;
}


static void ide_gd_release(struct gendisk *disk, fmode_t mode)
{
	struct ide_disk_obj *idkp = ide_drv_g(disk, ide_disk_obj);
	ide_drive_t *drive = idkp->drive;

	ide_debug_log(IDE_DBG_FUNC, "enter");

	mutex_lock(&ide_gd_mutex);
	if (idkp->openers == 1)
		drive->disk_ops->flush(drive);

	if ((drive->dev_flags & IDE_DFLAG_REMOVABLE) && idkp->openers == 1) {
		drive->disk_ops->set_doorlock(drive, disk, 0);
		drive->dev_flags &= ~IDE_DFLAG_FORMAT_IN_PROGRESS;
	}

	idkp->openers--;

	ide_disk_put(idkp);
	mutex_unlock(&ide_gd_mutex);
}

static int ide_gd_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	struct ide_disk_obj *idkp = ide_drv_g(bdev->bd_disk, ide_disk_obj);
	ide_drive_t *drive = idkp->drive;

	geo->heads = drive->bios_head;
	geo->sectors = drive->bios_sect;
	geo->cylinders = (u16)drive->bios_cyl; /* truncate */
	return 0;
}

static void ide_gd_unlock_native_capacity(struct gendisk *disk)
{
	struct ide_disk_obj *idkp = ide_drv_g(disk, ide_disk_obj);
	ide_drive_t *drive = idkp->drive;
	const struct ide_disk_ops *disk_ops = drive->disk_ops;

	if (disk_ops->unlock_native_capacity)
		disk_ops->unlock_native_capacity(drive);
}

static int ide_gd_ioctl(struct block_device *bdev, fmode_t mode,
			     unsigned int cmd, unsigned long arg)
{
	struct ide_disk_obj *idkp = ide_drv_g(bdev->bd_disk, ide_disk_obj);
	ide_drive_t *drive = idkp->drive;

	return drive->disk_ops->ioctl(drive, bdev, mode, cmd, arg);
}

#ifdef CONFIG_COMPAT
static int ide_gd_compat_ioctl(struct block_device *bdev, fmode_t mode,
			       unsigned int cmd, unsigned long arg)
{
	struct ide_disk_obj *idkp = ide_drv_g(bdev->bd_disk, ide_disk_obj);
	ide_drive_t *drive = idkp->drive;

	if (!drive->disk_ops->compat_ioctl)
		return -ENOIOCTLCMD;

	return drive->disk_ops->compat_ioctl(drive, bdev, mode, cmd, arg);
}
#endif

static const struct block_device_operations ide_gd_ops = {
	.owner			= THIS_MODULE,
	.open			= ide_gd_unlocked_open,
	.release		= ide_gd_release,
	.ioctl			= ide_gd_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl		= ide_gd_compat_ioctl,
#endif
	.getgeo			= ide_gd_getgeo,
	.unlock_native_capacity	= ide_gd_unlock_native_capacity,
};

static int ide_gd_probe(ide_drive_t *drive)
{
	const struct ide_disk_ops *disk_ops = NULL;
	struct ide_disk_obj *idkp;
	struct gendisk *g;

	/* strstr("foo", "") is non-NULL */
	if (!strstr("ide-gd", drive->driver_req))
		goto failed;

#ifdef CONFIG_IDE_GD_ATA
	if (drive->media == ide_disk)
		disk_ops = &ide_ata_disk_ops;
#endif
#ifdef CONFIG_IDE_GD_ATAPI
	if (drive->media == ide_floppy)
		disk_ops = &ide_atapi_disk_ops;
#endif
	if (disk_ops == NULL)
		goto failed;

	if (disk_ops->check(drive, DRV_NAME) == 0) {
		printk(KERN_ERR PFX "%s: not supported by this driver\n",
			drive->name);
		goto failed;
	}

	idkp = kzalloc(sizeof(*idkp), GFP_KERNEL);
	if (!idkp) {
		printk(KERN_ERR PFX "%s: can't allocate a disk structure\n",
			drive->name);
		goto failed;
	}

	g = alloc_disk_node(IDE_DISK_MINORS, hwif_to_node(drive->hwif));
	if (!g)
		goto out_free_idkp;

	ide_init_disk(g, drive);

	idkp->dev.parent = &drive->gendev;
	idkp->dev.release = ide_disk_release;
	dev_set_name(&idkp->dev, "%s", dev_name(&drive->gendev));

	if (device_register(&idkp->dev))
		goto out_free_disk;

	idkp->drive = drive;
	idkp->driver = &ide_gd_driver;
	idkp->disk = g;

	g->private_data = &idkp->driver;

	drive->driver_data = idkp;
	drive->debug_mask = debug_mask;
	drive->disk_ops = disk_ops;

	disk_ops->setup(drive);

	set_capacity(g, ide_gd_capacity(drive));

	g->minors = IDE_DISK_MINORS;
	g->flags |= GENHD_FL_EXT_DEVT;
	if (drive->dev_flags & IDE_DFLAG_REMOVABLE)
		g->flags = GENHD_FL_REMOVABLE;
	g->fops = &ide_gd_ops;
	g->events = DISK_EVENT_MEDIA_CHANGE;
	device_add_disk(&drive->gendev, g, NULL);
	return 0;

out_free_disk:
	put_disk(g);
out_free_idkp:
	kfree(idkp);
failed:
	return -ENODEV;
}

static int __init ide_gd_init(void)
{
	printk(KERN_INFO DRV_NAME " driver " IDE_GD_VERSION "\n");
	return driver_register(&ide_gd_driver.gen_driver);
}

static void __exit ide_gd_exit(void)
{
	driver_unregister(&ide_gd_driver.gen_driver);
}

MODULE_ALIAS("ide:*m-disk*");
MODULE_ALIAS("ide-disk");
MODULE_ALIAS("ide:*m-floppy*");
MODULE_ALIAS("ide-floppy");
module_init(ide_gd_init);
module_exit(ide_gd_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("generic ATA/ATAPI disk driver");
