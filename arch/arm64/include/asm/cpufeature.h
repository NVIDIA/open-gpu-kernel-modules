/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2014 Linaro Ltd. <ard.biesheuvel@linaro.org>
 */

#ifndef __ASM_CPUFEATURE_H
#define __ASM_CPUFEATURE_H

#include <asm/cpucaps.h>
#include <asm/cputype.h>
#include <asm/hwcap.h>
#include <asm/sysreg.h>

#define MAX_CPU_FEATURES	64
#define cpu_feature(x)		KERNEL_HWCAP_ ## x

#ifndef __ASSEMBLY__

#include <linux/bug.h>
#include <linux/jump_label.h>
#include <linux/kernel.h>

/*
 * CPU feature register tracking
 *
 * The safe value of a CPUID feature field is dependent on the implications
 * of the values assigned to it by the architecture. Based on the relationship
 * between the values, the features are classified into 3 types - LOWER_SAFE,
 * HIGHER_SAFE and EXACT.
 *
 * The lowest value of all the CPUs is chosen for LOWER_SAFE and highest
 * for HIGHER_SAFE. It is expected that all CPUs have the same value for
 * a field when EXACT is specified, failing which, the safe value specified
 * in the table is chosen.
 */

enum ftr_type {
	FTR_EXACT,			/* Use a predefined safe value */
	FTR_LOWER_SAFE,			/* Smaller value is safe */
	FTR_HIGHER_SAFE,		/* Bigger value is safe */
	FTR_HIGHER_OR_ZERO_SAFE,	/* Bigger value is safe, but 0 is biggest */
};

#define FTR_STRICT	true	/* SANITY check strict matching required */
#define FTR_NONSTRICT	false	/* SANITY check ignored */

#define FTR_SIGNED	true	/* Value should be treated as signed */
#define FTR_UNSIGNED	false	/* Value should be treated as unsigned */

#define FTR_VISIBLE	true	/* Feature visible to the user space */
#define FTR_HIDDEN	false	/* Feature is hidden from the user */

#define FTR_VISIBLE_IF_IS_ENABLED(config)		\
	(IS_ENABLED(config) ? FTR_VISIBLE : FTR_HIDDEN)

struct arm64_ftr_bits {
	bool		sign;	/* Value is signed ? */
	bool		visible;
	bool		strict;	/* CPU Sanity check: strict matching required ? */
	enum ftr_type	type;
	u8		shift;
	u8		width;
	s64		safe_val; /* safe value for FTR_EXACT features */
};

/*
 * Describe the early feature override to the core override code:
 *
 * @val			Values that are to be merged into the final
 *			sanitised value of the register. Only the bitfields
 *			set to 1 in @mask are valid
 * @mask		Mask of the features that are overridden by @val
 *
 * A @mask field set to full-1 indicates that the corresponding field
 * in @val is a valid override.
 *
 * A @mask field set to full-0 with the corresponding @val field set
 * to full-0 denotes that this field has no override
 *
 * A @mask field set to full-0 with the corresponding @val field set
 * to full-1 denotes thath this field has an invalid override.
 */
struct arm64_ftr_override {
	u64		val;
	u64		mask;
};

/*
 * @arm64_ftr_reg - Feature register
 * @strict_mask		Bits which should match across all CPUs for sanity.
 * @sys_val		Safe value across the CPUs (system view)
 */
struct arm64_ftr_reg {
	const char			*name;
	u64				strict_mask;
	u64				user_mask;
	u64				sys_val;
	u64				user_val;
	struct arm64_ftr_override	*override;
	const struct arm64_ftr_bits	*ftr_bits;
};

extern struct arm64_ftr_reg arm64_ftr_reg_ctrel0;

