/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  (c) 2003-2006 Advanced Micro Devices, Inc.
 */

struct powernow_k8_data {
	unsigned int cpu;

	u32 numps;  /* number of p-states */
	u32 batps;  /* number of p-states supported on battery */

	/* these values are constant when the PSB is used to determine
	 * vid/fid pairings, but are modified during the ->target() call
	 * when ACPI is used */
	u32 rvo;     /* ramp voltage offset */
	u32 irt;     /* isochronous relief time */
	u32 vidmvs;  /* usable value calculated from mvs */
	u32 vstable; /* voltage stabilization time, units 20 us */
	u32 plllock; /* pll lock time, units 1 us */
	u32 exttype; /* extended interface = 1 */

	/* keep track of the current fid / vid or pstate */
	u32 currvid;
	u32 currfid;

	/* the powernow_table includes all frequency and vid/fid pairings:
	 * fid are the lower 8 bits of the index, vid are the upper 8 bits.
	 * frequency is in kHz */
	struct cpufreq_frequency_table  *powernow_table;

	/* the acpi table needs to be kept. it's only available if ACPI was
	 * used to determine valid frequency/vid/fid states */
	struct acpi_processor_performance acpi_data;

	/* we need to keep track of associated cores, but let cpufreq
	 * handle hotplug events - so just point at cpufreq pol->cpus
	 * structure */
	struct cpumask *available_cores;
};

/* processor's cpuid instruction support */
#define CPUID_PROCESSOR_SIGNATURE	1	/* function 1 */
#define CPUID_XFAM			0x0ff00000	/* extended family */
#define CPUID_XFAM_K8			0
#define CPUID_XMOD			0x000f0000	/* extended model */
#define CPUID_XMOD_REV_MASK		0x000c0000
#define CPUID_XFAM_10H			0x00100000	/* family 0x10 */
#define CPUID_USE_XFAM_XMOD		0x00000f00
#define CPUID_GET_MAX_CAPABILITIES	0x80000000
#define CPUID_FREQ_VOLT_CAPABILITIES	0x80000007
#define P_STATE_TRANSITION_CAPABLE	6

/* Model Specific Registers for p-state transitions. MSRs are 64-bit. For     */
/* writes (wrmsr - opcode 0f 30), the register number is placed in ecx, and   */
/* the value to write is placed in edx:eax. For reads (rdmsr - opcode 0f 32), */
/* the register number is placed in ecx, and the data is returned in edx:eax. */

#define MSR_FIDVID_CTL      0xc0010041
#define MSR_FIDVID_STATUS   0xc0010042

/* Field definitions within the FID VID Low Control MSR : */
#define MSR_C_LO_INIT_FID_VID     0x00010000
#define MSR_C_LO_NEW_VID          0x00003f00
#define MSR_C_LO_NEW_FID          0x0000003f
#define MSR_C_LO_VID_SHIFT        8

/* Field definitions within the FID VID High Control MSR : */
#define MSR_C_HI_STP_GNT_TO	  0x000fffff

/* Field definitions within the FID VID Low Status MSR : */
#define MSR_S_LO_CHANGE_PENDING   0x80000000   /* cleared when completed */
#define MSR_S_LO_MAX_RAMP_VID     0x3f000000
#define MSR_S_LO_MAX_FID          0x003f0000
#define MSR_S_LO_START_FID        0x00003f00
#define MSR_S_LO_CURRENT_FID      0x0000003f

/* Field definitions within the FID VID High Status MSR : */
#define MSR_S_HI_MIN_WORKING_VID  0x3f000000
#define MSR_S_HI_MAX_WORKING_VID  0x003f0000
#define MSR_S_HI_START_VID        0x00003f00
#define MSR_S_HI_CURRENT_VID      0x0000003f
#define MSR_C_HI_STP_GNT_BENIGN	  0x00000001

