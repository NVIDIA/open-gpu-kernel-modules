// SPDX-License-Identifier: GPL-2.0-only
/*
 *  (c) 2005-2016 Advanced Micro Devices, Inc.
 *
 *  Written by Jacob Shin - AMD, Inc.
 *  Maintained by: Borislav Petkov <bp@alien8.de>
 *
 *  All MC4_MISCi registers are shared between cores on a node.
 */
#include <linux/interrupt.h>
#include <linux/notifier.h>
#include <linux/kobject.h>
#include <linux/percpu.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/cpu.h>
#include <linux/smp.h>
#include <linux/string.h>

#include <asm/amd_nb.h>
#include <asm/traps.h>
#include <asm/apic.h>
#include <asm/mce.h>
#include <asm/msr.h>
#include <asm/trace/irq_vectors.h>

#include "internal.h"

#define NR_BLOCKS         5
#define THRESHOLD_MAX     0xFFF
#define INT_TYPE_APIC     0x00020000
#define MASK_VALID_HI     0x80000000
#define MASK_CNTP_HI      0x40000000
#define MASK_LOCKED_HI    0x20000000
#define MASK_LVTOFF_HI    0x00F00000
#define MASK_COUNT_EN_HI  0x00080000
#define MASK_INT_TYPE_HI  0x00060000
#define MASK_OVERFLOW_HI  0x00010000
#define MASK_ERR_COUNT_HI 0x00000FFF
#define MASK_BLKPTR_LO    0xFF000000
#define MCG_XBLK_ADDR     0xC0000400

/* Deferred error settings */
#define MSR_CU_DEF_ERR		0xC0000410
#define MASK_DEF_LVTOFF		0x000000F0
#define MASK_DEF_INT_TYPE	0x00000006
#define DEF_LVT_OFF		0x2
#define DEF_INT_TYPE_APIC	0x2

/* Scalable MCA: */

/* Threshold LVT offset is at MSR0xC0000410[15:12] */
#define SMCA_THR_LVT_OFF	0xF000

static bool thresholding_irq_en;

static const char * const th_names[] = {
	"load_store",
	"insn_fetch",
	"combined_unit",
	"decode_unit",
	"northbridge",
	"execution_unit",
};

static const char * const smca_umc_block_names[] = {
	"dram_ecc",
	"misc_umc"
};

struct smca_bank_name {
	const char *name;	/* Short name for sysfs */
	const char *long_name;	/* Long name for pretty-printing */
};

static struct smca_bank_name smca_names[] = {
	[SMCA_LS]	= { "load_store",	"Load Store Unit" },
	[SMCA_LS_V2]	= { "load_store",	"Load Store Unit" },
	[SMCA_IF]	= { "insn_fetch",	"Instruction Fetch Unit" },
	[SMCA_L2_CACHE]	= { "l2_cache",		"L2 Cache" },
	[SMCA_DE]	= { "decode_unit",	"Decode Unit" },
	[SMCA_RESERVED]	= { "reserved",		"Reserved" },
	[SMCA_EX]	= { "execution_unit",	"Execution Unit" },
	[SMCA_FP]	= { "floating_point",	"Floating Point Unit" },
	[SMCA_L3_CACHE]	= { "l3_cache",		"L3 Cache" },
	[SMCA_CS]	= { "coherent_slave",	"Coherent Slave" },
	[SMCA_CS_V2]	= { "coherent_slave",	"Coherent Slave" },
	[SMCA_PIE]	= { "pie",		"Power, Interrupts, etc." },
	[SMCA_UMC]	= { "umc",		"Unified Memory Controller" },
	[SMCA_PB]	= { "param_block",	"Parameter Block" },
	[SMCA_PSP]	= { "psp",		"Platform Security Processor" },
	[SMCA_PSP_V2]	= { "psp",		"Platform Security Processor" },
	[SMCA_SMU]	= { "smu",		"System Management Unit" },
	[SMCA_SMU_V2]	= { "smu",		"System Management Unit" },
	[SMCA_MP5]	= { "mp5",		"Microprocessor 5 Unit" },
	[SMCA_NBIO]	= { "nbio",		"Northbridge IO Unit" },
	[SMCA_PCIE]	= { "pcie",		"PCI Express Unit" },
};

static const char *smca_get_name(enum smca_bank_types t)
{
	if (t >= N_SMCA_BANK_TYPES)
		return NULL;

	return smca_names[t].name;
}

const char *smca_get_long_name(enum smca_bank_types t)
{
	if (t >= N_SMCA_BANK_TYPES)
		return NULL;

	return smca_names[t].long_name;
}
EXPORT_SYMBOL_GPL(smca_get_long_name);

static enum smca_bank_types smca_get_bank_type(unsigned int bank)
{
	struct smca_bank *b;

	if (bank >= MAX_NR_BANKS)
		return N_SMCA_BANK_TYPES;

	b = &smca_banks[bank];
	if (!b->hwid)
		return N_SMCA_BANK_TYPES;

	return b->hwid->bank_type;
}

static struct smca_hwid smca_hwid_mcatypes[] = {
	/* { bank_type, hwid_mcatype } */

	/* Reserved type */
	{ SMCA_RESERVED, HWID_MCATYPE(0x00, 0x0)	},

	/* ZN Core (HWID=0xB0) MCA types */
	{ SMCA_LS,	 HWID_MCATYPE(0xB0, 0x0)	},
	{ SMCA_LS_V2,	 HWID_MCATYPE(0xB0, 0x10)	},
	{ SMCA_IF,	 HWID_MCATYPE(0xB0, 0x1)	},
	{ SMCA_L2_CACHE, HWID_MCATYPE(0xB0, 0x2)	},
	{ SMCA_DE,	 HWID_MCATYPE(0xB0, 0x3)	},
	/* HWID 0xB0 MCATYPE 0x4 is Reserved */
	{ SMCA_EX,	 HWID_MCATYPE(0xB0, 0x5)	},
	{ SMCA_FP,	 HWID_MCATYPE(0xB0, 0x6)	},
	{ SMCA_L3_CACHE, HWID_MCATYPE(0xB0, 0x7)	},

