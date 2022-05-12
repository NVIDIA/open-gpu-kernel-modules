// SPDX-License-Identifier: GPL-2.0
// CCI Cache Coherent Interconnect PMU driver
// Copyright (C) 2013-2018 Arm Ltd.
// Author: Punit Agrawal <punit.agrawal@arm.com>, Suzuki Poulose <suzuki.poulose@arm.com>

#include <linux/arm-cci.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/perf_event.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#define DRIVER_NAME		"ARM-CCI PMU"

#define CCI_PMCR		0x0100
#define CCI_PID2		0x0fe8

#define CCI_PMCR_CEN		0x00000001
#define CCI_PMCR_NCNT_MASK	0x0000f800
#define CCI_PMCR_NCNT_SHIFT	11

#define CCI_PID2_REV_MASK	0xf0
#define CCI_PID2_REV_SHIFT	4

#define CCI_PMU_EVT_SEL		0x000
#define CCI_PMU_CNTR		0x004
#define CCI_PMU_CNTR_CTRL	0x008
#define CCI_PMU_OVRFLW		0x00c

#define CCI_PMU_OVRFLW_FLAG	1

#define CCI_PMU_CNTR_SIZE(model)	((model)->cntr_size)
#define CCI_PMU_CNTR_BASE(model, idx)	((idx) * CCI_PMU_CNTR_SIZE(model))
#define CCI_PMU_CNTR_MASK		((1ULL << 32) -1)
#define CCI_PMU_CNTR_LAST(cci_pmu)	(cci_pmu->num_cntrs - 1)

#define CCI_PMU_MAX_HW_CNTRS(model) \
	((model)->num_hw_cntrs + (model)->fixed_hw_cntrs)

/* Types of interfaces that can generate events */
enum {
	CCI_IF_SLAVE,
	CCI_IF_MASTER,
#ifdef CONFIG_ARM_CCI5xx_PMU
	CCI_IF_GLOBAL,
#endif
	CCI_IF_MAX,
};

#define NUM_HW_CNTRS_CII_4XX	4
#define NUM_HW_CNTRS_CII_5XX	8
#define NUM_HW_CNTRS_MAX	NUM_HW_CNTRS_CII_5XX

#define FIXED_HW_CNTRS_CII_4XX	1
#define FIXED_HW_CNTRS_CII_5XX	0
#define FIXED_HW_CNTRS_MAX	FIXED_HW_CNTRS_CII_4XX

#define HW_CNTRS_MAX		(NUM_HW_CNTRS_MAX + FIXED_HW_CNTRS_MAX)

struct event_range {
	u32 min;
	u32 max;
};

struct cci_pmu_hw_events {
	struct perf_event **events;
	unsigned long *used_mask;
	raw_spinlock_t pmu_lock;
};

struct cci_pmu;
/*
 * struct cci_pmu_model:
 * @fixed_hw_cntrs - Number of fixed event counters
 * @num_hw_cntrs - Maximum number of programmable event counters
 * @cntr_size - Size of an event counter mapping
 */
struct cci_pmu_model {
	char *name;
	u32 fixed_hw_cntrs;
	u32 num_hw_cntrs;
	u32 cntr_size;
	struct attribute **format_attrs;
	struct attribute **event_attrs;
	struct event_range event_ranges[CCI_IF_MAX];
	int (*validate_hw_event)(struct cci_pmu *, unsigned long);
	int (*get_event_idx)(struct cci_pmu *, struct cci_pmu_hw_events *, unsigned long);
	void (*write_counters)(struct cci_pmu *, unsigned long *);
};

static struct cci_pmu_model cci_pmu_models[];

struct cci_pmu {
	void __iomem *base;
	void __iomem *ctrl_base;
	struct pmu pmu;
	int cpu;
	int nr_irqs;
	int *irqs;
	unsigned long active_irqs;
	const struct cci_pmu_model *model;
	struct cci_pmu_hw_events hw_events;
	struct platform_device *plat_device;
	int num_cntrs;
	atomic_t active_events;
	struct mutex reserve_mutex;
};

#define to_cci_pmu(c)	(container_of(c, struct cci_pmu, pmu))

static struct cci_pmu *g_cci_pmu;

enum cci_models {
#ifdef CONFIG_ARM_CCI400_PMU
	CCI400_R0,
	CCI400_R1,
#endif
#ifdef CONFIG_ARM_CCI5xx_PMU
	CCI500_R0,
	CCI550_R0,
#endif
	CCI_MODEL_MAX
};

static void pmu_write_counters(struct cci_pmu *cci_pmu,
				 unsigned long *mask);
static ssize_t __maybe_unused cci_pmu_format_show(struct device *dev,
			struct device_attribute *attr, char *buf);
static ssize_t __maybe_unused cci_pmu_event_show(struct device *dev,
			struct device_attribute *attr, char *buf);

#define CCI_EXT_ATTR_ENTRY(_name, _func, _config) 				\
	&((struct dev_ext_attribute[]) {					\
		{ __ATTR(_name, S_IRUGO, _func, NULL), (void *)_config }	\
	})[0].attr.attr

#define CCI_FORMAT_EXT_ATTR_ENTRY(_name, _config) \
	CCI_EXT_ATTR_ENTRY(_name, cci_pmu_format_show, (char *)_config)
#define CCI_EVENT_EXT_ATTR_ENTRY(_name, _config) \
	CCI_EXT_ATTR_ENTRY(_name, cci_pmu_event_show, (unsigned long)_config)

/* CCI400 PMU Specific definitions */

#ifdef CONFIG_ARM_CCI400_PMU

/* Port ids */
#define CCI400_PORT_S0		0
#define CCI400_PORT_S1		1
#define CCI400_PORT_S2		2
#define CCI400_PORT_S3		3
#define CCI400_PORT_S4		4
#define CCI400_PORT_M0		5
#define CCI400_PORT_M1		6
#define CCI400_PORT_M2		7

#define CCI400_R1_PX		5

/*
 * Instead of an event id to monitor CCI cycles, a dedicated counter is
 * provided. Use 0xff to represent CCI cycles and hope that no future revisions
 * make use of this event in hardware.
 */
enum cci400_perf_events {
	CCI400_PMU_CYCLES = 0xff
};

#define CCI400_PMU_CYCLE_CNTR_IDX	0
#define CCI400_PMU_CNTR0_IDX		1

/*
 * CCI PMU event id is an 8-bit value made of two parts - bits 7:5 for one of 8
 * ports and bits 4:0 are event codes. There are different event codes
 * associated with each port type.
 *
 * Additionally, the range of events associated with the port types changed
 * between Rev0 and Rev1.
 *
 * The constants below define the range of valid codes for each port type for
 * the different revisions and are used to validate the event to be monitored.
 */

#define CCI400_PMU_EVENT_MASK		0xffUL
#define CCI400_PMU_EVENT_SOURCE_SHIFT	5
#define CCI400_PMU_EVENT_SOURCE_MASK	0x7
#define CCI400_PMU_EVENT_CODE_SHIFT	0
#define CCI400_PMU_EVENT_CODE_MASK	0x1f
#define CCI400_PMU_EVENT_SOURCE(event) \
	((event >> CCI400_PMU_EVENT_SOURCE_SHIFT) & \
			CCI400_PMU_EVENT_SOURCE_MASK)
#define CCI400_PMU_EVENT_CODE(event) \
	((event >> CCI400_PMU_EVENT_CODE_SHIFT) & CCI400_PMU_EVENT_CODE_MASK)

#define CCI400_R0_SLAVE_PORT_MIN_EV	0x00
#define CCI400_R0_SLAVE_PORT_MAX_EV	0x13
#define CCI400_R0_MASTER_PORT_MIN_EV	0x14
#define CCI400_R0_MASTER_PORT_MAX_EV	0x1a

#define CCI400_R1_SLAVE_PORT_MIN_EV	0x00
#define CCI400_R1_SLAVE_PORT_MAX_EV	0x14
#define CCI400_R1_MASTER_PORT_MIN_EV	0x00
#define CCI400_R1_MASTER_PORT_MAX_EV	0x11

