// SPDX-License-Identifier: GPL-2.0-only
/*======================================================================

  This driver provides a method to access memory not used by the kernel
  itself (i.e. if the kernel commandline mem=xxx is used). To actually
  use slram at least mtdblock or mtdchar is required (for block or
  character device access).

  Usage:

  if compiled as loadable module:
    modprobe slram map=<name>,<start>,<end/offset>
  if statically linked into the kernel use the following kernel cmd.line
    slram=<name>,<start>,<end/offset>

  <name>: name of the device that will be listed in /proc/mtd
  <start>: start of the memory region, decimal or hex (0xabcdef)
  <end/offset>: end of the memory region. It's possible to use +0x1234
                to specify the offset instead of the absolute address

  NOTE:
  With slram it's only possible to map a contiguous memory region. Therefore
  if there's a device mapped somewhere in the region specified slram will
  fail to load (see kernel log if modprobe fails).

  -

  Jochen Schaeuble <psionic@psionic.de>

======================================================================*/


#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/init.h>
#include <linux/io.h>

#include <linux/mtd/mtd.h>

#define SLRAM_MAX_DEVICES_PARAMS 6		/* 3 parameters / device */
#define SLRAM_BLK_SZ 0x4000

#define T(fmt, args...) printk(KERN_DEBUG fmt, ## args)
#define E(fmt, args...) printk(KERN_NOTICE fmt, ## args)

typedef struct slram_priv {
	u_char *start;
	u_char *end;
} slram_priv_t;

typedef struct slram_mtd_list {
	struct mtd_info *mtdinfo;
	struct slram_mtd_list *next;
} slram_mtd_list_t;

#ifdef MODULE
static char *map[SLRAM_MAX_DEVICES_PARAMS];

module_param_array(map, charp, NULL, 0);
MODULE_PARM_DESC(map, "List of memory regions to map. \"map=<name>, <start>, <length / end>\"");
#else
static char *map;
#endif

static slram_mtd_list_t *slram_mtdlist = NULL;

static int slram_erase(struct mtd_info *, struct erase_info *);
static int slram_point(struct mtd_info *, loff_t, size_t, size_t *, void **,
		resource_size_t *);
static int slram_unpoint(struct mtd_info *, loff_t, size_t);
static int slram_read(struct mtd_info *, loff_t, size_t, size_t *, u_char *);
static int slram_write(struct mtd_info *, loff_t, size_t, size_t *, const u_char *);

static int slram_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	slram_priv_t *priv = mtd->priv;

	memset(priv->start + instr->addr, 0xff, instr->len);

	return(0);
}

static int slram_point(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, void **virt, resource_size_t *phys)
{
	slram_priv_t *priv = mtd->priv;

	*virt = priv->start + from;
	*retlen = len;
	return(0);
}

static int slram_unpoint(struct mtd_info *mtd, loff_t from, size_t len)
{
	return 0;
}

static int slram_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
	slram_priv_t *priv = mtd->priv;

	memcpy(buf, priv->start + from, len);
	*retlen = len;
	return(0);
}

static int slram_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf)
{
	slram_priv_t *priv = mtd->priv;

	memcpy(priv->start + to, buf, len);
	*retlen = len;
	return(0);
}

/*====================================================================*/

static int register_device(char *name, unsigned long start, unsigned long length)
{
	slram_mtd_list_t **curmtd;

	curmtd = &slram_mtdlist;
	while (*curmtd) {
		curmtd = &(*curmtd)->next;
	}

	*curmtd = kmalloc(sizeof(slram_mtd_list_t), GFP_KERNEL);
	if (!(*curmtd)) {
		E("slram: Cannot allocate new MTD device.\n");
		return(-ENOMEM);
	}
	(*curmtd)->mtdinfo = kzalloc(sizeof(struct mtd_info), GFP_KERNEL);
	(*curmtd)->next = NULL;

	if ((*curmtd)->mtdinfo)	{
		(*curmtd)->mtdinfo->priv =
			kzalloc(sizeof(slram_priv_t), GFP_KERNEL);

		if (!(*curmtd)->mtdinfo->priv) {
			kfree((*curmtd)->mtdinfo);
			(*curmtd)->mtdinfo = NULL;
		}
	}

	if (!(*curmtd)->mtdinfo) {
		E("slram: Cannot allocate new MTD device.\n");
		return(-ENOMEM);
	}

	if (!(((slram_priv_t *)(*curmtd)->mtdinfo->priv)->start =
		memremap(start, length,
			 MEMREMAP_WB | MEMREMAP_WT | MEMREMAP_WC))) {
		E("slram: memremap failed\n");
		return -EIO;
	}
	((slram_priv_t *)(*curmtd)->mtdinfo->priv)->end =
		((slram_priv_t *)(*curmtd)->mtdinfo->priv)->start + length;


	(*curmtd)->mtdinfo->name = name;
	(*curmtd)->mtdinfo->size = length;
	(*curmtd)->mtdinfo->flags = MTD_CAP_RAM;
	(*curmtd)->mtdinfo->_erase = slram_erase;
	(*curmtd)->mtdinfo->_point = slram_point;
	(*curmtd)->mtdinfo->_unpoint = slram_unpoint;
	(*curmtd)->mtdinfo->_read = slram_read;
	(*curmtd)->mtdinfo->_write = slram_write;
	(*curmtd)->mtdinfo->owner = THIS_MODULE;
	(*curmtd)->mtdinfo->type = MTD_RAM;
	(*curmtd)->mtdinfo->erasesize = SLRAM_BLK_SZ;
	(*curmtd)->mtdinfo->writesize = 1;

	if (mtd_device_register((*curmtd)->mtdinfo, NULL, 0))	{
		E("slram: Failed to register new device\n");
		memunmap(((slram_priv_t *)(*curmtd)->mtdinfo->priv)->start);
		kfree((*curmtd)->mtdinfo->priv);
		kfree((*curmtd)->mtdinfo);
		return(-EAGAIN);
	}
	T("slram: Registered device %s from %luKiB to %luKiB\n", name,
			(start / 1024), ((start + length) / 1024));
	T("slram: Mapped from 0x%p to 0x%p\n",
			((slram_priv_t *)(*curmtd)->mtdinfo->priv)->start,
			((slram_priv_t *)(*curmtd)->mtdinfo->priv)->end);
	return(0);
}