	/* Data Fabric MCA types */
	{ SMCA_CS,	 HWID_MCATYPE(0x2E, 0x0)	},
	{ SMCA_PIE,	 HWID_MCATYPE(0x2E, 0x1)	},
	{ SMCA_CS_V2,	 HWID_MCATYPE(0x2E, 0x2)	},

	/* Unified Memory Controller MCA type */
	{ SMCA_UMC,	 HWID_MCATYPE(0x96, 0x0)	},

	/* Parameter Block MCA type */
	{ SMCA_PB,	 HWID_MCATYPE(0x05, 0x0)	},

	/* Platform Security Processor MCA type */
	{ SMCA_PSP,	 HWID_MCATYPE(0xFF, 0x0)	},
	{ SMCA_PSP_V2,	 HWID_MCATYPE(0xFF, 0x1)	},

	/* System Management Unit MCA type */
	{ SMCA_SMU,	 HWID_MCATYPE(0x01, 0x0)	},
	{ SMCA_SMU_V2,	 HWID_MCATYPE(0x01, 0x1)	},

	/* Microprocessor 5 Unit MCA type */
	{ SMCA_MP5,	 HWID_MCATYPE(0x01, 0x2)	},

	/* Northbridge IO Unit MCA type */
	{ SMCA_NBIO,	 HWID_MCATYPE(0x18, 0x0)	},

	/* PCI Express Unit MCA type */
	{ SMCA_PCIE,	 HWID_MCATYPE(0x46, 0x0)	},
};

struct smca_bank smca_banks[MAX_NR_BANKS];
EXPORT_SYMBOL_GPL(smca_banks);

/*
 * In SMCA enabled processors, we can have multiple banks for a given IP type.
 * So to define a unique name for each bank, we use a temp c-string to append
 * the MCA_IPID[InstanceId] to type's name in get_name().
 *
 * InstanceId is 32 bits which is 8 characters. Make sure MAX_MCATYPE_NAME_LEN
 * is greater than 8 plus 1 (for underscore) plus length of longest type name.
 */
#define MAX_MCATYPE_NAME_LEN	30
static char buf_mcatype[MAX_MCATYPE_NAME_LEN];

static DEFINE_PER_CPU(struct threshold_bank **, threshold_banks);

/*
 * A list of the banks enabled on each logical CPU. Controls which respective
 * descriptors to initialize later in mce_threshold_create_device().
 */
static DEFINE_PER_CPU(unsigned int, bank_map);

/* Map of banks that have more than MCA_MISC0 available. */
static DEFINE_PER_CPU(u32, smca_misc_banks_map);

static void amd_threshold_interrupt(void);
static void amd_deferred_error_interrupt(void);

static void default_deferred_error_interrupt(void)
{
	pr_err("Unexpected deferred interrupt at vector %x\n", DEFERRED_ERROR_VECTOR);
}
void (*deferred_error_int_vector)(void) = default_deferred_error_interrupt;

static void smca_set_misc_banks_map(unsigned int bank, unsigned int cpu)
{
	u32 low, high;

	/*
	 * For SMCA enabled processors, BLKPTR field of the first MISC register
	 * (MCx_MISC0) indicates presence of additional MISC regs set (MISC1-4).
	 */
	if (rdmsr_safe(MSR_AMD64_SMCA_MCx_CONFIG(bank), &low, &high))
		return;

	if (!(low & MCI_CONFIG_MCAX))
		return;

	if (rdmsr_safe(MSR_AMD64_SMCA_MCx_MISC(bank), &low, &high))
		return;

	if (low & MASK_BLKPTR_LO)
		per_cpu(smca_misc_banks_map, cpu) |= BIT(bank);

}

static void smca_configure(unsigned int bank, unsigned int cpu)
{
	unsigned int i, hwid_mcatype;
	struct smca_hwid *s_hwid;
	u32 high, low;
	u32 smca_config = MSR_AMD64_SMCA_MCx_CONFIG(bank);

	/* Set appropriate bits in MCA_CONFIG */
	if (!rdmsr_safe(smca_config, &low, &high)) {
		/*
		 * OS is required to set the MCAX bit to acknowledge that it is
		 * now using the new MSR ranges and new registers under each
		 * bank. It also means that the OS will configure deferred
		 * errors in the new MCx_CONFIG register. If the bit is not set,
		 * uncorrectable errors will cause a system panic.
		 *
		 * MCA_CONFIG[MCAX] is bit 32 (0 in the high portion of the MSR.)
		 */
		high |= BIT(0);

		/*
		 * SMCA sets the Deferred Error Interrupt type per bank.
		 *
		 * MCA_CONFIG[DeferredIntTypeSupported] is bit 5, and tells us
		 * if the DeferredIntType bit field is available.
		 *
		 * MCA_CONFIG[DeferredIntType] is bits [38:37] ([6:5] in the
		 * high portion of the MSR). OS should set this to 0x1 to enable
		 * APIC based interrupt. First, check that no interrupt has been
		 * set.
		 */
		if ((low & BIT(5)) && !((high >> 5) & 0x3))
			high |= BIT(5);

		wrmsr(smca_config, low, high);
	}

	smca_set_misc_banks_map(bank, cpu);

	/* Return early if this bank was already initialized. */
	if (smca_banks[bank].hwid && smca_banks[bank].hwid->hwid_mcatype != 0)
		return;

	if (rdmsr_safe(MSR_AMD64_SMCA_MCx_IPID(bank), &low, &high)) {
		pr_warn("Failed to read MCA_IPID for bank %d\n", bank);
		return;
	}

	hwid_mcatype = HWID_MCATYPE(high & MCI_IPID_HWID,
				    (high & MCI_IPID_MCATYPE) >> 16);

	for (i = 0; i < ARRAY_SIZE(smca_hwid_mcatypes); i++) {
		s_hwid = &smca_hwid_mcatypes[i];
		if (hwid_mcatype == s_hwid->hwid_mcatype) {
			smca_banks[bank].hwid = s_hwid;
			smca_banks[bank].id = low;
			smca_banks[bank].sysfs_id = s_hwid->count++;
			break;
		}
	}
}

struct thresh_restart {
	struct threshold_block	*b;
	int			reset;
	int			set_lvt_off;
	int			lvt_off;
	u16			old_limit;
};