#define CCI400_CYCLE_EVENT_EXT_ATTR_ENTRY(_name, _config) \
	CCI_EXT_ATTR_ENTRY(_name, cci400_pmu_cycle_event_show, \
					(unsigned long)_config)

static ssize_t cci400_pmu_cycle_event_show(struct device *dev,
			struct device_attribute *attr, char *buf);

static struct attribute *cci400_pmu_format_attrs[] = {
	CCI_FORMAT_EXT_ATTR_ENTRY(event, "config:0-4"),
	CCI_FORMAT_EXT_ATTR_ENTRY(source, "config:5-7"),
	NULL
};

static struct attribute *cci400_r0_pmu_event_attrs[] = {
	/* Slave events */
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_any, 0x0),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_device, 0x01),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_normal_or_nonshareable, 0x2),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_inner_or_outershareable, 0x3),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_cache_maintenance, 0x4),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_mem_barrier, 0x5),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_sync_barrier, 0x6),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_dvm_msg, 0x7),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_dvm_msg_sync, 0x8),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_stall_tt_full, 0x9),
	CCI_EVENT_EXT_ATTR_ENTRY(si_r_data_last_hs_snoop, 0xA),
	CCI_EVENT_EXT_ATTR_ENTRY(si_r_data_stall_rvalids_h_rready_l, 0xB),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_any, 0xC),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_device, 0xD),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_normal_or_nonshareable, 0xE),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_inner_or_outershare_wback_wclean, 0xF),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_write_unique, 0x10),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_write_line_unique, 0x11),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_evict, 0x12),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_stall_tt_full, 0x13),
	/* Master events */
	CCI_EVENT_EXT_ATTR_ENTRY(mi_retry_speculative_fetch, 0x14),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_addr_hazard, 0x15),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_id_hazard, 0x16),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_tt_full, 0x17),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_barrier_hazard, 0x18),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_wrq_stall_barrier_hazard, 0x19),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_wrq_stall_tt_full, 0x1A),
	/* Special event for cycles counter */
	CCI400_CYCLE_EVENT_EXT_ATTR_ENTRY(cycles, 0xff),
	NULL
};

static struct attribute *cci400_r1_pmu_event_attrs[] = {
	/* Slave events */
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_any, 0x0),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_device, 0x01),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_normal_or_nonshareable, 0x2),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_inner_or_outershareable, 0x3),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_cache_maintenance, 0x4),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_mem_barrier, 0x5),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_sync_barrier, 0x6),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_dvm_msg, 0x7),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_dvm_msg_sync, 0x8),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_stall_tt_full, 0x9),
	CCI_EVENT_EXT_ATTR_ENTRY(si_r_data_last_hs_snoop, 0xA),
	CCI_EVENT_EXT_ATTR_ENTRY(si_r_data_stall_rvalids_h_rready_l, 0xB),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_any, 0xC),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_device, 0xD),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_normal_or_nonshareable, 0xE),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_inner_or_outershare_wback_wclean, 0xF),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_write_unique, 0x10),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_write_line_unique, 0x11),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_evict, 0x12),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_stall_tt_full, 0x13),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_stall_slave_id_hazard, 0x14),
	/* Master events */
	CCI_EVENT_EXT_ATTR_ENTRY(mi_retry_speculative_fetch, 0x0),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_stall_cycle_addr_hazard, 0x1),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_master_id_hazard, 0x2),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_hi_prio_rtq_full, 0x3),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_barrier_hazard, 0x4),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_wrq_stall_barrier_hazard, 0x5),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_wrq_stall_wtq_full, 0x6),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_low_prio_rtq_full, 0x7),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_mid_prio_rtq_full, 0x8),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_qvn_vn0, 0x9),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_qvn_vn1, 0xA),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_qvn_vn2, 0xB),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall_qvn_vn3, 0xC),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_wrq_stall_qvn_vn0, 0xD),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_wrq_stall_qvn_vn1, 0xE),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_wrq_stall_qvn_vn2, 0xF),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_wrq_stall_qvn_vn3, 0x10),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_wrq_unique_or_line_unique_addr_hazard, 0x11),
	/* Special event for cycles counter */
	CCI400_CYCLE_EVENT_EXT_ATTR_ENTRY(cycles, 0xff),
	NULL
};

static ssize_t cci400_pmu_cycle_event_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct dev_ext_attribute *eattr = container_of(attr,
				struct dev_ext_attribute, attr);
	return sysfs_emit(buf, "config=0x%lx\n", (unsigned long)eattr->var);
}

static int cci400_get_event_idx(struct cci_pmu *cci_pmu,
				struct cci_pmu_hw_events *hw,
				unsigned long cci_event)
{
	int idx;

	/* cycles event idx is fixed */
	if (cci_event == CCI400_PMU_CYCLES) {
		if (test_and_set_bit(CCI400_PMU_CYCLE_CNTR_IDX, hw->used_mask))
			return -EAGAIN;

		return CCI400_PMU_CYCLE_CNTR_IDX;
	}

	for (idx = CCI400_PMU_CNTR0_IDX; idx <= CCI_PMU_CNTR_LAST(cci_pmu); ++idx)
		if (!test_and_set_bit(idx, hw->used_mask))
			return idx;

	/* No counters available */
	return -EAGAIN;
}

static int cci400_validate_hw_event(struct cci_pmu *cci_pmu, unsigned long hw_event)
{
	u8 ev_source = CCI400_PMU_EVENT_SOURCE(hw_event);
	u8 ev_code = CCI400_PMU_EVENT_CODE(hw_event);
	int if_type;

	if (hw_event & ~CCI400_PMU_EVENT_MASK)
		return -ENOENT;

	if (hw_event == CCI400_PMU_CYCLES)
		return hw_event;

	switch (ev_source) {
	case CCI400_PORT_S0:
	case CCI400_PORT_S1:
	case CCI400_PORT_S2:
	case CCI400_PORT_S3:
	case CCI400_PORT_S4:
		/* Slave Interface */
		if_type = CCI_IF_SLAVE;
		break;
	case CCI400_PORT_M0:
	case CCI400_PORT_M1:
	case CCI400_PORT_M2:
		/* Master Interface */
		if_type = CCI_IF_MASTER;
		break;
	default:
		return -ENOENT;
	}

	if (ev_code >= cci_pmu->model->event_ranges[if_type].min &&
		ev_code <= cci_pmu->model->event_ranges[if_type].max)
		return hw_event;

	return -ENOENT;
}

static int probe_cci400_revision(struct cci_pmu *cci_pmu)
{
	int rev;
	rev = readl_relaxed(cci_pmu->ctrl_base + CCI_PID2) & CCI_PID2_REV_MASK;
	rev >>= CCI_PID2_REV_SHIFT;

	if (rev < CCI400_R1_PX)
		return CCI400_R0;
	else
		return CCI400_R1;
}

static const struct cci_pmu_model *probe_cci_model(struct cci_pmu *cci_pmu)
{
	if (platform_has_secure_cci_access())
		return &cci_pmu_models[probe_cci400_revision(cci_pmu)];
	return NULL;
}
#else	/* !CONFIG_ARM_CCI400_PMU */
static inline struct cci_pmu_model *probe_cci_model(struct cci_pmu *cci_pmu)
{
	return NULL;
}
#endif	/* CONFIG_ARM_CCI400_PMU */

#ifdef CONFIG_ARM_CCI5xx_PMU

/*
 * CCI5xx PMU event id is an 9-bit value made of two parts.
 *	 bits [8:5] - Source for the event
 *	 bits [4:0] - Event code (specific to type of interface)
 *
 *
 */

/* Port ids */
#define CCI5xx_PORT_S0			0x0
#define CCI5xx_PORT_S1			0x1
#define CCI5xx_PORT_S2			0x2
#define CCI5xx_PORT_S3			0x3
#define CCI5xx_PORT_S4			0x4
#define CCI5xx_PORT_S5			0x5
#define CCI5xx_PORT_S6			0x6

