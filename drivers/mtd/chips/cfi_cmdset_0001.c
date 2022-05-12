/*
 * Common Flash Interface support:
 *   Intel Extended Vendor Command Set (ID 0x0001)
 *
 * (C) 2000 Red Hat. GPL'd
 *
 *
 * 10/10/2000	Nicolas Pitre <nico@fluxnic.net>
 * 	- completely revamped method functions so they are aware and
 * 	  independent of the flash geometry (buswidth, interleave, etc.)
 * 	- scalability vs code size is completely set at compile-time
 * 	  (see include/linux/mtd/cfi.h for selection)
 *	- optimized write buffer method
 * 02/05/2002	Christopher Hoover <ch@hpl.hp.com>/<ch@murgatroid.com>
 *	- reworked lock/unlock/erase support for var size flash
 * 21/03/2007   Rodolfo Giometti <giometti@linux.it>
 * 	- auto unlock sectors on resume for auto locking flash on power up
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <asm/byteorder.h>

#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/reboot.h>
#include <linux/bitmap.h>
#include <linux/mtd/xip.h>
#include <linux/mtd/map.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/cfi.h>

/* #define CMDSET0001_DISABLE_ERASE_SUSPEND_ON_WRITE */
/* #define CMDSET0001_DISABLE_WRITE_SUSPEND */

// debugging, turns off buffer write mode if set to 1
#define FORCE_WORD_WRITE 0

/* Intel chips */
#define I82802AB	0x00ad
#define I82802AC	0x00ac
#define PF38F4476	0x881c
#define M28F00AP30	0x8963
/* STMicroelectronics chips */
#define M50LPW080       0x002F
#define M50FLW080A	0x0080
#define M50FLW080B	0x0081
/* Atmel chips */
#define AT49BV640D	0x02de
#define AT49BV640DT	0x02db
/* Sharp chips */
#define LH28F640BFHE_PTTL90	0x00b0
#define LH28F640BFHE_PBTL90	0x00b1
#define LH28F640BFHE_PTTL70A	0x00b2
#define LH28F640BFHE_PBTL70A	0x00b3

static int cfi_intelext_read (struct mtd_info *, loff_t, size_t, size_t *, u_char *);
static int cfi_intelext_write_words(struct mtd_info *, loff_t, size_t, size_t *, const u_char *);
static int cfi_intelext_write_buffers(struct mtd_info *, loff_t, size_t, size_t *, const u_char *);
static int cfi_intelext_writev(struct mtd_info *, const struct kvec *, unsigned long, loff_t, size_t *);
static int cfi_intelext_erase_varsize(struct mtd_info *, struct erase_info *);
static void cfi_intelext_sync (struct mtd_info *);
static int cfi_intelext_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len);
static int cfi_intelext_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len);
static int cfi_intelext_is_locked(struct mtd_info *mtd, loff_t ofs,
				  uint64_t len);
#ifdef CONFIG_MTD_OTP
static int cfi_intelext_read_fact_prot_reg (struct mtd_info *, loff_t, size_t, size_t *, u_char *);
static int cfi_intelext_read_user_prot_reg (struct mtd_info *, loff_t, size_t, size_t *, u_char *);
static int cfi_intelext_write_user_prot_reg(struct mtd_info *, loff_t, size_t,
					    size_t *, const u_char *);
static int cfi_intelext_lock_user_prot_reg (struct mtd_info *, loff_t, size_t);
static int cfi_intelext_get_fact_prot_info(struct mtd_info *, size_t,
					   size_t *, struct otp_info *);
static int cfi_intelext_get_user_prot_info(struct mtd_info *, size_t,
					   size_t *, struct otp_info *);
#endif
static int cfi_intelext_suspend (struct mtd_info *);
static void cfi_intelext_resume (struct mtd_info *);
static int cfi_intelext_reboot (struct notifier_block *, unsigned long, void *);

static void cfi_intelext_destroy(struct mtd_info *);

struct mtd_info *cfi_cmdset_0001(struct map_info *, int);

static struct mtd_info *cfi_intelext_setup (struct mtd_info *);
static int cfi_intelext_partition_fixup(struct mtd_info *, struct cfi_private **);

static int cfi_intelext_point (struct mtd_info *mtd, loff_t from, size_t len,
		     size_t *retlen, void **virt, resource_size_t *phys);
static int cfi_intelext_unpoint(struct mtd_info *mtd, loff_t from, size_t len);

static int chip_ready (struct map_info *map, struct flchip *chip, unsigned long adr, int mode);
static int get_chip(struct map_info *map, struct flchip *chip, unsigned long adr, int mode);
static void put_chip(struct map_info *map, struct flchip *chip, unsigned long adr);
#include "fwh_lock.h"



/*
 *  *********** SETUP AND PROBE BITS  ***********
 */

static struct mtd_chip_driver cfi_intelext_chipdrv = {
	.probe		= NULL, /* Not usable directly */
	.destroy	= cfi_intelext_destroy,
	.name		= "cfi_cmdset_0001",
	.module		= THIS_MODULE
};

/* #define DEBUG_LOCK_BITS */
/* #define DEBUG_CFI_FEATURES */

#ifdef DEBUG_CFI_FEATURES
static void cfi_tell_features(struct cfi_pri_intelext *extp)
{
	int i;
	printk("  Extended Query version %c.%c\n", extp->MajorVersion, extp->MinorVersion);
	printk("  Feature/Command Support:      %4.4X\n", extp->FeatureSupport);
	printk("     - Chip Erase:              %s\n", extp->FeatureSupport&1?"supported":"unsupported");
	printk("     - Suspend Erase:           %s\n", extp->FeatureSupport&2?"supported":"unsupported");
	printk("     - Suspend Program:         %s\n", extp->FeatureSupport&4?"supported":"unsupported");
	printk("     - Legacy Lock/Unlock:      %s\n", extp->FeatureSupport&8?"supported":"unsupported");
	printk("     - Queued Erase:            %s\n", extp->FeatureSupport&16?"supported":"unsupported");
	printk("     - Instant block lock:      %s\n", extp->FeatureSupport&32?"supported":"unsupported");
	printk("     - Protection Bits:         %s\n", extp->FeatureSupport&64?"supported":"unsupported");
	printk("     - Page-mode read:          %s\n", extp->FeatureSupport&128?"supported":"unsupported");
	printk("     - Synchronous read:        %s\n", extp->FeatureSupport&256?"supported":"unsupported");
	printk("     - Simultaneous operations: %s\n", extp->FeatureSupport&512?"supported":"unsupported");
	printk("     - Extended Flash Array:    %s\n", extp->FeatureSupport&1024?"supported":"unsupported");
	for (i=11; i<32; i++) {
		if (extp->FeatureSupport & (1<<i))
			printk("     - Unknown Bit %X:      supported\n", i);
	}

	printk("  Supported functions after Suspend: %2.2X\n", extp->SuspendCmdSupport);
	printk("     - Program after Erase Suspend: %s\n", extp->SuspendCmdSupport&1?"supported":"unsupported");
	for (i=1; i<8; i++) {
		if (extp->SuspendCmdSupport & (1<<i))
			printk("     - Unknown Bit %X:               supported\n", i);
	}

	printk("  Block Status Register Mask: %4.4X\n", extp->BlkStatusRegMask);
	printk("     - Lock Bit Active:      %s\n", extp->BlkStatusRegMask&1?"yes":"no");
	printk("     - Lock-Down Bit Active: %s\n", extp->BlkStatusRegMask&2?"yes":"no");
	for (i=2; i<3; i++) {
		if (extp->BlkStatusRegMask & (1<<i))
			printk("     - Unknown Bit %X Active: yes\n",i);
	}
	printk("     - EFA Lock Bit:         %s\n", extp->BlkStatusRegMask&16?"yes":"no");
	printk("     - EFA Lock-Down Bit:    %s\n", extp->BlkStatusRegMask&32?"yes":"no");
	for (i=6; i<16; i++) {
		if (extp->BlkStatusRegMask & (1<<i))
			printk("     - Unknown Bit %X Active: yes\n",i);
	}

	printk("  Vcc Logic Supply Optimum Program/Erase Voltage: %d.%d V\n",
	       extp->VccOptimal >> 4, extp->VccOptimal & 0xf);
	if (extp->VppOptimal)
		printk("  Vpp Programming Supply Optimum Program/Erase Voltage: %d.%d V\n",
		       extp->VppOptimal >> 4, extp->VppOptimal & 0xf);
}
#endif

/* Atmel chips don't use the same PRI format as Intel chips */
static void fixup_convert_atmel_pri(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *extp = cfi->cmdset_priv;
	struct cfi_pri_atmel atmel_pri;
	uint32_t features = 0;

	/* Reverse byteswapping */
	extp->FeatureSupport = cpu_to_le32(extp->FeatureSupport);
	extp->BlkStatusRegMask = cpu_to_le16(extp->BlkStatusRegMask);
	extp->ProtRegAddr = cpu_to_le16(extp->ProtRegAddr);

	memcpy(&atmel_pri, extp, sizeof(atmel_pri));
	memset((char *)extp + 5, 0, sizeof(*extp) - 5);

	printk(KERN_ERR "atmel Features: %02x\n", atmel_pri.Features);

	if (atmel_pri.Features & 0x01) /* chip erase supported */
		features |= (1<<0);
	if (atmel_pri.Features & 0x02) /* erase suspend supported */
		features |= (1<<1);
	if (atmel_pri.Features & 0x04) /* program suspend supported */
		features |= (1<<2);
	if (atmel_pri.Features & 0x08) /* simultaneous operations supported */
		features |= (1<<9);
	if (atmel_pri.Features & 0x20) /* page mode read supported */
		features |= (1<<7);
	if (atmel_pri.Features & 0x40) /* queued erase supported */
		features |= (1<<4);
	if (atmel_pri.Features & 0x80) /* Protection bits supported */
		features |= (1<<6);

	extp->FeatureSupport = features;

	/* burst write mode not supported */
	cfi->cfiq->BufWriteTimeoutTyp = 0;
	cfi->cfiq->BufWriteTimeoutMax = 0;
}

static void fixup_at49bv640dx_lock(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *cfip = cfi->cmdset_priv;

	cfip->FeatureSupport |= (1 << 5);
	mtd->flags |= MTD_POWERUP_LOCK;
}

#ifdef CMDSET0001_DISABLE_ERASE_SUSPEND_ON_WRITE
/* Some Intel Strata Flash prior to FPO revision C has bugs in this area */
static void fixup_intel_strataflash(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *extp = cfi->cmdset_priv;

	printk(KERN_WARNING "cfi_cmdset_0001: Suspend "
	                    "erase on write disabled.\n");
	extp->SuspendCmdSupport &= ~1;
}
#endif

#ifdef CMDSET0001_DISABLE_WRITE_SUSPEND
static void fixup_no_write_suspend(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *cfip = cfi->cmdset_priv;

	if (cfip && (cfip->FeatureSupport&4)) {
		cfip->FeatureSupport &= ~4;
		printk(KERN_WARNING "cfi_cmdset_0001: write suspend disabled\n");
	}
}
#endif

static void fixup_st_m28w320ct(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;

	cfi->cfiq->BufWriteTimeoutTyp = 0;	/* Not supported */
	cfi->cfiq->BufWriteTimeoutMax = 0;	/* Not supported */
}

static void fixup_st_m28w320cb(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;

	/* Note this is done after the region info is endian swapped */
	cfi->cfiq->EraseRegionInfo[1] =
		(cfi->cfiq->EraseRegionInfo[1] & 0xffff0000) | 0x3e;
};

