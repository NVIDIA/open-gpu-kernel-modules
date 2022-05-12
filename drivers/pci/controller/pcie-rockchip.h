/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Rockchip AXI PCIe controller driver
 *
 * Copyright (c) 2018 Rockchip, Inc.
 *
 * Author: Shawn Lin <shawn.lin@rock-chips.com>
 *
 */

#ifndef _PCIE_ROCKCHIP_H
#define _PCIE_ROCKCHIP_H

#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/pci-ecam.h>

/*
 * The upper 16 bits of PCIE_CLIENT_CONFIG are a write mask for the lower 16
 * bits.  This allows atomic updates of the register without locking.
 */
#define HIWORD_UPDATE(mask, val)	(((mask) << 16) | (val))
#define HIWORD_UPDATE_BIT(val)		HIWORD_UPDATE(val, val)

#define ENCODE_LANES(x)			((((x) >> 1) & 3) << 4)
#define MAX_LANE_NUM			4
#define MAX_REGION_LIMIT		32
#define MIN_EP_APERTURE			28

#define PCIE_CLIENT_BASE		0x0
#define PCIE_CLIENT_CONFIG		(PCIE_CLIENT_BASE + 0x00)
#define   PCIE_CLIENT_CONF_ENABLE	  HIWORD_UPDATE_BIT(0x0001)
#define   PCIE_CLIENT_CONF_DISABLE       HIWORD_UPDATE(0x0001, 0)
#define   PCIE_CLIENT_LINK_TRAIN_ENABLE	  HIWORD_UPDATE_BIT(0x0002)
#define   PCIE_CLIENT_ARI_ENABLE	  HIWORD_UPDATE_BIT(0x0008)
#define   PCIE_CLIENT_CONF_LANE_NUM(x)	  HIWORD_UPDATE(0x0030, ENCODE_LANES(x))
#define   PCIE_CLIENT_MODE_RC		  HIWORD_UPDATE_BIT(0x0040)
#define   PCIE_CLIENT_MODE_EP            HIWORD_UPDATE(0x0040, 0)
#define   PCIE_CLIENT_GEN_SEL_1		  HIWORD_UPDATE(0x0080, 0)
#define   PCIE_CLIENT_GEN_SEL_2		  HIWORD_UPDATE_BIT(0x0080)
#define PCIE_CLIENT_DEBUG_OUT_0		(PCIE_CLIENT_BASE + 0x3c)
#define   PCIE_CLIENT_DEBUG_LTSSM_MASK		GENMASK(5, 0)
#define   PCIE_CLIENT_DEBUG_LTSSM_L1		0x18
#define   PCIE_CLIENT_DEBUG_LTSSM_L2		0x19
#define PCIE_CLIENT_BASIC_STATUS1	(PCIE_CLIENT_BASE + 0x48)
#define   PCIE_CLIENT_LINK_STATUS_UP		0x00300000
#define   PCIE_CLIENT_LINK_STATUS_MASK		0x00300000
#define PCIE_CLIENT_INT_MASK		(PCIE_CLIENT_BASE + 0x4c)
#define PCIE_CLIENT_INT_STATUS		(PCIE_CLIENT_BASE + 0x50)
#define   PCIE_CLIENT_INTR_MASK			GENMASK(8, 5)
#define   PCIE_CLIENT_INTR_SHIFT		5
#define   PCIE_CLIENT_INT_LEGACY_DONE		BIT(15)
#define   PCIE_CLIENT_INT_MSG			BIT(14)
#define   PCIE_CLIENT_INT_HOT_RST		BIT(13)
#define   PCIE_CLIENT_INT_DPA			BIT(12)
#define   PCIE_CLIENT_INT_FATAL_ERR		BIT(11)
#define   PCIE_CLIENT_INT_NFATAL_ERR		BIT(10)
#define   PCIE_CLIENT_INT_CORR_ERR		BIT(9)
#define   PCIE_CLIENT_INT_INTD			BIT(8)
#define   PCIE_CLIENT_INT_INTC			BIT(7)
#define   PCIE_CLIENT_INT_INTB			BIT(6)
#define   PCIE_CLIENT_INT_INTA			BIT(5)
#define   PCIE_CLIENT_INT_LOCAL			BIT(4)
#define   PCIE_CLIENT_INT_UDMA			BIT(3)
#define   PCIE_CLIENT_INT_PHY			BIT(2)
#define   PCIE_CLIENT_INT_HOT_PLUG		BIT(1)
#define   PCIE_CLIENT_INT_PWR_STCG		BIT(0)

