/* SPDX-License-Identifier: GPL-2.0 */
/*
 *	pci.h
 *
 *	PCI defines and function prototypes
 *	Copyright 1994, Drew Eckhardt
 *	Copyright 1997--1999 Martin Mares <mj@ucw.cz>
 *
 *	PCI Express ASPM defines and function prototypes
 *	Copyright (c) 2007 Intel Corp.
 *		Zhang Yanmin (yanmin.zhang@intel.com)
 *		Shaohua Li (shaohua.li@intel.com)
 *
 *	For more information, please consult the following manuals (look at
 *	http://www.pcisig.com/ for how to get them):
 *
 *	PCI BIOS Specification
 *	PCI Local Bus Specification
 *	PCI to PCI Bridge Specification
 *	PCI Express Specification
 *	PCI System Design Guide
 */
#ifndef AMDKCL_PCI_H
#define AMDKCL_PCI_H

#include <linux/pci.h>
#include <linux/version.h>

#ifndef PCI_EXP_DEVCAP2_ATOMIC_ROUTE
#define PCI_EXP_DEVCAP2_ATOMIC_ROUTE	0x00000040 /* Atomic Op routing */
#endif
#ifndef PCI_EXP_DEVCAP2_ATOMIC_COMP32
#define PCI_EXP_DEVCAP2_ATOMIC_COMP32	0x00000080 /* 32b AtomicOp completion */
#endif
#ifndef PCI_EXP_DEVCAP2_ATOMIC_COMP64
#define PCI_EXP_DEVCAP2_ATOMIC_COMP64	0x00000100 /* 64b AtomicOp completion*/
#endif
#ifndef PCI_EXP_DEVCAP2_ATOMIC_COMP128
#define PCI_EXP_DEVCAP2_ATOMIC_COMP128	0x00000200 /* 128b AtomicOp completion*/
#endif
#ifndef PCI_EXP_DEVCTL2_ATOMIC_REQ
#define PCI_EXP_DEVCTL2_ATOMIC_REQ	0x0040	/* Set Atomic requests */
#endif
#ifndef PCI_EXP_DEVCTL2_ATOMIC_BLOCK
#define PCI_EXP_DEVCTL2_ATOMIC_BLOCK	0x0040	/* Block AtomicOp on egress */
#endif
#ifndef PCI_EXP_LNKCTL2_ENTER_COMP
#define  PCI_EXP_LNKCTL2_ENTER_COMP   0x0010 /* Enter Compliance */
#endif
#ifndef PCI_EXP_LNKCTL2_TX_MARGIN
#define  PCI_EXP_LNKCTL2_TX_MARGIN    0x0380 /* Transmit Margin */
#endif

#ifndef PCI_EXP_LNKCTL2_TLS
#define  PCI_EXP_LNKCTL2_TLS          0x000f
#endif
#ifndef PCI_EXP_LNKCTL2_TLS_2_5GT
#define  PCI_EXP_LNKCTL2_TLS_2_5GT    0x0001 /* Supported Speed 2.5GT/s */
#endif
#ifndef PCI_EXP_LNKCTL2_TLS_5_0GT
#define  PCI_EXP_LNKCTL2_TLS_5_0GT    0x0002 /* Supported Speed 5GT/s */
#endif
#ifndef PCI_EXP_LNKCTL2_TLS_8_0GT
#define  PCI_EXP_LNKCTL2_TLS_8_0GT    0x0003 /* Supported Speed 8GT/s */
#endif

#define PCIE_SPEED_16_0GT		0x17
#define PCIE_SPEED_32_0GT		0x18

#ifndef PCI_EXP_LNKCAP2_SLS_16_0GB
#define PCI_EXP_LNKCAP2_SLS_16_0GB	0x00000010	/* Supported Speed 16GT/s */
#endif
#ifndef PCI_EXP_LNKCAP_SLS_16_0GB
#define PCI_EXP_LNKCAP_SLS_16_0GB	0x00000004	/* LNKCAP2 SLS Vector bit 3 */
#endif
#ifndef PCI_EXP_LNKSTA_CLS_16_0GB
#define PCI_EXP_LNKSTA_CLS_16_0GB	0x0004		/* Current Link Speed 16.0GT/s */
#endif