static int is_LH28F640BF(struct cfi_private *cfi)
{
	/* Sharp LH28F640BF Family */
	if (cfi->mfr == CFI_MFR_SHARP && (
	    cfi->id == LH28F640BFHE_PTTL90 || cfi->id == LH28F640BFHE_PBTL90 ||
	    cfi->id == LH28F640BFHE_PTTL70A || cfi->id == LH28F640BFHE_PBTL70A))
		return 1;
	return 0;
}

static void fixup_LH28F640BF(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *extp = cfi->cmdset_priv;

	/* Reset the Partition Configuration Register on LH28F640BF
	 * to a single partition (PCR = 0x000): PCR is embedded into A0-A15. */
	if (is_LH28F640BF(cfi)) {
		printk(KERN_INFO "Reset Partition Config. Register: 1 Partition of 4 planes\n");
		map_write(map, CMD(0x60), 0);
		map_write(map, CMD(0x04), 0);

		/* We have set one single partition thus
		 * Simultaneous Operations are not allowed */
		printk(KERN_INFO "cfi_cmdset_0001: Simultaneous Operations disabled\n");
		extp->FeatureSupport &= ~512;
	}
}

static void fixup_use_point(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	if (!mtd->_point && map_is_linear(map)) {
		mtd->_point   = cfi_intelext_point;
		mtd->_unpoint = cfi_intelext_unpoint;
	}
}

static void fixup_use_write_buffers(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	if (cfi->cfiq->BufWriteTimeoutTyp) {
		printk(KERN_INFO "Using buffer write method\n" );
		mtd->_write = cfi_intelext_write_buffers;
		mtd->_writev = cfi_intelext_writev;
	}
}

/*
 * Some chips power-up with all sectors locked by default.
 */
static void fixup_unlock_powerup_lock(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *cfip = cfi->cmdset_priv;

	if (cfip->FeatureSupport&32) {
		printk(KERN_INFO "Using auto-unlock on power-up/resume\n" );
		mtd->flags |= MTD_POWERUP_LOCK;
	}
}

static struct cfi_fixup cfi_fixup_table[] = {
	{ CFI_MFR_ATMEL, CFI_ID_ANY, fixup_convert_atmel_pri },
	{ CFI_MFR_ATMEL, AT49BV640D, fixup_at49bv640dx_lock },
	{ CFI_MFR_ATMEL, AT49BV640DT, fixup_at49bv640dx_lock },
#ifdef CMDSET0001_DISABLE_ERASE_SUSPEND_ON_WRITE
	{ CFI_MFR_ANY, CFI_ID_ANY, fixup_intel_strataflash },
#endif
#ifdef CMDSET0001_DISABLE_WRITE_SUSPEND
	{ CFI_MFR_ANY, CFI_ID_ANY, fixup_no_write_suspend },
#endif
#if !FORCE_WORD_WRITE
	{ CFI_MFR_ANY, CFI_ID_ANY, fixup_use_write_buffers },
#endif
	{ CFI_MFR_ST, 0x00ba, /* M28W320CT */ fixup_st_m28w320ct },
	{ CFI_MFR_ST, 0x00bb, /* M28W320CB */ fixup_st_m28w320cb },
	{ CFI_MFR_INTEL, CFI_ID_ANY, fixup_unlock_powerup_lock },
	{ CFI_MFR_SHARP, CFI_ID_ANY, fixup_unlock_powerup_lock },
	{ CFI_MFR_SHARP, CFI_ID_ANY, fixup_LH28F640BF },
	{ 0, 0, NULL }
};

static struct cfi_fixup jedec_fixup_table[] = {
	{ CFI_MFR_INTEL, I82802AB,   fixup_use_fwh_lock },
	{ CFI_MFR_INTEL, I82802AC,   fixup_use_fwh_lock },
	{ CFI_MFR_ST,    M50LPW080,  fixup_use_fwh_lock },
	{ CFI_MFR_ST,    M50FLW080A, fixup_use_fwh_lock },
	{ CFI_MFR_ST,    M50FLW080B, fixup_use_fwh_lock },
	{ 0, 0, NULL }
};
static struct cfi_fixup fixup_table[] = {
	/* The CFI vendor ids and the JEDEC vendor IDs appear
	 * to be common.  It is like the devices id's are as
	 * well.  This table is to pick all cases where
	 * we know that is the case.
	 */
	{ CFI_MFR_ANY, CFI_ID_ANY, fixup_use_point },
	{ 0, 0, NULL }
};

static void cfi_fixup_major_minor(struct cfi_private *cfi,
						struct cfi_pri_intelext *extp)
{
	if (cfi->mfr == CFI_MFR_INTEL &&
			cfi->id == PF38F4476 && extp->MinorVersion == '3')
		extp->MinorVersion = '1';
}

static int cfi_is_micron_28F00AP30(struct cfi_private *cfi, struct flchip *chip)
{
	/*
	 * Micron(was Numonyx) 1Gbit bottom boot are buggy w.r.t
	 * Erase Supend for their small Erase Blocks(0x8000)
	 */
	if (cfi->mfr == CFI_MFR_INTEL && cfi->id == M28F00AP30)
		return 1;
	return 0;
}

static inline struct cfi_pri_intelext *
read_pri_intelext(struct map_info *map, __u16 adr)
{
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *extp;
	unsigned int extra_size = 0;
	unsigned int extp_size = sizeof(*extp);

 again:
	extp = (struct cfi_pri_intelext *)cfi_read_pri(map, adr, extp_size, "Intel/Sharp");
	if (!extp)
		return NULL;

	cfi_fixup_major_minor(cfi, extp);

	if (extp->MajorVersion != '1' ||
	    (extp->MinorVersion < '0' || extp->MinorVersion > '5')) {
		printk(KERN_ERR "  Unknown Intel/Sharp Extended Query "
		       "version %c.%c.\n",  extp->MajorVersion,
		       extp->MinorVersion);
		kfree(extp);
		return NULL;
	}

	/* Do some byteswapping if necessary */
	extp->FeatureSupport = le32_to_cpu(extp->FeatureSupport);
	extp->BlkStatusRegMask = le16_to_cpu(extp->BlkStatusRegMask);
	extp->ProtRegAddr = le16_to_cpu(extp->ProtRegAddr);

	if (extp->MinorVersion >= '0') {
		extra_size = 0;

		/* Protection Register info */
		if (extp->NumProtectionFields)
			extra_size += (extp->NumProtectionFields - 1) *
				      sizeof(struct cfi_intelext_otpinfo);
	}

	if (extp->MinorVersion >= '1') {
		/* Burst Read info */
		extra_size += 2;
		if (extp_size < sizeof(*extp) + extra_size)
			goto need_more;
		extra_size += extp->extra[extra_size - 1];
	}

	if (extp->MinorVersion >= '3') {
		int nb_parts, i;

		/* Number of hardware-partitions */
		extra_size += 1;
		if (extp_size < sizeof(*extp) + extra_size)
			goto need_more;
		nb_parts = extp->extra[extra_size - 1];

		/* skip the sizeof(partregion) field in CFI 1.4 */
		if (extp->MinorVersion >= '4')
			extra_size += 2;

		for (i = 0; i < nb_parts; i++) {
			struct cfi_intelext_regioninfo *rinfo;
			rinfo = (struct cfi_intelext_regioninfo *)&extp->extra[extra_size];
			extra_size += sizeof(*rinfo);
			if (extp_size < sizeof(*extp) + extra_size)
				goto need_more;
			rinfo->NumIdentPartitions=le16_to_cpu(rinfo->NumIdentPartitions);
			extra_size += (rinfo->NumBlockTypes - 1)
				      * sizeof(struct cfi_intelext_blockinfo);
		}

		if (extp->MinorVersion >= '4')
			extra_size += sizeof(struct cfi_intelext_programming_regioninfo);

		if (extp_size < sizeof(*extp) + extra_size) {
			need_more:
			extp_size = sizeof(*extp) + extra_size;
			kfree(extp);
			if (extp_size > 4096) {
				printk(KERN_ERR
					"%s: cfi_pri_intelext is too fat\n",
					__func__);
				return NULL;
			}
			goto again;
		}
	}

	return extp;
}

struct mtd_info *cfi_cmdset_0001(struct map_info *map, int primary)
{
	struct cfi_private *cfi = map->fldrv_priv;
	struct mtd_info *mtd;
	int i;

	mtd = kzalloc(sizeof(*mtd), GFP_KERNEL);
	if (!mtd)
		return NULL;
	mtd->priv = map;
	mtd->type = MTD_NORFLASH;

	/* Fill in the default mtd operations */
	mtd->_erase   = cfi_intelext_erase_varsize;
	mtd->_read    = cfi_intelext_read;
	mtd->_write   = cfi_intelext_write_words;
	mtd->_sync    = cfi_intelext_sync;
	mtd->_lock    = cfi_intelext_lock;
	mtd->_unlock  = cfi_intelext_unlock;
	mtd->_is_locked = cfi_intelext_is_locked;
	mtd->_suspend = cfi_intelext_suspend;
	mtd->_resume  = cfi_intelext_resume;
	mtd->flags   = MTD_CAP_NORFLASH;
	mtd->name    = map->name;
	mtd->writesize = 1;
	mtd->writebufsize = cfi_interleave(cfi) << cfi->cfiq->MaxBufWriteSize;

	mtd->reboot_notifier.notifier_call = cfi_intelext_reboot;

	if (cfi->cfi_mode == CFI_MODE_CFI) {
		/*
		 * It's a real CFI chip, not one for which the probe
		 * routine faked a CFI structure. So we read the feature
		 * table from it.
		 */
		__u16 adr = primary?cfi->cfiq->P_ADR:cfi->cfiq->A_ADR;
		struct cfi_pri_intelext *extp;

		extp = read_pri_intelext(map, adr);
		if (!extp) {
			kfree(mtd);
			return NULL;
		}

		/* Install our own private info structure */
		cfi->cmdset_priv = extp;

		cfi_fixup(mtd, cfi_fixup_table);

#ifdef DEBUG_CFI_FEATURES
		/* Tell the user about it in lots of lovely detail */
		cfi_tell_features(extp);
#endif

		if(extp->SuspendCmdSupport & 1) {
			printk(KERN_NOTICE "cfi_cmdset_0001: Erase suspend on write enabled\n");
		}
	}
	else if (cfi->cfi_mode == CFI_MODE_JEDEC) {
		/* Apply jedec specific fixups */
		cfi_fixup(mtd, jedec_fixup_table);
	}
	/* Apply generic fixups */
	cfi_fixup(mtd, fixup_table);

	for (i=0; i< cfi->numchips; i++) {
		if (cfi->cfiq->WordWriteTimeoutTyp)
			cfi->chips[i].word_write_time =
				1<<cfi->cfiq->WordWriteTimeoutTyp;
		else
			cfi->chips[i].word_write_time = 50000;

		if (cfi->cfiq->BufWriteTimeoutTyp)
			cfi->chips[i].buffer_write_time =
				1<<cfi->cfiq->BufWriteTimeoutTyp;
		/* No default; if it isn't specified, we won't use it */

		if (cfi->cfiq->BlockEraseTimeoutTyp)
			cfi->chips[i].erase_time =
				1000<<cfi->cfiq->BlockEraseTimeoutTyp;
		else
			cfi->chips[i].erase_time = 2000000;

		if (cfi->cfiq->WordWriteTimeoutTyp &&
		    cfi->cfiq->WordWriteTimeoutMax)
			cfi->chips[i].word_write_time_max =
				1<<(cfi->cfiq->WordWriteTimeoutTyp +
				    cfi->cfiq->WordWriteTimeoutMax);
		else
			cfi->chips[i].word_write_time_max = 50000 * 8;

		if (cfi->cfiq->BufWriteTimeoutTyp &&
		    cfi->cfiq->BufWriteTimeoutMax)
			cfi->chips[i].buffer_write_time_max =
				1<<(cfi->cfiq->BufWriteTimeoutTyp +
				    cfi->cfiq->BufWriteTimeoutMax);

		if (cfi->cfiq->BlockEraseTimeoutTyp &&
		    cfi->cfiq->BlockEraseTimeoutMax)
			cfi->chips[i].erase_time_max =
				1000<<(cfi->cfiq->BlockEraseTimeoutTyp +
				       cfi->cfiq->BlockEraseTimeoutMax);
		else
			cfi->chips[i].erase_time_max = 2000000 * 8;

		cfi->chips[i].ref_point_counter = 0;
		init_waitqueue_head(&(cfi->chips[i].wq));
	}