#define PCIE_CLIENT_INT_LEGACY \
	(PCIE_CLIENT_INT_INTA | PCIE_CLIENT_INT_INTB | \
	PCIE_CLIENT_INT_INTC | PCIE_CLIENT_INT_INTD)

#define PCIE_CLIENT_INT_CLI \
	(PCIE_CLIENT_INT_CORR_ERR | PCIE_CLIENT_INT_NFATAL_ERR | \
	PCIE_CLIENT_INT_FATAL_ERR | PCIE_CLIENT_INT_DPA | \
	PCIE_CLIENT_INT_HOT_RST | PCIE_CLIENT_INT_MSG | \
	PCIE_CLIENT_INT_LEGACY_DONE | PCIE_CLIENT_INT_LEGACY | \
	PCIE_CLIENT_INT_PHY)

#define PCIE_CORE_CTRL_MGMT_BASE	0x900000
#define PCIE_CORE_CTRL			(PCIE_CORE_CTRL_MGMT_BASE + 0x000)
#define   PCIE_CORE_PL_CONF_SPEED_5G		0x00000008
#define   PCIE_CORE_PL_CONF_SPEED_MASK		0x00000018
#define   PCIE_CORE_PL_CONF_LANE_MASK		0x00000006
#define   PCIE_CORE_PL_CONF_LANE_SHIFT		1
#define PCIE_CORE_CTRL_PLC1		(PCIE_CORE_CTRL_MGMT_BASE + 0x004)
#define   PCIE_CORE_CTRL_PLC1_FTS_MASK		GENMASK(23, 8)
#define   PCIE_CORE_CTRL_PLC1_FTS_SHIFT		8
#define   PCIE_CORE_CTRL_PLC1_FTS_CNT		0xffff
#define PCIE_CORE_TXCREDIT_CFG1		(PCIE_CORE_CTRL_MGMT_BASE + 0x020)
#define   PCIE_CORE_TXCREDIT_CFG1_MUI_MASK	0xFFFF0000
#define   PCIE_CORE_TXCREDIT_CFG1_MUI_SHIFT	16
#define   PCIE_CORE_TXCREDIT_CFG1_MUI_ENCODE(x) \
		(((x) >> 3) << PCIE_CORE_TXCREDIT_CFG1_MUI_SHIFT)
#define PCIE_CORE_LANE_MAP             (PCIE_CORE_CTRL_MGMT_BASE + 0x200)
#define   PCIE_CORE_LANE_MAP_MASK              0x0000000f
#define   PCIE_CORE_LANE_MAP_REVERSE           BIT(16)
#define PCIE_CORE_INT_STATUS		(PCIE_CORE_CTRL_MGMT_BASE + 0x20c)
#define   PCIE_CORE_INT_PRFPE			BIT(0)
#define   PCIE_CORE_INT_CRFPE			BIT(1)
#define   PCIE_CORE_INT_RRPE			BIT(2)
#define   PCIE_CORE_INT_PRFO			BIT(3)
#define   PCIE_CORE_INT_CRFO			BIT(4)
#define   PCIE_CORE_INT_RT			BIT(5)
#define   PCIE_CORE_INT_RTR			BIT(6)
#define   PCIE_CORE_INT_PE			BIT(7)
#define   PCIE_CORE_INT_MTR			BIT(8)
#define   PCIE_CORE_INT_UCR			BIT(9)
#define   PCIE_CORE_INT_FCE			BIT(10)
#define   PCIE_CORE_INT_CT			BIT(11)
#define   PCIE_CORE_INT_UTC			BIT(18)
#define   PCIE_CORE_INT_MMVC			BIT(19)
#define PCIE_CORE_CONFIG_VENDOR		(PCIE_CORE_CTRL_MGMT_BASE + 0x44)
#define PCIE_CORE_INT_MASK		(PCIE_CORE_CTRL_MGMT_BASE + 0x210)
#define PCIE_CORE_PHY_FUNC_CFG		(PCIE_CORE_CTRL_MGMT_BASE + 0x2c0)
#define PCIE_RC_BAR_CONF		(PCIE_CORE_CTRL_MGMT_BASE + 0x300)
#define ROCKCHIP_PCIE_CORE_BAR_CFG_CTRL_DISABLED		0x0
#define ROCKCHIP_PCIE_CORE_BAR_CFG_CTRL_IO_32BITS		0x1
#define ROCKCHIP_PCIE_CORE_BAR_CFG_CTRL_MEM_32BITS		0x4
#define ROCKCHIP_PCIE_CORE_BAR_CFG_CTRL_PREFETCH_MEM_32BITS	0x5
#define ROCKCHIP_PCIE_CORE_BAR_CFG_CTRL_MEM_64BITS		0x6
#define ROCKCHIP_PCIE_CORE_BAR_CFG_CTRL_PREFETCH_MEM_64BITS	0x7

