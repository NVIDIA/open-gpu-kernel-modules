/*
 * Intel 3200/3210 Memory Controller kernel module
 * Copyright (C) 2008-2009 Akamai Technologies, Inc.
 * Portions by Hitoshi Mitake <h.mitake@gmail.com>.
 *
 * This file may be distributed under the terms of the
 * GNU General Public License.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/edac.h>
#include <linux/io.h>
#include "edac_module.h"

#include <linux/io-64-nonatomic-lo-hi.h>

#define EDAC_MOD_STR        "i3200_edac"

#define PCI_DEVICE_ID_INTEL_3200_HB    0x29f0

#define I3200_DIMMS		4
#define I3200_RANKS		8
#define I3200_RANKS_PER_CHANNEL	4
#define I3200_CHANNELS		2

/* Intel 3200 register addresses - device 0 function 0 - DRAM Controller */

#define I3200_MCHBAR_LOW	0x48	/* MCH Memory Mapped Register BAR */
#define I3200_MCHBAR_HIGH	0x4c
#define I3200_MCHBAR_MASK	0xfffffc000ULL	/* bits 35:14 */
#define I3200_MMR_WINDOW_SIZE	16384

#define I3200_TOM		0xa0	/* Top of Memory (16b)
		 *
		 * 15:10 reserved
		 *  9:0  total populated physical memory
		 */
#define I3200_TOM_MASK		0x3ff	/* bits 9:0 */
#define I3200_TOM_SHIFT		26	/* 64MiB grain */

#define I3200_ERRSTS		0xc8	/* Error Status Register (16b)
		 *
		 * 15    reserved
		 * 14    Isochronous TBWRR Run Behind FIFO Full
		 *       (ITCV)
		 * 13    Isochronous TBWRR Run Behind FIFO Put
		 *       (ITSTV)
		 * 12    reserved
		 * 11    MCH Thermal Sensor Event
		 *       for SMI/SCI/SERR (GTSE)
		 * 10    reserved
		 *  9    LOCK to non-DRAM Memory Flag (LCKF)
		 *  8    reserved
		 *  7    DRAM Throttle Flag (DTF)
		 *  6:2  reserved
		 *  1    Multi-bit DRAM ECC Error Flag (DMERR)
		 *  0    Single-bit DRAM ECC Error Flag (DSERR)
		 */
#define I3200_ERRSTS_UE		0x0002
#define I3200_ERRSTS_CE		0x0001
#define I3200_ERRSTS_BITS	(I3200_ERRSTS_UE | I3200_ERRSTS_CE)


/* Intel  MMIO register space - device 0 function 0 - MMR space */

#define I3200_C0DRB	0x200	/* Channel 0 DRAM Rank Boundary (16b x 4)
		 *
		 * 15:10 reserved
		 *  9:0  Channel 0 DRAM Rank Boundary Address
		 */
#define I3200_C1DRB	0x600	/* Channel 1 DRAM Rank Boundary (16b x 4) */
#define I3200_DRB_MASK	0x3ff	/* bits 9:0 */
#define I3200_DRB_SHIFT	26	/* 64MiB grain */

#define I3200_C0ECCERRLOG	0x280	/* Channel 0 ECC Error Log (64b)
		 *
		 * 63:48 Error Column Address (ERRCOL)
		 * 47:32 Error Row Address (ERRROW)
		 * 31:29 Error Bank Address (ERRBANK)
		 * 28:27 Error Rank Address (ERRRANK)
		 * 26:24 reserved
		 * 23:16 Error Syndrome (ERRSYND)
		 * 15: 2 reserved
		 *    1  Multiple Bit Error Status (MERRSTS)
		 *    0  Correctable Error Status (CERRSTS)
		 */
#define I3200_C1ECCERRLOG		0x680	/* Chan 1 ECC Error Log (64b) */
#define I3200_ECCERRLOG_CE		0x1
#define I3200_ECCERRLOG_UE		0x2
#define I3200_ECCERRLOG_RANK_BITS	0x18000000
#define I3200_ECCERRLOG_RANK_SHIFT	27
#define I3200_ECCERRLOG_SYNDROME_BITS	0xff0000
#define I3200_ECCERRLOG_SYNDROME_SHIFT	16
#define I3200_CAPID0			0xe0	/* P.95 of spec for details */

struct i3200_priv {
	void __iomem *window;
};

static int nr_channels;