	map->fldrv = &cfi_intelext_chipdrv;

	return cfi_intelext_setup(mtd);
}
struct mtd_info *cfi_cmdset_0003(struct map_info *map, int primary) __attribute__((alias("cfi_cmdset_0001")));
struct mtd_info *cfi_cmdset_0200(struct map_info *map, int primary) __attribute__((alias("cfi_cmdset_0001")));
EXPORT_SYMBOL_GPL(cfi_cmdset_0001);
EXPORT_SYMBOL_GPL(cfi_cmdset_0003);
EXPORT_SYMBOL_GPL(cfi_cmdset_0200);

static struct mtd_info *cfi_intelext_setup(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	unsigned long offset = 0;
	int i,j;
	unsigned long devsize = (1<<cfi->cfiq->DevSize) * cfi->interleave;

	//printk(KERN_DEBUG "number of CFI chips: %d\n", cfi->numchips);

	mtd->size = devsize * cfi->numchips;

	mtd->numeraseregions = cfi->cfiq->NumEraseRegions * cfi->numchips;
	mtd->eraseregions = kcalloc(mtd->numeraseregions,
				    sizeof(struct mtd_erase_region_info),
				    GFP_KERNEL);
	if (!mtd->eraseregions)
		goto setup_err;

	for (i=0; i<cfi->cfiq->NumEraseRegions; i++) {
		unsigned long ernum, ersize;
		ersize = ((cfi->cfiq->EraseRegionInfo[i] >> 8) & ~0xff) * cfi->interleave;
		ernum = (cfi->cfiq->EraseRegionInfo[i] & 0xffff) + 1;

		if (mtd->erasesize < ersize) {
			mtd->erasesize = ersize;
		}
		for (j=0; j<cfi->numchips; j++) {
			mtd->eraseregions[(j*cfi->cfiq->NumEraseRegions)+i].offset = (j*devsize)+offset;
			mtd->eraseregions[(j*cfi->cfiq->NumEraseRegions)+i].erasesize = ersize;
			mtd->eraseregions[(j*cfi->cfiq->NumEraseRegions)+i].numblocks = ernum;
			mtd->eraseregions[(j*cfi->cfiq->NumEraseRegions)+i].lockmap = kmalloc(ernum / 8 + 1, GFP_KERNEL);
			if (!mtd->eraseregions[(j*cfi->cfiq->NumEraseRegions)+i].lockmap)
				goto setup_err;
		}
		offset += (ersize * ernum);
	}

	if (offset != devsize) {
		/* Argh */
		printk(KERN_WARNING "Sum of regions (%lx) != total size of set of interleaved chips (%lx)\n", offset, devsize);
		goto setup_err;
	}

	for (i=0; i<mtd->numeraseregions;i++){
		printk(KERN_DEBUG "erase region %d: offset=0x%llx,size=0x%x,blocks=%d\n",
		       i,(unsigned long long)mtd->eraseregions[i].offset,
		       mtd->eraseregions[i].erasesize,
		       mtd->eraseregions[i].numblocks);
	}

#ifdef CONFIG_MTD_OTP
	mtd->_read_fact_prot_reg = cfi_intelext_read_fact_prot_reg;
	mtd->_read_user_prot_reg = cfi_intelext_read_user_prot_reg;
	mtd->_write_user_prot_reg = cfi_intelext_write_user_prot_reg;
	mtd->_lock_user_prot_reg = cfi_intelext_lock_user_prot_reg;
	mtd->_get_fact_prot_info = cfi_intelext_get_fact_prot_info;
	mtd->_get_user_prot_info = cfi_intelext_get_user_prot_info;
#endif

	/* This function has the potential to distort the reality
	   a bit and therefore should be called last. */
	if (cfi_intelext_partition_fixup(mtd, &cfi) != 0)
		goto setup_err;

	__module_get(THIS_MODULE);
	register_reboot_notifier(&mtd->reboot_notifier);
	return mtd;

 setup_err:
	if (mtd->eraseregions)
		for (i=0; i<cfi->cfiq->NumEraseRegions; i++)
			for (j=0; j<cfi->numchips; j++)
				kfree(mtd->eraseregions[(j*cfi->cfiq->NumEraseRegions)+i].lockmap);
	kfree(mtd->eraseregions);
	kfree(mtd);
	kfree(cfi->cmdset_priv);
	return NULL;
}

static int cfi_intelext_partition_fixup(struct mtd_info *mtd,
					struct cfi_private **pcfi)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = *pcfi;
	struct cfi_pri_intelext *extp = cfi->cmdset_priv;

	/*
	 * Probing of multi-partition flash chips.
	 *
	 * To support multiple partitions when available, we simply arrange
	 * for each of them to have their own flchip structure even if they
	 * are on the same physical chip.  This means completely recreating
	 * a new cfi_private structure right here which is a blatent code
	 * layering violation, but this is still the least intrusive
	 * arrangement at this point. This can be rearranged in the future
	 * if someone feels motivated enough.  --nico
	 */
	if (extp && extp->MajorVersion == '1' && extp->MinorVersion >= '3'
	    && extp->FeatureSupport & (1 << 9)) {
		int offs = 0;
		struct cfi_private *newcfi;
		struct flchip *chip;
		struct flchip_shared *shared;
		int numregions, numparts, partshift, numvirtchips, i, j;

		/* Protection Register info */
		if (extp->NumProtectionFields)
			offs = (extp->NumProtectionFields - 1) *
			       sizeof(struct cfi_intelext_otpinfo);

		/* Burst Read info */
		offs += extp->extra[offs+1]+2;

		/* Number of partition regions */
		numregions = extp->extra[offs];
		offs += 1;

		/* skip the sizeof(partregion) field in CFI 1.4 */
		if (extp->MinorVersion >= '4')
			offs += 2;

		/* Number of hardware partitions */
		numparts = 0;
		for (i = 0; i < numregions; i++) {
			struct cfi_intelext_regioninfo *rinfo;
			rinfo = (struct cfi_intelext_regioninfo *)&extp->extra[offs];
			numparts += rinfo->NumIdentPartitions;
			offs += sizeof(*rinfo)
				+ (rinfo->NumBlockTypes - 1) *
				  sizeof(struct cfi_intelext_blockinfo);
		}

		if (!numparts)
			numparts = 1;

		/* Programming Region info */
		if (extp->MinorVersion >= '4') {
			struct cfi_intelext_programming_regioninfo *prinfo;
			prinfo = (struct cfi_intelext_programming_regioninfo *)&extp->extra[offs];
			mtd->writesize = cfi->interleave << prinfo->ProgRegShift;
			mtd->flags &= ~MTD_BIT_WRITEABLE;
			printk(KERN_DEBUG "%s: program region size/ctrl_valid/ctrl_inval = %d/%d/%d\n",
			       map->name, mtd->writesize,
			       cfi->interleave * prinfo->ControlValid,
			       cfi->interleave * prinfo->ControlInvalid);
		}

		/*
		 * All functions below currently rely on all chips having
		 * the same geometry so we'll just assume that all hardware
		 * partitions are of the same size too.
		 */
		partshift = cfi->chipshift - __ffs(numparts);

		if ((1 << partshift) < mtd->erasesize) {
			printk( KERN_ERR
				"%s: bad number of hw partitions (%d)\n",
				__func__, numparts);
			return -EINVAL;
		}

		numvirtchips = cfi->numchips * numparts;
		newcfi = kmalloc(struct_size(newcfi, chips, numvirtchips),
				 GFP_KERNEL);
		if (!newcfi)
			return -ENOMEM;
		shared = kmalloc_array(cfi->numchips,
				       sizeof(struct flchip_shared),
				       GFP_KERNEL);
		if (!shared) {
			kfree(newcfi);
			return -ENOMEM;
		}
		memcpy(newcfi, cfi, sizeof(struct cfi_private));
		newcfi->numchips = numvirtchips;
		newcfi->chipshift = partshift;

		chip = &newcfi->chips[0];
		for (i = 0; i < cfi->numchips; i++) {
			shared[i].writing = shared[i].erasing = NULL;
			mutex_init(&shared[i].lock);
			for (j = 0; j < numparts; j++) {
				*chip = cfi->chips[i];
				chip->start += j << partshift;
				chip->priv = &shared[i];
				/* those should be reset too since
				   they create memory references. */
				init_waitqueue_head(&chip->wq);
				mutex_init(&chip->mutex);
				chip++;
			}
		}

		printk(KERN_DEBUG "%s: %d set(s) of %d interleaved chips "
				  "--> %d partitions of %d KiB\n",
				  map->name, cfi->numchips, cfi->interleave,
				  newcfi->numchips, 1<<(newcfi->chipshift-10));

		map->fldrv_priv = newcfi;
		*pcfi = newcfi;
		kfree(cfi);
	}

	return 0;
}

/*
 *  *********** CHIP ACCESS FUNCTIONS ***********
 */
