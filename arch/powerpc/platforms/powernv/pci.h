/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __POWERNV_PCI_H
#define __POWERNV_PCI_H

#include <linux/compiler.h>		/* for __printf */
#include <linux/iommu.h>
#include <asm/iommu.h>
#include <asm/msi_bitmap.h>

struct pci_dn;

enum pnv_phb_type {
	PNV_PHB_IODA1,
	PNV_PHB_IODA2,
	PNV_PHB_NPU_OCAPI,
};

/* Precise PHB model for error management */
enum pnv_phb_model {
	PNV_PHB_MODEL_UNKNOWN,
	PNV_PHB_MODEL_P7IOC,
	PNV_PHB_MODEL_PHB3,
};

#define PNV_PCI_DIAG_BUF_SIZE	8192
#define PNV_IODA_PE_DEV		(1 << 0)	/* PE has single PCI device	*/
#define PNV_IODA_PE_BUS		(1 << 1)	/* PE has primary PCI bus	*/
#define PNV_IODA_PE_BUS_ALL	(1 << 2)	/* PE has subordinate buses	*/
#define PNV_IODA_PE_MASTER	(1 << 3)	/* Master PE in compound case	*/
#define PNV_IODA_PE_SLAVE	(1 << 4)	/* Slave PE in compound case	*/
#define PNV_IODA_PE_VF		(1 << 5)	/* PE for one VF 		*/

/*
 * A brief note on PNV_IODA_PE_BUS_ALL
 *
 * This is needed because of the behaviour of PCIe-to-PCI bridges. The PHB uses
 * the Requester ID field of the PCIe request header to determine the device
 * (and PE) that initiated a DMA. In legacy PCI individual memory read/write
 * requests aren't tagged with the RID. To work around this the PCIe-to-PCI
 * bridge will use (secondary_bus_no << 8) | 0x00 as the RID on the PCIe side.
 *
 * PCIe-to-X bridges have a similar issue even though PCI-X requests also have
 * a RID in the transaction header. The PCIe-to-X bridge is permitted to "take
 * ownership" of a transaction by a PCI-X device when forwarding it to the PCIe
 * side of the bridge.
 *
 * To work around these problems we use the BUS_ALL flag since every subordinate
 * bus of the bridge should go into the same PE.
 */

/* Indicates operations are frozen for a PE: MMIO in PESTA & DMA in PESTB. */
#define PNV_IODA_STOPPED_STATE	0x8000000000000000

/* Data associated with a PE, including IOMMU tracking etc.. */
struct pnv_phb;
struct pnv_ioda_pe {
	unsigned long		flags;
	struct pnv_phb		*phb;
	int			device_count;

	/* A PE can be associated with a single device or an
	 * entire bus (& children). In the former case, pdev
	 * is populated, in the later case, pbus is.
	 */
#ifdef CONFIG_PCI_IOV
	struct pci_dev          *parent_dev;
#endif
	struct pci_dev		*pdev;
	struct pci_bus		*pbus;

	/* Effective RID (device RID for a device PE and base bus
	 * RID with devfn 0 for a bus PE)
	 */
	unsigned int		rid;

	/* PE number */
	unsigned int		pe_number;

	/* "Base" iommu table, ie, 4K TCEs, 32-bit DMA */
	struct iommu_table_group table_group;

	/* 64-bit TCE bypass region */
	bool			tce_bypass_enabled;
	uint64_t		tce_bypass_base;

	/*
	 * Used to track whether we've done DMA setup for this PE or not. We
	 * want to defer allocating TCE tables, etc until we've added a
	 * non-bridge device to the PE.
	 */
	bool			dma_setup_done;

	/* MSIs. MVE index is identical for 32 and 64 bit MSI
	 * and -1 if not supported. (It's actually identical to the
	 * PE number)
	 */
	int			mve_number;

	/* PEs in compound case */
	struct pnv_ioda_pe	*master;
	struct list_head	slaves;

	/* Link in list of PE#s */
	struct list_head	list;
};

#define PNV_PHB_FLAG_EEH	(1 << 0)

struct pnv_phb {
	struct pci_controller	*hose;
	enum pnv_phb_type	type;
	enum pnv_phb_model	model;
	u64			hub_id;
	u64			opal_id;
	int			flags;
	void __iomem		*regs;
	u64			regs_phys;
	spinlock_t		lock;

#ifdef CONFIG_DEBUG_FS
	int			has_dbgfs;
	struct dentry		*dbgfs;
#endif