#define CCI5xx_PORT_M0			0x8
#define CCI5xx_PORT_M1			0x9
#define CCI5xx_PORT_M2			0xa
#define CCI5xx_PORT_M3			0xb
#define CCI5xx_PORT_M4			0xc
#define CCI5xx_PORT_M5			0xd
#define CCI5xx_PORT_M6			0xe

#define CCI5xx_PORT_GLOBAL		0xf

#define CCI5xx_PMU_EVENT_MASK		0x1ffUL
#define CCI5xx_PMU_EVENT_SOURCE_SHIFT	0x5
#define CCI5xx_PMU_EVENT_SOURCE_MASK	0xf
#define CCI5xx_PMU_EVENT_CODE_SHIFT	0x0
#define CCI5xx_PMU_EVENT_CODE_MASK	0x1f

#define CCI5xx_PMU_EVENT_SOURCE(event)	\
	((event >> CCI5xx_PMU_EVENT_SOURCE_SHIFT) & CCI5xx_PMU_EVENT_SOURCE_MASK)
#define CCI5xx_PMU_EVENT_CODE(event)	\
	((event >> CCI5xx_PMU_EVENT_CODE_SHIFT) & CCI5xx_PMU_EVENT_CODE_MASK)

#define CCI5xx_SLAVE_PORT_MIN_EV	0x00
#define CCI5xx_SLAVE_PORT_MAX_EV	0x1f
#define CCI5xx_MASTER_PORT_MIN_EV	0x00
#define CCI5xx_MASTER_PORT_MAX_EV	0x06
#define CCI5xx_GLOBAL_PORT_MIN_EV	0x00
#define CCI5xx_GLOBAL_PORT_MAX_EV	0x0f


#define CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(_name, _config) \
	CCI_EXT_ATTR_ENTRY(_name, cci5xx_pmu_global_event_show, \
					(unsigned long) _config)

static ssize_t cci5xx_pmu_global_event_show(struct device *dev,
				struct device_attribute *attr, char *buf);

static struct attribute *cci5xx_pmu_format_attrs[] = {
	CCI_FORMAT_EXT_ATTR_ENTRY(event, "config:0-4"),
	CCI_FORMAT_EXT_ATTR_ENTRY(source, "config:5-8"),
	NULL,
};

static struct attribute *cci5xx_pmu_event_attrs[] = {
	/* Slave events */
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_arvalid, 0x0),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_dev, 0x1),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_nonshareable, 0x2),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_shareable_non_alloc, 0x3),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_shareable_alloc, 0x4),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_invalidate, 0x5),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_cache_maint, 0x6),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_dvm_msg, 0x7),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_rval, 0x8),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_hs_rlast_snoop, 0x9),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_hs_awalid, 0xA),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_dev, 0xB),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_non_shareable, 0xC),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_share_wb, 0xD),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_share_wlu, 0xE),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_share_wunique, 0xF),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_evict, 0x10),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_wrevict, 0x11),
	CCI_EVENT_EXT_ATTR_ENTRY(si_w_data_beat, 0x12),
	CCI_EVENT_EXT_ATTR_ENTRY(si_srq_acvalid, 0x13),
	CCI_EVENT_EXT_ATTR_ENTRY(si_srq_read, 0x14),
	CCI_EVENT_EXT_ATTR_ENTRY(si_srq_clean, 0x15),
	CCI_EVENT_EXT_ATTR_ENTRY(si_srq_data_transfer_low, 0x16),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rrq_stall_arvalid, 0x17),
	CCI_EVENT_EXT_ATTR_ENTRY(si_r_data_stall, 0x18),
	CCI_EVENT_EXT_ATTR_ENTRY(si_wrq_stall, 0x19),
	CCI_EVENT_EXT_ATTR_ENTRY(si_w_data_stall, 0x1A),
	CCI_EVENT_EXT_ATTR_ENTRY(si_w_resp_stall, 0x1B),
	CCI_EVENT_EXT_ATTR_ENTRY(si_srq_stall, 0x1C),
	CCI_EVENT_EXT_ATTR_ENTRY(si_s_data_stall, 0x1D),
	CCI_EVENT_EXT_ATTR_ENTRY(si_rq_stall_ot_limit, 0x1E),
	CCI_EVENT_EXT_ATTR_ENTRY(si_r_stall_arbit, 0x1F),

	/* Master events */
	CCI_EVENT_EXT_ATTR_ENTRY(mi_r_data_beat_any, 0x0),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_w_data_beat_any, 0x1),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_rrq_stall, 0x2),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_r_data_stall, 0x3),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_wrq_stall, 0x4),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_w_data_stall, 0x5),
	CCI_EVENT_EXT_ATTR_ENTRY(mi_w_resp_stall, 0x6),

	/* Global events */
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_access_filter_bank_0_1, 0x0),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_access_filter_bank_2_3, 0x1),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_access_filter_bank_4_5, 0x2),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_access_filter_bank_6_7, 0x3),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_access_miss_filter_bank_0_1, 0x4),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_access_miss_filter_bank_2_3, 0x5),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_access_miss_filter_bank_4_5, 0x6),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_access_miss_filter_bank_6_7, 0x7),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_back_invalidation, 0x8),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_stall_alloc_busy, 0x9),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_stall_tt_full, 0xA),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_wrq, 0xB),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_cd_hs, 0xC),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_rq_stall_addr_hazard, 0xD),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_rq_stall_tt_full, 0xE),
	CCI5xx_GLOBAL_EVENT_EXT_ATTR_ENTRY(cci_snoop_rq_tzmp1_prot, 0xF),
	NULL
};

static ssize_t cci5xx_pmu_global_event_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct dev_ext_attribute *eattr = container_of(attr,
					struct dev_ext_attribute, attr);
	/* Global events have single fixed source code */
	return sysfs_emit(buf, "event=0x%lx,source=0x%x\n",
			  (unsigned long)eattr->var, CCI5xx_PORT_GLOBAL);
}

/*
 * CCI500 provides 8 independent event counters that can count
 * any of the events available.
 * CCI500 PMU event source ids
 *	0x0-0x6 - Slave interfaces
 *	0x8-0xD - Master interfaces
 *	0xf     - Global Events
 *	0x7,0xe - Reserved
 */
static int cci500_validate_hw_event(struct cci_pmu *cci_pmu,
					unsigned long hw_event)
{
	u32 ev_source = CCI5xx_PMU_EVENT_SOURCE(hw_event);
	u32 ev_code = CCI5xx_PMU_EVENT_CODE(hw_event);
	int if_type;

	if (hw_event & ~CCI5xx_PMU_EVENT_MASK)
		return -ENOENT;

	switch (ev_source) {
	case CCI5xx_PORT_S0:
	case CCI5xx_PORT_S1:
	case CCI5xx_PORT_S2:
	case CCI5xx_PORT_S3:
	case CCI5xx_PORT_S4:
	case CCI5xx_PORT_S5:
	case CCI5xx_PORT_S6:
		if_type = CCI_IF_SLAVE;
		break;
	case CCI5xx_PORT_M0:
	case CCI5xx_PORT_M1:
	case CCI5xx_PORT_M2:
	case CCI5xx_PORT_M3:
	case CCI5xx_PORT_M4:
	case CCI5xx_PORT_M5:
		if_type = CCI_IF_MASTER;
		break;
	case CCI5xx_PORT_GLOBAL:
		if_type = CCI_IF_GLOBAL;
		break;
	default:
		return -ENOENT;
	}

	if (ev_code >= cci_pmu->model->event_ranges[if_type].min &&
		ev_code <= cci_pmu->model->event_ranges[if_type].max)
		return hw_event;

	return -ENOENT;
}

/*
 * CCI550 provides 8 independent event counters that can count
 * any of the events available.
 * CCI550 PMU event source ids
 *	0x0-0x6 - Slave interfaces
 *	0x8-0xe - Master interfaces
 *	0xf     - Global Events
 *	0x7	- Reserved
 */