static inline bool is_shared_bank(int bank)
{
	/*
	 * Scalable MCA provides for only one core to have access to the MSRs of
	 * a shared bank.
	 */
	if (mce_flags.smca)
		return false;

	/* Bank 4 is for northbridge reporting and is thus shared */
	return (bank == 4);
}

static const char *bank4_names(const struct threshold_block *b)
{
	switch (b->address) {
	/* MSR4_MISC0 */
	case 0x00000413:
		return "dram";

	case 0xc0000408:
		return "ht_links";

	case 0xc0000409:
		return "l3_cache";

	default:
		WARN(1, "Funny MSR: 0x%08x\n", b->address);
		return "";
	}
};


static bool lvt_interrupt_supported(unsigned int bank, u32 msr_high_bits)
{
	/*
	 * bank 4 supports APIC LVT interrupts implicitly since forever.
	 */
	if (bank == 4)
		return true;

	/*
	 * IntP: interrupt present; if this bit is set, the thresholding
	 * bank can generate APIC LVT interrupts
	 */
	return msr_high_bits & BIT(28);
}

static int lvt_off_valid(struct threshold_block *b, int apic, u32 lo, u32 hi)
{
	int msr = (hi & MASK_LVTOFF_HI) >> 20;

	if (apic < 0) {
		pr_err(FW_BUG "cpu %d, failed to setup threshold interrupt "
		       "for bank %d, block %d (MSR%08X=0x%x%08x)\n", b->cpu,
		       b->bank, b->block, b->address, hi, lo);
		return 0;
	}

	if (apic != msr) {
		/*
		 * On SMCA CPUs, LVT offset is programmed at a different MSR, and
		 * the BIOS provides the value. The original field where LVT offset
		 * was set is reserved. Return early here:
		 */
		if (mce_flags.smca)
			return 0;

		pr_err(FW_BUG "cpu %d, invalid threshold interrupt offset %d "
		       "for bank %d, block %d (MSR%08X=0x%x%08x)\n",
		       b->cpu, apic, b->bank, b->block, b->address, hi, lo);
		return 0;
	}

	return 1;
};

/* Reprogram MCx_MISC MSR behind this threshold bank. */
static void threshold_restart_bank(void *_tr)
{
	struct thresh_restart *tr = _tr;
	u32 hi, lo;

	/* sysfs write might race against an offline operation */
	if (this_cpu_read(threshold_banks))
		return;

	rdmsr(tr->b->address, lo, hi);

	if (tr->b->threshold_limit < (hi & THRESHOLD_MAX))
		tr->reset = 1;	/* limit cannot be lower than err count */

	if (tr->reset) {		/* reset err count and overflow bit */
		hi =
		    (hi & ~(MASK_ERR_COUNT_HI | MASK_OVERFLOW_HI)) |
		    (THRESHOLD_MAX - tr->b->threshold_limit);
	} else if (tr->old_limit) {	/* change limit w/o reset */
		int new_count = (hi & THRESHOLD_MAX) +
		    (tr->old_limit - tr->b->threshold_limit);

		hi = (hi & ~MASK_ERR_COUNT_HI) |
		    (new_count & THRESHOLD_MAX);
	}

	/* clear IntType */
	hi &= ~MASK_INT_TYPE_HI;

	if (!tr->b->interrupt_capable)
		goto done;

	if (tr->set_lvt_off) {
		if (lvt_off_valid(tr->b, tr->lvt_off, lo, hi)) {
			/* set new lvt offset */
			hi &= ~MASK_LVTOFF_HI;
			hi |= tr->lvt_off << 20;
		}
	}

	if (tr->b->interrupt_enable)
		hi |= INT_TYPE_APIC;

 done:

	hi |= MASK_COUNT_EN_HI;
	wrmsr(tr->b->address, lo, hi);
}

static void mce_threshold_block_init(struct threshold_block *b, int offset)
{
	struct thresh_restart tr = {
		.b			= b,
		.set_lvt_off		= 1,
		.lvt_off		= offset,
	};

	b->threshold_limit		= THRESHOLD_MAX;
	threshold_restart_bank(&tr);
};

static int setup_APIC_mce_threshold(int reserved, int new)
{
	if (reserved < 0 && !setup_APIC_eilvt(new, THRESHOLD_APIC_VECTOR,
					      APIC_EILVT_MSG_FIX, 0))
		return new;

	return reserved;
}

static int setup_APIC_deferred_error(int reserved, int new)
{
	if (reserved < 0 && !setup_APIC_eilvt(new, DEFERRED_ERROR_VECTOR,
					      APIC_EILVT_MSG_FIX, 0))
		return new;

	return reserved;
}

static void deferred_error_interrupt_enable(struct cpuinfo_x86 *c)
{
	u32 low = 0, high = 0;
	int def_offset = -1, def_new;

	if (rdmsr_safe(MSR_CU_DEF_ERR, &low, &high))
		return;

	def_new = (low & MASK_DEF_LVTOFF) >> 4;
	if (!(low & MASK_DEF_LVTOFF)) {
		pr_err(FW_BUG "Your BIOS is not setting up LVT offset 0x2 for deferred error IRQs correctly.\n");
		def_new = DEF_LVT_OFF;
		low = (low & ~MASK_DEF_LVTOFF) | (DEF_LVT_OFF << 4);
	}

	def_offset = setup_APIC_deferred_error(def_offset, def_new);
	if ((def_offset == def_new) &&
	    (deferred_error_int_vector != amd_deferred_error_interrupt))
		deferred_error_int_vector = amd_deferred_error_interrupt;

	if (!mce_flags.smca)
		low = (low & ~MASK_DEF_INT_TYPE) | DEF_INT_TYPE_APIC;

	wrmsr(MSR_CU_DEF_ERR, low, high);
}

static u32 smca_get_block_address(unsigned int bank, unsigned int block,
				  unsigned int cpu)
{
	if (!block)
		return MSR_AMD64_SMCA_MCx_MISC(bank);

	if (!(per_cpu(smca_misc_banks_map, cpu) & BIT(bank)))
		return 0;

	return MSR_AMD64_SMCA_MCx_MISCy(bank, block - 1);
}