/*
 * CPU capabilities:
 *
 * We use arm64_cpu_capabilities to represent system features, errata work
 * arounds (both used internally by kernel and tracked in cpu_hwcaps) and
 * ELF HWCAPs (which are exposed to user).
 *
 * To support systems with heterogeneous CPUs, we need to make sure that we
 * detect the capabilities correctly on the system and take appropriate
 * measures to ensure there are no incompatibilities.
 *
 * This comment tries to explain how we treat the capabilities.
 * Each capability has the following list of attributes :
 *
 * 1) Scope of Detection : The system detects a given capability by
 *    performing some checks at runtime. This could be, e.g, checking the
 *    value of a field in CPU ID feature register or checking the cpu
 *    model. The capability provides a call back ( @matches() ) to
 *    perform the check. Scope defines how the checks should be performed.
 *    There are three cases:
 *
 *     a) SCOPE_LOCAL_CPU: check all the CPUs and "detect" if at least one
 *        matches. This implies, we have to run the check on all the
 *        booting CPUs, until the system decides that state of the
 *        capability is finalised. (See section 2 below)
 *		Or
 *     b) SCOPE_SYSTEM: check all the CPUs and "detect" if all the CPUs
 *        matches. This implies, we run the check only once, when the
 *        system decides to finalise the state of the capability. If the
 *        capability relies on a field in one of the CPU ID feature
 *        registers, we use the sanitised value of the register from the
 *        CPU feature infrastructure to make the decision.
 *		Or
 *     c) SCOPE_BOOT_CPU: Check only on the primary boot CPU to detect the
 *        feature. This category is for features that are "finalised"
 *        (or used) by the kernel very early even before the SMP cpus
 *        are brought up.
 *
 *    The process of detection is usually denoted by "update" capability
 *    state in the code.
 *
 * 2) Finalise the state : The kernel should finalise the state of a
 *    capability at some point during its execution and take necessary
 *    actions if any. Usually, this is done, after all the boot-time
 *    enabled CPUs are brought up by the kernel, so that it can make
 *    better decision based on the available set of CPUs. However, there
 *    are some special cases, where the action is taken during the early
 *    boot by the primary boot CPU. (e.g, running the kernel at EL2 with
 *    Virtualisation Host Extensions). The kernel usually disallows any
 *    changes to the state of a capability once it finalises the capability
 *    and takes any action, as it may be impossible to execute the actions
 *    safely. A CPU brought up after a capability is "finalised" is
 *    referred to as "Late CPU" w.r.t the capability. e.g, all secondary
 *    CPUs are treated "late CPUs" for capabilities determined by the boot
 *    CPU.
 *
 *    At the moment there are two passes of finalising the capabilities.
 *      a) Boot CPU scope capabilities - Finalised by primary boot CPU via
 *         setup_boot_cpu_capabilities().
 *      b) Everything except (a) - Run via setup_system_capabilities().
 *
 * 3) Verification: When a CPU is brought online (e.g, by user or by the
 *    kernel), the kernel should make sure that it is safe to use the CPU,
 *    by verifying that the CPU is compliant with the state of the
 *    capabilities finalised already. This happens via :
 *
 *	secondary_start_kernel()-> check_local_cpu_capabilities()
 *
 *    As explained in (2) above, capabilities could be finalised at
 *    different points in the execution. Each newly booted CPU is verified
 *    against the capabilities that have been finalised by the time it
 *    boots.
 *
 *	a) SCOPE_BOOT_CPU : All CPUs are verified against the capability
 *	except for the primary boot CPU.
 *
 *	b) SCOPE_LOCAL_CPU, SCOPE_SYSTEM: All CPUs hotplugged on by the
 *	user after the kernel boot are verified against the capability.
 *
 *    If there is a conflict, the kernel takes an action, based on the
 *    severity (e.g, a CPU could be prevented from booting or cause a
 *    kernel panic). The CPU is allowed to "affect" the state of the
 *    capability, if it has not been finalised already. See section 5
 *    for more details on conflicts.
 *
 * 4) Action: As mentioned in (2), the kernel can take an action for each
 *    detected capability, on all CPUs on the system. Appropriate actions
 *    include, turning on an architectural feature, modifying the control
 *    registers (e.g, SCTLR, TCR etc.) or patching the kernel via
 *    alternatives. The kernel patching is batched and performed at later
 *    point. The actions are always initiated only after the capability
 *    is finalised. This is usally denoted by "enabling" the capability.
 *    The actions are initiated as follows :
 *	a) Action is triggered on all online CPUs, after the capability is
 *	finalised, invoked within the stop_machine() context from
 *	enable_cpu_capabilitie().
 *
 *	b) Any late CPU, brought up after (1), the action is triggered via:
 *
 *	  check_local_cpu_capabilities() -> verify_local_cpu_capabilities()
 *
 * 5) Conflicts: Based on the state of the capability on a late CPU vs.
 *    the system state, we could have the following combinations :
 *
 *		x-----------------------------x
 *		| Type  | System   | Late CPU |
 *		|-----------------------------|
 *		|  a    |   y      |    n     |
 *		|-----------------------------|
 *		|  b    |   n      |    y     |
 *		x-----------------------------x
 *
 *     Two separate flag bits are defined to indicate whether each kind of
 *     conflict can be allowed:
 *		ARM64_CPUCAP_OPTIONAL_FOR_LATE_CPU - Case(a) is allowed
 *		ARM64_CPUCAP_PERMITTED_FOR_LATE_CPU - Case(b) is allowed
 *
 *     Case (a) is not permitted for a capability that the system requires
 *     all CPUs to have in order for the capability to be enabled. This is
 *     typical for capabilities that represent enhanced functionality.
 *
 *     Case (b) is not permitted for a capability that must be enabled
 *     during boot if any CPU in the system requires it in order to run
 *     safely. This is typical for erratum work arounds that cannot be
 *     enabled after the corresponding capability is finalised.
 *
 *     In some non-typical cases either both (a) and (b), or neither,
 *     should be permitted. This can be described by including neither
 *     or both flags in the capability's type field.
 *
 *     In case of a conflict, the CPU is prevented from booting. If the
 *     ARM64_CPUCAP_PANIC_ON_CONFLICT flag is specified for the capability,
 *     then a kernel panic is triggered.
 */


