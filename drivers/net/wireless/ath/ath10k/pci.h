/* SPDX-License-Identifier: ISC */
/*
 * Copyright (c) 2005-2011 Atheros Communications Inc.
 * Copyright (c) 2011-2017 Qualcomm Atheros, Inc.
 */

#ifndef _PCI_H_
#define _PCI_H_

#include <linux/interrupt.h>
#include <linux/mutex.h>

#include "hw.h"
#include "ce.h"
#include "ahb.h"

/*
 * maximum number of bytes that can be
 * handled atomically by DiagRead/DiagWrite
 */
#define DIAG_TRANSFER_LIMIT 2048

struct bmi_xfer {
	bool tx_done;
	bool rx_done;
	bool wait_for_resp;
	u32 resp_len;
};

/*
 * PCI-specific Target state
 *
 * NOTE: Structure is shared between Host software and Target firmware!
 *
 * Much of this may be of interest to the Host so
 * HOST_INTEREST->hi_interconnect_state points here
 * (and all members are 32-bit quantities in order to
 * facilitate Host access). In particular, Host software is
 * required to initialize pipe_cfg_addr and svc_to_pipe_map.
 */
struct pcie_state {
	/* Pipe configuration Target address */
	/* NB: ce_pipe_config[CE_COUNT] */
	u32 pipe_cfg_addr;

	/* Service to pipe map Target address */
	/* NB: service_to_pipe[PIPE_TO_CE_MAP_CN] */
	u32 svc_to_pipe_map;

	/* number of MSI interrupts requested */
	u32 msi_requested;

	/* number of MSI interrupts granted */
	u32 msi_granted;

	/* Message Signalled Interrupt address */
	u32 msi_addr;

	/* Base data */
	u32 msi_data;

	/*
	 * Data for firmware interrupt;
	 * MSI data for other interrupts are
	 * in various SoC registers
	 */
	u32 msi_fw_intr_data;

	/* PCIE_PWR_METHOD_* */
	u32 power_mgmt_method;

	/* PCIE_CONFIG_FLAG_* */
	u32 config_flags;
};

/* PCIE_CONFIG_FLAG definitions */
#define PCIE_CONFIG_FLAG_ENABLE_L1  0x0000001

/* Per-pipe state. */
struct ath10k_pci_pipe {
	/* Handle of underlying Copy Engine */
	struct ath10k_ce_pipe *ce_hdl;

	/* Our pipe number; facilitiates use of pipe_info ptrs. */
	u8 pipe_num;

	/* Convenience back pointer to hif_ce_state. */
	struct ath10k *hif_ce_state;

	size_t buf_sz;

	/* protects compl_free and num_send_allowed */
	spinlock_t pipe_lock;
};

struct ath10k_pci_supp_chip {
	u32 dev_id;
	u32 rev_id;
};

enum ath10k_pci_irq_mode {
	ATH10K_PCI_IRQ_AUTO = 0,
	ATH10K_PCI_IRQ_LEGACY = 1,
	ATH10K_PCI_IRQ_MSI = 2,
};

struct ath10k_pci {
	struct pci_dev *pdev;
	struct device *dev;
	struct ath10k *ar;
	void __iomem *mem;
	size_t mem_len;

	/* Operating interrupt mode */
	enum ath10k_pci_irq_mode oper_irq_mode;

	struct ath10k_pci_pipe pipe_info[CE_COUNT_MAX];

	/* Copy Engine used for Diagnostic Accesses */
	struct ath10k_ce_pipe *ce_diag;
	/* For protecting ce_diag */
	struct mutex ce_diag_mutex;

	struct work_struct dump_work;

	struct ath10k_ce ce;
	struct timer_list rx_post_retry;

	/* Due to HW quirks it is recommended to disable ASPM during device
	 * bootup. To do that the original PCI-E Link Control is stored before
	 * device bootup is executed and re-programmed later.
	 */
	u16 link_ctl;

	/* Protects ps_awake and ps_wake_refcount */
	spinlock_t ps_lock;

	/* The device has a special powersave-oriented register. When device is
	 * considered asleep it drains less power and driver is forbidden from
	 * accessing most MMIO registers. If host were to access them without
	 * waking up the device might scribble over host memory or return
	 * 0xdeadbeef readouts.
	 */
	unsigned long ps_wake_refcount;

	/* Waking up takes some time (up to 2ms in some cases) so it can be bad
	 * for latency. To mitigate this the device isn't immediately allowed
	 * to sleep after all references are undone - instead there's a grace
	 * period after which the powersave register is updated unless some
	 * activity to/from device happened in the meantime.
	 *
	 * Also see comments on ATH10K_PCI_SLEEP_GRACE_PERIOD_MSEC.
	 */
	struct timer_list ps_timer;

	/* MMIO registers are used to communicate with the device. With
	 * intensive traffic accessing powersave register would be a bit
	 * wasteful overhead and would needlessly stall CPU. It is far more
	 * efficient to rely on a variable in RAM and update it only upon
	 * powersave register state changes.
	 */
	bool ps_awake;