static int how_many_channels(struct pci_dev *pdev)
{
	int n_channels;

	unsigned char capid0_8b; /* 8th byte of CAPID0 */

	pci_read_config_byte(pdev, I3200_CAPID0 + 8, &capid0_8b);

	if (capid0_8b & 0x20) { /* check DCD: Dual Channel Disable */
		edac_dbg(0, "In single channel mode\n");
		n_channels = 1;
	} else {
		edac_dbg(0, "In dual channel mode\n");
		n_channels = 2;
	}

	if (capid0_8b & 0x10) /* check if both channels are filled */
		edac_dbg(0, "2 DIMMS per channel disabled\n");
	else
		edac_dbg(0, "2 DIMMS per channel enabled\n");

	return n_channels;
}

static unsigned long eccerrlog_syndrome(u64 log)
{
	return (log & I3200_ECCERRLOG_SYNDROME_BITS) >>
		I3200_ECCERRLOG_SYNDROME_SHIFT;
}

static int eccerrlog_row(int channel, u64 log)
{
	u64 rank = ((log & I3200_ECCERRLOG_RANK_BITS) >>
		I3200_ECCERRLOG_RANK_SHIFT);
	return rank | (channel * I3200_RANKS_PER_CHANNEL);
}

enum i3200_chips {
	I3200 = 0,
};

struct i3200_dev_info {
	const char *ctl_name;
};

struct i3200_error_info {
	u16 errsts;
	u16 errsts2;
	u64 eccerrlog[I3200_CHANNELS];
};

static const struct i3200_dev_info i3200_devs[] = {
	[I3200] = {
		.ctl_name = "i3200"
	},
};

static struct pci_dev *mci_pdev;
static int i3200_registered = 1;


static void i3200_clear_error_info(struct mem_ctl_info *mci)
{
	struct pci_dev *pdev;

	pdev = to_pci_dev(mci->pdev);

	/*
	 * Clear any error bits.
	 * (Yes, we really clear bits by writing 1 to them.)
	 */
	pci_write_bits16(pdev, I3200_ERRSTS, I3200_ERRSTS_BITS,
		I3200_ERRSTS_BITS);
}

static void i3200_get_and_clear_error_info(struct mem_ctl_info *mci,
		struct i3200_error_info *info)
{
	struct pci_dev *pdev;
	struct i3200_priv *priv = mci->pvt_info;
	void __iomem *window = priv->window;

	pdev = to_pci_dev(mci->pdev);

	/*
	 * This is a mess because there is no atomic way to read all the
	 * registers at once and the registers can transition from CE being
	 * overwritten by UE.
	 */
	pci_read_config_word(pdev, I3200_ERRSTS, &info->errsts);
	if (!(info->errsts & I3200_ERRSTS_BITS))
		return;

	info->eccerrlog[0] = readq(window + I3200_C0ECCERRLOG);
	if (nr_channels == 2)
		info->eccerrlog[1] = readq(window + I3200_C1ECCERRLOG);

	pci_read_config_word(pdev, I3200_ERRSTS, &info->errsts2);

	/*
	 * If the error is the same for both reads then the first set
	 * of reads is valid.  If there is a change then there is a CE
	 * with no info and the second set of reads is valid and
	 * should be UE info.
	 */
	if ((info->errsts ^ info->errsts2) & I3200_ERRSTS_BITS) {
		info->eccerrlog[0] = readq(window + I3200_C0ECCERRLOG);
		if (nr_channels == 2)
			info->eccerrlog[1] = readq(window + I3200_C1ECCERRLOG);
	}

	i3200_clear_error_info(mci);
}

static void i3200_process_error_info(struct mem_ctl_info *mci,
		struct i3200_error_info *info)
{
	int channel;
	u64 log;

	if (!(info->errsts & I3200_ERRSTS_BITS))
		return;

	if ((info->errsts ^ info->errsts2) & I3200_ERRSTS_BITS) {
		edac_mc_handle_error(HW_EVENT_ERR_UNCORRECTED, mci, 1, 0, 0, 0,
				     -1, -1, -1, "UE overwrote CE", "");
		info->errsts = info->errsts2;
	}

	for (channel = 0; channel < nr_channels; channel++) {
		log = info->eccerrlog[channel];
		if (log & I3200_ECCERRLOG_UE) {
			edac_mc_handle_error(HW_EVENT_ERR_UNCORRECTED, mci, 1,
					     0, 0, 0,
					     eccerrlog_row(channel, log),
					     -1, -1,
					     "i3000 UE", "");
		} else if (log & I3200_ECCERRLOG_CE) {
			edac_mc_handle_error(HW_EVENT_ERR_CORRECTED, mci, 1,
					     0, 0, eccerrlog_syndrome(log),
					     eccerrlog_row(channel, log),
					     -1, -1,
					     "i3000 CE", "");
		}
	}
}