static int cci550_validate_hw_event(struct cci_pmu *cci_pmu,
					unsigned long hw_event)
{
	u32 ev_source = CCI5xx_PMU_EVENT_SOURCE(hw_event);
	u32 ev_code = CCI5xx_PMU_EVENT_CODE(hw_event);
	int if_type;

	if (hw_event & ~CCI5xx_PMU_EVENT_MASK)
		return -ENOENT;

	switch (ev_source) {
	case CCI5xx_PORT_S0:
	case CCI5xx_PORT_S1:
	case CCI5xx_PORT_S2:
	case CCI5xx_PORT_S3:
	case CCI5xx_PORT_S4:
	case CCI5xx_PORT_S5:
	case CCI5xx_PORT_S6:
		if_type = CCI_IF_SLAVE;
		break;
	case CCI5xx_PORT_M0:
	case CCI5xx_PORT_M1:
	case CCI5xx_PORT_M2:
	case CCI5xx_PORT_M3:
	case CCI5xx_PORT_M4:
	case CCI5xx_PORT_M5:
	case CCI5xx_PORT_M6:
		if_type = CCI_IF_MASTER;
		break;
	case CCI5xx_PORT_GLOBAL:
		if_type = CCI_IF_GLOBAL;
		break;
	default:
		return -ENOENT;
	}

	if (ev_code >= cci_pmu->model->event_ranges[if_type].min &&
		ev_code <= cci_pmu->model->event_ranges[if_type].max)
		return hw_event;

	return -ENOENT;
}

#endif	/* CONFIG_ARM_CCI5xx_PMU */

/*
 * Program the CCI PMU counters which have PERF_HES_ARCH set
 * with the event period and mark them ready before we enable
 * PMU.
 */
static void cci_pmu_sync_counters(struct cci_pmu *cci_pmu)
{
	int i;
	struct cci_pmu_hw_events *cci_hw = &cci_pmu->hw_events;
	DECLARE_BITMAP(mask, HW_CNTRS_MAX);

	bitmap_zero(mask, cci_pmu->num_cntrs);
	for_each_set_bit(i, cci_pmu->hw_events.used_mask, cci_pmu->num_cntrs) {
		struct perf_event *event = cci_hw->events[i];

		if (WARN_ON(!event))
			continue;

		/* Leave the events which are not counting */
		if (event->hw.state & PERF_HES_STOPPED)
			continue;
		if (event->hw.state & PERF_HES_ARCH) {
			set_bit(i, mask);
			event->hw.state &= ~PERF_HES_ARCH;
		}
	}

	pmu_write_counters(cci_pmu, mask);
}

/* Should be called with cci_pmu->hw_events->pmu_lock held */
static void __cci_pmu_enable_nosync(struct cci_pmu *cci_pmu)
{
	u32 val;

	/* Enable all the PMU counters. */
	val = readl_relaxed(cci_pmu->ctrl_base + CCI_PMCR) | CCI_PMCR_CEN;
	writel(val, cci_pmu->ctrl_base + CCI_PMCR);
}

/* Should be called with cci_pmu->hw_events->pmu_lock held */
static void __cci_pmu_enable_sync(struct cci_pmu *cci_pmu)
{
	cci_pmu_sync_counters(cci_pmu);
	__cci_pmu_enable_nosync(cci_pmu);
}

/* Should be called with cci_pmu->hw_events->pmu_lock held */
static void __cci_pmu_disable(struct cci_pmu *cci_pmu)
{
	u32 val;

	/* Disable all the PMU counters. */
	val = readl_relaxed(cci_pmu->ctrl_base + CCI_PMCR) & ~CCI_PMCR_CEN;
	writel(val, cci_pmu->ctrl_base + CCI_PMCR);
}

static ssize_t cci_pmu_format_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct dev_ext_attribute *eattr = container_of(attr,
				struct dev_ext_attribute, attr);
	return sysfs_emit(buf, "%s\n", (char *)eattr->var);
}

static ssize_t cci_pmu_event_show(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct dev_ext_attribute *eattr = container_of(attr,
				struct dev_ext_attribute, attr);
	/* source parameter is mandatory for normal PMU events */
	return sysfs_emit(buf, "source=?,event=0x%lx\n",
			  (unsigned long)eattr->var);
}

static int pmu_is_valid_counter(struct cci_pmu *cci_pmu, int idx)
{
	return 0 <= idx && idx <= CCI_PMU_CNTR_LAST(cci_pmu);
}

static u32 pmu_read_register(struct cci_pmu *cci_pmu, int idx, unsigned int offset)
{
	return readl_relaxed(cci_pmu->base +
			     CCI_PMU_CNTR_BASE(cci_pmu->model, idx) + offset);
}

static void pmu_write_register(struct cci_pmu *cci_pmu, u32 value,
			       int idx, unsigned int offset)
{
	writel_relaxed(value, cci_pmu->base +
		       CCI_PMU_CNTR_BASE(cci_pmu->model, idx) + offset);
}

static void pmu_disable_counter(struct cci_pmu *cci_pmu, int idx)
{
	pmu_write_register(cci_pmu, 0, idx, CCI_PMU_CNTR_CTRL);
}

static void pmu_enable_counter(struct cci_pmu *cci_pmu, int idx)
{
	pmu_write_register(cci_pmu, 1, idx, CCI_PMU_CNTR_CTRL);
}

static bool __maybe_unused
pmu_counter_is_enabled(struct cci_pmu *cci_pmu, int idx)
{
	return (pmu_read_register(cci_pmu, idx, CCI_PMU_CNTR_CTRL) & 0x1) != 0;
}

static void pmu_set_event(struct cci_pmu *cci_pmu, int idx, unsigned long event)
{
	pmu_write_register(cci_pmu, event, idx, CCI_PMU_EVT_SEL);
}

/*
 * For all counters on the CCI-PMU, disable any 'enabled' counters,
 * saving the changed counters in the mask, so that we can restore
 * it later using pmu_restore_counters. The mask is private to the
 * caller. We cannot rely on the used_mask maintained by the CCI_PMU
 * as it only tells us if the counter is assigned to perf_event or not.
 * The state of the perf_event cannot be locked by the PMU layer, hence
 * we check the individual counter status (which can be locked by
 * cci_pm->hw_events->pmu_lock).
 *
 * @mask should be initialised to empty by the caller.
 */
static void __maybe_unused
pmu_save_counters(struct cci_pmu *cci_pmu, unsigned long *mask)
{
	int i;

	for (i = 0; i < cci_pmu->num_cntrs; i++) {
		if (pmu_counter_is_enabled(cci_pmu, i)) {
			set_bit(i, mask);
			pmu_disable_counter(cci_pmu, i);
		}
	}
}

/*
 * Restore the status of the counters. Reversal of the pmu_save_counters().
 * For each counter set in the mask, enable the counter back.
 */
static void __maybe_unused
pmu_restore_counters(struct cci_pmu *cci_pmu, unsigned long *mask)
{
	int i;

	for_each_set_bit(i, mask, cci_pmu->num_cntrs)
		pmu_enable_counter(cci_pmu, i);
}

/*
 * Returns the number of programmable counters actually implemented
 * by the cci
 */
static u32 pmu_get_max_counters(struct cci_pmu *cci_pmu)
{
	return (readl_relaxed(cci_pmu->ctrl_base + CCI_PMCR) &
		CCI_PMCR_NCNT_MASK) >> CCI_PMCR_NCNT_SHIFT;
}

static int pmu_get_event_idx(struct cci_pmu_hw_events *hw, struct perf_event *event)
{
	struct cci_pmu *cci_pmu = to_cci_pmu(event->pmu);
	unsigned long cci_event = event->hw.config_base;
	int idx;

	if (cci_pmu->model->get_event_idx)
		return cci_pmu->model->get_event_idx(cci_pmu, hw, cci_event);

	/* Generic code to find an unused idx from the mask */
	for(idx = 0; idx <= CCI_PMU_CNTR_LAST(cci_pmu); idx++)
		if (!test_and_set_bit(idx, hw->used_mask))
			return idx;

	/* No counters available */
	return -EAGAIN;
}