	/* pci power save, disable for QCA988X and QCA99X0.
	 * Writing 'false' to this variable avoids frequent locking
	 * on MMIO read/write.
	 */
	bool pci_ps;

	/* Chip specific pci reset routine used to do a safe reset */
	int (*pci_soft_reset)(struct ath10k *ar);

	/* Chip specific pci full reset function */
	int (*pci_hard_reset)(struct ath10k *ar);

	/* chip specific methods for converting target CPU virtual address
	 * space to CE address space
	 */
	u32 (*targ_cpu_to_ce_addr)(struct ath10k *ar, u32 addr);

	struct ce_attr *attr;
	struct ce_pipe_config *pipe_config;
	struct ce_service_to_pipe *serv_to_pipe;

	/* Keep this entry in the last, memory for struct ath10k_ahb is
	 * allocated (ahb support enabled case) in the continuation of
	 * this struct.
	 */
	struct ath10k_ahb ahb[];

};

static inline struct ath10k_pci *ath10k_pci_priv(struct ath10k *ar)
{
	return (struct ath10k_pci *)ar->drv_priv;
}

#define ATH10K_PCI_RX_POST_RETRY_MS 50
#define ATH_PCI_RESET_WAIT_MAX 10 /* ms */
#define PCIE_WAKE_TIMEOUT 30000	/* 30ms */
#define PCIE_WAKE_LATE_US 10000	/* 10ms */

#define BAR_NUM 0

#define CDC_WAR_MAGIC_STR   0xceef0000
#define CDC_WAR_DATA_CE     4

/* Wait up to this many Ms for a Diagnostic Access CE operation to complete */
#define DIAG_ACCESS_CE_TIMEOUT_US 10000 /* 10 ms */
#define DIAG_ACCESS_CE_WAIT_US	50

void ath10k_pci_write32(struct ath10k *ar, u32 offset, u32 value);
void ath10k_pci_soc_write32(struct ath10k *ar, u32 addr, u32 val);
void ath10k_pci_reg_write32(struct ath10k *ar, u32 addr, u32 val);

u32 ath10k_pci_read32(struct ath10k *ar, u32 offset);
u32 ath10k_pci_soc_read32(struct ath10k *ar, u32 addr);
u32 ath10k_pci_reg_read32(struct ath10k *ar, u32 addr);

int ath10k_pci_hif_tx_sg(struct ath10k *ar, u8 pipe_id,
			 struct ath10k_hif_sg_item *items, int n_items);
int ath10k_pci_hif_diag_read(struct ath10k *ar, u32 address, void *buf,
			     size_t buf_len);
int ath10k_pci_diag_write_mem(struct ath10k *ar, u32 address,
			      const void *data, int nbytes);
int ath10k_pci_hif_exchange_bmi_msg(struct ath10k *ar, void *req, u32 req_len,
				    void *resp, u32 *resp_len);
int ath10k_pci_hif_map_service_to_pipe(struct ath10k *ar, u16 service_id,
				       u8 *ul_pipe, u8 *dl_pipe);
void ath10k_pci_hif_get_default_pipe(struct ath10k *ar, u8 *ul_pipe,
				     u8 *dl_pipe);
void ath10k_pci_hif_send_complete_check(struct ath10k *ar, u8 pipe,
					int force);
u16 ath10k_pci_hif_get_free_queue_number(struct ath10k *ar, u8 pipe);
void ath10k_pci_hif_power_down(struct ath10k *ar);
int ath10k_pci_alloc_pipes(struct ath10k *ar);
void ath10k_pci_free_pipes(struct ath10k *ar);
void ath10k_pci_free_pipes(struct ath10k *ar);
void ath10k_pci_rx_replenish_retry(struct timer_list *t);
void ath10k_pci_ce_deinit(struct ath10k *ar);
void ath10k_pci_init_napi(struct ath10k *ar);
int ath10k_pci_init_pipes(struct ath10k *ar);
int ath10k_pci_init_config(struct ath10k *ar);
void ath10k_pci_rx_post(struct ath10k *ar);
void ath10k_pci_flush(struct ath10k *ar);
void ath10k_pci_enable_legacy_irq(struct ath10k *ar);
bool ath10k_pci_irq_pending(struct ath10k *ar);
void ath10k_pci_disable_and_clear_legacy_irq(struct ath10k *ar);
void ath10k_pci_irq_msi_fw_mask(struct ath10k *ar);
int ath10k_pci_wait_for_target_init(struct ath10k *ar);
int ath10k_pci_setup_resource(struct ath10k *ar);
void ath10k_pci_release_resource(struct ath10k *ar);

/* QCA6174 is known to have Tx/Rx issues when SOC_WAKE register is poked too
 * frequently. To avoid this put SoC to sleep after a very conservative grace
 * period. Adjust with great care.
 */
#define ATH10K_PCI_SLEEP_GRACE_PERIOD_MSEC 60

#endif /* _PCI_H_ */