static int chip_ready (struct map_info *map, struct flchip *chip, unsigned long adr, int mode)
{
	DECLARE_WAITQUEUE(wait, current);
	struct cfi_private *cfi = map->fldrv_priv;
	map_word status, status_OK = CMD(0x80), status_PWS = CMD(0x01);
	struct cfi_pri_intelext *cfip = cfi->cmdset_priv;
	unsigned long timeo = jiffies + HZ;

	/* Prevent setting state FL_SYNCING for chip in suspended state. */
	if (mode == FL_SYNCING && chip->oldstate != FL_READY)
		goto sleep;

	switch (chip->state) {

	case FL_STATUS:
		for (;;) {
			status = map_read(map, adr);
			if (map_word_andequal(map, status, status_OK, status_OK))
				break;

			/* At this point we're fine with write operations
			   in other partitions as they don't conflict. */
			if (chip->priv && map_word_andequal(map, status, status_PWS, status_PWS))
				break;

			mutex_unlock(&chip->mutex);
			cfi_udelay(1);
			mutex_lock(&chip->mutex);
			/* Someone else might have been playing with it. */
			return -EAGAIN;
		}
		fallthrough;
	case FL_READY:
	case FL_CFI_QUERY:
	case FL_JEDEC_QUERY:
		return 0;

	case FL_ERASING:
		if (!cfip ||
		    !(cfip->FeatureSupport & 2) ||
		    !(mode == FL_READY || mode == FL_POINT ||
		     (mode == FL_WRITING && (cfip->SuspendCmdSupport & 1))))
			goto sleep;

		/* Do not allow suspend iff read/write to EB address */
		if ((adr & chip->in_progress_block_mask) ==
		    chip->in_progress_block_addr)
			goto sleep;

		/* do not suspend small EBs, buggy Micron Chips */
		if (cfi_is_micron_28F00AP30(cfi, chip) &&
		    (chip->in_progress_block_mask == ~(0x8000-1)))
			goto sleep;

		/* Erase suspend */
		map_write(map, CMD(0xB0), chip->in_progress_block_addr);

		/* If the flash has finished erasing, then 'erase suspend'
		 * appears to make some (28F320) flash devices switch to
		 * 'read' mode.  Make sure that we switch to 'read status'
		 * mode so we get the right data. --rmk
		 */
		map_write(map, CMD(0x70), chip->in_progress_block_addr);
		chip->oldstate = FL_ERASING;
		chip->state = FL_ERASE_SUSPENDING;
		chip->erase_suspended = 1;
		for (;;) {
			status = map_read(map, chip->in_progress_block_addr);
			if (map_word_andequal(map, status, status_OK, status_OK))
			        break;

			if (time_after(jiffies, timeo)) {
				/* Urgh. Resume and pretend we weren't here.
				 * Make sure we're in 'read status' mode if it had finished */
				put_chip(map, chip, adr);
				printk(KERN_ERR "%s: Chip not ready after erase "
				       "suspended: status = 0x%lx\n", map->name, status.x[0]);
				return -EIO;
			}

			mutex_unlock(&chip->mutex);
			cfi_udelay(1);
			mutex_lock(&chip->mutex);
			/* Nobody will touch it while it's in state FL_ERASE_SUSPENDING.
			   So we can just loop here. */
		}
		chip->state = FL_STATUS;
		return 0;

	case FL_XIP_WHILE_ERASING:
		if (mode != FL_READY && mode != FL_POINT &&
		    (mode != FL_WRITING || !cfip || !(cfip->SuspendCmdSupport&1)))
			goto sleep;
		chip->oldstate = chip->state;
		chip->state = FL_READY;
		return 0;

	case FL_SHUTDOWN:
		/* The machine is rebooting now,so no one can get chip anymore */
		return -EIO;
	case FL_POINT:
		/* Only if there's no operation suspended... */
		if (mode == FL_READY && chip->oldstate == FL_READY)
			return 0;
		fallthrough;
	default:
	sleep:
		set_current_state(TASK_UNINTERRUPTIBLE);
		add_wait_queue(&chip->wq, &wait);
		mutex_unlock(&chip->mutex);
		schedule();
		remove_wait_queue(&chip->wq, &wait);
		mutex_lock(&chip->mutex);
		return -EAGAIN;
	}
}

static int get_chip(struct map_info *map, struct flchip *chip, unsigned long adr, int mode)
{
	int ret;
	DECLARE_WAITQUEUE(wait, current);

 retry:
	if (chip->priv &&
	    (mode == FL_WRITING || mode == FL_ERASING || mode == FL_OTP_WRITE
	    || mode == FL_SHUTDOWN) && chip->state != FL_SYNCING) {
		/*
		 * OK. We have possibility for contention on the write/erase
		 * operations which are global to the real chip and not per
		 * partition.  So let's fight it over in the partition which
		 * currently has authority on the operation.
		 *
		 * The rules are as follows:
		 *
		 * - any write operation must own shared->writing.
		 *
		 * - any erase operation must own _both_ shared->writing and
		 *   shared->erasing.
		 *
		 * - contention arbitration is handled in the owner's context.
		 *
		 * The 'shared' struct can be read and/or written only when
		 * its lock is taken.
		 */
		struct flchip_shared *shared = chip->priv;
		struct flchip *contender;
		mutex_lock(&shared->lock);
		contender = shared->writing;
		if (contender && contender != chip) {
			/*
			 * The engine to perform desired operation on this
			 * partition is already in use by someone else.
			 * Let's fight over it in the context of the chip
			 * currently using it.  If it is possible to suspend,
			 * that other partition will do just that, otherwise
			 * it'll happily send us to sleep.  In any case, when
			 * get_chip returns success we're clear to go ahead.
			 */
			ret = mutex_trylock(&contender->mutex);
			mutex_unlock(&shared->lock);
			if (!ret)
				goto retry;
			mutex_unlock(&chip->mutex);
			ret = chip_ready(map, contender, contender->start, mode);
			mutex_lock(&chip->mutex);

			if (ret == -EAGAIN) {
				mutex_unlock(&contender->mutex);
				goto retry;
			}
			if (ret) {
				mutex_unlock(&contender->mutex);
				return ret;
			}
			mutex_lock(&shared->lock);

			/* We should not own chip if it is already
			 * in FL_SYNCING state. Put contender and retry. */
			if (chip->state == FL_SYNCING) {
				put_chip(map, contender, contender->start);
				mutex_unlock(&contender->mutex);
				goto retry;
			}
			mutex_unlock(&contender->mutex);
		}

		/* Check if we already have suspended erase
		 * on this chip. Sleep. */
		if (mode == FL_ERASING && shared->erasing
		    && shared->erasing->oldstate == FL_ERASING) {
			mutex_unlock(&shared->lock);
			set_current_state(TASK_UNINTERRUPTIBLE);
			add_wait_queue(&chip->wq, &wait);
			mutex_unlock(&chip->mutex);
			schedule();
			remove_wait_queue(&chip->wq, &wait);
			mutex_lock(&chip->mutex);
			goto retry;
		}

		/* We now own it */
		shared->writing = chip;
		if (mode == FL_ERASING)
			shared->erasing = chip;
		mutex_unlock(&shared->lock);
	}
	ret = chip_ready(map, chip, adr, mode);
	if (ret == -EAGAIN)
		goto retry;

	return ret;
}

static void put_chip(struct map_info *map, struct flchip *chip, unsigned long adr)
{
	struct cfi_private *cfi = map->fldrv_priv;

	if (chip->priv) {
		struct flchip_shared *shared = chip->priv;
		mutex_lock(&shared->lock);
		if (shared->writing == chip && chip->oldstate == FL_READY) {
			/* We own the ability to write, but we're done */
			shared->writing = shared->erasing;
			if (shared->writing && shared->writing != chip) {
				/* give back ownership to who we loaned it from */
				struct flchip *loaner = shared->writing;
				mutex_lock(&loaner->mutex);
				mutex_unlock(&shared->lock);
				mutex_unlock(&chip->mutex);
				put_chip(map, loaner, loaner->start);
				mutex_lock(&chip->mutex);
				mutex_unlock(&loaner->mutex);
				wake_up(&chip->wq);
				return;
			}
			shared->erasing = NULL;
			shared->writing = NULL;
		} else if (shared->erasing == chip && shared->writing != chip) {
			/*
			 * We own the ability to erase without the ability
			 * to write, which means the erase was suspended
			 * and some other partition is currently writing.
			 * Don't let the switch below mess things up since
			 * we don't have ownership to resume anything.
			 */
			mutex_unlock(&shared->lock);
			wake_up(&chip->wq);
			return;
		}
		mutex_unlock(&shared->lock);
	}

	switch(chip->oldstate) {
	case FL_ERASING:
		/* What if one interleaved chip has finished and the
		   other hasn't? The old code would leave the finished
		   one in READY mode. That's bad, and caused -EROFS
		   errors to be returned from do_erase_oneblock because
		   that's the only bit it checked for at the time.
		   As the state machine appears to explicitly allow
		   sending the 0x70 (Read Status) command to an erasing
		   chip and expecting it to be ignored, that's what we
		   do. */
		map_write(map, CMD(0xd0), chip->in_progress_block_addr);
		map_write(map, CMD(0x70), chip->in_progress_block_addr);
		chip->oldstate = FL_READY;
		chip->state = FL_ERASING;
		break;

	case FL_XIP_WHILE_ERASING:
		chip->state = chip->oldstate;
		chip->oldstate = FL_READY;
		break;

	case FL_READY:
	case FL_STATUS:
	case FL_JEDEC_QUERY:
		break;
	default:
		printk(KERN_ERR "%s: put_chip() called with oldstate %d!!\n", map->name, chip->oldstate);
	}
	wake_up(&chip->wq);
}

#ifdef CONFIG_MTD_XIP

/*
 * No interrupt what so ever can be serviced while the flash isn't in array
 * mode.  This is ensured by the xip_disable() and xip_enable() functions
 * enclosing any code path where the flash is known not to be in array mode.
 * And within a XIP disabled code path, only functions marked with __xipram
 * may be called and nothing else (it's a good thing to inspect generated
 * assembly to make sure inline functions were actually inlined and that gcc
 * didn't emit calls to its own support functions). Also configuring MTD CFI
 * support to a single buswidth and a single interleave is also recommended.
 */

static void xip_disable(struct map_info *map, struct flchip *chip,
			unsigned long adr)
{
	/* TODO: chips with no XIP use should ignore and return */
	(void) map_read(map, adr); /* ensure mmu mapping is up to date */
	local_irq_disable();
}

static void __xipram xip_enable(struct map_info *map, struct flchip *chip,
				unsigned long adr)
{
	struct cfi_private *cfi = map->fldrv_priv;
	if (chip->state != FL_POINT && chip->state != FL_READY) {
		map_write(map, CMD(0xff), adr);
		chip->state = FL_READY;
	}
	(void) map_read(map, adr);
	xip_iprefetch();
	local_irq_enable();
}

/*
 * When a delay is required for the flash operation to complete, the
 * xip_wait_for_operation() function is polling for both the given timeout
 * and pending (but still masked) hardware interrupts.  Whenever there is an
 * interrupt pending then the flash erase or write operation is suspended,
 * array mode restored and interrupts unmasked.  Task scheduling might also
 * happen at that point.  The CPU eventually returns from the interrupt or
 * the call to schedule() and the suspended flash operation is resumed for
 * the remaining of the delay period.
 *
 * Warning: this function _will_ fool interrupt latency tracing tools.
 */

static int __xipram xip_wait_for_operation(
		struct map_info *map, struct flchip *chip,
		unsigned long adr, unsigned int chip_op_time_max)
{
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *cfip = cfi->cmdset_priv;
	map_word status, OK = CMD(0x80);
	unsigned long usec, suspended, start, done;
	flstate_t oldstate, newstate;

       	start = xip_currtime();
	usec = chip_op_time_max;
	if (usec == 0)
		usec = 500000;
	done = 0;

	do {
		cpu_relax();
		if (xip_irqpending() && cfip &&
		    ((chip->state == FL_ERASING && (cfip->FeatureSupport&2)) ||
		     (chip->state == FL_WRITING && (cfip->FeatureSupport&4))) &&
		    (cfi_interleave_is_1(cfi) || chip->oldstate == FL_READY)) {
			/*
			 * Let's suspend the erase or write operation when
			 * supported.  Note that we currently don't try to
			 * suspend interleaved chips if there is already
			 * another operation suspended (imagine what happens
			 * when one chip was already done with the current
			 * operation while another chip suspended it, then
			 * we resume the whole thing at once).  Yes, it
			 * can happen!
			 */
			usec -= done;
			map_write(map, CMD(0xb0), adr);
			map_write(map, CMD(0x70), adr);
			suspended = xip_currtime();
			do {
				if (xip_elapsed_since(suspended) > 100000) {
					/*
					 * The chip doesn't want to suspend
					 * after waiting for 100 msecs.
					 * This is a critical error but there
					 * is not much we can do here.
					 */
					return -EIO;
				}
				status = map_read(map, adr);
			} while (!map_word_andequal(map, status, OK, OK));

			/* Suspend succeeded */
			oldstate = chip->state;
			if (oldstate == FL_ERASING) {
				if (!map_word_bitsset(map, status, CMD(0x40)))
					break;
				newstate = FL_XIP_WHILE_ERASING;
				chip->erase_suspended = 1;
			} else {
				if (!map_word_bitsset(map, status, CMD(0x04)))
					break;
				newstate = FL_XIP_WHILE_WRITING;
				chip->write_suspended = 1;
			}
			chip->state = newstate;
			map_write(map, CMD(0xff), adr);
			(void) map_read(map, adr);
			xip_iprefetch();
			local_irq_enable();
			mutex_unlock(&chip->mutex);
			xip_iprefetch();
			cond_resched();

			/*
			 * We're back.  However someone else might have
			 * decided to go write to the chip if we are in
			 * a suspended erase state.  If so let's wait
			 * until it's done.
			 */
			mutex_lock(&chip->mutex);
			while (chip->state != newstate) {
				DECLARE_WAITQUEUE(wait, current);
				set_current_state(TASK_UNINTERRUPTIBLE);
				add_wait_queue(&chip->wq, &wait);
				mutex_unlock(&chip->mutex);
				schedule();
				remove_wait_queue(&chip->wq, &wait);
				mutex_lock(&chip->mutex);
			}
			/* Disallow XIP again */
			local_irq_disable();

			/* Resume the write or erase operation */
			map_write(map, CMD(0xd0), adr);
			map_write(map, CMD(0x70), adr);
			chip->state = oldstate;
			start = xip_currtime();
		} else if (usec >= 1000000/HZ) {
			/*
			 * Try to save on CPU power when waiting delay
			 * is at least a system timer tick period.
			 * No need to be extremely accurate here.
			 */
			xip_cpu_idle();
		}
		status = map_read(map, adr);
		done = xip_elapsed_since(start);
	} while (!map_word_andequal(map, status, OK, OK)
		 && done < usec);

	return (done >= usec) ? -ETIME : 0;
}