static int pmu_map_event(struct perf_event *event)
{
	struct cci_pmu *cci_pmu = to_cci_pmu(event->pmu);

	if (event->attr.type < PERF_TYPE_MAX ||
			!cci_pmu->model->validate_hw_event)
		return -ENOENT;

	return	cci_pmu->model->validate_hw_event(cci_pmu, event->attr.config);
}

static int pmu_request_irq(struct cci_pmu *cci_pmu, irq_handler_t handler)
{
	int i;
	struct platform_device *pmu_device = cci_pmu->plat_device;

	if (unlikely(!pmu_device))
		return -ENODEV;

	if (cci_pmu->nr_irqs < 1) {
		dev_err(&pmu_device->dev, "no irqs for CCI PMUs defined\n");
		return -ENODEV;
	}

	/*
	 * Register all available CCI PMU interrupts. In the interrupt handler
	 * we iterate over the counters checking for interrupt source (the
	 * overflowing counter) and clear it.
	 *
	 * This should allow handling of non-unique interrupt for the counters.
	 */
	for (i = 0; i < cci_pmu->nr_irqs; i++) {
		int err = request_irq(cci_pmu->irqs[i], handler, IRQF_SHARED,
				"arm-cci-pmu", cci_pmu);
		if (err) {
			dev_err(&pmu_device->dev, "unable to request IRQ%d for ARM CCI PMU counters\n",
				cci_pmu->irqs[i]);
			return err;
		}

		set_bit(i, &cci_pmu->active_irqs);
	}

	return 0;
}

static void pmu_free_irq(struct cci_pmu *cci_pmu)
{
	int i;

	for (i = 0; i < cci_pmu->nr_irqs; i++) {
		if (!test_and_clear_bit(i, &cci_pmu->active_irqs))
			continue;

		free_irq(cci_pmu->irqs[i], cci_pmu);
	}
}

static u32 pmu_read_counter(struct perf_event *event)
{
	struct cci_pmu *cci_pmu = to_cci_pmu(event->pmu);
	struct hw_perf_event *hw_counter = &event->hw;
	int idx = hw_counter->idx;
	u32 value;

	if (unlikely(!pmu_is_valid_counter(cci_pmu, idx))) {
		dev_err(&cci_pmu->plat_device->dev, "Invalid CCI PMU counter %d\n", idx);
		return 0;
	}
	value = pmu_read_register(cci_pmu, idx, CCI_PMU_CNTR);

	return value;
}

static void pmu_write_counter(struct cci_pmu *cci_pmu, u32 value, int idx)
{
	pmu_write_register(cci_pmu, value, idx, CCI_PMU_CNTR);
}

static void __pmu_write_counters(struct cci_pmu *cci_pmu, unsigned long *mask)
{
	int i;
	struct cci_pmu_hw_events *cci_hw = &cci_pmu->hw_events;

	for_each_set_bit(i, mask, cci_pmu->num_cntrs) {
		struct perf_event *event = cci_hw->events[i];

		if (WARN_ON(!event))
			continue;
		pmu_write_counter(cci_pmu, local64_read(&event->hw.prev_count), i);
	}
}

static void pmu_write_counters(struct cci_pmu *cci_pmu, unsigned long *mask)
{
	if (cci_pmu->model->write_counters)
		cci_pmu->model->write_counters(cci_pmu, mask);
	else
		__pmu_write_counters(cci_pmu, mask);
}

#ifdef CONFIG_ARM_CCI5xx_PMU

/*
 * CCI-500/CCI-550 has advanced power saving policies, which could gate the
 * clocks to the PMU counters, which makes the writes to them ineffective.
 * The only way to write to those counters is when the global counters
 * are enabled and the particular counter is enabled.
 *
 * So we do the following :
 *
 * 1) Disable all the PMU counters, saving their current state
 * 2) Enable the global PMU profiling, now that all counters are
 *    disabled.
 *
 * For each counter to be programmed, repeat steps 3-7:
 *
 * 3) Write an invalid event code to the event control register for the
      counter, so that the counters are not modified.
 * 4) Enable the counter control for the counter.
 * 5) Set the counter value
 * 6) Disable the counter
 * 7) Restore the event in the target counter
 *
 * 8) Disable the global PMU.
 * 9) Restore the status of the rest of the counters.
 *
 * We choose an event which for CCI-5xx is guaranteed not to count.
 * We use the highest possible event code (0x1f) for the master interface 0.
 */
#define CCI5xx_INVALID_EVENT	((CCI5xx_PORT_M0 << CCI5xx_PMU_EVENT_SOURCE_SHIFT) | \
				 (CCI5xx_PMU_EVENT_CODE_MASK << CCI5xx_PMU_EVENT_CODE_SHIFT))
static void cci5xx_pmu_write_counters(struct cci_pmu *cci_pmu, unsigned long *mask)
{
	int i;
	DECLARE_BITMAP(saved_mask, HW_CNTRS_MAX);

	bitmap_zero(saved_mask, cci_pmu->num_cntrs);
	pmu_save_counters(cci_pmu, saved_mask);

	/*
	 * Now that all the counters are disabled, we can safely turn the PMU on,
	 * without syncing the status of the counters
	 */
	__cci_pmu_enable_nosync(cci_pmu);

	for_each_set_bit(i, mask, cci_pmu->num_cntrs) {
		struct perf_event *event = cci_pmu->hw_events.events[i];

		if (WARN_ON(!event))
			continue;

		pmu_set_event(cci_pmu, i, CCI5xx_INVALID_EVENT);
		pmu_enable_counter(cci_pmu, i);
		pmu_write_counter(cci_pmu, local64_read(&event->hw.prev_count), i);
		pmu_disable_counter(cci_pmu, i);
		pmu_set_event(cci_pmu, i, event->hw.config_base);
	}

	__cci_pmu_disable(cci_pmu);

	pmu_restore_counters(cci_pmu, saved_mask);
}

#endif	/* CONFIG_ARM_CCI5xx_PMU */

static u64 pmu_event_update(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	u64 delta, prev_raw_count, new_raw_count;

	do {
		prev_raw_count = local64_read(&hwc->prev_count);
		new_raw_count = pmu_read_counter(event);
	} while (local64_cmpxchg(&hwc->prev_count, prev_raw_count,
		 new_raw_count) != prev_raw_count);

	delta = (new_raw_count - prev_raw_count) & CCI_PMU_CNTR_MASK;

	local64_add(delta, &event->count);

	return new_raw_count;
}

static void pmu_read(struct perf_event *event)
{
	pmu_event_update(event);
}

static void pmu_event_set_period(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	/*
	 * The CCI PMU counters have a period of 2^32. To account for the
	 * possiblity of extreme interrupt latency we program for a period of
	 * half that. Hopefully we can handle the interrupt before another 2^31
	 * events occur and the counter overtakes its previous value.
	 */
	u64 val = 1ULL << 31;
	local64_set(&hwc->prev_count, val);

	/*
	 * CCI PMU uses PERF_HES_ARCH to keep track of the counters, whose
	 * values needs to be sync-ed with the s/w state before the PMU is
	 * enabled.
	 * Mark this counter for sync.
	 */
	hwc->state |= PERF_HES_ARCH;
}

static irqreturn_t pmu_handle_irq(int irq_num, void *dev)
{
	struct cci_pmu *cci_pmu = dev;
	struct cci_pmu_hw_events *events = &cci_pmu->hw_events;
	int idx, handled = IRQ_NONE;

	raw_spin_lock(&events->pmu_lock);

	/* Disable the PMU while we walk through the counters */
	__cci_pmu_disable(cci_pmu);
	/*
	 * Iterate over counters and update the corresponding perf events.
	 * This should work regardless of whether we have per-counter overflow
	 * interrupt or a combined overflow interrupt.
	 */
	for (idx = 0; idx <= CCI_PMU_CNTR_LAST(cci_pmu); idx++) {
		struct perf_event *event = events->events[idx];

		if (!event)
			continue;

		/* Did this counter overflow? */
		if (!(pmu_read_register(cci_pmu, idx, CCI_PMU_OVRFLW) &
		      CCI_PMU_OVRFLW_FLAG))
			continue;

		pmu_write_register(cci_pmu, CCI_PMU_OVRFLW_FLAG, idx,
							CCI_PMU_OVRFLW);

		pmu_event_update(event);
		pmu_event_set_period(event);
		handled = IRQ_HANDLED;
	}

	/* Enable the PMU and sync possibly overflowed counters */
	__cci_pmu_enable_sync(cci_pmu);
	raw_spin_unlock(&events->pmu_lock);

	return IRQ_RETVAL(handled);
}