/*
 * Decide how the capability is detected.
 * On any local CPU vs System wide vs the primary boot CPU
 */
#define ARM64_CPUCAP_SCOPE_LOCAL_CPU		((u16)BIT(0))
#define ARM64_CPUCAP_SCOPE_SYSTEM		((u16)BIT(1))
/*
 * The capabilitiy is detected on the Boot CPU and is used by kernel
 * during early boot. i.e, the capability should be "detected" and
 * "enabled" as early as possibly on all booting CPUs.
 */
#define ARM64_CPUCAP_SCOPE_BOOT_CPU		((u16)BIT(2))
#define ARM64_CPUCAP_SCOPE_MASK			\
	(ARM64_CPUCAP_SCOPE_SYSTEM	|	\
	 ARM64_CPUCAP_SCOPE_LOCAL_CPU	|	\
	 ARM64_CPUCAP_SCOPE_BOOT_CPU)

#define SCOPE_SYSTEM				ARM64_CPUCAP_SCOPE_SYSTEM
#define SCOPE_LOCAL_CPU				ARM64_CPUCAP_SCOPE_LOCAL_CPU
#define SCOPE_BOOT_CPU				ARM64_CPUCAP_SCOPE_BOOT_CPU
#define SCOPE_ALL				ARM64_CPUCAP_SCOPE_MASK

/*
 * Is it permitted for a late CPU to have this capability when system
 * hasn't already enabled it ?
 */
#define ARM64_CPUCAP_PERMITTED_FOR_LATE_CPU	((u16)BIT(4))
/* Is it safe for a late CPU to miss this capability when system has it */
#define ARM64_CPUCAP_OPTIONAL_FOR_LATE_CPU	((u16)BIT(5))
/* Panic when a conflict is detected */
#define ARM64_CPUCAP_PANIC_ON_CONFLICT		((u16)BIT(6))

/*
 * CPU errata workarounds that need to be enabled at boot time if one or
 * more CPUs in the system requires it. When one of these capabilities
 * has been enabled, it is safe to allow any CPU to boot that doesn't
 * require the workaround. However, it is not safe if a "late" CPU
 * requires a workaround and the system hasn't enabled it already.
 */