/*
 * The INVALIDATE_CACHED_RANGE() macro is normally used in parallel while
 * the flash is actively programming or erasing since we have to poll for
 * the operation to complete anyway.  We can't do that in a generic way with
 * a XIP setup so do it before the actual flash operation in this case
 * and stub it out from INVAL_CACHE_AND_WAIT.
 */
#define XIP_INVAL_CACHED_RANGE(map, from, size)  \
	INVALIDATE_CACHED_RANGE(map, from, size)

#define INVAL_CACHE_AND_WAIT(map, chip, cmd_adr, inval_adr, inval_len, usec, usec_max) \
	xip_wait_for_operation(map, chip, cmd_adr, usec_max)

#else

#define xip_disable(map, chip, adr)
#define xip_enable(map, chip, adr)
#define XIP_INVAL_CACHED_RANGE(x...)
#define INVAL_CACHE_AND_WAIT inval_cache_and_wait_for_operation

static int inval_cache_and_wait_for_operation(
		struct map_info *map, struct flchip *chip,
		unsigned long cmd_adr, unsigned long inval_adr, int inval_len,
		unsigned int chip_op_time, unsigned int chip_op_time_max)
{
	struct cfi_private *cfi = map->fldrv_priv;
	map_word status, status_OK = CMD(0x80);
	int chip_state = chip->state;
	unsigned int timeo, sleep_time, reset_timeo;

	mutex_unlock(&chip->mutex);
	if (inval_len)
		INVALIDATE_CACHED_RANGE(map, inval_adr, inval_len);
	mutex_lock(&chip->mutex);

	timeo = chip_op_time_max;
	if (!timeo)
		timeo = 500000;
	reset_timeo = timeo;
	sleep_time = chip_op_time / 2;

	for (;;) {
		if (chip->state != chip_state) {
			/* Someone's suspended the operation: sleep */
			DECLARE_WAITQUEUE(wait, current);
			set_current_state(TASK_UNINTERRUPTIBLE);
			add_wait_queue(&chip->wq, &wait);
			mutex_unlock(&chip->mutex);
			schedule();
			remove_wait_queue(&chip->wq, &wait);
			mutex_lock(&chip->mutex);
			continue;
		}

		status = map_read(map, cmd_adr);
		if (map_word_andequal(map, status, status_OK, status_OK))
			break;

		if (chip->erase_suspended && chip_state == FL_ERASING)  {
			/* Erase suspend occurred while sleep: reset timeout */
			timeo = reset_timeo;
			chip->erase_suspended = 0;
		}
		if (chip->write_suspended && chip_state == FL_WRITING)  {
			/* Write suspend occurred while sleep: reset timeout */
			timeo = reset_timeo;
			chip->write_suspended = 0;
		}
		if (!timeo) {
			map_write(map, CMD(0x70), cmd_adr);
			chip->state = FL_STATUS;
			return -ETIME;
		}

		/* OK Still waiting. Drop the lock, wait a while and retry. */
		mutex_unlock(&chip->mutex);
		if (sleep_time >= 1000000/HZ) {
			/*
			 * Half of the normal delay still remaining
			 * can be performed with a sleeping delay instead
			 * of busy waiting.
			 */
			msleep(sleep_time/1000);
			timeo -= sleep_time;
			sleep_time = 1000000/HZ;
		} else {
			udelay(1);
			cond_resched();
			timeo--;
		}
		mutex_lock(&chip->mutex);
	}

	/* Done and happy. */
 	chip->state = FL_STATUS;
	return 0;
}

#endif

#define WAIT_TIMEOUT(map, chip, adr, udelay, udelay_max) \
	INVAL_CACHE_AND_WAIT(map, chip, adr, 0, 0, udelay, udelay_max);


static int do_point_onechip (struct map_info *map, struct flchip *chip, loff_t adr, size_t len)
{
	unsigned long cmd_addr;
	struct cfi_private *cfi = map->fldrv_priv;
	int ret;

	adr += chip->start;

	/* Ensure cmd read/writes are aligned. */
	cmd_addr = adr & ~(map_bankwidth(map)-1);

	mutex_lock(&chip->mutex);

	ret = get_chip(map, chip, cmd_addr, FL_POINT);

	if (!ret) {
		if (chip->state != FL_POINT && chip->state != FL_READY)
			map_write(map, CMD(0xff), cmd_addr);

		chip->state = FL_POINT;
		chip->ref_point_counter++;
	}
	mutex_unlock(&chip->mutex);

	return ret;
}

static int cfi_intelext_point(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, void **virt, resource_size_t *phys)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	unsigned long ofs, last_end = 0;
	int chipnum;
	int ret;

	if (!map->virt)
		return -EINVAL;

	/* Now lock the chip(s) to POINT state */

	/* ofs: offset within the first chip that the first read should start */
	chipnum = (from >> cfi->chipshift);
	ofs = from - (chipnum << cfi->chipshift);

	*virt = map->virt + cfi->chips[chipnum].start + ofs;
	if (phys)
		*phys = map->phys + cfi->chips[chipnum].start + ofs;

	while (len) {
		unsigned long thislen;

		if (chipnum >= cfi->numchips)
			break;

		/* We cannot point across chips that are virtually disjoint */
		if (!last_end)
			last_end = cfi->chips[chipnum].start;
		else if (cfi->chips[chipnum].start != last_end)
			break;

		if ((len + ofs -1) >> cfi->chipshift)
			thislen = (1<<cfi->chipshift) - ofs;
		else
			thislen = len;

		ret = do_point_onechip(map, &cfi->chips[chipnum], ofs, thislen);
		if (ret)
			break;

		*retlen += thislen;
		len -= thislen;

		ofs = 0;
		last_end += 1 << cfi->chipshift;
		chipnum++;
	}
	return 0;
}

static int cfi_intelext_unpoint(struct mtd_info *mtd, loff_t from, size_t len)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	unsigned long ofs;
	int chipnum, err = 0;

	/* Now unlock the chip(s) POINT state */

	/* ofs: offset within the first chip that the first read should start */
	chipnum = (from >> cfi->chipshift);
	ofs = from - (chipnum <<  cfi->chipshift);

	while (len && !err) {
		unsigned long thislen;
		struct flchip *chip;

		chip = &cfi->chips[chipnum];
		if (chipnum >= cfi->numchips)
			break;

		if ((len + ofs -1) >> cfi->chipshift)
			thislen = (1<<cfi->chipshift) - ofs;
		else
			thislen = len;

		mutex_lock(&chip->mutex);
		if (chip->state == FL_POINT) {
			chip->ref_point_counter--;
			if(chip->ref_point_counter == 0)
				chip->state = FL_READY;
		} else {
			printk(KERN_ERR "%s: Error: unpoint called on non pointed region\n", map->name);
			err = -EINVAL;
		}

		put_chip(map, chip, chip->start);
		mutex_unlock(&chip->mutex);

		len -= thislen;
		ofs = 0;
		chipnum++;
	}

	return err;
}

static inline int do_read_onechip(struct map_info *map, struct flchip *chip, loff_t adr, size_t len, u_char *buf)
{
	unsigned long cmd_addr;
	struct cfi_private *cfi = map->fldrv_priv;
	int ret;

	adr += chip->start;

	/* Ensure cmd read/writes are aligned. */
	cmd_addr = adr & ~(map_bankwidth(map)-1);

	mutex_lock(&chip->mutex);
	ret = get_chip(map, chip, cmd_addr, FL_READY);
	if (ret) {
		mutex_unlock(&chip->mutex);
		return ret;
	}

	if (chip->state != FL_POINT && chip->state != FL_READY) {
		map_write(map, CMD(0xff), cmd_addr);

		chip->state = FL_READY;
	}

	map_copy_from(map, buf, adr, len);

	put_chip(map, chip, cmd_addr);

	mutex_unlock(&chip->mutex);
	return 0;
}

static int cfi_intelext_read (struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	unsigned long ofs;
	int chipnum;
	int ret = 0;

	/* ofs: offset within the first chip that the first read should start */
	chipnum = (from >> cfi->chipshift);
	ofs = from - (chipnum <<  cfi->chipshift);

	while (len) {
		unsigned long thislen;

		if (chipnum >= cfi->numchips)
			break;

		if ((len + ofs -1) >> cfi->chipshift)
			thislen = (1<<cfi->chipshift) - ofs;
		else
			thislen = len;

		ret = do_read_onechip(map, &cfi->chips[chipnum], ofs, thislen, buf);
		if (ret)
			break;

		*retlen += thislen;
		len -= thislen;
		buf += thislen;

		ofs = 0;
		chipnum++;
	}
	return ret;
}

static int __xipram do_write_oneword(struct map_info *map, struct flchip *chip,
				     unsigned long adr, map_word datum, int mode)
{
	struct cfi_private *cfi = map->fldrv_priv;
	map_word status, write_cmd;
	int ret;

	adr += chip->start;

	switch (mode) {
	case FL_WRITING:
		write_cmd = (cfi->cfiq->P_ID != P_ID_INTEL_PERFORMANCE) ? CMD(0x40) : CMD(0x41);
		break;
	case FL_OTP_WRITE:
		write_cmd = CMD(0xc0);
		break;
	default:
		return -EINVAL;
	}

	mutex_lock(&chip->mutex);
	ret = get_chip(map, chip, adr, mode);
	if (ret) {
		mutex_unlock(&chip->mutex);
		return ret;
	}

	XIP_INVAL_CACHED_RANGE(map, adr, map_bankwidth(map));
	ENABLE_VPP(map);
	xip_disable(map, chip, adr);
	map_write(map, write_cmd, adr);
	map_write(map, datum, adr);
	chip->state = mode;

	ret = INVAL_CACHE_AND_WAIT(map, chip, adr,
				   adr, map_bankwidth(map),
				   chip->word_write_time,
				   chip->word_write_time_max);
	if (ret) {
		xip_enable(map, chip, adr);
		printk(KERN_ERR "%s: word write error (status timeout)\n", map->name);
		goto out;
	}

	/* check for errors */
	status = map_read(map, adr);
	if (map_word_bitsset(map, status, CMD(0x1a))) {
		unsigned long chipstatus = MERGESTATUS(status);

		/* reset status */
		map_write(map, CMD(0x50), adr);
		map_write(map, CMD(0x70), adr);
		xip_enable(map, chip, adr);

		if (chipstatus & 0x02) {
			ret = -EROFS;
		} else if (chipstatus & 0x08) {
			printk(KERN_ERR "%s: word write error (bad VPP)\n", map->name);
			ret = -EIO;
		} else {
			printk(KERN_ERR "%s: word write error (status 0x%lx)\n", map->name, chipstatus);
			ret = -EINVAL;
		}

		goto out;
	}

	xip_enable(map, chip, adr);
 out:	DISABLE_VPP(map);
	put_chip(map, chip, adr);
	mutex_unlock(&chip->mutex);
	return ret;
}