/* PCIe link information */
#ifndef PCIE_SPEED2STR
#define PCIE_SPEED2STR(speed) \
	((speed) == PCIE_SPEED_16_0GT ? "16 GT/s" : \
	(speed) == PCIE_SPEED_8_0GT ? "8 GT/s" : \
	(speed) == PCIE_SPEED_5_0GT ? "5 GT/s" : \
	(speed) == PCIE_SPEED_2_5GT ? "2.5 GT/s" : \
	"Unknown speed")
#endif

/* PCIe speed to Mb/s reduced by encoding overhead */
#ifndef PCIE_SPEED2MBS_ENC
#define PCIE_SPEED2MBS_ENC(speed) \
	((speed) == PCIE_SPEED_16_0GT ? 16000*128/130 : \
	(speed) == PCIE_SPEED_8_0GT  ?  8000*128/130 : \
	(speed) == PCIE_SPEED_5_0GT  ?  5000*8/10 : \
	(speed) == PCIE_SPEED_2_5GT  ?  2500*8/10 : \
	0)
#endif

#if !defined(HAVE_PCIE_GET_SPEED_AND_WIDTH_CAP)
extern enum pci_bus_speed (*_kcl_pcie_get_speed_cap)(struct pci_dev *dev);
extern enum pcie_link_width (*_kcl_pcie_get_width_cap)(struct pci_dev *dev);
#endif

static inline enum pci_bus_speed kcl_pcie_get_speed_cap(struct pci_dev *dev)
{
#if defined(HAVE_PCIE_GET_SPEED_AND_WIDTH_CAP)
	return pcie_get_speed_cap(dev);
#else
	return _kcl_pcie_get_speed_cap(dev);
#endif
}

static inline enum pcie_link_width kcl_pcie_get_width_cap(struct pci_dev *dev)
{
#if defined(HAVE_PCIE_GET_SPEED_AND_WIDTH_CAP)
	return pcie_get_width_cap(dev);
#else
	return _kcl_pcie_get_width_cap(dev);
#endif
}

#if !defined(HAVE_PCIE_ENABLE_ATOMIC_OPS_TO_ROOT)
int _kcl_pci_enable_atomic_ops_to_root(struct pci_dev *dev, u32 comp_caps);
static inline
int pci_enable_atomic_ops_to_root(struct pci_dev *dev, u32 cap_mask)
{
	return _kcl_pci_enable_atomic_ops_to_root(dev, cap_mask);
}
#endif

/* Copied from v3.12-rc2-29-gc6bde215acfd include/linux/pci.h */
#if !defined(HAVE_PCI_UPSTREAM_BRIDGE)
static inline struct pci_dev *pci_upstream_bridge(struct pci_dev *dev)
{
	dev = pci_physfn(dev);
	if (pci_is_root_bus(dev->bus))
		return NULL;

	return dev->bus->self;
}
#endif

#if !defined(HAVE_PCIE_BANDWIDTH_AVAILABLE)
u32 _kcl_pcie_bandwidth_available(struct pci_dev *dev, struct pci_dev **limiting_dev,
			     enum pci_bus_speed *speed,
			     enum pcie_link_width *width);
static inline
u32 pcie_bandwidth_available(struct pci_dev *dev, struct pci_dev **limiting_dev,
				enum pci_bus_speed *speed,
				enum pcie_link_width *width)
{
	return _kcl_pcie_bandwidth_available(dev, limiting_dev, speed, width);
}
#endif

#if !defined(HAVE_PCI_CONFIGURE_EXTENDED_TAGS)
void _kcl_pci_configure_extended_tags(struct pci_dev *dev);
#endif