#define ARM64_CPUCAP_LOCAL_CPU_ERRATUM		\
	(ARM64_CPUCAP_SCOPE_LOCAL_CPU | ARM64_CPUCAP_OPTIONAL_FOR_LATE_CPU)
/*
 * CPU feature detected at boot time based on system-wide value of a
 * feature. It is safe for a late CPU to have this feature even though
 * the system hasn't enabled it, although the feature will not be used
 * by Linux in this case. If the system has enabled this feature already,
 * then every late CPU must have it.
 */
#define ARM64_CPUCAP_SYSTEM_FEATURE	\
	(ARM64_CPUCAP_SCOPE_SYSTEM | ARM64_CPUCAP_PERMITTED_FOR_LATE_CPU)
/*
 * CPU feature detected at boot time based on feature of one or more CPUs.
 * All possible conflicts for a late CPU are ignored.
 * NOTE: this means that a late CPU with the feature will *not* cause the
 * capability to be advertised by cpus_have_*cap()!
 */
#define ARM64_CPUCAP_WEAK_LOCAL_CPU_FEATURE		\
	(ARM64_CPUCAP_SCOPE_LOCAL_CPU		|	\
	 ARM64_CPUCAP_OPTIONAL_FOR_LATE_CPU	|	\
	 ARM64_CPUCAP_PERMITTED_FOR_LATE_CPU)

/*
 * CPU feature detected at boot time, on one or more CPUs. A late CPU
 * is not allowed to have the capability when the system doesn't have it.
 * It is Ok for a late CPU to miss the feature.
 */
#define ARM64_CPUCAP_BOOT_RESTRICTED_CPU_LOCAL_FEATURE	\
	(ARM64_CPUCAP_SCOPE_LOCAL_CPU		|	\
	 ARM64_CPUCAP_OPTIONAL_FOR_LATE_CPU)

/*
 * CPU feature used early in the boot based on the boot CPU. All secondary
 * CPUs must match the state of the capability as detected by the boot CPU. In
 * case of a conflict, a kernel panic is triggered.
 */
#define ARM64_CPUCAP_STRICT_BOOT_CPU_FEATURE		\
	(ARM64_CPUCAP_SCOPE_BOOT_CPU | ARM64_CPUCAP_PANIC_ON_CONFLICT)

/*
 * CPU feature used early in the boot based on the boot CPU. It is safe for a
 * late CPU to have this feature even though the boot CPU hasn't enabled it,
 * although the feature will not be used by Linux in this case. If the boot CPU
 * has enabled this feature already, then every late CPU must have it.
 */
#define ARM64_CPUCAP_BOOT_CPU_FEATURE                  \
	(ARM64_CPUCAP_SCOPE_BOOT_CPU | ARM64_CPUCAP_PERMITTED_FOR_LATE_CPU)

struct arm64_cpu_capabilities {
	const char *desc;
	u16 capability;
	u16 type;
	bool (*matches)(const struct arm64_cpu_capabilities *caps, int scope);
	/*
	 * Take the appropriate actions to configure this capability
	 * for this CPU. If the capability is detected by the kernel
	 * this will be called on all the CPUs in the system,
	 * including the hotplugged CPUs, regardless of whether the
	 * capability is available on that specific CPU. This is
	 * useful for some capabilities (e.g, working around CPU
	 * errata), where all the CPUs must take some action (e.g,
	 * changing system control/configuration). Thus, if an action
	 * is required only if the CPU has the capability, then the
	 * routine must check it before taking any action.
	 */
	void (*cpu_enable)(const struct arm64_cpu_capabilities *cap);
	union {
		struct {	/* To be used for erratum handling only */
			struct midr_range midr_range;
			const struct arm64_midr_revidr {
				u32 midr_rv;		/* revision/variant */
				u32 revidr_mask;
			} * const fixed_revs;
		};

		const struct midr_range *midr_range_list;
		struct {	/* Feature register checking */
			u32 sys_reg;
			u8 field_pos;
			u8 min_field_value;
			u8 hwcap_type;
			bool sign;
			unsigned long hwcap;
		};
	};