static int cfi_intelext_write_words (struct mtd_info *mtd, loff_t to , size_t len, size_t *retlen, const u_char *buf)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	int ret;
	int chipnum;
	unsigned long ofs;

	chipnum = to >> cfi->chipshift;
	ofs = to  - (chipnum << cfi->chipshift);

	/* If it's not bus-aligned, do the first byte write */
	if (ofs & (map_bankwidth(map)-1)) {
		unsigned long bus_ofs = ofs & ~(map_bankwidth(map)-1);
		int gap = ofs - bus_ofs;
		int n;
		map_word datum;

		n = min_t(int, len, map_bankwidth(map)-gap);
		datum = map_word_ff(map);
		datum = map_word_load_partial(map, datum, buf, gap, n);

		ret = do_write_oneword(map, &cfi->chips[chipnum],
					       bus_ofs, datum, FL_WRITING);
		if (ret)
			return ret;

		len -= n;
		ofs += n;
		buf += n;
		(*retlen) += n;

		if (ofs >> cfi->chipshift) {
			chipnum ++;
			ofs = 0;
			if (chipnum == cfi->numchips)
				return 0;
		}
	}

	while(len >= map_bankwidth(map)) {
		map_word datum = map_word_load(map, buf);

		ret = do_write_oneword(map, &cfi->chips[chipnum],
				       ofs, datum, FL_WRITING);
		if (ret)
			return ret;

		ofs += map_bankwidth(map);
		buf += map_bankwidth(map);
		(*retlen) += map_bankwidth(map);
		len -= map_bankwidth(map);

		if (ofs >> cfi->chipshift) {
			chipnum ++;
			ofs = 0;
			if (chipnum == cfi->numchips)
				return 0;
		}
	}

	if (len & (map_bankwidth(map)-1)) {
		map_word datum;

		datum = map_word_ff(map);
		datum = map_word_load_partial(map, datum, buf, 0, len);

		ret = do_write_oneword(map, &cfi->chips[chipnum],
				       ofs, datum, FL_WRITING);
		if (ret)
			return ret;

		(*retlen) += len;
	}

	return 0;
}


static int __xipram do_write_buffer(struct map_info *map, struct flchip *chip,
				    unsigned long adr, const struct kvec **pvec,
				    unsigned long *pvec_seek, int len)
{
	struct cfi_private *cfi = map->fldrv_priv;
	map_word status, write_cmd, datum;
	unsigned long cmd_adr;
	int ret, wbufsize, word_gap, words;
	const struct kvec *vec;
	unsigned long vec_seek;
	unsigned long initial_adr;
	int initial_len = len;

	wbufsize = cfi_interleave(cfi) << cfi->cfiq->MaxBufWriteSize;
	adr += chip->start;
	initial_adr = adr;
	cmd_adr = adr & ~(wbufsize-1);

	/* Sharp LH28F640BF chips need the first address for the
	 * Page Buffer Program command. See Table 5 of
	 * LH28F320BF, LH28F640BF, LH28F128BF Series (Appendix FUM00701) */
	if (is_LH28F640BF(cfi))
		cmd_adr = adr;

	/* Let's determine this according to the interleave only once */
	write_cmd = (cfi->cfiq->P_ID != P_ID_INTEL_PERFORMANCE) ? CMD(0xe8) : CMD(0xe9);

	mutex_lock(&chip->mutex);
	ret = get_chip(map, chip, cmd_adr, FL_WRITING);
	if (ret) {
		mutex_unlock(&chip->mutex);
		return ret;
	}

	XIP_INVAL_CACHED_RANGE(map, initial_adr, initial_len);
	ENABLE_VPP(map);
	xip_disable(map, chip, cmd_adr);

	/* §4.8 of the 28FxxxJ3A datasheet says "Any time SR.4 and/or SR.5 is set
	   [...], the device will not accept any more Write to Buffer commands".
	   So we must check here and reset those bits if they're set. Otherwise
	   we're just pissing in the wind */
	if (chip->state != FL_STATUS) {
		map_write(map, CMD(0x70), cmd_adr);
		chip->state = FL_STATUS;
	}
	status = map_read(map, cmd_adr);
	if (map_word_bitsset(map, status, CMD(0x30))) {
		xip_enable(map, chip, cmd_adr);
		printk(KERN_WARNING "SR.4 or SR.5 bits set in buffer write (status %lx). Clearing.\n", status.x[0]);
		xip_disable(map, chip, cmd_adr);
		map_write(map, CMD(0x50), cmd_adr);
		map_write(map, CMD(0x70), cmd_adr);
	}

	chip->state = FL_WRITING_TO_BUFFER;
	map_write(map, write_cmd, cmd_adr);
	ret = WAIT_TIMEOUT(map, chip, cmd_adr, 0, 0);
	if (ret) {
		/* Argh. Not ready for write to buffer */
		map_word Xstatus = map_read(map, cmd_adr);
		map_write(map, CMD(0x70), cmd_adr);
		chip->state = FL_STATUS;
		status = map_read(map, cmd_adr);
		map_write(map, CMD(0x50), cmd_adr);
		map_write(map, CMD(0x70), cmd_adr);
		xip_enable(map, chip, cmd_adr);
		printk(KERN_ERR "%s: Chip not ready for buffer write. Xstatus = %lx, status = %lx\n",
				map->name, Xstatus.x[0], status.x[0]);
		goto out;
	}

	/* Figure out the number of words to write */
	word_gap = (-adr & (map_bankwidth(map)-1));
	words = DIV_ROUND_UP(len - word_gap, map_bankwidth(map));
	if (!word_gap) {
		words--;
	} else {
		word_gap = map_bankwidth(map) - word_gap;
		adr -= word_gap;
		datum = map_word_ff(map);
	}

	/* Write length of data to come */
	map_write(map, CMD(words), cmd_adr );

	/* Write data */
	vec = *pvec;
	vec_seek = *pvec_seek;
	do {
		int n = map_bankwidth(map) - word_gap;
		if (n > vec->iov_len - vec_seek)
			n = vec->iov_len - vec_seek;
		if (n > len)
			n = len;

		if (!word_gap && len < map_bankwidth(map))
			datum = map_word_ff(map);

		datum = map_word_load_partial(map, datum,
					      vec->iov_base + vec_seek,
					      word_gap, n);

		len -= n;
		word_gap += n;
		if (!len || word_gap == map_bankwidth(map)) {
			map_write(map, datum, adr);
			adr += map_bankwidth(map);
			word_gap = 0;
		}

		vec_seek += n;
		if (vec_seek == vec->iov_len) {
			vec++;
			vec_seek = 0;
		}
	} while (len);
	*pvec = vec;
	*pvec_seek = vec_seek;

	/* GO GO GO */
	map_write(map, CMD(0xd0), cmd_adr);
	chip->state = FL_WRITING;

	ret = INVAL_CACHE_AND_WAIT(map, chip, cmd_adr,
				   initial_adr, initial_len,
				   chip->buffer_write_time,
				   chip->buffer_write_time_max);
	if (ret) {
		map_write(map, CMD(0x70), cmd_adr);
		chip->state = FL_STATUS;
		xip_enable(map, chip, cmd_adr);
		printk(KERN_ERR "%s: buffer write error (status timeout)\n", map->name);
		goto out;
	}

	/* check for errors */
	status = map_read(map, cmd_adr);
	if (map_word_bitsset(map, status, CMD(0x1a))) {
		unsigned long chipstatus = MERGESTATUS(status);

		/* reset status */
		map_write(map, CMD(0x50), cmd_adr);
		map_write(map, CMD(0x70), cmd_adr);
		xip_enable(map, chip, cmd_adr);

		if (chipstatus & 0x02) {
			ret = -EROFS;
		} else if (chipstatus & 0x08) {
			printk(KERN_ERR "%s: buffer write error (bad VPP)\n", map->name);
			ret = -EIO;
		} else {
			printk(KERN_ERR "%s: buffer write error (status 0x%lx)\n", map->name, chipstatus);
			ret = -EINVAL;
		}

		goto out;
	}

	xip_enable(map, chip, cmd_adr);
 out:	DISABLE_VPP(map);
	put_chip(map, chip, cmd_adr);
	mutex_unlock(&chip->mutex);
	return ret;
}

static int cfi_intelext_writev (struct mtd_info *mtd, const struct kvec *vecs,
				unsigned long count, loff_t to, size_t *retlen)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	int wbufsize = cfi_interleave(cfi) << cfi->cfiq->MaxBufWriteSize;
	int ret;
	int chipnum;
	unsigned long ofs, vec_seek, i;
	size_t len = 0;

	for (i = 0; i < count; i++)
		len += vecs[i].iov_len;

	if (!len)
		return 0;

	chipnum = to >> cfi->chipshift;
	ofs = to - (chipnum << cfi->chipshift);
	vec_seek = 0;

	do {
		/* We must not cross write block boundaries */
		int size = wbufsize - (ofs & (wbufsize-1));

		if (size > len)
			size = len;
		ret = do_write_buffer(map, &cfi->chips[chipnum],
				      ofs, &vecs, &vec_seek, size);
		if (ret)
			return ret;

		ofs += size;
		(*retlen) += size;
		len -= size;

		if (ofs >> cfi->chipshift) {
			chipnum ++;
			ofs = 0;
			if (chipnum == cfi->numchips)
				return 0;
		}

		/* Be nice and reschedule with the chip in a usable state for other
		   processes. */
		cond_resched();

	} while (len);

	return 0;
}

static int cfi_intelext_write_buffers (struct mtd_info *mtd, loff_t to,
				       size_t len, size_t *retlen, const u_char *buf)
{
	struct kvec vec;

	vec.iov_base = (void *) buf;
	vec.iov_len = len;

	return cfi_intelext_writev(mtd, &vec, 1, to, retlen);
}

