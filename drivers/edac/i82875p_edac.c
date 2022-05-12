/*
 * Intel D82875P Memory Controller kernel module
 * (C) 2003 Linux Networx (http://lnxi.com)
 * This file may be distributed under the terms of the
 * GNU General Public License.
 *
 * Written by Thayne Harbaugh
 * Contributors:
 *	Wang Zhenyu at intel.com
 *
 * $Id: edac_i82875p.c,v 1.5.2.11 2005/10/05 00:43:44 dsp_llnl Exp $
 *
 * Note: E7210 appears same as D82875P - zhenyu.z.wang at intel.com
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/pci_ids.h>
#include <linux/edac.h>
#include "edac_module.h"

#define EDAC_MOD_STR		"i82875p_edac"

#define i82875p_printk(level, fmt, arg...) \
	edac_printk(level, "i82875p", fmt, ##arg)

#define i82875p_mc_printk(mci, level, fmt, arg...) \
	edac_mc_chipset_printk(mci, level, "i82875p", fmt, ##arg)

#ifndef PCI_DEVICE_ID_INTEL_82875_0
#define PCI_DEVICE_ID_INTEL_82875_0	0x2578
#endif				/* PCI_DEVICE_ID_INTEL_82875_0 */

#ifndef PCI_DEVICE_ID_INTEL_82875_6
#define PCI_DEVICE_ID_INTEL_82875_6	0x257e
#endif				/* PCI_DEVICE_ID_INTEL_82875_6 */

/* four csrows in dual channel, eight in single channel */
#define I82875P_NR_DIMMS		8
#define I82875P_NR_CSROWS(nr_chans)	(I82875P_NR_DIMMS / (nr_chans))

/* Intel 82875p register addresses - device 0 function 0 - DRAM Controller */
#define I82875P_EAP		0x58	/* Error Address Pointer (32b)
					 *
					 * 31:12 block address
					 * 11:0  reserved
					 */

#define I82875P_DERRSYN		0x5c	/* DRAM Error Syndrome (8b)
					 *
					 *  7:0  DRAM ECC Syndrome
					 */

#define I82875P_DES		0x5d	/* DRAM Error Status (8b)
					 *
					 *  7:1  reserved
					 *  0    Error channel 0/1
					 */

#define I82875P_ERRSTS		0xc8	/* Error Status Register (16b)
					 *
					 * 15:10 reserved
					 *  9    non-DRAM lock error (ndlock)
					 *  8    Sftwr Generated SMI
					 *  7    ECC UE
					 *  6    reserved
					 *  5    MCH detects unimplemented cycle
					 *  4    AGP access outside GA
					 *  3    Invalid AGP access
					 *  2    Invalid GA translation table
					 *  1    Unsupported AGP command
					 *  0    ECC CE
					 */

#define I82875P_ERRCMD		0xca	/* Error Command (16b)
					 *
					 * 15:10 reserved
					 *  9    SERR on non-DRAM lock
					 *  8    SERR on ECC UE
					 *  7    SERR on ECC CE
					 *  6    target abort on high exception
					 *  5    detect unimplemented cyc
					 *  4    AGP access outside of GA
					 *  3    SERR on invalid AGP access
					 *  2    invalid translation table
					 *  1    SERR on unsupported AGP command
					 *  0    reserved
					 */

/* Intel 82875p register addresses - device 6 function 0 - DRAM Controller */
#define I82875P_PCICMD6		0x04	/* PCI Command Register (16b)
					 *
					 * 15:10 reserved
					 *  9    fast back-to-back - ro 0
					 *  8    SERR enable - ro 0
					 *  7    addr/data stepping - ro 0
					 *  6    parity err enable - ro 0
					 *  5    VGA palette snoop - ro 0
					 *  4    mem wr & invalidate - ro 0
					 *  3    special cycle - ro 0
					 *  2    bus master - ro 0
					 *  1    mem access dev6 - 0(dis),1(en)
					 *  0    IO access dev3 - 0(dis),1(en)
					 */

