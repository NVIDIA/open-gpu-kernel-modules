/* bnx2.c: QLogic bnx2 network driver.
 *
 * Copyright (c) 2004-2014 Broadcom Corporation
 * Copyright (c) 2014-2015 QLogic Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * Written by: Michael Chan  (mchan@broadcom.com)
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/stringify.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/dma-mapping.h>
#include <linux/bitops.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <linux/delay.h>
#include <asm/byteorder.h>
#include <asm/page.h>
#include <linux/time.h>
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/if.h>
#include <linux/if_vlan.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/checksum.h>
#include <linux/workqueue.h>
#include <linux/crc32.h>
#include <linux/prefetch.h>
#include <linux/cache.h>
#include <linux/firmware.h>
#include <linux/log2.h>
#include <linux/aer.h>
#include <linux/crash_dump.h>

#if IS_ENABLED(CONFIG_CNIC)
#define BCM_CNIC 1
#include "cnic_if.h"
#endif
#include "bnx2.h"
#include "bnx2_fw.h"

#define DRV_MODULE_NAME		"bnx2"
#define FW_MIPS_FILE_06		"bnx2/bnx2-mips-06-6.2.3.fw"
#define FW_RV2P_FILE_06		"bnx2/bnx2-rv2p-06-6.0.15.fw"
#define FW_MIPS_FILE_09		"bnx2/bnx2-mips-09-6.2.1b.fw"
#define FW_RV2P_FILE_09_Ax	"bnx2/bnx2-rv2p-09ax-6.0.17.fw"
#define FW_RV2P_FILE_09		"bnx2/bnx2-rv2p-09-6.0.17.fw"

#define RUN_AT(x) (jiffies + (x))

/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT  (5*HZ)

MODULE_AUTHOR("Michael Chan <mchan@broadcom.com>");
MODULE_DESCRIPTION("QLogic BCM5706/5708/5709/5716 Driver");
MODULE_LICENSE("GPL");
MODULE_FIRMWARE(FW_MIPS_FILE_06);
MODULE_FIRMWARE(FW_RV2P_FILE_06);
MODULE_FIRMWARE(FW_MIPS_FILE_09);
MODULE_FIRMWARE(FW_RV2P_FILE_09);
MODULE_FIRMWARE(FW_RV2P_FILE_09_Ax);

static int disable_msi = 0;

module_param(disable_msi, int, 0444);
MODULE_PARM_DESC(disable_msi, "Disable Message Signaled Interrupt (MSI)");

typedef enum {
	BCM5706 = 0,
	NC370T,
	NC370I,
	BCM5706S,
	NC370F,
	BCM5708,
	BCM5708S,
	BCM5709,
	BCM5709S,
	BCM5716,
	BCM5716S,
} board_t;

/* indexed by board_t, above */
static struct {
	char *name;
} board_info[] = {
	{ "Broadcom NetXtreme II BCM5706 1000Base-T" },
	{ "HP NC370T Multifunction Gigabit Server Adapter" },
	{ "HP NC370i Multifunction Gigabit Server Adapter" },
	{ "Broadcom NetXtreme II BCM5706 1000Base-SX" },
	{ "HP NC370F Multifunction Gigabit Server Adapter" },
	{ "Broadcom NetXtreme II BCM5708 1000Base-T" },
	{ "Broadcom NetXtreme II BCM5708 1000Base-SX" },
	{ "Broadcom NetXtreme II BCM5709 1000Base-T" },
	{ "Broadcom NetXtreme II BCM5709 1000Base-SX" },
	{ "Broadcom NetXtreme II BCM5716 1000Base-T" },
	{ "Broadcom NetXtreme II BCM5716 1000Base-SX" },
	};

static const struct pci_device_id bnx2_pci_tbl[] = {
	{ PCI_VENDOR_ID_BROADCOM, PCI_DEVICE_ID_NX2_5706,
	  PCI_VENDOR_ID_HP, 0x3101, 0, 0, NC370T },
	{ PCI_VENDOR_ID_BROADCOM, PCI_DEVICE_ID_NX2_5706,
	  PCI_VENDOR_ID_HP, 0x3106, 0, 0, NC370I },
	{ PCI_VENDOR_ID_BROADCOM, PCI_DEVICE_ID_NX2_5706,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5706 },
	{ PCI_VENDOR_ID_BROADCOM, PCI_DEVICE_ID_NX2_5708,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5708 },
	{ PCI_VENDOR_ID_BROADCOM, PCI_DEVICE_ID_NX2_5706S,
	  PCI_VENDOR_ID_HP, 0x3102, 0, 0, NC370F },
	{ PCI_VENDOR_ID_BROADCOM, PCI_DEVICE_ID_NX2_5706S,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5706S },
	{ PCI_VENDOR_ID_BROADCOM, PCI_DEVICE_ID_NX2_5708S,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5708S },
	{ PCI_VENDOR_ID_BROADCOM, PCI_DEVICE_ID_NX2_5709,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5709 },
	{ PCI_VENDOR_ID_BROADCOM, PCI_DEVICE_ID_NX2_5709S,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5709S },
	{ PCI_VENDOR_ID_BROADCOM, 0x163b,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5716 },
	{ PCI_VENDOR_ID_BROADCOM, 0x163c,
	  PCI_ANY_ID, PCI_ANY_ID, 0, 0, BCM5716S },
	{ 0, }
};

static const struct flash_spec flash_table[] =
{
#define BUFFERED_FLAGS		(BNX2_NV_BUFFERED | BNX2_NV_TRANSLATE)
#define NONBUFFERED_FLAGS	(BNX2_NV_WREN)
	/* Slow EEPROM */
	{0x00000000, 0x40830380, 0x009f0081, 0xa184a053, 0xaf000400,
	 BUFFERED_FLAGS, SEEPROM_PAGE_BITS, SEEPROM_PAGE_SIZE,
	 SEEPROM_BYTE_ADDR_MASK, SEEPROM_TOTAL_SIZE,
	 "EEPROM - slow"},
	/* Expansion entry 0001 */
	{0x08000002, 0x4b808201, 0x00050081, 0x03840253, 0xaf020406,
	 NONBUFFERED_FLAGS, SAIFUN_FLASH_PAGE_BITS, SAIFUN_FLASH_PAGE_SIZE,
	 SAIFUN_FLASH_BYTE_ADDR_MASK, 0,
	 "Entry 0001"},
	/* Saifun SA25F010 (non-buffered flash) */
	/* strap, cfg1, & write1 need updates */
	{0x04000001, 0x47808201, 0x00050081, 0x03840253, 0xaf020406,
	 NONBUFFERED_FLAGS, SAIFUN_FLASH_PAGE_BITS, SAIFUN_FLASH_PAGE_SIZE,
	 SAIFUN_FLASH_BYTE_ADDR_MASK, SAIFUN_FLASH_BASE_TOTAL_SIZE*2,
	 "Non-buffered flash (128kB)"},
	/* Saifun SA25F020 (non-buffered flash) */
	/* strap, cfg1, & write1 need updates */
	{0x0c000003, 0x4f808201, 0x00050081, 0x03840253, 0xaf020406,
	 NONBUFFERED_FLAGS, SAIFUN_FLASH_PAGE_BITS, SAIFUN_FLASH_PAGE_SIZE,
	 SAIFUN_FLASH_BYTE_ADDR_MASK, SAIFUN_FLASH_BASE_TOTAL_SIZE*4,
	 "Non-buffered flash (256kB)"},
	/* Expansion entry 0100 */
	{0x11000000, 0x53808201, 0x00050081, 0x03840253, 0xaf020406,
	 NONBUFFERED_FLAGS, SAIFUN_FLASH_PAGE_BITS, SAIFUN_FLASH_PAGE_SIZE,
	 SAIFUN_FLASH_BYTE_ADDR_MASK, 0,
	 "Entry 0100"},
	/* Entry 0101: ST M45PE10 (non-buffered flash, TetonII B0) */
	{0x19000002, 0x5b808201, 0x000500db, 0x03840253, 0xaf020406,
	 NONBUFFERED_FLAGS, ST_MICRO_FLASH_PAGE_BITS, ST_MICRO_FLASH_PAGE_SIZE,
	 ST_MICRO_FLASH_BYTE_ADDR_MASK, ST_MICRO_FLASH_BASE_TOTAL_SIZE*2,
	 "Entry 0101: ST M45PE10 (128kB non-bufferred)"},
	/* Entry 0110: ST M45PE20 (non-buffered flash)*/
	{0x15000001, 0x57808201, 0x000500db, 0x03840253, 0xaf020406,
	 NONBUFFERED_FLAGS, ST_MICRO_FLASH_PAGE_BITS, ST_MICRO_FLASH_PAGE_SIZE,
	 ST_MICRO_FLASH_BYTE_ADDR_MASK, ST_MICRO_FLASH_BASE_TOTAL_SIZE*4,
	 "Entry 0110: ST M45PE20 (256kB non-bufferred)"},
	/* Saifun SA25F005 (non-buffered flash) */
	/* strap, cfg1, & write1 need updates */
	{0x1d000003, 0x5f808201, 0x00050081, 0x03840253, 0xaf020406,
	 NONBUFFERED_FLAGS, SAIFUN_FLASH_PAGE_BITS, SAIFUN_FLASH_PAGE_SIZE,
	 SAIFUN_FLASH_BYTE_ADDR_MASK, SAIFUN_FLASH_BASE_TOTAL_SIZE,
	 "Non-buffered flash (64kB)"},
	/* Fast EEPROM */
	{0x22000000, 0x62808380, 0x009f0081, 0xa184a053, 0xaf000400,
	 BUFFERED_FLAGS, SEEPROM_PAGE_BITS, SEEPROM_PAGE_SIZE,
	 SEEPROM_BYTE_ADDR_MASK, SEEPROM_TOTAL_SIZE,
	 "EEPROM - fast"},
	/* Expansion entry 1001 */
	{0x2a000002, 0x6b808201, 0x00050081, 0x03840253, 0xaf020406,
	 NONBUFFERED_FLAGS, SAIFUN_FLASH_PAGE_BITS, SAIFUN_FLASH_PAGE_SIZE,
	 SAIFUN_FLASH_BYTE_ADDR_MASK, 0,
	 "Entry 1001"},
	/* Expansion entry 1010 */
	{0x26000001, 0x67808201, 0x00050081, 0x03840253, 0xaf020406,
	 NONBUFFERED_FLAGS, SAIFUN_FLASH_PAGE_BITS, SAIFUN_FLASH_PAGE_SIZE,
	 SAIFUN_FLASH_BYTE_ADDR_MASK, 0,
	 "Entry 1010"},
	/* ATMEL AT45DB011B (buffered flash) */
	{0x2e000003, 0x6e808273, 0x00570081, 0x68848353, 0xaf000400,
	 BUFFERED_FLAGS, BUFFERED_FLASH_PAGE_BITS, BUFFERED_FLASH_PAGE_SIZE,
	 BUFFERED_FLASH_BYTE_ADDR_MASK, BUFFERED_FLASH_TOTAL_SIZE,
	 "Buffered flash (128kB)"},
	/* Expansion entry 1100 */
	{0x33000000, 0x73808201, 0x00050081, 0x03840253, 0xaf020406,
	 NONBUFFERED_FLAGS, SAIFUN_FLASH_PAGE_BITS, SAIFUN_FLASH_PAGE_SIZE,
	 SAIFUN_FLASH_BYTE_ADDR_MASK, 0,
	 "Entry 1100"},
	/* Expansion entry 1101 */
	{0x3b000002, 0x7b808201, 0x00050081, 0x03840253, 0xaf020406,
	 NONBUFFERED_FLAGS, SAIFUN_FLASH_PAGE_BITS, SAIFUN_FLASH_PAGE_SIZE,
	 SAIFUN_FLASH_BYTE_ADDR_MASK, 0,
	 "Entry 1101"},
	/* Ateml Expansion entry 1110 */
	{0x37000001, 0x76808273, 0x00570081, 0x68848353, 0xaf000400,
	 BUFFERED_FLAGS, BUFFERED_FLASH_PAGE_BITS, BUFFERED_FLASH_PAGE_SIZE,
	 BUFFERED_FLASH_BYTE_ADDR_MASK, 0,
	 "Entry 1110 (Atmel)"},
	/* ATMEL AT45DB021B (buffered flash) */
	{0x3f000003, 0x7e808273, 0x00570081, 0x68848353, 0xaf000400,
	 BUFFERED_FLAGS, BUFFERED_FLASH_PAGE_BITS, BUFFERED_FLASH_PAGE_SIZE,
	 BUFFERED_FLASH_BYTE_ADDR_MASK, BUFFERED_FLASH_TOTAL_SIZE*2,
	 "Buffered flash (256kB)"},
};

static const struct flash_spec flash_5709 = {
	.flags		= BNX2_NV_BUFFERED,
	.page_bits	= BCM5709_FLASH_PAGE_BITS,
	.page_size	= BCM5709_FLASH_PAGE_SIZE,
	.addr_mask	= BCM5709_FLASH_BYTE_ADDR_MASK,
	.total_size	= BUFFERED_FLASH_TOTAL_SIZE*2,
	.name		= "5709 Buffered flash (256kB)",
};

MODULE_DEVICE_TABLE(pci, bnx2_pci_tbl);

static void bnx2_init_napi(struct bnx2 *bp);
static void bnx2_del_napi(struct bnx2 *bp);

static inline u32 bnx2_tx_avail(struct bnx2 *bp, struct bnx2_tx_ring_info *txr)
{
	u32 diff;

	/* The ring uses 256 indices for 255 entries, one of them
	 * needs to be skipped.
	 */
	diff = READ_ONCE(txr->tx_prod) - READ_ONCE(txr->tx_cons);
	if (unlikely(diff >= BNX2_TX_DESC_CNT)) {
		diff &= 0xffff;
		if (diff == BNX2_TX_DESC_CNT)
			diff = BNX2_MAX_TX_DESC_CNT;
	}
	return bp->tx_ring_size - diff;
}

static u32
bnx2_reg_rd_ind(struct bnx2 *bp, u32 offset)
{
	unsigned long flags;
	u32 val;

	spin_lock_irqsave(&bp->indirect_lock, flags);
	BNX2_WR(bp, BNX2_PCICFG_REG_WINDOW_ADDRESS, offset);
	val = BNX2_RD(bp, BNX2_PCICFG_REG_WINDOW);
	spin_unlock_irqrestore(&bp->indirect_lock, flags);
	return val;
}

static void
bnx2_reg_wr_ind(struct bnx2 *bp, u32 offset, u32 val)
{
	unsigned long flags;

	spin_lock_irqsave(&bp->indirect_lock, flags);
	BNX2_WR(bp, BNX2_PCICFG_REG_WINDOW_ADDRESS, offset);
	BNX2_WR(bp, BNX2_PCICFG_REG_WINDOW, val);
	spin_unlock_irqrestore(&bp->indirect_lock, flags);
}

static void
bnx2_shmem_wr(struct bnx2 *bp, u32 offset, u32 val)
{
	bnx2_reg_wr_ind(bp, bp->shmem_base + offset, val);
}

static u32
bnx2_shmem_rd(struct bnx2 *bp, u32 offset)
{
	return bnx2_reg_rd_ind(bp, bp->shmem_base + offset);
}

static void
bnx2_ctx_wr(struct bnx2 *bp, u32 cid_addr, u32 offset, u32 val)
{
	unsigned long flags;

	offset += cid_addr;
	spin_lock_irqsave(&bp->indirect_lock, flags);
	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		int i;

		BNX2_WR(bp, BNX2_CTX_CTX_DATA, val);
		BNX2_WR(bp, BNX2_CTX_CTX_CTRL,
			offset | BNX2_CTX_CTX_CTRL_WRITE_REQ);
		for (i = 0; i < 5; i++) {
			val = BNX2_RD(bp, BNX2_CTX_CTX_CTRL);
			if ((val & BNX2_CTX_CTX_CTRL_WRITE_REQ) == 0)
				break;
			udelay(5);
		}
	} else {
		BNX2_WR(bp, BNX2_CTX_DATA_ADR, offset);
		BNX2_WR(bp, BNX2_CTX_DATA, val);
	}
	spin_unlock_irqrestore(&bp->indirect_lock, flags);
}

#ifdef BCM_CNIC
static int
bnx2_drv_ctl(struct net_device *dev, struct drv_ctl_info *info)
{
	struct bnx2 *bp = netdev_priv(dev);
	struct drv_ctl_io *io = &info->data.io;

	switch (info->cmd) {
	case DRV_CTL_IO_WR_CMD:
		bnx2_reg_wr_ind(bp, io->offset, io->data);
		break;
	case DRV_CTL_IO_RD_CMD:
		io->data = bnx2_reg_rd_ind(bp, io->offset);
		break;
	case DRV_CTL_CTX_WR_CMD:
		bnx2_ctx_wr(bp, io->cid_addr, io->offset, io->data);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static void bnx2_setup_cnic_irq_info(struct bnx2 *bp)
{
	struct cnic_eth_dev *cp = &bp->cnic_eth_dev;
	struct bnx2_napi *bnapi = &bp->bnx2_napi[0];
	int sb_id;

	if (bp->flags & BNX2_FLAG_USING_MSIX) {
		cp->drv_state |= CNIC_DRV_STATE_USING_MSIX;
		bnapi->cnic_present = 0;
		sb_id = bp->irq_nvecs;
		cp->irq_arr[0].irq_flags |= CNIC_IRQ_FL_MSIX;
	} else {
		cp->drv_state &= ~CNIC_DRV_STATE_USING_MSIX;
		bnapi->cnic_tag = bnapi->last_status_idx;
		bnapi->cnic_present = 1;
		sb_id = 0;
		cp->irq_arr[0].irq_flags &= ~CNIC_IRQ_FL_MSIX;
	}

	cp->irq_arr[0].vector = bp->irq_tbl[sb_id].vector;
	cp->irq_arr[0].status_blk = (void *)
		((unsigned long) bnapi->status_blk.msi +
		(BNX2_SBLK_MSIX_ALIGN_SIZE * sb_id));
	cp->irq_arr[0].status_blk_num = sb_id;
	cp->num_irq = 1;
}

static int bnx2_register_cnic(struct net_device *dev, struct cnic_ops *ops,
			      void *data)
{
	struct bnx2 *bp = netdev_priv(dev);
	struct cnic_eth_dev *cp = &bp->cnic_eth_dev;

	if (!ops)
		return -EINVAL;

	if (cp->drv_state & CNIC_DRV_STATE_REGD)
		return -EBUSY;

	if (!bnx2_reg_rd_ind(bp, BNX2_FW_MAX_ISCSI_CONN))
		return -ENODEV;

	bp->cnic_data = data;
	rcu_assign_pointer(bp->cnic_ops, ops);

	cp->num_irq = 0;
	cp->drv_state = CNIC_DRV_STATE_REGD;

	bnx2_setup_cnic_irq_info(bp);

	return 0;
}

static int bnx2_unregister_cnic(struct net_device *dev)
{
	struct bnx2 *bp = netdev_priv(dev);
	struct bnx2_napi *bnapi = &bp->bnx2_napi[0];
	struct cnic_eth_dev *cp = &bp->cnic_eth_dev;

	mutex_lock(&bp->cnic_lock);
	cp->drv_state = 0;
	bnapi->cnic_present = 0;
	RCU_INIT_POINTER(bp->cnic_ops, NULL);
	mutex_unlock(&bp->cnic_lock);
	synchronize_rcu();
	return 0;
}

static struct cnic_eth_dev *bnx2_cnic_probe(struct net_device *dev)
{
	struct bnx2 *bp = netdev_priv(dev);
	struct cnic_eth_dev *cp = &bp->cnic_eth_dev;

	if (!cp->max_iscsi_conn)
		return NULL;

	cp->drv_owner = THIS_MODULE;
	cp->chip_id = bp->chip_id;
	cp->pdev = bp->pdev;
	cp->io_base = bp->regview;
	cp->drv_ctl = bnx2_drv_ctl;
	cp->drv_register_cnic = bnx2_register_cnic;
	cp->drv_unregister_cnic = bnx2_unregister_cnic;

	return cp;
}

static void
bnx2_cnic_stop(struct bnx2 *bp)
{
	struct cnic_ops *c_ops;
	struct cnic_ctl_info info;

	mutex_lock(&bp->cnic_lock);
	c_ops = rcu_dereference_protected(bp->cnic_ops,
					  lockdep_is_held(&bp->cnic_lock));
	if (c_ops) {
		info.cmd = CNIC_CTL_STOP_CMD;
		c_ops->cnic_ctl(bp->cnic_data, &info);
	}
	mutex_unlock(&bp->cnic_lock);
}

static void
bnx2_cnic_start(struct bnx2 *bp)
{
	struct cnic_ops *c_ops;
	struct cnic_ctl_info info;

	mutex_lock(&bp->cnic_lock);
	c_ops = rcu_dereference_protected(bp->cnic_ops,
					  lockdep_is_held(&bp->cnic_lock));
	if (c_ops) {
		if (!(bp->flags & BNX2_FLAG_USING_MSIX)) {
			struct bnx2_napi *bnapi = &bp->bnx2_napi[0];

			bnapi->cnic_tag = bnapi->last_status_idx;
		}
		info.cmd = CNIC_CTL_START_CMD;
		c_ops->cnic_ctl(bp->cnic_data, &info);
	}
	mutex_unlock(&bp->cnic_lock);
}

#else

static void
bnx2_cnic_stop(struct bnx2 *bp)
{
}

static void
bnx2_cnic_start(struct bnx2 *bp)
{
}

#endif

static int
bnx2_read_phy(struct bnx2 *bp, u32 reg, u32 *val)
{
	u32 val1;
	int i, ret;

	if (bp->phy_flags & BNX2_PHY_FLAG_INT_MODE_AUTO_POLLING) {
		val1 = BNX2_RD(bp, BNX2_EMAC_MDIO_MODE);
		val1 &= ~BNX2_EMAC_MDIO_MODE_AUTO_POLL;

		BNX2_WR(bp, BNX2_EMAC_MDIO_MODE, val1);
		BNX2_RD(bp, BNX2_EMAC_MDIO_MODE);

		udelay(40);
	}

	val1 = (bp->phy_addr << 21) | (reg << 16) |
		BNX2_EMAC_MDIO_COMM_COMMAND_READ | BNX2_EMAC_MDIO_COMM_DISEXT |
		BNX2_EMAC_MDIO_COMM_START_BUSY;
	BNX2_WR(bp, BNX2_EMAC_MDIO_COMM, val1);

	for (i = 0; i < 50; i++) {
		udelay(10);

		val1 = BNX2_RD(bp, BNX2_EMAC_MDIO_COMM);
		if (!(val1 & BNX2_EMAC_MDIO_COMM_START_BUSY)) {
			udelay(5);

			val1 = BNX2_RD(bp, BNX2_EMAC_MDIO_COMM);
			val1 &= BNX2_EMAC_MDIO_COMM_DATA;

			break;
		}
	}

	if (val1 & BNX2_EMAC_MDIO_COMM_START_BUSY) {
		*val = 0x0;
		ret = -EBUSY;
	}
	else {
		*val = val1;
		ret = 0;
	}

	if (bp->phy_flags & BNX2_PHY_FLAG_INT_MODE_AUTO_POLLING) {
		val1 = BNX2_RD(bp, BNX2_EMAC_MDIO_MODE);
		val1 |= BNX2_EMAC_MDIO_MODE_AUTO_POLL;

		BNX2_WR(bp, BNX2_EMAC_MDIO_MODE, val1);
		BNX2_RD(bp, BNX2_EMAC_MDIO_MODE);

		udelay(40);
	}

	return ret;
}

static int
bnx2_write_phy(struct bnx2 *bp, u32 reg, u32 val)
{
	u32 val1;
	int i, ret;

	if (bp->phy_flags & BNX2_PHY_FLAG_INT_MODE_AUTO_POLLING) {
		val1 = BNX2_RD(bp, BNX2_EMAC_MDIO_MODE);
		val1 &= ~BNX2_EMAC_MDIO_MODE_AUTO_POLL;

		BNX2_WR(bp, BNX2_EMAC_MDIO_MODE, val1);
		BNX2_RD(bp, BNX2_EMAC_MDIO_MODE);

		udelay(40);
	}

	val1 = (bp->phy_addr << 21) | (reg << 16) | val |
		BNX2_EMAC_MDIO_COMM_COMMAND_WRITE |
		BNX2_EMAC_MDIO_COMM_START_BUSY | BNX2_EMAC_MDIO_COMM_DISEXT;
	BNX2_WR(bp, BNX2_EMAC_MDIO_COMM, val1);

	for (i = 0; i < 50; i++) {
		udelay(10);

		val1 = BNX2_RD(bp, BNX2_EMAC_MDIO_COMM);
		if (!(val1 & BNX2_EMAC_MDIO_COMM_START_BUSY)) {
			udelay(5);
			break;
		}
	}

	if (val1 & BNX2_EMAC_MDIO_COMM_START_BUSY)
        	ret = -EBUSY;
	else
		ret = 0;

	if (bp->phy_flags & BNX2_PHY_FLAG_INT_MODE_AUTO_POLLING) {
		val1 = BNX2_RD(bp, BNX2_EMAC_MDIO_MODE);
		val1 |= BNX2_EMAC_MDIO_MODE_AUTO_POLL;

		BNX2_WR(bp, BNX2_EMAC_MDIO_MODE, val1);
		BNX2_RD(bp, BNX2_EMAC_MDIO_MODE);

		udelay(40);
	}

	return ret;
}

static void
bnx2_disable_int(struct bnx2 *bp)
{
	int i;
	struct bnx2_napi *bnapi;

	for (i = 0; i < bp->irq_nvecs; i++) {
		bnapi = &bp->bnx2_napi[i];
		BNX2_WR(bp, BNX2_PCICFG_INT_ACK_CMD, bnapi->int_num |
		       BNX2_PCICFG_INT_ACK_CMD_MASK_INT);
	}
	BNX2_RD(bp, BNX2_PCICFG_INT_ACK_CMD);
}

static void
bnx2_enable_int(struct bnx2 *bp)
{
	int i;
	struct bnx2_napi *bnapi;

	for (i = 0; i < bp->irq_nvecs; i++) {
		bnapi = &bp->bnx2_napi[i];

		BNX2_WR(bp, BNX2_PCICFG_INT_ACK_CMD, bnapi->int_num |
			BNX2_PCICFG_INT_ACK_CMD_INDEX_VALID |
			BNX2_PCICFG_INT_ACK_CMD_MASK_INT |
			bnapi->last_status_idx);

		BNX2_WR(bp, BNX2_PCICFG_INT_ACK_CMD, bnapi->int_num |
			BNX2_PCICFG_INT_ACK_CMD_INDEX_VALID |
			bnapi->last_status_idx);
	}
	BNX2_WR(bp, BNX2_HC_COMMAND, bp->hc_cmd | BNX2_HC_COMMAND_COAL_NOW);
}

static void
bnx2_disable_int_sync(struct bnx2 *bp)
{
	int i;

	atomic_inc(&bp->intr_sem);
	if (!netif_running(bp->dev))
		return;

	bnx2_disable_int(bp);
	for (i = 0; i < bp->irq_nvecs; i++)
		synchronize_irq(bp->irq_tbl[i].vector);
}

static void
bnx2_napi_disable(struct bnx2 *bp)
{
	int i;

	for (i = 0; i < bp->irq_nvecs; i++)
		napi_disable(&bp->bnx2_napi[i].napi);
}

static void
bnx2_napi_enable(struct bnx2 *bp)
{
	int i;

	for (i = 0; i < bp->irq_nvecs; i++)
		napi_enable(&bp->bnx2_napi[i].napi);
}

static void
bnx2_netif_stop(struct bnx2 *bp, bool stop_cnic)
{
	if (stop_cnic)
		bnx2_cnic_stop(bp);
	if (netif_running(bp->dev)) {
		bnx2_napi_disable(bp);
		netif_tx_disable(bp->dev);
	}
	bnx2_disable_int_sync(bp);
	netif_carrier_off(bp->dev);	/* prevent tx timeout */
}

static void
bnx2_netif_start(struct bnx2 *bp, bool start_cnic)
{
	if (atomic_dec_and_test(&bp->intr_sem)) {
		if (netif_running(bp->dev)) {
			netif_tx_wake_all_queues(bp->dev);
			spin_lock_bh(&bp->phy_lock);
			if (bp->link_up)
				netif_carrier_on(bp->dev);
			spin_unlock_bh(&bp->phy_lock);
			bnx2_napi_enable(bp);
			bnx2_enable_int(bp);
			if (start_cnic)
				bnx2_cnic_start(bp);
		}
	}
}

static void
bnx2_free_tx_mem(struct bnx2 *bp)
{
	int i;

	for (i = 0; i < bp->num_tx_rings; i++) {
		struct bnx2_napi *bnapi = &bp->bnx2_napi[i];
		struct bnx2_tx_ring_info *txr = &bnapi->tx_ring;

		if (txr->tx_desc_ring) {
			dma_free_coherent(&bp->pdev->dev, TXBD_RING_SIZE,
					  txr->tx_desc_ring,
					  txr->tx_desc_mapping);
			txr->tx_desc_ring = NULL;
		}
		kfree(txr->tx_buf_ring);
		txr->tx_buf_ring = NULL;
	}
}

static void
bnx2_free_rx_mem(struct bnx2 *bp)
{
	int i;

	for (i = 0; i < bp->num_rx_rings; i++) {
		struct bnx2_napi *bnapi = &bp->bnx2_napi[i];
		struct bnx2_rx_ring_info *rxr = &bnapi->rx_ring;
		int j;

		for (j = 0; j < bp->rx_max_ring; j++) {
			if (rxr->rx_desc_ring[j])
				dma_free_coherent(&bp->pdev->dev, RXBD_RING_SIZE,
						  rxr->rx_desc_ring[j],
						  rxr->rx_desc_mapping[j]);
			rxr->rx_desc_ring[j] = NULL;
		}
		vfree(rxr->rx_buf_ring);
		rxr->rx_buf_ring = NULL;

		for (j = 0; j < bp->rx_max_pg_ring; j++) {
			if (rxr->rx_pg_desc_ring[j])
				dma_free_coherent(&bp->pdev->dev, RXBD_RING_SIZE,
						  rxr->rx_pg_desc_ring[j],
						  rxr->rx_pg_desc_mapping[j]);
			rxr->rx_pg_desc_ring[j] = NULL;
		}
		vfree(rxr->rx_pg_ring);
		rxr->rx_pg_ring = NULL;
	}
}

static int
bnx2_alloc_tx_mem(struct bnx2 *bp)
{
	int i;

	for (i = 0; i < bp->num_tx_rings; i++) {
		struct bnx2_napi *bnapi = &bp->bnx2_napi[i];
		struct bnx2_tx_ring_info *txr = &bnapi->tx_ring;

		txr->tx_buf_ring = kzalloc(SW_TXBD_RING_SIZE, GFP_KERNEL);
		if (!txr->tx_buf_ring)
			return -ENOMEM;

		txr->tx_desc_ring =
			dma_alloc_coherent(&bp->pdev->dev, TXBD_RING_SIZE,
					   &txr->tx_desc_mapping, GFP_KERNEL);
		if (!txr->tx_desc_ring)
			return -ENOMEM;
	}
	return 0;
}

static int
bnx2_alloc_rx_mem(struct bnx2 *bp)
{
	int i;

	for (i = 0; i < bp->num_rx_rings; i++) {
		struct bnx2_napi *bnapi = &bp->bnx2_napi[i];
		struct bnx2_rx_ring_info *rxr = &bnapi->rx_ring;
		int j;

		rxr->rx_buf_ring =
			vzalloc(array_size(SW_RXBD_RING_SIZE, bp->rx_max_ring));
		if (!rxr->rx_buf_ring)
			return -ENOMEM;

		for (j = 0; j < bp->rx_max_ring; j++) {
			rxr->rx_desc_ring[j] =
				dma_alloc_coherent(&bp->pdev->dev,
						   RXBD_RING_SIZE,
						   &rxr->rx_desc_mapping[j],
						   GFP_KERNEL);
			if (!rxr->rx_desc_ring[j])
				return -ENOMEM;

		}

		if (bp->rx_pg_ring_size) {
			rxr->rx_pg_ring =
				vzalloc(array_size(SW_RXPG_RING_SIZE,
						   bp->rx_max_pg_ring));
			if (!rxr->rx_pg_ring)
				return -ENOMEM;

		}

		for (j = 0; j < bp->rx_max_pg_ring; j++) {
			rxr->rx_pg_desc_ring[j] =
				dma_alloc_coherent(&bp->pdev->dev,
						   RXBD_RING_SIZE,
						   &rxr->rx_pg_desc_mapping[j],
						   GFP_KERNEL);
			if (!rxr->rx_pg_desc_ring[j])
				return -ENOMEM;

		}
	}
	return 0;
}

static void
bnx2_free_stats_blk(struct net_device *dev)
{
	struct bnx2 *bp = netdev_priv(dev);

	if (bp->status_blk) {
		dma_free_coherent(&bp->pdev->dev, bp->status_stats_size,
				  bp->status_blk,
				  bp->status_blk_mapping);
		bp->status_blk = NULL;
		bp->stats_blk = NULL;
	}
}

static int
bnx2_alloc_stats_blk(struct net_device *dev)
{
	int status_blk_size;
	void *status_blk;
	struct bnx2 *bp = netdev_priv(dev);

	/* Combine status and statistics blocks into one allocation. */
	status_blk_size = L1_CACHE_ALIGN(sizeof(struct status_block));
	if (bp->flags & BNX2_FLAG_MSIX_CAP)
		status_blk_size = L1_CACHE_ALIGN(BNX2_MAX_MSIX_HW_VEC *
						 BNX2_SBLK_MSIX_ALIGN_SIZE);
	bp->status_stats_size = status_blk_size +
				sizeof(struct statistics_block);
	status_blk = dma_alloc_coherent(&bp->pdev->dev, bp->status_stats_size,
					&bp->status_blk_mapping, GFP_KERNEL);
	if (!status_blk)
		return -ENOMEM;

	bp->status_blk = status_blk;
	bp->stats_blk = status_blk + status_blk_size;
	bp->stats_blk_mapping = bp->status_blk_mapping + status_blk_size;

	return 0;
}

static void
bnx2_free_mem(struct bnx2 *bp)
{
	int i;
	struct bnx2_napi *bnapi = &bp->bnx2_napi[0];

	bnx2_free_tx_mem(bp);
	bnx2_free_rx_mem(bp);

	for (i = 0; i < bp->ctx_pages; i++) {
		if (bp->ctx_blk[i]) {
			dma_free_coherent(&bp->pdev->dev, BNX2_PAGE_SIZE,
					  bp->ctx_blk[i],
					  bp->ctx_blk_mapping[i]);
			bp->ctx_blk[i] = NULL;
		}
	}

	if (bnapi->status_blk.msi)
		bnapi->status_blk.msi = NULL;
}

static int
bnx2_alloc_mem(struct bnx2 *bp)
{
	int i, err;
	struct bnx2_napi *bnapi;

	bnapi = &bp->bnx2_napi[0];
	bnapi->status_blk.msi = bp->status_blk;
	bnapi->hw_tx_cons_ptr =
		&bnapi->status_blk.msi->status_tx_quick_consumer_index0;
	bnapi->hw_rx_cons_ptr =
		&bnapi->status_blk.msi->status_rx_quick_consumer_index0;
	if (bp->flags & BNX2_FLAG_MSIX_CAP) {
		for (i = 1; i < bp->irq_nvecs; i++) {
			struct status_block_msix *sblk;

			bnapi = &bp->bnx2_napi[i];

			sblk = (bp->status_blk + BNX2_SBLK_MSIX_ALIGN_SIZE * i);
			bnapi->status_blk.msix = sblk;
			bnapi->hw_tx_cons_ptr =
				&sblk->status_tx_quick_consumer_index;
			bnapi->hw_rx_cons_ptr =
				&sblk->status_rx_quick_consumer_index;
			bnapi->int_num = i << 24;
		}
	}

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		bp->ctx_pages = 0x2000 / BNX2_PAGE_SIZE;
		if (bp->ctx_pages == 0)
			bp->ctx_pages = 1;
		for (i = 0; i < bp->ctx_pages; i++) {
			bp->ctx_blk[i] = dma_alloc_coherent(&bp->pdev->dev,
						BNX2_PAGE_SIZE,
						&bp->ctx_blk_mapping[i],
						GFP_KERNEL);
			if (!bp->ctx_blk[i])
				goto alloc_mem_err;
		}
	}

	err = bnx2_alloc_rx_mem(bp);
	if (err)
		goto alloc_mem_err;

	err = bnx2_alloc_tx_mem(bp);
	if (err)
		goto alloc_mem_err;

	return 0;

alloc_mem_err:
	bnx2_free_mem(bp);
	return -ENOMEM;
}

static void
bnx2_report_fw_link(struct bnx2 *bp)
{
	u32 fw_link_status = 0;

	if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP)
		return;

	if (bp->link_up) {
		u32 bmsr;

		switch (bp->line_speed) {
		case SPEED_10:
			if (bp->duplex == DUPLEX_HALF)
				fw_link_status = BNX2_LINK_STATUS_10HALF;
			else
				fw_link_status = BNX2_LINK_STATUS_10FULL;
			break;
		case SPEED_100:
			if (bp->duplex == DUPLEX_HALF)
				fw_link_status = BNX2_LINK_STATUS_100HALF;
			else
				fw_link_status = BNX2_LINK_STATUS_100FULL;
			break;
		case SPEED_1000:
			if (bp->duplex == DUPLEX_HALF)
				fw_link_status = BNX2_LINK_STATUS_1000HALF;
			else
				fw_link_status = BNX2_LINK_STATUS_1000FULL;
			break;
		case SPEED_2500:
			if (bp->duplex == DUPLEX_HALF)
				fw_link_status = BNX2_LINK_STATUS_2500HALF;
			else
				fw_link_status = BNX2_LINK_STATUS_2500FULL;
			break;
		}

		fw_link_status |= BNX2_LINK_STATUS_LINK_UP;

		if (bp->autoneg) {
			fw_link_status |= BNX2_LINK_STATUS_AN_ENABLED;

			bnx2_read_phy(bp, bp->mii_bmsr, &bmsr);
			bnx2_read_phy(bp, bp->mii_bmsr, &bmsr);

			if (!(bmsr & BMSR_ANEGCOMPLETE) ||
			    bp->phy_flags & BNX2_PHY_FLAG_PARALLEL_DETECT)
				fw_link_status |= BNX2_LINK_STATUS_PARALLEL_DET;
			else
				fw_link_status |= BNX2_LINK_STATUS_AN_COMPLETE;
		}
	}
	else
		fw_link_status = BNX2_LINK_STATUS_LINK_DOWN;

	bnx2_shmem_wr(bp, BNX2_LINK_STATUS, fw_link_status);
}

static char *
bnx2_xceiver_str(struct bnx2 *bp)
{
	return (bp->phy_port == PORT_FIBRE) ? "SerDes" :
		((bp->phy_flags & BNX2_PHY_FLAG_SERDES) ? "Remote Copper" :
		 "Copper");
}

static void
bnx2_report_link(struct bnx2 *bp)
{
	if (bp->link_up) {
		netif_carrier_on(bp->dev);
		netdev_info(bp->dev, "NIC %s Link is Up, %d Mbps %s duplex",
			    bnx2_xceiver_str(bp),
			    bp->line_speed,
			    bp->duplex == DUPLEX_FULL ? "full" : "half");

		if (bp->flow_ctrl) {
			if (bp->flow_ctrl & FLOW_CTRL_RX) {
				pr_cont(", receive ");
				if (bp->flow_ctrl & FLOW_CTRL_TX)
					pr_cont("& transmit ");
			}
			else {
				pr_cont(", transmit ");
			}
			pr_cont("flow control ON");
		}
		pr_cont("\n");
	} else {
		netif_carrier_off(bp->dev);
		netdev_err(bp->dev, "NIC %s Link is Down\n",
			   bnx2_xceiver_str(bp));
	}

	bnx2_report_fw_link(bp);
}

static void
bnx2_resolve_flow_ctrl(struct bnx2 *bp)
{
	u32 local_adv, remote_adv;

	bp->flow_ctrl = 0;
	if ((bp->autoneg & (AUTONEG_SPEED | AUTONEG_FLOW_CTRL)) !=
		(AUTONEG_SPEED | AUTONEG_FLOW_CTRL)) {

		if (bp->duplex == DUPLEX_FULL) {
			bp->flow_ctrl = bp->req_flow_ctrl;
		}
		return;
	}

	if (bp->duplex != DUPLEX_FULL) {
		return;
	}

	if ((bp->phy_flags & BNX2_PHY_FLAG_SERDES) &&
	    (BNX2_CHIP(bp) == BNX2_CHIP_5708)) {
		u32 val;

		bnx2_read_phy(bp, BCM5708S_1000X_STAT1, &val);
		if (val & BCM5708S_1000X_STAT1_TX_PAUSE)
			bp->flow_ctrl |= FLOW_CTRL_TX;
		if (val & BCM5708S_1000X_STAT1_RX_PAUSE)
			bp->flow_ctrl |= FLOW_CTRL_RX;
		return;
	}

	bnx2_read_phy(bp, bp->mii_adv, &local_adv);
	bnx2_read_phy(bp, bp->mii_lpa, &remote_adv);

	if (bp->phy_flags & BNX2_PHY_FLAG_SERDES) {
		u32 new_local_adv = 0;
		u32 new_remote_adv = 0;

		if (local_adv & ADVERTISE_1000XPAUSE)
			new_local_adv |= ADVERTISE_PAUSE_CAP;
		if (local_adv & ADVERTISE_1000XPSE_ASYM)
			new_local_adv |= ADVERTISE_PAUSE_ASYM;
		if (remote_adv & ADVERTISE_1000XPAUSE)
			new_remote_adv |= ADVERTISE_PAUSE_CAP;
		if (remote_adv & ADVERTISE_1000XPSE_ASYM)
			new_remote_adv |= ADVERTISE_PAUSE_ASYM;

		local_adv = new_local_adv;
		remote_adv = new_remote_adv;
	}

	/* See Table 28B-3 of 802.3ab-1999 spec. */
	if (local_adv & ADVERTISE_PAUSE_CAP) {
		if(local_adv & ADVERTISE_PAUSE_ASYM) {
	                if (remote_adv & ADVERTISE_PAUSE_CAP) {
				bp->flow_ctrl = FLOW_CTRL_TX | FLOW_CTRL_RX;
			}
			else if (remote_adv & ADVERTISE_PAUSE_ASYM) {
				bp->flow_ctrl = FLOW_CTRL_RX;
			}
		}
		else {
			if (remote_adv & ADVERTISE_PAUSE_CAP) {
				bp->flow_ctrl = FLOW_CTRL_TX | FLOW_CTRL_RX;
			}
		}
	}
	else if (local_adv & ADVERTISE_PAUSE_ASYM) {
		if ((remote_adv & ADVERTISE_PAUSE_CAP) &&
			(remote_adv & ADVERTISE_PAUSE_ASYM)) {

			bp->flow_ctrl = FLOW_CTRL_TX;
		}
	}
}

static int
bnx2_5709s_linkup(struct bnx2 *bp)
{
	u32 val, speed;

	bp->link_up = 1;

	bnx2_write_phy(bp, MII_BNX2_BLK_ADDR, MII_BNX2_BLK_ADDR_GP_STATUS);
	bnx2_read_phy(bp, MII_BNX2_GP_TOP_AN_STATUS1, &val);
	bnx2_write_phy(bp, MII_BNX2_BLK_ADDR, MII_BNX2_BLK_ADDR_COMBO_IEEEB0);

	if ((bp->autoneg & AUTONEG_SPEED) == 0) {
		bp->line_speed = bp->req_line_speed;
		bp->duplex = bp->req_duplex;
		return 0;
	}
	speed = val & MII_BNX2_GP_TOP_AN_SPEED_MSK;
	switch (speed) {
		case MII_BNX2_GP_TOP_AN_SPEED_10:
			bp->line_speed = SPEED_10;
			break;
		case MII_BNX2_GP_TOP_AN_SPEED_100:
			bp->line_speed = SPEED_100;
			break;
		case MII_BNX2_GP_TOP_AN_SPEED_1G:
		case MII_BNX2_GP_TOP_AN_SPEED_1GKV:
			bp->line_speed = SPEED_1000;
			break;
		case MII_BNX2_GP_TOP_AN_SPEED_2_5G:
			bp->line_speed = SPEED_2500;
			break;
	}
	if (val & MII_BNX2_GP_TOP_AN_FD)
		bp->duplex = DUPLEX_FULL;
	else
		bp->duplex = DUPLEX_HALF;
	return 0;
}

static int
bnx2_5708s_linkup(struct bnx2 *bp)
{
	u32 val;

	bp->link_up = 1;
	bnx2_read_phy(bp, BCM5708S_1000X_STAT1, &val);
	switch (val & BCM5708S_1000X_STAT1_SPEED_MASK) {
		case BCM5708S_1000X_STAT1_SPEED_10:
			bp->line_speed = SPEED_10;
			break;
		case BCM5708S_1000X_STAT1_SPEED_100:
			bp->line_speed = SPEED_100;
			break;
		case BCM5708S_1000X_STAT1_SPEED_1G:
			bp->line_speed = SPEED_1000;
			break;
		case BCM5708S_1000X_STAT1_SPEED_2G5:
			bp->line_speed = SPEED_2500;
			break;
	}
	if (val & BCM5708S_1000X_STAT1_FD)
		bp->duplex = DUPLEX_FULL;
	else
		bp->duplex = DUPLEX_HALF;

	return 0;
}

static int
bnx2_5706s_linkup(struct bnx2 *bp)
{
	u32 bmcr, local_adv, remote_adv, common;

	bp->link_up = 1;
	bp->line_speed = SPEED_1000;

	bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);
	if (bmcr & BMCR_FULLDPLX) {
		bp->duplex = DUPLEX_FULL;
	}
	else {
		bp->duplex = DUPLEX_HALF;
	}

	if (!(bmcr & BMCR_ANENABLE)) {
		return 0;
	}

	bnx2_read_phy(bp, bp->mii_adv, &local_adv);
	bnx2_read_phy(bp, bp->mii_lpa, &remote_adv);

	common = local_adv & remote_adv;
	if (common & (ADVERTISE_1000XHALF | ADVERTISE_1000XFULL)) {

		if (common & ADVERTISE_1000XFULL) {
			bp->duplex = DUPLEX_FULL;
		}
		else {
			bp->duplex = DUPLEX_HALF;
		}
	}

	return 0;
}

static int
bnx2_copper_linkup(struct bnx2 *bp)
{
	u32 bmcr;

	bp->phy_flags &= ~BNX2_PHY_FLAG_MDIX;

	bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);
	if (bmcr & BMCR_ANENABLE) {
		u32 local_adv, remote_adv, common;

		bnx2_read_phy(bp, MII_CTRL1000, &local_adv);
		bnx2_read_phy(bp, MII_STAT1000, &remote_adv);

		common = local_adv & (remote_adv >> 2);
		if (common & ADVERTISE_1000FULL) {
			bp->line_speed = SPEED_1000;
			bp->duplex = DUPLEX_FULL;
		}
		else if (common & ADVERTISE_1000HALF) {
			bp->line_speed = SPEED_1000;
			bp->duplex = DUPLEX_HALF;
		}
		else {
			bnx2_read_phy(bp, bp->mii_adv, &local_adv);
			bnx2_read_phy(bp, bp->mii_lpa, &remote_adv);

			common = local_adv & remote_adv;
			if (common & ADVERTISE_100FULL) {
				bp->line_speed = SPEED_100;
				bp->duplex = DUPLEX_FULL;
			}
			else if (common & ADVERTISE_100HALF) {
				bp->line_speed = SPEED_100;
				bp->duplex = DUPLEX_HALF;
			}
			else if (common & ADVERTISE_10FULL) {
				bp->line_speed = SPEED_10;
				bp->duplex = DUPLEX_FULL;
			}
			else if (common & ADVERTISE_10HALF) {
				bp->line_speed = SPEED_10;
				bp->duplex = DUPLEX_HALF;
			}
			else {
				bp->line_speed = 0;
				bp->link_up = 0;
			}
		}
	}
	else {
		if (bmcr & BMCR_SPEED100) {
			bp->line_speed = SPEED_100;
		}
		else {
			bp->line_speed = SPEED_10;
		}
		if (bmcr & BMCR_FULLDPLX) {
			bp->duplex = DUPLEX_FULL;
		}
		else {
			bp->duplex = DUPLEX_HALF;
		}
	}

	if (bp->link_up) {
		u32 ext_status;

		bnx2_read_phy(bp, MII_BNX2_EXT_STATUS, &ext_status);
		if (ext_status & EXT_STATUS_MDIX)
			bp->phy_flags |= BNX2_PHY_FLAG_MDIX;
	}

	return 0;
}

static void
bnx2_init_rx_context(struct bnx2 *bp, u32 cid)
{
	u32 val, rx_cid_addr = GET_CID_ADDR(cid);

	val = BNX2_L2CTX_CTX_TYPE_CTX_BD_CHN_TYPE_VALUE;
	val |= BNX2_L2CTX_CTX_TYPE_SIZE_L2;
	val |= 0x02 << 8;

	if (bp->flow_ctrl & FLOW_CTRL_TX)
		val |= BNX2_L2CTX_FLOW_CTRL_ENABLE;

	bnx2_ctx_wr(bp, rx_cid_addr, BNX2_L2CTX_CTX_TYPE, val);
}

static void
bnx2_init_all_rx_contexts(struct bnx2 *bp)
{
	int i;
	u32 cid;

	for (i = 0, cid = RX_CID; i < bp->num_rx_rings; i++, cid++) {
		if (i == 1)
			cid = RX_RSS_CID;
		bnx2_init_rx_context(bp, cid);
	}
}

static void
bnx2_set_mac_link(struct bnx2 *bp)
{
	u32 val;

	BNX2_WR(bp, BNX2_EMAC_TX_LENGTHS, 0x2620);
	if (bp->link_up && (bp->line_speed == SPEED_1000) &&
		(bp->duplex == DUPLEX_HALF)) {
		BNX2_WR(bp, BNX2_EMAC_TX_LENGTHS, 0x26ff);
	}

	/* Configure the EMAC mode register. */
	val = BNX2_RD(bp, BNX2_EMAC_MODE);

	val &= ~(BNX2_EMAC_MODE_PORT | BNX2_EMAC_MODE_HALF_DUPLEX |
		BNX2_EMAC_MODE_MAC_LOOP | BNX2_EMAC_MODE_FORCE_LINK |
		BNX2_EMAC_MODE_25G_MODE);

	if (bp->link_up) {
		switch (bp->line_speed) {
			case SPEED_10:
				if (BNX2_CHIP(bp) != BNX2_CHIP_5706) {
					val |= BNX2_EMAC_MODE_PORT_MII_10M;
					break;
				}
				fallthrough;
			case SPEED_100:
				val |= BNX2_EMAC_MODE_PORT_MII;
				break;
			case SPEED_2500:
				val |= BNX2_EMAC_MODE_25G_MODE;
				fallthrough;
			case SPEED_1000:
				val |= BNX2_EMAC_MODE_PORT_GMII;
				break;
		}
	}
	else {
		val |= BNX2_EMAC_MODE_PORT_GMII;
	}

	/* Set the MAC to operate in the appropriate duplex mode. */
	if (bp->duplex == DUPLEX_HALF)
		val |= BNX2_EMAC_MODE_HALF_DUPLEX;
	BNX2_WR(bp, BNX2_EMAC_MODE, val);

	/* Enable/disable rx PAUSE. */
	bp->rx_mode &= ~BNX2_EMAC_RX_MODE_FLOW_EN;

	if (bp->flow_ctrl & FLOW_CTRL_RX)
		bp->rx_mode |= BNX2_EMAC_RX_MODE_FLOW_EN;
	BNX2_WR(bp, BNX2_EMAC_RX_MODE, bp->rx_mode);

	/* Enable/disable tx PAUSE. */
	val = BNX2_RD(bp, BNX2_EMAC_TX_MODE);
	val &= ~BNX2_EMAC_TX_MODE_FLOW_EN;

	if (bp->flow_ctrl & FLOW_CTRL_TX)
		val |= BNX2_EMAC_TX_MODE_FLOW_EN;
	BNX2_WR(bp, BNX2_EMAC_TX_MODE, val);

	/* Acknowledge the interrupt. */
	BNX2_WR(bp, BNX2_EMAC_STATUS, BNX2_EMAC_STATUS_LINK_CHANGE);

	bnx2_init_all_rx_contexts(bp);
}

static void
bnx2_enable_bmsr1(struct bnx2 *bp)
{
	if ((bp->phy_flags & BNX2_PHY_FLAG_SERDES) &&
	    (BNX2_CHIP(bp) == BNX2_CHIP_5709))
		bnx2_write_phy(bp, MII_BNX2_BLK_ADDR,
			       MII_BNX2_BLK_ADDR_GP_STATUS);
}

static void
bnx2_disable_bmsr1(struct bnx2 *bp)
{
	if ((bp->phy_flags & BNX2_PHY_FLAG_SERDES) &&
	    (BNX2_CHIP(bp) == BNX2_CHIP_5709))
		bnx2_write_phy(bp, MII_BNX2_BLK_ADDR,
			       MII_BNX2_BLK_ADDR_COMBO_IEEEB0);
}

static int
bnx2_test_and_enable_2g5(struct bnx2 *bp)
{
	u32 up1;
	int ret = 1;

	if (!(bp->phy_flags & BNX2_PHY_FLAG_2_5G_CAPABLE))
		return 0;

	if (bp->autoneg & AUTONEG_SPEED)
		bp->advertising |= ADVERTISED_2500baseX_Full;

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
		bnx2_write_phy(bp, MII_BNX2_BLK_ADDR, MII_BNX2_BLK_ADDR_OVER1G);

	bnx2_read_phy(bp, bp->mii_up1, &up1);
	if (!(up1 & BCM5708S_UP1_2G5)) {
		up1 |= BCM5708S_UP1_2G5;
		bnx2_write_phy(bp, bp->mii_up1, up1);
		ret = 0;
	}

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
		bnx2_write_phy(bp, MII_BNX2_BLK_ADDR,
			       MII_BNX2_BLK_ADDR_COMBO_IEEEB0);

	return ret;
}

static int
bnx2_test_and_disable_2g5(struct bnx2 *bp)
{
	u32 up1;
	int ret = 0;

	if (!(bp->phy_flags & BNX2_PHY_FLAG_2_5G_CAPABLE))
		return 0;

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
		bnx2_write_phy(bp, MII_BNX2_BLK_ADDR, MII_BNX2_BLK_ADDR_OVER1G);

	bnx2_read_phy(bp, bp->mii_up1, &up1);
	if (up1 & BCM5708S_UP1_2G5) {
		up1 &= ~BCM5708S_UP1_2G5;
		bnx2_write_phy(bp, bp->mii_up1, up1);
		ret = 1;
	}

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
		bnx2_write_phy(bp, MII_BNX2_BLK_ADDR,
			       MII_BNX2_BLK_ADDR_COMBO_IEEEB0);

	return ret;
}

static void
bnx2_enable_forced_2g5(struct bnx2 *bp)
{
	u32 bmcr;
	int err;

	if (!(bp->phy_flags & BNX2_PHY_FLAG_2_5G_CAPABLE))
		return;

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		u32 val;

		bnx2_write_phy(bp, MII_BNX2_BLK_ADDR,
			       MII_BNX2_BLK_ADDR_SERDES_DIG);
		if (!bnx2_read_phy(bp, MII_BNX2_SERDES_DIG_MISC1, &val)) {
			val &= ~MII_BNX2_SD_MISC1_FORCE_MSK;
			val |= MII_BNX2_SD_MISC1_FORCE |
				MII_BNX2_SD_MISC1_FORCE_2_5G;
			bnx2_write_phy(bp, MII_BNX2_SERDES_DIG_MISC1, val);
		}

		bnx2_write_phy(bp, MII_BNX2_BLK_ADDR,
			       MII_BNX2_BLK_ADDR_COMBO_IEEEB0);
		err = bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);

	} else if (BNX2_CHIP(bp) == BNX2_CHIP_5708) {
		err = bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);
		if (!err)
			bmcr |= BCM5708S_BMCR_FORCE_2500;
	} else {
		return;
	}

	if (err)
		return;

	if (bp->autoneg & AUTONEG_SPEED) {
		bmcr &= ~BMCR_ANENABLE;
		if (bp->req_duplex == DUPLEX_FULL)
			bmcr |= BMCR_FULLDPLX;
	}
	bnx2_write_phy(bp, bp->mii_bmcr, bmcr);
}

static void
bnx2_disable_forced_2g5(struct bnx2 *bp)
{
	u32 bmcr;
	int err;

	if (!(bp->phy_flags & BNX2_PHY_FLAG_2_5G_CAPABLE))
		return;

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		u32 val;

		bnx2_write_phy(bp, MII_BNX2_BLK_ADDR,
			       MII_BNX2_BLK_ADDR_SERDES_DIG);
		if (!bnx2_read_phy(bp, MII_BNX2_SERDES_DIG_MISC1, &val)) {
			val &= ~MII_BNX2_SD_MISC1_FORCE;
			bnx2_write_phy(bp, MII_BNX2_SERDES_DIG_MISC1, val);
		}

		bnx2_write_phy(bp, MII_BNX2_BLK_ADDR,
			       MII_BNX2_BLK_ADDR_COMBO_IEEEB0);
		err = bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);

	} else if (BNX2_CHIP(bp) == BNX2_CHIP_5708) {
		err = bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);
		if (!err)
			bmcr &= ~BCM5708S_BMCR_FORCE_2500;
	} else {
		return;
	}

	if (err)
		return;

	if (bp->autoneg & AUTONEG_SPEED)
		bmcr |= BMCR_SPEED1000 | BMCR_ANENABLE | BMCR_ANRESTART;
	bnx2_write_phy(bp, bp->mii_bmcr, bmcr);
}

static void
bnx2_5706s_force_link_dn(struct bnx2 *bp, int start)
{
	u32 val;

	bnx2_write_phy(bp, MII_BNX2_DSP_ADDRESS, MII_EXPAND_SERDES_CTL);
	bnx2_read_phy(bp, MII_BNX2_DSP_RW_PORT, &val);
	if (start)
		bnx2_write_phy(bp, MII_BNX2_DSP_RW_PORT, val & 0xff0f);
	else
		bnx2_write_phy(bp, MII_BNX2_DSP_RW_PORT, val | 0xc0);
}

static int
bnx2_set_link(struct bnx2 *bp)
{
	u32 bmsr;
	u8 link_up;

	if (bp->loopback == MAC_LOOPBACK || bp->loopback == PHY_LOOPBACK) {
		bp->link_up = 1;
		return 0;
	}

	if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP)
		return 0;

	link_up = bp->link_up;

	bnx2_enable_bmsr1(bp);
	bnx2_read_phy(bp, bp->mii_bmsr1, &bmsr);
	bnx2_read_phy(bp, bp->mii_bmsr1, &bmsr);
	bnx2_disable_bmsr1(bp);

	if ((bp->phy_flags & BNX2_PHY_FLAG_SERDES) &&
	    (BNX2_CHIP(bp) == BNX2_CHIP_5706)) {
		u32 val, an_dbg;

		if (bp->phy_flags & BNX2_PHY_FLAG_FORCED_DOWN) {
			bnx2_5706s_force_link_dn(bp, 0);
			bp->phy_flags &= ~BNX2_PHY_FLAG_FORCED_DOWN;
		}
		val = BNX2_RD(bp, BNX2_EMAC_STATUS);

		bnx2_write_phy(bp, MII_BNX2_MISC_SHADOW, MISC_SHDW_AN_DBG);
		bnx2_read_phy(bp, MII_BNX2_MISC_SHADOW, &an_dbg);
		bnx2_read_phy(bp, MII_BNX2_MISC_SHADOW, &an_dbg);

		if ((val & BNX2_EMAC_STATUS_LINK) &&
		    !(an_dbg & MISC_SHDW_AN_DBG_NOSYNC))
			bmsr |= BMSR_LSTATUS;
		else
			bmsr &= ~BMSR_LSTATUS;
	}

	if (bmsr & BMSR_LSTATUS) {
		bp->link_up = 1;

		if (bp->phy_flags & BNX2_PHY_FLAG_SERDES) {
			if (BNX2_CHIP(bp) == BNX2_CHIP_5706)
				bnx2_5706s_linkup(bp);
			else if (BNX2_CHIP(bp) == BNX2_CHIP_5708)
				bnx2_5708s_linkup(bp);
			else if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
				bnx2_5709s_linkup(bp);
		}
		else {
			bnx2_copper_linkup(bp);
		}
		bnx2_resolve_flow_ctrl(bp);
	}
	else {
		if ((bp->phy_flags & BNX2_PHY_FLAG_SERDES) &&
		    (bp->autoneg & AUTONEG_SPEED))
			bnx2_disable_forced_2g5(bp);

		if (bp->phy_flags & BNX2_PHY_FLAG_PARALLEL_DETECT) {
			u32 bmcr;

			bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);
			bmcr |= BMCR_ANENABLE;
			bnx2_write_phy(bp, bp->mii_bmcr, bmcr);

			bp->phy_flags &= ~BNX2_PHY_FLAG_PARALLEL_DETECT;
		}
		bp->link_up = 0;
	}

	if (bp->link_up != link_up) {
		bnx2_report_link(bp);
	}

	bnx2_set_mac_link(bp);

	return 0;
}

static int
bnx2_reset_phy(struct bnx2 *bp)
{
	int i;
	u32 reg;

        bnx2_write_phy(bp, bp->mii_bmcr, BMCR_RESET);

#define PHY_RESET_MAX_WAIT 100
	for (i = 0; i < PHY_RESET_MAX_WAIT; i++) {
		udelay(10);

		bnx2_read_phy(bp, bp->mii_bmcr, &reg);
		if (!(reg & BMCR_RESET)) {
			udelay(20);
			break;
		}
	}
	if (i == PHY_RESET_MAX_WAIT) {
		return -EBUSY;
	}
	return 0;
}

static u32
bnx2_phy_get_pause_adv(struct bnx2 *bp)
{
	u32 adv = 0;

	if ((bp->req_flow_ctrl & (FLOW_CTRL_RX | FLOW_CTRL_TX)) ==
		(FLOW_CTRL_RX | FLOW_CTRL_TX)) {

		if (bp->phy_flags & BNX2_PHY_FLAG_SERDES) {
			adv = ADVERTISE_1000XPAUSE;
		}
		else {
			adv = ADVERTISE_PAUSE_CAP;
		}
	}
	else if (bp->req_flow_ctrl & FLOW_CTRL_TX) {
		if (bp->phy_flags & BNX2_PHY_FLAG_SERDES) {
			adv = ADVERTISE_1000XPSE_ASYM;
		}
		else {
			adv = ADVERTISE_PAUSE_ASYM;
		}
	}
	else if (bp->req_flow_ctrl & FLOW_CTRL_RX) {
		if (bp->phy_flags & BNX2_PHY_FLAG_SERDES) {
			adv = ADVERTISE_1000XPAUSE | ADVERTISE_1000XPSE_ASYM;
		}
		else {
			adv = ADVERTISE_PAUSE_CAP | ADVERTISE_PAUSE_ASYM;
		}
	}
	return adv;
}

static int bnx2_fw_sync(struct bnx2 *, u32, int, int);

static int
bnx2_setup_remote_phy(struct bnx2 *bp, u8 port)
__releases(&bp->phy_lock)
__acquires(&bp->phy_lock)
{
	u32 speed_arg = 0, pause_adv;

	pause_adv = bnx2_phy_get_pause_adv(bp);

	if (bp->autoneg & AUTONEG_SPEED) {
		speed_arg |= BNX2_NETLINK_SET_LINK_ENABLE_AUTONEG;
		if (bp->advertising & ADVERTISED_10baseT_Half)
			speed_arg |= BNX2_NETLINK_SET_LINK_SPEED_10HALF;
		if (bp->advertising & ADVERTISED_10baseT_Full)
			speed_arg |= BNX2_NETLINK_SET_LINK_SPEED_10FULL;
		if (bp->advertising & ADVERTISED_100baseT_Half)
			speed_arg |= BNX2_NETLINK_SET_LINK_SPEED_100HALF;
		if (bp->advertising & ADVERTISED_100baseT_Full)
			speed_arg |= BNX2_NETLINK_SET_LINK_SPEED_100FULL;
		if (bp->advertising & ADVERTISED_1000baseT_Full)
			speed_arg |= BNX2_NETLINK_SET_LINK_SPEED_1GFULL;
		if (bp->advertising & ADVERTISED_2500baseX_Full)
			speed_arg |= BNX2_NETLINK_SET_LINK_SPEED_2G5FULL;
	} else {
		if (bp->req_line_speed == SPEED_2500)
			speed_arg = BNX2_NETLINK_SET_LINK_SPEED_2G5FULL;
		else if (bp->req_line_speed == SPEED_1000)
			speed_arg = BNX2_NETLINK_SET_LINK_SPEED_1GFULL;
		else if (bp->req_line_speed == SPEED_100) {
			if (bp->req_duplex == DUPLEX_FULL)
				speed_arg = BNX2_NETLINK_SET_LINK_SPEED_100FULL;
			else
				speed_arg = BNX2_NETLINK_SET_LINK_SPEED_100HALF;
		} else if (bp->req_line_speed == SPEED_10) {
			if (bp->req_duplex == DUPLEX_FULL)
				speed_arg = BNX2_NETLINK_SET_LINK_SPEED_10FULL;
			else
				speed_arg = BNX2_NETLINK_SET_LINK_SPEED_10HALF;
		}
	}

	if (pause_adv & (ADVERTISE_1000XPAUSE | ADVERTISE_PAUSE_CAP))
		speed_arg |= BNX2_NETLINK_SET_LINK_FC_SYM_PAUSE;
	if (pause_adv & (ADVERTISE_1000XPSE_ASYM | ADVERTISE_PAUSE_ASYM))
		speed_arg |= BNX2_NETLINK_SET_LINK_FC_ASYM_PAUSE;

	if (port == PORT_TP)
		speed_arg |= BNX2_NETLINK_SET_LINK_PHY_APP_REMOTE |
			     BNX2_NETLINK_SET_LINK_ETH_AT_WIRESPEED;

	bnx2_shmem_wr(bp, BNX2_DRV_MB_ARG0, speed_arg);

	spin_unlock_bh(&bp->phy_lock);
	bnx2_fw_sync(bp, BNX2_DRV_MSG_CODE_CMD_SET_LINK, 1, 0);
	spin_lock_bh(&bp->phy_lock);

	return 0;
}

static int
bnx2_setup_serdes_phy(struct bnx2 *bp, u8 port)
__releases(&bp->phy_lock)
__acquires(&bp->phy_lock)
{
	u32 adv, bmcr;
	u32 new_adv = 0;

	if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP)
		return bnx2_setup_remote_phy(bp, port);

	if (!(bp->autoneg & AUTONEG_SPEED)) {
		u32 new_bmcr;
		int force_link_down = 0;

		if (bp->req_line_speed == SPEED_2500) {
			if (!bnx2_test_and_enable_2g5(bp))
				force_link_down = 1;
		} else if (bp->req_line_speed == SPEED_1000) {
			if (bnx2_test_and_disable_2g5(bp))
				force_link_down = 1;
		}
		bnx2_read_phy(bp, bp->mii_adv, &adv);
		adv &= ~(ADVERTISE_1000XFULL | ADVERTISE_1000XHALF);

		bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);
		new_bmcr = bmcr & ~BMCR_ANENABLE;
		new_bmcr |= BMCR_SPEED1000;

		if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
			if (bp->req_line_speed == SPEED_2500)
				bnx2_enable_forced_2g5(bp);
			else if (bp->req_line_speed == SPEED_1000) {
				bnx2_disable_forced_2g5(bp);
				new_bmcr &= ~0x2000;
			}

		} else if (BNX2_CHIP(bp) == BNX2_CHIP_5708) {
			if (bp->req_line_speed == SPEED_2500)
				new_bmcr |= BCM5708S_BMCR_FORCE_2500;
			else
				new_bmcr = bmcr & ~BCM5708S_BMCR_FORCE_2500;
		}

		if (bp->req_duplex == DUPLEX_FULL) {
			adv |= ADVERTISE_1000XFULL;
			new_bmcr |= BMCR_FULLDPLX;
		}
		else {
			adv |= ADVERTISE_1000XHALF;
			new_bmcr &= ~BMCR_FULLDPLX;
		}
		if ((new_bmcr != bmcr) || (force_link_down)) {
			/* Force a link down visible on the other side */
			if (bp->link_up) {
				bnx2_write_phy(bp, bp->mii_adv, adv &
					       ~(ADVERTISE_1000XFULL |
						 ADVERTISE_1000XHALF));
				bnx2_write_phy(bp, bp->mii_bmcr, bmcr |
					BMCR_ANRESTART | BMCR_ANENABLE);

				bp->link_up = 0;
				netif_carrier_off(bp->dev);
				bnx2_write_phy(bp, bp->mii_bmcr, new_bmcr);
				bnx2_report_link(bp);
			}
			bnx2_write_phy(bp, bp->mii_adv, adv);
			bnx2_write_phy(bp, bp->mii_bmcr, new_bmcr);
		} else {
			bnx2_resolve_flow_ctrl(bp);
			bnx2_set_mac_link(bp);
		}
		return 0;
	}

	bnx2_test_and_enable_2g5(bp);

	if (bp->advertising & ADVERTISED_1000baseT_Full)
		new_adv |= ADVERTISE_1000XFULL;

	new_adv |= bnx2_phy_get_pause_adv(bp);

	bnx2_read_phy(bp, bp->mii_adv, &adv);
	bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);

	bp->serdes_an_pending = 0;
	if ((adv != new_adv) || ((bmcr & BMCR_ANENABLE) == 0)) {
		/* Force a link down visible on the other side */
		if (bp->link_up) {
			bnx2_write_phy(bp, bp->mii_bmcr, BMCR_LOOPBACK);
			spin_unlock_bh(&bp->phy_lock);
			msleep(20);
			spin_lock_bh(&bp->phy_lock);
		}

		bnx2_write_phy(bp, bp->mii_adv, new_adv);
		bnx2_write_phy(bp, bp->mii_bmcr, bmcr | BMCR_ANRESTART |
			BMCR_ANENABLE);
		/* Speed up link-up time when the link partner
		 * does not autonegotiate which is very common
		 * in blade servers. Some blade servers use
		 * IPMI for kerboard input and it's important
		 * to minimize link disruptions. Autoneg. involves
		 * exchanging base pages plus 3 next pages and
		 * normally completes in about 120 msec.
		 */
		bp->current_interval = BNX2_SERDES_AN_TIMEOUT;
		bp->serdes_an_pending = 1;
		mod_timer(&bp->timer, jiffies + bp->current_interval);
	} else {
		bnx2_resolve_flow_ctrl(bp);
		bnx2_set_mac_link(bp);
	}

	return 0;
}

#define ETHTOOL_ALL_FIBRE_SPEED						\
	(bp->phy_flags & BNX2_PHY_FLAG_2_5G_CAPABLE) ?			\
		(ADVERTISED_2500baseX_Full | ADVERTISED_1000baseT_Full) :\
		(ADVERTISED_1000baseT_Full)

#define ETHTOOL_ALL_COPPER_SPEED					\
	(ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full |		\
	ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full |		\
	ADVERTISED_1000baseT_Full)

#define PHY_ALL_10_100_SPEED (ADVERTISE_10HALF | ADVERTISE_10FULL | \
	ADVERTISE_100HALF | ADVERTISE_100FULL | ADVERTISE_CSMA)

#define PHY_ALL_1000_SPEED (ADVERTISE_1000HALF | ADVERTISE_1000FULL)

static void
bnx2_set_default_remote_link(struct bnx2 *bp)
{
	u32 link;

	if (bp->phy_port == PORT_TP)
		link = bnx2_shmem_rd(bp, BNX2_RPHY_COPPER_LINK);
	else
		link = bnx2_shmem_rd(bp, BNX2_RPHY_SERDES_LINK);

	if (link & BNX2_NETLINK_SET_LINK_ENABLE_AUTONEG) {
		bp->req_line_speed = 0;
		bp->autoneg |= AUTONEG_SPEED;
		bp->advertising = ADVERTISED_Autoneg;
		if (link & BNX2_NETLINK_SET_LINK_SPEED_10HALF)
			bp->advertising |= ADVERTISED_10baseT_Half;
		if (link & BNX2_NETLINK_SET_LINK_SPEED_10FULL)
			bp->advertising |= ADVERTISED_10baseT_Full;
		if (link & BNX2_NETLINK_SET_LINK_SPEED_100HALF)
			bp->advertising |= ADVERTISED_100baseT_Half;
		if (link & BNX2_NETLINK_SET_LINK_SPEED_100FULL)
			bp->advertising |= ADVERTISED_100baseT_Full;
		if (link & BNX2_NETLINK_SET_LINK_SPEED_1GFULL)
			bp->advertising |= ADVERTISED_1000baseT_Full;
		if (link & BNX2_NETLINK_SET_LINK_SPEED_2G5FULL)
			bp->advertising |= ADVERTISED_2500baseX_Full;
	} else {
		bp->autoneg = 0;
		bp->advertising = 0;
		bp->req_duplex = DUPLEX_FULL;
		if (link & BNX2_NETLINK_SET_LINK_SPEED_10) {
			bp->req_line_speed = SPEED_10;
			if (link & BNX2_NETLINK_SET_LINK_SPEED_10HALF)
				bp->req_duplex = DUPLEX_HALF;
		}
		if (link & BNX2_NETLINK_SET_LINK_SPEED_100) {
			bp->req_line_speed = SPEED_100;
			if (link & BNX2_NETLINK_SET_LINK_SPEED_100HALF)
				bp->req_duplex = DUPLEX_HALF;
		}
		if (link & BNX2_NETLINK_SET_LINK_SPEED_1GFULL)
			bp->req_line_speed = SPEED_1000;
		if (link & BNX2_NETLINK_SET_LINK_SPEED_2G5FULL)
			bp->req_line_speed = SPEED_2500;
	}
}

static void
bnx2_set_default_link(struct bnx2 *bp)
{
	if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP) {
		bnx2_set_default_remote_link(bp);
		return;
	}

	bp->autoneg = AUTONEG_SPEED | AUTONEG_FLOW_CTRL;
	bp->req_line_speed = 0;
	if (bp->phy_flags & BNX2_PHY_FLAG_SERDES) {
		u32 reg;

		bp->advertising = ETHTOOL_ALL_FIBRE_SPEED | ADVERTISED_Autoneg;

		reg = bnx2_shmem_rd(bp, BNX2_PORT_HW_CFG_CONFIG);
		reg &= BNX2_PORT_HW_CFG_CFG_DFLT_LINK_MASK;
		if (reg == BNX2_PORT_HW_CFG_CFG_DFLT_LINK_1G) {
			bp->autoneg = 0;
			bp->req_line_speed = bp->line_speed = SPEED_1000;
			bp->req_duplex = DUPLEX_FULL;
		}
	} else
		bp->advertising = ETHTOOL_ALL_COPPER_SPEED | ADVERTISED_Autoneg;
}

static void
bnx2_send_heart_beat(struct bnx2 *bp)
{
	u32 msg;
	u32 addr;

	spin_lock(&bp->indirect_lock);
	msg = (u32) (++bp->fw_drv_pulse_wr_seq & BNX2_DRV_PULSE_SEQ_MASK);
	addr = bp->shmem_base + BNX2_DRV_PULSE_MB;
	BNX2_WR(bp, BNX2_PCICFG_REG_WINDOW_ADDRESS, addr);
	BNX2_WR(bp, BNX2_PCICFG_REG_WINDOW, msg);
	spin_unlock(&bp->indirect_lock);
}

static void
bnx2_remote_phy_event(struct bnx2 *bp)
{
	u32 msg;
	u8 link_up = bp->link_up;
	u8 old_port;

	msg = bnx2_shmem_rd(bp, BNX2_LINK_STATUS);

	if (msg & BNX2_LINK_STATUS_HEART_BEAT_EXPIRED)
		bnx2_send_heart_beat(bp);

	msg &= ~BNX2_LINK_STATUS_HEART_BEAT_EXPIRED;

	if ((msg & BNX2_LINK_STATUS_LINK_UP) == BNX2_LINK_STATUS_LINK_DOWN)
		bp->link_up = 0;
	else {
		u32 speed;

		bp->link_up = 1;
		speed = msg & BNX2_LINK_STATUS_SPEED_MASK;
		bp->duplex = DUPLEX_FULL;
		switch (speed) {
			case BNX2_LINK_STATUS_10HALF:
				bp->duplex = DUPLEX_HALF;
				fallthrough;
			case BNX2_LINK_STATUS_10FULL:
				bp->line_speed = SPEED_10;
				break;
			case BNX2_LINK_STATUS_100HALF:
				bp->duplex = DUPLEX_HALF;
				fallthrough;
			case BNX2_LINK_STATUS_100BASE_T4:
			case BNX2_LINK_STATUS_100FULL:
				bp->line_speed = SPEED_100;
				break;
			case BNX2_LINK_STATUS_1000HALF:
				bp->duplex = DUPLEX_HALF;
				fallthrough;
			case BNX2_LINK_STATUS_1000FULL:
				bp->line_speed = SPEED_1000;
				break;
			case BNX2_LINK_STATUS_2500HALF:
				bp->duplex = DUPLEX_HALF;
				fallthrough;
			case BNX2_LINK_STATUS_2500FULL:
				bp->line_speed = SPEED_2500;
				break;
			default:
				bp->line_speed = 0;
				break;
		}

		bp->flow_ctrl = 0;
		if ((bp->autoneg & (AUTONEG_SPEED | AUTONEG_FLOW_CTRL)) !=
		    (AUTONEG_SPEED | AUTONEG_FLOW_CTRL)) {
			if (bp->duplex == DUPLEX_FULL)
				bp->flow_ctrl = bp->req_flow_ctrl;
		} else {
			if (msg & BNX2_LINK_STATUS_TX_FC_ENABLED)
				bp->flow_ctrl |= FLOW_CTRL_TX;
			if (msg & BNX2_LINK_STATUS_RX_FC_ENABLED)
				bp->flow_ctrl |= FLOW_CTRL_RX;
		}

		old_port = bp->phy_port;
		if (msg & BNX2_LINK_STATUS_SERDES_LINK)
			bp->phy_port = PORT_FIBRE;
		else
			bp->phy_port = PORT_TP;

		if (old_port != bp->phy_port)
			bnx2_set_default_link(bp);

	}
	if (bp->link_up != link_up)
		bnx2_report_link(bp);

	bnx2_set_mac_link(bp);
}

static int
bnx2_set_remote_link(struct bnx2 *bp)
{
	u32 evt_code;

	evt_code = bnx2_shmem_rd(bp, BNX2_FW_EVT_CODE_MB);
	switch (evt_code) {
		case BNX2_FW_EVT_CODE_LINK_EVENT:
			bnx2_remote_phy_event(bp);
			break;
		case BNX2_FW_EVT_CODE_SW_TIMER_EXPIRATION_EVENT:
		default:
			bnx2_send_heart_beat(bp);
			break;
	}
	return 0;
}

static int
bnx2_setup_copper_phy(struct bnx2 *bp)
__releases(&bp->phy_lock)
__acquires(&bp->phy_lock)
{
	u32 bmcr, adv_reg, new_adv = 0;
	u32 new_bmcr;

	bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);

	bnx2_read_phy(bp, bp->mii_adv, &adv_reg);
	adv_reg &= (PHY_ALL_10_100_SPEED | ADVERTISE_PAUSE_CAP |
		    ADVERTISE_PAUSE_ASYM);

	new_adv = ADVERTISE_CSMA | ethtool_adv_to_mii_adv_t(bp->advertising);

	if (bp->autoneg & AUTONEG_SPEED) {
		u32 adv1000_reg;
		u32 new_adv1000 = 0;

		new_adv |= bnx2_phy_get_pause_adv(bp);

		bnx2_read_phy(bp, MII_CTRL1000, &adv1000_reg);
		adv1000_reg &= PHY_ALL_1000_SPEED;

		new_adv1000 |= ethtool_adv_to_mii_ctrl1000_t(bp->advertising);
		if ((adv1000_reg != new_adv1000) ||
			(adv_reg != new_adv) ||
			((bmcr & BMCR_ANENABLE) == 0)) {

			bnx2_write_phy(bp, bp->mii_adv, new_adv);
			bnx2_write_phy(bp, MII_CTRL1000, new_adv1000);
			bnx2_write_phy(bp, bp->mii_bmcr, BMCR_ANRESTART |
				BMCR_ANENABLE);
		}
		else if (bp->link_up) {
			/* Flow ctrl may have changed from auto to forced */
			/* or vice-versa. */

			bnx2_resolve_flow_ctrl(bp);
			bnx2_set_mac_link(bp);
		}
		return 0;
	}

	/* advertise nothing when forcing speed */
	if (adv_reg != new_adv)
		bnx2_write_phy(bp, bp->mii_adv, new_adv);

	new_bmcr = 0;
	if (bp->req_line_speed == SPEED_100) {
		new_bmcr |= BMCR_SPEED100;
	}
	if (bp->req_duplex == DUPLEX_FULL) {
		new_bmcr |= BMCR_FULLDPLX;
	}
	if (new_bmcr != bmcr) {
		u32 bmsr;

		bnx2_read_phy(bp, bp->mii_bmsr, &bmsr);
		bnx2_read_phy(bp, bp->mii_bmsr, &bmsr);

		if (bmsr & BMSR_LSTATUS) {
			/* Force link down */
			bnx2_write_phy(bp, bp->mii_bmcr, BMCR_LOOPBACK);
			spin_unlock_bh(&bp->phy_lock);
			msleep(50);
			spin_lock_bh(&bp->phy_lock);

			bnx2_read_phy(bp, bp->mii_bmsr, &bmsr);
			bnx2_read_phy(bp, bp->mii_bmsr, &bmsr);
		}

		bnx2_write_phy(bp, bp->mii_bmcr, new_bmcr);

		/* Normally, the new speed is setup after the link has
		 * gone down and up again. In some cases, link will not go
		 * down so we need to set up the new speed here.
		 */
		if (bmsr & BMSR_LSTATUS) {
			bp->line_speed = bp->req_line_speed;
			bp->duplex = bp->req_duplex;
			bnx2_resolve_flow_ctrl(bp);
			bnx2_set_mac_link(bp);
		}
	} else {
		bnx2_resolve_flow_ctrl(bp);
		bnx2_set_mac_link(bp);
	}
	return 0;
}

static int
bnx2_setup_phy(struct bnx2 *bp, u8 port)
__releases(&bp->phy_lock)
__acquires(&bp->phy_lock)
{
	if (bp->loopback == MAC_LOOPBACK)
		return 0;

	if (bp->phy_flags & BNX2_PHY_FLAG_SERDES) {
		return bnx2_setup_serdes_phy(bp, port);
	}
	else {
		return bnx2_setup_copper_phy(bp);
	}
}

static int
bnx2_init_5709s_phy(struct bnx2 *bp, int reset_phy)
{
	u32 val;

	bp->mii_bmcr = MII_BMCR + 0x10;
	bp->mii_bmsr = MII_BMSR + 0x10;
	bp->mii_bmsr1 = MII_BNX2_GP_TOP_AN_STATUS1;
	bp->mii_adv = MII_ADVERTISE + 0x10;
	bp->mii_lpa = MII_LPA + 0x10;
	bp->mii_up1 = MII_BNX2_OVER1G_UP1;

	bnx2_write_phy(bp, MII_BNX2_BLK_ADDR, MII_BNX2_BLK_ADDR_AER);
	bnx2_write_phy(bp, MII_BNX2_AER_AER, MII_BNX2_AER_AER_AN_MMD);

	bnx2_write_phy(bp, MII_BNX2_BLK_ADDR, MII_BNX2_BLK_ADDR_COMBO_IEEEB0);
	if (reset_phy)
		bnx2_reset_phy(bp);

	bnx2_write_phy(bp, MII_BNX2_BLK_ADDR, MII_BNX2_BLK_ADDR_SERDES_DIG);

	bnx2_read_phy(bp, MII_BNX2_SERDES_DIG_1000XCTL1, &val);
	val &= ~MII_BNX2_SD_1000XCTL1_AUTODET;
	val |= MII_BNX2_SD_1000XCTL1_FIBER;
	bnx2_write_phy(bp, MII_BNX2_SERDES_DIG_1000XCTL1, val);

	bnx2_write_phy(bp, MII_BNX2_BLK_ADDR, MII_BNX2_BLK_ADDR_OVER1G);
	bnx2_read_phy(bp, MII_BNX2_OVER1G_UP1, &val);
	if (bp->phy_flags & BNX2_PHY_FLAG_2_5G_CAPABLE)
		val |= BCM5708S_UP1_2G5;
	else
		val &= ~BCM5708S_UP1_2G5;
	bnx2_write_phy(bp, MII_BNX2_OVER1G_UP1, val);

	bnx2_write_phy(bp, MII_BNX2_BLK_ADDR, MII_BNX2_BLK_ADDR_BAM_NXTPG);
	bnx2_read_phy(bp, MII_BNX2_BAM_NXTPG_CTL, &val);
	val |= MII_BNX2_NXTPG_CTL_T2 | MII_BNX2_NXTPG_CTL_BAM;
	bnx2_write_phy(bp, MII_BNX2_BAM_NXTPG_CTL, val);

	bnx2_write_phy(bp, MII_BNX2_BLK_ADDR, MII_BNX2_BLK_ADDR_CL73_USERB0);

	val = MII_BNX2_CL73_BAM_EN | MII_BNX2_CL73_BAM_STA_MGR_EN |
	      MII_BNX2_CL73_BAM_NP_AFT_BP_EN;
	bnx2_write_phy(bp, MII_BNX2_CL73_BAM_CTL1, val);

	bnx2_write_phy(bp, MII_BNX2_BLK_ADDR, MII_BNX2_BLK_ADDR_COMBO_IEEEB0);

	return 0;
}

static int
bnx2_init_5708s_phy(struct bnx2 *bp, int reset_phy)
{
	u32 val;

	if (reset_phy)
		bnx2_reset_phy(bp);

	bp->mii_up1 = BCM5708S_UP1;

	bnx2_write_phy(bp, BCM5708S_BLK_ADDR, BCM5708S_BLK_ADDR_DIG3);
	bnx2_write_phy(bp, BCM5708S_DIG_3_0, BCM5708S_DIG_3_0_USE_IEEE);
	bnx2_write_phy(bp, BCM5708S_BLK_ADDR, BCM5708S_BLK_ADDR_DIG);

	bnx2_read_phy(bp, BCM5708S_1000X_CTL1, &val);
	val |= BCM5708S_1000X_CTL1_FIBER_MODE | BCM5708S_1000X_CTL1_AUTODET_EN;
	bnx2_write_phy(bp, BCM5708S_1000X_CTL1, val);

	bnx2_read_phy(bp, BCM5708S_1000X_CTL2, &val);
	val |= BCM5708S_1000X_CTL2_PLLEL_DET_EN;
	bnx2_write_phy(bp, BCM5708S_1000X_CTL2, val);

	if (bp->phy_flags & BNX2_PHY_FLAG_2_5G_CAPABLE) {
		bnx2_read_phy(bp, BCM5708S_UP1, &val);
		val |= BCM5708S_UP1_2G5;
		bnx2_write_phy(bp, BCM5708S_UP1, val);
	}

	if ((BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5708_A0) ||
	    (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5708_B0) ||
	    (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5708_B1)) {
		/* increase tx signal amplitude */
		bnx2_write_phy(bp, BCM5708S_BLK_ADDR,
			       BCM5708S_BLK_ADDR_TX_MISC);
		bnx2_read_phy(bp, BCM5708S_TX_ACTL1, &val);
		val &= ~BCM5708S_TX_ACTL1_DRIVER_VCM;
		bnx2_write_phy(bp, BCM5708S_TX_ACTL1, val);
		bnx2_write_phy(bp, BCM5708S_BLK_ADDR, BCM5708S_BLK_ADDR_DIG);
	}

	val = bnx2_shmem_rd(bp, BNX2_PORT_HW_CFG_CONFIG) &
	      BNX2_PORT_HW_CFG_CFG_TXCTL3_MASK;

	if (val) {
		u32 is_backplane;

		is_backplane = bnx2_shmem_rd(bp, BNX2_SHARED_HW_CFG_CONFIG);
		if (is_backplane & BNX2_SHARED_HW_CFG_PHY_BACKPLANE) {
			bnx2_write_phy(bp, BCM5708S_BLK_ADDR,
				       BCM5708S_BLK_ADDR_TX_MISC);
			bnx2_write_phy(bp, BCM5708S_TX_ACTL3, val);
			bnx2_write_phy(bp, BCM5708S_BLK_ADDR,
				       BCM5708S_BLK_ADDR_DIG);
		}
	}
	return 0;
}

static int
bnx2_init_5706s_phy(struct bnx2 *bp, int reset_phy)
{
	if (reset_phy)
		bnx2_reset_phy(bp);

	bp->phy_flags &= ~BNX2_PHY_FLAG_PARALLEL_DETECT;

	if (BNX2_CHIP(bp) == BNX2_CHIP_5706)
		BNX2_WR(bp, BNX2_MISC_GP_HW_CTL0, 0x300);

	if (bp->dev->mtu > ETH_DATA_LEN) {
		u32 val;

		/* Set extended packet length bit */
		bnx2_write_phy(bp, 0x18, 0x7);
		bnx2_read_phy(bp, 0x18, &val);
		bnx2_write_phy(bp, 0x18, (val & 0xfff8) | 0x4000);

		bnx2_write_phy(bp, 0x1c, 0x6c00);
		bnx2_read_phy(bp, 0x1c, &val);
		bnx2_write_phy(bp, 0x1c, (val & 0x3ff) | 0xec02);
	}
	else {
		u32 val;

		bnx2_write_phy(bp, 0x18, 0x7);
		bnx2_read_phy(bp, 0x18, &val);
		bnx2_write_phy(bp, 0x18, val & ~0x4007);

		bnx2_write_phy(bp, 0x1c, 0x6c00);
		bnx2_read_phy(bp, 0x1c, &val);
		bnx2_write_phy(bp, 0x1c, (val & 0x3fd) | 0xec00);
	}

	return 0;
}

static int
bnx2_init_copper_phy(struct bnx2 *bp, int reset_phy)
{
	u32 val;

	if (reset_phy)
		bnx2_reset_phy(bp);

	if (bp->phy_flags & BNX2_PHY_FLAG_CRC_FIX) {
		bnx2_write_phy(bp, 0x18, 0x0c00);
		bnx2_write_phy(bp, 0x17, 0x000a);
		bnx2_write_phy(bp, 0x15, 0x310b);
		bnx2_write_phy(bp, 0x17, 0x201f);
		bnx2_write_phy(bp, 0x15, 0x9506);
		bnx2_write_phy(bp, 0x17, 0x401f);
		bnx2_write_phy(bp, 0x15, 0x14e2);
		bnx2_write_phy(bp, 0x18, 0x0400);
	}

	if (bp->phy_flags & BNX2_PHY_FLAG_DIS_EARLY_DAC) {
		bnx2_write_phy(bp, MII_BNX2_DSP_ADDRESS,
			       MII_BNX2_DSP_EXPAND_REG | 0x8);
		bnx2_read_phy(bp, MII_BNX2_DSP_RW_PORT, &val);
		val &= ~(1 << 8);
		bnx2_write_phy(bp, MII_BNX2_DSP_RW_PORT, val);
	}

	if (bp->dev->mtu > ETH_DATA_LEN) {
		/* Set extended packet length bit */
		bnx2_write_phy(bp, 0x18, 0x7);
		bnx2_read_phy(bp, 0x18, &val);
		bnx2_write_phy(bp, 0x18, val | 0x4000);

		bnx2_read_phy(bp, 0x10, &val);
		bnx2_write_phy(bp, 0x10, val | 0x1);
	}
	else {
		bnx2_write_phy(bp, 0x18, 0x7);
		bnx2_read_phy(bp, 0x18, &val);
		bnx2_write_phy(bp, 0x18, val & ~0x4007);

		bnx2_read_phy(bp, 0x10, &val);
		bnx2_write_phy(bp, 0x10, val & ~0x1);
	}

	/* ethernet@wirespeed */
	bnx2_write_phy(bp, MII_BNX2_AUX_CTL, AUX_CTL_MISC_CTL);
	bnx2_read_phy(bp, MII_BNX2_AUX_CTL, &val);
	val |=  AUX_CTL_MISC_CTL_WR | AUX_CTL_MISC_CTL_WIRESPEED;

	/* auto-mdix */
	if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
		val |=  AUX_CTL_MISC_CTL_AUTOMDIX;

	bnx2_write_phy(bp, MII_BNX2_AUX_CTL, val);
	return 0;
}


static int
bnx2_init_phy(struct bnx2 *bp, int reset_phy)
__releases(&bp->phy_lock)
__acquires(&bp->phy_lock)
{
	u32 val;
	int rc = 0;

	bp->phy_flags &= ~BNX2_PHY_FLAG_INT_MODE_MASK;
	bp->phy_flags |= BNX2_PHY_FLAG_INT_MODE_LINK_READY;

	bp->mii_bmcr = MII_BMCR;
	bp->mii_bmsr = MII_BMSR;
	bp->mii_bmsr1 = MII_BMSR;
	bp->mii_adv = MII_ADVERTISE;
	bp->mii_lpa = MII_LPA;

	BNX2_WR(bp, BNX2_EMAC_ATTENTION_ENA, BNX2_EMAC_ATTENTION_ENA_LINK);

	if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP)
		goto setup_phy;

	bnx2_read_phy(bp, MII_PHYSID1, &val);
	bp->phy_id = val << 16;
	bnx2_read_phy(bp, MII_PHYSID2, &val);
	bp->phy_id |= val & 0xffff;

	if (bp->phy_flags & BNX2_PHY_FLAG_SERDES) {
		if (BNX2_CHIP(bp) == BNX2_CHIP_5706)
			rc = bnx2_init_5706s_phy(bp, reset_phy);
		else if (BNX2_CHIP(bp) == BNX2_CHIP_5708)
			rc = bnx2_init_5708s_phy(bp, reset_phy);
		else if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
			rc = bnx2_init_5709s_phy(bp, reset_phy);
	}
	else {
		rc = bnx2_init_copper_phy(bp, reset_phy);
	}

setup_phy:
	if (!rc)
		rc = bnx2_setup_phy(bp, bp->phy_port);

	return rc;
}

static int
bnx2_set_mac_loopback(struct bnx2 *bp)
{
	u32 mac_mode;

	mac_mode = BNX2_RD(bp, BNX2_EMAC_MODE);
	mac_mode &= ~BNX2_EMAC_MODE_PORT;
	mac_mode |= BNX2_EMAC_MODE_MAC_LOOP | BNX2_EMAC_MODE_FORCE_LINK;
	BNX2_WR(bp, BNX2_EMAC_MODE, mac_mode);
	bp->link_up = 1;
	return 0;
}

static int bnx2_test_link(struct bnx2 *);

static int
bnx2_set_phy_loopback(struct bnx2 *bp)
{
	u32 mac_mode;
	int rc, i;

	spin_lock_bh(&bp->phy_lock);
	rc = bnx2_write_phy(bp, bp->mii_bmcr, BMCR_LOOPBACK | BMCR_FULLDPLX |
			    BMCR_SPEED1000);
	spin_unlock_bh(&bp->phy_lock);
	if (rc)
		return rc;

	for (i = 0; i < 10; i++) {
		if (bnx2_test_link(bp) == 0)
			break;
		msleep(100);
	}

	mac_mode = BNX2_RD(bp, BNX2_EMAC_MODE);
	mac_mode &= ~(BNX2_EMAC_MODE_PORT | BNX2_EMAC_MODE_HALF_DUPLEX |
		      BNX2_EMAC_MODE_MAC_LOOP | BNX2_EMAC_MODE_FORCE_LINK |
		      BNX2_EMAC_MODE_25G_MODE);

	mac_mode |= BNX2_EMAC_MODE_PORT_GMII;
	BNX2_WR(bp, BNX2_EMAC_MODE, mac_mode);
	bp->link_up = 1;
	return 0;
}

static void
bnx2_dump_mcp_state(struct bnx2 *bp)
{
	struct net_device *dev = bp->dev;
	u32 mcp_p0, mcp_p1;

	netdev_err(dev, "<--- start MCP states dump --->\n");
	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		mcp_p0 = BNX2_MCP_STATE_P0;
		mcp_p1 = BNX2_MCP_STATE_P1;
	} else {
		mcp_p0 = BNX2_MCP_STATE_P0_5708;
		mcp_p1 = BNX2_MCP_STATE_P1_5708;
	}
	netdev_err(dev, "DEBUG: MCP_STATE_P0[%08x] MCP_STATE_P1[%08x]\n",
		   bnx2_reg_rd_ind(bp, mcp_p0), bnx2_reg_rd_ind(bp, mcp_p1));
	netdev_err(dev, "DEBUG: MCP mode[%08x] state[%08x] evt_mask[%08x]\n",
		   bnx2_reg_rd_ind(bp, BNX2_MCP_CPU_MODE),
		   bnx2_reg_rd_ind(bp, BNX2_MCP_CPU_STATE),
		   bnx2_reg_rd_ind(bp, BNX2_MCP_CPU_EVENT_MASK));
	netdev_err(dev, "DEBUG: pc[%08x] pc[%08x] instr[%08x]\n",
		   bnx2_reg_rd_ind(bp, BNX2_MCP_CPU_PROGRAM_COUNTER),
		   bnx2_reg_rd_ind(bp, BNX2_MCP_CPU_PROGRAM_COUNTER),
		   bnx2_reg_rd_ind(bp, BNX2_MCP_CPU_INSTRUCTION));
	netdev_err(dev, "DEBUG: shmem states:\n");
	netdev_err(dev, "DEBUG: drv_mb[%08x] fw_mb[%08x] link_status[%08x]",
		   bnx2_shmem_rd(bp, BNX2_DRV_MB),
		   bnx2_shmem_rd(bp, BNX2_FW_MB),
		   bnx2_shmem_rd(bp, BNX2_LINK_STATUS));
	pr_cont(" drv_pulse_mb[%08x]\n", bnx2_shmem_rd(bp, BNX2_DRV_PULSE_MB));
	netdev_err(dev, "DEBUG: dev_info_signature[%08x] reset_type[%08x]",
		   bnx2_shmem_rd(bp, BNX2_DEV_INFO_SIGNATURE),
		   bnx2_shmem_rd(bp, BNX2_BC_STATE_RESET_TYPE));
	pr_cont(" condition[%08x]\n",
		bnx2_shmem_rd(bp, BNX2_BC_STATE_CONDITION));
	DP_SHMEM_LINE(bp, BNX2_BC_RESET_TYPE);
	DP_SHMEM_LINE(bp, 0x3cc);
	DP_SHMEM_LINE(bp, 0x3dc);
	DP_SHMEM_LINE(bp, 0x3ec);
	netdev_err(dev, "DEBUG: 0x3fc[%08x]\n", bnx2_shmem_rd(bp, 0x3fc));
	netdev_err(dev, "<--- end MCP states dump --->\n");
}

static int
bnx2_fw_sync(struct bnx2 *bp, u32 msg_data, int ack, int silent)
{
	int i;
	u32 val;

	bp->fw_wr_seq++;
	msg_data |= bp->fw_wr_seq;
	bp->fw_last_msg = msg_data;

	bnx2_shmem_wr(bp, BNX2_DRV_MB, msg_data);

	if (!ack)
		return 0;

	/* wait for an acknowledgement. */
	for (i = 0; i < (BNX2_FW_ACK_TIME_OUT_MS / 10); i++) {
		msleep(10);

		val = bnx2_shmem_rd(bp, BNX2_FW_MB);

		if ((val & BNX2_FW_MSG_ACK) == (msg_data & BNX2_DRV_MSG_SEQ))
			break;
	}
	if ((msg_data & BNX2_DRV_MSG_DATA) == BNX2_DRV_MSG_DATA_WAIT0)
		return 0;

	/* If we timed out, inform the firmware that this is the case. */
	if ((val & BNX2_FW_MSG_ACK) != (msg_data & BNX2_DRV_MSG_SEQ)) {
		msg_data &= ~BNX2_DRV_MSG_CODE;
		msg_data |= BNX2_DRV_MSG_CODE_FW_TIMEOUT;

		bnx2_shmem_wr(bp, BNX2_DRV_MB, msg_data);
		if (!silent) {
			pr_err("fw sync timeout, reset code = %x\n", msg_data);
			bnx2_dump_mcp_state(bp);
		}

		return -EBUSY;
	}

	if ((val & BNX2_FW_MSG_STATUS_MASK) != BNX2_FW_MSG_STATUS_OK)
		return -EIO;

	return 0;
}

static int
bnx2_init_5709_context(struct bnx2 *bp)
{
	int i, ret = 0;
	u32 val;

	val = BNX2_CTX_COMMAND_ENABLED | BNX2_CTX_COMMAND_MEM_INIT | (1 << 12);
	val |= (BNX2_PAGE_BITS - 8) << 16;
	BNX2_WR(bp, BNX2_CTX_COMMAND, val);
	for (i = 0; i < 10; i++) {
		val = BNX2_RD(bp, BNX2_CTX_COMMAND);
		if (!(val & BNX2_CTX_COMMAND_MEM_INIT))
			break;
		udelay(2);
	}
	if (val & BNX2_CTX_COMMAND_MEM_INIT)
		return -EBUSY;

	for (i = 0; i < bp->ctx_pages; i++) {
		int j;

		if (bp->ctx_blk[i])
			memset(bp->ctx_blk[i], 0, BNX2_PAGE_SIZE);
		else
			return -ENOMEM;

		BNX2_WR(bp, BNX2_CTX_HOST_PAGE_TBL_DATA0,
			(bp->ctx_blk_mapping[i] & 0xffffffff) |
			BNX2_CTX_HOST_PAGE_TBL_DATA0_VALID);
		BNX2_WR(bp, BNX2_CTX_HOST_PAGE_TBL_DATA1,
			(u64) bp->ctx_blk_mapping[i] >> 32);
		BNX2_WR(bp, BNX2_CTX_HOST_PAGE_TBL_CTRL, i |
			BNX2_CTX_HOST_PAGE_TBL_CTRL_WRITE_REQ);
		for (j = 0; j < 10; j++) {

			val = BNX2_RD(bp, BNX2_CTX_HOST_PAGE_TBL_CTRL);
			if (!(val & BNX2_CTX_HOST_PAGE_TBL_CTRL_WRITE_REQ))
				break;
			udelay(5);
		}
		if (val & BNX2_CTX_HOST_PAGE_TBL_CTRL_WRITE_REQ) {
			ret = -EBUSY;
			break;
		}
	}
	return ret;
}

static void
bnx2_init_context(struct bnx2 *bp)
{
	u32 vcid;

	vcid = 96;
	while (vcid) {
		u32 vcid_addr, pcid_addr, offset;
		int i;

		vcid--;

		if (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A0) {
			u32 new_vcid;

			vcid_addr = GET_PCID_ADDR(vcid);
			if (vcid & 0x8) {
				new_vcid = 0x60 + (vcid & 0xf0) + (vcid & 0x7);
			}
			else {
				new_vcid = vcid;
			}
			pcid_addr = GET_PCID_ADDR(new_vcid);
		}
		else {
	    		vcid_addr = GET_CID_ADDR(vcid);
			pcid_addr = vcid_addr;
		}

		for (i = 0; i < (CTX_SIZE / PHY_CTX_SIZE); i++) {
			vcid_addr += (i << PHY_CTX_SHIFT);
			pcid_addr += (i << PHY_CTX_SHIFT);

			BNX2_WR(bp, BNX2_CTX_VIRT_ADDR, vcid_addr);
			BNX2_WR(bp, BNX2_CTX_PAGE_TBL, pcid_addr);

			/* Zero out the context. */
			for (offset = 0; offset < PHY_CTX_SIZE; offset += 4)
				bnx2_ctx_wr(bp, vcid_addr, offset, 0);
		}
	}
}

static int
bnx2_alloc_bad_rbuf(struct bnx2 *bp)
{
	u16 *good_mbuf;
	u32 good_mbuf_cnt;
	u32 val;

	good_mbuf = kmalloc_array(512, sizeof(u16), GFP_KERNEL);
	if (!good_mbuf)
		return -ENOMEM;

	BNX2_WR(bp, BNX2_MISC_ENABLE_SET_BITS,
		BNX2_MISC_ENABLE_SET_BITS_RX_MBUF_ENABLE);

	good_mbuf_cnt = 0;

	/* Allocate a bunch of mbufs and save the good ones in an array. */
	val = bnx2_reg_rd_ind(bp, BNX2_RBUF_STATUS1);
	while (val & BNX2_RBUF_STATUS1_FREE_COUNT) {
		bnx2_reg_wr_ind(bp, BNX2_RBUF_COMMAND,
				BNX2_RBUF_COMMAND_ALLOC_REQ);

		val = bnx2_reg_rd_ind(bp, BNX2_RBUF_FW_BUF_ALLOC);

		val &= BNX2_RBUF_FW_BUF_ALLOC_VALUE;

		/* The addresses with Bit 9 set are bad memory blocks. */
		if (!(val & (1 << 9))) {
			good_mbuf[good_mbuf_cnt] = (u16) val;
			good_mbuf_cnt++;
		}

		val = bnx2_reg_rd_ind(bp, BNX2_RBUF_STATUS1);
	}

	/* Free the good ones back to the mbuf pool thus discarding
	 * all the bad ones. */
	while (good_mbuf_cnt) {
		good_mbuf_cnt--;

		val = good_mbuf[good_mbuf_cnt];
		val = (val << 9) | val | 1;

		bnx2_reg_wr_ind(bp, BNX2_RBUF_FW_BUF_FREE, val);
	}
	kfree(good_mbuf);
	return 0;
}

static void
bnx2_set_mac_addr(struct bnx2 *bp, u8 *mac_addr, u32 pos)
{
	u32 val;

	val = (mac_addr[0] << 8) | mac_addr[1];

	BNX2_WR(bp, BNX2_EMAC_MAC_MATCH0 + (pos * 8), val);

	val = (mac_addr[2] << 24) | (mac_addr[3] << 16) |
		(mac_addr[4] << 8) | mac_addr[5];

	BNX2_WR(bp, BNX2_EMAC_MAC_MATCH1 + (pos * 8), val);
}

static inline int
bnx2_alloc_rx_page(struct bnx2 *bp, struct bnx2_rx_ring_info *rxr, u16 index, gfp_t gfp)
{
	dma_addr_t mapping;
	struct bnx2_sw_pg *rx_pg = &rxr->rx_pg_ring[index];
	struct bnx2_rx_bd *rxbd =
		&rxr->rx_pg_desc_ring[BNX2_RX_RING(index)][BNX2_RX_IDX(index)];
	struct page *page = alloc_page(gfp);

	if (!page)
		return -ENOMEM;
	mapping = dma_map_page(&bp->pdev->dev, page, 0, PAGE_SIZE,
			       PCI_DMA_FROMDEVICE);
	if (dma_mapping_error(&bp->pdev->dev, mapping)) {
		__free_page(page);
		return -EIO;
	}

	rx_pg->page = page;
	dma_unmap_addr_set(rx_pg, mapping, mapping);
	rxbd->rx_bd_haddr_hi = (u64) mapping >> 32;
	rxbd->rx_bd_haddr_lo = (u64) mapping & 0xffffffff;
	return 0;
}

static void
bnx2_free_rx_page(struct bnx2 *bp, struct bnx2_rx_ring_info *rxr, u16 index)
{
	struct bnx2_sw_pg *rx_pg = &rxr->rx_pg_ring[index];
	struct page *page = rx_pg->page;

	if (!page)
		return;

	dma_unmap_page(&bp->pdev->dev, dma_unmap_addr(rx_pg, mapping),
		       PAGE_SIZE, PCI_DMA_FROMDEVICE);

	__free_page(page);
	rx_pg->page = NULL;
}

static inline int
bnx2_alloc_rx_data(struct bnx2 *bp, struct bnx2_rx_ring_info *rxr, u16 index, gfp_t gfp)
{
	u8 *data;
	struct bnx2_sw_bd *rx_buf = &rxr->rx_buf_ring[index];
	dma_addr_t mapping;
	struct bnx2_rx_bd *rxbd =
		&rxr->rx_desc_ring[BNX2_RX_RING(index)][BNX2_RX_IDX(index)];

	data = kmalloc(bp->rx_buf_size, gfp);
	if (!data)
		return -ENOMEM;

	mapping = dma_map_single(&bp->pdev->dev,
				 get_l2_fhdr(data),
				 bp->rx_buf_use_size,
				 PCI_DMA_FROMDEVICE);
	if (dma_mapping_error(&bp->pdev->dev, mapping)) {
		kfree(data);
		return -EIO;
	}

	rx_buf->data = data;
	dma_unmap_addr_set(rx_buf, mapping, mapping);

	rxbd->rx_bd_haddr_hi = (u64) mapping >> 32;
	rxbd->rx_bd_haddr_lo = (u64) mapping & 0xffffffff;

	rxr->rx_prod_bseq += bp->rx_buf_use_size;

	return 0;
}

static int
bnx2_phy_event_is_set(struct bnx2 *bp, struct bnx2_napi *bnapi, u32 event)
{
	struct status_block *sblk = bnapi->status_blk.msi;
	u32 new_link_state, old_link_state;
	int is_set = 1;

	new_link_state = sblk->status_attn_bits & event;
	old_link_state = sblk->status_attn_bits_ack & event;
	if (new_link_state != old_link_state) {
		if (new_link_state)
			BNX2_WR(bp, BNX2_PCICFG_STATUS_BIT_SET_CMD, event);
		else
			BNX2_WR(bp, BNX2_PCICFG_STATUS_BIT_CLEAR_CMD, event);
	} else
		is_set = 0;

	return is_set;
}

static void
bnx2_phy_int(struct bnx2 *bp, struct bnx2_napi *bnapi)
{
	spin_lock(&bp->phy_lock);

	if (bnx2_phy_event_is_set(bp, bnapi, STATUS_ATTN_BITS_LINK_STATE))
		bnx2_set_link(bp);
	if (bnx2_phy_event_is_set(bp, bnapi, STATUS_ATTN_BITS_TIMER_ABORT))
		bnx2_set_remote_link(bp);

	spin_unlock(&bp->phy_lock);

}

static inline u16
bnx2_get_hw_tx_cons(struct bnx2_napi *bnapi)
{
	u16 cons;

	cons = READ_ONCE(*bnapi->hw_tx_cons_ptr);

	if (unlikely((cons & BNX2_MAX_TX_DESC_CNT) == BNX2_MAX_TX_DESC_CNT))
		cons++;
	return cons;
}

static int
bnx2_tx_int(struct bnx2 *bp, struct bnx2_napi *bnapi, int budget)
{
	struct bnx2_tx_ring_info *txr = &bnapi->tx_ring;
	u16 hw_cons, sw_cons, sw_ring_cons;
	int tx_pkt = 0, index;
	unsigned int tx_bytes = 0;
	struct netdev_queue *txq;

	index = (bnapi - bp->bnx2_napi);
	txq = netdev_get_tx_queue(bp->dev, index);

	hw_cons = bnx2_get_hw_tx_cons(bnapi);
	sw_cons = txr->tx_cons;

	while (sw_cons != hw_cons) {
		struct bnx2_sw_tx_bd *tx_buf;
		struct sk_buff *skb;
		int i, last;

		sw_ring_cons = BNX2_TX_RING_IDX(sw_cons);

		tx_buf = &txr->tx_buf_ring[sw_ring_cons];
		skb = tx_buf->skb;

		/* prefetch skb_end_pointer() to speedup skb_shinfo(skb) */
		prefetch(&skb->end);

		/* partial BD completions possible with TSO packets */
		if (tx_buf->is_gso) {
			u16 last_idx, last_ring_idx;

			last_idx = sw_cons + tx_buf->nr_frags + 1;
			last_ring_idx = sw_ring_cons + tx_buf->nr_frags + 1;
			if (unlikely(last_ring_idx >= BNX2_MAX_TX_DESC_CNT)) {
				last_idx++;
			}
			if (((s16) ((s16) last_idx - (s16) hw_cons)) > 0) {
				break;
			}
		}

		dma_unmap_single(&bp->pdev->dev, dma_unmap_addr(tx_buf, mapping),
			skb_headlen(skb), PCI_DMA_TODEVICE);

		tx_buf->skb = NULL;
		last = tx_buf->nr_frags;

		for (i = 0; i < last; i++) {
			struct bnx2_sw_tx_bd *tx_buf;

			sw_cons = BNX2_NEXT_TX_BD(sw_cons);

			tx_buf = &txr->tx_buf_ring[BNX2_TX_RING_IDX(sw_cons)];
			dma_unmap_page(&bp->pdev->dev,
				dma_unmap_addr(tx_buf, mapping),
				skb_frag_size(&skb_shinfo(skb)->frags[i]),
				PCI_DMA_TODEVICE);
		}

		sw_cons = BNX2_NEXT_TX_BD(sw_cons);

		tx_bytes += skb->len;
		dev_kfree_skb_any(skb);
		tx_pkt++;
		if (tx_pkt == budget)
			break;

		if (hw_cons == sw_cons)
			hw_cons = bnx2_get_hw_tx_cons(bnapi);
	}

	netdev_tx_completed_queue(txq, tx_pkt, tx_bytes);
	txr->hw_tx_cons = hw_cons;
	txr->tx_cons = sw_cons;

	/* Need to make the tx_cons update visible to bnx2_start_xmit()
	 * before checking for netif_tx_queue_stopped().  Without the
	 * memory barrier, there is a small possibility that bnx2_start_xmit()
	 * will miss it and cause the queue to be stopped forever.
	 */
	smp_mb();

	if (unlikely(netif_tx_queue_stopped(txq)) &&
		     (bnx2_tx_avail(bp, txr) > bp->tx_wake_thresh)) {
		__netif_tx_lock(txq, smp_processor_id());
		if ((netif_tx_queue_stopped(txq)) &&
		    (bnx2_tx_avail(bp, txr) > bp->tx_wake_thresh))
			netif_tx_wake_queue(txq);
		__netif_tx_unlock(txq);
	}

	return tx_pkt;
}

static void
bnx2_reuse_rx_skb_pages(struct bnx2 *bp, struct bnx2_rx_ring_info *rxr,
			struct sk_buff *skb, int count)
{
	struct bnx2_sw_pg *cons_rx_pg, *prod_rx_pg;
	struct bnx2_rx_bd *cons_bd, *prod_bd;
	int i;
	u16 hw_prod, prod;
	u16 cons = rxr->rx_pg_cons;

	cons_rx_pg = &rxr->rx_pg_ring[cons];

	/* The caller was unable to allocate a new page to replace the
	 * last one in the frags array, so we need to recycle that page
	 * and then free the skb.
	 */
	if (skb) {
		struct page *page;
		struct skb_shared_info *shinfo;

		shinfo = skb_shinfo(skb);
		shinfo->nr_frags--;
		page = skb_frag_page(&shinfo->frags[shinfo->nr_frags]);
		__skb_frag_set_page(&shinfo->frags[shinfo->nr_frags], NULL);

		cons_rx_pg->page = page;
		dev_kfree_skb(skb);
	}

	hw_prod = rxr->rx_pg_prod;

	for (i = 0; i < count; i++) {
		prod = BNX2_RX_PG_RING_IDX(hw_prod);

		prod_rx_pg = &rxr->rx_pg_ring[prod];
		cons_rx_pg = &rxr->rx_pg_ring[cons];
		cons_bd = &rxr->rx_pg_desc_ring[BNX2_RX_RING(cons)]
						[BNX2_RX_IDX(cons)];
		prod_bd = &rxr->rx_pg_desc_ring[BNX2_RX_RING(prod)]
						[BNX2_RX_IDX(prod)];

		if (prod != cons) {
			prod_rx_pg->page = cons_rx_pg->page;
			cons_rx_pg->page = NULL;
			dma_unmap_addr_set(prod_rx_pg, mapping,
				dma_unmap_addr(cons_rx_pg, mapping));

			prod_bd->rx_bd_haddr_hi = cons_bd->rx_bd_haddr_hi;
			prod_bd->rx_bd_haddr_lo = cons_bd->rx_bd_haddr_lo;

		}
		cons = BNX2_RX_PG_RING_IDX(BNX2_NEXT_RX_BD(cons));
		hw_prod = BNX2_NEXT_RX_BD(hw_prod);
	}
	rxr->rx_pg_prod = hw_prod;
	rxr->rx_pg_cons = cons;
}

static inline void
bnx2_reuse_rx_data(struct bnx2 *bp, struct bnx2_rx_ring_info *rxr,
		   u8 *data, u16 cons, u16 prod)
{
	struct bnx2_sw_bd *cons_rx_buf, *prod_rx_buf;
	struct bnx2_rx_bd *cons_bd, *prod_bd;

	cons_rx_buf = &rxr->rx_buf_ring[cons];
	prod_rx_buf = &rxr->rx_buf_ring[prod];

	dma_sync_single_for_device(&bp->pdev->dev,
		dma_unmap_addr(cons_rx_buf, mapping),
		BNX2_RX_OFFSET + BNX2_RX_COPY_THRESH, PCI_DMA_FROMDEVICE);

	rxr->rx_prod_bseq += bp->rx_buf_use_size;

	prod_rx_buf->data = data;

	if (cons == prod)
		return;

	dma_unmap_addr_set(prod_rx_buf, mapping,
			dma_unmap_addr(cons_rx_buf, mapping));

	cons_bd = &rxr->rx_desc_ring[BNX2_RX_RING(cons)][BNX2_RX_IDX(cons)];
	prod_bd = &rxr->rx_desc_ring[BNX2_RX_RING(prod)][BNX2_RX_IDX(prod)];
	prod_bd->rx_bd_haddr_hi = cons_bd->rx_bd_haddr_hi;
	prod_bd->rx_bd_haddr_lo = cons_bd->rx_bd_haddr_lo;
}

static struct sk_buff *
bnx2_rx_skb(struct bnx2 *bp, struct bnx2_rx_ring_info *rxr, u8 *data,
	    unsigned int len, unsigned int hdr_len, dma_addr_t dma_addr,
	    u32 ring_idx)
{
	int err;
	u16 prod = ring_idx & 0xffff;
	struct sk_buff *skb;

	err = bnx2_alloc_rx_data(bp, rxr, prod, GFP_ATOMIC);
	if (unlikely(err)) {
		bnx2_reuse_rx_data(bp, rxr, data, (u16) (ring_idx >> 16), prod);
error:
		if (hdr_len) {
			unsigned int raw_len = len + 4;
			int pages = PAGE_ALIGN(raw_len - hdr_len) >> PAGE_SHIFT;

			bnx2_reuse_rx_skb_pages(bp, rxr, NULL, pages);
		}
		return NULL;
	}

	dma_unmap_single(&bp->pdev->dev, dma_addr, bp->rx_buf_use_size,
			 PCI_DMA_FROMDEVICE);
	skb = build_skb(data, 0);
	if (!skb) {
		kfree(data);
		goto error;
	}
	skb_reserve(skb, ((u8 *)get_l2_fhdr(data) - data) + BNX2_RX_OFFSET);
	if (hdr_len == 0) {
		skb_put(skb, len);
		return skb;
	} else {
		unsigned int i, frag_len, frag_size, pages;
		struct bnx2_sw_pg *rx_pg;
		u16 pg_cons = rxr->rx_pg_cons;
		u16 pg_prod = rxr->rx_pg_prod;

		frag_size = len + 4 - hdr_len;
		pages = PAGE_ALIGN(frag_size) >> PAGE_SHIFT;
		skb_put(skb, hdr_len);

		for (i = 0; i < pages; i++) {
			dma_addr_t mapping_old;

			frag_len = min(frag_size, (unsigned int) PAGE_SIZE);
			if (unlikely(frag_len <= 4)) {
				unsigned int tail = 4 - frag_len;

				rxr->rx_pg_cons = pg_cons;
				rxr->rx_pg_prod = pg_prod;
				bnx2_reuse_rx_skb_pages(bp, rxr, NULL,
							pages - i);
				skb->len -= tail;
				if (i == 0) {
					skb->tail -= tail;
				} else {
					skb_frag_t *frag =
						&skb_shinfo(skb)->frags[i - 1];
					skb_frag_size_sub(frag, tail);
					skb->data_len -= tail;
				}
				return skb;
			}
			rx_pg = &rxr->rx_pg_ring[pg_cons];

			/* Don't unmap yet.  If we're unable to allocate a new
			 * page, we need to recycle the page and the DMA addr.
			 */
			mapping_old = dma_unmap_addr(rx_pg, mapping);
			if (i == pages - 1)
				frag_len -= 4;

			skb_fill_page_desc(skb, i, rx_pg->page, 0, frag_len);
			rx_pg->page = NULL;

			err = bnx2_alloc_rx_page(bp, rxr,
						 BNX2_RX_PG_RING_IDX(pg_prod),
						 GFP_ATOMIC);
			if (unlikely(err)) {
				rxr->rx_pg_cons = pg_cons;
				rxr->rx_pg_prod = pg_prod;
				bnx2_reuse_rx_skb_pages(bp, rxr, skb,
							pages - i);
				return NULL;
			}

			dma_unmap_page(&bp->pdev->dev, mapping_old,
				       PAGE_SIZE, PCI_DMA_FROMDEVICE);

			frag_size -= frag_len;
			skb->data_len += frag_len;
			skb->truesize += PAGE_SIZE;
			skb->len += frag_len;

			pg_prod = BNX2_NEXT_RX_BD(pg_prod);
			pg_cons = BNX2_RX_PG_RING_IDX(BNX2_NEXT_RX_BD(pg_cons));
		}
		rxr->rx_pg_prod = pg_prod;
		rxr->rx_pg_cons = pg_cons;
	}
	return skb;
}

static inline u16
bnx2_get_hw_rx_cons(struct bnx2_napi *bnapi)
{
	u16 cons;

	cons = READ_ONCE(*bnapi->hw_rx_cons_ptr);

	if (unlikely((cons & BNX2_MAX_RX_DESC_CNT) == BNX2_MAX_RX_DESC_CNT))
		cons++;
	return cons;
}

static int
bnx2_rx_int(struct bnx2 *bp, struct bnx2_napi *bnapi, int budget)
{
	struct bnx2_rx_ring_info *rxr = &bnapi->rx_ring;
	u16 hw_cons, sw_cons, sw_ring_cons, sw_prod, sw_ring_prod;
	struct l2_fhdr *rx_hdr;
	int rx_pkt = 0, pg_ring_used = 0;

	if (budget <= 0)
		return rx_pkt;

	hw_cons = bnx2_get_hw_rx_cons(bnapi);
	sw_cons = rxr->rx_cons;
	sw_prod = rxr->rx_prod;

	/* Memory barrier necessary as speculative reads of the rx
	 * buffer can be ahead of the index in the status block
	 */
	rmb();
	while (sw_cons != hw_cons) {
		unsigned int len, hdr_len;
		u32 status;
		struct bnx2_sw_bd *rx_buf, *next_rx_buf;
		struct sk_buff *skb;
		dma_addr_t dma_addr;
		u8 *data;
		u16 next_ring_idx;

		sw_ring_cons = BNX2_RX_RING_IDX(sw_cons);
		sw_ring_prod = BNX2_RX_RING_IDX(sw_prod);

		rx_buf = &rxr->rx_buf_ring[sw_ring_cons];
		data = rx_buf->data;
		rx_buf->data = NULL;

		rx_hdr = get_l2_fhdr(data);
		prefetch(rx_hdr);

		dma_addr = dma_unmap_addr(rx_buf, mapping);

		dma_sync_single_for_cpu(&bp->pdev->dev, dma_addr,
			BNX2_RX_OFFSET + BNX2_RX_COPY_THRESH,
			PCI_DMA_FROMDEVICE);

		next_ring_idx = BNX2_RX_RING_IDX(BNX2_NEXT_RX_BD(sw_cons));
		next_rx_buf = &rxr->rx_buf_ring[next_ring_idx];
		prefetch(get_l2_fhdr(next_rx_buf->data));

		len = rx_hdr->l2_fhdr_pkt_len;
		status = rx_hdr->l2_fhdr_status;

		hdr_len = 0;
		if (status & L2_FHDR_STATUS_SPLIT) {
			hdr_len = rx_hdr->l2_fhdr_ip_xsum;
			pg_ring_used = 1;
		} else if (len > bp->rx_jumbo_thresh) {
			hdr_len = bp->rx_jumbo_thresh;
			pg_ring_used = 1;
		}

		if (unlikely(status & (L2_FHDR_ERRORS_BAD_CRC |
				       L2_FHDR_ERRORS_PHY_DECODE |
				       L2_FHDR_ERRORS_ALIGNMENT |
				       L2_FHDR_ERRORS_TOO_SHORT |
				       L2_FHDR_ERRORS_GIANT_FRAME))) {

			bnx2_reuse_rx_data(bp, rxr, data, sw_ring_cons,
					  sw_ring_prod);
			if (pg_ring_used) {
				int pages;

				pages = PAGE_ALIGN(len - hdr_len) >> PAGE_SHIFT;

				bnx2_reuse_rx_skb_pages(bp, rxr, NULL, pages);
			}
			goto next_rx;
		}

		len -= 4;

		if (len <= bp->rx_copy_thresh) {
			skb = netdev_alloc_skb(bp->dev, len + 6);
			if (!skb) {
				bnx2_reuse_rx_data(bp, rxr, data, sw_ring_cons,
						  sw_ring_prod);
				goto next_rx;
			}

			/* aligned copy */
			memcpy(skb->data,
			       (u8 *)rx_hdr + BNX2_RX_OFFSET - 6,
			       len + 6);
			skb_reserve(skb, 6);
			skb_put(skb, len);

			bnx2_reuse_rx_data(bp, rxr, data,
				sw_ring_cons, sw_ring_prod);

		} else {
			skb = bnx2_rx_skb(bp, rxr, data, len, hdr_len, dma_addr,
					  (sw_ring_cons << 16) | sw_ring_prod);
			if (!skb)
				goto next_rx;
		}
		if ((status & L2_FHDR_STATUS_L2_VLAN_TAG) &&
		    !(bp->rx_mode & BNX2_EMAC_RX_MODE_KEEP_VLAN_TAG))
			__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), rx_hdr->l2_fhdr_vlan_tag);

		skb->protocol = eth_type_trans(skb, bp->dev);

		if (len > (bp->dev->mtu + ETH_HLEN) &&
		    skb->protocol != htons(0x8100) &&
		    skb->protocol != htons(ETH_P_8021AD)) {

			dev_kfree_skb(skb);
			goto next_rx;

		}

		skb_checksum_none_assert(skb);
		if ((bp->dev->features & NETIF_F_RXCSUM) &&
			(status & (L2_FHDR_STATUS_TCP_SEGMENT |
			L2_FHDR_STATUS_UDP_DATAGRAM))) {

			if (likely((status & (L2_FHDR_ERRORS_TCP_XSUM |
					      L2_FHDR_ERRORS_UDP_XSUM)) == 0))
				skb->ip_summed = CHECKSUM_UNNECESSARY;
		}
		if ((bp->dev->features & NETIF_F_RXHASH) &&
		    ((status & L2_FHDR_STATUS_USE_RXHASH) ==
		     L2_FHDR_STATUS_USE_RXHASH))
			skb_set_hash(skb, rx_hdr->l2_fhdr_hash,
				     PKT_HASH_TYPE_L3);

		skb_record_rx_queue(skb, bnapi - &bp->bnx2_napi[0]);
		napi_gro_receive(&bnapi->napi, skb);
		rx_pkt++;

next_rx:
		sw_cons = BNX2_NEXT_RX_BD(sw_cons);
		sw_prod = BNX2_NEXT_RX_BD(sw_prod);

		if (rx_pkt == budget)
			break;

		/* Refresh hw_cons to see if there is new work */
		if (sw_cons == hw_cons) {
			hw_cons = bnx2_get_hw_rx_cons(bnapi);
			rmb();
		}
	}
	rxr->rx_cons = sw_cons;
	rxr->rx_prod = sw_prod;

	if (pg_ring_used)
		BNX2_WR16(bp, rxr->rx_pg_bidx_addr, rxr->rx_pg_prod);

	BNX2_WR16(bp, rxr->rx_bidx_addr, sw_prod);

	BNX2_WR(bp, rxr->rx_bseq_addr, rxr->rx_prod_bseq);

	return rx_pkt;

}

/* MSI ISR - The only difference between this and the INTx ISR
 * is that the MSI interrupt is always serviced.
 */
static irqreturn_t
bnx2_msi(int irq, void *dev_instance)
{
	struct bnx2_napi *bnapi = dev_instance;
	struct bnx2 *bp = bnapi->bp;

	prefetch(bnapi->status_blk.msi);
	BNX2_WR(bp, BNX2_PCICFG_INT_ACK_CMD,
		BNX2_PCICFG_INT_ACK_CMD_USE_INT_HC_PARAM |
		BNX2_PCICFG_INT_ACK_CMD_MASK_INT);

	/* Return here if interrupt is disabled. */
	if (unlikely(atomic_read(&bp->intr_sem) != 0))
		return IRQ_HANDLED;

	napi_schedule(&bnapi->napi);

	return IRQ_HANDLED;
}

static irqreturn_t
bnx2_msi_1shot(int irq, void *dev_instance)
{
	struct bnx2_napi *bnapi = dev_instance;
	struct bnx2 *bp = bnapi->bp;

	prefetch(bnapi->status_blk.msi);

	/* Return here if interrupt is disabled. */
	if (unlikely(atomic_read(&bp->intr_sem) != 0))
		return IRQ_HANDLED;

	napi_schedule(&bnapi->napi);

	return IRQ_HANDLED;
}

static irqreturn_t
bnx2_interrupt(int irq, void *dev_instance)
{
	struct bnx2_napi *bnapi = dev_instance;
	struct bnx2 *bp = bnapi->bp;
	struct status_block *sblk = bnapi->status_blk.msi;

	/* When using INTx, it is possible for the interrupt to arrive
	 * at the CPU before the status block posted prior to the
	 * interrupt. Reading a register will flush the status block.
	 * When using MSI, the MSI message will always complete after
	 * the status block write.
	 */
	if ((sblk->status_idx == bnapi->last_status_idx) &&
	    (BNX2_RD(bp, BNX2_PCICFG_MISC_STATUS) &
	     BNX2_PCICFG_MISC_STATUS_INTA_VALUE))
		return IRQ_NONE;

	BNX2_WR(bp, BNX2_PCICFG_INT_ACK_CMD,
		BNX2_PCICFG_INT_ACK_CMD_USE_INT_HC_PARAM |
		BNX2_PCICFG_INT_ACK_CMD_MASK_INT);

	/* Read back to deassert IRQ immediately to avoid too many
	 * spurious interrupts.
	 */
	BNX2_RD(bp, BNX2_PCICFG_INT_ACK_CMD);

	/* Return here if interrupt is shared and is disabled. */
	if (unlikely(atomic_read(&bp->intr_sem) != 0))
		return IRQ_HANDLED;

	if (napi_schedule_prep(&bnapi->napi)) {
		bnapi->last_status_idx = sblk->status_idx;
		__napi_schedule(&bnapi->napi);
	}

	return IRQ_HANDLED;
}

static inline int
bnx2_has_fast_work(struct bnx2_napi *bnapi)
{
	struct bnx2_tx_ring_info *txr = &bnapi->tx_ring;
	struct bnx2_rx_ring_info *rxr = &bnapi->rx_ring;

	if ((bnx2_get_hw_rx_cons(bnapi) != rxr->rx_cons) ||
	    (bnx2_get_hw_tx_cons(bnapi) != txr->hw_tx_cons))
		return 1;
	return 0;
}

#define STATUS_ATTN_EVENTS	(STATUS_ATTN_BITS_LINK_STATE | \
				 STATUS_ATTN_BITS_TIMER_ABORT)

static inline int
bnx2_has_work(struct bnx2_napi *bnapi)
{
	struct status_block *sblk = bnapi->status_blk.msi;

	if (bnx2_has_fast_work(bnapi))
		return 1;

#ifdef BCM_CNIC
	if (bnapi->cnic_present && (bnapi->cnic_tag != sblk->status_idx))
		return 1;
#endif

	if ((sblk->status_attn_bits & STATUS_ATTN_EVENTS) !=
	    (sblk->status_attn_bits_ack & STATUS_ATTN_EVENTS))
		return 1;

	return 0;
}

static void
bnx2_chk_missed_msi(struct bnx2 *bp)
{
	struct bnx2_napi *bnapi = &bp->bnx2_napi[0];
	u32 msi_ctrl;

	if (bnx2_has_work(bnapi)) {
		msi_ctrl = BNX2_RD(bp, BNX2_PCICFG_MSI_CONTROL);
		if (!(msi_ctrl & BNX2_PCICFG_MSI_CONTROL_ENABLE))
			return;

		if (bnapi->last_status_idx == bp->idle_chk_status_idx) {
			BNX2_WR(bp, BNX2_PCICFG_MSI_CONTROL, msi_ctrl &
				~BNX2_PCICFG_MSI_CONTROL_ENABLE);
			BNX2_WR(bp, BNX2_PCICFG_MSI_CONTROL, msi_ctrl);
			bnx2_msi(bp->irq_tbl[0].vector, bnapi);
		}
	}

	bp->idle_chk_status_idx = bnapi->last_status_idx;
}

#ifdef BCM_CNIC
static void bnx2_poll_cnic(struct bnx2 *bp, struct bnx2_napi *bnapi)
{
	struct cnic_ops *c_ops;

	if (!bnapi->cnic_present)
		return;

	rcu_read_lock();
	c_ops = rcu_dereference(bp->cnic_ops);
	if (c_ops)
		bnapi->cnic_tag = c_ops->cnic_handler(bp->cnic_data,
						      bnapi->status_blk.msi);
	rcu_read_unlock();
}
#endif

static void bnx2_poll_link(struct bnx2 *bp, struct bnx2_napi *bnapi)
{
	struct status_block *sblk = bnapi->status_blk.msi;
	u32 status_attn_bits = sblk->status_attn_bits;
	u32 status_attn_bits_ack = sblk->status_attn_bits_ack;

	if ((status_attn_bits & STATUS_ATTN_EVENTS) !=
	    (status_attn_bits_ack & STATUS_ATTN_EVENTS)) {

		bnx2_phy_int(bp, bnapi);

		/* This is needed to take care of transient status
		 * during link changes.
		 */
		BNX2_WR(bp, BNX2_HC_COMMAND,
			bp->hc_cmd | BNX2_HC_COMMAND_COAL_NOW_WO_INT);
		BNX2_RD(bp, BNX2_HC_COMMAND);
	}
}

static int bnx2_poll_work(struct bnx2 *bp, struct bnx2_napi *bnapi,
			  int work_done, int budget)
{
	struct bnx2_tx_ring_info *txr = &bnapi->tx_ring;
	struct bnx2_rx_ring_info *rxr = &bnapi->rx_ring;

	if (bnx2_get_hw_tx_cons(bnapi) != txr->hw_tx_cons)
		bnx2_tx_int(bp, bnapi, 0);

	if (bnx2_get_hw_rx_cons(bnapi) != rxr->rx_cons)
		work_done += bnx2_rx_int(bp, bnapi, budget - work_done);

	return work_done;
}

static int bnx2_poll_msix(struct napi_struct *napi, int budget)
{
	struct bnx2_napi *bnapi = container_of(napi, struct bnx2_napi, napi);
	struct bnx2 *bp = bnapi->bp;
	int work_done = 0;
	struct status_block_msix *sblk = bnapi->status_blk.msix;

	while (1) {
		work_done = bnx2_poll_work(bp, bnapi, work_done, budget);
		if (unlikely(work_done >= budget))
			break;

		bnapi->last_status_idx = sblk->status_idx;
		/* status idx must be read before checking for more work. */
		rmb();
		if (likely(!bnx2_has_fast_work(bnapi))) {

			napi_complete_done(napi, work_done);
			BNX2_WR(bp, BNX2_PCICFG_INT_ACK_CMD, bnapi->int_num |
				BNX2_PCICFG_INT_ACK_CMD_INDEX_VALID |
				bnapi->last_status_idx);
			break;
		}
	}
	return work_done;
}

static int bnx2_poll(struct napi_struct *napi, int budget)
{
	struct bnx2_napi *bnapi = container_of(napi, struct bnx2_napi, napi);
	struct bnx2 *bp = bnapi->bp;
	int work_done = 0;
	struct status_block *sblk = bnapi->status_blk.msi;

	while (1) {
		bnx2_poll_link(bp, bnapi);

		work_done = bnx2_poll_work(bp, bnapi, work_done, budget);

#ifdef BCM_CNIC
		bnx2_poll_cnic(bp, bnapi);
#endif

		/* bnapi->last_status_idx is used below to tell the hw how
		 * much work has been processed, so we must read it before
		 * checking for more work.
		 */
		bnapi->last_status_idx = sblk->status_idx;

		if (unlikely(work_done >= budget))
			break;

		rmb();
		if (likely(!bnx2_has_work(bnapi))) {
			napi_complete_done(napi, work_done);
			if (likely(bp->flags & BNX2_FLAG_USING_MSI_OR_MSIX)) {
				BNX2_WR(bp, BNX2_PCICFG_INT_ACK_CMD,
					BNX2_PCICFG_INT_ACK_CMD_INDEX_VALID |
					bnapi->last_status_idx);
				break;
			}
			BNX2_WR(bp, BNX2_PCICFG_INT_ACK_CMD,
				BNX2_PCICFG_INT_ACK_CMD_INDEX_VALID |
				BNX2_PCICFG_INT_ACK_CMD_MASK_INT |
				bnapi->last_status_idx);

			BNX2_WR(bp, BNX2_PCICFG_INT_ACK_CMD,
				BNX2_PCICFG_INT_ACK_CMD_INDEX_VALID |
				bnapi->last_status_idx);
			break;
		}
	}

	return work_done;
}

/* Called with rtnl_lock from vlan functions and also netif_tx_lock
 * from set_multicast.
 */
static void
bnx2_set_rx_mode(struct net_device *dev)
{
	struct bnx2 *bp = netdev_priv(dev);
	u32 rx_mode, sort_mode;
	struct netdev_hw_addr *ha;
	int i;

	if (!netif_running(dev))
		return;

	spin_lock_bh(&bp->phy_lock);

	rx_mode = bp->rx_mode & ~(BNX2_EMAC_RX_MODE_PROMISCUOUS |
				  BNX2_EMAC_RX_MODE_KEEP_VLAN_TAG);
	sort_mode = 1 | BNX2_RPM_SORT_USER0_BC_EN;
	if (!(dev->features & NETIF_F_HW_VLAN_CTAG_RX) &&
	     (bp->flags & BNX2_FLAG_CAN_KEEP_VLAN))
		rx_mode |= BNX2_EMAC_RX_MODE_KEEP_VLAN_TAG;
	if (dev->flags & IFF_PROMISC) {
		/* Promiscuous mode. */
		rx_mode |= BNX2_EMAC_RX_MODE_PROMISCUOUS;
		sort_mode |= BNX2_RPM_SORT_USER0_PROM_EN |
			     BNX2_RPM_SORT_USER0_PROM_VLAN;
	}
	else if (dev->flags & IFF_ALLMULTI) {
		for (i = 0; i < NUM_MC_HASH_REGISTERS; i++) {
			BNX2_WR(bp, BNX2_EMAC_MULTICAST_HASH0 + (i * 4),
				0xffffffff);
        	}
		sort_mode |= BNX2_RPM_SORT_USER0_MC_EN;
	}
	else {
		/* Accept one or more multicast(s). */
		u32 mc_filter[NUM_MC_HASH_REGISTERS];
		u32 regidx;
		u32 bit;
		u32 crc;

		memset(mc_filter, 0, 4 * NUM_MC_HASH_REGISTERS);

		netdev_for_each_mc_addr(ha, dev) {
			crc = ether_crc_le(ETH_ALEN, ha->addr);
			bit = crc & 0xff;
			regidx = (bit & 0xe0) >> 5;
			bit &= 0x1f;
			mc_filter[regidx] |= (1 << bit);
		}

		for (i = 0; i < NUM_MC_HASH_REGISTERS; i++) {
			BNX2_WR(bp, BNX2_EMAC_MULTICAST_HASH0 + (i * 4),
				mc_filter[i]);
		}

		sort_mode |= BNX2_RPM_SORT_USER0_MC_HSH_EN;
	}

	if (netdev_uc_count(dev) > BNX2_MAX_UNICAST_ADDRESSES) {
		rx_mode |= BNX2_EMAC_RX_MODE_PROMISCUOUS;
		sort_mode |= BNX2_RPM_SORT_USER0_PROM_EN |
			     BNX2_RPM_SORT_USER0_PROM_VLAN;
	} else if (!(dev->flags & IFF_PROMISC)) {
		/* Add all entries into to the match filter list */
		i = 0;
		netdev_for_each_uc_addr(ha, dev) {
			bnx2_set_mac_addr(bp, ha->addr,
					  i + BNX2_START_UNICAST_ADDRESS_INDEX);
			sort_mode |= (1 <<
				      (i + BNX2_START_UNICAST_ADDRESS_INDEX));
			i++;
		}

	}

	if (rx_mode != bp->rx_mode) {
		bp->rx_mode = rx_mode;
		BNX2_WR(bp, BNX2_EMAC_RX_MODE, rx_mode);
	}

	BNX2_WR(bp, BNX2_RPM_SORT_USER0, 0x0);
	BNX2_WR(bp, BNX2_RPM_SORT_USER0, sort_mode);
	BNX2_WR(bp, BNX2_RPM_SORT_USER0, sort_mode | BNX2_RPM_SORT_USER0_ENA);

	spin_unlock_bh(&bp->phy_lock);
}

static int
check_fw_section(const struct firmware *fw,
		 const struct bnx2_fw_file_section *section,
		 u32 alignment, bool non_empty)
{
	u32 offset = be32_to_cpu(section->offset);
	u32 len = be32_to_cpu(section->len);

	if ((offset == 0 && len != 0) || offset >= fw->size || offset & 3)
		return -EINVAL;
	if ((non_empty && len == 0) || len > fw->size - offset ||
	    len & (alignment - 1))
		return -EINVAL;
	return 0;
}

static int
check_mips_fw_entry(const struct firmware *fw,
		    const struct bnx2_mips_fw_file_entry *entry)
{
	if (check_fw_section(fw, &entry->text, 4, true) ||
	    check_fw_section(fw, &entry->data, 4, false) ||
	    check_fw_section(fw, &entry->rodata, 4, false))
		return -EINVAL;
	return 0;
}

static void bnx2_release_firmware(struct bnx2 *bp)
{
	if (bp->rv2p_firmware) {
		release_firmware(bp->mips_firmware);
		release_firmware(bp->rv2p_firmware);
		bp->rv2p_firmware = NULL;
	}
}

static int bnx2_request_uncached_firmware(struct bnx2 *bp)
{
	const char *mips_fw_file, *rv2p_fw_file;
	const struct bnx2_mips_fw_file *mips_fw;
	const struct bnx2_rv2p_fw_file *rv2p_fw;
	int rc;

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		mips_fw_file = FW_MIPS_FILE_09;
		if ((BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5709_A0) ||
		    (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5709_A1))
			rv2p_fw_file = FW_RV2P_FILE_09_Ax;
		else
			rv2p_fw_file = FW_RV2P_FILE_09;
	} else {
		mips_fw_file = FW_MIPS_FILE_06;
		rv2p_fw_file = FW_RV2P_FILE_06;
	}

	rc = request_firmware(&bp->mips_firmware, mips_fw_file, &bp->pdev->dev);
	if (rc) {
		pr_err("Can't load firmware file \"%s\"\n", mips_fw_file);
		goto out;
	}

	rc = request_firmware(&bp->rv2p_firmware, rv2p_fw_file, &bp->pdev->dev);
	if (rc) {
		pr_err("Can't load firmware file \"%s\"\n", rv2p_fw_file);
		goto err_release_mips_firmware;
	}
	mips_fw = (const struct bnx2_mips_fw_file *) bp->mips_firmware->data;
	rv2p_fw = (const struct bnx2_rv2p_fw_file *) bp->rv2p_firmware->data;
	if (bp->mips_firmware->size < sizeof(*mips_fw) ||
	    check_mips_fw_entry(bp->mips_firmware, &mips_fw->com) ||
	    check_mips_fw_entry(bp->mips_firmware, &mips_fw->cp) ||
	    check_mips_fw_entry(bp->mips_firmware, &mips_fw->rxp) ||
	    check_mips_fw_entry(bp->mips_firmware, &mips_fw->tpat) ||
	    check_mips_fw_entry(bp->mips_firmware, &mips_fw->txp)) {
		pr_err("Firmware file \"%s\" is invalid\n", mips_fw_file);
		rc = -EINVAL;
		goto err_release_firmware;
	}
	if (bp->rv2p_firmware->size < sizeof(*rv2p_fw) ||
	    check_fw_section(bp->rv2p_firmware, &rv2p_fw->proc1.rv2p, 8, true) ||
	    check_fw_section(bp->rv2p_firmware, &rv2p_fw->proc2.rv2p, 8, true)) {
		pr_err("Firmware file \"%s\" is invalid\n", rv2p_fw_file);
		rc = -EINVAL;
		goto err_release_firmware;
	}
out:
	return rc;

err_release_firmware:
	release_firmware(bp->rv2p_firmware);
	bp->rv2p_firmware = NULL;
err_release_mips_firmware:
	release_firmware(bp->mips_firmware);
	goto out;
}

static int bnx2_request_firmware(struct bnx2 *bp)
{
	return bp->rv2p_firmware ? 0 : bnx2_request_uncached_firmware(bp);
}

static u32
rv2p_fw_fixup(u32 rv2p_proc, int idx, u32 loc, u32 rv2p_code)
{
	switch (idx) {
	case RV2P_P1_FIXUP_PAGE_SIZE_IDX:
		rv2p_code &= ~RV2P_BD_PAGE_SIZE_MSK;
		rv2p_code |= RV2P_BD_PAGE_SIZE;
		break;
	}
	return rv2p_code;
}

static int
load_rv2p_fw(struct bnx2 *bp, u32 rv2p_proc,
	     const struct bnx2_rv2p_fw_file_entry *fw_entry)
{
	u32 rv2p_code_len, file_offset;
	__be32 *rv2p_code;
	int i;
	u32 val, cmd, addr;

	rv2p_code_len = be32_to_cpu(fw_entry->rv2p.len);
	file_offset = be32_to_cpu(fw_entry->rv2p.offset);

	rv2p_code = (__be32 *)(bp->rv2p_firmware->data + file_offset);

	if (rv2p_proc == RV2P_PROC1) {
		cmd = BNX2_RV2P_PROC1_ADDR_CMD_RDWR;
		addr = BNX2_RV2P_PROC1_ADDR_CMD;
	} else {
		cmd = BNX2_RV2P_PROC2_ADDR_CMD_RDWR;
		addr = BNX2_RV2P_PROC2_ADDR_CMD;
	}

	for (i = 0; i < rv2p_code_len; i += 8) {
		BNX2_WR(bp, BNX2_RV2P_INSTR_HIGH, be32_to_cpu(*rv2p_code));
		rv2p_code++;
		BNX2_WR(bp, BNX2_RV2P_INSTR_LOW, be32_to_cpu(*rv2p_code));
		rv2p_code++;

		val = (i / 8) | cmd;
		BNX2_WR(bp, addr, val);
	}

	rv2p_code = (__be32 *)(bp->rv2p_firmware->data + file_offset);
	for (i = 0; i < 8; i++) {
		u32 loc, code;

		loc = be32_to_cpu(fw_entry->fixup[i]);
		if (loc && ((loc * 4) < rv2p_code_len)) {
			code = be32_to_cpu(*(rv2p_code + loc - 1));
			BNX2_WR(bp, BNX2_RV2P_INSTR_HIGH, code);
			code = be32_to_cpu(*(rv2p_code + loc));
			code = rv2p_fw_fixup(rv2p_proc, i, loc, code);
			BNX2_WR(bp, BNX2_RV2P_INSTR_LOW, code);

			val = (loc / 2) | cmd;
			BNX2_WR(bp, addr, val);
		}
	}

	/* Reset the processor, un-stall is done later. */
	if (rv2p_proc == RV2P_PROC1) {
		BNX2_WR(bp, BNX2_RV2P_COMMAND, BNX2_RV2P_COMMAND_PROC1_RESET);
	}
	else {
		BNX2_WR(bp, BNX2_RV2P_COMMAND, BNX2_RV2P_COMMAND_PROC2_RESET);
	}

	return 0;
}

static int
load_cpu_fw(struct bnx2 *bp, const struct cpu_reg *cpu_reg,
	    const struct bnx2_mips_fw_file_entry *fw_entry)
{
	u32 addr, len, file_offset;
	__be32 *data;
	u32 offset;
	u32 val;

	/* Halt the CPU. */
	val = bnx2_reg_rd_ind(bp, cpu_reg->mode);
	val |= cpu_reg->mode_value_halt;
	bnx2_reg_wr_ind(bp, cpu_reg->mode, val);
	bnx2_reg_wr_ind(bp, cpu_reg->state, cpu_reg->state_value_clear);

	/* Load the Text area. */
	addr = be32_to_cpu(fw_entry->text.addr);
	len = be32_to_cpu(fw_entry->text.len);
	file_offset = be32_to_cpu(fw_entry->text.offset);
	data = (__be32 *)(bp->mips_firmware->data + file_offset);

	offset = cpu_reg->spad_base + (addr - cpu_reg->mips_view_base);
	if (len) {
		int j;

		for (j = 0; j < (len / 4); j++, offset += 4)
			bnx2_reg_wr_ind(bp, offset, be32_to_cpu(data[j]));
	}

	/* Load the Data area. */
	addr = be32_to_cpu(fw_entry->data.addr);
	len = be32_to_cpu(fw_entry->data.len);
	file_offset = be32_to_cpu(fw_entry->data.offset);
	data = (__be32 *)(bp->mips_firmware->data + file_offset);

	offset = cpu_reg->spad_base + (addr - cpu_reg->mips_view_base);
	if (len) {
		int j;

		for (j = 0; j < (len / 4); j++, offset += 4)
			bnx2_reg_wr_ind(bp, offset, be32_to_cpu(data[j]));
	}

	/* Load the Read-Only area. */
	addr = be32_to_cpu(fw_entry->rodata.addr);
	len = be32_to_cpu(fw_entry->rodata.len);
	file_offset = be32_to_cpu(fw_entry->rodata.offset);
	data = (__be32 *)(bp->mips_firmware->data + file_offset);

	offset = cpu_reg->spad_base + (addr - cpu_reg->mips_view_base);
	if (len) {
		int j;

		for (j = 0; j < (len / 4); j++, offset += 4)
			bnx2_reg_wr_ind(bp, offset, be32_to_cpu(data[j]));
	}

	/* Clear the pre-fetch instruction. */
	bnx2_reg_wr_ind(bp, cpu_reg->inst, 0);

	val = be32_to_cpu(fw_entry->start_addr);
	bnx2_reg_wr_ind(bp, cpu_reg->pc, val);

	/* Start the CPU. */
	val = bnx2_reg_rd_ind(bp, cpu_reg->mode);
	val &= ~cpu_reg->mode_value_halt;
	bnx2_reg_wr_ind(bp, cpu_reg->state, cpu_reg->state_value_clear);
	bnx2_reg_wr_ind(bp, cpu_reg->mode, val);

	return 0;
}

static int
bnx2_init_cpus(struct bnx2 *bp)
{
	const struct bnx2_mips_fw_file *mips_fw =
		(const struct bnx2_mips_fw_file *) bp->mips_firmware->data;
	const struct bnx2_rv2p_fw_file *rv2p_fw =
		(const struct bnx2_rv2p_fw_file *) bp->rv2p_firmware->data;
	int rc;

	/* Initialize the RV2P processor. */
	load_rv2p_fw(bp, RV2P_PROC1, &rv2p_fw->proc1);
	load_rv2p_fw(bp, RV2P_PROC2, &rv2p_fw->proc2);

	/* Initialize the RX Processor. */
	rc = load_cpu_fw(bp, &cpu_reg_rxp, &mips_fw->rxp);
	if (rc)
		goto init_cpu_err;

	/* Initialize the TX Processor. */
	rc = load_cpu_fw(bp, &cpu_reg_txp, &mips_fw->txp);
	if (rc)
		goto init_cpu_err;

	/* Initialize the TX Patch-up Processor. */
	rc = load_cpu_fw(bp, &cpu_reg_tpat, &mips_fw->tpat);
	if (rc)
		goto init_cpu_err;

	/* Initialize the Completion Processor. */
	rc = load_cpu_fw(bp, &cpu_reg_com, &mips_fw->com);
	if (rc)
		goto init_cpu_err;

	/* Initialize the Command Processor. */
	rc = load_cpu_fw(bp, &cpu_reg_cp, &mips_fw->cp);

init_cpu_err:
	return rc;
}

static void
bnx2_setup_wol(struct bnx2 *bp)
{
	int i;
	u32 val, wol_msg;

	if (bp->wol) {
		u32 advertising;
		u8 autoneg;

		autoneg = bp->autoneg;
		advertising = bp->advertising;

		if (bp->phy_port == PORT_TP) {
			bp->autoneg = AUTONEG_SPEED;
			bp->advertising = ADVERTISED_10baseT_Half |
				ADVERTISED_10baseT_Full |
				ADVERTISED_100baseT_Half |
				ADVERTISED_100baseT_Full |
				ADVERTISED_Autoneg;
		}

		spin_lock_bh(&bp->phy_lock);
		bnx2_setup_phy(bp, bp->phy_port);
		spin_unlock_bh(&bp->phy_lock);

		bp->autoneg = autoneg;
		bp->advertising = advertising;

		bnx2_set_mac_addr(bp, bp->dev->dev_addr, 0);

		val = BNX2_RD(bp, BNX2_EMAC_MODE);

		/* Enable port mode. */
		val &= ~BNX2_EMAC_MODE_PORT;
		val |= BNX2_EMAC_MODE_MPKT_RCVD |
		       BNX2_EMAC_MODE_ACPI_RCVD |
		       BNX2_EMAC_MODE_MPKT;
		if (bp->phy_port == PORT_TP) {
			val |= BNX2_EMAC_MODE_PORT_MII;
		} else {
			val |= BNX2_EMAC_MODE_PORT_GMII;
			if (bp->line_speed == SPEED_2500)
				val |= BNX2_EMAC_MODE_25G_MODE;
		}

		BNX2_WR(bp, BNX2_EMAC_MODE, val);

		/* receive all multicast */
		for (i = 0; i < NUM_MC_HASH_REGISTERS; i++) {
			BNX2_WR(bp, BNX2_EMAC_MULTICAST_HASH0 + (i * 4),
				0xffffffff);
		}
		BNX2_WR(bp, BNX2_EMAC_RX_MODE, BNX2_EMAC_RX_MODE_SORT_MODE);

		val = 1 | BNX2_RPM_SORT_USER0_BC_EN | BNX2_RPM_SORT_USER0_MC_EN;
		BNX2_WR(bp, BNX2_RPM_SORT_USER0, 0x0);
		BNX2_WR(bp, BNX2_RPM_SORT_USER0, val);
		BNX2_WR(bp, BNX2_RPM_SORT_USER0, val | BNX2_RPM_SORT_USER0_ENA);

		/* Need to enable EMAC and RPM for WOL. */
		BNX2_WR(bp, BNX2_MISC_ENABLE_SET_BITS,
			BNX2_MISC_ENABLE_SET_BITS_RX_PARSER_MAC_ENABLE |
			BNX2_MISC_ENABLE_SET_BITS_TX_HEADER_Q_ENABLE |
			BNX2_MISC_ENABLE_SET_BITS_EMAC_ENABLE);

		val = BNX2_RD(bp, BNX2_RPM_CONFIG);
		val &= ~BNX2_RPM_CONFIG_ACPI_ENA;
		BNX2_WR(bp, BNX2_RPM_CONFIG, val);

		wol_msg = BNX2_DRV_MSG_CODE_SUSPEND_WOL;
	} else {
			wol_msg = BNX2_DRV_MSG_CODE_SUSPEND_NO_WOL;
	}

	if (!(bp->flags & BNX2_FLAG_NO_WOL)) {
		u32 val;

		wol_msg |= BNX2_DRV_MSG_DATA_WAIT3;
		if (bp->fw_last_msg || BNX2_CHIP(bp) != BNX2_CHIP_5709) {
			bnx2_fw_sync(bp, wol_msg, 1, 0);
			return;
		}
		/* Tell firmware not to power down the PHY yet, otherwise
		 * the chip will take a long time to respond to MMIO reads.
		 */
		val = bnx2_shmem_rd(bp, BNX2_PORT_FEATURE);
		bnx2_shmem_wr(bp, BNX2_PORT_FEATURE,
			      val | BNX2_PORT_FEATURE_ASF_ENABLED);
		bnx2_fw_sync(bp, wol_msg, 1, 0);
		bnx2_shmem_wr(bp, BNX2_PORT_FEATURE, val);
	}

}

static int
bnx2_set_power_state(struct bnx2 *bp, pci_power_t state)
{
	switch (state) {
	case PCI_D0: {
		u32 val;

		pci_enable_wake(bp->pdev, PCI_D0, false);
		pci_set_power_state(bp->pdev, PCI_D0);

		val = BNX2_RD(bp, BNX2_EMAC_MODE);
		val |= BNX2_EMAC_MODE_MPKT_RCVD | BNX2_EMAC_MODE_ACPI_RCVD;
		val &= ~BNX2_EMAC_MODE_MPKT;
		BNX2_WR(bp, BNX2_EMAC_MODE, val);

		val = BNX2_RD(bp, BNX2_RPM_CONFIG);
		val &= ~BNX2_RPM_CONFIG_ACPI_ENA;
		BNX2_WR(bp, BNX2_RPM_CONFIG, val);
		break;
	}
	case PCI_D3hot: {
		bnx2_setup_wol(bp);
		pci_wake_from_d3(bp->pdev, bp->wol);
		if ((BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A0) ||
		    (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A1)) {

			if (bp->wol)
				pci_set_power_state(bp->pdev, PCI_D3hot);
			break;

		}
		if (!bp->fw_last_msg && BNX2_CHIP(bp) == BNX2_CHIP_5709) {
			u32 val;

			/* Tell firmware not to power down the PHY yet,
			 * otherwise the other port may not respond to
			 * MMIO reads.
			 */
			val = bnx2_shmem_rd(bp, BNX2_BC_STATE_CONDITION);
			val &= ~BNX2_CONDITION_PM_STATE_MASK;
			val |= BNX2_CONDITION_PM_STATE_UNPREP;
			bnx2_shmem_wr(bp, BNX2_BC_STATE_CONDITION, val);
		}
		pci_set_power_state(bp->pdev, PCI_D3hot);

		/* No more memory access after this point until
		 * device is brought back to D0.
		 */
		break;
	}
	default:
		return -EINVAL;
	}
	return 0;
}

static int
bnx2_acquire_nvram_lock(struct bnx2 *bp)
{
	u32 val;
	int j;

	/* Request access to the flash interface. */
	BNX2_WR(bp, BNX2_NVM_SW_ARB, BNX2_NVM_SW_ARB_ARB_REQ_SET2);
	for (j = 0; j < NVRAM_TIMEOUT_COUNT; j++) {
		val = BNX2_RD(bp, BNX2_NVM_SW_ARB);
		if (val & BNX2_NVM_SW_ARB_ARB_ARB2)
			break;

		udelay(5);
	}

	if (j >= NVRAM_TIMEOUT_COUNT)
		return -EBUSY;

	return 0;
}

static int
bnx2_release_nvram_lock(struct bnx2 *bp)
{
	int j;
	u32 val;

	/* Relinquish nvram interface. */
	BNX2_WR(bp, BNX2_NVM_SW_ARB, BNX2_NVM_SW_ARB_ARB_REQ_CLR2);

	for (j = 0; j < NVRAM_TIMEOUT_COUNT; j++) {
		val = BNX2_RD(bp, BNX2_NVM_SW_ARB);
		if (!(val & BNX2_NVM_SW_ARB_ARB_ARB2))
			break;

		udelay(5);
	}

	if (j >= NVRAM_TIMEOUT_COUNT)
		return -EBUSY;

	return 0;
}


static int
bnx2_enable_nvram_write(struct bnx2 *bp)
{
	u32 val;

	val = BNX2_RD(bp, BNX2_MISC_CFG);
	BNX2_WR(bp, BNX2_MISC_CFG, val | BNX2_MISC_CFG_NVM_WR_EN_PCI);

	if (bp->flash_info->flags & BNX2_NV_WREN) {
		int j;

		BNX2_WR(bp, BNX2_NVM_COMMAND, BNX2_NVM_COMMAND_DONE);
		BNX2_WR(bp, BNX2_NVM_COMMAND,
			BNX2_NVM_COMMAND_WREN | BNX2_NVM_COMMAND_DOIT);

		for (j = 0; j < NVRAM_TIMEOUT_COUNT; j++) {
			udelay(5);

			val = BNX2_RD(bp, BNX2_NVM_COMMAND);
			if (val & BNX2_NVM_COMMAND_DONE)
				break;
		}

		if (j >= NVRAM_TIMEOUT_COUNT)
			return -EBUSY;
	}
	return 0;
}

static void
bnx2_disable_nvram_write(struct bnx2 *bp)
{
	u32 val;

	val = BNX2_RD(bp, BNX2_MISC_CFG);
	BNX2_WR(bp, BNX2_MISC_CFG, val & ~BNX2_MISC_CFG_NVM_WR_EN);
}


static void
bnx2_enable_nvram_access(struct bnx2 *bp)
{
	u32 val;

	val = BNX2_RD(bp, BNX2_NVM_ACCESS_ENABLE);
	/* Enable both bits, even on read. */
	BNX2_WR(bp, BNX2_NVM_ACCESS_ENABLE,
		val | BNX2_NVM_ACCESS_ENABLE_EN | BNX2_NVM_ACCESS_ENABLE_WR_EN);
}

static void
bnx2_disable_nvram_access(struct bnx2 *bp)
{
	u32 val;

	val = BNX2_RD(bp, BNX2_NVM_ACCESS_ENABLE);
	/* Disable both bits, even after read. */
	BNX2_WR(bp, BNX2_NVM_ACCESS_ENABLE,
		val & ~(BNX2_NVM_ACCESS_ENABLE_EN |
			BNX2_NVM_ACCESS_ENABLE_WR_EN));
}

static int
bnx2_nvram_erase_page(struct bnx2 *bp, u32 offset)
{
	u32 cmd;
	int j;

	if (bp->flash_info->flags & BNX2_NV_BUFFERED)
		/* Buffered flash, no erase needed */
		return 0;

	/* Build an erase command */
	cmd = BNX2_NVM_COMMAND_ERASE | BNX2_NVM_COMMAND_WR |
	      BNX2_NVM_COMMAND_DOIT;

	/* Need to clear DONE bit separately. */
	BNX2_WR(bp, BNX2_NVM_COMMAND, BNX2_NVM_COMMAND_DONE);

	/* Address of the NVRAM to read from. */
	BNX2_WR(bp, BNX2_NVM_ADDR, offset & BNX2_NVM_ADDR_NVM_ADDR_VALUE);

	/* Issue an erase command. */
	BNX2_WR(bp, BNX2_NVM_COMMAND, cmd);

	/* Wait for completion. */
	for (j = 0; j < NVRAM_TIMEOUT_COUNT; j++) {
		u32 val;

		udelay(5);

		val = BNX2_RD(bp, BNX2_NVM_COMMAND);
		if (val & BNX2_NVM_COMMAND_DONE)
			break;
	}

	if (j >= NVRAM_TIMEOUT_COUNT)
		return -EBUSY;

	return 0;
}

static int
bnx2_nvram_read_dword(struct bnx2 *bp, u32 offset, u8 *ret_val, u32 cmd_flags)
{
	u32 cmd;
	int j;

	/* Build the command word. */
	cmd = BNX2_NVM_COMMAND_DOIT | cmd_flags;

	/* Calculate an offset of a buffered flash, not needed for 5709. */
	if (bp->flash_info->flags & BNX2_NV_TRANSLATE) {
		offset = ((offset / bp->flash_info->page_size) <<
			   bp->flash_info->page_bits) +
			  (offset % bp->flash_info->page_size);
	}

	/* Need to clear DONE bit separately. */
	BNX2_WR(bp, BNX2_NVM_COMMAND, BNX2_NVM_COMMAND_DONE);

	/* Address of the NVRAM to read from. */
	BNX2_WR(bp, BNX2_NVM_ADDR, offset & BNX2_NVM_ADDR_NVM_ADDR_VALUE);

	/* Issue a read command. */
	BNX2_WR(bp, BNX2_NVM_COMMAND, cmd);

	/* Wait for completion. */
	for (j = 0; j < NVRAM_TIMEOUT_COUNT; j++) {
		u32 val;

		udelay(5);

		val = BNX2_RD(bp, BNX2_NVM_COMMAND);
		if (val & BNX2_NVM_COMMAND_DONE) {
			__be32 v = cpu_to_be32(BNX2_RD(bp, BNX2_NVM_READ));
			memcpy(ret_val, &v, 4);
			break;
		}
	}
	if (j >= NVRAM_TIMEOUT_COUNT)
		return -EBUSY;

	return 0;
}


static int
bnx2_nvram_write_dword(struct bnx2 *bp, u32 offset, u8 *val, u32 cmd_flags)
{
	u32 cmd;
	__be32 val32;
	int j;

	/* Build the command word. */
	cmd = BNX2_NVM_COMMAND_DOIT | BNX2_NVM_COMMAND_WR | cmd_flags;

	/* Calculate an offset of a buffered flash, not needed for 5709. */
	if (bp->flash_info->flags & BNX2_NV_TRANSLATE) {
		offset = ((offset / bp->flash_info->page_size) <<
			  bp->flash_info->page_bits) +
			 (offset % bp->flash_info->page_size);
	}

	/* Need to clear DONE bit separately. */
	BNX2_WR(bp, BNX2_NVM_COMMAND, BNX2_NVM_COMMAND_DONE);

	memcpy(&val32, val, 4);

	/* Write the data. */
	BNX2_WR(bp, BNX2_NVM_WRITE, be32_to_cpu(val32));

	/* Address of the NVRAM to write to. */
	BNX2_WR(bp, BNX2_NVM_ADDR, offset & BNX2_NVM_ADDR_NVM_ADDR_VALUE);

	/* Issue the write command. */
	BNX2_WR(bp, BNX2_NVM_COMMAND, cmd);

	/* Wait for completion. */
	for (j = 0; j < NVRAM_TIMEOUT_COUNT; j++) {
		udelay(5);

		if (BNX2_RD(bp, BNX2_NVM_COMMAND) & BNX2_NVM_COMMAND_DONE)
			break;
	}
	if (j >= NVRAM_TIMEOUT_COUNT)
		return -EBUSY;

	return 0;
}

static int
bnx2_init_nvram(struct bnx2 *bp)
{
	u32 val;
	int j, entry_count, rc = 0;
	const struct flash_spec *flash;

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		bp->flash_info = &flash_5709;
		goto get_flash_size;
	}

	/* Determine the selected interface. */
	val = BNX2_RD(bp, BNX2_NVM_CFG1);

	entry_count = ARRAY_SIZE(flash_table);

	if (val & 0x40000000) {

		/* Flash interface has been reconfigured */
		for (j = 0, flash = &flash_table[0]; j < entry_count;
		     j++, flash++) {
			if ((val & FLASH_BACKUP_STRAP_MASK) ==
			    (flash->config1 & FLASH_BACKUP_STRAP_MASK)) {
				bp->flash_info = flash;
				break;
			}
		}
	}
	else {
		u32 mask;
		/* Not yet been reconfigured */

		if (val & (1 << 23))
			mask = FLASH_BACKUP_STRAP_MASK;
		else
			mask = FLASH_STRAP_MASK;

		for (j = 0, flash = &flash_table[0]; j < entry_count;
			j++, flash++) {

			if ((val & mask) == (flash->strapping & mask)) {
				bp->flash_info = flash;

				/* Request access to the flash interface. */
				if ((rc = bnx2_acquire_nvram_lock(bp)) != 0)
					return rc;

				/* Enable access to flash interface */
				bnx2_enable_nvram_access(bp);

				/* Reconfigure the flash interface */
				BNX2_WR(bp, BNX2_NVM_CFG1, flash->config1);
				BNX2_WR(bp, BNX2_NVM_CFG2, flash->config2);
				BNX2_WR(bp, BNX2_NVM_CFG3, flash->config3);
				BNX2_WR(bp, BNX2_NVM_WRITE1, flash->write1);

				/* Disable access to flash interface */
				bnx2_disable_nvram_access(bp);
				bnx2_release_nvram_lock(bp);

				break;
			}
		}
	} /* if (val & 0x40000000) */

	if (j == entry_count) {
		bp->flash_info = NULL;
		pr_alert("Unknown flash/EEPROM type\n");
		return -ENODEV;
	}

get_flash_size:
	val = bnx2_shmem_rd(bp, BNX2_SHARED_HW_CFG_CONFIG2);
	val &= BNX2_SHARED_HW_CFG2_NVM_SIZE_MASK;
	if (val)
		bp->flash_size = val;
	else
		bp->flash_size = bp->flash_info->total_size;

	return rc;
}

static int
bnx2_nvram_read(struct bnx2 *bp, u32 offset, u8 *ret_buf,
		int buf_size)
{
	int rc = 0;
	u32 cmd_flags, offset32, len32, extra;

	if (buf_size == 0)
		return 0;

	/* Request access to the flash interface. */
	if ((rc = bnx2_acquire_nvram_lock(bp)) != 0)
		return rc;

	/* Enable access to flash interface */
	bnx2_enable_nvram_access(bp);

	len32 = buf_size;
	offset32 = offset;
	extra = 0;

	cmd_flags = 0;

	if (offset32 & 3) {
		u8 buf[4];
		u32 pre_len;

		offset32 &= ~3;
		pre_len = 4 - (offset & 3);

		if (pre_len >= len32) {
			pre_len = len32;
			cmd_flags = BNX2_NVM_COMMAND_FIRST |
				    BNX2_NVM_COMMAND_LAST;
		}
		else {
			cmd_flags = BNX2_NVM_COMMAND_FIRST;
		}

		rc = bnx2_nvram_read_dword(bp, offset32, buf, cmd_flags);

		if (rc)
			return rc;

		memcpy(ret_buf, buf + (offset & 3), pre_len);

		offset32 += 4;
		ret_buf += pre_len;
		len32 -= pre_len;
	}
	if (len32 & 3) {
		extra = 4 - (len32 & 3);
		len32 = (len32 + 4) & ~3;
	}

	if (len32 == 4) {
		u8 buf[4];

		if (cmd_flags)
			cmd_flags = BNX2_NVM_COMMAND_LAST;
		else
			cmd_flags = BNX2_NVM_COMMAND_FIRST |
				    BNX2_NVM_COMMAND_LAST;

		rc = bnx2_nvram_read_dword(bp, offset32, buf, cmd_flags);

		memcpy(ret_buf, buf, 4 - extra);
	}
	else if (len32 > 0) {
		u8 buf[4];

		/* Read the first word. */
		if (cmd_flags)
			cmd_flags = 0;
		else
			cmd_flags = BNX2_NVM_COMMAND_FIRST;

		rc = bnx2_nvram_read_dword(bp, offset32, ret_buf, cmd_flags);

		/* Advance to the next dword. */
		offset32 += 4;
		ret_buf += 4;
		len32 -= 4;

		while (len32 > 4 && rc == 0) {
			rc = bnx2_nvram_read_dword(bp, offset32, ret_buf, 0);

			/* Advance to the next dword. */
			offset32 += 4;
			ret_buf += 4;
			len32 -= 4;
		}

		if (rc)
			return rc;

		cmd_flags = BNX2_NVM_COMMAND_LAST;
		rc = bnx2_nvram_read_dword(bp, offset32, buf, cmd_flags);

		memcpy(ret_buf, buf, 4 - extra);
	}

	/* Disable access to flash interface */
	bnx2_disable_nvram_access(bp);

	bnx2_release_nvram_lock(bp);

	return rc;
}

static int
bnx2_nvram_write(struct bnx2 *bp, u32 offset, u8 *data_buf,
		int buf_size)
{
	u32 written, offset32, len32;
	u8 *buf, start[4], end[4], *align_buf = NULL, *flash_buffer = NULL;
	int rc = 0;
	int align_start, align_end;

	buf = data_buf;
	offset32 = offset;
	len32 = buf_size;
	align_start = align_end = 0;

	if ((align_start = (offset32 & 3))) {
		offset32 &= ~3;
		len32 += align_start;
		if (len32 < 4)
			len32 = 4;
		if ((rc = bnx2_nvram_read(bp, offset32, start, 4)))
			return rc;
	}

	if (len32 & 3) {
		align_end = 4 - (len32 & 3);
		len32 += align_end;
		if ((rc = bnx2_nvram_read(bp, offset32 + len32 - 4, end, 4)))
			return rc;
	}

	if (align_start || align_end) {
		align_buf = kmalloc(len32, GFP_KERNEL);
		if (!align_buf)
			return -ENOMEM;
		if (align_start) {
			memcpy(align_buf, start, 4);
		}
		if (align_end) {
			memcpy(align_buf + len32 - 4, end, 4);
		}
		memcpy(align_buf + align_start, data_buf, buf_size);
		buf = align_buf;
	}

	if (!(bp->flash_info->flags & BNX2_NV_BUFFERED)) {
		flash_buffer = kmalloc(264, GFP_KERNEL);
		if (!flash_buffer) {
			rc = -ENOMEM;
			goto nvram_write_end;
		}
	}

	written = 0;
	while ((written < len32) && (rc == 0)) {
		u32 page_start, page_end, data_start, data_end;
		u32 addr, cmd_flags;
		int i;

	        /* Find the page_start addr */
		page_start = offset32 + written;
		page_start -= (page_start % bp->flash_info->page_size);
		/* Find the page_end addr */
		page_end = page_start + bp->flash_info->page_size;
		/* Find the data_start addr */
		data_start = (written == 0) ? offset32 : page_start;
		/* Find the data_end addr */
		data_end = (page_end > offset32 + len32) ?
			(offset32 + len32) : page_end;

		/* Request access to the flash interface. */
		if ((rc = bnx2_acquire_nvram_lock(bp)) != 0)
			goto nvram_write_end;

		/* Enable access to flash interface */
		bnx2_enable_nvram_access(bp);

		cmd_flags = BNX2_NVM_COMMAND_FIRST;
		if (!(bp->flash_info->flags & BNX2_NV_BUFFERED)) {
			int j;

			/* Read the whole page into the buffer
			 * (non-buffer flash only) */
			for (j = 0; j < bp->flash_info->page_size; j += 4) {
				if (j == (bp->flash_info->page_size - 4)) {
					cmd_flags |= BNX2_NVM_COMMAND_LAST;
				}
				rc = bnx2_nvram_read_dword(bp,
					page_start + j,
					&flash_buffer[j],
					cmd_flags);

				if (rc)
					goto nvram_write_end;

				cmd_flags = 0;
			}
		}

		/* Enable writes to flash interface (unlock write-protect) */
		if ((rc = bnx2_enable_nvram_write(bp)) != 0)
			goto nvram_write_end;

		/* Loop to write back the buffer data from page_start to
		 * data_start */
		i = 0;
		if (!(bp->flash_info->flags & BNX2_NV_BUFFERED)) {
			/* Erase the page */
			if ((rc = bnx2_nvram_erase_page(bp, page_start)) != 0)
				goto nvram_write_end;

			/* Re-enable the write again for the actual write */
			bnx2_enable_nvram_write(bp);

			for (addr = page_start; addr < data_start;
				addr += 4, i += 4) {

				rc = bnx2_nvram_write_dword(bp, addr,
					&flash_buffer[i], cmd_flags);

				if (rc != 0)
					goto nvram_write_end;

				cmd_flags = 0;
			}
		}

		/* Loop to write the new data from data_start to data_end */
		for (addr = data_start; addr < data_end; addr += 4, i += 4) {
			if ((addr == page_end - 4) ||
				((bp->flash_info->flags & BNX2_NV_BUFFERED) &&
				 (addr == data_end - 4))) {

				cmd_flags |= BNX2_NVM_COMMAND_LAST;
			}
			rc = bnx2_nvram_write_dword(bp, addr, buf,
				cmd_flags);

			if (rc != 0)
				goto nvram_write_end;

			cmd_flags = 0;
			buf += 4;
		}

		/* Loop to write back the buffer data from data_end
		 * to page_end */
		if (!(bp->flash_info->flags & BNX2_NV_BUFFERED)) {
			for (addr = data_end; addr < page_end;
				addr += 4, i += 4) {

				if (addr == page_end-4) {
					cmd_flags = BNX2_NVM_COMMAND_LAST;
                		}
				rc = bnx2_nvram_write_dword(bp, addr,
					&flash_buffer[i], cmd_flags);

				if (rc != 0)
					goto nvram_write_end;

				cmd_flags = 0;
			}
		}

		/* Disable writes to flash interface (lock write-protect) */
		bnx2_disable_nvram_write(bp);

		/* Disable access to flash interface */
		bnx2_disable_nvram_access(bp);
		bnx2_release_nvram_lock(bp);

		/* Increment written */
		written += data_end - data_start;
	}

nvram_write_end:
	kfree(flash_buffer);
	kfree(align_buf);
	return rc;
}

static void
bnx2_init_fw_cap(struct bnx2 *bp)
{
	u32 val, sig = 0;

	bp->phy_flags &= ~BNX2_PHY_FLAG_REMOTE_PHY_CAP;
	bp->flags &= ~BNX2_FLAG_CAN_KEEP_VLAN;

	if (!(bp->flags & BNX2_FLAG_ASF_ENABLE))
		bp->flags |= BNX2_FLAG_CAN_KEEP_VLAN;

	val = bnx2_shmem_rd(bp, BNX2_FW_CAP_MB);
	if ((val & BNX2_FW_CAP_SIGNATURE_MASK) != BNX2_FW_CAP_SIGNATURE)
		return;

	if ((val & BNX2_FW_CAP_CAN_KEEP_VLAN) == BNX2_FW_CAP_CAN_KEEP_VLAN) {
		bp->flags |= BNX2_FLAG_CAN_KEEP_VLAN;
		sig |= BNX2_DRV_ACK_CAP_SIGNATURE | BNX2_FW_CAP_CAN_KEEP_VLAN;
	}

	if ((bp->phy_flags & BNX2_PHY_FLAG_SERDES) &&
	    (val & BNX2_FW_CAP_REMOTE_PHY_CAPABLE)) {
		u32 link;

		bp->phy_flags |= BNX2_PHY_FLAG_REMOTE_PHY_CAP;

		link = bnx2_shmem_rd(bp, BNX2_LINK_STATUS);
		if (link & BNX2_LINK_STATUS_SERDES_LINK)
			bp->phy_port = PORT_FIBRE;
		else
			bp->phy_port = PORT_TP;

		sig |= BNX2_DRV_ACK_CAP_SIGNATURE |
		       BNX2_FW_CAP_REMOTE_PHY_CAPABLE;
	}

	if (netif_running(bp->dev) && sig)
		bnx2_shmem_wr(bp, BNX2_DRV_ACK_CAP_MB, sig);
}

static void
bnx2_setup_msix_tbl(struct bnx2 *bp)
{
	BNX2_WR(bp, BNX2_PCI_GRC_WINDOW_ADDR, BNX2_PCI_GRC_WINDOW_ADDR_SEP_WIN);

	BNX2_WR(bp, BNX2_PCI_GRC_WINDOW2_ADDR, BNX2_MSIX_TABLE_ADDR);
	BNX2_WR(bp, BNX2_PCI_GRC_WINDOW3_ADDR, BNX2_MSIX_PBA_ADDR);
}

static void
bnx2_wait_dma_complete(struct bnx2 *bp)
{
	u32 val;
	int i;

	/*
	 * Wait for the current PCI transaction to complete before
	 * issuing a reset.
	 */
	if ((BNX2_CHIP(bp) == BNX2_CHIP_5706) ||
	    (BNX2_CHIP(bp) == BNX2_CHIP_5708)) {
		BNX2_WR(bp, BNX2_MISC_ENABLE_CLR_BITS,
			BNX2_MISC_ENABLE_CLR_BITS_TX_DMA_ENABLE |
			BNX2_MISC_ENABLE_CLR_BITS_DMA_ENGINE_ENABLE |
			BNX2_MISC_ENABLE_CLR_BITS_RX_DMA_ENABLE |
			BNX2_MISC_ENABLE_CLR_BITS_HOST_COALESCE_ENABLE);
		val = BNX2_RD(bp, BNX2_MISC_ENABLE_CLR_BITS);
		udelay(5);
	} else {  /* 5709 */
		val = BNX2_RD(bp, BNX2_MISC_NEW_CORE_CTL);
		val &= ~BNX2_MISC_NEW_CORE_CTL_DMA_ENABLE;
		BNX2_WR(bp, BNX2_MISC_NEW_CORE_CTL, val);
		val = BNX2_RD(bp, BNX2_MISC_NEW_CORE_CTL);

		for (i = 0; i < 100; i++) {
			msleep(1);
			val = BNX2_RD(bp, BNX2_PCICFG_DEVICE_CONTROL);
			if (!(val & BNX2_PCICFG_DEVICE_STATUS_NO_PEND))
				break;
		}
	}

	return;
}


static int
bnx2_reset_chip(struct bnx2 *bp, u32 reset_code)
{
	u32 val;
	int i, rc = 0;
	u8 old_port;

	/* Wait for the current PCI transaction to complete before
	 * issuing a reset. */
	bnx2_wait_dma_complete(bp);

	/* Wait for the firmware to tell us it is ok to issue a reset. */
	bnx2_fw_sync(bp, BNX2_DRV_MSG_DATA_WAIT0 | reset_code, 1, 1);

	/* Deposit a driver reset signature so the firmware knows that
	 * this is a soft reset. */
	bnx2_shmem_wr(bp, BNX2_DRV_RESET_SIGNATURE,
		      BNX2_DRV_RESET_SIGNATURE_MAGIC);

	/* Do a dummy read to force the chip to complete all current transaction
	 * before we issue a reset. */
	val = BNX2_RD(bp, BNX2_MISC_ID);

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		BNX2_WR(bp, BNX2_MISC_COMMAND, BNX2_MISC_COMMAND_SW_RESET);
		BNX2_RD(bp, BNX2_MISC_COMMAND);
		udelay(5);

		val = BNX2_PCICFG_MISC_CONFIG_REG_WINDOW_ENA |
		      BNX2_PCICFG_MISC_CONFIG_TARGET_MB_WORD_SWAP;

		BNX2_WR(bp, BNX2_PCICFG_MISC_CONFIG, val);

	} else {
		val = BNX2_PCICFG_MISC_CONFIG_CORE_RST_REQ |
		      BNX2_PCICFG_MISC_CONFIG_REG_WINDOW_ENA |
		      BNX2_PCICFG_MISC_CONFIG_TARGET_MB_WORD_SWAP;

		/* Chip reset. */
		BNX2_WR(bp, BNX2_PCICFG_MISC_CONFIG, val);

		/* Reading back any register after chip reset will hang the
		 * bus on 5706 A0 and A1.  The msleep below provides plenty
		 * of margin for write posting.
		 */
		if ((BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A0) ||
		    (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A1))
			msleep(20);

		/* Reset takes approximate 30 usec */
		for (i = 0; i < 10; i++) {
			val = BNX2_RD(bp, BNX2_PCICFG_MISC_CONFIG);
			if ((val & (BNX2_PCICFG_MISC_CONFIG_CORE_RST_REQ |
				    BNX2_PCICFG_MISC_CONFIG_CORE_RST_BSY)) == 0)
				break;
			udelay(10);
		}

		if (val & (BNX2_PCICFG_MISC_CONFIG_CORE_RST_REQ |
			   BNX2_PCICFG_MISC_CONFIG_CORE_RST_BSY)) {
			pr_err("Chip reset did not complete\n");
			return -EBUSY;
		}
	}

	/* Make sure byte swapping is properly configured. */
	val = BNX2_RD(bp, BNX2_PCI_SWAP_DIAG0);
	if (val != 0x01020304) {
		pr_err("Chip not in correct endian mode\n");
		return -ENODEV;
	}

	/* Wait for the firmware to finish its initialization. */
	rc = bnx2_fw_sync(bp, BNX2_DRV_MSG_DATA_WAIT1 | reset_code, 1, 0);
	if (rc)
		return rc;

	spin_lock_bh(&bp->phy_lock);
	old_port = bp->phy_port;
	bnx2_init_fw_cap(bp);
	if ((bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP) &&
	    old_port != bp->phy_port)
		bnx2_set_default_remote_link(bp);
	spin_unlock_bh(&bp->phy_lock);

	if (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A0) {
		/* Adjust the voltage regular to two steps lower.  The default
		 * of this register is 0x0000000e. */
		BNX2_WR(bp, BNX2_MISC_VREG_CONTROL, 0x000000fa);

		/* Remove bad rbuf memory from the free pool. */
		rc = bnx2_alloc_bad_rbuf(bp);
	}

	if (bp->flags & BNX2_FLAG_USING_MSIX) {
		bnx2_setup_msix_tbl(bp);
		/* Prevent MSIX table reads and write from timing out */
		BNX2_WR(bp, BNX2_MISC_ECO_HW_CTL,
			BNX2_MISC_ECO_HW_CTL_LARGE_GRC_TMOUT_EN);
	}

	return rc;
}

static int
bnx2_init_chip(struct bnx2 *bp)
{
	u32 val, mtu;
	int rc, i;

	/* Make sure the interrupt is not active. */
	BNX2_WR(bp, BNX2_PCICFG_INT_ACK_CMD, BNX2_PCICFG_INT_ACK_CMD_MASK_INT);

	val = BNX2_DMA_CONFIG_DATA_BYTE_SWAP |
	      BNX2_DMA_CONFIG_DATA_WORD_SWAP |
#ifdef __BIG_ENDIAN
	      BNX2_DMA_CONFIG_CNTL_BYTE_SWAP |
#endif
	      BNX2_DMA_CONFIG_CNTL_WORD_SWAP |
	      DMA_READ_CHANS << 12 |
	      DMA_WRITE_CHANS << 16;

	val |= (0x2 << 20) | (1 << 11);

	if ((bp->flags & BNX2_FLAG_PCIX) && (bp->bus_speed_mhz == 133))
		val |= (1 << 23);

	if ((BNX2_CHIP(bp) == BNX2_CHIP_5706) &&
	    (BNX2_CHIP_ID(bp) != BNX2_CHIP_ID_5706_A0) &&
	    !(bp->flags & BNX2_FLAG_PCIX))
		val |= BNX2_DMA_CONFIG_CNTL_PING_PONG_DMA;

	BNX2_WR(bp, BNX2_DMA_CONFIG, val);

	if (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A0) {
		val = BNX2_RD(bp, BNX2_TDMA_CONFIG);
		val |= BNX2_TDMA_CONFIG_ONE_DMA;
		BNX2_WR(bp, BNX2_TDMA_CONFIG, val);
	}

	if (bp->flags & BNX2_FLAG_PCIX) {
		u16 val16;

		pci_read_config_word(bp->pdev, bp->pcix_cap + PCI_X_CMD,
				     &val16);
		pci_write_config_word(bp->pdev, bp->pcix_cap + PCI_X_CMD,
				      val16 & ~PCI_X_CMD_ERO);
	}

	BNX2_WR(bp, BNX2_MISC_ENABLE_SET_BITS,
		BNX2_MISC_ENABLE_SET_BITS_HOST_COALESCE_ENABLE |
		BNX2_MISC_ENABLE_STATUS_BITS_RX_V2P_ENABLE |
		BNX2_MISC_ENABLE_STATUS_BITS_CONTEXT_ENABLE);

	/* Initialize context mapping and zero out the quick contexts.  The
	 * context block must have already been enabled. */
	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		rc = bnx2_init_5709_context(bp);
		if (rc)
			return rc;
	} else
		bnx2_init_context(bp);

	if ((rc = bnx2_init_cpus(bp)) != 0)
		return rc;

	bnx2_init_nvram(bp);

	bnx2_set_mac_addr(bp, bp->dev->dev_addr, 0);

	val = BNX2_RD(bp, BNX2_MQ_CONFIG);
	val &= ~BNX2_MQ_CONFIG_KNL_BYP_BLK_SIZE;
	val |= BNX2_MQ_CONFIG_KNL_BYP_BLK_SIZE_256;
	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		val |= BNX2_MQ_CONFIG_BIN_MQ_MODE;
		if (BNX2_CHIP_REV(bp) == BNX2_CHIP_REV_Ax)
			val |= BNX2_MQ_CONFIG_HALT_DIS;
	}

	BNX2_WR(bp, BNX2_MQ_CONFIG, val);

	val = 0x10000 + (MAX_CID_CNT * MB_KERNEL_CTX_SIZE);
	BNX2_WR(bp, BNX2_MQ_KNL_BYP_WIND_START, val);
	BNX2_WR(bp, BNX2_MQ_KNL_WIND_END, val);

	val = (BNX2_PAGE_BITS - 8) << 24;
	BNX2_WR(bp, BNX2_RV2P_CONFIG, val);

	/* Configure page size. */
	val = BNX2_RD(bp, BNX2_TBDR_CONFIG);
	val &= ~BNX2_TBDR_CONFIG_PAGE_SIZE;
	val |= (BNX2_PAGE_BITS - 8) << 24 | 0x40;
	BNX2_WR(bp, BNX2_TBDR_CONFIG, val);

	val = bp->mac_addr[0] +
	      (bp->mac_addr[1] << 8) +
	      (bp->mac_addr[2] << 16) +
	      bp->mac_addr[3] +
	      (bp->mac_addr[4] << 8) +
	      (bp->mac_addr[5] << 16);
	BNX2_WR(bp, BNX2_EMAC_BACKOFF_SEED, val);

	/* Program the MTU.  Also include 4 bytes for CRC32. */
	mtu = bp->dev->mtu;
	val = mtu + ETH_HLEN + ETH_FCS_LEN;
	if (val > (MAX_ETHERNET_PACKET_SIZE + ETH_HLEN + 4))
		val |= BNX2_EMAC_RX_MTU_SIZE_JUMBO_ENA;
	BNX2_WR(bp, BNX2_EMAC_RX_MTU_SIZE, val);

	if (mtu < ETH_DATA_LEN)
		mtu = ETH_DATA_LEN;

	bnx2_reg_wr_ind(bp, BNX2_RBUF_CONFIG, BNX2_RBUF_CONFIG_VAL(mtu));
	bnx2_reg_wr_ind(bp, BNX2_RBUF_CONFIG2, BNX2_RBUF_CONFIG2_VAL(mtu));
	bnx2_reg_wr_ind(bp, BNX2_RBUF_CONFIG3, BNX2_RBUF_CONFIG3_VAL(mtu));

	memset(bp->bnx2_napi[0].status_blk.msi, 0, bp->status_stats_size);
	for (i = 0; i < BNX2_MAX_MSIX_VEC; i++)
		bp->bnx2_napi[i].last_status_idx = 0;

	bp->idle_chk_status_idx = 0xffff;

	/* Set up how to generate a link change interrupt. */
	BNX2_WR(bp, BNX2_EMAC_ATTENTION_ENA, BNX2_EMAC_ATTENTION_ENA_LINK);

	BNX2_WR(bp, BNX2_HC_STATUS_ADDR_L,
		(u64) bp->status_blk_mapping & 0xffffffff);
	BNX2_WR(bp, BNX2_HC_STATUS_ADDR_H, (u64) bp->status_blk_mapping >> 32);

	BNX2_WR(bp, BNX2_HC_STATISTICS_ADDR_L,
		(u64) bp->stats_blk_mapping & 0xffffffff);
	BNX2_WR(bp, BNX2_HC_STATISTICS_ADDR_H,
		(u64) bp->stats_blk_mapping >> 32);

	BNX2_WR(bp, BNX2_HC_TX_QUICK_CONS_TRIP,
		(bp->tx_quick_cons_trip_int << 16) | bp->tx_quick_cons_trip);

	BNX2_WR(bp, BNX2_HC_RX_QUICK_CONS_TRIP,
		(bp->rx_quick_cons_trip_int << 16) | bp->rx_quick_cons_trip);

	BNX2_WR(bp, BNX2_HC_COMP_PROD_TRIP,
		(bp->comp_prod_trip_int << 16) | bp->comp_prod_trip);

	BNX2_WR(bp, BNX2_HC_TX_TICKS, (bp->tx_ticks_int << 16) | bp->tx_ticks);

	BNX2_WR(bp, BNX2_HC_RX_TICKS, (bp->rx_ticks_int << 16) | bp->rx_ticks);

	BNX2_WR(bp, BNX2_HC_COM_TICKS,
		(bp->com_ticks_int << 16) | bp->com_ticks);

	BNX2_WR(bp, BNX2_HC_CMD_TICKS,
		(bp->cmd_ticks_int << 16) | bp->cmd_ticks);

	if (bp->flags & BNX2_FLAG_BROKEN_STATS)
		BNX2_WR(bp, BNX2_HC_STATS_TICKS, 0);
	else
		BNX2_WR(bp, BNX2_HC_STATS_TICKS, bp->stats_ticks);
	BNX2_WR(bp, BNX2_HC_STAT_COLLECT_TICKS, 0xbb8);  /* 3ms */

	if (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A1)
		val = BNX2_HC_CONFIG_COLLECT_STATS;
	else {
		val = BNX2_HC_CONFIG_RX_TMR_MODE | BNX2_HC_CONFIG_TX_TMR_MODE |
		      BNX2_HC_CONFIG_COLLECT_STATS;
	}

	if (bp->flags & BNX2_FLAG_USING_MSIX) {
		BNX2_WR(bp, BNX2_HC_MSIX_BIT_VECTOR,
			BNX2_HC_MSIX_BIT_VECTOR_VAL);

		val |= BNX2_HC_CONFIG_SB_ADDR_INC_128B;
	}

	if (bp->flags & BNX2_FLAG_ONE_SHOT_MSI)
		val |= BNX2_HC_CONFIG_ONE_SHOT | BNX2_HC_CONFIG_USE_INT_PARAM;

	BNX2_WR(bp, BNX2_HC_CONFIG, val);

	if (bp->rx_ticks < 25)
		bnx2_reg_wr_ind(bp, BNX2_FW_RX_LOW_LATENCY, 1);
	else
		bnx2_reg_wr_ind(bp, BNX2_FW_RX_LOW_LATENCY, 0);

	for (i = 1; i < bp->irq_nvecs; i++) {
		u32 base = ((i - 1) * BNX2_HC_SB_CONFIG_SIZE) +
			   BNX2_HC_SB_CONFIG_1;

		BNX2_WR(bp, base,
			BNX2_HC_SB_CONFIG_1_TX_TMR_MODE |
			BNX2_HC_SB_CONFIG_1_RX_TMR_MODE |
			BNX2_HC_SB_CONFIG_1_ONE_SHOT);

		BNX2_WR(bp, base + BNX2_HC_TX_QUICK_CONS_TRIP_OFF,
			(bp->tx_quick_cons_trip_int << 16) |
			 bp->tx_quick_cons_trip);

		BNX2_WR(bp, base + BNX2_HC_TX_TICKS_OFF,
			(bp->tx_ticks_int << 16) | bp->tx_ticks);

		BNX2_WR(bp, base + BNX2_HC_RX_QUICK_CONS_TRIP_OFF,
			(bp->rx_quick_cons_trip_int << 16) |
			bp->rx_quick_cons_trip);

		BNX2_WR(bp, base + BNX2_HC_RX_TICKS_OFF,
			(bp->rx_ticks_int << 16) | bp->rx_ticks);
	}

	/* Clear internal stats counters. */
	BNX2_WR(bp, BNX2_HC_COMMAND, BNX2_HC_COMMAND_CLR_STAT_NOW);

	BNX2_WR(bp, BNX2_HC_ATTN_BITS_ENABLE, STATUS_ATTN_EVENTS);

	/* Initialize the receive filter. */
	bnx2_set_rx_mode(bp->dev);

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		val = BNX2_RD(bp, BNX2_MISC_NEW_CORE_CTL);
		val |= BNX2_MISC_NEW_CORE_CTL_DMA_ENABLE;
		BNX2_WR(bp, BNX2_MISC_NEW_CORE_CTL, val);
	}
	rc = bnx2_fw_sync(bp, BNX2_DRV_MSG_DATA_WAIT2 | BNX2_DRV_MSG_CODE_RESET,
			  1, 0);

	BNX2_WR(bp, BNX2_MISC_ENABLE_SET_BITS, BNX2_MISC_ENABLE_DEFAULT);
	BNX2_RD(bp, BNX2_MISC_ENABLE_SET_BITS);

	udelay(20);

	bp->hc_cmd = BNX2_RD(bp, BNX2_HC_COMMAND);

	return rc;
}

static void
bnx2_clear_ring_states(struct bnx2 *bp)
{
	struct bnx2_napi *bnapi;
	struct bnx2_tx_ring_info *txr;
	struct bnx2_rx_ring_info *rxr;
	int i;

	for (i = 0; i < BNX2_MAX_MSIX_VEC; i++) {
		bnapi = &bp->bnx2_napi[i];
		txr = &bnapi->tx_ring;
		rxr = &bnapi->rx_ring;

		txr->tx_cons = 0;
		txr->hw_tx_cons = 0;
		rxr->rx_prod_bseq = 0;
		rxr->rx_prod = 0;
		rxr->rx_cons = 0;
		rxr->rx_pg_prod = 0;
		rxr->rx_pg_cons = 0;
	}
}

static void
bnx2_init_tx_context(struct bnx2 *bp, u32 cid, struct bnx2_tx_ring_info *txr)
{
	u32 val, offset0, offset1, offset2, offset3;
	u32 cid_addr = GET_CID_ADDR(cid);

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		offset0 = BNX2_L2CTX_TYPE_XI;
		offset1 = BNX2_L2CTX_CMD_TYPE_XI;
		offset2 = BNX2_L2CTX_TBDR_BHADDR_HI_XI;
		offset3 = BNX2_L2CTX_TBDR_BHADDR_LO_XI;
	} else {
		offset0 = BNX2_L2CTX_TYPE;
		offset1 = BNX2_L2CTX_CMD_TYPE;
		offset2 = BNX2_L2CTX_TBDR_BHADDR_HI;
		offset3 = BNX2_L2CTX_TBDR_BHADDR_LO;
	}
	val = BNX2_L2CTX_TYPE_TYPE_L2 | BNX2_L2CTX_TYPE_SIZE_L2;
	bnx2_ctx_wr(bp, cid_addr, offset0, val);

	val = BNX2_L2CTX_CMD_TYPE_TYPE_L2 | (8 << 16);
	bnx2_ctx_wr(bp, cid_addr, offset1, val);

	val = (u64) txr->tx_desc_mapping >> 32;
	bnx2_ctx_wr(bp, cid_addr, offset2, val);

	val = (u64) txr->tx_desc_mapping & 0xffffffff;
	bnx2_ctx_wr(bp, cid_addr, offset3, val);
}

static void
bnx2_init_tx_ring(struct bnx2 *bp, int ring_num)
{
	struct bnx2_tx_bd *txbd;
	u32 cid = TX_CID;
	struct bnx2_napi *bnapi;
	struct bnx2_tx_ring_info *txr;

	bnapi = &bp->bnx2_napi[ring_num];
	txr = &bnapi->tx_ring;

	if (ring_num == 0)
		cid = TX_CID;
	else
		cid = TX_TSS_CID + ring_num - 1;

	bp->tx_wake_thresh = bp->tx_ring_size / 2;

	txbd = &txr->tx_desc_ring[BNX2_MAX_TX_DESC_CNT];

	txbd->tx_bd_haddr_hi = (u64) txr->tx_desc_mapping >> 32;
	txbd->tx_bd_haddr_lo = (u64) txr->tx_desc_mapping & 0xffffffff;

	txr->tx_prod = 0;
	txr->tx_prod_bseq = 0;

	txr->tx_bidx_addr = MB_GET_CID_ADDR(cid) + BNX2_L2CTX_TX_HOST_BIDX;
	txr->tx_bseq_addr = MB_GET_CID_ADDR(cid) + BNX2_L2CTX_TX_HOST_BSEQ;

	bnx2_init_tx_context(bp, cid, txr);
}

static void
bnx2_init_rxbd_rings(struct bnx2_rx_bd *rx_ring[], dma_addr_t dma[],
		     u32 buf_size, int num_rings)
{
	int i;
	struct bnx2_rx_bd *rxbd;

	for (i = 0; i < num_rings; i++) {
		int j;

		rxbd = &rx_ring[i][0];
		for (j = 0; j < BNX2_MAX_RX_DESC_CNT; j++, rxbd++) {
			rxbd->rx_bd_len = buf_size;
			rxbd->rx_bd_flags = RX_BD_FLAGS_START | RX_BD_FLAGS_END;
		}
		if (i == (num_rings - 1))
			j = 0;
		else
			j = i + 1;
		rxbd->rx_bd_haddr_hi = (u64) dma[j] >> 32;
		rxbd->rx_bd_haddr_lo = (u64) dma[j] & 0xffffffff;
	}
}

static void
bnx2_init_rx_ring(struct bnx2 *bp, int ring_num)
{
	int i;
	u16 prod, ring_prod;
	u32 cid, rx_cid_addr, val;
	struct bnx2_napi *bnapi = &bp->bnx2_napi[ring_num];
	struct bnx2_rx_ring_info *rxr = &bnapi->rx_ring;

	if (ring_num == 0)
		cid = RX_CID;
	else
		cid = RX_RSS_CID + ring_num - 1;

	rx_cid_addr = GET_CID_ADDR(cid);

	bnx2_init_rxbd_rings(rxr->rx_desc_ring, rxr->rx_desc_mapping,
			     bp->rx_buf_use_size, bp->rx_max_ring);

	bnx2_init_rx_context(bp, cid);

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		val = BNX2_RD(bp, BNX2_MQ_MAP_L2_5);
		BNX2_WR(bp, BNX2_MQ_MAP_L2_5, val | BNX2_MQ_MAP_L2_5_ARM);
	}

	bnx2_ctx_wr(bp, rx_cid_addr, BNX2_L2CTX_PG_BUF_SIZE, 0);
	if (bp->rx_pg_ring_size) {
		bnx2_init_rxbd_rings(rxr->rx_pg_desc_ring,
				     rxr->rx_pg_desc_mapping,
				     PAGE_SIZE, bp->rx_max_pg_ring);
		val = (bp->rx_buf_use_size << 16) | PAGE_SIZE;
		bnx2_ctx_wr(bp, rx_cid_addr, BNX2_L2CTX_PG_BUF_SIZE, val);
		bnx2_ctx_wr(bp, rx_cid_addr, BNX2_L2CTX_RBDC_KEY,
		       BNX2_L2CTX_RBDC_JUMBO_KEY - ring_num);

		val = (u64) rxr->rx_pg_desc_mapping[0] >> 32;
		bnx2_ctx_wr(bp, rx_cid_addr, BNX2_L2CTX_NX_PG_BDHADDR_HI, val);

		val = (u64) rxr->rx_pg_desc_mapping[0] & 0xffffffff;
		bnx2_ctx_wr(bp, rx_cid_addr, BNX2_L2CTX_NX_PG_BDHADDR_LO, val);

		if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
			BNX2_WR(bp, BNX2_MQ_MAP_L2_3, BNX2_MQ_MAP_L2_3_DEFAULT);
	}

	val = (u64) rxr->rx_desc_mapping[0] >> 32;
	bnx2_ctx_wr(bp, rx_cid_addr, BNX2_L2CTX_NX_BDHADDR_HI, val);

	val = (u64) rxr->rx_desc_mapping[0] & 0xffffffff;
	bnx2_ctx_wr(bp, rx_cid_addr, BNX2_L2CTX_NX_BDHADDR_LO, val);

	ring_prod = prod = rxr->rx_pg_prod;
	for (i = 0; i < bp->rx_pg_ring_size; i++) {
		if (bnx2_alloc_rx_page(bp, rxr, ring_prod, GFP_KERNEL) < 0) {
			netdev_warn(bp->dev, "init'ed rx page ring %d with %d/%d pages only\n",
				    ring_num, i, bp->rx_pg_ring_size);
			break;
		}
		prod = BNX2_NEXT_RX_BD(prod);
		ring_prod = BNX2_RX_PG_RING_IDX(prod);
	}
	rxr->rx_pg_prod = prod;

	ring_prod = prod = rxr->rx_prod;
	for (i = 0; i < bp->rx_ring_size; i++) {
		if (bnx2_alloc_rx_data(bp, rxr, ring_prod, GFP_KERNEL) < 0) {
			netdev_warn(bp->dev, "init'ed rx ring %d with %d/%d skbs only\n",
				    ring_num, i, bp->rx_ring_size);
			break;
		}
		prod = BNX2_NEXT_RX_BD(prod);
		ring_prod = BNX2_RX_RING_IDX(prod);
	}
	rxr->rx_prod = prod;

	rxr->rx_bidx_addr = MB_GET_CID_ADDR(cid) + BNX2_L2CTX_HOST_BDIDX;
	rxr->rx_bseq_addr = MB_GET_CID_ADDR(cid) + BNX2_L2CTX_HOST_BSEQ;
	rxr->rx_pg_bidx_addr = MB_GET_CID_ADDR(cid) + BNX2_L2CTX_HOST_PG_BDIDX;

	BNX2_WR16(bp, rxr->rx_pg_bidx_addr, rxr->rx_pg_prod);
	BNX2_WR16(bp, rxr->rx_bidx_addr, prod);

	BNX2_WR(bp, rxr->rx_bseq_addr, rxr->rx_prod_bseq);
}

static void
bnx2_init_all_rings(struct bnx2 *bp)
{
	int i;
	u32 val;

	bnx2_clear_ring_states(bp);

	BNX2_WR(bp, BNX2_TSCH_TSS_CFG, 0);
	for (i = 0; i < bp->num_tx_rings; i++)
		bnx2_init_tx_ring(bp, i);

	if (bp->num_tx_rings > 1)
		BNX2_WR(bp, BNX2_TSCH_TSS_CFG, ((bp->num_tx_rings - 1) << 24) |
			(TX_TSS_CID << 7));

	BNX2_WR(bp, BNX2_RLUP_RSS_CONFIG, 0);
	bnx2_reg_wr_ind(bp, BNX2_RXP_SCRATCH_RSS_TBL_SZ, 0);

	for (i = 0; i < bp->num_rx_rings; i++)
		bnx2_init_rx_ring(bp, i);

	if (bp->num_rx_rings > 1) {
		u32 tbl_32 = 0;

		for (i = 0; i < BNX2_RXP_SCRATCH_RSS_TBL_MAX_ENTRIES; i++) {
			int shift = (i % 8) << 2;

			tbl_32 |= (i % (bp->num_rx_rings - 1)) << shift;
			if ((i % 8) == 7) {
				BNX2_WR(bp, BNX2_RLUP_RSS_DATA, tbl_32);
				BNX2_WR(bp, BNX2_RLUP_RSS_COMMAND, (i >> 3) |
					BNX2_RLUP_RSS_COMMAND_RSS_WRITE_MASK |
					BNX2_RLUP_RSS_COMMAND_WRITE |
					BNX2_RLUP_RSS_COMMAND_HASH_MASK);
				tbl_32 = 0;
			}
		}

		val = BNX2_RLUP_RSS_CONFIG_IPV4_RSS_TYPE_ALL_XI |
		      BNX2_RLUP_RSS_CONFIG_IPV6_RSS_TYPE_ALL_XI;

		BNX2_WR(bp, BNX2_RLUP_RSS_CONFIG, val);

	}
}

static u32 bnx2_find_max_ring(u32 ring_size, u32 max_size)
{
	u32 max, num_rings = 1;

	while (ring_size > BNX2_MAX_RX_DESC_CNT) {
		ring_size -= BNX2_MAX_RX_DESC_CNT;
		num_rings++;
	}
	/* round to next power of 2 */
	max = max_size;
	while ((max & num_rings) == 0)
		max >>= 1;

	if (num_rings != max)
		max <<= 1;

	return max;
}

static void
bnx2_set_rx_ring_size(struct bnx2 *bp, u32 size)
{
	u32 rx_size, rx_space, jumbo_size;

	/* 8 for CRC and VLAN */
	rx_size = bp->dev->mtu + ETH_HLEN + BNX2_RX_OFFSET + 8;

	rx_space = SKB_DATA_ALIGN(rx_size + BNX2_RX_ALIGN) + NET_SKB_PAD +
		SKB_DATA_ALIGN(sizeof(struct skb_shared_info));

	bp->rx_copy_thresh = BNX2_RX_COPY_THRESH;
	bp->rx_pg_ring_size = 0;
	bp->rx_max_pg_ring = 0;
	bp->rx_max_pg_ring_idx = 0;
	if ((rx_space > PAGE_SIZE) && !(bp->flags & BNX2_FLAG_JUMBO_BROKEN)) {
		int pages = PAGE_ALIGN(bp->dev->mtu - 40) >> PAGE_SHIFT;

		jumbo_size = size * pages;
		if (jumbo_size > BNX2_MAX_TOTAL_RX_PG_DESC_CNT)
			jumbo_size = BNX2_MAX_TOTAL_RX_PG_DESC_CNT;

		bp->rx_pg_ring_size = jumbo_size;
		bp->rx_max_pg_ring = bnx2_find_max_ring(jumbo_size,
							BNX2_MAX_RX_PG_RINGS);
		bp->rx_max_pg_ring_idx =
			(bp->rx_max_pg_ring * BNX2_RX_DESC_CNT) - 1;
		rx_size = BNX2_RX_COPY_THRESH + BNX2_RX_OFFSET;
		bp->rx_copy_thresh = 0;
	}

	bp->rx_buf_use_size = rx_size;
	/* hw alignment + build_skb() overhead*/
	bp->rx_buf_size = SKB_DATA_ALIGN(bp->rx_buf_use_size + BNX2_RX_ALIGN) +
		NET_SKB_PAD + SKB_DATA_ALIGN(sizeof(struct skb_shared_info));
	bp->rx_jumbo_thresh = rx_size - BNX2_RX_OFFSET;
	bp->rx_ring_size = size;
	bp->rx_max_ring = bnx2_find_max_ring(size, BNX2_MAX_RX_RINGS);
	bp->rx_max_ring_idx = (bp->rx_max_ring * BNX2_RX_DESC_CNT) - 1;
}

static void
bnx2_free_tx_skbs(struct bnx2 *bp)
{
	int i;

	for (i = 0; i < bp->num_tx_rings; i++) {
		struct bnx2_napi *bnapi = &bp->bnx2_napi[i];
		struct bnx2_tx_ring_info *txr = &bnapi->tx_ring;
		int j;

		if (!txr->tx_buf_ring)
			continue;

		for (j = 0; j < BNX2_TX_DESC_CNT; ) {
			struct bnx2_sw_tx_bd *tx_buf = &txr->tx_buf_ring[j];
			struct sk_buff *skb = tx_buf->skb;
			int k, last;

			if (!skb) {
				j = BNX2_NEXT_TX_BD(j);
				continue;
			}

			dma_unmap_single(&bp->pdev->dev,
					 dma_unmap_addr(tx_buf, mapping),
					 skb_headlen(skb),
					 PCI_DMA_TODEVICE);

			tx_buf->skb = NULL;

			last = tx_buf->nr_frags;
			j = BNX2_NEXT_TX_BD(j);
			for (k = 0; k < last; k++, j = BNX2_NEXT_TX_BD(j)) {
				tx_buf = &txr->tx_buf_ring[BNX2_TX_RING_IDX(j)];
				dma_unmap_page(&bp->pdev->dev,
					dma_unmap_addr(tx_buf, mapping),
					skb_frag_size(&skb_shinfo(skb)->frags[k]),
					PCI_DMA_TODEVICE);
			}
			dev_kfree_skb(skb);
		}
		netdev_tx_reset_queue(netdev_get_tx_queue(bp->dev, i));
	}
}

static void
bnx2_free_rx_skbs(struct bnx2 *bp)
{
	int i;

	for (i = 0; i < bp->num_rx_rings; i++) {
		struct bnx2_napi *bnapi = &bp->bnx2_napi[i];
		struct bnx2_rx_ring_info *rxr = &bnapi->rx_ring;
		int j;

		if (!rxr->rx_buf_ring)
			return;

		for (j = 0; j < bp->rx_max_ring_idx; j++) {
			struct bnx2_sw_bd *rx_buf = &rxr->rx_buf_ring[j];
			u8 *data = rx_buf->data;

			if (!data)
				continue;

			dma_unmap_single(&bp->pdev->dev,
					 dma_unmap_addr(rx_buf, mapping),
					 bp->rx_buf_use_size,
					 PCI_DMA_FROMDEVICE);

			rx_buf->data = NULL;

			kfree(data);
		}
		for (j = 0; j < bp->rx_max_pg_ring_idx; j++)
			bnx2_free_rx_page(bp, rxr, j);
	}
}

static void
bnx2_free_skbs(struct bnx2 *bp)
{
	bnx2_free_tx_skbs(bp);
	bnx2_free_rx_skbs(bp);
}

static int
bnx2_reset_nic(struct bnx2 *bp, u32 reset_code)
{
	int rc;

	rc = bnx2_reset_chip(bp, reset_code);
	bnx2_free_skbs(bp);
	if (rc)
		return rc;

	if ((rc = bnx2_init_chip(bp)) != 0)
		return rc;

	bnx2_init_all_rings(bp);
	return 0;
}

static int
bnx2_init_nic(struct bnx2 *bp, int reset_phy)
{
	int rc;

	if ((rc = bnx2_reset_nic(bp, BNX2_DRV_MSG_CODE_RESET)) != 0)
		return rc;

	spin_lock_bh(&bp->phy_lock);
	bnx2_init_phy(bp, reset_phy);
	bnx2_set_link(bp);
	if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP)
		bnx2_remote_phy_event(bp);
	spin_unlock_bh(&bp->phy_lock);
	return 0;
}

static int
bnx2_shutdown_chip(struct bnx2 *bp)
{
	u32 reset_code;

	if (bp->flags & BNX2_FLAG_NO_WOL)
		reset_code = BNX2_DRV_MSG_CODE_UNLOAD_LNK_DN;
	else if (bp->wol)
		reset_code = BNX2_DRV_MSG_CODE_SUSPEND_WOL;
	else
		reset_code = BNX2_DRV_MSG_CODE_SUSPEND_NO_WOL;

	return bnx2_reset_chip(bp, reset_code);
}

static int
bnx2_test_registers(struct bnx2 *bp)
{
	int ret;
	int i, is_5709;
	static const struct {
		u16   offset;
		u16   flags;
#define BNX2_FL_NOT_5709	1
		u32   rw_mask;
		u32   ro_mask;
	} reg_tbl[] = {
		{ 0x006c, 0, 0x00000000, 0x0000003f },
		{ 0x0090, 0, 0xffffffff, 0x00000000 },
		{ 0x0094, 0, 0x00000000, 0x00000000 },

		{ 0x0404, BNX2_FL_NOT_5709, 0x00003f00, 0x00000000 },
		{ 0x0418, BNX2_FL_NOT_5709, 0x00000000, 0xffffffff },
		{ 0x041c, BNX2_FL_NOT_5709, 0x00000000, 0xffffffff },
		{ 0x0420, BNX2_FL_NOT_5709, 0x00000000, 0x80ffffff },
		{ 0x0424, BNX2_FL_NOT_5709, 0x00000000, 0x00000000 },
		{ 0x0428, BNX2_FL_NOT_5709, 0x00000000, 0x00000001 },
		{ 0x0450, BNX2_FL_NOT_5709, 0x00000000, 0x0000ffff },
		{ 0x0454, BNX2_FL_NOT_5709, 0x00000000, 0xffffffff },
		{ 0x0458, BNX2_FL_NOT_5709, 0x00000000, 0xffffffff },

		{ 0x0808, BNX2_FL_NOT_5709, 0x00000000, 0xffffffff },
		{ 0x0854, BNX2_FL_NOT_5709, 0x00000000, 0xffffffff },
		{ 0x0868, BNX2_FL_NOT_5709, 0x00000000, 0x77777777 },
		{ 0x086c, BNX2_FL_NOT_5709, 0x00000000, 0x77777777 },
		{ 0x0870, BNX2_FL_NOT_5709, 0x00000000, 0x77777777 },
		{ 0x0874, BNX2_FL_NOT_5709, 0x00000000, 0x77777777 },

		{ 0x0c00, BNX2_FL_NOT_5709, 0x00000000, 0x00000001 },
		{ 0x0c04, BNX2_FL_NOT_5709, 0x00000000, 0x03ff0001 },
		{ 0x0c08, BNX2_FL_NOT_5709,  0x0f0ff073, 0x00000000 },

		{ 0x1000, 0, 0x00000000, 0x00000001 },
		{ 0x1004, BNX2_FL_NOT_5709, 0x00000000, 0x000f0001 },

		{ 0x1408, 0, 0x01c00800, 0x00000000 },
		{ 0x149c, 0, 0x8000ffff, 0x00000000 },
		{ 0x14a8, 0, 0x00000000, 0x000001ff },
		{ 0x14ac, 0, 0x0fffffff, 0x10000000 },
		{ 0x14b0, 0, 0x00000002, 0x00000001 },
		{ 0x14b8, 0, 0x00000000, 0x00000000 },
		{ 0x14c0, 0, 0x00000000, 0x00000009 },
		{ 0x14c4, 0, 0x00003fff, 0x00000000 },
		{ 0x14cc, 0, 0x00000000, 0x00000001 },
		{ 0x14d0, 0, 0xffffffff, 0x00000000 },

		{ 0x1800, 0, 0x00000000, 0x00000001 },
		{ 0x1804, 0, 0x00000000, 0x00000003 },

		{ 0x2800, 0, 0x00000000, 0x00000001 },
		{ 0x2804, 0, 0x00000000, 0x00003f01 },
		{ 0x2808, 0, 0x0f3f3f03, 0x00000000 },
		{ 0x2810, 0, 0xffff0000, 0x00000000 },
		{ 0x2814, 0, 0xffff0000, 0x00000000 },
		{ 0x2818, 0, 0xffff0000, 0x00000000 },
		{ 0x281c, 0, 0xffff0000, 0x00000000 },
		{ 0x2834, 0, 0xffffffff, 0x00000000 },
		{ 0x2840, 0, 0x00000000, 0xffffffff },
		{ 0x2844, 0, 0x00000000, 0xffffffff },
		{ 0x2848, 0, 0xffffffff, 0x00000000 },
		{ 0x284c, 0, 0xf800f800, 0x07ff07ff },

		{ 0x2c00, 0, 0x00000000, 0x00000011 },
		{ 0x2c04, 0, 0x00000000, 0x00030007 },

		{ 0x3c00, 0, 0x00000000, 0x00000001 },
		{ 0x3c04, 0, 0x00000000, 0x00070000 },
		{ 0x3c08, 0, 0x00007f71, 0x07f00000 },
		{ 0x3c0c, 0, 0x1f3ffffc, 0x00000000 },
		{ 0x3c10, 0, 0xffffffff, 0x00000000 },
		{ 0x3c14, 0, 0x00000000, 0xffffffff },
		{ 0x3c18, 0, 0x00000000, 0xffffffff },
		{ 0x3c1c, 0, 0xfffff000, 0x00000000 },
		{ 0x3c20, 0, 0xffffff00, 0x00000000 },

		{ 0x5004, 0, 0x00000000, 0x0000007f },
		{ 0x5008, 0, 0x0f0007ff, 0x00000000 },

		{ 0x5c00, 0, 0x00000000, 0x00000001 },
		{ 0x5c04, 0, 0x00000000, 0x0003000f },
		{ 0x5c08, 0, 0x00000003, 0x00000000 },
		{ 0x5c0c, 0, 0x0000fff8, 0x00000000 },
		{ 0x5c10, 0, 0x00000000, 0xffffffff },
		{ 0x5c80, 0, 0x00000000, 0x0f7113f1 },
		{ 0x5c84, 0, 0x00000000, 0x0000f333 },
		{ 0x5c88, 0, 0x00000000, 0x00077373 },
		{ 0x5c8c, 0, 0x00000000, 0x0007f737 },

		{ 0x6808, 0, 0x0000ff7f, 0x00000000 },
		{ 0x680c, 0, 0xffffffff, 0x00000000 },
		{ 0x6810, 0, 0xffffffff, 0x00000000 },
		{ 0x6814, 0, 0xffffffff, 0x00000000 },
		{ 0x6818, 0, 0xffffffff, 0x00000000 },
		{ 0x681c, 0, 0xffffffff, 0x00000000 },
		{ 0x6820, 0, 0x00ff00ff, 0x00000000 },
		{ 0x6824, 0, 0x00ff00ff, 0x00000000 },
		{ 0x6828, 0, 0x00ff00ff, 0x00000000 },
		{ 0x682c, 0, 0x03ff03ff, 0x00000000 },
		{ 0x6830, 0, 0x03ff03ff, 0x00000000 },
		{ 0x6834, 0, 0x03ff03ff, 0x00000000 },
		{ 0x6838, 0, 0x03ff03ff, 0x00000000 },
		{ 0x683c, 0, 0x0000ffff, 0x00000000 },
		{ 0x6840, 0, 0x00000ff0, 0x00000000 },
		{ 0x6844, 0, 0x00ffff00, 0x00000000 },
		{ 0x684c, 0, 0xffffffff, 0x00000000 },
		{ 0x6850, 0, 0x7f7f7f7f, 0x00000000 },
		{ 0x6854, 0, 0x7f7f7f7f, 0x00000000 },
		{ 0x6858, 0, 0x7f7f7f7f, 0x00000000 },
		{ 0x685c, 0, 0x7f7f7f7f, 0x00000000 },
		{ 0x6908, 0, 0x00000000, 0x0001ff0f },
		{ 0x690c, 0, 0x00000000, 0x0ffe00f0 },

		{ 0xffff, 0, 0x00000000, 0x00000000 },
	};

	ret = 0;
	is_5709 = 0;
	if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
		is_5709 = 1;

	for (i = 0; reg_tbl[i].offset != 0xffff; i++) {
		u32 offset, rw_mask, ro_mask, save_val, val;
		u16 flags = reg_tbl[i].flags;

		if (is_5709 && (flags & BNX2_FL_NOT_5709))
			continue;

		offset = (u32) reg_tbl[i].offset;
		rw_mask = reg_tbl[i].rw_mask;
		ro_mask = reg_tbl[i].ro_mask;

		save_val = readl(bp->regview + offset);

		writel(0, bp->regview + offset);

		val = readl(bp->regview + offset);
		if ((val & rw_mask) != 0) {
			goto reg_test_err;
		}

		if ((val & ro_mask) != (save_val & ro_mask)) {
			goto reg_test_err;
		}

		writel(0xffffffff, bp->regview + offset);

		val = readl(bp->regview + offset);
		if ((val & rw_mask) != rw_mask) {
			goto reg_test_err;
		}

		if ((val & ro_mask) != (save_val & ro_mask)) {
			goto reg_test_err;
		}

		writel(save_val, bp->regview + offset);
		continue;

reg_test_err:
		writel(save_val, bp->regview + offset);
		ret = -ENODEV;
		break;
	}
	return ret;
}

static int
bnx2_do_mem_test(struct bnx2 *bp, u32 start, u32 size)
{
	static const u32 test_pattern[] = { 0x00000000, 0xffffffff, 0x55555555,
		0xaaaaaaaa , 0xaa55aa55, 0x55aa55aa };
	int i;

	for (i = 0; i < sizeof(test_pattern) / 4; i++) {
		u32 offset;

		for (offset = 0; offset < size; offset += 4) {

			bnx2_reg_wr_ind(bp, start + offset, test_pattern[i]);

			if (bnx2_reg_rd_ind(bp, start + offset) !=
				test_pattern[i]) {
				return -ENODEV;
			}
		}
	}
	return 0;
}

static int
bnx2_test_memory(struct bnx2 *bp)
{
	int ret = 0;
	int i;
	static struct mem_entry {
		u32   offset;
		u32   len;
	} mem_tbl_5706[] = {
		{ 0x60000,  0x4000 },
		{ 0xa0000,  0x3000 },
		{ 0xe0000,  0x4000 },
		{ 0x120000, 0x4000 },
		{ 0x1a0000, 0x4000 },
		{ 0x160000, 0x4000 },
		{ 0xffffffff, 0    },
	},
	mem_tbl_5709[] = {
		{ 0x60000,  0x4000 },
		{ 0xa0000,  0x3000 },
		{ 0xe0000,  0x4000 },
		{ 0x120000, 0x4000 },
		{ 0x1a0000, 0x4000 },
		{ 0xffffffff, 0    },
	};
	struct mem_entry *mem_tbl;

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
		mem_tbl = mem_tbl_5709;
	else
		mem_tbl = mem_tbl_5706;

	for (i = 0; mem_tbl[i].offset != 0xffffffff; i++) {
		if ((ret = bnx2_do_mem_test(bp, mem_tbl[i].offset,
			mem_tbl[i].len)) != 0) {
			return ret;
		}
	}

	return ret;
}

#define BNX2_MAC_LOOPBACK	0
#define BNX2_PHY_LOOPBACK	1

static int
bnx2_run_loopback(struct bnx2 *bp, int loopback_mode)
{
	unsigned int pkt_size, num_pkts, i;
	struct sk_buff *skb;
	u8 *data;
	unsigned char *packet;
	u16 rx_start_idx, rx_idx;
	dma_addr_t map;
	struct bnx2_tx_bd *txbd;
	struct bnx2_sw_bd *rx_buf;
	struct l2_fhdr *rx_hdr;
	int ret = -ENODEV;
	struct bnx2_napi *bnapi = &bp->bnx2_napi[0], *tx_napi;
	struct bnx2_tx_ring_info *txr;
	struct bnx2_rx_ring_info *rxr;

	tx_napi = bnapi;

	txr = &tx_napi->tx_ring;
	rxr = &bnapi->rx_ring;
	if (loopback_mode == BNX2_MAC_LOOPBACK) {
		bp->loopback = MAC_LOOPBACK;
		bnx2_set_mac_loopback(bp);
	}
	else if (loopback_mode == BNX2_PHY_LOOPBACK) {
		if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP)
			return 0;

		bp->loopback = PHY_LOOPBACK;
		bnx2_set_phy_loopback(bp);
	}
	else
		return -EINVAL;

	pkt_size = min(bp->dev->mtu + ETH_HLEN, bp->rx_jumbo_thresh - 4);
	skb = netdev_alloc_skb(bp->dev, pkt_size);
	if (!skb)
		return -ENOMEM;
	packet = skb_put(skb, pkt_size);
	memcpy(packet, bp->dev->dev_addr, ETH_ALEN);
	memset(packet + ETH_ALEN, 0x0, 8);
	for (i = 14; i < pkt_size; i++)
		packet[i] = (unsigned char) (i & 0xff);

	map = dma_map_single(&bp->pdev->dev, skb->data, pkt_size,
			     PCI_DMA_TODEVICE);
	if (dma_mapping_error(&bp->pdev->dev, map)) {
		dev_kfree_skb(skb);
		return -EIO;
	}

	BNX2_WR(bp, BNX2_HC_COMMAND,
		bp->hc_cmd | BNX2_HC_COMMAND_COAL_NOW_WO_INT);

	BNX2_RD(bp, BNX2_HC_COMMAND);

	udelay(5);
	rx_start_idx = bnx2_get_hw_rx_cons(bnapi);

	num_pkts = 0;

	txbd = &txr->tx_desc_ring[BNX2_TX_RING_IDX(txr->tx_prod)];

	txbd->tx_bd_haddr_hi = (u64) map >> 32;
	txbd->tx_bd_haddr_lo = (u64) map & 0xffffffff;
	txbd->tx_bd_mss_nbytes = pkt_size;
	txbd->tx_bd_vlan_tag_flags = TX_BD_FLAGS_START | TX_BD_FLAGS_END;

	num_pkts++;
	txr->tx_prod = BNX2_NEXT_TX_BD(txr->tx_prod);
	txr->tx_prod_bseq += pkt_size;

	BNX2_WR16(bp, txr->tx_bidx_addr, txr->tx_prod);
	BNX2_WR(bp, txr->tx_bseq_addr, txr->tx_prod_bseq);

	udelay(100);

	BNX2_WR(bp, BNX2_HC_COMMAND,
		bp->hc_cmd | BNX2_HC_COMMAND_COAL_NOW_WO_INT);

	BNX2_RD(bp, BNX2_HC_COMMAND);

	udelay(5);

	dma_unmap_single(&bp->pdev->dev, map, pkt_size, PCI_DMA_TODEVICE);
	dev_kfree_skb(skb);

	if (bnx2_get_hw_tx_cons(tx_napi) != txr->tx_prod)
		goto loopback_test_done;

	rx_idx = bnx2_get_hw_rx_cons(bnapi);
	if (rx_idx != rx_start_idx + num_pkts) {
		goto loopback_test_done;
	}

	rx_buf = &rxr->rx_buf_ring[rx_start_idx];
	data = rx_buf->data;

	rx_hdr = get_l2_fhdr(data);
	data = (u8 *)rx_hdr + BNX2_RX_OFFSET;

	dma_sync_single_for_cpu(&bp->pdev->dev,
		dma_unmap_addr(rx_buf, mapping),
		bp->rx_buf_use_size, PCI_DMA_FROMDEVICE);

	if (rx_hdr->l2_fhdr_status &
		(L2_FHDR_ERRORS_BAD_CRC |
		L2_FHDR_ERRORS_PHY_DECODE |
		L2_FHDR_ERRORS_ALIGNMENT |
		L2_FHDR_ERRORS_TOO_SHORT |
		L2_FHDR_ERRORS_GIANT_FRAME)) {

		goto loopback_test_done;
	}

	if ((rx_hdr->l2_fhdr_pkt_len - 4) != pkt_size) {
		goto loopback_test_done;
	}

	for (i = 14; i < pkt_size; i++) {
		if (*(data + i) != (unsigned char) (i & 0xff)) {
			goto loopback_test_done;
		}
	}

	ret = 0;

loopback_test_done:
	bp->loopback = 0;
	return ret;
}

#define BNX2_MAC_LOOPBACK_FAILED	1
#define BNX2_PHY_LOOPBACK_FAILED	2
#define BNX2_LOOPBACK_FAILED		(BNX2_MAC_LOOPBACK_FAILED |	\
					 BNX2_PHY_LOOPBACK_FAILED)

static int
bnx2_test_loopback(struct bnx2 *bp)
{
	int rc = 0;

	if (!netif_running(bp->dev))
		return BNX2_LOOPBACK_FAILED;

	bnx2_reset_nic(bp, BNX2_DRV_MSG_CODE_RESET);
	spin_lock_bh(&bp->phy_lock);
	bnx2_init_phy(bp, 1);
	spin_unlock_bh(&bp->phy_lock);
	if (bnx2_run_loopback(bp, BNX2_MAC_LOOPBACK))
		rc |= BNX2_MAC_LOOPBACK_FAILED;
	if (bnx2_run_loopback(bp, BNX2_PHY_LOOPBACK))
		rc |= BNX2_PHY_LOOPBACK_FAILED;
	return rc;
}

#define NVRAM_SIZE 0x200
#define CRC32_RESIDUAL 0xdebb20e3

static int
bnx2_test_nvram(struct bnx2 *bp)
{
	__be32 buf[NVRAM_SIZE / 4];
	u8 *data = (u8 *) buf;
	int rc = 0;
	u32 magic, csum;

	if ((rc = bnx2_nvram_read(bp, 0, data, 4)) != 0)
		goto test_nvram_done;

        magic = be32_to_cpu(buf[0]);
	if (magic != 0x669955aa) {
		rc = -ENODEV;
		goto test_nvram_done;
	}

	if ((rc = bnx2_nvram_read(bp, 0x100, data, NVRAM_SIZE)) != 0)
		goto test_nvram_done;

	csum = ether_crc_le(0x100, data);
	if (csum != CRC32_RESIDUAL) {
		rc = -ENODEV;
		goto test_nvram_done;
	}

	csum = ether_crc_le(0x100, data + 0x100);
	if (csum != CRC32_RESIDUAL) {
		rc = -ENODEV;
	}

test_nvram_done:
	return rc;
}

static int
bnx2_test_link(struct bnx2 *bp)
{
	u32 bmsr;

	if (!netif_running(bp->dev))
		return -ENODEV;

	if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP) {
		if (bp->link_up)
			return 0;
		return -ENODEV;
	}
	spin_lock_bh(&bp->phy_lock);
	bnx2_enable_bmsr1(bp);
	bnx2_read_phy(bp, bp->mii_bmsr1, &bmsr);
	bnx2_read_phy(bp, bp->mii_bmsr1, &bmsr);
	bnx2_disable_bmsr1(bp);
	spin_unlock_bh(&bp->phy_lock);

	if (bmsr & BMSR_LSTATUS) {
		return 0;
	}
	return -ENODEV;
}

static int
bnx2_test_intr(struct bnx2 *bp)
{
	int i;
	u16 status_idx;

	if (!netif_running(bp->dev))
		return -ENODEV;

	status_idx = BNX2_RD(bp, BNX2_PCICFG_INT_ACK_CMD) & 0xffff;

	/* This register is not touched during run-time. */
	BNX2_WR(bp, BNX2_HC_COMMAND, bp->hc_cmd | BNX2_HC_COMMAND_COAL_NOW);
	BNX2_RD(bp, BNX2_HC_COMMAND);

	for (i = 0; i < 10; i++) {
		if ((BNX2_RD(bp, BNX2_PCICFG_INT_ACK_CMD) & 0xffff) !=
			status_idx) {

			break;
		}

		msleep_interruptible(10);
	}
	if (i < 10)
		return 0;

	return -ENODEV;
}

/* Determining link for parallel detection. */
static int
bnx2_5706_serdes_has_link(struct bnx2 *bp)
{
	u32 mode_ctl, an_dbg, exp;

	if (bp->phy_flags & BNX2_PHY_FLAG_NO_PARALLEL)
		return 0;

	bnx2_write_phy(bp, MII_BNX2_MISC_SHADOW, MISC_SHDW_MODE_CTL);
	bnx2_read_phy(bp, MII_BNX2_MISC_SHADOW, &mode_ctl);

	if (!(mode_ctl & MISC_SHDW_MODE_CTL_SIG_DET))
		return 0;

	bnx2_write_phy(bp, MII_BNX2_MISC_SHADOW, MISC_SHDW_AN_DBG);
	bnx2_read_phy(bp, MII_BNX2_MISC_SHADOW, &an_dbg);
	bnx2_read_phy(bp, MII_BNX2_MISC_SHADOW, &an_dbg);

	if (an_dbg & (MISC_SHDW_AN_DBG_NOSYNC | MISC_SHDW_AN_DBG_RUDI_INVALID))
		return 0;

	bnx2_write_phy(bp, MII_BNX2_DSP_ADDRESS, MII_EXPAND_REG1);
	bnx2_read_phy(bp, MII_BNX2_DSP_RW_PORT, &exp);
	bnx2_read_phy(bp, MII_BNX2_DSP_RW_PORT, &exp);

	if (exp & MII_EXPAND_REG1_RUDI_C)	/* receiving CONFIG */
		return 0;

	return 1;
}

static void
bnx2_5706_serdes_timer(struct bnx2 *bp)
{
	int check_link = 1;

	spin_lock(&bp->phy_lock);
	if (bp->serdes_an_pending) {
		bp->serdes_an_pending--;
		check_link = 0;
	} else if ((bp->link_up == 0) && (bp->autoneg & AUTONEG_SPEED)) {
		u32 bmcr;

		bp->current_interval = BNX2_TIMER_INTERVAL;

		bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);

		if (bmcr & BMCR_ANENABLE) {
			if (bnx2_5706_serdes_has_link(bp)) {
				bmcr &= ~BMCR_ANENABLE;
				bmcr |= BMCR_SPEED1000 | BMCR_FULLDPLX;
				bnx2_write_phy(bp, bp->mii_bmcr, bmcr);
				bp->phy_flags |= BNX2_PHY_FLAG_PARALLEL_DETECT;
			}
		}
	}
	else if ((bp->link_up) && (bp->autoneg & AUTONEG_SPEED) &&
		 (bp->phy_flags & BNX2_PHY_FLAG_PARALLEL_DETECT)) {
		u32 phy2;

		bnx2_write_phy(bp, 0x17, 0x0f01);
		bnx2_read_phy(bp, 0x15, &phy2);
		if (phy2 & 0x20) {
			u32 bmcr;

			bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);
			bmcr |= BMCR_ANENABLE;
			bnx2_write_phy(bp, bp->mii_bmcr, bmcr);

			bp->phy_flags &= ~BNX2_PHY_FLAG_PARALLEL_DETECT;
		}
	} else
		bp->current_interval = BNX2_TIMER_INTERVAL;

	if (check_link) {
		u32 val;

		bnx2_write_phy(bp, MII_BNX2_MISC_SHADOW, MISC_SHDW_AN_DBG);
		bnx2_read_phy(bp, MII_BNX2_MISC_SHADOW, &val);
		bnx2_read_phy(bp, MII_BNX2_MISC_SHADOW, &val);

		if (bp->link_up && (val & MISC_SHDW_AN_DBG_NOSYNC)) {
			if (!(bp->phy_flags & BNX2_PHY_FLAG_FORCED_DOWN)) {
				bnx2_5706s_force_link_dn(bp, 1);
				bp->phy_flags |= BNX2_PHY_FLAG_FORCED_DOWN;
			} else
				bnx2_set_link(bp);
		} else if (!bp->link_up && !(val & MISC_SHDW_AN_DBG_NOSYNC))
			bnx2_set_link(bp);
	}
	spin_unlock(&bp->phy_lock);
}

static void
bnx2_5708_serdes_timer(struct bnx2 *bp)
{
	if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP)
		return;

	if ((bp->phy_flags & BNX2_PHY_FLAG_2_5G_CAPABLE) == 0) {
		bp->serdes_an_pending = 0;
		return;
	}

	spin_lock(&bp->phy_lock);
	if (bp->serdes_an_pending)
		bp->serdes_an_pending--;
	else if ((bp->link_up == 0) && (bp->autoneg & AUTONEG_SPEED)) {
		u32 bmcr;

		bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);
		if (bmcr & BMCR_ANENABLE) {
			bnx2_enable_forced_2g5(bp);
			bp->current_interval = BNX2_SERDES_FORCED_TIMEOUT;
		} else {
			bnx2_disable_forced_2g5(bp);
			bp->serdes_an_pending = 2;
			bp->current_interval = BNX2_TIMER_INTERVAL;
		}

	} else
		bp->current_interval = BNX2_TIMER_INTERVAL;

	spin_unlock(&bp->phy_lock);
}

static void
bnx2_timer(struct timer_list *t)
{
	struct bnx2 *bp = from_timer(bp, t, timer);

	if (!netif_running(bp->dev))
		return;

	if (atomic_read(&bp->intr_sem) != 0)
		goto bnx2_restart_timer;

	if ((bp->flags & (BNX2_FLAG_USING_MSI | BNX2_FLAG_ONE_SHOT_MSI)) ==
	     BNX2_FLAG_USING_MSI)
		bnx2_chk_missed_msi(bp);

	bnx2_send_heart_beat(bp);

	bp->stats_blk->stat_FwRxDrop =
		bnx2_reg_rd_ind(bp, BNX2_FW_RX_DROP_COUNT);

	/* workaround occasional corrupted counters */
	if ((bp->flags & BNX2_FLAG_BROKEN_STATS) && bp->stats_ticks)
		BNX2_WR(bp, BNX2_HC_COMMAND, bp->hc_cmd |
			BNX2_HC_COMMAND_STATS_NOW);

	if (bp->phy_flags & BNX2_PHY_FLAG_SERDES) {
		if (BNX2_CHIP(bp) == BNX2_CHIP_5706)
			bnx2_5706_serdes_timer(bp);
		else
			bnx2_5708_serdes_timer(bp);
	}

bnx2_restart_timer:
	mod_timer(&bp->timer, jiffies + bp->current_interval);
}

static int
bnx2_request_irq(struct bnx2 *bp)
{
	unsigned long flags;
	struct bnx2_irq *irq;
	int rc = 0, i;

	if (bp->flags & BNX2_FLAG_USING_MSI_OR_MSIX)
		flags = 0;
	else
		flags = IRQF_SHARED;

	for (i = 0; i < bp->irq_nvecs; i++) {
		irq = &bp->irq_tbl[i];
		rc = request_irq(irq->vector, irq->handler, flags, irq->name,
				 &bp->bnx2_napi[i]);
		if (rc)
			break;
		irq->requested = 1;
	}
	return rc;
}

static void
__bnx2_free_irq(struct bnx2 *bp)
{
	struct bnx2_irq *irq;
	int i;

	for (i = 0; i < bp->irq_nvecs; i++) {
		irq = &bp->irq_tbl[i];
		if (irq->requested)
			free_irq(irq->vector, &bp->bnx2_napi[i]);
		irq->requested = 0;
	}
}

static void
bnx2_free_irq(struct bnx2 *bp)
{

	__bnx2_free_irq(bp);
	if (bp->flags & BNX2_FLAG_USING_MSI)
		pci_disable_msi(bp->pdev);
	else if (bp->flags & BNX2_FLAG_USING_MSIX)
		pci_disable_msix(bp->pdev);

	bp->flags &= ~(BNX2_FLAG_USING_MSI_OR_MSIX | BNX2_FLAG_ONE_SHOT_MSI);
}

static void
bnx2_enable_msix(struct bnx2 *bp, int msix_vecs)
{
	int i, total_vecs;
	struct msix_entry msix_ent[BNX2_MAX_MSIX_VEC];
	struct net_device *dev = bp->dev;
	const int len = sizeof(bp->irq_tbl[0].name);

	bnx2_setup_msix_tbl(bp);
	BNX2_WR(bp, BNX2_PCI_MSIX_CONTROL, BNX2_MAX_MSIX_HW_VEC - 1);
	BNX2_WR(bp, BNX2_PCI_MSIX_TBL_OFF_BIR, BNX2_PCI_GRC_WINDOW2_BASE);
	BNX2_WR(bp, BNX2_PCI_MSIX_PBA_OFF_BIT, BNX2_PCI_GRC_WINDOW3_BASE);

	/*  Need to flush the previous three writes to ensure MSI-X
	 *  is setup properly */
	BNX2_RD(bp, BNX2_PCI_MSIX_CONTROL);

	for (i = 0; i < BNX2_MAX_MSIX_VEC; i++) {
		msix_ent[i].entry = i;
		msix_ent[i].vector = 0;
	}

	total_vecs = msix_vecs;
#ifdef BCM_CNIC
	total_vecs++;
#endif
	total_vecs = pci_enable_msix_range(bp->pdev, msix_ent,
					   BNX2_MIN_MSIX_VEC, total_vecs);
	if (total_vecs < 0)
		return;

	msix_vecs = total_vecs;
#ifdef BCM_CNIC
	msix_vecs--;
#endif
	bp->irq_nvecs = msix_vecs;
	bp->flags |= BNX2_FLAG_USING_MSIX | BNX2_FLAG_ONE_SHOT_MSI;
	for (i = 0; i < total_vecs; i++) {
		bp->irq_tbl[i].vector = msix_ent[i].vector;
		snprintf(bp->irq_tbl[i].name, len, "%s-%d", dev->name, i);
		bp->irq_tbl[i].handler = bnx2_msi_1shot;
	}
}

static int
bnx2_setup_int_mode(struct bnx2 *bp, int dis_msi)
{
	int cpus = netif_get_num_default_rss_queues();
	int msix_vecs;

	if (!bp->num_req_rx_rings)
		msix_vecs = max(cpus + 1, bp->num_req_tx_rings);
	else if (!bp->num_req_tx_rings)
		msix_vecs = max(cpus, bp->num_req_rx_rings);
	else
		msix_vecs = max(bp->num_req_rx_rings, bp->num_req_tx_rings);

	msix_vecs = min(msix_vecs, RX_MAX_RINGS);

	bp->irq_tbl[0].handler = bnx2_interrupt;
	strcpy(bp->irq_tbl[0].name, bp->dev->name);
	bp->irq_nvecs = 1;
	bp->irq_tbl[0].vector = bp->pdev->irq;

	if ((bp->flags & BNX2_FLAG_MSIX_CAP) && !dis_msi)
		bnx2_enable_msix(bp, msix_vecs);

	if ((bp->flags & BNX2_FLAG_MSI_CAP) && !dis_msi &&
	    !(bp->flags & BNX2_FLAG_USING_MSIX)) {
		if (pci_enable_msi(bp->pdev) == 0) {
			bp->flags |= BNX2_FLAG_USING_MSI;
			if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
				bp->flags |= BNX2_FLAG_ONE_SHOT_MSI;
				bp->irq_tbl[0].handler = bnx2_msi_1shot;
			} else
				bp->irq_tbl[0].handler = bnx2_msi;

			bp->irq_tbl[0].vector = bp->pdev->irq;
		}
	}

	if (!bp->num_req_tx_rings)
		bp->num_tx_rings = rounddown_pow_of_two(bp->irq_nvecs);
	else
		bp->num_tx_rings = min(bp->irq_nvecs, bp->num_req_tx_rings);

	if (!bp->num_req_rx_rings)
		bp->num_rx_rings = bp->irq_nvecs;
	else
		bp->num_rx_rings = min(bp->irq_nvecs, bp->num_req_rx_rings);

	netif_set_real_num_tx_queues(bp->dev, bp->num_tx_rings);

	return netif_set_real_num_rx_queues(bp->dev, bp->num_rx_rings);
}

/* Called with rtnl_lock */
static int
bnx2_open(struct net_device *dev)
{
	struct bnx2 *bp = netdev_priv(dev);
	int rc;

	rc = bnx2_request_firmware(bp);
	if (rc < 0)
		goto out;

	netif_carrier_off(dev);

	bnx2_disable_int(bp);

	rc = bnx2_setup_int_mode(bp, disable_msi);
	if (rc)
		goto open_err;
	bnx2_init_napi(bp);
	bnx2_napi_enable(bp);
	rc = bnx2_alloc_mem(bp);
	if (rc)
		goto open_err;

	rc = bnx2_request_irq(bp);
	if (rc)
		goto open_err;

	rc = bnx2_init_nic(bp, 1);
	if (rc)
		goto open_err;

	mod_timer(&bp->timer, jiffies + bp->current_interval);

	atomic_set(&bp->intr_sem, 0);

	memset(bp->temp_stats_blk, 0, sizeof(struct statistics_block));

	bnx2_enable_int(bp);

	if (bp->flags & BNX2_FLAG_USING_MSI) {
		/* Test MSI to make sure it is working
		 * If MSI test fails, go back to INTx mode
		 */
		if (bnx2_test_intr(bp) != 0) {
			netdev_warn(bp->dev, "No interrupt was generated using MSI, switching to INTx mode. Please report this failure to the PCI maintainer and include system chipset information.\n");

			bnx2_disable_int(bp);
			bnx2_free_irq(bp);

			bnx2_setup_int_mode(bp, 1);

			rc = bnx2_init_nic(bp, 0);

			if (!rc)
				rc = bnx2_request_irq(bp);

			if (rc) {
				del_timer_sync(&bp->timer);
				goto open_err;
			}
			bnx2_enable_int(bp);
		}
	}
	if (bp->flags & BNX2_FLAG_USING_MSI)
		netdev_info(dev, "using MSI\n");
	else if (bp->flags & BNX2_FLAG_USING_MSIX)
		netdev_info(dev, "using MSIX\n");

	netif_tx_start_all_queues(dev);
out:
	return rc;

open_err:
	bnx2_napi_disable(bp);
	bnx2_free_skbs(bp);
	bnx2_free_irq(bp);
	bnx2_free_mem(bp);
	bnx2_del_napi(bp);
	bnx2_release_firmware(bp);
	goto out;
}

static void
bnx2_reset_task(struct work_struct *work)
{
	struct bnx2 *bp = container_of(work, struct bnx2, reset_task);
	int rc;
	u16 pcicmd;

	rtnl_lock();
	if (!netif_running(bp->dev)) {
		rtnl_unlock();
		return;
	}

	bnx2_netif_stop(bp, true);

	pci_read_config_word(bp->pdev, PCI_COMMAND, &pcicmd);
	if (!(pcicmd & PCI_COMMAND_MEMORY)) {
		/* in case PCI block has reset */
		pci_restore_state(bp->pdev);
		pci_save_state(bp->pdev);
	}
	rc = bnx2_init_nic(bp, 1);
	if (rc) {
		netdev_err(bp->dev, "failed to reset NIC, closing\n");
		bnx2_napi_enable(bp);
		dev_close(bp->dev);
		rtnl_unlock();
		return;
	}

	atomic_set(&bp->intr_sem, 1);
	bnx2_netif_start(bp, true);
	rtnl_unlock();
}

#define BNX2_FTQ_ENTRY(ftq) { __stringify(ftq##FTQ_CTL), BNX2_##ftq##FTQ_CTL }

static void
bnx2_dump_ftq(struct bnx2 *bp)
{
	int i;
	u32 reg, bdidx, cid, valid;
	struct net_device *dev = bp->dev;
	static const struct ftq_reg {
		char *name;
		u32 off;
	} ftq_arr[] = {
		BNX2_FTQ_ENTRY(RV2P_P),
		BNX2_FTQ_ENTRY(RV2P_T),
		BNX2_FTQ_ENTRY(RV2P_M),
		BNX2_FTQ_ENTRY(TBDR_),
		BNX2_FTQ_ENTRY(TDMA_),
		BNX2_FTQ_ENTRY(TXP_),
		BNX2_FTQ_ENTRY(TXP_),
		BNX2_FTQ_ENTRY(TPAT_),
		BNX2_FTQ_ENTRY(RXP_C),
		BNX2_FTQ_ENTRY(RXP_),
		BNX2_FTQ_ENTRY(COM_COMXQ_),
		BNX2_FTQ_ENTRY(COM_COMTQ_),
		BNX2_FTQ_ENTRY(COM_COMQ_),
		BNX2_FTQ_ENTRY(CP_CPQ_),
	};

	netdev_err(dev, "<--- start FTQ dump --->\n");
	for (i = 0; i < ARRAY_SIZE(ftq_arr); i++)
		netdev_err(dev, "%s %08x\n", ftq_arr[i].name,
			   bnx2_reg_rd_ind(bp, ftq_arr[i].off));

	netdev_err(dev, "CPU states:\n");
	for (reg = BNX2_TXP_CPU_MODE; reg <= BNX2_CP_CPU_MODE; reg += 0x40000)
		netdev_err(dev, "%06x mode %x state %x evt_mask %x pc %x pc %x instr %x\n",
			   reg, bnx2_reg_rd_ind(bp, reg),
			   bnx2_reg_rd_ind(bp, reg + 4),
			   bnx2_reg_rd_ind(bp, reg + 8),
			   bnx2_reg_rd_ind(bp, reg + 0x1c),
			   bnx2_reg_rd_ind(bp, reg + 0x1c),
			   bnx2_reg_rd_ind(bp, reg + 0x20));

	netdev_err(dev, "<--- end FTQ dump --->\n");
	netdev_err(dev, "<--- start TBDC dump --->\n");
	netdev_err(dev, "TBDC free cnt: %ld\n",
		   BNX2_RD(bp, BNX2_TBDC_STATUS) & BNX2_TBDC_STATUS_FREE_CNT);
	netdev_err(dev, "LINE     CID  BIDX   CMD  VALIDS\n");
	for (i = 0; i < 0x20; i++) {
		int j = 0;

		BNX2_WR(bp, BNX2_TBDC_BD_ADDR, i);
		BNX2_WR(bp, BNX2_TBDC_CAM_OPCODE,
			BNX2_TBDC_CAM_OPCODE_OPCODE_CAM_READ);
		BNX2_WR(bp, BNX2_TBDC_COMMAND, BNX2_TBDC_COMMAND_CMD_REG_ARB);
		while ((BNX2_RD(bp, BNX2_TBDC_COMMAND) &
			BNX2_TBDC_COMMAND_CMD_REG_ARB) && j < 100)
			j++;

		cid = BNX2_RD(bp, BNX2_TBDC_CID);
		bdidx = BNX2_RD(bp, BNX2_TBDC_BIDX);
		valid = BNX2_RD(bp, BNX2_TBDC_CAM_OPCODE);
		netdev_err(dev, "%02x    %06x  %04lx   %02x    [%x]\n",
			   i, cid, bdidx & BNX2_TBDC_BDIDX_BDIDX,
			   bdidx >> 24, (valid >> 8) & 0x0ff);
	}
	netdev_err(dev, "<--- end TBDC dump --->\n");
}

static void
bnx2_dump_state(struct bnx2 *bp)
{
	struct net_device *dev = bp->dev;
	u32 val1, val2;

	pci_read_config_dword(bp->pdev, PCI_COMMAND, &val1);
	netdev_err(dev, "DEBUG: intr_sem[%x] PCI_CMD[%08x]\n",
		   atomic_read(&bp->intr_sem), val1);
	pci_read_config_dword(bp->pdev, bp->pm_cap + PCI_PM_CTRL, &val1);
	pci_read_config_dword(bp->pdev, BNX2_PCICFG_MISC_CONFIG, &val2);
	netdev_err(dev, "DEBUG: PCI_PM[%08x] PCI_MISC_CFG[%08x]\n", val1, val2);
	netdev_err(dev, "DEBUG: EMAC_TX_STATUS[%08x] EMAC_RX_STATUS[%08x]\n",
		   BNX2_RD(bp, BNX2_EMAC_TX_STATUS),
		   BNX2_RD(bp, BNX2_EMAC_RX_STATUS));
	netdev_err(dev, "DEBUG: RPM_MGMT_PKT_CTRL[%08x]\n",
		   BNX2_RD(bp, BNX2_RPM_MGMT_PKT_CTRL));
	netdev_err(dev, "DEBUG: HC_STATS_INTERRUPT_STATUS[%08x]\n",
		   BNX2_RD(bp, BNX2_HC_STATS_INTERRUPT_STATUS));
	if (bp->flags & BNX2_FLAG_USING_MSIX)
		netdev_err(dev, "DEBUG: PBA[%08x]\n",
			   BNX2_RD(bp, BNX2_PCI_GRC_WINDOW3_BASE));
}

static void
bnx2_tx_timeout(struct net_device *dev, unsigned int txqueue)
{
	struct bnx2 *bp = netdev_priv(dev);

	bnx2_dump_ftq(bp);
	bnx2_dump_state(bp);
	bnx2_dump_mcp_state(bp);

	/* This allows the netif to be shutdown gracefully before resetting */
	schedule_work(&bp->reset_task);
}

/* Called with netif_tx_lock.
 * bnx2_tx_int() runs without netif_tx_lock unless it needs to call
 * netif_wake_queue().
 */
static netdev_tx_t
bnx2_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct bnx2 *bp = netdev_priv(dev);
	dma_addr_t mapping;
	struct bnx2_tx_bd *txbd;
	struct bnx2_sw_tx_bd *tx_buf;
	u32 len, vlan_tag_flags, last_frag, mss;
	u16 prod, ring_prod;
	int i;
	struct bnx2_napi *bnapi;
	struct bnx2_tx_ring_info *txr;
	struct netdev_queue *txq;

	/*  Determine which tx ring we will be placed on */
	i = skb_get_queue_mapping(skb);
	bnapi = &bp->bnx2_napi[i];
	txr = &bnapi->tx_ring;
	txq = netdev_get_tx_queue(dev, i);

	if (unlikely(bnx2_tx_avail(bp, txr) <
	    (skb_shinfo(skb)->nr_frags + 1))) {
		netif_tx_stop_queue(txq);
		netdev_err(dev, "BUG! Tx ring full when queue awake!\n");

		return NETDEV_TX_BUSY;
	}
	len = skb_headlen(skb);
	prod = txr->tx_prod;
	ring_prod = BNX2_TX_RING_IDX(prod);

	vlan_tag_flags = 0;
	if (skb->ip_summed == CHECKSUM_PARTIAL) {
		vlan_tag_flags |= TX_BD_FLAGS_TCP_UDP_CKSUM;
	}

	if (skb_vlan_tag_present(skb)) {
		vlan_tag_flags |=
			(TX_BD_FLAGS_VLAN_TAG | (skb_vlan_tag_get(skb) << 16));
	}

	if ((mss = skb_shinfo(skb)->gso_size)) {
		u32 tcp_opt_len;
		struct iphdr *iph;

		vlan_tag_flags |= TX_BD_FLAGS_SW_LSO;

		tcp_opt_len = tcp_optlen(skb);

		if (skb_shinfo(skb)->gso_type & SKB_GSO_TCPV6) {
			u32 tcp_off = skb_transport_offset(skb) -
				      sizeof(struct ipv6hdr) - ETH_HLEN;

			vlan_tag_flags |= ((tcp_opt_len >> 2) << 8) |
					  TX_BD_FLAGS_SW_FLAGS;
			if (likely(tcp_off == 0))
				vlan_tag_flags &= ~TX_BD_FLAGS_TCP6_OFF0_MSK;
			else {
				tcp_off >>= 3;
				vlan_tag_flags |= ((tcp_off & 0x3) <<
						   TX_BD_FLAGS_TCP6_OFF0_SHL) |
						  ((tcp_off & 0x10) <<
						   TX_BD_FLAGS_TCP6_OFF4_SHL);
				mss |= (tcp_off & 0xc) << TX_BD_TCP6_OFF2_SHL;
			}
		} else {
			iph = ip_hdr(skb);
			if (tcp_opt_len || (iph->ihl > 5)) {
				vlan_tag_flags |= ((iph->ihl - 5) +
						   (tcp_opt_len >> 2)) << 8;
			}
		}
	} else
		mss = 0;

	mapping = dma_map_single(&bp->pdev->dev, skb->data, len, PCI_DMA_TODEVICE);
	if (dma_mapping_error(&bp->pdev->dev, mapping)) {
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}

	tx_buf = &txr->tx_buf_ring[ring_prod];
	tx_buf->skb = skb;
	dma_unmap_addr_set(tx_buf, mapping, mapping);

	txbd = &txr->tx_desc_ring[ring_prod];

	txbd->tx_bd_haddr_hi = (u64) mapping >> 32;
	txbd->tx_bd_haddr_lo = (u64) mapping & 0xffffffff;
	txbd->tx_bd_mss_nbytes = len | (mss << 16);
	txbd->tx_bd_vlan_tag_flags = vlan_tag_flags | TX_BD_FLAGS_START;

	last_frag = skb_shinfo(skb)->nr_frags;
	tx_buf->nr_frags = last_frag;
	tx_buf->is_gso = skb_is_gso(skb);

	for (i = 0; i < last_frag; i++) {
		const skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

		prod = BNX2_NEXT_TX_BD(prod);
		ring_prod = BNX2_TX_RING_IDX(prod);
		txbd = &txr->tx_desc_ring[ring_prod];

		len = skb_frag_size(frag);
		mapping = skb_frag_dma_map(&bp->pdev->dev, frag, 0, len,
					   DMA_TO_DEVICE);
		if (dma_mapping_error(&bp->pdev->dev, mapping))
			goto dma_error;
		dma_unmap_addr_set(&txr->tx_buf_ring[ring_prod], mapping,
				   mapping);

		txbd->tx_bd_haddr_hi = (u64) mapping >> 32;
		txbd->tx_bd_haddr_lo = (u64) mapping & 0xffffffff;
		txbd->tx_bd_mss_nbytes = len | (mss << 16);
		txbd->tx_bd_vlan_tag_flags = vlan_tag_flags;

	}
	txbd->tx_bd_vlan_tag_flags |= TX_BD_FLAGS_END;

	/* Sync BD data before updating TX mailbox */
	wmb();

	netdev_tx_sent_queue(txq, skb->len);

	prod = BNX2_NEXT_TX_BD(prod);
	txr->tx_prod_bseq += skb->len;

	BNX2_WR16(bp, txr->tx_bidx_addr, prod);
	BNX2_WR(bp, txr->tx_bseq_addr, txr->tx_prod_bseq);

	txr->tx_prod = prod;

	if (unlikely(bnx2_tx_avail(bp, txr) <= MAX_SKB_FRAGS)) {
		netif_tx_stop_queue(txq);

		/* netif_tx_stop_queue() must be done before checking
		 * tx index in bnx2_tx_avail() below, because in
		 * bnx2_tx_int(), we update tx index before checking for
		 * netif_tx_queue_stopped().
		 */
		smp_mb();
		if (bnx2_tx_avail(bp, txr) > bp->tx_wake_thresh)
			netif_tx_wake_queue(txq);
	}

	return NETDEV_TX_OK;
dma_error:
	/* save value of frag that failed */
	last_frag = i;

	/* start back at beginning and unmap skb */
	prod = txr->tx_prod;
	ring_prod = BNX2_TX_RING_IDX(prod);
	tx_buf = &txr->tx_buf_ring[ring_prod];
	tx_buf->skb = NULL;
	dma_unmap_single(&bp->pdev->dev, dma_unmap_addr(tx_buf, mapping),
			 skb_headlen(skb), PCI_DMA_TODEVICE);

	/* unmap remaining mapped pages */
	for (i = 0; i < last_frag; i++) {
		prod = BNX2_NEXT_TX_BD(prod);
		ring_prod = BNX2_TX_RING_IDX(prod);
		tx_buf = &txr->tx_buf_ring[ring_prod];
		dma_unmap_page(&bp->pdev->dev, dma_unmap_addr(tx_buf, mapping),
			       skb_frag_size(&skb_shinfo(skb)->frags[i]),
			       PCI_DMA_TODEVICE);
	}

	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}

/* Called with rtnl_lock */
static int
bnx2_close(struct net_device *dev)
{
	struct bnx2 *bp = netdev_priv(dev);

	bnx2_disable_int_sync(bp);
	bnx2_napi_disable(bp);
	netif_tx_disable(dev);
	del_timer_sync(&bp->timer);
	bnx2_shutdown_chip(bp);
	bnx2_free_irq(bp);
	bnx2_free_skbs(bp);
	bnx2_free_mem(bp);
	bnx2_del_napi(bp);
	bp->link_up = 0;
	netif_carrier_off(bp->dev);
	return 0;
}

static void
bnx2_save_stats(struct bnx2 *bp)
{
	u32 *hw_stats = (u32 *) bp->stats_blk;
	u32 *temp_stats = (u32 *) bp->temp_stats_blk;
	int i;

	/* The 1st 10 counters are 64-bit counters */
	for (i = 0; i < 20; i += 2) {
		u32 hi;
		u64 lo;

		hi = temp_stats[i] + hw_stats[i];
		lo = (u64) temp_stats[i + 1] + (u64) hw_stats[i + 1];
		if (lo > 0xffffffff)
			hi++;
		temp_stats[i] = hi;
		temp_stats[i + 1] = lo & 0xffffffff;
	}

	for ( ; i < sizeof(struct statistics_block) / 4; i++)
		temp_stats[i] += hw_stats[i];
}

#define GET_64BIT_NET_STATS64(ctr)		\
	(((u64) (ctr##_hi) << 32) + (u64) (ctr##_lo))

#define GET_64BIT_NET_STATS(ctr)				\
	GET_64BIT_NET_STATS64(bp->stats_blk->ctr) +		\
	GET_64BIT_NET_STATS64(bp->temp_stats_blk->ctr)

#define GET_32BIT_NET_STATS(ctr)				\
	(unsigned long) (bp->stats_blk->ctr +			\
			 bp->temp_stats_blk->ctr)

static void
bnx2_get_stats64(struct net_device *dev, struct rtnl_link_stats64 *net_stats)
{
	struct bnx2 *bp = netdev_priv(dev);

	if (!bp->stats_blk)
		return;

	net_stats->rx_packets =
		GET_64BIT_NET_STATS(stat_IfHCInUcastPkts) +
		GET_64BIT_NET_STATS(stat_IfHCInMulticastPkts) +
		GET_64BIT_NET_STATS(stat_IfHCInBroadcastPkts);

	net_stats->tx_packets =
		GET_64BIT_NET_STATS(stat_IfHCOutUcastPkts) +
		GET_64BIT_NET_STATS(stat_IfHCOutMulticastPkts) +
		GET_64BIT_NET_STATS(stat_IfHCOutBroadcastPkts);

	net_stats->rx_bytes =
		GET_64BIT_NET_STATS(stat_IfHCInOctets);

	net_stats->tx_bytes =
		GET_64BIT_NET_STATS(stat_IfHCOutOctets);

	net_stats->multicast =
		GET_64BIT_NET_STATS(stat_IfHCInMulticastPkts);

	net_stats->collisions =
		GET_32BIT_NET_STATS(stat_EtherStatsCollisions);

	net_stats->rx_length_errors =
		GET_32BIT_NET_STATS(stat_EtherStatsUndersizePkts) +
		GET_32BIT_NET_STATS(stat_EtherStatsOverrsizePkts);

	net_stats->rx_over_errors =
		GET_32BIT_NET_STATS(stat_IfInFTQDiscards) +
		GET_32BIT_NET_STATS(stat_IfInMBUFDiscards);

	net_stats->rx_frame_errors =
		GET_32BIT_NET_STATS(stat_Dot3StatsAlignmentErrors);

	net_stats->rx_crc_errors =
		GET_32BIT_NET_STATS(stat_Dot3StatsFCSErrors);

	net_stats->rx_errors = net_stats->rx_length_errors +
		net_stats->rx_over_errors + net_stats->rx_frame_errors +
		net_stats->rx_crc_errors;

	net_stats->tx_aborted_errors =
		GET_32BIT_NET_STATS(stat_Dot3StatsExcessiveCollisions) +
		GET_32BIT_NET_STATS(stat_Dot3StatsLateCollisions);

	if ((BNX2_CHIP(bp) == BNX2_CHIP_5706) ||
	    (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5708_A0))
		net_stats->tx_carrier_errors = 0;
	else {
		net_stats->tx_carrier_errors =
			GET_32BIT_NET_STATS(stat_Dot3StatsCarrierSenseErrors);
	}

	net_stats->tx_errors =
		GET_32BIT_NET_STATS(stat_emac_tx_stat_dot3statsinternalmactransmiterrors) +
		net_stats->tx_aborted_errors +
		net_stats->tx_carrier_errors;

	net_stats->rx_missed_errors =
		GET_32BIT_NET_STATS(stat_IfInFTQDiscards) +
		GET_32BIT_NET_STATS(stat_IfInMBUFDiscards) +
		GET_32BIT_NET_STATS(stat_FwRxDrop);

}

/* All ethtool functions called with rtnl_lock */

static int
bnx2_get_link_ksettings(struct net_device *dev,
			struct ethtool_link_ksettings *cmd)
{
	struct bnx2 *bp = netdev_priv(dev);
	int support_serdes = 0, support_copper = 0;
	u32 supported, advertising;

	supported = SUPPORTED_Autoneg;
	if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP) {
		support_serdes = 1;
		support_copper = 1;
	} else if (bp->phy_port == PORT_FIBRE)
		support_serdes = 1;
	else
		support_copper = 1;

	if (support_serdes) {
		supported |= SUPPORTED_1000baseT_Full |
			SUPPORTED_FIBRE;
		if (bp->phy_flags & BNX2_PHY_FLAG_2_5G_CAPABLE)
			supported |= SUPPORTED_2500baseX_Full;
	}
	if (support_copper) {
		supported |= SUPPORTED_10baseT_Half |
			SUPPORTED_10baseT_Full |
			SUPPORTED_100baseT_Half |
			SUPPORTED_100baseT_Full |
			SUPPORTED_1000baseT_Full |
			SUPPORTED_TP;
	}

	spin_lock_bh(&bp->phy_lock);
	cmd->base.port = bp->phy_port;
	advertising = bp->advertising;

	if (bp->autoneg & AUTONEG_SPEED) {
		cmd->base.autoneg = AUTONEG_ENABLE;
	} else {
		cmd->base.autoneg = AUTONEG_DISABLE;
	}

	if (netif_carrier_ok(dev)) {
		cmd->base.speed = bp->line_speed;
		cmd->base.duplex = bp->duplex;
		if (!(bp->phy_flags & BNX2_PHY_FLAG_SERDES)) {
			if (bp->phy_flags & BNX2_PHY_FLAG_MDIX)
				cmd->base.eth_tp_mdix = ETH_TP_MDI_X;
			else
				cmd->base.eth_tp_mdix = ETH_TP_MDI;
		}
	}
	else {
		cmd->base.speed = SPEED_UNKNOWN;
		cmd->base.duplex = DUPLEX_UNKNOWN;
	}
	spin_unlock_bh(&bp->phy_lock);

	cmd->base.phy_address = bp->phy_addr;

	ethtool_convert_legacy_u32_to_link_mode(cmd->link_modes.supported,
						supported);
	ethtool_convert_legacy_u32_to_link_mode(cmd->link_modes.advertising,
						advertising);

	return 0;
}

static int
bnx2_set_link_ksettings(struct net_device *dev,
			const struct ethtool_link_ksettings *cmd)
{
	struct bnx2 *bp = netdev_priv(dev);
	u8 autoneg = bp->autoneg;
	u8 req_duplex = bp->req_duplex;
	u16 req_line_speed = bp->req_line_speed;
	u32 advertising = bp->advertising;
	int err = -EINVAL;

	spin_lock_bh(&bp->phy_lock);

	if (cmd->base.port != PORT_TP && cmd->base.port != PORT_FIBRE)
		goto err_out_unlock;

	if (cmd->base.port != bp->phy_port &&
	    !(bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP))
		goto err_out_unlock;

	/* If device is down, we can store the settings only if the user
	 * is setting the currently active port.
	 */
	if (!netif_running(dev) && cmd->base.port != bp->phy_port)
		goto err_out_unlock;

	if (cmd->base.autoneg == AUTONEG_ENABLE) {
		autoneg |= AUTONEG_SPEED;

		ethtool_convert_link_mode_to_legacy_u32(
			&advertising, cmd->link_modes.advertising);

		if (cmd->base.port == PORT_TP) {
			advertising &= ETHTOOL_ALL_COPPER_SPEED;
			if (!advertising)
				advertising = ETHTOOL_ALL_COPPER_SPEED;
		} else {
			advertising &= ETHTOOL_ALL_FIBRE_SPEED;
			if (!advertising)
				advertising = ETHTOOL_ALL_FIBRE_SPEED;
		}
		advertising |= ADVERTISED_Autoneg;
	}
	else {
		u32 speed = cmd->base.speed;

		if (cmd->base.port == PORT_FIBRE) {
			if ((speed != SPEED_1000 &&
			     speed != SPEED_2500) ||
			    (cmd->base.duplex != DUPLEX_FULL))
				goto err_out_unlock;

			if (speed == SPEED_2500 &&
			    !(bp->phy_flags & BNX2_PHY_FLAG_2_5G_CAPABLE))
				goto err_out_unlock;
		} else if (speed == SPEED_1000 || speed == SPEED_2500)
			goto err_out_unlock;

		autoneg &= ~AUTONEG_SPEED;
		req_line_speed = speed;
		req_duplex = cmd->base.duplex;
		advertising = 0;
	}

	bp->autoneg = autoneg;
	bp->advertising = advertising;
	bp->req_line_speed = req_line_speed;
	bp->req_duplex = req_duplex;

	err = 0;
	/* If device is down, the new settings will be picked up when it is
	 * brought up.
	 */
	if (netif_running(dev))
		err = bnx2_setup_phy(bp, cmd->base.port);

err_out_unlock:
	spin_unlock_bh(&bp->phy_lock);

	return err;
}

static void
bnx2_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct bnx2 *bp = netdev_priv(dev);

	strlcpy(info->driver, DRV_MODULE_NAME, sizeof(info->driver));
	strlcpy(info->bus_info, pci_name(bp->pdev), sizeof(info->bus_info));
	strlcpy(info->fw_version, bp->fw_version, sizeof(info->fw_version));
}

#define BNX2_REGDUMP_LEN		(32 * 1024)

static int
bnx2_get_regs_len(struct net_device *dev)
{
	return BNX2_REGDUMP_LEN;
}

static void
bnx2_get_regs(struct net_device *dev, struct ethtool_regs *regs, void *_p)
{
	u32 *p = _p, i, offset;
	u8 *orig_p = _p;
	struct bnx2 *bp = netdev_priv(dev);
	static const u32 reg_boundaries[] = {
		0x0000, 0x0098, 0x0400, 0x045c,
		0x0800, 0x0880, 0x0c00, 0x0c10,
		0x0c30, 0x0d08, 0x1000, 0x101c,
		0x1040, 0x1048, 0x1080, 0x10a4,
		0x1400, 0x1490, 0x1498, 0x14f0,
		0x1500, 0x155c, 0x1580, 0x15dc,
		0x1600, 0x1658, 0x1680, 0x16d8,
		0x1800, 0x1820, 0x1840, 0x1854,
		0x1880, 0x1894, 0x1900, 0x1984,
		0x1c00, 0x1c0c, 0x1c40, 0x1c54,
		0x1c80, 0x1c94, 0x1d00, 0x1d84,
		0x2000, 0x2030, 0x23c0, 0x2400,
		0x2800, 0x2820, 0x2830, 0x2850,
		0x2b40, 0x2c10, 0x2fc0, 0x3058,
		0x3c00, 0x3c94, 0x4000, 0x4010,
		0x4080, 0x4090, 0x43c0, 0x4458,
		0x4c00, 0x4c18, 0x4c40, 0x4c54,
		0x4fc0, 0x5010, 0x53c0, 0x5444,
		0x5c00, 0x5c18, 0x5c80, 0x5c90,
		0x5fc0, 0x6000, 0x6400, 0x6428,
		0x6800, 0x6848, 0x684c, 0x6860,
		0x6888, 0x6910, 0x8000
	};

	regs->version = 0;

	memset(p, 0, BNX2_REGDUMP_LEN);

	if (!netif_running(bp->dev))
		return;

	i = 0;
	offset = reg_boundaries[0];
	p += offset;
	while (offset < BNX2_REGDUMP_LEN) {
		*p++ = BNX2_RD(bp, offset);
		offset += 4;
		if (offset == reg_boundaries[i + 1]) {
			offset = reg_boundaries[i + 2];
			p = (u32 *) (orig_p + offset);
			i += 2;
		}
	}
}

static void
bnx2_get_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct bnx2 *bp = netdev_priv(dev);

	if (bp->flags & BNX2_FLAG_NO_WOL) {
		wol->supported = 0;
		wol->wolopts = 0;
	}
	else {
		wol->supported = WAKE_MAGIC;
		if (bp->wol)
			wol->wolopts = WAKE_MAGIC;
		else
			wol->wolopts = 0;
	}
	memset(&wol->sopass, 0, sizeof(wol->sopass));
}

static int
bnx2_set_wol(struct net_device *dev, struct ethtool_wolinfo *wol)
{
	struct bnx2 *bp = netdev_priv(dev);

	if (wol->wolopts & ~WAKE_MAGIC)
		return -EINVAL;

	if (wol->wolopts & WAKE_MAGIC) {
		if (bp->flags & BNX2_FLAG_NO_WOL)
			return -EINVAL;

		bp->wol = 1;
	}
	else {
		bp->wol = 0;
	}

	device_set_wakeup_enable(&bp->pdev->dev, bp->wol);

	return 0;
}

static int
bnx2_nway_reset(struct net_device *dev)
{
	struct bnx2 *bp = netdev_priv(dev);
	u32 bmcr;

	if (!netif_running(dev))
		return -EAGAIN;

	if (!(bp->autoneg & AUTONEG_SPEED)) {
		return -EINVAL;
	}

	spin_lock_bh(&bp->phy_lock);

	if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP) {
		int rc;

		rc = bnx2_setup_remote_phy(bp, bp->phy_port);
		spin_unlock_bh(&bp->phy_lock);
		return rc;
	}

	/* Force a link down visible on the other side */
	if (bp->phy_flags & BNX2_PHY_FLAG_SERDES) {
		bnx2_write_phy(bp, bp->mii_bmcr, BMCR_LOOPBACK);
		spin_unlock_bh(&bp->phy_lock);

		msleep(20);

		spin_lock_bh(&bp->phy_lock);

		bp->current_interval = BNX2_SERDES_AN_TIMEOUT;
		bp->serdes_an_pending = 1;
		mod_timer(&bp->timer, jiffies + bp->current_interval);
	}

	bnx2_read_phy(bp, bp->mii_bmcr, &bmcr);
	bmcr &= ~BMCR_LOOPBACK;
	bnx2_write_phy(bp, bp->mii_bmcr, bmcr | BMCR_ANRESTART | BMCR_ANENABLE);

	spin_unlock_bh(&bp->phy_lock);

	return 0;
}

static u32
bnx2_get_link(struct net_device *dev)
{
	struct bnx2 *bp = netdev_priv(dev);

	return bp->link_up;
}

static int
bnx2_get_eeprom_len(struct net_device *dev)
{
	struct bnx2 *bp = netdev_priv(dev);

	if (!bp->flash_info)
		return 0;

	return (int) bp->flash_size;
}

static int
bnx2_get_eeprom(struct net_device *dev, struct ethtool_eeprom *eeprom,
		u8 *eebuf)
{
	struct bnx2 *bp = netdev_priv(dev);
	int rc;

	/* parameters already validated in ethtool_get_eeprom */

	rc = bnx2_nvram_read(bp, eeprom->offset, eebuf, eeprom->len);

	return rc;
}

static int
bnx2_set_eeprom(struct net_device *dev, struct ethtool_eeprom *eeprom,
		u8 *eebuf)
{
	struct bnx2 *bp = netdev_priv(dev);
	int rc;

	/* parameters already validated in ethtool_set_eeprom */

	rc = bnx2_nvram_write(bp, eeprom->offset, eebuf, eeprom->len);

	return rc;
}

static int
bnx2_get_coalesce(struct net_device *dev, struct ethtool_coalesce *coal)
{
	struct bnx2 *bp = netdev_priv(dev);

	memset(coal, 0, sizeof(struct ethtool_coalesce));

	coal->rx_coalesce_usecs = bp->rx_ticks;
	coal->rx_max_coalesced_frames = bp->rx_quick_cons_trip;
	coal->rx_coalesce_usecs_irq = bp->rx_ticks_int;
	coal->rx_max_coalesced_frames_irq = bp->rx_quick_cons_trip_int;

	coal->tx_coalesce_usecs = bp->tx_ticks;
	coal->tx_max_coalesced_frames = bp->tx_quick_cons_trip;
	coal->tx_coalesce_usecs_irq = bp->tx_ticks_int;
	coal->tx_max_coalesced_frames_irq = bp->tx_quick_cons_trip_int;

	coal->stats_block_coalesce_usecs = bp->stats_ticks;

	return 0;
}

static int
bnx2_set_coalesce(struct net_device *dev, struct ethtool_coalesce *coal)
{
	struct bnx2 *bp = netdev_priv(dev);

	bp->rx_ticks = (u16) coal->rx_coalesce_usecs;
	if (bp->rx_ticks > 0x3ff) bp->rx_ticks = 0x3ff;

	bp->rx_quick_cons_trip = (u16) coal->rx_max_coalesced_frames;
	if (bp->rx_quick_cons_trip > 0xff) bp->rx_quick_cons_trip = 0xff;

	bp->rx_ticks_int = (u16) coal->rx_coalesce_usecs_irq;
	if (bp->rx_ticks_int > 0x3ff) bp->rx_ticks_int = 0x3ff;

	bp->rx_quick_cons_trip_int = (u16) coal->rx_max_coalesced_frames_irq;
	if (bp->rx_quick_cons_trip_int > 0xff)
		bp->rx_quick_cons_trip_int = 0xff;

	bp->tx_ticks = (u16) coal->tx_coalesce_usecs;
	if (bp->tx_ticks > 0x3ff) bp->tx_ticks = 0x3ff;

	bp->tx_quick_cons_trip = (u16) coal->tx_max_coalesced_frames;
	if (bp->tx_quick_cons_trip > 0xff) bp->tx_quick_cons_trip = 0xff;

	bp->tx_ticks_int = (u16) coal->tx_coalesce_usecs_irq;
	if (bp->tx_ticks_int > 0x3ff) bp->tx_ticks_int = 0x3ff;

	bp->tx_quick_cons_trip_int = (u16) coal->tx_max_coalesced_frames_irq;
	if (bp->tx_quick_cons_trip_int > 0xff) bp->tx_quick_cons_trip_int =
		0xff;

	bp->stats_ticks = coal->stats_block_coalesce_usecs;
	if (bp->flags & BNX2_FLAG_BROKEN_STATS) {
		if (bp->stats_ticks != 0 && bp->stats_ticks != USEC_PER_SEC)
			bp->stats_ticks = USEC_PER_SEC;
	}
	if (bp->stats_ticks > BNX2_HC_STATS_TICKS_HC_STAT_TICKS)
		bp->stats_ticks = BNX2_HC_STATS_TICKS_HC_STAT_TICKS;
	bp->stats_ticks &= BNX2_HC_STATS_TICKS_HC_STAT_TICKS;

	if (netif_running(bp->dev)) {
		bnx2_netif_stop(bp, true);
		bnx2_init_nic(bp, 0);
		bnx2_netif_start(bp, true);
	}

	return 0;
}

static void
bnx2_get_ringparam(struct net_device *dev, struct ethtool_ringparam *ering)
{
	struct bnx2 *bp = netdev_priv(dev);

	ering->rx_max_pending = BNX2_MAX_TOTAL_RX_DESC_CNT;
	ering->rx_jumbo_max_pending = BNX2_MAX_TOTAL_RX_PG_DESC_CNT;

	ering->rx_pending = bp->rx_ring_size;
	ering->rx_jumbo_pending = bp->rx_pg_ring_size;

	ering->tx_max_pending = BNX2_MAX_TX_DESC_CNT;
	ering->tx_pending = bp->tx_ring_size;
}

static int
bnx2_change_ring_size(struct bnx2 *bp, u32 rx, u32 tx, bool reset_irq)
{
	if (netif_running(bp->dev)) {
		/* Reset will erase chipset stats; save them */
		bnx2_save_stats(bp);

		bnx2_netif_stop(bp, true);
		bnx2_reset_chip(bp, BNX2_DRV_MSG_CODE_RESET);
		if (reset_irq) {
			bnx2_free_irq(bp);
			bnx2_del_napi(bp);
		} else {
			__bnx2_free_irq(bp);
		}
		bnx2_free_skbs(bp);
		bnx2_free_mem(bp);
	}

	bnx2_set_rx_ring_size(bp, rx);
	bp->tx_ring_size = tx;

	if (netif_running(bp->dev)) {
		int rc = 0;

		if (reset_irq) {
			rc = bnx2_setup_int_mode(bp, disable_msi);
			bnx2_init_napi(bp);
		}

		if (!rc)
			rc = bnx2_alloc_mem(bp);

		if (!rc)
			rc = bnx2_request_irq(bp);

		if (!rc)
			rc = bnx2_init_nic(bp, 0);

		if (rc) {
			bnx2_napi_enable(bp);
			dev_close(bp->dev);
			return rc;
		}
#ifdef BCM_CNIC
		mutex_lock(&bp->cnic_lock);
		/* Let cnic know about the new status block. */
		if (bp->cnic_eth_dev.drv_state & CNIC_DRV_STATE_REGD)
			bnx2_setup_cnic_irq_info(bp);
		mutex_unlock(&bp->cnic_lock);
#endif
		bnx2_netif_start(bp, true);
	}
	return 0;
}

static int
bnx2_set_ringparam(struct net_device *dev, struct ethtool_ringparam *ering)
{
	struct bnx2 *bp = netdev_priv(dev);
	int rc;

	if ((ering->rx_pending > BNX2_MAX_TOTAL_RX_DESC_CNT) ||
		(ering->tx_pending > BNX2_MAX_TX_DESC_CNT) ||
		(ering->tx_pending <= MAX_SKB_FRAGS)) {

		return -EINVAL;
	}
	rc = bnx2_change_ring_size(bp, ering->rx_pending, ering->tx_pending,
				   false);
	return rc;
}

static void
bnx2_get_pauseparam(struct net_device *dev, struct ethtool_pauseparam *epause)
{
	struct bnx2 *bp = netdev_priv(dev);

	epause->autoneg = ((bp->autoneg & AUTONEG_FLOW_CTRL) != 0);
	epause->rx_pause = ((bp->flow_ctrl & FLOW_CTRL_RX) != 0);
	epause->tx_pause = ((bp->flow_ctrl & FLOW_CTRL_TX) != 0);
}

static int
bnx2_set_pauseparam(struct net_device *dev, struct ethtool_pauseparam *epause)
{
	struct bnx2 *bp = netdev_priv(dev);

	bp->req_flow_ctrl = 0;
	if (epause->rx_pause)
		bp->req_flow_ctrl |= FLOW_CTRL_RX;
	if (epause->tx_pause)
		bp->req_flow_ctrl |= FLOW_CTRL_TX;

	if (epause->autoneg) {
		bp->autoneg |= AUTONEG_FLOW_CTRL;
	}
	else {
		bp->autoneg &= ~AUTONEG_FLOW_CTRL;
	}

	if (netif_running(dev)) {
		spin_lock_bh(&bp->phy_lock);
		bnx2_setup_phy(bp, bp->phy_port);
		spin_unlock_bh(&bp->phy_lock);
	}

	return 0;
}

static struct {
	char string[ETH_GSTRING_LEN];
} bnx2_stats_str_arr[] = {
	{ "rx_bytes" },
	{ "rx_error_bytes" },
	{ "tx_bytes" },
	{ "tx_error_bytes" },
	{ "rx_ucast_packets" },
	{ "rx_mcast_packets" },
	{ "rx_bcast_packets" },
	{ "tx_ucast_packets" },
	{ "tx_mcast_packets" },
	{ "tx_bcast_packets" },
	{ "tx_mac_errors" },
	{ "tx_carrier_errors" },
	{ "rx_crc_errors" },
	{ "rx_align_errors" },
	{ "tx_single_collisions" },
	{ "tx_multi_collisions" },
	{ "tx_deferred" },
	{ "tx_excess_collisions" },
	{ "tx_late_collisions" },
	{ "tx_total_collisions" },
	{ "rx_fragments" },
	{ "rx_jabbers" },
	{ "rx_undersize_packets" },
	{ "rx_oversize_packets" },
	{ "rx_64_byte_packets" },
	{ "rx_65_to_127_byte_packets" },
	{ "rx_128_to_255_byte_packets" },
	{ "rx_256_to_511_byte_packets" },
	{ "rx_512_to_1023_byte_packets" },
	{ "rx_1024_to_1522_byte_packets" },
	{ "rx_1523_to_9022_byte_packets" },
	{ "tx_64_byte_packets" },
	{ "tx_65_to_127_byte_packets" },
	{ "tx_128_to_255_byte_packets" },
	{ "tx_256_to_511_byte_packets" },
	{ "tx_512_to_1023_byte_packets" },
	{ "tx_1024_to_1522_byte_packets" },
	{ "tx_1523_to_9022_byte_packets" },
	{ "rx_xon_frames" },
	{ "rx_xoff_frames" },
	{ "tx_xon_frames" },
	{ "tx_xoff_frames" },
	{ "rx_mac_ctrl_frames" },
	{ "rx_filtered_packets" },
	{ "rx_ftq_discards" },
	{ "rx_discards" },
	{ "rx_fw_discards" },
};

#define BNX2_NUM_STATS ARRAY_SIZE(bnx2_stats_str_arr)

#define STATS_OFFSET32(offset_name) (offsetof(struct statistics_block, offset_name) / 4)

static const unsigned long bnx2_stats_offset_arr[BNX2_NUM_STATS] = {
    STATS_OFFSET32(stat_IfHCInOctets_hi),
    STATS_OFFSET32(stat_IfHCInBadOctets_hi),
    STATS_OFFSET32(stat_IfHCOutOctets_hi),
    STATS_OFFSET32(stat_IfHCOutBadOctets_hi),
    STATS_OFFSET32(stat_IfHCInUcastPkts_hi),
    STATS_OFFSET32(stat_IfHCInMulticastPkts_hi),
    STATS_OFFSET32(stat_IfHCInBroadcastPkts_hi),
    STATS_OFFSET32(stat_IfHCOutUcastPkts_hi),
    STATS_OFFSET32(stat_IfHCOutMulticastPkts_hi),
    STATS_OFFSET32(stat_IfHCOutBroadcastPkts_hi),
    STATS_OFFSET32(stat_emac_tx_stat_dot3statsinternalmactransmiterrors),
    STATS_OFFSET32(stat_Dot3StatsCarrierSenseErrors),
    STATS_OFFSET32(stat_Dot3StatsFCSErrors),
    STATS_OFFSET32(stat_Dot3StatsAlignmentErrors),
    STATS_OFFSET32(stat_Dot3StatsSingleCollisionFrames),
    STATS_OFFSET32(stat_Dot3StatsMultipleCollisionFrames),
    STATS_OFFSET32(stat_Dot3StatsDeferredTransmissions),
    STATS_OFFSET32(stat_Dot3StatsExcessiveCollisions),
    STATS_OFFSET32(stat_Dot3StatsLateCollisions),
    STATS_OFFSET32(stat_EtherStatsCollisions),
    STATS_OFFSET32(stat_EtherStatsFragments),
    STATS_OFFSET32(stat_EtherStatsJabbers),
    STATS_OFFSET32(stat_EtherStatsUndersizePkts),
    STATS_OFFSET32(stat_EtherStatsOverrsizePkts),
    STATS_OFFSET32(stat_EtherStatsPktsRx64Octets),
    STATS_OFFSET32(stat_EtherStatsPktsRx65Octetsto127Octets),
    STATS_OFFSET32(stat_EtherStatsPktsRx128Octetsto255Octets),
    STATS_OFFSET32(stat_EtherStatsPktsRx256Octetsto511Octets),
    STATS_OFFSET32(stat_EtherStatsPktsRx512Octetsto1023Octets),
    STATS_OFFSET32(stat_EtherStatsPktsRx1024Octetsto1522Octets),
    STATS_OFFSET32(stat_EtherStatsPktsRx1523Octetsto9022Octets),
    STATS_OFFSET32(stat_EtherStatsPktsTx64Octets),
    STATS_OFFSET32(stat_EtherStatsPktsTx65Octetsto127Octets),
    STATS_OFFSET32(stat_EtherStatsPktsTx128Octetsto255Octets),
    STATS_OFFSET32(stat_EtherStatsPktsTx256Octetsto511Octets),
    STATS_OFFSET32(stat_EtherStatsPktsTx512Octetsto1023Octets),
    STATS_OFFSET32(stat_EtherStatsPktsTx1024Octetsto1522Octets),
    STATS_OFFSET32(stat_EtherStatsPktsTx1523Octetsto9022Octets),
    STATS_OFFSET32(stat_XonPauseFramesReceived),
    STATS_OFFSET32(stat_XoffPauseFramesReceived),
    STATS_OFFSET32(stat_OutXonSent),
    STATS_OFFSET32(stat_OutXoffSent),
    STATS_OFFSET32(stat_MacControlFramesReceived),
    STATS_OFFSET32(stat_IfInFramesL2FilterDiscards),
    STATS_OFFSET32(stat_IfInFTQDiscards),
    STATS_OFFSET32(stat_IfInMBUFDiscards),
    STATS_OFFSET32(stat_FwRxDrop),
};

/* stat_IfHCInBadOctets and stat_Dot3StatsCarrierSenseErrors are
 * skipped because of errata.
 */
static u8 bnx2_5706_stats_len_arr[BNX2_NUM_STATS] = {
	8,0,8,8,8,8,8,8,8,8,
	4,0,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,
};

static u8 bnx2_5708_stats_len_arr[BNX2_NUM_STATS] = {
	8,0,8,8,8,8,8,8,8,8,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,4,4,4,
	4,4,4,4,4,4,4,
};

#define BNX2_NUM_TESTS 6

static struct {
	char string[ETH_GSTRING_LEN];
} bnx2_tests_str_arr[BNX2_NUM_TESTS] = {
	{ "register_test (offline)" },
	{ "memory_test (offline)" },
	{ "loopback_test (offline)" },
	{ "nvram_test (online)" },
	{ "interrupt_test (online)" },
	{ "link_test (online)" },
};

static int
bnx2_get_sset_count(struct net_device *dev, int sset)
{
	switch (sset) {
	case ETH_SS_TEST:
		return BNX2_NUM_TESTS;
	case ETH_SS_STATS:
		return BNX2_NUM_STATS;
	default:
		return -EOPNOTSUPP;
	}
}

static void
bnx2_self_test(struct net_device *dev, struct ethtool_test *etest, u64 *buf)
{
	struct bnx2 *bp = netdev_priv(dev);

	memset(buf, 0, sizeof(u64) * BNX2_NUM_TESTS);
	if (etest->flags & ETH_TEST_FL_OFFLINE) {
		int i;

		bnx2_netif_stop(bp, true);
		bnx2_reset_chip(bp, BNX2_DRV_MSG_CODE_DIAG);
		bnx2_free_skbs(bp);

		if (bnx2_test_registers(bp) != 0) {
			buf[0] = 1;
			etest->flags |= ETH_TEST_FL_FAILED;
		}
		if (bnx2_test_memory(bp) != 0) {
			buf[1] = 1;
			etest->flags |= ETH_TEST_FL_FAILED;
		}
		if ((buf[2] = bnx2_test_loopback(bp)) != 0)
			etest->flags |= ETH_TEST_FL_FAILED;

		if (!netif_running(bp->dev))
			bnx2_shutdown_chip(bp);
		else {
			bnx2_init_nic(bp, 1);
			bnx2_netif_start(bp, true);
		}

		/* wait for link up */
		for (i = 0; i < 7; i++) {
			if (bp->link_up)
				break;
			msleep_interruptible(1000);
		}
	}

	if (bnx2_test_nvram(bp) != 0) {
		buf[3] = 1;
		etest->flags |= ETH_TEST_FL_FAILED;
	}
	if (bnx2_test_intr(bp) != 0) {
		buf[4] = 1;
		etest->flags |= ETH_TEST_FL_FAILED;
	}

	if (bnx2_test_link(bp) != 0) {
		buf[5] = 1;
		etest->flags |= ETH_TEST_FL_FAILED;

	}
}

static void
bnx2_get_strings(struct net_device *dev, u32 stringset, u8 *buf)
{
	switch (stringset) {
	case ETH_SS_STATS:
		memcpy(buf, bnx2_stats_str_arr,
			sizeof(bnx2_stats_str_arr));
		break;
	case ETH_SS_TEST:
		memcpy(buf, bnx2_tests_str_arr,
			sizeof(bnx2_tests_str_arr));
		break;
	}
}

static void
bnx2_get_ethtool_stats(struct net_device *dev,
		struct ethtool_stats *stats, u64 *buf)
{
	struct bnx2 *bp = netdev_priv(dev);
	int i;
	u32 *hw_stats = (u32 *) bp->stats_blk;
	u32 *temp_stats = (u32 *) bp->temp_stats_blk;
	u8 *stats_len_arr = NULL;

	if (!hw_stats) {
		memset(buf, 0, sizeof(u64) * BNX2_NUM_STATS);
		return;
	}

	if ((BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A0) ||
	    (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A1) ||
	    (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A2) ||
	    (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5708_A0))
		stats_len_arr = bnx2_5706_stats_len_arr;
	else
		stats_len_arr = bnx2_5708_stats_len_arr;

	for (i = 0; i < BNX2_NUM_STATS; i++) {
		unsigned long offset;

		if (stats_len_arr[i] == 0) {
			/* skip this counter */
			buf[i] = 0;
			continue;
		}

		offset = bnx2_stats_offset_arr[i];
		if (stats_len_arr[i] == 4) {
			/* 4-byte counter */
			buf[i] = (u64) *(hw_stats + offset) +
				 *(temp_stats + offset);
			continue;
		}
		/* 8-byte counter */
		buf[i] = (((u64) *(hw_stats + offset)) << 32) +
			 *(hw_stats + offset + 1) +
			 (((u64) *(temp_stats + offset)) << 32) +
			 *(temp_stats + offset + 1);
	}
}

static int
bnx2_set_phys_id(struct net_device *dev, enum ethtool_phys_id_state state)
{
	struct bnx2 *bp = netdev_priv(dev);

	switch (state) {
	case ETHTOOL_ID_ACTIVE:
		bp->leds_save = BNX2_RD(bp, BNX2_MISC_CFG);
		BNX2_WR(bp, BNX2_MISC_CFG, BNX2_MISC_CFG_LEDMODE_MAC);
		return 1;	/* cycle on/off once per second */

	case ETHTOOL_ID_ON:
		BNX2_WR(bp, BNX2_EMAC_LED, BNX2_EMAC_LED_OVERRIDE |
			BNX2_EMAC_LED_1000MB_OVERRIDE |
			BNX2_EMAC_LED_100MB_OVERRIDE |
			BNX2_EMAC_LED_10MB_OVERRIDE |
			BNX2_EMAC_LED_TRAFFIC_OVERRIDE |
			BNX2_EMAC_LED_TRAFFIC);
		break;

	case ETHTOOL_ID_OFF:
		BNX2_WR(bp, BNX2_EMAC_LED, BNX2_EMAC_LED_OVERRIDE);
		break;

	case ETHTOOL_ID_INACTIVE:
		BNX2_WR(bp, BNX2_EMAC_LED, 0);
		BNX2_WR(bp, BNX2_MISC_CFG, bp->leds_save);
		break;
	}

	return 0;
}

static int
bnx2_set_features(struct net_device *dev, netdev_features_t features)
{
	struct bnx2 *bp = netdev_priv(dev);

	/* TSO with VLAN tag won't work with current firmware */
	if (features & NETIF_F_HW_VLAN_CTAG_TX)
		dev->vlan_features |= (dev->hw_features & NETIF_F_ALL_TSO);
	else
		dev->vlan_features &= ~NETIF_F_ALL_TSO;

	if ((!!(features & NETIF_F_HW_VLAN_CTAG_RX) !=
	    !!(bp->rx_mode & BNX2_EMAC_RX_MODE_KEEP_VLAN_TAG)) &&
	    netif_running(dev)) {
		bnx2_netif_stop(bp, false);
		dev->features = features;
		bnx2_set_rx_mode(dev);
		bnx2_fw_sync(bp, BNX2_DRV_MSG_CODE_KEEP_VLAN_UPDATE, 0, 1);
		bnx2_netif_start(bp, false);
		return 1;
	}

	return 0;
}

static void bnx2_get_channels(struct net_device *dev,
			      struct ethtool_channels *channels)
{
	struct bnx2 *bp = netdev_priv(dev);
	u32 max_rx_rings = 1;
	u32 max_tx_rings = 1;

	if ((bp->flags & BNX2_FLAG_MSIX_CAP) && !disable_msi) {
		max_rx_rings = RX_MAX_RINGS;
		max_tx_rings = TX_MAX_RINGS;
	}

	channels->max_rx = max_rx_rings;
	channels->max_tx = max_tx_rings;
	channels->max_other = 0;
	channels->max_combined = 0;
	channels->rx_count = bp->num_rx_rings;
	channels->tx_count = bp->num_tx_rings;
	channels->other_count = 0;
	channels->combined_count = 0;
}

static int bnx2_set_channels(struct net_device *dev,
			      struct ethtool_channels *channels)
{
	struct bnx2 *bp = netdev_priv(dev);
	u32 max_rx_rings = 1;
	u32 max_tx_rings = 1;
	int rc = 0;

	if ((bp->flags & BNX2_FLAG_MSIX_CAP) && !disable_msi) {
		max_rx_rings = RX_MAX_RINGS;
		max_tx_rings = TX_MAX_RINGS;
	}
	if (channels->rx_count > max_rx_rings ||
	    channels->tx_count > max_tx_rings)
		return -EINVAL;

	bp->num_req_rx_rings = channels->rx_count;
	bp->num_req_tx_rings = channels->tx_count;

	if (netif_running(dev))
		rc = bnx2_change_ring_size(bp, bp->rx_ring_size,
					   bp->tx_ring_size, true);

	return rc;
}

static const struct ethtool_ops bnx2_ethtool_ops = {
	.supported_coalesce_params = ETHTOOL_COALESCE_USECS |
				     ETHTOOL_COALESCE_MAX_FRAMES |
				     ETHTOOL_COALESCE_USECS_IRQ |
				     ETHTOOL_COALESCE_MAX_FRAMES_IRQ |
				     ETHTOOL_COALESCE_STATS_BLOCK_USECS,
	.get_drvinfo		= bnx2_get_drvinfo,
	.get_regs_len		= bnx2_get_regs_len,
	.get_regs		= bnx2_get_regs,
	.get_wol		= bnx2_get_wol,
	.set_wol		= bnx2_set_wol,
	.nway_reset		= bnx2_nway_reset,
	.get_link		= bnx2_get_link,
	.get_eeprom_len		= bnx2_get_eeprom_len,
	.get_eeprom		= bnx2_get_eeprom,
	.set_eeprom		= bnx2_set_eeprom,
	.get_coalesce		= bnx2_get_coalesce,
	.set_coalesce		= bnx2_set_coalesce,
	.get_ringparam		= bnx2_get_ringparam,
	.set_ringparam		= bnx2_set_ringparam,
	.get_pauseparam		= bnx2_get_pauseparam,
	.set_pauseparam		= bnx2_set_pauseparam,
	.self_test		= bnx2_self_test,
	.get_strings		= bnx2_get_strings,
	.set_phys_id		= bnx2_set_phys_id,
	.get_ethtool_stats	= bnx2_get_ethtool_stats,
	.get_sset_count		= bnx2_get_sset_count,
	.get_channels		= bnx2_get_channels,
	.set_channels		= bnx2_set_channels,
	.get_link_ksettings	= bnx2_get_link_ksettings,
	.set_link_ksettings	= bnx2_set_link_ksettings,
};

/* Called with rtnl_lock */
static int
bnx2_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct mii_ioctl_data *data = if_mii(ifr);
	struct bnx2 *bp = netdev_priv(dev);
	int err;

	switch(cmd) {
	case SIOCGMIIPHY:
		data->phy_id = bp->phy_addr;

		fallthrough;
	case SIOCGMIIREG: {
		u32 mii_regval;

		if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP)
			return -EOPNOTSUPP;

		if (!netif_running(dev))
			return -EAGAIN;

		spin_lock_bh(&bp->phy_lock);
		err = bnx2_read_phy(bp, data->reg_num & 0x1f, &mii_regval);
		spin_unlock_bh(&bp->phy_lock);

		data->val_out = mii_regval;

		return err;
	}

	case SIOCSMIIREG:
		if (bp->phy_flags & BNX2_PHY_FLAG_REMOTE_PHY_CAP)
			return -EOPNOTSUPP;

		if (!netif_running(dev))
			return -EAGAIN;

		spin_lock_bh(&bp->phy_lock);
		err = bnx2_write_phy(bp, data->reg_num & 0x1f, data->val_in);
		spin_unlock_bh(&bp->phy_lock);

		return err;

	default:
		/* do nothing */
		break;
	}
	return -EOPNOTSUPP;
}

/* Called with rtnl_lock */
static int
bnx2_change_mac_addr(struct net_device *dev, void *p)
{
	struct sockaddr *addr = p;
	struct bnx2 *bp = netdev_priv(dev);

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
	if (netif_running(dev))
		bnx2_set_mac_addr(bp, bp->dev->dev_addr, 0);

	return 0;
}

/* Called with rtnl_lock */
static int
bnx2_change_mtu(struct net_device *dev, int new_mtu)
{
	struct bnx2 *bp = netdev_priv(dev);

	dev->mtu = new_mtu;
	return bnx2_change_ring_size(bp, bp->rx_ring_size, bp->tx_ring_size,
				     false);
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void
poll_bnx2(struct net_device *dev)
{
	struct bnx2 *bp = netdev_priv(dev);
	int i;

	for (i = 0; i < bp->irq_nvecs; i++) {
		struct bnx2_irq *irq = &bp->irq_tbl[i];

		disable_irq(irq->vector);
		irq->handler(irq->vector, &bp->bnx2_napi[i]);
		enable_irq(irq->vector);
	}
}
#endif

static void
bnx2_get_5709_media(struct bnx2 *bp)
{
	u32 val = BNX2_RD(bp, BNX2_MISC_DUAL_MEDIA_CTRL);
	u32 bond_id = val & BNX2_MISC_DUAL_MEDIA_CTRL_BOND_ID;
	u32 strap;

	if (bond_id == BNX2_MISC_DUAL_MEDIA_CTRL_BOND_ID_C)
		return;
	else if (bond_id == BNX2_MISC_DUAL_MEDIA_CTRL_BOND_ID_S) {
		bp->phy_flags |= BNX2_PHY_FLAG_SERDES;
		return;
	}

	if (val & BNX2_MISC_DUAL_MEDIA_CTRL_STRAP_OVERRIDE)
		strap = (val & BNX2_MISC_DUAL_MEDIA_CTRL_PHY_CTRL) >> 21;
	else
		strap = (val & BNX2_MISC_DUAL_MEDIA_CTRL_PHY_CTRL_STRAP) >> 8;

	if (bp->func == 0) {
		switch (strap) {
		case 0x4:
		case 0x5:
		case 0x6:
			bp->phy_flags |= BNX2_PHY_FLAG_SERDES;
			return;
		}
	} else {
		switch (strap) {
		case 0x1:
		case 0x2:
		case 0x4:
			bp->phy_flags |= BNX2_PHY_FLAG_SERDES;
			return;
		}
	}
}

static void
bnx2_get_pci_speed(struct bnx2 *bp)
{
	u32 reg;

	reg = BNX2_RD(bp, BNX2_PCICFG_MISC_STATUS);
	if (reg & BNX2_PCICFG_MISC_STATUS_PCIX_DET) {
		u32 clkreg;

		bp->flags |= BNX2_FLAG_PCIX;

		clkreg = BNX2_RD(bp, BNX2_PCICFG_PCI_CLOCK_CONTROL_BITS);

		clkreg &= BNX2_PCICFG_PCI_CLOCK_CONTROL_BITS_PCI_CLK_SPD_DET;
		switch (clkreg) {
		case BNX2_PCICFG_PCI_CLOCK_CONTROL_BITS_PCI_CLK_SPD_DET_133MHZ:
			bp->bus_speed_mhz = 133;
			break;

		case BNX2_PCICFG_PCI_CLOCK_CONTROL_BITS_PCI_CLK_SPD_DET_95MHZ:
			bp->bus_speed_mhz = 100;
			break;

		case BNX2_PCICFG_PCI_CLOCK_CONTROL_BITS_PCI_CLK_SPD_DET_66MHZ:
		case BNX2_PCICFG_PCI_CLOCK_CONTROL_BITS_PCI_CLK_SPD_DET_80MHZ:
			bp->bus_speed_mhz = 66;
			break;

		case BNX2_PCICFG_PCI_CLOCK_CONTROL_BITS_PCI_CLK_SPD_DET_48MHZ:
		case BNX2_PCICFG_PCI_CLOCK_CONTROL_BITS_PCI_CLK_SPD_DET_55MHZ:
			bp->bus_speed_mhz = 50;
			break;

		case BNX2_PCICFG_PCI_CLOCK_CONTROL_BITS_PCI_CLK_SPD_DET_LOW:
		case BNX2_PCICFG_PCI_CLOCK_CONTROL_BITS_PCI_CLK_SPD_DET_32MHZ:
		case BNX2_PCICFG_PCI_CLOCK_CONTROL_BITS_PCI_CLK_SPD_DET_38MHZ:
			bp->bus_speed_mhz = 33;
			break;
		}
	}
	else {
		if (reg & BNX2_PCICFG_MISC_STATUS_M66EN)
			bp->bus_speed_mhz = 66;
		else
			bp->bus_speed_mhz = 33;
	}

	if (reg & BNX2_PCICFG_MISC_STATUS_32BIT_DET)
		bp->flags |= BNX2_FLAG_PCI_32BIT;

}

static void
bnx2_read_vpd_fw_ver(struct bnx2 *bp)
{
	int rc, i, j;
	u8 *data;
	unsigned int block_end, rosize, len;

#define BNX2_VPD_NVRAM_OFFSET	0x300
#define BNX2_VPD_LEN		128
#define BNX2_MAX_VER_SLEN	30

	data = kmalloc(256, GFP_KERNEL);
	if (!data)
		return;

	rc = bnx2_nvram_read(bp, BNX2_VPD_NVRAM_OFFSET, data + BNX2_VPD_LEN,
			     BNX2_VPD_LEN);
	if (rc)
		goto vpd_done;

	for (i = 0; i < BNX2_VPD_LEN; i += 4) {
		data[i] = data[i + BNX2_VPD_LEN + 3];
		data[i + 1] = data[i + BNX2_VPD_LEN + 2];
		data[i + 2] = data[i + BNX2_VPD_LEN + 1];
		data[i + 3] = data[i + BNX2_VPD_LEN];
	}

	i = pci_vpd_find_tag(data, BNX2_VPD_LEN, PCI_VPD_LRDT_RO_DATA);
	if (i < 0)
		goto vpd_done;

	rosize = pci_vpd_lrdt_size(&data[i]);
	i += PCI_VPD_LRDT_TAG_SIZE;
	block_end = i + rosize;

	if (block_end > BNX2_VPD_LEN)
		goto vpd_done;

	j = pci_vpd_find_info_keyword(data, i, rosize,
				      PCI_VPD_RO_KEYWORD_MFR_ID);
	if (j < 0)
		goto vpd_done;

	len = pci_vpd_info_field_size(&data[j]);

	j += PCI_VPD_INFO_FLD_HDR_SIZE;
	if (j + len > block_end || len != 4 ||
	    memcmp(&data[j], "1028", 4))
		goto vpd_done;

	j = pci_vpd_find_info_keyword(data, i, rosize,
				      PCI_VPD_RO_KEYWORD_VENDOR0);
	if (j < 0)
		goto vpd_done;

	len = pci_vpd_info_field_size(&data[j]);

	j += PCI_VPD_INFO_FLD_HDR_SIZE;
	if (j + len > block_end || len > BNX2_MAX_VER_SLEN)
		goto vpd_done;

	memcpy(bp->fw_version, &data[j], len);
	bp->fw_version[len] = ' ';

vpd_done:
	kfree(data);
}

static int
bnx2_init_board(struct pci_dev *pdev, struct net_device *dev)
{
	struct bnx2 *bp;
	int rc, i, j;
	u32 reg;
	u64 dma_mask, persist_dma_mask;
	int err;

	SET_NETDEV_DEV(dev, &pdev->dev);
	bp = netdev_priv(dev);

	bp->flags = 0;
	bp->phy_flags = 0;

	bp->temp_stats_blk =
		kzalloc(sizeof(struct statistics_block), GFP_KERNEL);

	if (!bp->temp_stats_blk) {
		rc = -ENOMEM;
		goto err_out;
	}

	/* enable device (incl. PCI PM wakeup), and bus-mastering */
	rc = pci_enable_device(pdev);
	if (rc) {
		dev_err(&pdev->dev, "Cannot enable PCI device, aborting\n");
		goto err_out;
	}

	if (!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM)) {
		dev_err(&pdev->dev,
			"Cannot find PCI device base address, aborting\n");
		rc = -ENODEV;
		goto err_out_disable;
	}

	rc = pci_request_regions(pdev, DRV_MODULE_NAME);
	if (rc) {
		dev_err(&pdev->dev, "Cannot obtain PCI resources, aborting\n");
		goto err_out_disable;
	}

	pci_set_master(pdev);

	bp->pm_cap = pdev->pm_cap;
	if (bp->pm_cap == 0) {
		dev_err(&pdev->dev,
			"Cannot find power management capability, aborting\n");
		rc = -EIO;
		goto err_out_release;
	}

	bp->dev = dev;
	bp->pdev = pdev;

	spin_lock_init(&bp->phy_lock);
	spin_lock_init(&bp->indirect_lock);
#ifdef BCM_CNIC
	mutex_init(&bp->cnic_lock);
#endif
	INIT_WORK(&bp->reset_task, bnx2_reset_task);

	bp->regview = pci_iomap(pdev, 0, MB_GET_CID_ADDR(TX_TSS_CID +
							 TX_MAX_TSS_RINGS + 1));
	if (!bp->regview) {
		dev_err(&pdev->dev, "Cannot map register space, aborting\n");
		rc = -ENOMEM;
		goto err_out_release;
	}

	/* Configure byte swap and enable write to the reg_window registers.
	 * Rely on CPU to do target byte swapping on big endian systems
	 * The chip's target access swapping will not swap all accesses
	 */
	BNX2_WR(bp, BNX2_PCICFG_MISC_CONFIG,
		BNX2_PCICFG_MISC_CONFIG_REG_WINDOW_ENA |
		BNX2_PCICFG_MISC_CONFIG_TARGET_MB_WORD_SWAP);

	bp->chip_id = BNX2_RD(bp, BNX2_MISC_ID);

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709) {
		if (!pci_is_pcie(pdev)) {
			dev_err(&pdev->dev, "Not PCIE, aborting\n");
			rc = -EIO;
			goto err_out_unmap;
		}
		bp->flags |= BNX2_FLAG_PCIE;
		if (BNX2_CHIP_REV(bp) == BNX2_CHIP_REV_Ax)
			bp->flags |= BNX2_FLAG_JUMBO_BROKEN;

		/* AER (Advanced Error Reporting) hooks */
		err = pci_enable_pcie_error_reporting(pdev);
		if (!err)
			bp->flags |= BNX2_FLAG_AER_ENABLED;

	} else {
		bp->pcix_cap = pci_find_capability(pdev, PCI_CAP_ID_PCIX);
		if (bp->pcix_cap == 0) {
			dev_err(&pdev->dev,
				"Cannot find PCIX capability, aborting\n");
			rc = -EIO;
			goto err_out_unmap;
		}
		bp->flags |= BNX2_FLAG_BROKEN_STATS;
	}

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709 &&
	    BNX2_CHIP_REV(bp) != BNX2_CHIP_REV_Ax) {
		if (pdev->msix_cap)
			bp->flags |= BNX2_FLAG_MSIX_CAP;
	}

	if (BNX2_CHIP_ID(bp) != BNX2_CHIP_ID_5706_A0 &&
	    BNX2_CHIP_ID(bp) != BNX2_CHIP_ID_5706_A1) {
		if (pdev->msi_cap)
			bp->flags |= BNX2_FLAG_MSI_CAP;
	}

	/* 5708 cannot support DMA addresses > 40-bit.  */
	if (BNX2_CHIP(bp) == BNX2_CHIP_5708)
		persist_dma_mask = dma_mask = DMA_BIT_MASK(40);
	else
		persist_dma_mask = dma_mask = DMA_BIT_MASK(64);

	/* Configure DMA attributes. */
	if (pci_set_dma_mask(pdev, dma_mask) == 0) {
		dev->features |= NETIF_F_HIGHDMA;
		rc = pci_set_consistent_dma_mask(pdev, persist_dma_mask);
		if (rc) {
			dev_err(&pdev->dev,
				"pci_set_consistent_dma_mask failed, aborting\n");
			goto err_out_unmap;
		}
	} else if ((rc = pci_set_dma_mask(pdev, DMA_BIT_MASK(32))) != 0) {
		dev_err(&pdev->dev, "System does not support DMA, aborting\n");
		goto err_out_unmap;
	}

	if (!(bp->flags & BNX2_FLAG_PCIE))
		bnx2_get_pci_speed(bp);

	/* 5706A0 may falsely detect SERR and PERR. */
	if (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A0) {
		reg = BNX2_RD(bp, PCI_COMMAND);
		reg &= ~(PCI_COMMAND_SERR | PCI_COMMAND_PARITY);
		BNX2_WR(bp, PCI_COMMAND, reg);
	} else if ((BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A1) &&
		!(bp->flags & BNX2_FLAG_PCIX)) {
		dev_err(&pdev->dev,
			"5706 A1 can only be used in a PCIX bus, aborting\n");
		rc = -EPERM;
		goto err_out_unmap;
	}

	bnx2_init_nvram(bp);

	reg = bnx2_reg_rd_ind(bp, BNX2_SHM_HDR_SIGNATURE);

	if (bnx2_reg_rd_ind(bp, BNX2_MCP_TOE_ID) & BNX2_MCP_TOE_ID_FUNCTION_ID)
		bp->func = 1;

	if ((reg & BNX2_SHM_HDR_SIGNATURE_SIG_MASK) ==
	    BNX2_SHM_HDR_SIGNATURE_SIG) {
		u32 off = bp->func << 2;

		bp->shmem_base = bnx2_reg_rd_ind(bp, BNX2_SHM_HDR_ADDR_0 + off);
	} else
		bp->shmem_base = HOST_VIEW_SHMEM_BASE;

	/* Get the permanent MAC address.  First we need to make sure the
	 * firmware is actually running.
	 */
	reg = bnx2_shmem_rd(bp, BNX2_DEV_INFO_SIGNATURE);

	if ((reg & BNX2_DEV_INFO_SIGNATURE_MAGIC_MASK) !=
	    BNX2_DEV_INFO_SIGNATURE_MAGIC) {
		dev_err(&pdev->dev, "Firmware not running, aborting\n");
		rc = -ENODEV;
		goto err_out_unmap;
	}

	bnx2_read_vpd_fw_ver(bp);

	j = strlen(bp->fw_version);
	reg = bnx2_shmem_rd(bp, BNX2_DEV_INFO_BC_REV);
	for (i = 0; i < 3 && j < 24; i++) {
		u8 num, k, skip0;

		if (i == 0) {
			bp->fw_version[j++] = 'b';
			bp->fw_version[j++] = 'c';
			bp->fw_version[j++] = ' ';
		}
		num = (u8) (reg >> (24 - (i * 8)));
		for (k = 100, skip0 = 1; k >= 1; num %= k, k /= 10) {
			if (num >= k || !skip0 || k == 1) {
				bp->fw_version[j++] = (num / k) + '0';
				skip0 = 0;
			}
		}
		if (i != 2)
			bp->fw_version[j++] = '.';
	}
	reg = bnx2_shmem_rd(bp, BNX2_PORT_FEATURE);
	if (reg & BNX2_PORT_FEATURE_WOL_ENABLED)
		bp->wol = 1;

	if (reg & BNX2_PORT_FEATURE_ASF_ENABLED) {
		bp->flags |= BNX2_FLAG_ASF_ENABLE;

		for (i = 0; i < 30; i++) {
			reg = bnx2_shmem_rd(bp, BNX2_BC_STATE_CONDITION);
			if (reg & BNX2_CONDITION_MFW_RUN_MASK)
				break;
			msleep(10);
		}
	}
	reg = bnx2_shmem_rd(bp, BNX2_BC_STATE_CONDITION);
	reg &= BNX2_CONDITION_MFW_RUN_MASK;
	if (reg != BNX2_CONDITION_MFW_RUN_UNKNOWN &&
	    reg != BNX2_CONDITION_MFW_RUN_NONE) {
		u32 addr = bnx2_shmem_rd(bp, BNX2_MFW_VER_PTR);

		if (j < 32)
			bp->fw_version[j++] = ' ';
		for (i = 0; i < 3 && j < 28; i++) {
			reg = bnx2_reg_rd_ind(bp, addr + i * 4);
			reg = be32_to_cpu(reg);
			memcpy(&bp->fw_version[j], &reg, 4);
			j += 4;
		}
	}

	reg = bnx2_shmem_rd(bp, BNX2_PORT_HW_CFG_MAC_UPPER);
	bp->mac_addr[0] = (u8) (reg >> 8);
	bp->mac_addr[1] = (u8) reg;

	reg = bnx2_shmem_rd(bp, BNX2_PORT_HW_CFG_MAC_LOWER);
	bp->mac_addr[2] = (u8) (reg >> 24);
	bp->mac_addr[3] = (u8) (reg >> 16);
	bp->mac_addr[4] = (u8) (reg >> 8);
	bp->mac_addr[5] = (u8) reg;

	bp->tx_ring_size = BNX2_MAX_TX_DESC_CNT;
	bnx2_set_rx_ring_size(bp, 255);

	bp->tx_quick_cons_trip_int = 2;
	bp->tx_quick_cons_trip = 20;
	bp->tx_ticks_int = 18;
	bp->tx_ticks = 80;

	bp->rx_quick_cons_trip_int = 2;
	bp->rx_quick_cons_trip = 12;
	bp->rx_ticks_int = 18;
	bp->rx_ticks = 18;

	bp->stats_ticks = USEC_PER_SEC & BNX2_HC_STATS_TICKS_HC_STAT_TICKS;

	bp->current_interval = BNX2_TIMER_INTERVAL;

	bp->phy_addr = 1;

	/* allocate stats_blk */
	rc = bnx2_alloc_stats_blk(dev);
	if (rc)
		goto err_out_unmap;

	/* Disable WOL support if we are running on a SERDES chip. */
	if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
		bnx2_get_5709_media(bp);
	else if (BNX2_CHIP_BOND(bp) & BNX2_CHIP_BOND_SERDES_BIT)
		bp->phy_flags |= BNX2_PHY_FLAG_SERDES;

	bp->phy_port = PORT_TP;
	if (bp->phy_flags & BNX2_PHY_FLAG_SERDES) {
		bp->phy_port = PORT_FIBRE;
		reg = bnx2_shmem_rd(bp, BNX2_SHARED_HW_CFG_CONFIG);
		if (!(reg & BNX2_SHARED_HW_CFG_GIG_LINK_ON_VAUX)) {
			bp->flags |= BNX2_FLAG_NO_WOL;
			bp->wol = 0;
		}
		if (BNX2_CHIP(bp) == BNX2_CHIP_5706) {
			/* Don't do parallel detect on this board because of
			 * some board problems.  The link will not go down
			 * if we do parallel detect.
			 */
			if (pdev->subsystem_vendor == PCI_VENDOR_ID_HP &&
			    pdev->subsystem_device == 0x310c)
				bp->phy_flags |= BNX2_PHY_FLAG_NO_PARALLEL;
		} else {
			bp->phy_addr = 2;
			if (reg & BNX2_SHARED_HW_CFG_PHY_2_5G)
				bp->phy_flags |= BNX2_PHY_FLAG_2_5G_CAPABLE;
		}
	} else if (BNX2_CHIP(bp) == BNX2_CHIP_5706 ||
		   BNX2_CHIP(bp) == BNX2_CHIP_5708)
		bp->phy_flags |= BNX2_PHY_FLAG_CRC_FIX;
	else if (BNX2_CHIP(bp) == BNX2_CHIP_5709 &&
		 (BNX2_CHIP_REV(bp) == BNX2_CHIP_REV_Ax ||
		  BNX2_CHIP_REV(bp) == BNX2_CHIP_REV_Bx))
		bp->phy_flags |= BNX2_PHY_FLAG_DIS_EARLY_DAC;

	bnx2_init_fw_cap(bp);

	if ((BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5708_A0) ||
	    (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5708_B0) ||
	    (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5708_B1) ||
	    !(BNX2_RD(bp, BNX2_PCI_CONFIG_3) & BNX2_PCI_CONFIG_3_VAUX_PRESET)) {
		bp->flags |= BNX2_FLAG_NO_WOL;
		bp->wol = 0;
	}

	if (bp->flags & BNX2_FLAG_NO_WOL)
		device_set_wakeup_capable(&bp->pdev->dev, false);
	else
		device_set_wakeup_enable(&bp->pdev->dev, bp->wol);

	if (BNX2_CHIP_ID(bp) == BNX2_CHIP_ID_5706_A0) {
		bp->tx_quick_cons_trip_int =
			bp->tx_quick_cons_trip;
		bp->tx_ticks_int = bp->tx_ticks;
		bp->rx_quick_cons_trip_int =
			bp->rx_quick_cons_trip;
		bp->rx_ticks_int = bp->rx_ticks;
		bp->comp_prod_trip_int = bp->comp_prod_trip;
		bp->com_ticks_int = bp->com_ticks;
		bp->cmd_ticks_int = bp->cmd_ticks;
	}

	/* Disable MSI on 5706 if AMD 8132 bridge is found.
	 *
	 * MSI is defined to be 32-bit write.  The 5706 does 64-bit MSI writes
	 * with byte enables disabled on the unused 32-bit word.  This is legal
	 * but causes problems on the AMD 8132 which will eventually stop
	 * responding after a while.
	 *
	 * AMD believes this incompatibility is unique to the 5706, and
	 * prefers to locally disable MSI rather than globally disabling it.
	 */
	if (BNX2_CHIP(bp) == BNX2_CHIP_5706 && disable_msi == 0) {
		struct pci_dev *amd_8132 = NULL;

		while ((amd_8132 = pci_get_device(PCI_VENDOR_ID_AMD,
						  PCI_DEVICE_ID_AMD_8132_BRIDGE,
						  amd_8132))) {

			if (amd_8132->revision >= 0x10 &&
			    amd_8132->revision <= 0x13) {
				disable_msi = 1;
				pci_dev_put(amd_8132);
				break;
			}
		}
	}

	bnx2_set_default_link(bp);
	bp->req_flow_ctrl = FLOW_CTRL_RX | FLOW_CTRL_TX;

	timer_setup(&bp->timer, bnx2_timer, 0);
	bp->timer.expires = RUN_AT(BNX2_TIMER_INTERVAL);

#ifdef BCM_CNIC
	if (bnx2_shmem_rd(bp, BNX2_ISCSI_INITIATOR) & BNX2_ISCSI_INITIATOR_EN)
		bp->cnic_eth_dev.max_iscsi_conn =
			(bnx2_shmem_rd(bp, BNX2_ISCSI_MAX_CONN) &
			 BNX2_ISCSI_MAX_CONN_MASK) >> BNX2_ISCSI_MAX_CONN_SHIFT;
	bp->cnic_probe = bnx2_cnic_probe;
#endif
	pci_save_state(pdev);

	return 0;

err_out_unmap:
	if (bp->flags & BNX2_FLAG_AER_ENABLED) {
		pci_disable_pcie_error_reporting(pdev);
		bp->flags &= ~BNX2_FLAG_AER_ENABLED;
	}

	pci_iounmap(pdev, bp->regview);
	bp->regview = NULL;

err_out_release:
	pci_release_regions(pdev);

err_out_disable:
	pci_disable_device(pdev);

err_out:
	kfree(bp->temp_stats_blk);

	return rc;
}

static char *
bnx2_bus_string(struct bnx2 *bp, char *str)
{
	char *s = str;

	if (bp->flags & BNX2_FLAG_PCIE) {
		s += sprintf(s, "PCI Express");
	} else {
		s += sprintf(s, "PCI");
		if (bp->flags & BNX2_FLAG_PCIX)
			s += sprintf(s, "-X");
		if (bp->flags & BNX2_FLAG_PCI_32BIT)
			s += sprintf(s, " 32-bit");
		else
			s += sprintf(s, " 64-bit");
		s += sprintf(s, " %dMHz", bp->bus_speed_mhz);
	}
	return str;
}

static void
bnx2_del_napi(struct bnx2 *bp)
{
	int i;

	for (i = 0; i < bp->irq_nvecs; i++)
		netif_napi_del(&bp->bnx2_napi[i].napi);
}

static void
bnx2_init_napi(struct bnx2 *bp)
{
	int i;

	for (i = 0; i < bp->irq_nvecs; i++) {
		struct bnx2_napi *bnapi = &bp->bnx2_napi[i];
		int (*poll)(struct napi_struct *, int);

		if (i == 0)
			poll = bnx2_poll;
		else
			poll = bnx2_poll_msix;

		netif_napi_add(bp->dev, &bp->bnx2_napi[i].napi, poll, 64);
		bnapi->bp = bp;
	}
}

static const struct net_device_ops bnx2_netdev_ops = {
	.ndo_open		= bnx2_open,
	.ndo_start_xmit		= bnx2_start_xmit,
	.ndo_stop		= bnx2_close,
	.ndo_get_stats64	= bnx2_get_stats64,
	.ndo_set_rx_mode	= bnx2_set_rx_mode,
	.ndo_do_ioctl		= bnx2_ioctl,
	.ndo_validate_addr	= eth_validate_addr,
	.ndo_set_mac_address	= bnx2_change_mac_addr,
	.ndo_change_mtu		= bnx2_change_mtu,
	.ndo_set_features	= bnx2_set_features,
	.ndo_tx_timeout		= bnx2_tx_timeout,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_poll_controller	= poll_bnx2,
#endif
};

static int
bnx2_init_one(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	struct net_device *dev;
	struct bnx2 *bp;
	int rc;
	char str[40];

	/* dev zeroed in init_etherdev */
	dev = alloc_etherdev_mq(sizeof(*bp), TX_MAX_RINGS);
	if (!dev)
		return -ENOMEM;

	rc = bnx2_init_board(pdev, dev);
	if (rc < 0)
		goto err_free;

	dev->netdev_ops = &bnx2_netdev_ops;
	dev->watchdog_timeo = TX_TIMEOUT;
	dev->ethtool_ops = &bnx2_ethtool_ops;

	bp = netdev_priv(dev);

	pci_set_drvdata(pdev, dev);

	/*
	 * In-flight DMA from 1st kernel could continue going in kdump kernel.
	 * New io-page table has been created before bnx2 does reset at open stage.
	 * We have to wait for the in-flight DMA to complete to avoid it look up
	 * into the newly created io-page table.
	 */
	if (is_kdump_kernel())
		bnx2_wait_dma_complete(bp);

	memcpy(dev->dev_addr, bp->mac_addr, ETH_ALEN);

	dev->hw_features = NETIF_F_IP_CSUM | NETIF_F_SG |
		NETIF_F_TSO | NETIF_F_TSO_ECN |
		NETIF_F_RXHASH | NETIF_F_RXCSUM;

	if (BNX2_CHIP(bp) == BNX2_CHIP_5709)
		dev->hw_features |= NETIF_F_IPV6_CSUM | NETIF_F_TSO6;

	dev->vlan_features = dev->hw_features;
	dev->hw_features |= NETIF_F_HW_VLAN_CTAG_TX | NETIF_F_HW_VLAN_CTAG_RX;
	dev->features |= dev->hw_features;
	dev->priv_flags |= IFF_UNICAST_FLT;
	dev->min_mtu = MIN_ETHERNET_PACKET_SIZE;
	dev->max_mtu = MAX_ETHERNET_JUMBO_PACKET_SIZE;

	if (!(bp->flags & BNX2_FLAG_CAN_KEEP_VLAN))
		dev->hw_features &= ~NETIF_F_HW_VLAN_CTAG_RX;

	if ((rc = register_netdev(dev))) {
		dev_err(&pdev->dev, "Cannot register net device\n");
		goto error;
	}

	netdev_info(dev, "%s (%c%d) %s found at mem %lx, IRQ %d, "
		    "node addr %pM\n", board_info[ent->driver_data].name,
		    ((BNX2_CHIP_ID(bp) & 0xf000) >> 12) + 'A',
		    ((BNX2_CHIP_ID(bp) & 0x0ff0) >> 4),
		    bnx2_bus_string(bp, str), (long)pci_resource_start(pdev, 0),
		    pdev->irq, dev->dev_addr);

	return 0;

error:
	pci_iounmap(pdev, bp->regview);
	pci_release_regions(pdev);
	pci_disable_device(pdev);
err_free:
	bnx2_free_stats_blk(dev);
	free_netdev(dev);
	return rc;
}

static void
bnx2_remove_one(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);
	struct bnx2 *bp = netdev_priv(dev);

	unregister_netdev(dev);

	del_timer_sync(&bp->timer);
	cancel_work_sync(&bp->reset_task);

	pci_iounmap(bp->pdev, bp->regview);

	bnx2_free_stats_blk(dev);
	kfree(bp->temp_stats_blk);

	if (bp->flags & BNX2_FLAG_AER_ENABLED) {
		pci_disable_pcie_error_reporting(pdev);
		bp->flags &= ~BNX2_FLAG_AER_ENABLED;
	}

	bnx2_release_firmware(bp);

	free_netdev(dev);

	pci_release_regions(pdev);
	pci_disable_device(pdev);
}

#ifdef CONFIG_PM_SLEEP
static int
bnx2_suspend(struct device *device)
{
	struct net_device *dev = dev_get_drvdata(device);
	struct bnx2 *bp = netdev_priv(dev);

	if (netif_running(dev)) {
		cancel_work_sync(&bp->reset_task);
		bnx2_netif_stop(bp, true);
		netif_device_detach(dev);
		del_timer_sync(&bp->timer);
		bnx2_shutdown_chip(bp);
		__bnx2_free_irq(bp);
		bnx2_free_skbs(bp);
	}
	bnx2_setup_wol(bp);
	return 0;
}

static int
bnx2_resume(struct device *device)
{
	struct net_device *dev = dev_get_drvdata(device);
	struct bnx2 *bp = netdev_priv(dev);

	if (!netif_running(dev))
		return 0;

	bnx2_set_power_state(bp, PCI_D0);
	netif_device_attach(dev);
	bnx2_request_irq(bp);
	bnx2_init_nic(bp, 1);
	bnx2_netif_start(bp, true);
	return 0;
}

static SIMPLE_DEV_PM_OPS(bnx2_pm_ops, bnx2_suspend, bnx2_resume);
#define BNX2_PM_OPS (&bnx2_pm_ops)

#else

#define BNX2_PM_OPS NULL

#endif /* CONFIG_PM_SLEEP */
/**
 * bnx2_io_error_detected - called when PCI error is detected
 * @pdev: Pointer to PCI device
 * @state: The current pci connection state
 *
 * This function is called after a PCI bus error affecting
 * this device has been detected.
 */
static pci_ers_result_t bnx2_io_error_detected(struct pci_dev *pdev,
					       pci_channel_state_t state)
{
	struct net_device *dev = pci_get_drvdata(pdev);
	struct bnx2 *bp = netdev_priv(dev);

	rtnl_lock();
	netif_device_detach(dev);

	if (state == pci_channel_io_perm_failure) {
		rtnl_unlock();
		return PCI_ERS_RESULT_DISCONNECT;
	}

	if (netif_running(dev)) {
		bnx2_netif_stop(bp, true);
		del_timer_sync(&bp->timer);
		bnx2_reset_nic(bp, BNX2_DRV_MSG_CODE_RESET);
	}

	pci_disable_device(pdev);
	rtnl_unlock();

	/* Request a slot slot reset. */
	return PCI_ERS_RESULT_NEED_RESET;
}

/**
 * bnx2_io_slot_reset - called after the pci bus has been reset.
 * @pdev: Pointer to PCI device
 *
 * Restart the card from scratch, as if from a cold-boot.
 */
static pci_ers_result_t bnx2_io_slot_reset(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);
	struct bnx2 *bp = netdev_priv(dev);
	pci_ers_result_t result = PCI_ERS_RESULT_DISCONNECT;
	int err = 0;

	rtnl_lock();
	if (pci_enable_device(pdev)) {
		dev_err(&pdev->dev,
			"Cannot re-enable PCI device after reset\n");
	} else {
		pci_set_master(pdev);
		pci_restore_state(pdev);
		pci_save_state(pdev);

		if (netif_running(dev))
			err = bnx2_init_nic(bp, 1);

		if (!err)
			result = PCI_ERS_RESULT_RECOVERED;
	}

	if (result != PCI_ERS_RESULT_RECOVERED && netif_running(dev)) {
		bnx2_napi_enable(bp);
		dev_close(dev);
	}
	rtnl_unlock();

	if (!(bp->flags & BNX2_FLAG_AER_ENABLED))
		return result;

	return result;
}

/**
 * bnx2_io_resume - called when traffic can start flowing again.
 * @pdev: Pointer to PCI device
 *
 * This callback is called when the error recovery driver tells us that
 * its OK to resume normal operation.
 */
static void bnx2_io_resume(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);
	struct bnx2 *bp = netdev_priv(dev);

	rtnl_lock();
	if (netif_running(dev))
		bnx2_netif_start(bp, true);

	netif_device_attach(dev);
	rtnl_unlock();
}

static void bnx2_shutdown(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);
	struct bnx2 *bp;

	if (!dev)
		return;

	bp = netdev_priv(dev);
	if (!bp)
		return;

	rtnl_lock();
	if (netif_running(dev))
		dev_close(bp->dev);

	if (system_state == SYSTEM_POWER_OFF)
		bnx2_set_power_state(bp, PCI_D3hot);

	rtnl_unlock();
}

static const struct pci_error_handlers bnx2_err_handler = {
	.error_detected	= bnx2_io_error_detected,
	.slot_reset	= bnx2_io_slot_reset,
	.resume		= bnx2_io_resume,
};

static struct pci_driver bnx2_pci_driver = {
	.name		= DRV_MODULE_NAME,
	.id_table	= bnx2_pci_tbl,
	.probe		= bnx2_init_one,
	.remove		= bnx2_remove_one,
	.driver.pm	= BNX2_PM_OPS,
	.err_handler	= &bnx2_err_handler,
	.shutdown	= bnx2_shutdown,
};

module_pci_driver(bnx2_pci_driver);