static void i3200_check(struct mem_ctl_info *mci)
{
	struct i3200_error_info info;

	i3200_get_and_clear_error_info(mci, &info);
	i3200_process_error_info(mci, &info);
}

static void __iomem *i3200_map_mchbar(struct pci_dev *pdev)
{
	union {
		u64 mchbar;
		struct {
			u32 mchbar_low;
			u32 mchbar_high;
		};
	} u;
	void __iomem *window;

	pci_read_config_dword(pdev, I3200_MCHBAR_LOW, &u.mchbar_low);
	pci_read_config_dword(pdev, I3200_MCHBAR_HIGH, &u.mchbar_high);
	u.mchbar &= I3200_MCHBAR_MASK;

	if (u.mchbar != (resource_size_t)u.mchbar) {
		printk(KERN_ERR
			"i3200: mmio space beyond accessible range (0x%llx)\n",
			(unsigned long long)u.mchbar);
		return NULL;
	}

	window = ioremap(u.mchbar, I3200_MMR_WINDOW_SIZE);
	if (!window)
		printk(KERN_ERR "i3200: cannot map mmio space at 0x%llx\n",
			(unsigned long long)u.mchbar);

	return window;
}


static void i3200_get_drbs(void __iomem *window,
	u16 drbs[I3200_CHANNELS][I3200_RANKS_PER_CHANNEL])
{
	int i;

	for (i = 0; i < I3200_RANKS_PER_CHANNEL; i++) {
		drbs[0][i] = readw(window + I3200_C0DRB + 2*i) & I3200_DRB_MASK;
		drbs[1][i] = readw(window + I3200_C1DRB + 2*i) & I3200_DRB_MASK;

		edac_dbg(0, "drb[0][%d] = %d, drb[1][%d] = %d\n", i, drbs[0][i], i, drbs[1][i]);
	}
}

static bool i3200_is_stacked(struct pci_dev *pdev,
	u16 drbs[I3200_CHANNELS][I3200_RANKS_PER_CHANNEL])
{
	u16 tom;

	pci_read_config_word(pdev, I3200_TOM, &tom);
	tom &= I3200_TOM_MASK;

	return drbs[I3200_CHANNELS - 1][I3200_RANKS_PER_CHANNEL - 1] == tom;
}

static unsigned long drb_to_nr_pages(
	u16 drbs[I3200_CHANNELS][I3200_RANKS_PER_CHANNEL], bool stacked,
	int channel, int rank)
{
	int n;

	n = drbs[channel][rank];
	if (!n)
		return 0;

	if (rank > 0)
		n -= drbs[channel][rank - 1];
	if (stacked && (channel == 1) &&
	drbs[channel][rank] == drbs[channel][I3200_RANKS_PER_CHANNEL - 1])
		n -= drbs[0][I3200_RANKS_PER_CHANNEL - 1];

	n <<= (I3200_DRB_SHIFT - PAGE_SHIFT);
	return n;
}

static int i3200_probe1(struct pci_dev *pdev, int dev_idx)
{
	int rc;
	int i, j;
	struct mem_ctl_info *mci = NULL;
	struct edac_mc_layer layers[2];
	u16 drbs[I3200_CHANNELS][I3200_RANKS_PER_CHANNEL];
	bool stacked;
	void __iomem *window;
	struct i3200_priv *priv;

	edac_dbg(0, "MC:\n");

	window = i3200_map_mchbar(pdev);
	if (!window)
		return -ENODEV;

	i3200_get_drbs(window, drbs);
	nr_channels = how_many_channels(pdev);

	layers[0].type = EDAC_MC_LAYER_CHIP_SELECT;
	layers[0].size = I3200_DIMMS;
	layers[0].is_virt_csrow = true;
	layers[1].type = EDAC_MC_LAYER_CHANNEL;
	layers[1].size = nr_channels;
	layers[1].is_virt_csrow = false;
	mci = edac_mc_alloc(0, ARRAY_SIZE(layers), layers,
			    sizeof(struct i3200_priv));
	if (!mci)
		return -ENOMEM;

	edac_dbg(3, "MC: init mci\n");

	mci->pdev = &pdev->dev;
	mci->mtype_cap = MEM_FLAG_DDR2;

	mci->edac_ctl_cap = EDAC_FLAG_SECDED;
	mci->edac_cap = EDAC_FLAG_SECDED;

	mci->mod_name = EDAC_MOD_STR;
	mci->ctl_name = i3200_devs[dev_idx].ctl_name;
	mci->dev_name = pci_name(pdev);
	mci->edac_check = i3200_check;
	mci->ctl_page_to_phys = NULL;
	priv = mci->pvt_info;
	priv->window = window;

	stacked = i3200_is_stacked(pdev, drbs);

	/*
	 * The dram rank boundary (DRB) reg values are boundary addresses
	 * for each DRAM rank with a granularity of 64MB.  DRB regs are
	 * cumulative; the last one will contain the total memory
	 * contained in all ranks.
	 */
	for (i = 0; i < I3200_DIMMS; i++) {
		unsigned long nr_pages;

		for (j = 0; j < nr_channels; j++) {
			struct dimm_info *dimm = edac_get_dimm(mci, i, j, 0);

			nr_pages = drb_to_nr_pages(drbs, stacked, j, i);
			if (nr_pages == 0)
				continue;

			edac_dbg(0, "csrow %d, channel %d%s, size = %ld MiB\n", i, j,
				 stacked ? " (stacked)" : "", PAGES_TO_MiB(nr_pages));

			dimm->nr_pages = nr_pages;
			dimm->grain = nr_pages << PAGE_SHIFT;
			dimm->mtype = MEM_DDR2;
			dimm->dtype = DEV_UNKNOWN;
			dimm->edac_mode = EDAC_UNKNOWN;
		}
	}

	i3200_clear_error_info(mci);

	rc = -ENODEV;
	if (edac_mc_add_mc(mci)) {
		edac_dbg(3, "MC: failed edac_mc_add_mc()\n");
		goto fail;
	}

	/* get this far and it's successful */
	edac_dbg(3, "MC: success\n");
	return 0;

fail:
	iounmap(window);
	if (mci)
		edac_mc_free(mci);

	return rc;
}

