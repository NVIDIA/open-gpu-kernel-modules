/*
 * Copyright 2003-2011 NetLogic Microsystems, Inc. (NetLogic). All rights
 * reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the NetLogic
 * license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NETLOGIC ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL NETLOGIC OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __NLM_HAL_SYS_H__
#define __NLM_HAL_SYS_H__

/**
* @file_name sys.h
* @author Netlogic Microsystems
* @brief HAL for System configuration registers
*/
#define SYS_CHIP_RESET				0x00
#define SYS_POWER_ON_RESET_CFG			0x01
#define SYS_EFUSE_DEVICE_CFG_STATUS0		0x02
#define SYS_EFUSE_DEVICE_CFG_STATUS1		0x03
#define SYS_EFUSE_DEVICE_CFG_STATUS2		0x04
#define SYS_EFUSE_DEVICE_CFG3			0x05
#define SYS_EFUSE_DEVICE_CFG4			0x06
#define SYS_EFUSE_DEVICE_CFG5			0x07
#define SYS_EFUSE_DEVICE_CFG6			0x08
#define SYS_EFUSE_DEVICE_CFG7			0x09
#define SYS_PLL_CTRL				0x0a
#define SYS_CPU_RESET				0x0b
#define SYS_CPU_NONCOHERENT_MODE		0x0d
#define SYS_CORE_DFS_DIS_CTRL			0x0e
#define SYS_CORE_DFS_RST_CTRL			0x0f
#define SYS_CORE_DFS_BYP_CTRL			0x10
#define SYS_CORE_DFS_PHA_CTRL			0x11
#define SYS_CORE_DFS_DIV_INC_CTRL		0x12
#define SYS_CORE_DFS_DIV_DEC_CTRL		0x13
#define SYS_CORE_DFS_DIV_VALUE			0x14
#define SYS_RESET				0x15
#define SYS_DFS_DIS_CTRL			0x16
#define SYS_DFS_RST_CTRL			0x17
#define SYS_DFS_BYP_CTRL			0x18
#define SYS_DFS_DIV_INC_CTRL			0x19
#define SYS_DFS_DIV_DEC_CTRL			0x1a
#define SYS_DFS_DIV_VALUE0			0x1b
#define SYS_DFS_DIV_VALUE1			0x1c
#define SYS_SENSE_AMP_DLY			0x1d
#define SYS_SOC_SENSE_AMP_DLY			0x1e
#define SYS_CTRL0				0x1f
#define SYS_CTRL1				0x20
#define SYS_TIMEOUT_BS1				0x21
#define SYS_BYTE_SWAP				0x22
#define SYS_VRM_VID				0x23
#define SYS_PWR_RAM_CMD				0x24
#define SYS_PWR_RAM_ADDR			0x25
#define SYS_PWR_RAM_DATA0			0x26
#define SYS_PWR_RAM_DATA1			0x27
#define SYS_PWR_RAM_DATA2			0x28
#define SYS_PWR_UCODE				0x29
#define SYS_CPU0_PWR_STATUS			0x2a
#define SYS_CPU1_PWR_STATUS			0x2b
#define SYS_CPU2_PWR_STATUS			0x2c
#define SYS_CPU3_PWR_STATUS			0x2d
#define SYS_CPU4_PWR_STATUS			0x2e
#define SYS_CPU5_PWR_STATUS			0x2f
#define SYS_CPU6_PWR_STATUS			0x30
#define SYS_CPU7_PWR_STATUS			0x31
#define SYS_STATUS				0x32
#define SYS_INT_POL				0x33
#define SYS_INT_TYPE				0x34
#define SYS_INT_STATUS				0x35
#define SYS_INT_MASK0				0x36
#define SYS_INT_MASK1				0x37
#define SYS_UCO_S_ECC				0x38
#define SYS_UCO_M_ECC				0x39
#define SYS_UCO_ADDR				0x3a
#define SYS_UCO_INSTR				0x3b
#define SYS_MEM_BIST0				0x3c
#define SYS_MEM_BIST1				0x3d
#define SYS_MEM_BIST2				0x3e
#define SYS_MEM_BIST3				0x3f
#define SYS_MEM_BIST4				0x40
#define SYS_MEM_BIST5				0x41
#define SYS_MEM_BIST6				0x42
#define SYS_MEM_BIST7				0x43
#define SYS_MEM_BIST8				0x44
#define SYS_MEM_BIST9				0x45
#define SYS_MEM_BIST10				0x46
#define SYS_MEM_BIST11				0x47
#define SYS_MEM_BIST12				0x48
#define SYS_SCRTCH0				0x49
#define SYS_SCRTCH1				0x4a
#define SYS_SCRTCH2				0x4b
#define SYS_SCRTCH3				0x4c

/* PLL registers XLP2XX */
#define SYS_CPU_PLL_CTRL0(core)			(0x1c0 + (core * 4))
#define SYS_CPU_PLL_CTRL1(core)			(0x1c1 + (core * 4))
#define SYS_CPU_PLL_CTRL2(core)			(0x1c2 + (core * 4))
#define SYS_CPU_PLL_CTRL3(core)			(0x1c3 + (core * 4))
#define SYS_PLL_CTRL0				0x240
#define SYS_PLL_CTRL1				0x241
#define SYS_PLL_CTRL2				0x242
#define SYS_PLL_CTRL3				0x243
#define SYS_DMC_PLL_CTRL0			0x244
#define SYS_DMC_PLL_CTRL1			0x245
#define SYS_DMC_PLL_CTRL2			0x246
#define SYS_DMC_PLL_CTRL3			0x247