	unsigned int		msi_base;
	unsigned int		msi32_support;
	struct msi_bitmap	msi_bmp;
	int (*msi_setup)(struct pnv_phb *phb, struct pci_dev *dev,
			 unsigned int hwirq, unsigned int virq,
			 unsigned int is_64, struct msi_msg *msg);
	int (*init_m64)(struct pnv_phb *phb);
	int (*get_pe_state)(struct pnv_phb *phb, int pe_no);
	void (*freeze_pe)(struct pnv_phb *phb, int pe_no);
	int (*unfreeze_pe)(struct pnv_phb *phb, int pe_no, int opt);

	struct {
		/* Global bridge info */
		unsigned int		total_pe_num;
		unsigned int		reserved_pe_idx;
		unsigned int		root_pe_idx;

		/* 32-bit MMIO window */
		unsigned int		m32_size;
		unsigned int		m32_segsize;
		unsigned int		m32_pci_base;

		/* 64-bit MMIO window */
		unsigned int		m64_bar_idx;
		unsigned long		m64_size;
		unsigned long		m64_segsize;
		unsigned long		m64_base;
#define MAX_M64_BARS 64
		unsigned long		m64_bar_alloc;

		/* IO ports */
		unsigned int		io_size;
		unsigned int		io_segsize;
		unsigned int		io_pci_base;

		/* PE allocation */
		struct mutex		pe_alloc_mutex;
		unsigned long		*pe_alloc;
		struct pnv_ioda_pe	*pe_array;

		/* M32 & IO segment maps */
		unsigned int		*m64_segmap;
		unsigned int		*m32_segmap;
		unsigned int		*io_segmap;

		/* DMA32 segment maps - IODA1 only */
		unsigned int		dma32_count;
		unsigned int		*dma32_segmap;

		/* IRQ chip */
		int			irq_chip_init;
		struct irq_chip		irq_chip;

		/* Sorted list of used PE's based
		 * on the sequence of creation
		 */
		struct list_head	pe_list;
		struct mutex            pe_list_mutex;

		/* Reverse map of PEs, indexed by {bus, devfn} */
		unsigned int		pe_rmap[0x10000];
	} ioda;

	/* PHB and hub diagnostics */
	unsigned int		diag_data_size;
	u8			*diag_data;
};


/* IODA PE management */

static inline bool pnv_pci_is_m64(struct pnv_phb *phb, struct resource *r)
{
	/*
	 * WARNING: We cannot rely on the resource flags. The Linux PCI
	 * allocation code sometimes decides to put a 64-bit prefetchable
	 * BAR in the 32-bit window, so we have to compare the addresses.
	 *
	 * For simplicity we only test resource start.
	 */
	return (r->start >= phb->ioda.m64_base &&
		r->start < (phb->ioda.m64_base + phb->ioda.m64_size));
}

static inline bool pnv_pci_is_m64_flags(unsigned long resource_flags)
{
	unsigned long flags = (IORESOURCE_MEM_64 | IORESOURCE_PREFETCH);

	return (resource_flags & flags) == flags;
}

int pnv_ioda_configure_pe(struct pnv_phb *phb, struct pnv_ioda_pe *pe);
int pnv_ioda_deconfigure_pe(struct pnv_phb *phb, struct pnv_ioda_pe *pe);

void pnv_pci_ioda2_setup_dma_pe(struct pnv_phb *phb, struct pnv_ioda_pe *pe);
void pnv_pci_ioda2_release_pe_dma(struct pnv_ioda_pe *pe);

struct pnv_ioda_pe *pnv_ioda_alloc_pe(struct pnv_phb *phb, int count);
void pnv_ioda_free_pe(struct pnv_ioda_pe *pe);

#ifdef CONFIG_PCI_IOV
/*
 * For SR-IOV we want to put each VF's MMIO resource in to a separate PE.
 * This requires a bit of acrobatics with the MMIO -> PE configuration
 * and this structure is used to keep track of it all.
 */
struct pnv_iov_data {
	/* number of VFs enabled */
	u16     num_vfs;

	/* pointer to the array of VF PEs. num_vfs long*/
	struct pnv_ioda_pe *vf_pe_arr;

	/* Did we map the VF BAR with single-PE IODA BARs? */
	bool    m64_single_mode[PCI_SRIOV_NUM_BARS];

	/*
	 * True if we're using any segmented windows. In that case we need
	 * shift the start of the IOV resource the segment corresponding to
	 * the allocated PE.
	 */
	bool    need_shift;

	/*
	 * Bit mask used to track which m64 windows are used to map the
	 * SR-IOV BARs for this device.
	 */
	DECLARE_BITMAP(used_m64_bar_mask, MAX_M64_BARS);

	/*
	 * If we map the SR-IOV BARs with a segmented window then
	 * parts of that window will be "claimed" by other PEs.
	 *
	 * "holes" here is used to reserve the leading portion
	 * of the window that is used by other (non VF) PEs.
	 */
	struct resource holes[PCI_SRIOV_NUM_BARS];
};