static u32 get_block_address(u32 current_addr, u32 low, u32 high,
			     unsigned int bank, unsigned int block,
			     unsigned int cpu)
{
	u32 addr = 0, offset = 0;

	if ((bank >= per_cpu(mce_num_banks, cpu)) || (block >= NR_BLOCKS))
		return addr;

	if (mce_flags.smca)
		return smca_get_block_address(bank, block, cpu);

	/* Fall back to method we used for older processors: */
	switch (block) {
	case 0:
		addr = msr_ops.misc(bank);
		break;
	case 1:
		offset = ((low & MASK_BLKPTR_LO) >> 21);
		if (offset)
			addr = MCG_XBLK_ADDR + offset;
		break;
	default:
		addr = ++current_addr;
	}
	return addr;
}

static int
prepare_threshold_block(unsigned int bank, unsigned int block, u32 addr,
			int offset, u32 misc_high)
{
	unsigned int cpu = smp_processor_id();
	u32 smca_low, smca_high;
	struct threshold_block b;
	int new;

	if (!block)
		per_cpu(bank_map, cpu) |= (1 << bank);

	memset(&b, 0, sizeof(b));
	b.cpu			= cpu;
	b.bank			= bank;
	b.block			= block;
	b.address		= addr;
	b.interrupt_capable	= lvt_interrupt_supported(bank, misc_high);

	if (!b.interrupt_capable)
		goto done;

	b.interrupt_enable = 1;

	if (!mce_flags.smca) {
		new = (misc_high & MASK_LVTOFF_HI) >> 20;
		goto set_offset;
	}

	/* Gather LVT offset for thresholding: */
	if (rdmsr_safe(MSR_CU_DEF_ERR, &smca_low, &smca_high))
		goto out;

	new = (smca_low & SMCA_THR_LVT_OFF) >> 12;

set_offset:
	offset = setup_APIC_mce_threshold(offset, new);
	if (offset == new)
		thresholding_irq_en = true;

done:
	mce_threshold_block_init(&b, offset);

out:
	return offset;
}

bool amd_filter_mce(struct mce *m)
{
	enum smca_bank_types bank_type = smca_get_bank_type(m->bank);
	struct cpuinfo_x86 *c = &boot_cpu_data;

	/* See Family 17h Models 10h-2Fh Erratum #1114. */
	if (c->x86 == 0x17 &&
	    c->x86_model >= 0x10 && c->x86_model <= 0x2F &&
	    bank_type == SMCA_IF && XEC(m->status, 0x3f) == 10)
		return true;

	/* NB GART TLB error reporting is disabled by default. */
	if (c->x86 < 0x17) {
		if (m->bank == 4 && XEC(m->status, 0x1f) == 0x5)
			return true;
	}

	return false;
}

/*
 * Turn off thresholding banks for the following conditions:
 * - MC4_MISC thresholding is not supported on Family 0x15.
 * - Prevent possible spurious interrupts from the IF bank on Family 0x17
 *   Models 0x10-0x2F due to Erratum #1114.
 */
static void disable_err_thresholding(struct cpuinfo_x86 *c, unsigned int bank)
{
	int i, num_msrs;
	u64 hwcr;
	bool need_toggle;
	u32 msrs[NR_BLOCKS];

	if (c->x86 == 0x15 && bank == 4) {
		msrs[0] = 0x00000413; /* MC4_MISC0 */
		msrs[1] = 0xc0000408; /* MC4_MISC1 */
		num_msrs = 2;
	} else if (c->x86 == 0x17 &&
		   (c->x86_model >= 0x10 && c->x86_model <= 0x2F)) {

		if (smca_get_bank_type(bank) != SMCA_IF)
			return;

		msrs[0] = MSR_AMD64_SMCA_MCx_MISC(bank);
		num_msrs = 1;
	} else {
		return;
	}

	rdmsrl(MSR_K7_HWCR, hwcr);

	/* McStatusWrEn has to be set */
	need_toggle = !(hwcr & BIT(18));
	if (need_toggle)
		wrmsrl(MSR_K7_HWCR, hwcr | BIT(18));

	/* Clear CntP bit safely */
	for (i = 0; i < num_msrs; i++)
		msr_clear_bit(msrs[i], 62);

	/* restore old settings */
	if (need_toggle)
		wrmsrl(MSR_K7_HWCR, hwcr);
}

/* cpu init entry point, called from mce.c with preempt off */
void mce_amd_feature_init(struct cpuinfo_x86 *c)
{
	unsigned int bank, block, cpu = smp_processor_id();
	u32 low = 0, high = 0, address = 0;
	int offset = -1;


	for (bank = 0; bank < this_cpu_read(mce_num_banks); ++bank) {
		if (mce_flags.smca)
			smca_configure(bank, cpu);

		disable_err_thresholding(c, bank);

		for (block = 0; block < NR_BLOCKS; ++block) {
			address = get_block_address(address, low, high, bank, block, cpu);
			if (!address)
				break;

			if (rdmsr_safe(address, &low, &high))
				break;

			if (!(high & MASK_VALID_HI))
				continue;

			if (!(high & MASK_CNTP_HI)  ||
			     (high & MASK_LOCKED_HI))
				continue;

			offset = prepare_threshold_block(bank, block, address, offset, high);
		}
	}

	if (mce_flags.succor)
		deferred_error_interrupt_enable(c);
}