#define I82875P_BAR6		0x10	/* Mem Delays Base ADDR Reg (32b)
					 *
					 * 31:12 mem base addr [31:12]
					 * 11:4  address mask - ro 0
					 *  3    prefetchable - ro 0(non),1(pre)
					 *  2:1  mem type - ro 0
					 *  0    mem space - ro 0
					 */

/* Intel 82875p MMIO register space - device 0 function 0 - MMR space */

#define I82875P_DRB_SHIFT 26	/* 64MiB grain */
#define I82875P_DRB		0x00	/* DRAM Row Boundary (8b x 8)
					 *
					 *  7    reserved
					 *  6:0  64MiB row boundary addr
					 */

#define I82875P_DRA		0x10	/* DRAM Row Attribute (4b x 8)
					 *
					 *  7    reserved
					 *  6:4  row attr row 1
					 *  3    reserved
					 *  2:0  row attr row 0
					 *
					 * 000 =  4KiB
					 * 001 =  8KiB
					 * 010 = 16KiB
					 * 011 = 32KiB
					 */

#define I82875P_DRC		0x68	/* DRAM Controller Mode (32b)
					 *
					 * 31:30 reserved
					 * 29    init complete
					 * 28:23 reserved
					 * 22:21 nr chan 00=1,01=2
					 * 20    reserved
					 * 19:18 Data Integ Mode 00=none,01=ecc
					 * 17:11 reserved
					 * 10:8  refresh mode
					 *  7    reserved
					 *  6:4  mode select
					 *  3:2  reserved
					 *  1:0  DRAM type 01=DDR
					 */

enum i82875p_chips {
	I82875P = 0,
};

struct i82875p_pvt {
	struct pci_dev *ovrfl_pdev;
	void __iomem *ovrfl_window;
};

struct i82875p_dev_info {
	const char *ctl_name;
};

struct i82875p_error_info {
	u16 errsts;
	u32 eap;
	u8 des;
	u8 derrsyn;
	u16 errsts2;
};

static const struct i82875p_dev_info i82875p_devs[] = {
	[I82875P] = {
		.ctl_name = "i82875p"},
};

static struct pci_dev *mci_pdev;	/* init dev: in case that AGP code has
					 * already registered driver
					 */

static struct edac_pci_ctl_info *i82875p_pci;

static void i82875p_get_error_info(struct mem_ctl_info *mci,
				struct i82875p_error_info *info)
{
	struct pci_dev *pdev;

	pdev = to_pci_dev(mci->pdev);

	/*
	 * This is a mess because there is no atomic way to read all the
	 * registers at once and the registers can transition from CE being
	 * overwritten by UE.
	 */
	pci_read_config_word(pdev, I82875P_ERRSTS, &info->errsts);

	if (!(info->errsts & 0x0081))
		return;

	pci_read_config_dword(pdev, I82875P_EAP, &info->eap);
	pci_read_config_byte(pdev, I82875P_DES, &info->des);
	pci_read_config_byte(pdev, I82875P_DERRSYN, &info->derrsyn);
	pci_read_config_word(pdev, I82875P_ERRSTS, &info->errsts2);

	/*
	 * If the error is the same then we can for both reads then
	 * the first set of reads is valid.  If there is a change then
	 * there is a CE no info and the second set of reads is valid
	 * and should be UE info.
	 */
	if ((info->errsts ^ info->errsts2) & 0x0081) {
		pci_read_config_dword(pdev, I82875P_EAP, &info->eap);
		pci_read_config_byte(pdev, I82875P_DES, &info->des);
		pci_read_config_byte(pdev, I82875P_DERRSYN, &info->derrsyn);
	}

	pci_write_bits16(pdev, I82875P_ERRSTS, 0x0081, 0x0081);
}