static int cci_pmu_get_hw(struct cci_pmu *cci_pmu)
{
	int ret = pmu_request_irq(cci_pmu, pmu_handle_irq);
	if (ret) {
		pmu_free_irq(cci_pmu);
		return ret;
	}
	return 0;
}

static void cci_pmu_put_hw(struct cci_pmu *cci_pmu)
{
	pmu_free_irq(cci_pmu);
}

static void hw_perf_event_destroy(struct perf_event *event)
{
	struct cci_pmu *cci_pmu = to_cci_pmu(event->pmu);
	atomic_t *active_events = &cci_pmu->active_events;
	struct mutex *reserve_mutex = &cci_pmu->reserve_mutex;

	if (atomic_dec_and_mutex_lock(active_events, reserve_mutex)) {
		cci_pmu_put_hw(cci_pmu);
		mutex_unlock(reserve_mutex);
	}
}

static void cci_pmu_enable(struct pmu *pmu)
{
	struct cci_pmu *cci_pmu = to_cci_pmu(pmu);
	struct cci_pmu_hw_events *hw_events = &cci_pmu->hw_events;
	int enabled = bitmap_weight(hw_events->used_mask, cci_pmu->num_cntrs);
	unsigned long flags;

	if (!enabled)
		return;

	raw_spin_lock_irqsave(&hw_events->pmu_lock, flags);
	__cci_pmu_enable_sync(cci_pmu);
	raw_spin_unlock_irqrestore(&hw_events->pmu_lock, flags);

}

static void cci_pmu_disable(struct pmu *pmu)
{
	struct cci_pmu *cci_pmu = to_cci_pmu(pmu);
	struct cci_pmu_hw_events *hw_events = &cci_pmu->hw_events;
	unsigned long flags;

	raw_spin_lock_irqsave(&hw_events->pmu_lock, flags);
	__cci_pmu_disable(cci_pmu);
	raw_spin_unlock_irqrestore(&hw_events->pmu_lock, flags);
}

/*
 * Check if the idx represents a non-programmable counter.
 * All the fixed event counters are mapped before the programmable
 * counters.
 */
static bool pmu_fixed_hw_idx(struct cci_pmu *cci_pmu, int idx)
{
	return (idx >= 0) && (idx < cci_pmu->model->fixed_hw_cntrs);
}

static void cci_pmu_start(struct perf_event *event, int pmu_flags)
{
	struct cci_pmu *cci_pmu = to_cci_pmu(event->pmu);
	struct cci_pmu_hw_events *hw_events = &cci_pmu->hw_events;
	struct hw_perf_event *hwc = &event->hw;
	int idx = hwc->idx;
	unsigned long flags;

	/*
	 * To handle interrupt latency, we always reprogram the period
	 * regardlesss of PERF_EF_RELOAD.
	 */
	if (pmu_flags & PERF_EF_RELOAD)
		WARN_ON_ONCE(!(hwc->state & PERF_HES_UPTODATE));

	hwc->state = 0;

	if (unlikely(!pmu_is_valid_counter(cci_pmu, idx))) {
		dev_err(&cci_pmu->plat_device->dev, "Invalid CCI PMU counter %d\n", idx);
		return;
	}

	raw_spin_lock_irqsave(&hw_events->pmu_lock, flags);

	/* Configure the counter unless you are counting a fixed event */
	if (!pmu_fixed_hw_idx(cci_pmu, idx))
		pmu_set_event(cci_pmu, idx, hwc->config_base);

	pmu_event_set_period(event);
	pmu_enable_counter(cci_pmu, idx);

	raw_spin_unlock_irqrestore(&hw_events->pmu_lock, flags);
}

static void cci_pmu_stop(struct perf_event *event, int pmu_flags)
{
	struct cci_pmu *cci_pmu = to_cci_pmu(event->pmu);
	struct hw_perf_event *hwc = &event->hw;
	int idx = hwc->idx;

	if (hwc->state & PERF_HES_STOPPED)
		return;

	if (unlikely(!pmu_is_valid_counter(cci_pmu, idx))) {
		dev_err(&cci_pmu->plat_device->dev, "Invalid CCI PMU counter %d\n", idx);
		return;
	}

	/*
	 * We always reprogram the counter, so ignore PERF_EF_UPDATE. See
	 * cci_pmu_start()
	 */
	pmu_disable_counter(cci_pmu, idx);
	pmu_event_update(event);
	hwc->state |= PERF_HES_STOPPED | PERF_HES_UPTODATE;
}

static int cci_pmu_add(struct perf_event *event, int flags)
{
	struct cci_pmu *cci_pmu = to_cci_pmu(event->pmu);
	struct cci_pmu_hw_events *hw_events = &cci_pmu->hw_events;
	struct hw_perf_event *hwc = &event->hw;
	int idx;

	/* If we don't have a space for the counter then finish early. */
	idx = pmu_get_event_idx(hw_events, event);
	if (idx < 0)
		return idx;

	event->hw.idx = idx;
	hw_events->events[idx] = event;

	hwc->state = PERF_HES_STOPPED | PERF_HES_UPTODATE;
	if (flags & PERF_EF_START)
		cci_pmu_start(event, PERF_EF_RELOAD);

	/* Propagate our changes to the userspace mapping. */
	perf_event_update_userpage(event);

	return 0;
}

static void cci_pmu_del(struct perf_event *event, int flags)
{
	struct cci_pmu *cci_pmu = to_cci_pmu(event->pmu);
	struct cci_pmu_hw_events *hw_events = &cci_pmu->hw_events;
	struct hw_perf_event *hwc = &event->hw;
	int idx = hwc->idx;

	cci_pmu_stop(event, PERF_EF_UPDATE);
	hw_events->events[idx] = NULL;
	clear_bit(idx, hw_events->used_mask);

	perf_event_update_userpage(event);
}

static int validate_event(struct pmu *cci_pmu,
			  struct cci_pmu_hw_events *hw_events,
			  struct perf_event *event)
{
	if (is_software_event(event))
		return 1;

	/*
	 * Reject groups spanning multiple HW PMUs (e.g. CPU + CCI). The
	 * core perf code won't check that the pmu->ctx == leader->ctx
	 * until after pmu->event_init(event).
	 */
	if (event->pmu != cci_pmu)
		return 0;

	if (event->state < PERF_EVENT_STATE_OFF)
		return 1;

	if (event->state == PERF_EVENT_STATE_OFF && !event->attr.enable_on_exec)
		return 1;

	return pmu_get_event_idx(hw_events, event) >= 0;
}

static int validate_group(struct perf_event *event)
{
	struct perf_event *sibling, *leader = event->group_leader;
	struct cci_pmu *cci_pmu = to_cci_pmu(event->pmu);
	unsigned long mask[BITS_TO_LONGS(HW_CNTRS_MAX)];
	struct cci_pmu_hw_events fake_pmu = {
		/*
		 * Initialise the fake PMU. We only need to populate the
		 * used_mask for the purposes of validation.
		 */
		.used_mask = mask,
	};
	memset(mask, 0, BITS_TO_LONGS(cci_pmu->num_cntrs) * sizeof(unsigned long));

	if (!validate_event(event->pmu, &fake_pmu, leader))
		return -EINVAL;

	for_each_sibling_event(sibling, leader) {
		if (!validate_event(event->pmu, &fake_pmu, sibling))
			return -EINVAL;
	}

	if (!validate_event(event->pmu, &fake_pmu, event))
		return -EINVAL;

	return 0;
}