	/*
	 * An optional list of "matches/cpu_enable" pair for the same
	 * "capability" of the same "type" as described by the parent.
	 * Only matches(), cpu_enable() and fields relevant to these
	 * methods are significant in the list. The cpu_enable is
	 * invoked only if the corresponding entry "matches()".
	 * However, if a cpu_enable() method is associated
	 * with multiple matches(), care should be taken that either
	 * the match criteria are mutually exclusive, or that the
	 * method is robust against being called multiple times.
	 */
	const struct arm64_cpu_capabilities *match_list;
};

static inline int cpucap_default_scope(const struct arm64_cpu_capabilities *cap)
{
	return cap->type & ARM64_CPUCAP_SCOPE_MASK;
}

/*
 * Generic helper for handling capabilities with multiple (match,enable) pairs
 * of call backs, sharing the same capability bit.
 * Iterate over each entry to see if at least one matches.
 */
static inline bool
cpucap_multi_entry_cap_matches(const struct arm64_cpu_capabilities *entry,
			       int scope)
{
	const struct arm64_cpu_capabilities *caps;

	for (caps = entry->match_list; caps->matches; caps++)
		if (caps->matches(caps, scope))
			return true;

	return false;
}

static __always_inline bool is_vhe_hyp_code(void)
{
	/* Only defined for code run in VHE hyp context */
	return __is_defined(__KVM_VHE_HYPERVISOR__);
}

static __always_inline bool is_nvhe_hyp_code(void)
{
	/* Only defined for code run in NVHE hyp context */
	return __is_defined(__KVM_NVHE_HYPERVISOR__);
}

static __always_inline bool is_hyp_code(void)
{
	return is_vhe_hyp_code() || is_nvhe_hyp_code();
}

extern DECLARE_BITMAP(cpu_hwcaps, ARM64_NCAPS);
extern struct static_key_false cpu_hwcap_keys[ARM64_NCAPS];
extern struct static_key_false arm64_const_caps_ready;

/* ARM64 CAPS + alternative_cb */
#define ARM64_NPATCHABLE (ARM64_NCAPS + 1)
extern DECLARE_BITMAP(boot_capabilities, ARM64_NPATCHABLE);

#define for_each_available_cap(cap)		\
	for_each_set_bit(cap, cpu_hwcaps, ARM64_NCAPS)

bool this_cpu_has_cap(unsigned int cap);
void cpu_set_feature(unsigned int num);
bool cpu_have_feature(unsigned int num);
unsigned long cpu_get_elf_hwcap(void);
unsigned long cpu_get_elf_hwcap2(void);

#define cpu_set_named_feature(name) cpu_set_feature(cpu_feature(name))
#define cpu_have_named_feature(name) cpu_have_feature(cpu_feature(name))

static __always_inline bool system_capabilities_finalized(void)
{
	return static_branch_likely(&arm64_const_caps_ready);
}

/*
 * Test for a capability with a runtime check.
 *
 * Before the capability is detected, this returns false.
 */
static inline bool cpus_have_cap(unsigned int num)
{
	if (num >= ARM64_NCAPS)
		return false;
	return test_bit(num, cpu_hwcaps);
}

/*
 * Test for a capability without a runtime check.
 *
 * Before capabilities are finalized, this returns false.
 * After capabilities are finalized, this is patched to avoid a runtime check.
 *
 * @num must be a compile-time constant.
 */
static __always_inline bool __cpus_have_const_cap(int num)
{
	if (num >= ARM64_NCAPS)
		return false;
	return static_branch_unlikely(&cpu_hwcap_keys[num]);
}

/*
 * Test for a capability without a runtime check.
 *
 * Before capabilities are finalized, this will BUG().
 * After capabilities are finalized, this is patched to avoid a runtime check.
 *
 * @num must be a compile-time constant.
 */
static __always_inline bool cpus_have_final_cap(int num)
{
	if (system_capabilities_finalized())
		return __cpus_have_const_cap(num);
	else
		BUG();
}