int umc_normaddr_to_sysaddr(u64 norm_addr, u16 nid, u8 umc, u64 *sys_addr)
{
	u64 dram_base_addr, dram_limit_addr, dram_hole_base;
	/* We start from the normalized address */
	u64 ret_addr = norm_addr;

	u32 tmp;

	u8 die_id_shift, die_id_mask, socket_id_shift, socket_id_mask;
	u8 intlv_num_dies, intlv_num_chan, intlv_num_sockets;
	u8 intlv_addr_sel, intlv_addr_bit;
	u8 num_intlv_bits, hashed_bit;
	u8 lgcy_mmio_hole_en, base = 0;
	u8 cs_mask, cs_id = 0;
	bool hash_enabled = false;

	/* Read D18F0x1B4 (DramOffset), check if base 1 is used. */
	if (amd_df_indirect_read(nid, 0, 0x1B4, umc, &tmp))
		goto out_err;

	/* Remove HiAddrOffset from normalized address, if enabled: */
	if (tmp & BIT(0)) {
		u64 hi_addr_offset = (tmp & GENMASK_ULL(31, 20)) << 8;

		if (norm_addr >= hi_addr_offset) {
			ret_addr -= hi_addr_offset;
			base = 1;
		}
	}

	/* Read D18F0x110 (DramBaseAddress). */
	if (amd_df_indirect_read(nid, 0, 0x110 + (8 * base), umc, &tmp))
		goto out_err;

	/* Check if address range is valid. */
	if (!(tmp & BIT(0))) {
		pr_err("%s: Invalid DramBaseAddress range: 0x%x.\n",
			__func__, tmp);
		goto out_err;
	}

	lgcy_mmio_hole_en = tmp & BIT(1);
	intlv_num_chan	  = (tmp >> 4) & 0xF;
	intlv_addr_sel	  = (tmp >> 8) & 0x7;
	dram_base_addr	  = (tmp & GENMASK_ULL(31, 12)) << 16;

	/* {0, 1, 2, 3} map to address bits {8, 9, 10, 11} respectively */
	if (intlv_addr_sel > 3) {
		pr_err("%s: Invalid interleave address select %d.\n",
			__func__, intlv_addr_sel);
		goto out_err;
	}

	/* Read D18F0x114 (DramLimitAddress). */
	if (amd_df_indirect_read(nid, 0, 0x114 + (8 * base), umc, &tmp))
		goto out_err;

	intlv_num_sockets = (tmp >> 8) & 0x1;
	intlv_num_dies	  = (tmp >> 10) & 0x3;
	dram_limit_addr	  = ((tmp & GENMASK_ULL(31, 12)) << 16) | GENMASK_ULL(27, 0);

	intlv_addr_bit = intlv_addr_sel + 8;

	/* Re-use intlv_num_chan by setting it equal to log2(#channels) */
	switch (intlv_num_chan) {
	case 0:	intlv_num_chan = 0; break;
	case 1: intlv_num_chan = 1; break;
	case 3: intlv_num_chan = 2; break;
	case 5:	intlv_num_chan = 3; break;
	case 7:	intlv_num_chan = 4; break;

	case 8: intlv_num_chan = 1;
		hash_enabled = true;
		break;
	default:
		pr_err("%s: Invalid number of interleaved channels %d.\n",
			__func__, intlv_num_chan);
		goto out_err;
	}

	num_intlv_bits = intlv_num_chan;

	if (intlv_num_dies > 2) {
		pr_err("%s: Invalid number of interleaved nodes/dies %d.\n",
			__func__, intlv_num_dies);
		goto out_err;
	}

	num_intlv_bits += intlv_num_dies;

	/* Add a bit if sockets are interleaved. */
	num_intlv_bits += intlv_num_sockets;

	/* Assert num_intlv_bits <= 4 */
	if (num_intlv_bits > 4) {
		pr_err("%s: Invalid interleave bits %d.\n",
			__func__, num_intlv_bits);
		goto out_err;
	}

	if (num_intlv_bits > 0) {
		u64 temp_addr_x, temp_addr_i, temp_addr_y;
		u8 die_id_bit, sock_id_bit, cs_fabric_id;

		/*
		 * Read FabricBlockInstanceInformation3_CS[BlockFabricID].
		 * This is the fabric id for this coherent slave. Use
		 * umc/channel# as instance id of the coherent slave
		 * for FICAA.
		 */
		if (amd_df_indirect_read(nid, 0, 0x50, umc, &tmp))
			goto out_err;

		cs_fabric_id = (tmp >> 8) & 0xFF;
		die_id_bit   = 0;

		/* If interleaved over more than 1 channel: */
		if (intlv_num_chan) {
			die_id_bit = intlv_num_chan;
			cs_mask	   = (1 << die_id_bit) - 1;
			cs_id	   = cs_fabric_id & cs_mask;
		}

		sock_id_bit = die_id_bit;

		/* Read D18F1x208 (SystemFabricIdMask). */
		if (intlv_num_dies || intlv_num_sockets)
			if (amd_df_indirect_read(nid, 1, 0x208, umc, &tmp))
				goto out_err;

		/* If interleaved over more than 1 die. */
		if (intlv_num_dies) {
			sock_id_bit  = die_id_bit + intlv_num_dies;
			die_id_shift = (tmp >> 24) & 0xF;
			die_id_mask  = (tmp >> 8) & 0xFF;

			cs_id |= ((cs_fabric_id & die_id_mask) >> die_id_shift) << die_id_bit;
		}

		/* If interleaved over more than 1 socket. */
		if (intlv_num_sockets) {
			socket_id_shift	= (tmp >> 28) & 0xF;
			socket_id_mask	= (tmp >> 16) & 0xFF;

			cs_id |= ((cs_fabric_id & socket_id_mask) >> socket_id_shift) << sock_id_bit;
		}

		/*
		 * The pre-interleaved address consists of XXXXXXIIIYYYYY
		 * where III is the ID for this CS, and XXXXXXYYYYY are the
		 * address bits from the post-interleaved address.
		 * "num_intlv_bits" has been calculated to tell us how many "I"
		 * bits there are. "intlv_addr_bit" tells us how many "Y" bits
		 * there are (where "I" starts).
		 */
		temp_addr_y = ret_addr & GENMASK_ULL(intlv_addr_bit-1, 0);
		temp_addr_i = (cs_id << intlv_addr_bit);
		temp_addr_x = (ret_addr & GENMASK_ULL(63, intlv_addr_bit)) << num_intlv_bits;
		ret_addr    = temp_addr_x | temp_addr_i | temp_addr_y;
	}

	/* Add dram base address */
	ret_addr += dram_base_addr;

	/* If legacy MMIO hole enabled */
	if (lgcy_mmio_hole_en) {
		if (amd_df_indirect_read(nid, 0, 0x104, umc, &tmp))
			goto out_err;

		dram_hole_base = tmp & GENMASK(31, 24);
		if (ret_addr >= dram_hole_base)
			ret_addr += (BIT_ULL(32) - dram_hole_base);
	}

	if (hash_enabled) {
		/* Save some parentheses and grab ls-bit at the end. */
		hashed_bit =	(ret_addr >> 12) ^
				(ret_addr >> 18) ^
				(ret_addr >> 21) ^
				(ret_addr >> 30) ^
				cs_id;

		hashed_bit &= BIT(0);

		if (hashed_bit != ((ret_addr >> intlv_addr_bit) & BIT(0)))
			ret_addr ^= BIT(intlv_addr_bit);
	}

	/* Is calculated system address is above DRAM limit address? */
	if (ret_addr > dram_limit_addr)
		goto out_err;

	*sys_addr = ret_addr;
	return 0;

out_err:
	return -EINVAL;
}
EXPORT_SYMBOL_GPL(umc_normaddr_to_sysaddr);