static int __xipram do_erase_oneblock(struct map_info *map, struct flchip *chip,
				      unsigned long adr, int len, void *thunk)
{
	struct cfi_private *cfi = map->fldrv_priv;
	map_word status;
	int retries = 3;
	int ret;

	adr += chip->start;

 retry:
	mutex_lock(&chip->mutex);
	ret = get_chip(map, chip, adr, FL_ERASING);
	if (ret) {
		mutex_unlock(&chip->mutex);
		return ret;
	}

	XIP_INVAL_CACHED_RANGE(map, adr, len);
	ENABLE_VPP(map);
	xip_disable(map, chip, adr);

	/* Clear the status register first */
	map_write(map, CMD(0x50), adr);

	/* Now erase */
	map_write(map, CMD(0x20), adr);
	map_write(map, CMD(0xD0), adr);
	chip->state = FL_ERASING;
	chip->erase_suspended = 0;
	chip->in_progress_block_addr = adr;
	chip->in_progress_block_mask = ~(len - 1);

	ret = INVAL_CACHE_AND_WAIT(map, chip, adr,
				   adr, len,
				   chip->erase_time,
				   chip->erase_time_max);
	if (ret) {
		map_write(map, CMD(0x70), adr);
		chip->state = FL_STATUS;
		xip_enable(map, chip, adr);
		printk(KERN_ERR "%s: block erase error: (status timeout)\n", map->name);
		goto out;
	}

	/* We've broken this before. It doesn't hurt to be safe */
	map_write(map, CMD(0x70), adr);
	chip->state = FL_STATUS;
	status = map_read(map, adr);

	/* check for errors */
	if (map_word_bitsset(map, status, CMD(0x3a))) {
		unsigned long chipstatus = MERGESTATUS(status);

		/* Reset the error bits */
		map_write(map, CMD(0x50), adr);
		map_write(map, CMD(0x70), adr);
		xip_enable(map, chip, adr);

		if ((chipstatus & 0x30) == 0x30) {
			printk(KERN_ERR "%s: block erase error: (bad command sequence, status 0x%lx)\n", map->name, chipstatus);
			ret = -EINVAL;
		} else if (chipstatus & 0x02) {
			/* Protection bit set */
			ret = -EROFS;
		} else if (chipstatus & 0x8) {
			/* Voltage */
			printk(KERN_ERR "%s: block erase error: (bad VPP)\n", map->name);
			ret = -EIO;
		} else if (chipstatus & 0x20 && retries--) {
			printk(KERN_DEBUG "block erase failed at 0x%08lx: status 0x%lx. Retrying...\n", adr, chipstatus);
			DISABLE_VPP(map);
			put_chip(map, chip, adr);
			mutex_unlock(&chip->mutex);
			goto retry;
		} else {
			printk(KERN_ERR "%s: block erase failed at 0x%08lx (status 0x%lx)\n", map->name, adr, chipstatus);
			ret = -EIO;
		}

		goto out;
	}

	xip_enable(map, chip, adr);
 out:	DISABLE_VPP(map);
	put_chip(map, chip, adr);
	mutex_unlock(&chip->mutex);
	return ret;
}

static int cfi_intelext_erase_varsize(struct mtd_info *mtd, struct erase_info *instr)
{
	return cfi_varsize_frob(mtd, do_erase_oneblock, instr->addr,
				instr->len, NULL);
}

static void cfi_intelext_sync (struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	int i;
	struct flchip *chip;
	int ret = 0;

	for (i=0; !ret && i<cfi->numchips; i++) {
		chip = &cfi->chips[i];

		mutex_lock(&chip->mutex);
		ret = get_chip(map, chip, chip->start, FL_SYNCING);

		if (!ret) {
			chip->oldstate = chip->state;
			chip->state = FL_SYNCING;
			/* No need to wake_up() on this state change -
			 * as the whole point is that nobody can do anything
			 * with the chip now anyway.
			 */
		}
		mutex_unlock(&chip->mutex);
	}

	/* Unlock the chips again */

	for (i--; i >=0; i--) {
		chip = &cfi->chips[i];

		mutex_lock(&chip->mutex);

		if (chip->state == FL_SYNCING) {
			chip->state = chip->oldstate;
			chip->oldstate = FL_READY;
			wake_up(&chip->wq);
		}
		mutex_unlock(&chip->mutex);
	}
}

static int __xipram do_getlockstatus_oneblock(struct map_info *map,
						struct flchip *chip,
						unsigned long adr,
						int len, void *thunk)
{
	struct cfi_private *cfi = map->fldrv_priv;
	int status, ofs_factor = cfi->interleave * cfi->device_type;

	adr += chip->start;
	xip_disable(map, chip, adr+(2*ofs_factor));
	map_write(map, CMD(0x90), adr+(2*ofs_factor));
	chip->state = FL_JEDEC_QUERY;
	status = cfi_read_query(map, adr+(2*ofs_factor));
	xip_enable(map, chip, 0);
	return status;
}

#ifdef DEBUG_LOCK_BITS
static int __xipram do_printlockstatus_oneblock(struct map_info *map,
						struct flchip *chip,
						unsigned long adr,
						int len, void *thunk)
{
	printk(KERN_DEBUG "block status register for 0x%08lx is %x\n",
	       adr, do_getlockstatus_oneblock(map, chip, adr, len, thunk));
	return 0;
}
#endif

#define DO_XXLOCK_ONEBLOCK_LOCK		((void *) 1)
#define DO_XXLOCK_ONEBLOCK_UNLOCK	((void *) 2)

static int __xipram do_xxlock_oneblock(struct map_info *map, struct flchip *chip,
				       unsigned long adr, int len, void *thunk)
{
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *extp = cfi->cmdset_priv;
	int mdelay;
	int ret;

	adr += chip->start;

	mutex_lock(&chip->mutex);
	ret = get_chip(map, chip, adr, FL_LOCKING);
	if (ret) {
		mutex_unlock(&chip->mutex);
		return ret;
	}

	ENABLE_VPP(map);
	xip_disable(map, chip, adr);

	map_write(map, CMD(0x60), adr);
	if (thunk == DO_XXLOCK_ONEBLOCK_LOCK) {
		map_write(map, CMD(0x01), adr);
		chip->state = FL_LOCKING;
	} else if (thunk == DO_XXLOCK_ONEBLOCK_UNLOCK) {
		map_write(map, CMD(0xD0), adr);
		chip->state = FL_UNLOCKING;
	} else
		BUG();

	/*
	 * If Instant Individual Block Locking supported then no need
	 * to delay.
	 */
	/*
	 * Unlocking may take up to 1.4 seconds on some Intel flashes. So
	 * lets use a max of 1.5 seconds (1500ms) as timeout.
	 *
	 * See "Clear Block Lock-Bits Time" on page 40 in
	 * "3 Volt Intel StrataFlash Memory" 28F128J3,28F640J3,28F320J3 manual
	 * from February 2003
	 */
	mdelay = (!extp || !(extp->FeatureSupport & (1 << 5))) ? 1500 : 0;

	ret = WAIT_TIMEOUT(map, chip, adr, mdelay, mdelay * 1000);
	if (ret) {
		map_write(map, CMD(0x70), adr);
		chip->state = FL_STATUS;
		xip_enable(map, chip, adr);
		printk(KERN_ERR "%s: block unlock error: (status timeout)\n", map->name);
		goto out;
	}

	xip_enable(map, chip, adr);
 out:	DISABLE_VPP(map);
	put_chip(map, chip, adr);
	mutex_unlock(&chip->mutex);
	return ret;
}

static int cfi_intelext_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	int ret;

#ifdef DEBUG_LOCK_BITS
	printk(KERN_DEBUG "%s: lock status before, ofs=0x%08llx, len=0x%08X\n",
	       __func__, ofs, len);
	cfi_varsize_frob(mtd, do_printlockstatus_oneblock,
		ofs, len, NULL);
#endif

	ret = cfi_varsize_frob(mtd, do_xxlock_oneblock,
		ofs, len, DO_XXLOCK_ONEBLOCK_LOCK);

#ifdef DEBUG_LOCK_BITS
	printk(KERN_DEBUG "%s: lock status after, ret=%d\n",
	       __func__, ret);
	cfi_varsize_frob(mtd, do_printlockstatus_oneblock,
		ofs, len, NULL);
#endif

	return ret;
}

static int cfi_intelext_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	int ret;

#ifdef DEBUG_LOCK_BITS
	printk(KERN_DEBUG "%s: lock status before, ofs=0x%08llx, len=0x%08X\n",
	       __func__, ofs, len);
	cfi_varsize_frob(mtd, do_printlockstatus_oneblock,
		ofs, len, NULL);
#endif

	ret = cfi_varsize_frob(mtd, do_xxlock_oneblock,
					ofs, len, DO_XXLOCK_ONEBLOCK_UNLOCK);

#ifdef DEBUG_LOCK_BITS
	printk(KERN_DEBUG "%s: lock status after, ret=%d\n",
	       __func__, ret);
	cfi_varsize_frob(mtd, do_printlockstatus_oneblock,
		ofs, len, NULL);
#endif

	return ret;
}

static int cfi_intelext_is_locked(struct mtd_info *mtd, loff_t ofs,
				  uint64_t len)
{
	return cfi_varsize_frob(mtd, do_getlockstatus_oneblock,
				ofs, len, NULL) ? 1 : 0;
}

#ifdef CONFIG_MTD_OTP

typedef int (*otp_op_t)(struct map_info *map, struct flchip *chip,
			u_long data_offset, u_char *buf, u_int size,
			u_long prot_offset, u_int groupno, u_int groupsize);

static int __xipram
do_otp_read(struct map_info *map, struct flchip *chip, u_long offset,
	    u_char *buf, u_int size, u_long prot, u_int grpno, u_int grpsz)
{
	struct cfi_private *cfi = map->fldrv_priv;
	int ret;

	mutex_lock(&chip->mutex);
	ret = get_chip(map, chip, chip->start, FL_JEDEC_QUERY);
	if (ret) {
		mutex_unlock(&chip->mutex);
		return ret;
	}

	/* let's ensure we're not reading back cached data from array mode */
	INVALIDATE_CACHED_RANGE(map, chip->start + offset, size);

	xip_disable(map, chip, chip->start);
	if (chip->state != FL_JEDEC_QUERY) {
		map_write(map, CMD(0x90), chip->start);
		chip->state = FL_JEDEC_QUERY;
	}
	map_copy_from(map, buf, chip->start + offset, size);
	xip_enable(map, chip, chip->start);

	/* then ensure we don't keep OTP data in the cache */
	INVALIDATE_CACHED_RANGE(map, chip->start + offset, size);

	put_chip(map, chip, chip->start);
	mutex_unlock(&chip->mutex);
	return 0;
}

static int
do_otp_write(struct map_info *map, struct flchip *chip, u_long offset,
	     u_char *buf, u_int size, u_long prot, u_int grpno, u_int grpsz)
{
	int ret;

	while (size) {
		unsigned long bus_ofs = offset & ~(map_bankwidth(map)-1);
		int gap = offset - bus_ofs;
		int n = min_t(int, size, map_bankwidth(map)-gap);
		map_word datum = map_word_ff(map);

		datum = map_word_load_partial(map, datum, buf, gap, n);
		ret = do_write_oneword(map, chip, bus_ofs, datum, FL_OTP_WRITE);
		if (ret)
			return ret;

		offset += n;
		buf += n;
		size -= n;
	}

	return 0;
}

static int
do_otp_lock(struct map_info *map, struct flchip *chip, u_long offset,
	    u_char *buf, u_int size, u_long prot, u_int grpno, u_int grpsz)
{
	struct cfi_private *cfi = map->fldrv_priv;
	map_word datum;

	/* make sure area matches group boundaries */
	if (size != grpsz)
		return -EXDEV;

	datum = map_word_ff(map);
	datum = map_word_clr(map, datum, CMD(1 << grpno));
	return do_write_oneword(map, chip, prot, datum, FL_OTP_WRITE);
}