static int i82875p_process_error_info(struct mem_ctl_info *mci,
				struct i82875p_error_info *info,
				int handle_errors)
{
	int row, multi_chan;

	multi_chan = mci->csrows[0]->nr_channels - 1;

	if (!(info->errsts & 0x0081))
		return 0;

	if (!handle_errors)
		return 1;

	if ((info->errsts ^ info->errsts2) & 0x0081) {
		edac_mc_handle_error(HW_EVENT_ERR_UNCORRECTED, mci, 1, 0, 0, 0,
				     -1, -1, -1,
				     "UE overwrote CE", "");
		info->errsts = info->errsts2;
	}

	info->eap >>= PAGE_SHIFT;
	row = edac_mc_find_csrow_by_page(mci, info->eap);

	if (info->errsts & 0x0080)
		edac_mc_handle_error(HW_EVENT_ERR_UNCORRECTED, mci, 1,
				     info->eap, 0, 0,
				     row, -1, -1,
				     "i82875p UE", "");
	else
		edac_mc_handle_error(HW_EVENT_ERR_CORRECTED, mci, 1,
				     info->eap, 0, info->derrsyn,
				     row, multi_chan ? (info->des & 0x1) : 0,
				     -1, "i82875p CE", "");

	return 1;
}

static void i82875p_check(struct mem_ctl_info *mci)
{
	struct i82875p_error_info info;

	i82875p_get_error_info(mci, &info);
	i82875p_process_error_info(mci, &info, 1);
}

/* Return 0 on success or 1 on failure. */
static int i82875p_setup_overfl_dev(struct pci_dev *pdev,
				struct pci_dev **ovrfl_pdev,
				void __iomem **ovrfl_window)
{
	struct pci_dev *dev;
	void __iomem *window;

	*ovrfl_pdev = NULL;
	*ovrfl_window = NULL;
	dev = pci_get_device(PCI_VEND_DEV(INTEL, 82875_6), NULL);

	if (dev == NULL) {
		/* Intel tells BIOS developers to hide device 6 which
		 * configures the overflow device access containing
		 * the DRBs - this is where we expose device 6.
		 * http://www.x86-secret.com/articles/tweak/pat/patsecrets-2.htm
		 */
		pci_write_bits8(pdev, 0xf4, 0x2, 0x2);
		dev = pci_scan_single_device(pdev->bus, PCI_DEVFN(6, 0));

		if (dev == NULL)
			return 1;

		pci_bus_assign_resources(dev->bus);
		pci_bus_add_device(dev);
	}

	*ovrfl_pdev = dev;

	if (pci_enable_device(dev)) {
		i82875p_printk(KERN_ERR, "%s(): Failed to enable overflow "
			"device\n", __func__);
		return 1;
	}

	if (pci_request_regions(dev, pci_name(dev))) {
#ifdef CORRECT_BIOS
		goto fail0;
#endif
	}

	/* cache is irrelevant for PCI bus reads/writes */
	window = pci_ioremap_bar(dev, 0);
	if (window == NULL) {
		i82875p_printk(KERN_ERR, "%s(): Failed to ioremap bar6\n",
			__func__);
		goto fail1;
	}

	*ovrfl_window = window;
	return 0;

fail1:
	pci_release_regions(dev);

#ifdef CORRECT_BIOS
fail0:
	pci_disable_device(dev);
#endif
	/* NOTE: the ovrfl proc entry and pci_dev are intentionally left */
	return 1;
}

/* Return 1 if dual channel mode is active.  Else return 0. */
static inline int dual_channel_active(u32 drc)
{
	return (drc >> 21) & 0x1;
}