bool amd_mce_is_memory_error(struct mce *m)
{
	/* ErrCodeExt[20:16] */
	u8 xec = (m->status >> 16) & 0x1f;

	if (mce_flags.smca)
		return smca_get_bank_type(m->bank) == SMCA_UMC && xec == 0x0;

	return m->bank == 4 && xec == 0x8;
}

static void __log_error(unsigned int bank, u64 status, u64 addr, u64 misc)
{
	struct mce m;

	mce_setup(&m);

	m.status = status;
	m.misc   = misc;
	m.bank   = bank;
	m.tsc	 = rdtsc();

	if (m.status & MCI_STATUS_ADDRV) {
		m.addr = addr;

		/*
		 * Extract [55:<lsb>] where lsb is the least significant
		 * *valid* bit of the address bits.
		 */
		if (mce_flags.smca) {
			u8 lsb = (m.addr >> 56) & 0x3f;

			m.addr &= GENMASK_ULL(55, lsb);
		}
	}

	if (mce_flags.smca) {
		rdmsrl(MSR_AMD64_SMCA_MCx_IPID(bank), m.ipid);

		if (m.status & MCI_STATUS_SYNDV)
			rdmsrl(MSR_AMD64_SMCA_MCx_SYND(bank), m.synd);
	}

	mce_log(&m);
}

DEFINE_IDTENTRY_SYSVEC(sysvec_deferred_error)
{
	trace_deferred_error_apic_entry(DEFERRED_ERROR_VECTOR);
	inc_irq_stat(irq_deferred_error_count);
	deferred_error_int_vector();
	trace_deferred_error_apic_exit(DEFERRED_ERROR_VECTOR);
	ack_APIC_irq();
}

/*
 * Returns true if the logged error is deferred. False, otherwise.
 */
static inline bool
_log_error_bank(unsigned int bank, u32 msr_stat, u32 msr_addr, u64 misc)
{
	u64 status, addr = 0;

	rdmsrl(msr_stat, status);
	if (!(status & MCI_STATUS_VAL))
		return false;

	if (status & MCI_STATUS_ADDRV)
		rdmsrl(msr_addr, addr);

	__log_error(bank, status, addr, misc);

	wrmsrl(msr_stat, 0);

	return status & MCI_STATUS_DEFERRED;
}

/*
 * We have three scenarios for checking for Deferred errors:
 *
 * 1) Non-SMCA systems check MCA_STATUS and log error if found.
 * 2) SMCA systems check MCA_STATUS. If error is found then log it and also
 *    clear MCA_DESTAT.
 * 3) SMCA systems check MCA_DESTAT, if error was not found in MCA_STATUS, and
 *    log it.
 */
static void log_error_deferred(unsigned int bank)
{
	bool defrd;

	defrd = _log_error_bank(bank, msr_ops.status(bank),
					msr_ops.addr(bank), 0);

	if (!mce_flags.smca)
		return;

	/* Clear MCA_DESTAT if we logged the deferred error from MCA_STATUS. */
	if (defrd) {
		wrmsrl(MSR_AMD64_SMCA_MCx_DESTAT(bank), 0);
		return;
	}

	/*
	 * Only deferred errors are logged in MCA_DE{STAT,ADDR} so just check
	 * for a valid error.
	 */
	_log_error_bank(bank, MSR_AMD64_SMCA_MCx_DESTAT(bank),
			      MSR_AMD64_SMCA_MCx_DEADDR(bank), 0);
}

/* APIC interrupt handler for deferred errors */
static void amd_deferred_error_interrupt(void)
{
	unsigned int bank;

	for (bank = 0; bank < this_cpu_read(mce_num_banks); ++bank)
		log_error_deferred(bank);
}

static void log_error_thresholding(unsigned int bank, u64 misc)
{
	_log_error_bank(bank, msr_ops.status(bank), msr_ops.addr(bank), misc);
}

static void log_and_reset_block(struct threshold_block *block)
{
	struct thresh_restart tr;
	u32 low = 0, high = 0;

	if (!block)
		return;

	if (rdmsr_safe(block->address, &low, &high))
		return;

	if (!(high & MASK_OVERFLOW_HI))
		return;

	/* Log the MCE which caused the threshold event. */
	log_error_thresholding(block->bank, ((u64)high << 32) | low);

	/* Reset threshold block after logging error. */
	memset(&tr, 0, sizeof(tr));
	tr.b = block;
	threshold_restart_bank(&tr);
}

/*
 * Threshold interrupt handler will service THRESHOLD_APIC_VECTOR. The interrupt
 * goes off when error_count reaches threshold_limit.
 */
static void amd_threshold_interrupt(void)
{
	struct threshold_block *first_block = NULL, *block = NULL, *tmp = NULL;
	struct threshold_bank **bp = this_cpu_read(threshold_banks);
	unsigned int bank, cpu = smp_processor_id();

	/*
	 * Validate that the threshold bank has been initialized already. The
	 * handler is installed at boot time, but on a hotplug event the
	 * interrupt might fire before the data has been initialized.
	 */
	if (!bp)
		return;

	for (bank = 0; bank < this_cpu_read(mce_num_banks); ++bank) {
		if (!(per_cpu(bank_map, cpu) & (1 << bank)))
			continue;

		first_block = bp[bank]->blocks;
		if (!first_block)
			continue;

		/*
		 * The first block is also the head of the list. Check it first
		 * before iterating over the rest.
		 */
		log_and_reset_block(first_block);
		list_for_each_entry_safe(block, tmp, &first_block->miscj, miscj)
			log_and_reset_block(block);
	}
}

/*
 * Sysfs Interface
 */

struct threshold_attr {
	struct attribute attr;
	ssize_t (*show) (struct threshold_block *, char *);
	ssize_t (*store) (struct threshold_block *, const char *, size_t count);
};