static int i3200_init_one(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int rc;

	edac_dbg(0, "MC:\n");

	if (pci_enable_device(pdev) < 0)
		return -EIO;

	rc = i3200_probe1(pdev, ent->driver_data);
	if (!mci_pdev)
		mci_pdev = pci_dev_get(pdev);

	return rc;
}

static void i3200_remove_one(struct pci_dev *pdev)
{
	struct mem_ctl_info *mci;
	struct i3200_priv *priv;

	edac_dbg(0, "\n");

	mci = edac_mc_del_mc(&pdev->dev);
	if (!mci)
		return;

	priv = mci->pvt_info;
	iounmap(priv->window);

	edac_mc_free(mci);

	pci_disable_device(pdev);
}

static const struct pci_device_id i3200_pci_tbl[] = {
	{
		PCI_VEND_DEV(INTEL, 3200_HB), PCI_ANY_ID, PCI_ANY_ID, 0, 0,
		I3200},
	{
		0,
	}            /* 0 terminated list. */
};

MODULE_DEVICE_TABLE(pci, i3200_pci_tbl);

static struct pci_driver i3200_driver = {
	.name = EDAC_MOD_STR,
	.probe = i3200_init_one,
	.remove = i3200_remove_one,
	.id_table = i3200_pci_tbl,
};

static int __init i3200_init(void)
{
	int pci_rc;

	edac_dbg(3, "MC:\n");

	/* Ensure that the OPSTATE is set correctly for POLL or NMI */
	opstate_init();

	pci_rc = pci_register_driver(&i3200_driver);
	if (pci_rc < 0)
		goto fail0;

	if (!mci_pdev) {
		i3200_registered = 0;
		mci_pdev = pci_get_device(PCI_VENDOR_ID_INTEL,
				PCI_DEVICE_ID_INTEL_3200_HB, NULL);
		if (!mci_pdev) {
			edac_dbg(0, "i3200 pci_get_device fail\n");
			pci_rc = -ENODEV;
			goto fail1;
		}

		pci_rc = i3200_init_one(mci_pdev, i3200_pci_tbl);
		if (pci_rc < 0) {
			edac_dbg(0, "i3200 init fail\n");
			pci_rc = -ENODEV;
			goto fail1;
		}
	}

	return 0;

fail1:
	pci_unregister_driver(&i3200_driver);

fail0:
	pci_dev_put(mci_pdev);

	return pci_rc;
}

static void __exit i3200_exit(void)
{
	edac_dbg(3, "MC:\n");

	pci_unregister_driver(&i3200_driver);
	if (!i3200_registered) {
		i3200_remove_one(mci_pdev);
		pci_dev_put(mci_pdev);
	}
}

module_init(i3200_init);
module_exit(i3200_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akamai Technologies, Inc.");
MODULE_DESCRIPTION("MC support for Intel 3200 memory hub controllers");

module_param(edac_op_state, int, 0444);
MODULE_PARM_DESC(edac_op_state, "EDAC Error Reporting state: 0=Poll,1=NMI");