#define PCIE_CORE_INT \
		(PCIE_CORE_INT_PRFPE | PCIE_CORE_INT_CRFPE | \
		 PCIE_CORE_INT_RRPE | PCIE_CORE_INT_CRFO | \
		 PCIE_CORE_INT_RT | PCIE_CORE_INT_RTR | \
		 PCIE_CORE_INT_PE | PCIE_CORE_INT_MTR | \
		 PCIE_CORE_INT_UCR | PCIE_CORE_INT_FCE | \
		 PCIE_CORE_INT_CT | PCIE_CORE_INT_UTC | \
		 PCIE_CORE_INT_MMVC)

#define PCIE_RC_RP_ATS_BASE		0x400000
#define PCIE_RC_CONFIG_NORMAL_BASE	0x800000
#define PCIE_RC_CONFIG_BASE		0xa00000
#define PCIE_RC_CONFIG_RID_CCR		(PCIE_RC_CONFIG_BASE + 0x08)
#define   PCIE_RC_CONFIG_SCC_SHIFT		16
#define PCIE_RC_CONFIG_DCR		(PCIE_RC_CONFIG_BASE + 0xc4)
#define   PCIE_RC_CONFIG_DCR_CSPL_SHIFT		18
#define   PCIE_RC_CONFIG_DCR_CSPL_LIMIT		0xff
#define   PCIE_RC_CONFIG_DCR_CPLS_SHIFT		26
#define PCIE_RC_CONFIG_DCSR		(PCIE_RC_CONFIG_BASE + 0xc8)
#define   PCIE_RC_CONFIG_DCSR_MPS_MASK		GENMASK(7, 5)
#define   PCIE_RC_CONFIG_DCSR_MPS_256		(0x1 << 5)
#define PCIE_RC_CONFIG_LINK_CAP		(PCIE_RC_CONFIG_BASE + 0xcc)
#define   PCIE_RC_CONFIG_LINK_CAP_L0S		BIT(10)
#define PCIE_RC_CONFIG_LCS		(PCIE_RC_CONFIG_BASE + 0xd0)
#define PCIE_RC_CONFIG_L1_SUBSTATE_CTRL2 (PCIE_RC_CONFIG_BASE + 0x90c)
#define PCIE_RC_CONFIG_THP_CAP		(PCIE_RC_CONFIG_BASE + 0x274)
#define   PCIE_RC_CONFIG_THP_CAP_NEXT_MASK	GENMASK(31, 20)

#define PCIE_CORE_AXI_CONF_BASE		0xc00000
#define PCIE_CORE_OB_REGION_ADDR0	(PCIE_CORE_AXI_CONF_BASE + 0x0)
#define   PCIE_CORE_OB_REGION_ADDR0_NUM_BITS	0x3f
#define   PCIE_CORE_OB_REGION_ADDR0_LO_ADDR	0xffffff00
#define PCIE_CORE_OB_REGION_ADDR1	(PCIE_CORE_AXI_CONF_BASE + 0x4)
#define PCIE_CORE_OB_REGION_DESC0	(PCIE_CORE_AXI_CONF_BASE + 0x8)
#define PCIE_CORE_OB_REGION_DESC1	(PCIE_CORE_AXI_CONF_BASE + 0xc)