#define SHOW_FIELDS(name)						\
static ssize_t show_ ## name(struct threshold_block *b, char *buf)	\
{									\
	return sprintf(buf, "%lu\n", (unsigned long) b->name);		\
}
SHOW_FIELDS(interrupt_enable)
SHOW_FIELDS(threshold_limit)

static ssize_t
store_interrupt_enable(struct threshold_block *b, const char *buf, size_t size)
{
	struct thresh_restart tr;
	unsigned long new;

	if (!b->interrupt_capable)
		return -EINVAL;

	if (kstrtoul(buf, 0, &new) < 0)
		return -EINVAL;

	b->interrupt_enable = !!new;

	memset(&tr, 0, sizeof(tr));
	tr.b		= b;

	if (smp_call_function_single(b->cpu, threshold_restart_bank, &tr, 1))
		return -ENODEV;

	return size;
}

static ssize_t
store_threshold_limit(struct threshold_block *b, const char *buf, size_t size)
{
	struct thresh_restart tr;
	unsigned long new;

	if (kstrtoul(buf, 0, &new) < 0)
		return -EINVAL;

	if (new > THRESHOLD_MAX)
		new = THRESHOLD_MAX;
	if (new < 1)
		new = 1;

	memset(&tr, 0, sizeof(tr));
	tr.old_limit = b->threshold_limit;
	b->threshold_limit = new;
	tr.b = b;

	if (smp_call_function_single(b->cpu, threshold_restart_bank, &tr, 1))
		return -ENODEV;

	return size;
}

static ssize_t show_error_count(struct threshold_block *b, char *buf)
{
	u32 lo, hi;

	/* CPU might be offline by now */
	if (rdmsr_on_cpu(b->cpu, b->address, &lo, &hi))
		return -ENODEV;

	return sprintf(buf, "%u\n", ((hi & THRESHOLD_MAX) -
				     (THRESHOLD_MAX - b->threshold_limit)));
}

static struct threshold_attr error_count = {
	.attr = {.name = __stringify(error_count), .mode = 0444 },
	.show = show_error_count,
};

#define RW_ATTR(val)							\
static struct threshold_attr val = {					\
	.attr	= {.name = __stringify(val), .mode = 0644 },		\
	.show	= show_## val,						\
	.store	= store_## val,						\
};

RW_ATTR(interrupt_enable);
RW_ATTR(threshold_limit);

static struct attribute *default_attrs[] = {
	&threshold_limit.attr,
	&error_count.attr,
	NULL,	/* possibly interrupt_enable if supported, see below */
	NULL,
};

#define to_block(k)	container_of(k, struct threshold_block, kobj)
#define to_attr(a)	container_of(a, struct threshold_attr, attr)

static ssize_t show(struct kobject *kobj, struct attribute *attr, char *buf)
{
	struct threshold_block *b = to_block(kobj);
	struct threshold_attr *a = to_attr(attr);
	ssize_t ret;

	ret = a->show ? a->show(b, buf) : -EIO;

	return ret;
}

static ssize_t store(struct kobject *kobj, struct attribute *attr,
		     const char *buf, size_t count)
{
	struct threshold_block *b = to_block(kobj);
	struct threshold_attr *a = to_attr(attr);
	ssize_t ret;

	ret = a->store ? a->store(b, buf, count) : -EIO;

	return ret;
}

static const struct sysfs_ops threshold_ops = {
	.show			= show,
	.store			= store,
};

static void threshold_block_release(struct kobject *kobj);

static struct kobj_type threshold_ktype = {
	.sysfs_ops		= &threshold_ops,
	.default_attrs		= default_attrs,
	.release		= threshold_block_release,
};

static const char *get_name(unsigned int bank, struct threshold_block *b)
{
	enum smca_bank_types bank_type;

	if (!mce_flags.smca) {
		if (b && bank == 4)
			return bank4_names(b);

		return th_names[bank];
	}

	bank_type = smca_get_bank_type(bank);
	if (bank_type >= N_SMCA_BANK_TYPES)
		return NULL;

	if (b && bank_type == SMCA_UMC) {
		if (b->block < ARRAY_SIZE(smca_umc_block_names))
			return smca_umc_block_names[b->block];
		return NULL;
	}

	if (smca_banks[bank].hwid->count == 1)
		return smca_get_name(bank_type);

	snprintf(buf_mcatype, MAX_MCATYPE_NAME_LEN,
		 "%s_%x", smca_get_name(bank_type),
			  smca_banks[bank].sysfs_id);
	return buf_mcatype;
}

static int allocate_threshold_blocks(unsigned int cpu, struct threshold_bank *tb,
				     unsigned int bank, unsigned int block,
				     u32 address)
{
	struct threshold_block *b = NULL;
	u32 low, high;
	int err;

	if ((bank >= this_cpu_read(mce_num_banks)) || (block >= NR_BLOCKS))
		return 0;

	if (rdmsr_safe(address, &low, &high))
		return 0;

	if (!(high & MASK_VALID_HI)) {
		if (block)
			goto recurse;
		else
			return 0;
	}

	if (!(high & MASK_CNTP_HI)  ||
	     (high & MASK_LOCKED_HI))
		goto recurse;

	b = kzalloc(sizeof(struct threshold_block), GFP_KERNEL);
	if (!b)
		return -ENOMEM;

	b->block		= block;
	b->bank			= bank;
	b->cpu			= cpu;
	b->address		= address;
	b->interrupt_enable	= 0;
	b->interrupt_capable	= lvt_interrupt_supported(bank, high);
	b->threshold_limit	= THRESHOLD_MAX;

	if (b->interrupt_capable) {
		threshold_ktype.default_attrs[2] = &interrupt_enable.attr;
		b->interrupt_enable = 1;
	} else {
		threshold_ktype.default_attrs[2] = NULL;
	}

	INIT_LIST_HEAD(&b->miscj);

	/* This is safe as @tb is not visible yet */
	if (tb->blocks)
		list_add(&b->miscj, &tb->blocks->miscj);
	else
		tb->blocks = b;

	err = kobject_init_and_add(&b->kobj, &threshold_ktype, tb->kobj, get_name(bank, b));
	if (err)
		goto out_free;
recurse:
	address = get_block_address(address, low, high, bank, ++block, cpu);
	if (!address)
		return 0;

	err = allocate_threshold_blocks(cpu, tb, bank, block, address);
	if (err)
		goto out_free;

	if (b)
		kobject_uevent(&b->kobj, KOBJ_ADD);

	return 0;

out_free:
	if (b) {
		list_del(&b->miscj);
		kobject_put(&b->kobj);
	}
	return err;
}

