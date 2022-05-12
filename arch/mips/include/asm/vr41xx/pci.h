/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Include file for NEC VR4100 series PCI Control Unit.
 *
 *  Copyright (C) 2004-2005  Yoichi Yuasa <yuasa@linux-mips.org>
 */
#ifndef __NEC_VR41XX_PCI_H
#define __NEC_VR41XX_PCI_H

#define PCI_MASTER_ADDRESS_MASK 0x7fffffffU

struct pci_master_address_conversion {
	uint32_t bus_base_address;
	uint32_t address_mask;
	uint32_t pci_base_address;
};

struct pci_target_address_conversion {
	uint32_t address_mask;
	uint32_t bus_base_address;
};

typedef enum {
	CANNOT_LOCK_FROM_DEVICE,
	CAN_LOCK_FROM_DEVICE,
} pci_exclusive_access_t;

struct pci_mailbox_address {
	uint32_t base_address;
};

struct pci_target_address_window {
	uint32_t base_address;
};

typedef enum {
	PCI_ARBITRATION_MODE_FAIR,
	PCI_ARBITRATION_MODE_ALTERNATE_0,
	PCI_ARBITRATION_MODE_ALTERNATE_B,
} pci_arbiter_priority_control_t;

typedef enum {
	PCI_TAKE_AWAY_GNT_DISABLE,
	PCI_TAKE_AWAY_GNT_ENABLE,
} pci_take_away_gnt_mode_t;

struct pci_controller_unit_setup {
	struct pci_master_address_conversion *master_memory1;
	struct pci_master_address_conversion *master_memory2;

	struct pci_target_address_conversion *target_memory1;
	struct pci_target_address_conversion *target_memory2;

	struct pci_master_address_conversion *master_io;

	pci_exclusive_access_t exclusive_access;

	uint32_t pci_clock_max;
	uint8_t wait_time_limit_from_irdy_to_trdy;	/* Only VR4122 is supported */

	struct pci_mailbox_address *mailbox;
	struct pci_target_address_window *target_window1;
	struct pci_target_address_window *target_window2;

	uint8_t master_latency_timer;
	uint8_t retry_limit;

	pci_arbiter_priority_control_t arbiter_priority_control;
	pci_take_away_gnt_mode_t take_away_gnt_mode;

	struct resource *mem_resource;
	struct resource *io_resource;
};

extern void vr41xx_pciu_setup(struct pci_controller_unit_setup *setup);

#endif /* __NEC_VR41XX_PCI_H */