#define PCIE_CORE_AXI_INBOUND_BASE	0xc00800
#define PCIE_RP_IB_ADDR0		(PCIE_CORE_AXI_INBOUND_BASE + 0x0)
#define   PCIE_CORE_IB_REGION_ADDR0_NUM_BITS	0x3f
#define   PCIE_CORE_IB_REGION_ADDR0_LO_ADDR	0xffffff00
#define PCIE_RP_IB_ADDR1		(PCIE_CORE_AXI_INBOUND_BASE + 0x4)

/* Size of one AXI Region (not Region 0) */
#define AXI_REGION_SIZE				BIT(20)
/* Size of Region 0, equal to sum of sizes of other regions */
#define AXI_REGION_0_SIZE			(32 * (0x1 << 20))
#define OB_REG_SIZE_SHIFT			5
#define IB_ROOT_PORT_REG_SIZE_SHIFT		3
#define AXI_WRAPPER_IO_WRITE			0x6
#define AXI_WRAPPER_MEM_WRITE			0x2
#define AXI_WRAPPER_TYPE0_CFG			0xa
#define AXI_WRAPPER_TYPE1_CFG			0xb
#define AXI_WRAPPER_NOR_MSG			0xc

#define MAX_AXI_IB_ROOTPORT_REGION_NUM		3
#define MIN_AXI_ADDR_BITS_PASSED		8
#define PCIE_RC_SEND_PME_OFF			0x11960
#define ROCKCHIP_VENDOR_ID			0x1d87
#define PCIE_LINK_IS_L2(x) \
	(((x) & PCIE_CLIENT_DEBUG_LTSSM_MASK) == PCIE_CLIENT_DEBUG_LTSSM_L2)
#define PCIE_LINK_UP(x) \
	(((x) & PCIE_CLIENT_LINK_STATUS_MASK) == PCIE_CLIENT_LINK_STATUS_UP)
#define PCIE_LINK_IS_GEN2(x) \
	(((x) & PCIE_CORE_PL_CONF_SPEED_MASK) == PCIE_CORE_PL_CONF_SPEED_5G)

#define RC_REGION_0_ADDR_TRANS_H		0x00000000
#define RC_REGION_0_ADDR_TRANS_L		0x00000000
#define RC_REGION_0_PASS_BITS			(25 - 1)
#define RC_REGION_0_TYPE_MASK			GENMASK(3, 0)
#define MAX_AXI_WRAPPER_REGION_NUM		33

#define ROCKCHIP_PCIE_MSG_ROUTING_TO_RC		0x0
#define ROCKCHIP_PCIE_MSG_ROUTING_VIA_ADDR		0x1
#define ROCKCHIP_PCIE_MSG_ROUTING_VIA_ID		0x2
#define ROCKCHIP_PCIE_MSG_ROUTING_BROADCAST		0x3
#define ROCKCHIP_PCIE_MSG_ROUTING_LOCAL_INTX		0x4
#define ROCKCHIP_PCIE_MSG_ROUTING_PME_ACK		0x5
#define ROCKCHIP_PCIE_MSG_CODE_ASSERT_INTA		0x20
#define ROCKCHIP_PCIE_MSG_CODE_ASSERT_INTB		0x21
#define ROCKCHIP_PCIE_MSG_CODE_ASSERT_INTC		0x22
#define ROCKCHIP_PCIE_MSG_CODE_ASSERT_INTD		0x23
#define ROCKCHIP_PCIE_MSG_CODE_DEASSERT_INTA		0x24
#define ROCKCHIP_PCIE_MSG_CODE_DEASSERT_INTB		0x25
#define ROCKCHIP_PCIE_MSG_CODE_DEASSERT_INTC		0x26
#define ROCKCHIP_PCIE_MSG_CODE_DEASSERT_INTD		0x27
#define ROCKCHIP_PCIE_MSG_ROUTING_MASK			GENMASK(7, 5)
#define ROCKCHIP_PCIE_MSG_ROUTING(route) \
	(((route) << 5) & ROCKCHIP_PCIE_MSG_ROUTING_MASK)