static int __threshold_add_blocks(struct threshold_bank *b)
{
	struct list_head *head = &b->blocks->miscj;
	struct threshold_block *pos = NULL;
	struct threshold_block *tmp = NULL;
	int err = 0;

	err = kobject_add(&b->blocks->kobj, b->kobj, b->blocks->kobj.name);
	if (err)
		return err;

	list_for_each_entry_safe(pos, tmp, head, miscj) {

		err = kobject_add(&pos->kobj, b->kobj, pos->kobj.name);
		if (err) {
			list_for_each_entry_safe_reverse(pos, tmp, head, miscj)
				kobject_del(&pos->kobj);

			return err;
		}
	}
	return err;
}

static int threshold_create_bank(struct threshold_bank **bp, unsigned int cpu,
				 unsigned int bank)
{
	struct device *dev = this_cpu_read(mce_device);
	struct amd_northbridge *nb = NULL;
	struct threshold_bank *b = NULL;
	const char *name = get_name(bank, NULL);
	int err = 0;

	if (!dev)
		return -ENODEV;

	if (is_shared_bank(bank)) {
		nb = node_to_amd_nb(topology_die_id(cpu));

		/* threshold descriptor already initialized on this node? */
		if (nb && nb->bank4) {
			/* yes, use it */
			b = nb->bank4;
			err = kobject_add(b->kobj, &dev->kobj, name);
			if (err)
				goto out;

			bp[bank] = b;
			refcount_inc(&b->cpus);

			err = __threshold_add_blocks(b);

			goto out;
		}
	}

	b = kzalloc(sizeof(struct threshold_bank), GFP_KERNEL);
	if (!b) {
		err = -ENOMEM;
		goto out;
	}

	/* Associate the bank with the per-CPU MCE device */
	b->kobj = kobject_create_and_add(name, &dev->kobj);
	if (!b->kobj) {
		err = -EINVAL;
		goto out_free;
	}

	if (is_shared_bank(bank)) {
		b->shared = 1;
		refcount_set(&b->cpus, 1);

		/* nb is already initialized, see above */
		if (nb) {
			WARN_ON(nb->bank4);
			nb->bank4 = b;
		}
	}

	err = allocate_threshold_blocks(cpu, b, bank, 0, msr_ops.misc(bank));
	if (err)
		goto out_kobj;

	bp[bank] = b;
	return 0;

out_kobj:
	kobject_put(b->kobj);
out_free:
	kfree(b);
out:
	return err;
}

static void threshold_block_release(struct kobject *kobj)
{
	kfree(to_block(kobj));
}

static void deallocate_threshold_blocks(struct threshold_bank *bank)
{
	struct threshold_block *pos, *tmp;

	list_for_each_entry_safe(pos, tmp, &bank->blocks->miscj, miscj) {
		list_del(&pos->miscj);
		kobject_put(&pos->kobj);
	}

	kobject_put(&bank->blocks->kobj);
}

static void __threshold_remove_blocks(struct threshold_bank *b)
{
	struct threshold_block *pos = NULL;
	struct threshold_block *tmp = NULL;

	kobject_del(b->kobj);

	list_for_each_entry_safe(pos, tmp, &b->blocks->miscj, miscj)
		kobject_del(&pos->kobj);
}

static void threshold_remove_bank(struct threshold_bank *bank)
{
	struct amd_northbridge *nb;

	if (!bank->blocks)
		goto out_free;

	if (!bank->shared)
		goto out_dealloc;

	if (!refcount_dec_and_test(&bank->cpus)) {
		__threshold_remove_blocks(bank);
		return;
	} else {
		/*
		 * The last CPU on this node using the shared bank is going
		 * away, remove that bank now.
		 */
		nb = node_to_amd_nb(topology_die_id(smp_processor_id()));
		nb->bank4 = NULL;
	}

out_dealloc:
	deallocate_threshold_blocks(bank);

out_free:
	kobject_put(bank->kobj);
	kfree(bank);
}

int mce_threshold_remove_device(unsigned int cpu)
{
	struct threshold_bank **bp = this_cpu_read(threshold_banks);
	unsigned int bank, numbanks = this_cpu_read(mce_num_banks);

	if (!bp)
		return 0;

	/*
	 * Clear the pointer before cleaning up, so that the interrupt won't
	 * touch anything of this.
	 */
	this_cpu_write(threshold_banks, NULL);

	for (bank = 0; bank < numbanks; bank++) {
		if (bp[bank]) {
			threshold_remove_bank(bp[bank]);
			bp[bank] = NULL;
		}
	}
	kfree(bp);
	return 0;
}

/**
 * mce_threshold_create_device - Create the per-CPU MCE threshold device
 * @cpu:	The plugged in CPU
 *
 * Create directories and files for all valid threshold banks.
 *
 * This is invoked from the CPU hotplug callback which was installed in
 * mcheck_init_device(). The invocation happens in context of the hotplug
 * thread running on @cpu.  The callback is invoked on all CPUs which are
 * online when the callback is installed or during a real hotplug event.
 */
int mce_threshold_create_device(unsigned int cpu)
{
	unsigned int numbanks, bank;
	struct threshold_bank **bp;
	int err;

	if (!mce_flags.amd_threshold)
		return 0;

	bp = this_cpu_read(threshold_banks);
	if (bp)
		return 0;

	numbanks = this_cpu_read(mce_num_banks);
	bp = kcalloc(numbanks, sizeof(*bp), GFP_KERNEL);
	if (!bp)
		return -ENOMEM;

	for (bank = 0; bank < numbanks; ++bank) {
		if (!(this_cpu_read(bank_map) & (1 << bank)))
			continue;
		err = threshold_create_bank(bp, cpu, bank);
		if (err)
			goto out_err;
	}
	this_cpu_write(threshold_banks, bp);

	if (thresholding_irq_en)
		mce_threshold_vector = amd_threshold_interrupt;
	return 0;
out_err:
	mce_threshold_remove_device(cpu);
	return err;
}