static int __hw_perf_event_init(struct perf_event *event)
{
	struct hw_perf_event *hwc = &event->hw;
	int mapping;

	mapping = pmu_map_event(event);

	if (mapping < 0) {
		pr_debug("event %x:%llx not supported\n", event->attr.type,
			 event->attr.config);
		return mapping;
	}

	/*
	 * We don't assign an index until we actually place the event onto
	 * hardware. Use -1 to signify that we haven't decided where to put it
	 * yet.
	 */
	hwc->idx		= -1;
	hwc->config_base	= 0;
	hwc->config		= 0;
	hwc->event_base		= 0;

	/*
	 * Store the event encoding into the config_base field.
	 */
	hwc->config_base	    |= (unsigned long)mapping;

	if (event->group_leader != event) {
		if (validate_group(event) != 0)
			return -EINVAL;
	}

	return 0;
}

static int cci_pmu_event_init(struct perf_event *event)
{
	struct cci_pmu *cci_pmu = to_cci_pmu(event->pmu);
	atomic_t *active_events = &cci_pmu->active_events;
	int err = 0;

	if (event->attr.type != event->pmu->type)
		return -ENOENT;

	/* Shared by all CPUs, no meaningful state to sample */
	if (is_sampling_event(event) || event->attach_state & PERF_ATTACH_TASK)
		return -EOPNOTSUPP;

	/*
	 * Following the example set by other "uncore" PMUs, we accept any CPU
	 * and rewrite its affinity dynamically rather than having perf core
	 * handle cpu == -1 and pid == -1 for this case.
	 *
	 * The perf core will pin online CPUs for the duration of this call and
	 * the event being installed into its context, so the PMU's CPU can't
	 * change under our feet.
	 */
	if (event->cpu < 0)
		return -EINVAL;
	event->cpu = cci_pmu->cpu;

	event->destroy = hw_perf_event_destroy;
	if (!atomic_inc_not_zero(active_events)) {
		mutex_lock(&cci_pmu->reserve_mutex);
		if (atomic_read(active_events) == 0)
			err = cci_pmu_get_hw(cci_pmu);
		if (!err)
			atomic_inc(active_events);
		mutex_unlock(&cci_pmu->reserve_mutex);
	}
	if (err)
		return err;

	err = __hw_perf_event_init(event);
	if (err)
		hw_perf_event_destroy(event);

	return err;
}

static ssize_t pmu_cpumask_attr_show(struct device *dev,
				     struct device_attribute *attr, char *buf)
{
	struct pmu *pmu = dev_get_drvdata(dev);
	struct cci_pmu *cci_pmu = to_cci_pmu(pmu);

	return cpumap_print_to_pagebuf(true, buf, cpumask_of(cci_pmu->cpu));
}

static struct device_attribute pmu_cpumask_attr =
	__ATTR(cpumask, S_IRUGO, pmu_cpumask_attr_show, NULL);

static struct attribute *pmu_attrs[] = {
	&pmu_cpumask_attr.attr,
	NULL,
};

static const struct attribute_group pmu_attr_group = {
	.attrs = pmu_attrs,
};

static struct attribute_group pmu_format_attr_group = {
	.name = "format",
	.attrs = NULL,		/* Filled in cci_pmu_init_attrs */
};

static struct attribute_group pmu_event_attr_group = {
	.name = "events",
	.attrs = NULL,		/* Filled in cci_pmu_init_attrs */
};

static const struct attribute_group *pmu_attr_groups[] = {
	&pmu_attr_group,
	&pmu_format_attr_group,
	&pmu_event_attr_group,
	NULL
};

static int cci_pmu_init(struct cci_pmu *cci_pmu, struct platform_device *pdev)
{
	const struct cci_pmu_model *model = cci_pmu->model;
	char *name = model->name;
	u32 num_cntrs;

	if (WARN_ON(model->num_hw_cntrs > NUM_HW_CNTRS_MAX))
		return -EINVAL;
	if (WARN_ON(model->fixed_hw_cntrs > FIXED_HW_CNTRS_MAX))
		return -EINVAL;

	pmu_event_attr_group.attrs = model->event_attrs;
	pmu_format_attr_group.attrs = model->format_attrs;

	cci_pmu->pmu = (struct pmu) {
		.module		= THIS_MODULE,
		.name		= cci_pmu->model->name,
		.task_ctx_nr	= perf_invalid_context,
		.pmu_enable	= cci_pmu_enable,
		.pmu_disable	= cci_pmu_disable,
		.event_init	= cci_pmu_event_init,
		.add		= cci_pmu_add,
		.del		= cci_pmu_del,
		.start		= cci_pmu_start,
		.stop		= cci_pmu_stop,
		.read		= pmu_read,
		.attr_groups	= pmu_attr_groups,
		.capabilities	= PERF_PMU_CAP_NO_EXCLUDE,
	};

	cci_pmu->plat_device = pdev;
	num_cntrs = pmu_get_max_counters(cci_pmu);
	if (num_cntrs > cci_pmu->model->num_hw_cntrs) {
		dev_warn(&pdev->dev,
			"PMU implements more counters(%d) than supported by"
			" the model(%d), truncated.",
			num_cntrs, cci_pmu->model->num_hw_cntrs);
		num_cntrs = cci_pmu->model->num_hw_cntrs;
	}
	cci_pmu->num_cntrs = num_cntrs + cci_pmu->model->fixed_hw_cntrs;

	return perf_pmu_register(&cci_pmu->pmu, name, -1);
}

static int cci_pmu_offline_cpu(unsigned int cpu)
{
	int target;

	if (!g_cci_pmu || cpu != g_cci_pmu->cpu)
		return 0;

	target = cpumask_any_but(cpu_online_mask, cpu);
	if (target >= nr_cpu_ids)
		return 0;

	perf_pmu_migrate_context(&g_cci_pmu->pmu, cpu, target);
	g_cci_pmu->cpu = target;
	return 0;
}

static __maybe_unused struct cci_pmu_model cci_pmu_models[] = {
#ifdef CONFIG_ARM_CCI400_PMU
	[CCI400_R0] = {
		.name = "CCI_400",
		.fixed_hw_cntrs = FIXED_HW_CNTRS_CII_4XX, /* Cycle counter */
		.num_hw_cntrs = NUM_HW_CNTRS_CII_4XX,
		.cntr_size = SZ_4K,
		.format_attrs = cci400_pmu_format_attrs,
		.event_attrs = cci400_r0_pmu_event_attrs,
		.event_ranges = {
			[CCI_IF_SLAVE] = {
				CCI400_R0_SLAVE_PORT_MIN_EV,
				CCI400_R0_SLAVE_PORT_MAX_EV,
			},
			[CCI_IF_MASTER] = {
				CCI400_R0_MASTER_PORT_MIN_EV,
				CCI400_R0_MASTER_PORT_MAX_EV,
			},
		},
		.validate_hw_event = cci400_validate_hw_event,
		.get_event_idx = cci400_get_event_idx,
	},
	[CCI400_R1] = {
		.name = "CCI_400_r1",
		.fixed_hw_cntrs = FIXED_HW_CNTRS_CII_4XX, /* Cycle counter */
		.num_hw_cntrs = NUM_HW_CNTRS_CII_4XX,
		.cntr_size = SZ_4K,
		.format_attrs = cci400_pmu_format_attrs,
		.event_attrs = cci400_r1_pmu_event_attrs,
		.event_ranges = {
			[CCI_IF_SLAVE] = {
				CCI400_R1_SLAVE_PORT_MIN_EV,
				CCI400_R1_SLAVE_PORT_MAX_EV,
			},
			[CCI_IF_MASTER] = {
				CCI400_R1_MASTER_PORT_MIN_EV,
				CCI400_R1_MASTER_PORT_MAX_EV,
			},
		},
		.validate_hw_event = cci400_validate_hw_event,
		.get_event_idx = cci400_get_event_idx,
	},
#endif
#ifdef CONFIG_ARM_CCI5xx_PMU
	[CCI500_R0] = {
		.name = "CCI_500",
		.fixed_hw_cntrs = FIXED_HW_CNTRS_CII_5XX,
		.num_hw_cntrs = NUM_HW_CNTRS_CII_5XX,
		.cntr_size = SZ_64K,
		.format_attrs = cci5xx_pmu_format_attrs,
		.event_attrs = cci5xx_pmu_event_attrs,
		.event_ranges = {
			[CCI_IF_SLAVE] = {
				CCI5xx_SLAVE_PORT_MIN_EV,
				CCI5xx_SLAVE_PORT_MAX_EV,
			},
			[CCI_IF_MASTER] = {
				CCI5xx_MASTER_PORT_MIN_EV,
				CCI5xx_MASTER_PORT_MAX_EV,
			},
			[CCI_IF_GLOBAL] = {
				CCI5xx_GLOBAL_PORT_MIN_EV,
				CCI5xx_GLOBAL_PORT_MAX_EV,
			},
		},
		.validate_hw_event = cci500_validate_hw_event,
		.write_counters	= cci5xx_pmu_write_counters,
	},
	[CCI550_R0] = {
		.name = "CCI_550",
		.fixed_hw_cntrs = FIXED_HW_CNTRS_CII_5XX,
		.num_hw_cntrs = NUM_HW_CNTRS_CII_5XX,
		.cntr_size = SZ_64K,
		.format_attrs = cci5xx_pmu_format_attrs,
		.event_attrs = cci5xx_pmu_event_attrs,
		.event_ranges = {
			[CCI_IF_SLAVE] = {
				CCI5xx_SLAVE_PORT_MIN_EV,
				CCI5xx_SLAVE_PORT_MAX_EV,
			},
			[CCI_IF_MASTER] = {
				CCI5xx_MASTER_PORT_MIN_EV,
				CCI5xx_MASTER_PORT_MAX_EV,
			},
			[CCI_IF_GLOBAL] = {
				CCI5xx_GLOBAL_PORT_MIN_EV,
				CCI5xx_GLOBAL_PORT_MAX_EV,
			},
		},
		.validate_hw_event = cci550_validate_hw_event,
		.write_counters	= cci5xx_pmu_write_counters,
	},
#endif
};