static void i82875p_init_csrows(struct mem_ctl_info *mci,
				struct pci_dev *pdev,
				void __iomem * ovrfl_window, u32 drc)
{
	struct csrow_info *csrow;
	struct dimm_info *dimm;
	unsigned nr_chans = dual_channel_active(drc) + 1;
	unsigned long last_cumul_size;
	u8 value;
	u32 drc_ddim;		/* DRAM Data Integrity Mode 0=none,2=edac */
	u32 cumul_size, nr_pages;
	int index, j;

	drc_ddim = (drc >> 18) & 0x1;
	last_cumul_size = 0;

	/* The dram row boundary (DRB) reg values are boundary address
	 * for each DRAM row with a granularity of 32 or 64MB (single/dual
	 * channel operation).  DRB regs are cumulative; therefore DRB7 will
	 * contain the total memory contained in all eight rows.
	 */

	for (index = 0; index < mci->nr_csrows; index++) {
		csrow = mci->csrows[index];

		value = readb(ovrfl_window + I82875P_DRB + index);
		cumul_size = value << (I82875P_DRB_SHIFT - PAGE_SHIFT);
		edac_dbg(3, "(%d) cumul_size 0x%x\n", index, cumul_size);
		if (cumul_size == last_cumul_size)
			continue;	/* not populated */

		csrow->first_page = last_cumul_size;
		csrow->last_page = cumul_size - 1;
		nr_pages = cumul_size - last_cumul_size;
		last_cumul_size = cumul_size;

		for (j = 0; j < nr_chans; j++) {
			dimm = csrow->channels[j]->dimm;

			dimm->nr_pages = nr_pages / nr_chans;
			dimm->grain = 1 << 12;	/* I82875P_EAP has 4KiB reolution */
			dimm->mtype = MEM_DDR;
			dimm->dtype = DEV_UNKNOWN;
			dimm->edac_mode = drc_ddim ? EDAC_SECDED : EDAC_NONE;
		}
	}
}

static int i82875p_probe1(struct pci_dev *pdev, int dev_idx)
{
	int rc = -ENODEV;
	struct mem_ctl_info *mci;
	struct edac_mc_layer layers[2];
	struct i82875p_pvt *pvt;
	struct pci_dev *ovrfl_pdev;
	void __iomem *ovrfl_window;
	u32 drc;
	u32 nr_chans;
	struct i82875p_error_info discard;

	edac_dbg(0, "\n");

	if (i82875p_setup_overfl_dev(pdev, &ovrfl_pdev, &ovrfl_window))
		return -ENODEV;
	drc = readl(ovrfl_window + I82875P_DRC);
	nr_chans = dual_channel_active(drc) + 1;

	layers[0].type = EDAC_MC_LAYER_CHIP_SELECT;
	layers[0].size = I82875P_NR_CSROWS(nr_chans);
	layers[0].is_virt_csrow = true;
	layers[1].type = EDAC_MC_LAYER_CHANNEL;
	layers[1].size = nr_chans;
	layers[1].is_virt_csrow = false;
	mci = edac_mc_alloc(0, ARRAY_SIZE(layers), layers, sizeof(*pvt));
	if (!mci) {
		rc = -ENOMEM;
		goto fail0;
	}

	edac_dbg(3, "init mci\n");
	mci->pdev = &pdev->dev;
	mci->mtype_cap = MEM_FLAG_DDR;
	mci->edac_ctl_cap = EDAC_FLAG_NONE | EDAC_FLAG_SECDED;
	mci->edac_cap = EDAC_FLAG_UNKNOWN;
	mci->mod_name = EDAC_MOD_STR;
	mci->ctl_name = i82875p_devs[dev_idx].ctl_name;
	mci->dev_name = pci_name(pdev);
	mci->edac_check = i82875p_check;
	mci->ctl_page_to_phys = NULL;
	edac_dbg(3, "init pvt\n");
	pvt = (struct i82875p_pvt *)mci->pvt_info;
	pvt->ovrfl_pdev = ovrfl_pdev;
	pvt->ovrfl_window = ovrfl_window;
	i82875p_init_csrows(mci, pdev, ovrfl_window, drc);
	i82875p_get_error_info(mci, &discard);	/* clear counters */

	/* Here we assume that we will never see multiple instances of this
	 * type of memory controller.  The ID is therefore hardcoded to 0.
	 */
	if (edac_mc_add_mc(mci)) {
		edac_dbg(3, "failed edac_mc_add_mc()\n");
		goto fail1;
	}

	/* allocating generic PCI control info */
	i82875p_pci = edac_pci_create_generic_ctl(&pdev->dev, EDAC_MOD_STR);
	if (!i82875p_pci) {
		printk(KERN_WARNING
			"%s(): Unable to create PCI control\n",
			__func__);
		printk(KERN_WARNING
			"%s(): PCI error report via EDAC not setup\n",
			__func__);
	}

	/* get this far and it's successful */
	edac_dbg(3, "success\n");
	return 0;

fail1:
	edac_mc_free(mci);

fail0:
	iounmap(ovrfl_window);
	pci_release_regions(ovrfl_pdev);

	pci_disable_device(ovrfl_pdev);
	/* NOTE: the ovrfl proc entry and pci_dev are intentionally left */
	return rc;
}

