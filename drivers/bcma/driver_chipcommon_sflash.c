/*
 * Broadcom specific AMBA
 * ChipCommon serial flash interface
 *
 * Licensed under the GNU/GPL. See COPYING for details.
 */

#include "bcma_private.h"

#include <linux/platform_device.h>
#include <linux/bcma/bcma.h>

static struct resource bcma_sflash_resource = {
	.name	= "bcma_sflash",
	.start	= BCMA_SOC_FLASH2,
	.end	= 0,
	.flags  = IORESOURCE_MEM | IORESOURCE_READONLY,
};

struct platform_device bcma_sflash_dev = {
	.name		= "bcma_sflash",
	.resource	= &bcma_sflash_resource,
	.num_resources	= 1,
};

struct bcma_sflash_tbl_e {
	char *name;
	u32 id;
	u32 blocksize;
	u16 numblocks;
};

static const struct bcma_sflash_tbl_e bcma_sflash_st_tbl[] = {
	{ "M25P20", 0x11, 0x10000, 4, },
	{ "M25P40", 0x12, 0x10000, 8, },

	{ "M25P16", 0x14, 0x10000, 32, },
	{ "M25P32", 0x15, 0x10000, 64, },
	{ "M25P64", 0x16, 0x10000, 128, },
	{ "M25FL128", 0x17, 0x10000, 256, },
	{ "MX25L25635F", 0x18, 0x10000, 512, },
	{ NULL },
};

static const struct bcma_sflash_tbl_e bcma_sflash_sst_tbl[] = {
	{ "SST25WF512", 1, 0x1000, 16, },
	{ "SST25VF512", 0x48, 0x1000, 16, },
	{ "SST25WF010", 2, 0x1000, 32, },
	{ "SST25VF010", 0x49, 0x1000, 32, },
	{ "SST25WF020", 3, 0x1000, 64, },
	{ "SST25VF020", 0x43, 0x1000, 64, },
	{ "SST25WF040", 4, 0x1000, 128, },
	{ "SST25VF040", 0x44, 0x1000, 128, },
	{ "SST25VF040B", 0x8d, 0x1000, 128, },
	{ "SST25WF080", 5, 0x1000, 256, },
	{ "SST25VF080B", 0x8e, 0x1000, 256, },
	{ "SST25VF016", 0x41, 0x1000, 512, },
	{ "SST25VF032", 0x4a, 0x1000, 1024, },
	{ "SST25VF064", 0x4b, 0x1000, 2048, },
	{ NULL },
};

static const struct bcma_sflash_tbl_e bcma_sflash_at_tbl[] = {
	{ "AT45DB011", 0xc, 256, 512, },
	{ "AT45DB021", 0x14, 256, 1024, },
	{ "AT45DB041", 0x1c, 256, 2048, },
	{ "AT45DB081", 0x24, 256, 4096, },
	{ "AT45DB161", 0x2c, 512, 4096, },
	{ "AT45DB321", 0x34, 512, 8192, },
	{ "AT45DB642", 0x3c, 1024, 8192, },
	{ NULL },
};

static void bcma_sflash_cmd(struct bcma_drv_cc *cc, u32 opcode)
{
	int i;
	bcma_cc_write32(cc, BCMA_CC_FLASHCTL,
			BCMA_CC_FLASHCTL_START | opcode);
	for (i = 0; i < 1000; i++) {
		if (!(bcma_cc_read32(cc, BCMA_CC_FLASHCTL) &
		      BCMA_CC_FLASHCTL_BUSY))
			return;
		cpu_relax();
	}
	bcma_err(cc->core->bus, "SFLASH control command failed (timeout)!\n");
}

/* Initialize serial flash access */
int bcma_sflash_init(struct bcma_drv_cc *cc)
{
	struct bcma_bus *bus = cc->core->bus;
	struct bcma_sflash *sflash = &cc->sflash;
	const struct bcma_sflash_tbl_e *e;
	u32 id, id2;

	switch (cc->capabilities & BCMA_CC_CAP_FLASHT) {
	case BCMA_CC_FLASHT_STSER:
		bcma_sflash_cmd(cc, BCMA_CC_FLASHCTL_ST_DP);

		bcma_cc_write32(cc, BCMA_CC_FLASHADDR, 0);
		bcma_sflash_cmd(cc, BCMA_CC_FLASHCTL_ST_RES);
		id = bcma_cc_read32(cc, BCMA_CC_FLASHDATA);

		bcma_cc_write32(cc, BCMA_CC_FLASHADDR, 1);
		bcma_sflash_cmd(cc, BCMA_CC_FLASHCTL_ST_RES);
		id2 = bcma_cc_read32(cc, BCMA_CC_FLASHDATA);

		switch (id) {
		case 0xbf:
			for (e = bcma_sflash_sst_tbl; e->name; e++) {
				if (e->id == id2)
					break;
			}
			break;
		case 0x13:
			return -ENOTSUPP;
		default:
			for (e = bcma_sflash_st_tbl; e->name; e++) {
				if (e->id == id)
					break;
			}
			break;
		}
		if (!e->name) {
			bcma_err(bus, "Unsupported ST serial flash (id: 0x%X, id2: 0x%X)\n", id, id2);
			return -ENOTSUPP;
		}

		break;
	case BCMA_CC_FLASHT_ATSER:
		bcma_sflash_cmd(cc, BCMA_CC_FLASHCTL_AT_STATUS);
		id = bcma_cc_read32(cc, BCMA_CC_FLASHDATA) & 0x3c;

		for (e = bcma_sflash_at_tbl; e->name; e++) {
			if (e->id == id)
				break;
		}
		if (!e->name) {
			bcma_err(bus, "Unsupported Atmel serial flash (id: 0x%X)\n", id);
			return -ENOTSUPP;
		}

		break;
	default:
		bcma_err(bus, "Unsupported flash type\n");
		return -ENOTSUPP;
	}

	sflash->blocksize = e->blocksize;
	sflash->numblocks = e->numblocks;
	sflash->size = sflash->blocksize * sflash->numblocks;
	sflash->present = true;

	bcma_info(bus, "Found %s serial flash (size: %dKiB, blocksize: 0x%X, blocks: %d)\n",
		  e->name, sflash->size / 1024, sflash->blocksize,
		  sflash->numblocks);

	/* Prepare platform device, but don't register it yet. It's too early,
	 * malloc (required by device_private_init) is not available yet. */
	bcma_sflash_dev.resource[0].end = bcma_sflash_dev.resource[0].start +
					  sflash->size;
	bcma_sflash_dev.dev.platform_data = sflash;

	return 0;
}