static int cfi_intelext_otp_walk(struct mtd_info *mtd, loff_t from, size_t len,
				 size_t *retlen, u_char *buf,
				 otp_op_t action, int user_regs)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *extp = cfi->cmdset_priv;
	struct flchip *chip;
	struct cfi_intelext_otpinfo *otp;
	u_long devsize, reg_prot_offset, data_offset;
	u_int chip_num, chip_step, field, reg_fact_size, reg_user_size;
	u_int groups, groupno, groupsize, reg_fact_groups, reg_user_groups;
	int ret;

	*retlen = 0;

	/* Check that we actually have some OTP registers */
	if (!extp || !(extp->FeatureSupport & 64) || !extp->NumProtectionFields)
		return -ENODATA;

	/* we need real chips here not virtual ones */
	devsize = (1 << cfi->cfiq->DevSize) * cfi->interleave;
	chip_step = devsize >> cfi->chipshift;
	chip_num = 0;

	/* Some chips have OTP located in the _top_ partition only.
	   For example: Intel 28F256L18T (T means top-parameter device) */
	if (cfi->mfr == CFI_MFR_INTEL) {
		switch (cfi->id) {
		case 0x880b:
		case 0x880c:
		case 0x880d:
			chip_num = chip_step - 1;
		}
	}

	for ( ; chip_num < cfi->numchips; chip_num += chip_step) {
		chip = &cfi->chips[chip_num];
		otp = (struct cfi_intelext_otpinfo *)&extp->extra[0];

		/* first OTP region */
		field = 0;
		reg_prot_offset = extp->ProtRegAddr;
		reg_fact_groups = 1;
		reg_fact_size = 1 << extp->FactProtRegSize;
		reg_user_groups = 1;
		reg_user_size = 1 << extp->UserProtRegSize;

		while (len > 0) {
			/* flash geometry fixup */
			data_offset = reg_prot_offset + 1;
			data_offset *= cfi->interleave * cfi->device_type;
			reg_prot_offset *= cfi->interleave * cfi->device_type;
			reg_fact_size *= cfi->interleave;
			reg_user_size *= cfi->interleave;

			if (user_regs) {
				groups = reg_user_groups;
				groupsize = reg_user_size;
				/* skip over factory reg area */
				groupno = reg_fact_groups;
				data_offset += reg_fact_groups * reg_fact_size;
			} else {
				groups = reg_fact_groups;
				groupsize = reg_fact_size;
				groupno = 0;
			}

			while (len > 0 && groups > 0) {
				if (!action) {
					/*
					 * Special case: if action is NULL
					 * we fill buf with otp_info records.
					 */
					struct otp_info *otpinfo;
					map_word lockword;
					len -= sizeof(struct otp_info);
					if (len <= 0)
						return -ENOSPC;
					ret = do_otp_read(map, chip,
							  reg_prot_offset,
							  (u_char *)&lockword,
							  map_bankwidth(map),
							  0, 0,  0);
					if (ret)
						return ret;
					otpinfo = (struct otp_info *)buf;
					otpinfo->start = from;
					otpinfo->length = groupsize;
					otpinfo->locked =
					   !map_word_bitsset(map, lockword,
							     CMD(1 << groupno));
					from += groupsize;
					buf += sizeof(*otpinfo);
					*retlen += sizeof(*otpinfo);
				} else if (from >= groupsize) {
					from -= groupsize;
					data_offset += groupsize;
				} else {
					int size = groupsize;
					data_offset += from;
					size -= from;
					from = 0;
					if (size > len)
						size = len;
					ret = action(map, chip, data_offset,
						     buf, size, reg_prot_offset,
						     groupno, groupsize);
					if (ret < 0)
						return ret;
					buf += size;
					len -= size;
					*retlen += size;
					data_offset += size;
				}
				groupno++;
				groups--;
			}

			/* next OTP region */
			if (++field == extp->NumProtectionFields)
				break;
			reg_prot_offset = otp->ProtRegAddr;
			reg_fact_groups = otp->FactGroups;
			reg_fact_size = 1 << otp->FactProtRegSize;
			reg_user_groups = otp->UserGroups;
			reg_user_size = 1 << otp->UserProtRegSize;
			otp++;
		}
	}

	return 0;
}

static int cfi_intelext_read_fact_prot_reg(struct mtd_info *mtd, loff_t from,
					   size_t len, size_t *retlen,
					    u_char *buf)
{
	return cfi_intelext_otp_walk(mtd, from, len, retlen,
				     buf, do_otp_read, 0);
}

static int cfi_intelext_read_user_prot_reg(struct mtd_info *mtd, loff_t from,
					   size_t len, size_t *retlen,
					    u_char *buf)
{
	return cfi_intelext_otp_walk(mtd, from, len, retlen,
				     buf, do_otp_read, 1);
}

static int cfi_intelext_write_user_prot_reg(struct mtd_info *mtd, loff_t from,
					    size_t len, size_t *retlen,
					    const u_char *buf)
{
	return cfi_intelext_otp_walk(mtd, from, len, retlen,
				     (u_char *)buf, do_otp_write, 1);
}

static int cfi_intelext_lock_user_prot_reg(struct mtd_info *mtd,
					   loff_t from, size_t len)
{
	size_t retlen;
	return cfi_intelext_otp_walk(mtd, from, len, &retlen,
				     NULL, do_otp_lock, 1);
}

static int cfi_intelext_get_fact_prot_info(struct mtd_info *mtd, size_t len,
					   size_t *retlen, struct otp_info *buf)

{
	return cfi_intelext_otp_walk(mtd, 0, len, retlen, (u_char *)buf,
				     NULL, 0);
}

static int cfi_intelext_get_user_prot_info(struct mtd_info *mtd, size_t len,
					   size_t *retlen, struct otp_info *buf)
{
	return cfi_intelext_otp_walk(mtd, 0, len, retlen, (u_char *)buf,
				     NULL, 1);
}

#endif

static void cfi_intelext_save_locks(struct mtd_info *mtd)
{
	struct mtd_erase_region_info *region;
	int block, status, i;
	unsigned long adr;
	size_t len;

	for (i = 0; i < mtd->numeraseregions; i++) {
		region = &mtd->eraseregions[i];
		if (!region->lockmap)
			continue;

		for (block = 0; block < region->numblocks; block++){
			len = region->erasesize;
			adr = region->offset + block * len;

			status = cfi_varsize_frob(mtd,
					do_getlockstatus_oneblock, adr, len, NULL);
			if (status)
				set_bit(block, region->lockmap);
			else
				clear_bit(block, region->lockmap);
		}
	}
}

static int cfi_intelext_suspend(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *extp = cfi->cmdset_priv;
	int i;
	struct flchip *chip;
	int ret = 0;

	if ((mtd->flags & MTD_POWERUP_LOCK)
	    && extp && (extp->FeatureSupport & (1 << 5)))
		cfi_intelext_save_locks(mtd);

	for (i=0; !ret && i<cfi->numchips; i++) {
		chip = &cfi->chips[i];

		mutex_lock(&chip->mutex);

		switch (chip->state) {
		case FL_READY:
		case FL_STATUS:
		case FL_CFI_QUERY:
		case FL_JEDEC_QUERY:
			if (chip->oldstate == FL_READY) {
				/* place the chip in a known state before suspend */
				map_write(map, CMD(0xFF), cfi->chips[i].start);
				chip->oldstate = chip->state;
				chip->state = FL_PM_SUSPENDED;
				/* No need to wake_up() on this state change -
				 * as the whole point is that nobody can do anything
				 * with the chip now anyway.
				 */
			} else {
				/* There seems to be an operation pending. We must wait for it. */
				printk(KERN_NOTICE "Flash device refused suspend due to pending operation (oldstate %d)\n", chip->oldstate);
				ret = -EAGAIN;
			}
			break;
		default:
			/* Should we actually wait? Once upon a time these routines weren't
			   allowed to. Or should we return -EAGAIN, because the upper layers
			   ought to have already shut down anything which was using the device
			   anyway? The latter for now. */
			printk(KERN_NOTICE "Flash device refused suspend due to active operation (state %d)\n", chip->state);
			ret = -EAGAIN;
			break;
		case FL_PM_SUSPENDED:
			break;
		}
		mutex_unlock(&chip->mutex);
	}

	/* Unlock the chips again */

	if (ret) {
		for (i--; i >=0; i--) {
			chip = &cfi->chips[i];

			mutex_lock(&chip->mutex);

			if (chip->state == FL_PM_SUSPENDED) {
				/* No need to force it into a known state here,
				   because we're returning failure, and it didn't
				   get power cycled */
				chip->state = chip->oldstate;
				chip->oldstate = FL_READY;
				wake_up(&chip->wq);
			}
			mutex_unlock(&chip->mutex);
		}
	}

	return ret;
}

static void cfi_intelext_restore_locks(struct mtd_info *mtd)
{
	struct mtd_erase_region_info *region;
	int block, i;
	unsigned long adr;
	size_t len;

	for (i = 0; i < mtd->numeraseregions; i++) {
		region = &mtd->eraseregions[i];
		if (!region->lockmap)
			continue;

		for_each_clear_bit(block, region->lockmap, region->numblocks) {
			len = region->erasesize;
			adr = region->offset + block * len;
			cfi_intelext_unlock(mtd, adr, len);
		}
	}
}

static void cfi_intelext_resume(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	struct cfi_pri_intelext *extp = cfi->cmdset_priv;
	int i;
	struct flchip *chip;

	for (i=0; i<cfi->numchips; i++) {

		chip = &cfi->chips[i];

		mutex_lock(&chip->mutex);

		/* Go to known state. Chip may have been power cycled */
		if (chip->state == FL_PM_SUSPENDED) {
			/* Refresh LH28F640BF Partition Config. Register */
			fixup_LH28F640BF(mtd);
			map_write(map, CMD(0xFF), cfi->chips[i].start);
			chip->oldstate = chip->state = FL_READY;
			wake_up(&chip->wq);
		}

		mutex_unlock(&chip->mutex);
	}

	if ((mtd->flags & MTD_POWERUP_LOCK)
	    && extp && (extp->FeatureSupport & (1 << 5)))
		cfi_intelext_restore_locks(mtd);
}

static int cfi_intelext_reset(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	int i, ret;

	for (i=0; i < cfi->numchips; i++) {
		struct flchip *chip = &cfi->chips[i];

		/* force the completion of any ongoing operation
		   and switch to array mode so any bootloader in
		   flash is accessible for soft reboot. */
		mutex_lock(&chip->mutex);
		ret = get_chip(map, chip, chip->start, FL_SHUTDOWN);
		if (!ret) {
			map_write(map, CMD(0xff), chip->start);
			chip->state = FL_SHUTDOWN;
			put_chip(map, chip, chip->start);
		}
		mutex_unlock(&chip->mutex);
	}

	return 0;
}

static int cfi_intelext_reboot(struct notifier_block *nb, unsigned long val,
			       void *v)
{
	struct mtd_info *mtd;

	mtd = container_of(nb, struct mtd_info, reboot_notifier);
	cfi_intelext_reset(mtd);
	return NOTIFY_DONE;
}

static void cfi_intelext_destroy(struct mtd_info *mtd)
{
	struct map_info *map = mtd->priv;
	struct cfi_private *cfi = map->fldrv_priv;
	struct mtd_erase_region_info *region;
	int i;
	cfi_intelext_reset(mtd);
	unregister_reboot_notifier(&mtd->reboot_notifier);
	kfree(cfi->cmdset_priv);
	kfree(cfi->cfiq);
	kfree(cfi->chips[0].priv);
	kfree(cfi);
	for (i = 0; i < mtd->numeraseregions; i++) {
		region = &mtd->eraseregions[i];
		kfree(region->lockmap);
	}
	kfree(mtd->eraseregions);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Woodhouse <dwmw2@infradead.org> et al.");
MODULE_DESCRIPTION("MTD chip driver for Intel/Sharp flash chips");
MODULE_ALIAS("cfi_cmdset_0003");
MODULE_ALIAS("cfi_cmdset_0200");