static inline void kcl_pci_configure_extended_tags(struct pci_dev *dev)
{
#if !defined(HAVE_PCI_CONFIGURE_EXTENDED_TAGS)
	_kcl_pci_configure_extended_tags(dev);
#endif
}

/* Copied from v5.1-rc1-5-g4e544bac8267 include/linux/pci.h */
#if !defined(HAVE_PCI_DEV_ID)
static inline u16 pci_dev_id(struct pci_dev *dev)
{
	return PCI_DEVID(dev->bus->number, dev->devfn);
}
#endif /* HAVE_PCI_DEV_ID */

#ifndef HAVE_PCI_IS_THUNDERBOLD_ATTACHED
static inline bool pci_is_thunderbolt_attached(struct pci_dev *pdev)
{
	return false;
}
#endif /* HAVE_PCI_IS_THUNDERBOLD_ATTACHED */

#ifndef HAVE_PCI_PR3_PRESENT
#ifdef CONFIG_ACPI
bool _kcl_pci_pr3_present(struct pci_dev *pdev);
static inline bool pci_pr3_present(struct pci_dev *pdev)
{
	return _kcl_pci_pr3_present(pdev);
}
#else
static inline bool pci_pr3_present(struct pci_dev *pdev) { return false; }
#endif
#endif /* HAVE_PCI_PR3_PRESENT */

#ifndef PCI_EXP_LNKCAP_SLS_8_0GB
#define AMDKCL_CREATE_MEASURE_FILE
#define  PCI_EXP_LNKCAP_SLS_8_0GB 0x00000003 /* LNKCAP2 SLS Vector bit 2 */
int  _kcl_pci_create_measure_file(struct pci_dev *pdev);
void _kcl_pci_remove_measure_file(struct pci_dev *pdev);
#endif

static inline int kcl_pci_create_measure_file(struct pci_dev *pdev)
{
#ifdef AMDKCL_CREATE_MEASURE_FILE
	return _kcl_pci_create_measure_file(pdev);
#else
	return 0;
#endif
}

static inline void kcl_pci_remove_measure_file(struct pci_dev *pdev)
{
#ifdef AMDKCL_CREATE_MEASURE_FILE
	_kcl_pci_remove_measure_file(pdev);
#endif
}

/*
 * v4.18-rc1-3-gb1277a226d8c PCI: Cleanup PCI_REBAR_CTRL_BAR_SHIFT handling
 * v4.18-rc1-2-gd3252ace0bc6 PCI: Restore resized BAR state on resume
 * v4.14-rc3-3-g8bb705e3e79d PCI: Add pci_resize_resource() for resizing BARs
 * v4.14-rc3-2-g276b738deb5b PCI: Add resizable BAR infrastructure
 */
#ifdef PCI_REBAR_CTRL_BAR_SHIFT
#define AMDKCL_ENABLE_RESIZE_FB_BAR

/* Copied from 192f1bf7559e895d51f81c3976c5892c8b1e0601 include/linux/pci.h */
#ifndef HAVE_PCI_REBAR_BYTES_TO_SIZE
static inline int pci_rebar_bytes_to_size(u64 bytes)
{
	bytes = roundup_pow_of_two(bytes);

	/* Return BAR size as defined in the resizable BAR specification */
	return max(ilog2(bytes), 20) - 20;
}

/*
 * 907830b0fc9e PCI: Add a REBAR size quirk for Sapphire RX 5600 XT Pulse
 * 8fbdbb66f8c1 PCI: Export pci_rebar_get_possible_sizes()
 */
u32 _kcl_pci_rebar_get_possible_sizes(struct pci_dev *pdev, int bar);
static inline
u32 pci_rebar_get_possible_sizes(struct pci_dev *pdev, int bar)
{
	return _kcl_pci_rebar_get_possible_sizes(pdev, bar);
}
#endif

#endif /* PCI_REBAR_CTRL_BAR_SHIFT */

#endif /* AMDKCL_PCI_H */
