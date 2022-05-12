/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * tools/testing/selftests/kvm/include/x86_64/processor.h
 *
 * Copyright (C) 2018, Google LLC.
 */

#ifndef SELFTEST_KVM_PROCESSOR_H
#define SELFTEST_KVM_PROCESSOR_H

#include <assert.h>
#include <stdint.h>

#include <asm/msr-index.h>

#define X86_EFLAGS_FIXED	 (1u << 1)

#define X86_CR4_VME		(1ul << 0)
#define X86_CR4_PVI		(1ul << 1)
#define X86_CR4_TSD		(1ul << 2)
#define X86_CR4_DE		(1ul << 3)
#define X86_CR4_PSE		(1ul << 4)
#define X86_CR4_PAE		(1ul << 5)
#define X86_CR4_MCE		(1ul << 6)
#define X86_CR4_PGE		(1ul << 7)
#define X86_CR4_PCE		(1ul << 8)
#define X86_CR4_OSFXSR		(1ul << 9)
#define X86_CR4_OSXMMEXCPT	(1ul << 10)
#define X86_CR4_UMIP		(1ul << 11)
#define X86_CR4_LA57		(1ul << 12)
#define X86_CR4_VMXE		(1ul << 13)
#define X86_CR4_SMXE		(1ul << 14)
#define X86_CR4_FSGSBASE	(1ul << 16)
#define X86_CR4_PCIDE		(1ul << 17)
#define X86_CR4_OSXSAVE		(1ul << 18)
#define X86_CR4_SMEP		(1ul << 20)
#define X86_CR4_SMAP		(1ul << 21)
#define X86_CR4_PKE		(1ul << 22)

/* CPUID.1.ECX */
#define CPUID_VMX		(1ul << 5)
#define CPUID_SMX		(1ul << 6)
#define CPUID_PCID		(1ul << 17)
#define CPUID_XSAVE		(1ul << 26)

/* CPUID.7.EBX */
#define CPUID_FSGSBASE		(1ul << 0)
#define CPUID_SMEP		(1ul << 7)
#define CPUID_SMAP		(1ul << 20)

/* CPUID.7.ECX */
#define CPUID_UMIP		(1ul << 2)
#define CPUID_PKU		(1ul << 3)
#define CPUID_LA57		(1ul << 16)

#define UNEXPECTED_VECTOR_PORT 0xfff0u

/* General Registers in 64-Bit Mode */
struct gpr64_regs {
	u64 rax;
	u64 rcx;
	u64 rdx;
	u64 rbx;
	u64 rsp;
	u64 rbp;
	u64 rsi;
	u64 rdi;
	u64 r8;
	u64 r9;
	u64 r10;
	u64 r11;
	u64 r12;
	u64 r13;
	u64 r14;
	u64 r15;
};

struct desc64 {
	uint16_t limit0;
	uint16_t base0;
	unsigned base1:8, type:4, s:1, dpl:2, p:1;
	unsigned limit1:4, avl:1, l:1, db:1, g:1, base2:8;
	uint32_t base3;
	uint32_t zero1;
} __attribute__((packed));

struct desc_ptr {
	uint16_t size;
	uint64_t address;
} __attribute__((packed));

static inline uint64_t get_desc64_base(const struct desc64 *desc)
{
	return ((uint64_t)desc->base3 << 32) |
		(desc->base0 | ((desc->base1) << 16) | ((desc->base2) << 24));
}

static inline uint64_t rdtsc(void)
{
	uint32_t eax, edx;
	uint64_t tsc_val;
	/*
	 * The lfence is to wait (on Intel CPUs) until all previous
	 * instructions have been executed. If software requires RDTSC to be
	 * executed prior to execution of any subsequent instruction, it can
	 * execute LFENCE immediately after RDTSC
	 */
	__asm__ __volatile__("lfence; rdtsc; lfence" : "=a"(eax), "=d"(edx));
	tsc_val = ((uint64_t)edx) << 32 | eax;
	return tsc_val;
}

static inline uint64_t rdtscp(uint32_t *aux)
{
	uint32_t eax, edx;

	__asm__ __volatile__("rdtscp" : "=a"(eax), "=d"(edx), "=c"(*aux));
	return ((uint64_t)edx) << 32 | eax;
}

