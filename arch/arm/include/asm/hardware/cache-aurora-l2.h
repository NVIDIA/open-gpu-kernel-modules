/*
 * AURORA shared L2 cache controller support
 *
 * Copyright (C) 2012 Marvell
 *
 * Yehuda Yitschak <yehuday@marvell.com>
 * Gregory CLEMENT <gregory.clement@free-electrons.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __ASM_ARM_HARDWARE_AURORA_L2_H
#define __ASM_ARM_HARDWARE_AURORA_L2_H

#define AURORA_SYNC_REG		    0x700
#define AURORA_RANGE_BASE_ADDR_REG  0x720
#define AURORA_FLUSH_PHY_ADDR_REG   0x7f0
#define AURORA_INVAL_RANGE_REG	    0x774
#define AURORA_CLEAN_RANGE_REG	    0x7b4
#define AURORA_FLUSH_RANGE_REG	    0x7f4

#define AURORA_ACR_REPLACEMENT_OFFSET	    27
#define AURORA_ACR_REPLACEMENT_MASK	     \
	(0x3 << AURORA_ACR_REPLACEMENT_OFFSET)
#define AURORA_ACR_REPLACEMENT_TYPE_WAYRR    \
	(0 << AURORA_ACR_REPLACEMENT_OFFSET)
#define AURORA_ACR_REPLACEMENT_TYPE_LFSR     \
	(1 << AURORA_ACR_REPLACEMENT_OFFSET)
#define AURORA_ACR_REPLACEMENT_TYPE_SEMIPLRU \
	(3 << AURORA_ACR_REPLACEMENT_OFFSET)

#define AURORA_ACR_PARITY_EN	(1 << 21)
#define AURORA_ACR_ECC_EN	(1 << 20)

#define AURORA_ACR_FORCE_WRITE_POLICY_OFFSET	0
#define AURORA_ACR_FORCE_WRITE_POLICY_MASK	\
	(0x3 << AURORA_ACR_FORCE_WRITE_POLICY_OFFSET)
#define AURORA_ACR_FORCE_WRITE_POLICY_DIS	\
	(0 << AURORA_ACR_FORCE_WRITE_POLICY_OFFSET)
#define AURORA_ACR_FORCE_WRITE_BACK_POLICY	\
	(1 << AURORA_ACR_FORCE_WRITE_POLICY_OFFSET)
#define AURORA_ACR_FORCE_WRITE_THRO_POLICY	\
	(2 << AURORA_ACR_FORCE_WRITE_POLICY_OFFSET)

#define AURORA_ERR_CNT_REG          0x600
#define AURORA_ERR_ATTR_CAP_REG     0x608
#define AURORA_ERR_ADDR_CAP_REG     0x60c
#define AURORA_ERR_WAY_CAP_REG      0x610
#define AURORA_ERR_INJECT_CTL_REG   0x614
#define AURORA_ERR_INJECT_MASK_REG  0x618

#define AURORA_ERR_CNT_CLR_OFFSET         31
#define AURORA_ERR_CNT_CLR		   \
	(0x1 << AURORA_ERR_CNT_CLR_OFFSET)
#define AURORA_ERR_CNT_UE_OFFSET          16
#define AURORA_ERR_CNT_UE_MASK             \
	(0x7fff << AURORA_ERR_CNT_UE_OFFSET)
#define AURORA_ERR_CNT_CE_OFFSET           0
#define AURORA_ERR_CNT_CE_MASK             \
	(0xffff << AURORA_ERR_CNT_CE_OFFSET)

#define AURORA_ERR_ATTR_SRC_OFF           16
#define AURORA_ERR_ATTR_SRC_MSK            \
	(0x7 << AURORA_ERR_ATTR_SRC_OFF)
#define AURORA_ERR_ATTR_TXN_OFF           12
#define AURORA_ERR_ATTR_TXN_MSK            \
	(0xf << AURORA_ERR_ATTR_TXN_OFF)
#define AURORA_ERR_ATTR_ERR_OFF            8
#define AURORA_ERR_ATTR_ERR_MSK            \
	(0x3 << AURORA_ERR_ATTR_ERR_OFF)
#define AURORA_ERR_ATTR_CAP_VALID_OFF      0
#define AURORA_ERR_ATTR_CAP_VALID          \
	(0x1 << AURORA_ERR_ATTR_CAP_VALID_OFF)

#define AURORA_ERR_ADDR_CAP_ADDR_MASK 0xffffffe0

#define AURORA_ERR_WAY_IDX_OFF             8
#define AURORA_ERR_WAY_IDX_MSK             \
	(0xfff << AURORA_ERR_WAY_IDX_OFF)
#define AURORA_ERR_WAY_CAP_WAY_OFFSET      1
#define AURORA_ERR_WAY_CAP_WAY_MASK        \
	(0xf << AURORA_ERR_WAY_CAP_WAY_OFFSET)

#define AURORA_ERR_INJECT_CTL_ADDR_MASK 0xfffffff0
#define AURORA_ERR_ATTR_TXN_OFF   12
#define AURORA_ERR_INJECT_CTL_EN_MASK          0x3
#define AURORA_ERR_INJECT_CTL_EN_PARITY        0x2
#define AURORA_ERR_INJECT_CTL_EN_ECC           0x1

#define AURORA_MAX_RANGE_SIZE	1024

#define AURORA_WAY_SIZE_SHIFT	2

#define AURORA_CTRL_FW		0x100

/* chose a number outside L2X0_CACHE_ID_PART_MASK to be sure to make
 * the distinction between a number coming from hardware and a number
 * coming from the device tree */
#define AURORA_CACHE_ID	       0x100

#endif /* __ASM_ARM_HARDWARE_AURORA_L2_H */