static inline struct pnv_iov_data *pnv_iov_get(struct pci_dev *pdev)
{
	return pdev->dev.archdata.iov_data;
}

void pnv_pci_ioda_fixup_iov(struct pci_dev *pdev);
resource_size_t pnv_pci_iov_resource_alignment(struct pci_dev *pdev, int resno);

int pnv_pcibios_sriov_enable(struct pci_dev *pdev, u16 num_vfs);
int pnv_pcibios_sriov_disable(struct pci_dev *pdev);
#endif /* CONFIG_PCI_IOV */

extern struct pci_ops pnv_pci_ops;

void pnv_pci_dump_phb_diag_data(struct pci_controller *hose,
				unsigned char *log_buff);
int pnv_pci_cfg_read(struct pci_dn *pdn,
		     int where, int size, u32 *val);
int pnv_pci_cfg_write(struct pci_dn *pdn,
		      int where, int size, u32 val);
extern struct iommu_table *pnv_pci_table_alloc(int nid);

extern void pnv_pci_init_ioda_hub(struct device_node *np);
extern void pnv_pci_init_ioda2_phb(struct device_node *np);
extern void pnv_pci_init_npu2_opencapi_phb(struct device_node *np);
extern void pnv_pci_reset_secondary_bus(struct pci_dev *dev);
extern int pnv_eeh_phb_reset(struct pci_controller *hose, int option);

extern int pnv_setup_msi_irqs(struct pci_dev *pdev, int nvec, int type);
extern void pnv_teardown_msi_irqs(struct pci_dev *pdev);
extern struct pnv_ioda_pe *pnv_pci_bdfn_to_pe(struct pnv_phb *phb, u16 bdfn);
extern struct pnv_ioda_pe *pnv_ioda_get_pe(struct pci_dev *dev);
extern void pnv_set_msi_irq_chip(struct pnv_phb *phb, unsigned int virq);
extern unsigned long pnv_pci_ioda2_get_table_size(__u32 page_shift,
		__u64 window_size, __u32 levels);
extern int pnv_eeh_post_init(void);

__printf(3, 4)
extern void pe_level_printk(const struct pnv_ioda_pe *pe, const char *level,
			    const char *fmt, ...);
#define pe_err(pe, fmt, ...)					\
	pe_level_printk(pe, KERN_ERR, fmt, ##__VA_ARGS__)
#define pe_warn(pe, fmt, ...)					\
	pe_level_printk(pe, KERN_WARNING, fmt, ##__VA_ARGS__)
#define pe_info(pe, fmt, ...)					\
	pe_level_printk(pe, KERN_INFO, fmt, ##__VA_ARGS__)

/* pci-ioda-tce.c */
#define POWERNV_IOMMU_DEFAULT_LEVELS	2
#define POWERNV_IOMMU_MAX_LEVELS	5

extern int pnv_tce_build(struct iommu_table *tbl, long index, long npages,
		unsigned long uaddr, enum dma_data_direction direction,
		unsigned long attrs);
extern void pnv_tce_free(struct iommu_table *tbl, long index, long npages);
extern int pnv_tce_xchg(struct iommu_table *tbl, long index,
		unsigned long *hpa, enum dma_data_direction *direction,
		bool alloc);
extern __be64 *pnv_tce_useraddrptr(struct iommu_table *tbl, long index,
		bool alloc);
extern unsigned long pnv_tce_get(struct iommu_table *tbl, long index);

extern long pnv_pci_ioda2_table_alloc_pages(int nid, __u64 bus_offset,
		__u32 page_shift, __u64 window_size, __u32 levels,
		bool alloc_userspace_copy, struct iommu_table *tbl);
extern void pnv_pci_ioda2_table_free_pages(struct iommu_table *tbl);

extern long pnv_pci_link_table_and_group(int node, int num,
		struct iommu_table *tbl,
		struct iommu_table_group *table_group);
extern void pnv_pci_unlink_table_and_group(struct iommu_table *tbl,
		struct iommu_table_group *table_group);
extern void pnv_pci_setup_iommu_table(struct iommu_table *tbl,
		void *tce_mem, u64 tce_size,
		u64 dma_offset, unsigned int page_shift);

extern unsigned long pnv_ioda_parse_tce_sizes(struct pnv_phb *phb);

static inline struct pnv_phb *pci_bus_to_pnvhb(struct pci_bus *bus)
{
	struct pci_controller *hose = bus->sysdata;

	if (hose)
		return hose->private_data;

	return NULL;
}

#endif /* __POWERNV_PCI_H */