/*
 * Test for a capability, possibly with a runtime check for non-hyp code.
 *
 * For hyp code, this behaves the same as cpus_have_final_cap().
 *
 * For non-hyp code:
 * Before capabilities are finalized, this behaves as cpus_have_cap().
 * After capabilities are finalized, this is patched to avoid a runtime check.
 *
 * @num must be a compile-time constant.
 */
static __always_inline bool cpus_have_const_cap(int num)
{
	if (is_hyp_code())
		return cpus_have_final_cap(num);
	else if (system_capabilities_finalized())
		return __cpus_have_const_cap(num);
	else
		return cpus_have_cap(num);
}

static inline void cpus_set_cap(unsigned int num)
{
	if (num >= ARM64_NCAPS) {
		pr_warn("Attempt to set an illegal CPU capability (%d >= %d)\n",
			num, ARM64_NCAPS);
	} else {
		__set_bit(num, cpu_hwcaps);
	}
}

static inline int __attribute_const__
cpuid_feature_extract_signed_field_width(u64 features, int field, int width)
{
	return (s64)(features << (64 - width - field)) >> (64 - width);
}

static inline int __attribute_const__
cpuid_feature_extract_signed_field(u64 features, int field)
{
	return cpuid_feature_extract_signed_field_width(features, field, 4);
}

static __always_inline unsigned int __attribute_const__
cpuid_feature_extract_unsigned_field_width(u64 features, int field, int width)
{
	return (u64)(features << (64 - width - field)) >> (64 - width);
}

static __always_inline unsigned int __attribute_const__
cpuid_feature_extract_unsigned_field(u64 features, int field)
{
	return cpuid_feature_extract_unsigned_field_width(features, field, 4);
}

/*
 * Fields that identify the version of the Performance Monitors Extension do
 * not follow the standard ID scheme. See ARM DDI 0487E.a page D13-2825,
 * "Alternative ID scheme used for the Performance Monitors Extension version".
 */
static inline u64 __attribute_const__
cpuid_feature_cap_perfmon_field(u64 features, int field, u64 cap)
{
	u64 val = cpuid_feature_extract_unsigned_field(features, field);
	u64 mask = GENMASK_ULL(field + 3, field);

	/* Treat IMPLEMENTATION DEFINED functionality as unimplemented */
	if (val == 0xf)
		val = 0;

	if (val > cap) {
		features &= ~mask;
		features |= (cap << field) & mask;
	}

	return features;
}

static inline u64 arm64_ftr_mask(const struct arm64_ftr_bits *ftrp)
{
	return (u64)GENMASK(ftrp->shift + ftrp->width - 1, ftrp->shift);
}

static inline u64 arm64_ftr_reg_user_value(const struct arm64_ftr_reg *reg)
{
	return (reg->user_val | (reg->sys_val & reg->user_mask));
}

static inline int __attribute_const__
cpuid_feature_extract_field_width(u64 features, int field, int width, bool sign)
{
	return (sign) ?
		cpuid_feature_extract_signed_field_width(features, field, width) :
		cpuid_feature_extract_unsigned_field_width(features, field, width);
}

static inline int __attribute_const__
cpuid_feature_extract_field(u64 features, int field, bool sign)
{
	return cpuid_feature_extract_field_width(features, field, 4, sign);
}

static inline s64 arm64_ftr_value(const struct arm64_ftr_bits *ftrp, u64 val)
{
	return (s64)cpuid_feature_extract_field_width(val, ftrp->shift, ftrp->width, ftrp->sign);
}

static inline bool id_aa64mmfr0_mixed_endian_el0(u64 mmfr0)
{
	return cpuid_feature_extract_unsigned_field(mmfr0, ID_AA64MMFR0_BIGENDEL_SHIFT) == 0x1 ||
		cpuid_feature_extract_unsigned_field(mmfr0, ID_AA64MMFR0_BIGENDEL0_SHIFT) == 0x1;
}

static inline bool id_aa64pfr0_32bit_el1(u64 pfr0)
{
	u32 val = cpuid_feature_extract_unsigned_field(pfr0, ID_AA64PFR0_EL1_SHIFT);

	return val == ID_AA64PFR0_EL1_32BIT_64BIT;
}