static const struct of_device_id arm_cci_pmu_matches[] = {
#ifdef CONFIG_ARM_CCI400_PMU
	{
		.compatible = "arm,cci-400-pmu",
		.data	= NULL,
	},
	{
		.compatible = "arm,cci-400-pmu,r0",
		.data	= &cci_pmu_models[CCI400_R0],
	},
	{
		.compatible = "arm,cci-400-pmu,r1",
		.data	= &cci_pmu_models[CCI400_R1],
	},
#endif
#ifdef CONFIG_ARM_CCI5xx_PMU
	{
		.compatible = "arm,cci-500-pmu,r0",
		.data = &cci_pmu_models[CCI500_R0],
	},
	{
		.compatible = "arm,cci-550-pmu,r0",
		.data = &cci_pmu_models[CCI550_R0],
	},
#endif
	{},
};
MODULE_DEVICE_TABLE(of, arm_cci_pmu_matches);

static bool is_duplicate_irq(int irq, int *irqs, int nr_irqs)
{
	int i;

	for (i = 0; i < nr_irqs; i++)
		if (irq == irqs[i])
			return true;

	return false;
}

static struct cci_pmu *cci_pmu_alloc(struct device *dev)
{
	struct cci_pmu *cci_pmu;
	const struct cci_pmu_model *model;

	/*
	 * All allocations are devm_* hence we don't have to free
	 * them explicitly on an error, as it would end up in driver
	 * detach.
	 */
	cci_pmu = devm_kzalloc(dev, sizeof(*cci_pmu), GFP_KERNEL);
	if (!cci_pmu)
		return ERR_PTR(-ENOMEM);

	cci_pmu->ctrl_base = *(void __iomem **)dev->platform_data;

	model = of_device_get_match_data(dev);
	if (!model) {
		dev_warn(dev,
			 "DEPRECATED compatible property, requires secure access to CCI registers");
		model = probe_cci_model(cci_pmu);
	}
	if (!model) {
		dev_warn(dev, "CCI PMU version not supported\n");
		return ERR_PTR(-ENODEV);
	}

	cci_pmu->model = model;
	cci_pmu->irqs = devm_kcalloc(dev, CCI_PMU_MAX_HW_CNTRS(model),
					sizeof(*cci_pmu->irqs), GFP_KERNEL);
	if (!cci_pmu->irqs)
		return ERR_PTR(-ENOMEM);
	cci_pmu->hw_events.events = devm_kcalloc(dev,
					     CCI_PMU_MAX_HW_CNTRS(model),
					     sizeof(*cci_pmu->hw_events.events),
					     GFP_KERNEL);
	if (!cci_pmu->hw_events.events)
		return ERR_PTR(-ENOMEM);
	cci_pmu->hw_events.used_mask = devm_kcalloc(dev,
						BITS_TO_LONGS(CCI_PMU_MAX_HW_CNTRS(model)),
						sizeof(*cci_pmu->hw_events.used_mask),
						GFP_KERNEL);
	if (!cci_pmu->hw_events.used_mask)
		return ERR_PTR(-ENOMEM);

	return cci_pmu;
}

static int cci_pmu_probe(struct platform_device *pdev)
{
	struct cci_pmu *cci_pmu;
	int i, ret, irq;

	cci_pmu = cci_pmu_alloc(&pdev->dev);
	if (IS_ERR(cci_pmu))
		return PTR_ERR(cci_pmu);

	cci_pmu->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(cci_pmu->base))
		return -ENOMEM;

	/*
	 * CCI PMU has one overflow interrupt per counter; but some may be tied
	 * together to a common interrupt.
	 */
	cci_pmu->nr_irqs = 0;
	for (i = 0; i < CCI_PMU_MAX_HW_CNTRS(cci_pmu->model); i++) {
		irq = platform_get_irq(pdev, i);
		if (irq < 0)
			break;

		if (is_duplicate_irq(irq, cci_pmu->irqs, cci_pmu->nr_irqs))
			continue;

		cci_pmu->irqs[cci_pmu->nr_irqs++] = irq;
	}

	/*
	 * Ensure that the device tree has as many interrupts as the number
	 * of counters.
	 */
	if (i < CCI_PMU_MAX_HW_CNTRS(cci_pmu->model)) {
		dev_warn(&pdev->dev, "In-correct number of interrupts: %d, should be %d\n",
			i, CCI_PMU_MAX_HW_CNTRS(cci_pmu->model));
		return -EINVAL;
	}

	raw_spin_lock_init(&cci_pmu->hw_events.pmu_lock);
	mutex_init(&cci_pmu->reserve_mutex);
	atomic_set(&cci_pmu->active_events, 0);

	cci_pmu->cpu = raw_smp_processor_id();
	g_cci_pmu = cci_pmu;
	cpuhp_setup_state_nocalls(CPUHP_AP_PERF_ARM_CCI_ONLINE,
				  "perf/arm/cci:online", NULL,
				  cci_pmu_offline_cpu);

	ret = cci_pmu_init(cci_pmu, pdev);
	if (ret)
		goto error_pmu_init;

	pr_info("ARM %s PMU driver probed", cci_pmu->model->name);
	return 0;

error_pmu_init:
	cpuhp_remove_state(CPUHP_AP_PERF_ARM_CCI_ONLINE);
	g_cci_pmu = NULL;
	return ret;
}

static int cci_pmu_remove(struct platform_device *pdev)
{
	if (!g_cci_pmu)
		return 0;

	cpuhp_remove_state(CPUHP_AP_PERF_ARM_CCI_ONLINE);
	perf_pmu_unregister(&g_cci_pmu->pmu);
	g_cci_pmu = NULL;

	return 0;
}

static struct platform_driver cci_pmu_driver = {
	.driver = {
		   .name = DRIVER_NAME,
		   .of_match_table = arm_cci_pmu_matches,
		   .suppress_bind_attrs = true,
		  },
	.probe = cci_pmu_probe,
	.remove = cci_pmu_remove,
};

module_platform_driver(cci_pmu_driver);
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("ARM CCI PMU support");