#define SYS_PLL_CTRL0_DEVX(x)			(0x248 + (x) * 4)
#define SYS_PLL_CTRL1_DEVX(x)			(0x249 + (x) * 4)
#define SYS_PLL_CTRL2_DEVX(x)			(0x24a + (x) * 4)
#define SYS_PLL_CTRL3_DEVX(x)			(0x24b + (x) * 4)

#define SYS_CPU_PLL_CHG_CTRL			0x288
#define SYS_PLL_CHG_CTRL			0x289
#define SYS_CLK_DEV_DIS				0x28a
#define SYS_CLK_DEV_SEL				0x28b
#define SYS_CLK_DEV_DIV				0x28c
#define SYS_CLK_DEV_CHG				0x28d
#define SYS_CLK_DEV_SEL_REG			0x28e
#define SYS_CLK_DEV_DIV_REG			0x28f
#define SYS_CPU_PLL_LOCK			0x29f
#define SYS_SYS_PLL_LOCK			0x2a0
#define SYS_PLL_MEM_CMD				0x2a1
#define SYS_CPU_PLL_MEM_REQ			0x2a2
#define SYS_SYS_PLL_MEM_REQ			0x2a3
#define SYS_PLL_MEM_STAT			0x2a4

/* PLL registers XLP9XX */
#define SYS_9XX_CPU_PLL_CTRL0(core)		(0xc0 + (core * 4))
#define SYS_9XX_CPU_PLL_CTRL1(core)		(0xc1 + (core * 4))
#define SYS_9XX_CPU_PLL_CTRL2(core)		(0xc2 + (core * 4))
#define SYS_9XX_CPU_PLL_CTRL3(core)		(0xc3 + (core * 4))
#define SYS_9XX_DMC_PLL_CTRL0			0x140
#define SYS_9XX_DMC_PLL_CTRL1			0x141
#define SYS_9XX_DMC_PLL_CTRL2			0x142
#define SYS_9XX_DMC_PLL_CTRL3			0x143
#define SYS_9XX_PLL_CTRL0			0x144
#define SYS_9XX_PLL_CTRL1			0x145
#define SYS_9XX_PLL_CTRL2			0x146
#define SYS_9XX_PLL_CTRL3			0x147

#define SYS_9XX_PLL_CTRL0_DEVX(x)		(0x148 + (x) * 4)
#define SYS_9XX_PLL_CTRL1_DEVX(x)		(0x149 + (x) * 4)
#define SYS_9XX_PLL_CTRL2_DEVX(x)		(0x14a + (x) * 4)
#define SYS_9XX_PLL_CTRL3_DEVX(x)		(0x14b + (x) * 4)

#define SYS_9XX_CPU_PLL_CHG_CTRL		0x188
#define SYS_9XX_PLL_CHG_CTRL			0x189
#define SYS_9XX_CLK_DEV_DIS			0x18a
#define SYS_9XX_CLK_DEV_SEL			0x18b
#define SYS_9XX_CLK_DEV_DIV			0x18d
#define SYS_9XX_CLK_DEV_CHG			0x18f

#define SYS_9XX_CLK_DEV_SEL_REG			0x1a4
#define SYS_9XX_CLK_DEV_DIV_REG			0x1a6

/* Registers changed on 9XX */
#define SYS_9XX_POWER_ON_RESET_CFG		0x00
#define SYS_9XX_CHIP_RESET			0x01
#define SYS_9XX_CPU_RESET			0x02
#define SYS_9XX_CPU_NONCOHERENT_MODE		0x03

/* XLP 9XX fuse block registers */
#define FUSE_9XX_DEVCFG6			0xc6

#ifndef __ASSEMBLY__

#define nlm_read_sys_reg(b, r)		nlm_read_reg(b, r)
#define nlm_write_sys_reg(b, r, v)	nlm_write_reg(b, r, v)
#define nlm_get_sys_pcibase(node)	nlm_pcicfg_base(cpu_is_xlp9xx() ? \
		XLP9XX_IO_SYS_OFFSET(node) : XLP_IO_SYS_OFFSET(node))
#define nlm_get_sys_regbase(node) (nlm_get_sys_pcibase(node) + XLP_IO_PCI_HDRSZ)

/* XLP9XX fuse block */
#define nlm_get_fuse_pcibase(node)	\
			nlm_pcicfg_base(XLP9XX_IO_FUSE_OFFSET(node))
#define nlm_get_fuse_regbase(node)	\
			(nlm_get_fuse_pcibase(node) + XLP_IO_PCI_HDRSZ)

#define nlm_get_clock_pcibase(node)	\
			nlm_pcicfg_base(XLP9XX_IO_CLOCK_OFFSET(node))
#define nlm_get_clock_regbase(node)	\
			(nlm_get_clock_pcibase(node) + XLP_IO_PCI_HDRSZ)

unsigned int nlm_get_pic_frequency(int node);
#endif
#endif