static inline bool id_aa64pfr0_32bit_el0(u64 pfr0)
{
	u32 val = cpuid_feature_extract_unsigned_field(pfr0, ID_AA64PFR0_EL0_SHIFT);

	return val == ID_AA64PFR0_EL0_32BIT_64BIT;
}

static inline bool id_aa64pfr0_sve(u64 pfr0)
{
	u32 val = cpuid_feature_extract_unsigned_field(pfr0, ID_AA64PFR0_SVE_SHIFT);

	return val > 0;
}

void __init setup_cpu_features(void);
void check_local_cpu_capabilities(void);

u64 read_sanitised_ftr_reg(u32 id);
u64 __read_sysreg_by_encoding(u32 sys_id);

static inline bool cpu_supports_mixed_endian_el0(void)
{
	return id_aa64mmfr0_mixed_endian_el0(read_cpuid(ID_AA64MMFR0_EL1));
}

static inline bool system_supports_32bit_el0(void)
{
	return cpus_have_const_cap(ARM64_HAS_32BIT_EL0);
}

static inline bool system_supports_4kb_granule(void)
{
	u64 mmfr0;
	u32 val;

	mmfr0 =	read_sanitised_ftr_reg(SYS_ID_AA64MMFR0_EL1);
	val = cpuid_feature_extract_unsigned_field(mmfr0,
						ID_AA64MMFR0_TGRAN4_SHIFT);

	return val == ID_AA64MMFR0_TGRAN4_SUPPORTED;
}

static inline bool system_supports_64kb_granule(void)
{
	u64 mmfr0;
	u32 val;

	mmfr0 =	read_sanitised_ftr_reg(SYS_ID_AA64MMFR0_EL1);
	val = cpuid_feature_extract_unsigned_field(mmfr0,
						ID_AA64MMFR0_TGRAN64_SHIFT);

	return val == ID_AA64MMFR0_TGRAN64_SUPPORTED;
}

static inline bool system_supports_16kb_granule(void)
{
	u64 mmfr0;
	u32 val;

	mmfr0 =	read_sanitised_ftr_reg(SYS_ID_AA64MMFR0_EL1);
	val = cpuid_feature_extract_unsigned_field(mmfr0,
						ID_AA64MMFR0_TGRAN16_SHIFT);

	return val == ID_AA64MMFR0_TGRAN16_SUPPORTED;
}

static inline bool system_supports_mixed_endian_el0(void)
{
	return id_aa64mmfr0_mixed_endian_el0(read_sanitised_ftr_reg(SYS_ID_AA64MMFR0_EL1));
}

static inline bool system_supports_mixed_endian(void)
{
	u64 mmfr0;
	u32 val;

	mmfr0 =	read_sanitised_ftr_reg(SYS_ID_AA64MMFR0_EL1);
	val = cpuid_feature_extract_unsigned_field(mmfr0,
						ID_AA64MMFR0_BIGENDEL_SHIFT);

	return val == 0x1;
}

static __always_inline bool system_supports_fpsimd(void)
{
	return !cpus_have_const_cap(ARM64_HAS_NO_FPSIMD);
}

static inline bool system_uses_hw_pan(void)
{
	return IS_ENABLED(CONFIG_ARM64_PAN) &&
		cpus_have_const_cap(ARM64_HAS_PAN);
}

static inline bool system_uses_ttbr0_pan(void)
{
	return IS_ENABLED(CONFIG_ARM64_SW_TTBR0_PAN) &&
		!system_uses_hw_pan();
}

static __always_inline bool system_supports_sve(void)
{
	return IS_ENABLED(CONFIG_ARM64_SVE) &&
		cpus_have_const_cap(ARM64_SVE);
}

static __always_inline bool system_supports_cnp(void)
{
	return IS_ENABLED(CONFIG_ARM64_CNP) &&
		cpus_have_const_cap(ARM64_HAS_CNP);
}

static inline bool system_supports_address_auth(void)
{
	return IS_ENABLED(CONFIG_ARM64_PTR_AUTH) &&
		cpus_have_const_cap(ARM64_HAS_ADDRESS_AUTH);
}