static inline uint64_t rdmsr(uint32_t msr)
{
	uint32_t a, d;

	__asm__ __volatile__("rdmsr" : "=a"(a), "=d"(d) : "c"(msr) : "memory");

	return a | ((uint64_t) d << 32);
}

static inline void wrmsr(uint32_t msr, uint64_t value)
{
	uint32_t a = value;
	uint32_t d = value >> 32;

	__asm__ __volatile__("wrmsr" :: "a"(a), "d"(d), "c"(msr) : "memory");
}


static inline uint16_t inw(uint16_t port)
{
	uint16_t tmp;

	__asm__ __volatile__("in %%dx, %%ax"
		: /* output */ "=a" (tmp)
		: /* input */ "d" (port));

	return tmp;
}

static inline uint16_t get_es(void)
{
	uint16_t es;

	__asm__ __volatile__("mov %%es, %[es]"
			     : /* output */ [es]"=rm"(es));
	return es;
}

static inline uint16_t get_cs(void)
{
	uint16_t cs;

	__asm__ __volatile__("mov %%cs, %[cs]"
			     : /* output */ [cs]"=rm"(cs));
	return cs;
}

static inline uint16_t get_ss(void)
{
	uint16_t ss;

	__asm__ __volatile__("mov %%ss, %[ss]"
			     : /* output */ [ss]"=rm"(ss));
	return ss;
}

static inline uint16_t get_ds(void)
{
	uint16_t ds;

	__asm__ __volatile__("mov %%ds, %[ds]"
			     : /* output */ [ds]"=rm"(ds));
	return ds;
}

static inline uint16_t get_fs(void)
{
	uint16_t fs;

	__asm__ __volatile__("mov %%fs, %[fs]"
			     : /* output */ [fs]"=rm"(fs));
	return fs;
}

static inline uint16_t get_gs(void)
{
	uint16_t gs;

	__asm__ __volatile__("mov %%gs, %[gs]"
			     : /* output */ [gs]"=rm"(gs));
	return gs;
}

static inline uint16_t get_tr(void)
{
	uint16_t tr;

	__asm__ __volatile__("str %[tr]"
			     : /* output */ [tr]"=rm"(tr));
	return tr;
}

static inline uint64_t get_cr0(void)
{
	uint64_t cr0;

	__asm__ __volatile__("mov %%cr0, %[cr0]"
			     : /* output */ [cr0]"=r"(cr0));
	return cr0;
}

static inline uint64_t get_cr3(void)
{
	uint64_t cr3;

	__asm__ __volatile__("mov %%cr3, %[cr3]"
			     : /* output */ [cr3]"=r"(cr3));
	return cr3;
}

static inline uint64_t get_cr4(void)
{
	uint64_t cr4;

	__asm__ __volatile__("mov %%cr4, %[cr4]"
			     : /* output */ [cr4]"=r"(cr4));
	return cr4;
}

static inline void set_cr4(uint64_t val)
{
	__asm__ __volatile__("mov %0, %%cr4" : : "r" (val) : "memory");
}

static inline struct desc_ptr get_gdt(void)
{
	struct desc_ptr gdt;
	__asm__ __volatile__("sgdt %[gdt]"
			     : /* output */ [gdt]"=m"(gdt));
	return gdt;
}

static inline struct desc_ptr get_idt(void)
{
	struct desc_ptr idt;
	__asm__ __volatile__("sidt %[idt]"
			     : /* output */ [idt]"=m"(idt));
	return idt;
}

static inline void outl(uint16_t port, uint32_t value)
{
	__asm__ __volatile__("outl %%eax, %%dx" : : "d"(port), "a"(value));
}

static inline void cpuid(uint32_t *eax, uint32_t *ebx,
			 uint32_t *ecx, uint32_t *edx)
{
	/* ecx is often an input as well as an output. */
	asm volatile("cpuid"
	    : "=a" (*eax),
	      "=b" (*ebx),
	      "=c" (*ecx),
	      "=d" (*edx)
	    : "0" (*eax), "2" (*ecx)
	    : "memory");
}