/* returns count (>= 0), or negative on error */
static int i82875p_init_one(struct pci_dev *pdev,
			    const struct pci_device_id *ent)
{
	int rc;

	edac_dbg(0, "\n");
	i82875p_printk(KERN_INFO, "i82875p init one\n");

	if (pci_enable_device(pdev) < 0)
		return -EIO;

	rc = i82875p_probe1(pdev, ent->driver_data);

	if (mci_pdev == NULL)
		mci_pdev = pci_dev_get(pdev);

	return rc;
}

static void i82875p_remove_one(struct pci_dev *pdev)
{
	struct mem_ctl_info *mci;
	struct i82875p_pvt *pvt = NULL;

	edac_dbg(0, "\n");

	if (i82875p_pci)
		edac_pci_release_generic_ctl(i82875p_pci);

	if ((mci = edac_mc_del_mc(&pdev->dev)) == NULL)
		return;

	pvt = (struct i82875p_pvt *)mci->pvt_info;

	if (pvt->ovrfl_window)
		iounmap(pvt->ovrfl_window);

	if (pvt->ovrfl_pdev) {
#ifdef CORRECT_BIOS
		pci_release_regions(pvt->ovrfl_pdev);
#endif				/*CORRECT_BIOS */
		pci_disable_device(pvt->ovrfl_pdev);
		pci_dev_put(pvt->ovrfl_pdev);
	}

	edac_mc_free(mci);
}

static const struct pci_device_id i82875p_pci_tbl[] = {
	{
	 PCI_VEND_DEV(INTEL, 82875_0), PCI_ANY_ID, PCI_ANY_ID, 0, 0,
	 I82875P},
	{
	 0,
	 }			/* 0 terminated list. */
};

MODULE_DEVICE_TABLE(pci, i82875p_pci_tbl);

static struct pci_driver i82875p_driver = {
	.name = EDAC_MOD_STR,
	.probe = i82875p_init_one,
	.remove = i82875p_remove_one,
	.id_table = i82875p_pci_tbl,
};

static int __init i82875p_init(void)
{
	int pci_rc;

	edac_dbg(3, "\n");

       /* Ensure that the OPSTATE is set correctly for POLL or NMI */
       opstate_init();

	pci_rc = pci_register_driver(&i82875p_driver);

	if (pci_rc < 0)
		goto fail0;

	if (mci_pdev == NULL) {
		mci_pdev = pci_get_device(PCI_VENDOR_ID_INTEL,
					PCI_DEVICE_ID_INTEL_82875_0, NULL);

		if (!mci_pdev) {
			edac_dbg(0, "875p pci_get_device fail\n");
			pci_rc = -ENODEV;
			goto fail1;
		}

		pci_rc = i82875p_init_one(mci_pdev, i82875p_pci_tbl);

		if (pci_rc < 0) {
			edac_dbg(0, "875p init fail\n");
			pci_rc = -ENODEV;
			goto fail1;
		}
	}

	return 0;

fail1:
	pci_unregister_driver(&i82875p_driver);

fail0:
	pci_dev_put(mci_pdev);
	return pci_rc;
}

static void __exit i82875p_exit(void)
{
	edac_dbg(3, "\n");

	i82875p_remove_one(mci_pdev);
	pci_dev_put(mci_pdev);

	pci_unregister_driver(&i82875p_driver);

}

module_init(i82875p_init);
module_exit(i82875p_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Linux Networx (http://lnxi.com) Thayne Harbaugh");
MODULE_DESCRIPTION("MC support for Intel 82875 memory hub controllers");

module_param(edac_op_state, int, 0444);
MODULE_PARM_DESC(edac_op_state, "EDAC Error Reporting state: 0=Poll,1=NMI");