static void unregister_devices(void)
{
	slram_mtd_list_t *nextitem;

	while (slram_mtdlist) {
		nextitem = slram_mtdlist->next;
		mtd_device_unregister(slram_mtdlist->mtdinfo);
		memunmap(((slram_priv_t *)slram_mtdlist->mtdinfo->priv)->start);
		kfree(slram_mtdlist->mtdinfo->priv);
		kfree(slram_mtdlist->mtdinfo);
		kfree(slram_mtdlist);
		slram_mtdlist = nextitem;
	}
}

static unsigned long handle_unit(unsigned long value, char *unit)
{
	if ((*unit == 'M') || (*unit == 'm')) {
		return(value * 1024 * 1024);
	} else if ((*unit == 'K') || (*unit == 'k')) {
		return(value * 1024);
	}
	return(value);
}

static int parse_cmdline(char *devname, char *szstart, char *szlength)
{
	char *buffer;
	unsigned long devstart;
	unsigned long devlength;

	if ((!devname) || (!szstart) || (!szlength)) {
		unregister_devices();
		return(-EINVAL);
	}

	devstart = simple_strtoul(szstart, &buffer, 0);
	devstart = handle_unit(devstart, buffer);

	if (*(szlength) != '+') {
		devlength = simple_strtoul(szlength, &buffer, 0);
		devlength = handle_unit(devlength, buffer);
		if (devlength < devstart)
			goto err_out;

		devlength -= devstart;
	} else {
		devlength = simple_strtoul(szlength + 1, &buffer, 0);
		devlength = handle_unit(devlength, buffer);
	}
	T("slram: devname=%s, devstart=0x%lx, devlength=0x%lx\n",
			devname, devstart, devlength);
	if (devlength % SLRAM_BLK_SZ != 0)
		goto err_out;

	if ((devstart = register_device(devname, devstart, devlength))){
		unregister_devices();
		return((int)devstart);
	}
	return(0);

err_out:
	E("slram: Illegal length parameter.\n");
	return(-EINVAL);
}

#ifndef MODULE

static int __init mtd_slram_setup(char *str)
{
	map = str;
	return(1);
}

__setup("slram=", mtd_slram_setup);

#endif

static int __init init_slram(void)
{
	char *devname;

#ifndef MODULE
	char *devstart;
	char *devlength;

	if (!map) {
		E("slram: not enough parameters.\n");
		return(-EINVAL);
	}
	while (map) {
		devname = devstart = devlength = NULL;

		if (!(devname = strsep(&map, ","))) {
			E("slram: No devicename specified.\n");
			break;
		}
		T("slram: devname = %s\n", devname);
		if ((!map) || (!(devstart = strsep(&map, ",")))) {
			E("slram: No devicestart specified.\n");
		}
		T("slram: devstart = %s\n", devstart);
		if ((!map) || (!(devlength = strsep(&map, ",")))) {
			E("slram: No devicelength / -end specified.\n");
		}
		T("slram: devlength = %s\n", devlength);
		if (parse_cmdline(devname, devstart, devlength) != 0) {
			return(-EINVAL);
		}
	}
#else
	int count;
	int i;

	for (count = 0; count < SLRAM_MAX_DEVICES_PARAMS && map[count];
			count++) {
	}

	if ((count % 3 != 0) || (count == 0)) {
		E("slram: not enough parameters.\n");
		return(-EINVAL);
	}
	for (i = 0; i < (count / 3); i++) {
		devname = map[i * 3];

		if (parse_cmdline(devname, map[i * 3 + 1], map[i * 3 + 2])!=0) {
			return(-EINVAL);
		}

	}
#endif /* !MODULE */

	return(0);
}

static void __exit cleanup_slram(void)
{
	unregister_devices();
}

module_init(init_slram);
module_exit(cleanup_slram);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jochen Schaeuble <psionic@psionic.de>");
MODULE_DESCRIPTION("MTD driver for uncached system RAM");