#define SET_XMM(__var, __xmm) \
	asm volatile("movq %0, %%"#__xmm : : "r"(__var) : #__xmm)

static inline void set_xmm(int n, unsigned long val)
{
	switch (n) {
	case 0:
		SET_XMM(val, xmm0);
		break;
	case 1:
		SET_XMM(val, xmm1);
		break;
	case 2:
		SET_XMM(val, xmm2);
		break;
	case 3:
		SET_XMM(val, xmm3);
		break;
	case 4:
		SET_XMM(val, xmm4);
		break;
	case 5:
		SET_XMM(val, xmm5);
		break;
	case 6:
		SET_XMM(val, xmm6);
		break;
	case 7:
		SET_XMM(val, xmm7);
		break;
	}
}

typedef unsigned long v1di __attribute__ ((vector_size (8)));
static inline unsigned long get_xmm(int n)
{
	assert(n >= 0 && n <= 7);

	register v1di xmm0 __asm__("%xmm0");
	register v1di xmm1 __asm__("%xmm1");
	register v1di xmm2 __asm__("%xmm2");
	register v1di xmm3 __asm__("%xmm3");
	register v1di xmm4 __asm__("%xmm4");
	register v1di xmm5 __asm__("%xmm5");
	register v1di xmm6 __asm__("%xmm6");
	register v1di xmm7 __asm__("%xmm7");
	switch (n) {
	case 0:
		return (unsigned long)xmm0;
	case 1:
		return (unsigned long)xmm1;
	case 2:
		return (unsigned long)xmm2;
	case 3:
		return (unsigned long)xmm3;
	case 4:
		return (unsigned long)xmm4;
	case 5:
		return (unsigned long)xmm5;
	case 6:
		return (unsigned long)xmm6;
	case 7:
		return (unsigned long)xmm7;
	}
	return 0;
}

bool is_intel_cpu(void);

struct kvm_x86_state;
struct kvm_x86_state *vcpu_save_state(struct kvm_vm *vm, uint32_t vcpuid);
void vcpu_load_state(struct kvm_vm *vm, uint32_t vcpuid,
		     struct kvm_x86_state *state);

struct kvm_msr_list *kvm_get_msr_index_list(void);
uint64_t kvm_get_feature_msr(uint64_t msr_index);
struct kvm_cpuid2 *kvm_get_supported_cpuid(void);

struct kvm_cpuid2 *vcpu_get_cpuid(struct kvm_vm *vm, uint32_t vcpuid);
void vcpu_set_cpuid(struct kvm_vm *vm, uint32_t vcpuid,
		    struct kvm_cpuid2 *cpuid);

struct kvm_cpuid_entry2 *
kvm_get_supported_cpuid_index(uint32_t function, uint32_t index);

static inline struct kvm_cpuid_entry2 *
kvm_get_supported_cpuid_entry(uint32_t function)
{
	return kvm_get_supported_cpuid_index(function, 0);
}

uint64_t vcpu_get_msr(struct kvm_vm *vm, uint32_t vcpuid, uint64_t msr_index);
int _vcpu_set_msr(struct kvm_vm *vm, uint32_t vcpuid, uint64_t msr_index,
		  uint64_t msr_value);
void vcpu_set_msr(struct kvm_vm *vm, uint32_t vcpuid, uint64_t msr_index,
	  	  uint64_t msr_value);

uint32_t kvm_get_cpuid_max_basic(void);
uint32_t kvm_get_cpuid_max_extended(void);
void kvm_get_cpu_address_width(unsigned int *pa_bits, unsigned int *va_bits);

struct ex_regs {
	uint64_t rax, rcx, rdx, rbx;
	uint64_t rbp, rsi, rdi;
	uint64_t r8, r9, r10, r11;
	uint64_t r12, r13, r14, r15;
	uint64_t vector;
	uint64_t error_code;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
};

void vm_init_descriptor_tables(struct kvm_vm *vm);
void vcpu_init_descriptor_tables(struct kvm_vm *vm, uint32_t vcpuid);
void vm_handle_exception(struct kvm_vm *vm, int vector,
			void (*handler)(struct ex_regs *));

/*
 * set_cpuid() - overwrites a matching cpuid entry with the provided value.
 *		 matches based on ent->function && ent->index. returns true
 *		 if a match was found and successfully overwritten.
 * @cpuid: the kvm cpuid list to modify.
 * @ent: cpuid entry to insert
 */
bool set_cpuid(struct kvm_cpuid2 *cpuid, struct kvm_cpuid_entry2 *ent);

uint64_t kvm_hypercall(uint64_t nr, uint64_t a0, uint64_t a1, uint64_t a2,
		       uint64_t a3);

struct kvm_cpuid2 *kvm_get_supported_hv_cpuid(void);
void vcpu_set_hv_cpuid(struct kvm_vm *vm, uint32_t vcpuid);
struct kvm_cpuid2 *vcpu_get_supported_hv_cpuid(struct kvm_vm *vm, uint32_t vcpuid);

/*
 * Basic CPU control in CR0
 */
#define X86_CR0_PE          (1UL<<0) /* Protection Enable */
#define X86_CR0_MP          (1UL<<1) /* Monitor Coprocessor */
#define X86_CR0_EM          (1UL<<2) /* Emulation */
#define X86_CR0_TS          (1UL<<3) /* Task Switched */
#define X86_CR0_ET          (1UL<<4) /* Extension Type */
#define X86_CR0_NE          (1UL<<5) /* Numeric Error */
#define X86_CR0_WP          (1UL<<16) /* Write Protect */
#define X86_CR0_AM          (1UL<<18) /* Alignment Mask */
#define X86_CR0_NW          (1UL<<29) /* Not Write-through */
#define X86_CR0_CD          (1UL<<30) /* Cache Disable */
#define X86_CR0_PG          (1UL<<31) /* Paging */

#define APIC_DEFAULT_GPA		0xfee00000ULL

/* APIC base address MSR and fields */
#define MSR_IA32_APICBASE		0x0000001b
#define MSR_IA32_APICBASE_BSP		(1<<8)
#define MSR_IA32_APICBASE_EXTD		(1<<10)
#define MSR_IA32_APICBASE_ENABLE	(1<<11)
#define MSR_IA32_APICBASE_BASE		(0xfffff<<12)
#define		GET_APIC_BASE(x)	(((x) >> 12) << 12)

#define APIC_BASE_MSR	0x800
#define X2APIC_ENABLE	(1UL << 10)
#define	APIC_ID		0x20
#define	APIC_LVR	0x30
#define		GET_APIC_ID_FIELD(x)	(((x) >> 24) & 0xFF)
#define	APIC_TASKPRI	0x80
#define	APIC_PROCPRI	0xA0
#define	APIC_EOI	0xB0
#define	APIC_SPIV	0xF0
#define		APIC_SPIV_FOCUS_DISABLED	(1 << 9)
#define		APIC_SPIV_APIC_ENABLED		(1 << 8)
#define	APIC_ICR	0x300
#define		APIC_DEST_SELF		0x40000
#define		APIC_DEST_ALLINC	0x80000
#define		APIC_DEST_ALLBUT	0xC0000
#define		APIC_ICR_RR_MASK	0x30000
#define		APIC_ICR_RR_INVALID	0x00000
#define		APIC_ICR_RR_INPROG	0x10000
#define		APIC_ICR_RR_VALID	0x20000
#define		APIC_INT_LEVELTRIG	0x08000
#define		APIC_INT_ASSERT		0x04000
#define		APIC_ICR_BUSY		0x01000
#define		APIC_DEST_LOGICAL	0x00800
#define		APIC_DEST_PHYSICAL	0x00000
#define		APIC_DM_FIXED		0x00000
#define		APIC_DM_FIXED_MASK	0x00700
#define		APIC_DM_LOWEST		0x00100
#define		APIC_DM_SMI		0x00200
#define		APIC_DM_REMRD		0x00300
#define		APIC_DM_NMI		0x00400
#define		APIC_DM_INIT		0x00500
#define		APIC_DM_STARTUP		0x00600
#define		APIC_DM_EXTINT		0x00700
#define		APIC_VECTOR_MASK	0x000FF
#define	APIC_ICR2	0x310
#define		SET_APIC_DEST_FIELD(x)	((x) << 24)

/* VMX_EPT_VPID_CAP bits */
#define VMX_EPT_VPID_CAP_AD_BITS       (1ULL << 21)

#endif /* SELFTEST_KVM_PROCESSOR_H */