#define ROCKCHIP_PCIE_MSG_CODE_MASK			GENMASK(15, 8)
#define ROCKCHIP_PCIE_MSG_CODE(code) \
	(((code) << 8) & ROCKCHIP_PCIE_MSG_CODE_MASK)
#define ROCKCHIP_PCIE_MSG_NO_DATA			BIT(16)

#define ROCKCHIP_PCIE_EP_CMD_STATUS			0x4
#define   ROCKCHIP_PCIE_EP_CMD_STATUS_IS		BIT(19)
#define ROCKCHIP_PCIE_EP_MSI_CTRL_REG			0x90
#define   ROCKCHIP_PCIE_EP_MSI_CTRL_MMC_OFFSET		17
#define   ROCKCHIP_PCIE_EP_MSI_CTRL_MMC_MASK		GENMASK(19, 17)
#define   ROCKCHIP_PCIE_EP_MSI_CTRL_MME_OFFSET		20
#define   ROCKCHIP_PCIE_EP_MSI_CTRL_MME_MASK		GENMASK(22, 20)
#define   ROCKCHIP_PCIE_EP_MSI_CTRL_ME				BIT(16)
#define   ROCKCHIP_PCIE_EP_MSI_CTRL_MASK_MSI_CAP	BIT(24)
#define ROCKCHIP_PCIE_EP_DUMMY_IRQ_ADDR				0x1
#define ROCKCHIP_PCIE_EP_PCI_LEGACY_IRQ_ADDR		0x3
#define ROCKCHIP_PCIE_EP_FUNC_BASE(fn)	(((fn) << 12) & GENMASK(19, 12))
#define ROCKCHIP_PCIE_AT_IB_EP_FUNC_BAR_ADDR0(fn, bar) \
	(PCIE_RC_RP_ATS_BASE + 0x0840 + (fn) * 0x0040 + (bar) * 0x0008)
#define ROCKCHIP_PCIE_AT_IB_EP_FUNC_BAR_ADDR1(fn, bar) \
	(PCIE_RC_RP_ATS_BASE + 0x0844 + (fn) * 0x0040 + (bar) * 0x0008)
#define ROCKCHIP_PCIE_AT_OB_REGION_PCI_ADDR0(r) \
	(PCIE_RC_RP_ATS_BASE + 0x0000 + ((r) & 0x1f) * 0x0020)
#define ROCKCHIP_PCIE_AT_OB_REGION_PCI_ADDR0_DEVFN_MASK	GENMASK(19, 12)
#define ROCKCHIP_PCIE_AT_OB_REGION_PCI_ADDR0_DEVFN(devfn) \
	(((devfn) << 12) & \
		 ROCKCHIP_PCIE_AT_OB_REGION_PCI_ADDR0_DEVFN_MASK)
#define ROCKCHIP_PCIE_AT_OB_REGION_PCI_ADDR0_BUS_MASK	GENMASK(27, 20)
#define ROCKCHIP_PCIE_AT_OB_REGION_PCI_ADDR0_BUS(bus) \
		(((bus) << 20) & ROCKCHIP_PCIE_AT_OB_REGION_PCI_ADDR0_BUS_MASK)
#define ROCKCHIP_PCIE_AT_OB_REGION_PCI_ADDR1(r) \
		(PCIE_RC_RP_ATS_BASE + 0x0004 + ((r) & 0x1f) * 0x0020)
#define ROCKCHIP_PCIE_AT_OB_REGION_DESC0_HARDCODED_RID	BIT(23)
#define ROCKCHIP_PCIE_AT_OB_REGION_DESC0_DEVFN_MASK	GENMASK(31, 24)
#define ROCKCHIP_PCIE_AT_OB_REGION_DESC0_DEVFN(devfn) \
		(((devfn) << 24) & ROCKCHIP_PCIE_AT_OB_REGION_DESC0_DEVFN_MASK)
#define ROCKCHIP_PCIE_AT_OB_REGION_DESC0(r) \
		(PCIE_RC_RP_ATS_BASE + 0x0008 + ((r) & 0x1f) * 0x0020)
#define ROCKCHIP_PCIE_AT_OB_REGION_DESC1(r)	\
		(PCIE_RC_RP_ATS_BASE + 0x000c + ((r) & 0x1f) * 0x0020)