static inline bool system_supports_generic_auth(void)
{
	return IS_ENABLED(CONFIG_ARM64_PTR_AUTH) &&
		cpus_have_const_cap(ARM64_HAS_GENERIC_AUTH);
}

static inline bool system_has_full_ptr_auth(void)
{
	return system_supports_address_auth() && system_supports_generic_auth();
}

static __always_inline bool system_uses_irq_prio_masking(void)
{
	return IS_ENABLED(CONFIG_ARM64_PSEUDO_NMI) &&
	       cpus_have_const_cap(ARM64_HAS_IRQ_PRIO_MASKING);
}

static inline bool system_supports_mte(void)
{
	return IS_ENABLED(CONFIG_ARM64_MTE) &&
		cpus_have_const_cap(ARM64_MTE);
}

static inline bool system_has_prio_mask_debugging(void)
{
	return IS_ENABLED(CONFIG_ARM64_DEBUG_PRIORITY_MASKING) &&
	       system_uses_irq_prio_masking();
}

static inline bool system_supports_bti(void)
{
	return IS_ENABLED(CONFIG_ARM64_BTI) && cpus_have_const_cap(ARM64_BTI);
}

static inline bool system_supports_tlb_range(void)
{
	return IS_ENABLED(CONFIG_ARM64_TLB_RANGE) &&
		cpus_have_const_cap(ARM64_HAS_TLB_RANGE);
}

extern int do_emulate_mrs(struct pt_regs *regs, u32 sys_reg, u32 rt);

static inline u32 id_aa64mmfr0_parange_to_phys_shift(int parange)
{
	switch (parange) {
	case 0: return 32;
	case 1: return 36;
	case 2: return 40;
	case 3: return 42;
	case 4: return 44;
	case 5: return 48;
	case 6: return 52;
	/*
	 * A future PE could use a value unknown to the kernel.
	 * However, by the "D10.1.4 Principles of the ID scheme
	 * for fields in ID registers", ARM DDI 0487C.a, any new
	 * value is guaranteed to be higher than what we know already.
	 * As a safe limit, we return the limit supported by the kernel.
	 */
	default: return CONFIG_ARM64_PA_BITS;
	}
}

/* Check whether hardware update of the Access flag is supported */
static inline bool cpu_has_hw_af(void)
{
	u64 mmfr1;

	if (!IS_ENABLED(CONFIG_ARM64_HW_AFDBM))
		return false;

	mmfr1 = read_cpuid(ID_AA64MMFR1_EL1);
	return cpuid_feature_extract_unsigned_field(mmfr1,
						ID_AA64MMFR1_HADBS_SHIFT);
}

static inline bool cpu_has_pan(void)
{
	u64 mmfr1 = read_cpuid(ID_AA64MMFR1_EL1);
	return cpuid_feature_extract_unsigned_field(mmfr1,
						    ID_AA64MMFR1_PAN_SHIFT);
}

#ifdef CONFIG_ARM64_AMU_EXTN
/* Check whether the cpu supports the Activity Monitors Unit (AMU) */
extern bool cpu_has_amu_feat(int cpu);
#else
static inline bool cpu_has_amu_feat(int cpu)
{
	return false;
}
#endif

/* Get a cpu that supports the Activity Monitors Unit (AMU) */
extern int get_cpu_with_amu_feat(void);

static inline unsigned int get_vmid_bits(u64 mmfr1)
{
	int vmid_bits;

	vmid_bits = cpuid_feature_extract_unsigned_field(mmfr1,
						ID_AA64MMFR1_VMIDBITS_SHIFT);
	if (vmid_bits == ID_AA64MMFR1_VMIDBITS_16)
		return 16;

	/*
	 * Return the default here even if any reserved
	 * value is fetched from the system register.
	 */
	return 8;
}

extern struct arm64_ftr_override id_aa64mmfr1_override;
extern struct arm64_ftr_override id_aa64pfr1_override;
extern struct arm64_ftr_override id_aa64isar1_override;

u32 get_kvm_ipa_limit(void);
void dump_cpu_features(void);

#endif /* __ASSEMBLY__ */

#endif