/*
 * There are restrictions frequencies have to follow:
 * - only 1 entry in the low fid table ( <=1.4GHz )
 * - lowest entry in the high fid table must be >= 2 * the entry in the
 *   low fid table
 * - lowest entry in the high fid table must be a <= 200MHz + 2 * the entry
 *   in the low fid table
 * - the parts can only step at <= 200 MHz intervals, odd fid values are
 *   supported in revision G and later revisions.
 * - lowest frequency must be >= interprocessor hypertransport link speed
 *   (only applies to MP systems obviously)
 */

/* fids (frequency identifiers) are arranged in 2 tables - lo and hi */
#define LO_FID_TABLE_TOP     7	/* fid values marking the boundary    */
#define HI_FID_TABLE_BOTTOM  8	/* between the low and high tables    */

#define LO_VCOFREQ_TABLE_TOP    1400	/* corresponding vco frequency values */
#define HI_VCOFREQ_TABLE_BOTTOM 1600

#define MIN_FREQ_RESOLUTION  200 /* fids jump by 2 matching freq jumps by 200 */

#define MAX_FID 0x2a	/* Spec only gives FID values as far as 5 GHz */
#define LEAST_VID 0x3e	/* Lowest (numerically highest) useful vid value */

#define MIN_FREQ 800	/* Min and max freqs, per spec */
#define MAX_FREQ 5000

#define INVALID_FID_MASK 0xffffffc0  /* not a valid fid if these bits are set */
#define INVALID_VID_MASK 0xffffffc0  /* not a valid vid if these bits are set */

#define VID_OFF 0x3f

#define STOP_GRANT_5NS 1 /* min poss memory access latency for voltage change */

#define PLL_LOCK_CONVERSION (1000/5) /* ms to ns, then divide by clock period */

#define MAXIMUM_VID_STEPS 1  /* Current cpus only allow a single step of 25mV */
#define VST_UNITS_20US 20   /* Voltage Stabilization Time is in units of 20us */

/*
 * Most values of interest are encoded in a single field of the _PSS
 * entries: the "control" value.
 */

#define IRT_SHIFT      30
#define RVO_SHIFT      28
#define EXT_TYPE_SHIFT 27
#define PLL_L_SHIFT    20
#define MVS_SHIFT      18
#define VST_SHIFT      11
#define VID_SHIFT       6
#define IRT_MASK        3
#define RVO_MASK        3
#define EXT_TYPE_MASK   1
#define PLL_L_MASK   0x7f
#define MVS_MASK        3
#define VST_MASK     0x7f
#define VID_MASK     0x1f
#define FID_MASK     0x1f
#define EXT_VID_MASK 0x3f
#define EXT_FID_MASK 0x3f


/*
 * Version 1.4 of the PSB table. This table is constructed by BIOS and is
 * to tell the OS's power management driver which VIDs and FIDs are
 * supported by this particular processor.
 * If the data in the PSB / PST is wrong, then this driver will program the
 * wrong values into hardware, which is very likely to lead to a crash.
 */

#define PSB_ID_STRING      "AMDK7PNOW!"
#define PSB_ID_STRING_LEN  10

#define PSB_VERSION_1_4  0x14

struct psb_s {
	u8 signature[10];
	u8 tableversion;
	u8 flags1;
	u16 vstable;
	u8 flags2;
	u8 num_tables;
	u32 cpuid;
	u8 plllocktime;
	u8 maxfid;
	u8 maxvid;
	u8 numps;
};

/* Pairs of fid/vid values are appended to the version 1.4 PSB table. */
struct pst_s {
	u8 fid;
	u8 vid;
};

static int core_voltage_pre_transition(struct powernow_k8_data *data,
	u32 reqvid, u32 regfid);
static int core_voltage_post_transition(struct powernow_k8_data *data, u32 reqvid);
static int core_frequency_transition(struct powernow_k8_data *data, u32 reqfid);

static void powernow_k8_acpi_pst_values(struct powernow_k8_data *data, unsigned int index);

static int fill_powernow_table_fidvid(struct powernow_k8_data *data, struct cpufreq_frequency_table *powernow_table);