#define ROCKCHIP_PCIE_AT_OB_REGION_CPU_ADDR0(r) \
		(PCIE_RC_RP_ATS_BASE + 0x0018 + ((r) & 0x1f) * 0x0020)
#define ROCKCHIP_PCIE_AT_OB_REGION_CPU_ADDR1(r) \
		(PCIE_RC_RP_ATS_BASE + 0x001c + ((r) & 0x1f) * 0x0020)

#define ROCKCHIP_PCIE_CORE_EP_FUNC_BAR_CFG0(fn) \
		(PCIE_CORE_CTRL_MGMT_BASE + 0x0240 + (fn) * 0x0008)
#define ROCKCHIP_PCIE_CORE_EP_FUNC_BAR_CFG1(fn) \
		(PCIE_CORE_CTRL_MGMT_BASE + 0x0244 + (fn) * 0x0008)
#define ROCKCHIP_PCIE_CORE_EP_FUNC_BAR_CFG_BAR_APERTURE_MASK(b) \
		(GENMASK(4, 0) << ((b) * 8))
#define ROCKCHIP_PCIE_CORE_EP_FUNC_BAR_CFG_BAR_APERTURE(b, a) \
		(((a) << ((b) * 8)) & \
		 ROCKCHIP_PCIE_CORE_EP_FUNC_BAR_CFG_BAR_APERTURE_MASK(b))
#define ROCKCHIP_PCIE_CORE_EP_FUNC_BAR_CFG_BAR_CTRL_MASK(b) \
		(GENMASK(7, 5) << ((b) * 8))
#define ROCKCHIP_PCIE_CORE_EP_FUNC_BAR_CFG_BAR_CTRL(b, c) \
		(((c) << ((b) * 8 + 5)) & \
		 ROCKCHIP_PCIE_CORE_EP_FUNC_BAR_CFG_BAR_CTRL_MASK(b))

struct rockchip_pcie {
	void	__iomem *reg_base;		/* DT axi-base */
	void	__iomem *apb_base;		/* DT apb-base */
	bool    legacy_phy;
	struct  phy *phys[MAX_LANE_NUM];
	struct	reset_control *core_rst;
	struct	reset_control *mgmt_rst;
	struct	reset_control *mgmt_sticky_rst;
	struct	reset_control *pipe_rst;
	struct	reset_control *pm_rst;
	struct	reset_control *aclk_rst;
	struct	reset_control *pclk_rst;
	struct	clk *aclk_pcie;
	struct	clk *aclk_perf_pcie;
	struct	clk *hclk_pcie;
	struct	clk *clk_pcie_pm;
	struct	regulator *vpcie12v; /* 12V power supply */
	struct	regulator *vpcie3v3; /* 3.3V power supply */
	struct	regulator *vpcie1v8; /* 1.8V power supply */
	struct	regulator *vpcie0v9; /* 0.9V power supply */
	struct	gpio_desc *ep_gpio;
	u32	lanes;
	u8      lanes_map;
	int	link_gen;
	struct	device *dev;
	struct	irq_domain *irq_domain;
	int     offset;
	void    __iomem *msg_region;
	phys_addr_t msg_bus_addr;
	bool is_rc;
	struct resource *mem_res;
};

static u32 rockchip_pcie_read(struct rockchip_pcie *rockchip, u32 reg)
{
	return readl(rockchip->apb_base + reg);
}

static void rockchip_pcie_write(struct rockchip_pcie *rockchip, u32 val,
				u32 reg)
{
	writel(val, rockchip->apb_base + reg);
}

int rockchip_pcie_parse_dt(struct rockchip_pcie *rockchip);
int rockchip_pcie_init_port(struct rockchip_pcie *rockchip);
int rockchip_pcie_get_phys(struct rockchip_pcie *rockchip);
void rockchip_pcie_deinit_phys(struct rockchip_pcie *rockchip);
int rockchip_pcie_enable_clocks(struct rockchip_pcie *rockchip);
void rockchip_pcie_disable_clocks(void *data);
void rockchip_pcie_cfg_configuration_accesses(
		struct rockchip_pcie *rockchip, u32 type);

#endif /* _PCIE_ROCKCHIP_H */
