/*
 * Helper macros to support writing architecture specific
 * linker scripts.
 *
 * A minimal linker scripts has following content:
 * [This is a sample, architectures may have special requiriements]
 *
 * OUTPUT_FORMAT(...)
 * OUTPUT_ARCH(...)
 * ENTRY(...)
 * SECTIONS
 * {
 *	. = START;
 *	__init_begin = .;
 *	HEAD_TEXT_SECTION
 *	INIT_TEXT_SECTION(PAGE_SIZE)
 *	INIT_DATA_SECTION(...)
 *	PERCPU_SECTION(CACHELINE_SIZE)
 *	__init_end = .;
 *
 *	_stext = .;
 *	TEXT_SECTION = 0
 *	_etext = .;
 *
 *      _sdata = .;
 *	RO_DATA(PAGE_SIZE)
 *	RW_DATA(...)
 *	_edata = .;
 *
 *	EXCEPTION_TABLE(...)
 *
 *	BSS_SECTION(0, 0, 0)
 *	_end = .;
 *
 *	STABS_DEBUG
 *	DWARF_DEBUG
 *	ELF_DETAILS
 *
 *	DISCARDS		// must be the last
 * }
 *
 * [__init_begin, __init_end] is the init section that may be freed after init
 * 	// __init_begin and __init_end should be page aligned, so that we can
 *	// free the whole .init memory
 * [_stext, _etext] is the text section
 * [_sdata, _edata] is the data section
 *
 * Some of the included output section have their own set of constants.
 * Examples are: [__initramfs_start, __initramfs_end] for initramfs and
 *               [__nosave_begin, __nosave_end] for the nosave data
 */

#ifndef LOAD_OFFSET
#define LOAD_OFFSET 0
#endif

/*
 * Only some architectures want to have the .notes segment visible in
 * a separate PT_NOTE ELF Program Header. When this happens, it needs
 * to be visible in both the kernel text's PT_LOAD and the PT_NOTE
 * Program Headers. In this case, though, the PT_LOAD needs to be made
 * the default again so that all the following sections don't also end
 * up in the PT_NOTE Program Header.
 */
#ifdef EMITS_PT_NOTE
#define NOTES_HEADERS		:text :note
#define NOTES_HEADERS_RESTORE	__restore_ph : { *(.__restore_ph) } :text
#else
#define NOTES_HEADERS
#define NOTES_HEADERS_RESTORE
#endif

/*
 * Some architectures have non-executable read-only exception tables.
 * They can be added to the RO_DATA segment by specifying their desired
 * alignment.
 */
#ifdef RO_EXCEPTION_TABLE_ALIGN
#define RO_EXCEPTION_TABLE	EXCEPTION_TABLE(RO_EXCEPTION_TABLE_ALIGN)
#else
#define RO_EXCEPTION_TABLE
#endif

/* Align . to a 8 byte boundary equals to maximum function alignment. */
#define ALIGN_FUNCTION()  . = ALIGN(8)

/*
 * LD_DEAD_CODE_DATA_ELIMINATION option enables -fdata-sections, which
 * generates .data.identifier sections, which need to be pulled in with
 * .data. We don't want to pull in .data..other sections, which Linux
 * has defined. Same for text and bss.
 *
 * With LTO_CLANG, the linker also splits sections by default, so we need
 * these macros to combine the sections during the final link.
 *
 * RODATA_MAIN is not used because existing code already defines .rodata.x
 * sections to be brought in with rodata.
 */
#if defined(CONFIG_LD_DEAD_CODE_DATA_ELIMINATION) || defined(CONFIG_LTO_CLANG)
#define TEXT_MAIN .text .text.[0-9a-zA-Z_]*
#define DATA_MAIN .data .data.[0-9a-zA-Z_]* .data..L* .data..compoundliteral* .data.$__unnamed_* .data.$L*
#define SDATA_MAIN .sdata .sdata.[0-9a-zA-Z_]*
#define RODATA_MAIN .rodata .rodata.[0-9a-zA-Z_]* .rodata..L*
#define BSS_MAIN .bss .bss.[0-9a-zA-Z_]* .bss..compoundliteral*
#define SBSS_MAIN .sbss .sbss.[0-9a-zA-Z_]*
#else
#define TEXT_MAIN .text
#define DATA_MAIN .data
#define SDATA_MAIN .sdata
#define RODATA_MAIN .rodata
#define BSS_MAIN .bss
#define SBSS_MAIN .sbss
#endif

/*
 * GCC 4.5 and later have a 32 bytes section alignment for structures.
 * Except GCC 4.9, that feels the need to align on 64 bytes.
 */
#if __GNUC__ == 4 && __GNUC_MINOR__ == 9
#define STRUCT_ALIGNMENT 64
#else
#define STRUCT_ALIGNMENT 32
#endif
#define STRUCT_ALIGN() . = ALIGN(STRUCT_ALIGNMENT)

/*
 * The order of the sched class addresses are important, as they are
 * used to determine the order of the priority of each sched class in
 * relation to each other.
 */
#define SCHED_DATA				\
	STRUCT_ALIGN();				\
	__begin_sched_classes = .;		\
	*(__idle_sched_class)			\
	*(__fair_sched_class)			\
	*(__rt_sched_class)			\
	*(__dl_sched_class)			\
	*(__stop_sched_class)			\
	__end_sched_classes = .;

/* The actual configuration determine if the init/exit sections
 * are handled as text/data or they can be discarded (which
 * often happens at runtime)
 */
#ifdef CONFIG_HOTPLUG_CPU
#define CPU_KEEP(sec)    *(.cpu##sec)
#define CPU_DISCARD(sec)
#else
#define CPU_KEEP(sec)
#define CPU_DISCARD(sec) *(.cpu##sec)
#endif

#if defined(CONFIG_MEMORY_HOTPLUG)
#define MEM_KEEP(sec)    *(.mem##sec)
#define MEM_DISCARD(sec)
#else
#define MEM_KEEP(sec)
#define MEM_DISCARD(sec) *(.mem##sec)
#endif

#ifdef CONFIG_FTRACE_MCOUNT_RECORD
/*
 * The ftrace call sites are logged to a section whose name depends on the
 * compiler option used. A given kernel image will only use one, AKA
 * FTRACE_CALLSITE_SECTION. We capture all of them here to avoid header
 * dependencies for FTRACE_CALLSITE_SECTION's definition.
 *
 * Need to also make ftrace_stub_graph point to ftrace_stub
 * so that the same stub location may have different protocols
 * and not mess up with C verifiers.
 */
#define MCOUNT_REC()	. = ALIGN(8);				\
			__start_mcount_loc = .;			\
			KEEP(*(__mcount_loc))			\
			KEEP(*(__patchable_function_entries))	\
			__stop_mcount_loc = .;			\
			ftrace_stub_graph = ftrace_stub;
#else
# ifdef CONFIG_FUNCTION_TRACER
#  define MCOUNT_REC()	ftrace_stub_graph = ftrace_stub;
# else
#  define MCOUNT_REC()
# endif
#endif

#ifdef CONFIG_TRACE_BRANCH_PROFILING
#define LIKELY_PROFILE()	__start_annotated_branch_profile = .;	\
				KEEP(*(_ftrace_annotated_branch))	\
				__stop_annotated_branch_profile = .;
#else
#define LIKELY_PROFILE()
#endif

#ifdef CONFIG_PROFILE_ALL_BRANCHES
#define BRANCH_PROFILE()	__start_branch_profile = .;		\
				KEEP(*(_ftrace_branch))			\
				__stop_branch_profile = .;
#else
#define BRANCH_PROFILE()
#endif

#ifdef CONFIG_KPROBES
#define KPROBE_BLACKLIST()	. = ALIGN(8);				      \
				__start_kprobe_blacklist = .;		      \
				KEEP(*(_kprobe_blacklist))		      \
				__stop_kprobe_blacklist = .;
#else
#define KPROBE_BLACKLIST()
#endif

#ifdef CONFIG_FUNCTION_ERROR_INJECTION
#define ERROR_INJECT_WHITELIST()	STRUCT_ALIGN();			      \
			__start_error_injection_whitelist = .;		      \
			KEEP(*(_error_injection_whitelist))		      \
			__stop_error_injection_whitelist = .;
#else
#define ERROR_INJECT_WHITELIST()
#endif

#ifdef CONFIG_EVENT_TRACING
#define FTRACE_EVENTS()	. = ALIGN(8);					\
			__start_ftrace_events = .;			\
			KEEP(*(_ftrace_events))				\
			__stop_ftrace_events = .;			\
			__start_ftrace_eval_maps = .;			\
			KEEP(*(_ftrace_eval_map))			\
			__stop_ftrace_eval_maps = .;
#else
#define FTRACE_EVENTS()
#endif

#ifdef CONFIG_TRACING
#define TRACE_PRINTKS()	 __start___trace_bprintk_fmt = .;      \
			 KEEP(*(__trace_printk_fmt)) /* Trace_printk fmt' pointer */ \
			 __stop___trace_bprintk_fmt = .;
#define TRACEPOINT_STR() __start___tracepoint_str = .;	\
			 KEEP(*(__tracepoint_str)) /* Trace_printk fmt' pointer */ \
			 __stop___tracepoint_str = .;
#else
#define TRACE_PRINTKS()
#define TRACEPOINT_STR()
#endif

#ifdef CONFIG_FTRACE_SYSCALLS
#define TRACE_SYSCALLS() . = ALIGN(8);					\
			 __start_syscalls_metadata = .;			\
			 KEEP(*(__syscalls_metadata))			\
			 __stop_syscalls_metadata = .;
#else
#define TRACE_SYSCALLS()
#endif

#ifdef CONFIG_BPF_EVENTS
#define BPF_RAW_TP() STRUCT_ALIGN();					\
			 __start__bpf_raw_tp = .;			\
			 KEEP(*(__bpf_raw_tp_map))			\
			 __stop__bpf_raw_tp = .;
#else
#define BPF_RAW_TP()
#endif

#ifdef CONFIG_SERIAL_EARLYCON
#define EARLYCON_TABLE() . = ALIGN(8);				\
			 __earlycon_table = .;			\
			 KEEP(*(__earlycon_table))		\
			 __earlycon_table_end = .;
#else
#define EARLYCON_TABLE()
#endif

#ifdef CONFIG_SECURITY
#define LSM_TABLE()	. = ALIGN(8);					\
			__start_lsm_info = .;				\
			KEEP(*(.lsm_info.init))				\
			__end_lsm_info = .;
#define EARLY_LSM_TABLE()	. = ALIGN(8);				\
			__start_early_lsm_info = .;			\
			KEEP(*(.early_lsm_info.init))			\
			__end_early_lsm_info = .;
#else
#define LSM_TABLE()
#define EARLY_LSM_TABLE()
#endif

#define ___OF_TABLE(cfg, name)	_OF_TABLE_##cfg(name)
#define __OF_TABLE(cfg, name)	___OF_TABLE(cfg, name)
#define OF_TABLE(cfg, name)	__OF_TABLE(IS_ENABLED(cfg), name)
#define _OF_TABLE_0(name)
#define _OF_TABLE_1(name)						\
	. = ALIGN(8);							\
	__##name##_of_table = .;					\
	KEEP(*(__##name##_of_table))					\
	KEEP(*(__##name##_of_table_end))

#define TIMER_OF_TABLES()	OF_TABLE(CONFIG_TIMER_OF, timer)
#define IRQCHIP_OF_MATCH_TABLE() OF_TABLE(CONFIG_IRQCHIP, irqchip)
#define CLK_OF_TABLES()		OF_TABLE(CONFIG_COMMON_CLK, clk)
#define RESERVEDMEM_OF_TABLES()	OF_TABLE(CONFIG_OF_RESERVED_MEM, reservedmem)
#define CPU_METHOD_OF_TABLES()	OF_TABLE(CONFIG_SMP, cpu_method)
#define CPUIDLE_METHOD_OF_TABLES() OF_TABLE(CONFIG_CPU_IDLE, cpuidle_method)

#ifdef CONFIG_ACPI
#define ACPI_PROBE_TABLE(name)						\
	. = ALIGN(8);							\
	__##name##_acpi_probe_table = .;				\
	KEEP(*(__##name##_acpi_probe_table))				\
	__##name##_acpi_probe_table_end = .;
#else
#define ACPI_PROBE_TABLE(name)
#endif

#ifdef CONFIG_THERMAL
#define THERMAL_TABLE(name)						\
	. = ALIGN(8);							\
	__##name##_thermal_table = .;					\
	KEEP(*(__##name##_thermal_table))				\
	__##name##_thermal_table_end = .;
#else
#define THERMAL_TABLE(name)
#endif

#ifdef CONFIG_DTPM
#define DTPM_TABLE()							\
	. = ALIGN(8);							\
	__dtpm_table = .;						\
	KEEP(*(__dtpm_table))						\
	__dtpm_table_end = .;
#else
#define DTPM_TABLE()
#endif

#define KERNEL_DTB()							\
	STRUCT_ALIGN();							\
	__dtb_start = .;						\
	KEEP(*(.dtb.init.rodata))					\
	__dtb_end = .;

/*
 * .data section
 */
#define DATA_DATA							\
	*(.xiptext)							\
	*(DATA_MAIN)							\
	*(.ref.data)							\
	*(.data..shared_aligned) /* percpu related */			\
	MEM_KEEP(init.data*)						\
	MEM_KEEP(exit.data*)						\
	*(.data.unlikely)						\
	__start_once = .;						\
	*(.data.once)							\
	__end_once = .;							\
	STRUCT_ALIGN();							\
	*(__tracepoints)						\
	/* implement dynamic printk debug */				\
	. = ALIGN(8);							\
	__start___dyndbg = .;						\
	KEEP(*(__dyndbg))						\
	__stop___dyndbg = .;						\
	LIKELY_PROFILE()		       				\
	BRANCH_PROFILE()						\
	TRACE_PRINTKS()							\
	BPF_RAW_TP()							\
	TRACEPOINT_STR()

/*
 * Data section helpers
 */
#define NOSAVE_DATA							\
	. = ALIGN(PAGE_SIZE);						\
	__nosave_begin = .;						\
	*(.data..nosave)						\
	. = ALIGN(PAGE_SIZE);						\
	__nosave_end = .;

#define PAGE_ALIGNED_DATA(page_align)					\
	. = ALIGN(page_align);						\
	*(.data..page_aligned)						\
	. = ALIGN(page_align);

#define READ_MOSTLY_DATA(align)						\
	. = ALIGN(align);						\
	*(.data..read_mostly)						\
	. = ALIGN(align);

#define CACHELINE_ALIGNED_DATA(align)					\
	. = ALIGN(align);						\
	*(.data..cacheline_aligned)

#define INIT_TASK_DATA(align)						\
	. = ALIGN(align);						\
	__start_init_task = .;						\
	init_thread_union = .;						\
	init_stack = .;							\
	KEEP(*(.data..init_task))					\
	KEEP(*(.data..init_thread_info))				\
	. = __start_init_task + THREAD_SIZE;				\
	__end_init_task = .;

#define JUMP_TABLE_DATA							\
	. = ALIGN(8);							\
	__start___jump_table = .;					\
	KEEP(*(__jump_table))						\
	__stop___jump_table = .;

#define STATIC_CALL_DATA						\
	. = ALIGN(8);							\
	__start_static_call_sites = .;					\
	KEEP(*(.static_call_sites))					\
	__stop_static_call_sites = .;					\
	__start_static_call_tramp_key = .;				\
	KEEP(*(.static_call_tramp_key))					\
	__stop_static_call_tramp_key = .;

/*
 * Allow architectures to handle ro_after_init data on their
 * own by defining an empty RO_AFTER_INIT_DATA.
 */
#ifndef RO_AFTER_INIT_DATA
#define RO_AFTER_INIT_DATA						\
	. = ALIGN(8);							\
	__start_ro_after_init = .;					\
	*(.data..ro_after_init)						\
	JUMP_TABLE_DATA							\
	STATIC_CALL_DATA						\
	__end_ro_after_init = .;
#endif

/*
 * Read only Data
 */
#define RO_DATA(align)							\
	. = ALIGN((align));						\
	.rodata           : AT(ADDR(.rodata) - LOAD_OFFSET) {		\
		__start_rodata = .;					\
		*(.rodata) *(.rodata.*)					\
		SCHED_DATA						\
		RO_AFTER_INIT_DATA	/* Read only after init */	\
		. = ALIGN(8);						\
		__start___tracepoints_ptrs = .;				\
		KEEP(*(__tracepoints_ptrs)) /* Tracepoints: pointer array */ \
		__stop___tracepoints_ptrs = .;				\
		*(__tracepoints_strings)/* Tracepoints: strings */	\
	}								\
									\
	.rodata1          : AT(ADDR(.rodata1) - LOAD_OFFSET) {		\
		*(.rodata1)						\
	}								\
									\
	/* PCI quirks */						\
	.pci_fixup        : AT(ADDR(.pci_fixup) - LOAD_OFFSET) {	\
		__start_pci_fixups_early = .;				\
		KEEP(*(.pci_fixup_early))				\
		__end_pci_fixups_early = .;				\
		__start_pci_fixups_header = .;				\
		KEEP(*(.pci_fixup_header))				\
		__end_pci_fixups_header = .;				\
		__start_pci_fixups_final = .;				\
		KEEP(*(.pci_fixup_final))				\
		__end_pci_fixups_final = .;				\
		__start_pci_fixups_enable = .;				\
		KEEP(*(.pci_fixup_enable))				\
		__end_pci_fixups_enable = .;				\
		__start_pci_fixups_resume = .;				\
		KEEP(*(.pci_fixup_resume))				\
		__end_pci_fixups_resume = .;				\
		__start_pci_fixups_resume_early = .;			\
		KEEP(*(.pci_fixup_resume_early))			\
		__end_pci_fixups_resume_early = .;			\
		__start_pci_fixups_suspend = .;				\
		KEEP(*(.pci_fixup_suspend))				\
		__end_pci_fixups_suspend = .;				\
		__start_pci_fixups_suspend_late = .;			\
		KEEP(*(.pci_fixup_suspend_late))			\
		__end_pci_fixups_suspend_late = .;			\
	}								\
									\
	/* Built-in firmware blobs */					\
	.builtin_fw : AT(ADDR(.builtin_fw) - LOAD_OFFSET) ALIGN(8) {	\
		__start_builtin_fw = .;					\
		KEEP(*(.builtin_fw))					\
		__end_builtin_fw = .;					\
	}								\
									\
	TRACEDATA							\
									\
	/* Kernel symbol table: Normal symbols */			\
	__ksymtab         : AT(ADDR(__ksymtab) - LOAD_OFFSET) {		\
		__start___ksymtab = .;					\
		KEEP(*(SORT(___ksymtab+*)))				\
		__stop___ksymtab = .;					\
	}								\
									\
	/* Kernel symbol table: GPL-only symbols */			\
	__ksymtab_gpl     : AT(ADDR(__ksymtab_gpl) - LOAD_OFFSET) {	\
		__start___ksymtab_gpl = .;				\
		KEEP(*(SORT(___ksymtab_gpl+*)))				\
		__stop___ksymtab_gpl = .;				\
	}								\
									\
	/* Kernel symbol table: Normal symbols */			\
	__kcrctab         : AT(ADDR(__kcrctab) - LOAD_OFFSET) {		\
		__start___kcrctab = .;					\
		KEEP(*(SORT(___kcrctab+*)))				\
		__stop___kcrctab = .;					\
	}								\
									\
	/* Kernel symbol table: GPL-only symbols */			\
	__kcrctab_gpl     : AT(ADDR(__kcrctab_gpl) - LOAD_OFFSET) {	\
		__start___kcrctab_gpl = .;				\
		KEEP(*(SORT(___kcrctab_gpl+*)))				\
		__stop___kcrctab_gpl = .;				\
	}								\
									\
	/* Kernel symbol table: strings */				\
        __ksymtab_strings : AT(ADDR(__ksymtab_strings) - LOAD_OFFSET) {	\
		*(__ksymtab_strings)					\
	}								\
									\
	/* __*init sections */						\
	__init_rodata : AT(ADDR(__init_rodata) - LOAD_OFFSET) {		\
		*(.ref.rodata)						\
		MEM_KEEP(init.rodata)					\
		MEM_KEEP(exit.rodata)					\
	}								\
									\
	/* Built-in module parameters. */				\
	__param : AT(ADDR(__param) - LOAD_OFFSET) {			\
		__start___param = .;					\
		KEEP(*(__param))					\
		__stop___param = .;					\
	}								\
									\
	/* Built-in module versions. */					\
	__modver : AT(ADDR(__modver) - LOAD_OFFSET) {			\
		__start___modver = .;					\
		KEEP(*(__modver))					\
		__stop___modver = .;					\
	}								\
									\
	RO_EXCEPTION_TABLE						\
	NOTES								\
	BTF								\
									\
	. = ALIGN((align));						\
	__end_rodata = .;


/*
 * .text..L.cfi.jumptable.* contain Control-Flow Integrity (CFI)
 * jump table entries.
 */
#ifdef CONFIG_CFI_CLANG
#define TEXT_CFI_JT							\
		. = ALIGN(PMD_SIZE);					\
		__cfi_jt_start = .;					\
		*(.text..L.cfi.jumptable .text..L.cfi.jumptable.*)	\
		. = ALIGN(PMD_SIZE);					\
		__cfi_jt_end = .;
#else
#define TEXT_CFI_JT
#endif

/*
 * Non-instrumentable text section
 */
#define NOINSTR_TEXT							\
		ALIGN_FUNCTION();					\
		__noinstr_text_start = .;				\
		*(.noinstr.text)					\
		__noinstr_text_end = .;

/*
 * .text section. Map to function alignment to avoid address changes
 * during second ld run in second ld pass when generating System.map
 *
 * TEXT_MAIN here will match .text.fixup and .text.unlikely if dead
 * code elimination is enabled, so these sections should be converted
 * to use ".." first.
 */
#define TEXT_TEXT							\
		ALIGN_FUNCTION();					\
		*(.text.hot .text.hot.*)				\
		*(TEXT_MAIN .text.fixup)				\
		*(.text.unlikely .text.unlikely.*)			\
		*(.text.unknown .text.unknown.*)			\
		NOINSTR_TEXT						\
		*(.text..refcount)					\
		*(.ref.text)						\
		TEXT_CFI_JT						\
	MEM_KEEP(init.text*)						\
	MEM_KEEP(exit.text*)						\


/* sched.text is aling to function alignment to secure we have same
 * address even at second ld pass when generating System.map */
#define SCHED_TEXT							\
		ALIGN_FUNCTION();					\
		__sched_text_start = .;					\
		*(.sched.text)						\
		__sched_text_end = .;

/* spinlock.text is aling to function alignment to secure we have same
 * address even at second ld pass when generating System.map */
#define LOCK_TEXT							\
		ALIGN_FUNCTION();					\
		__lock_text_start = .;					\
		*(.spinlock.text)					\
		__lock_text_end = .;

#define CPUIDLE_TEXT							\
		ALIGN_FUNCTION();					\
		__cpuidle_text_start = .;				\
		*(.cpuidle.text)					\
		__cpuidle_text_end = .;

#define KPROBES_TEXT							\
		ALIGN_FUNCTION();					\
		__kprobes_text_start = .;				\
		*(.kprobes.text)					\
		__kprobes_text_end = .;

#define ENTRY_TEXT							\
		ALIGN_FUNCTION();					\
		__entry_text_start = .;					\
		*(.entry.text)						\
		__entry_text_end = .;

#define IRQENTRY_TEXT							\
		ALIGN_FUNCTION();					\
		__irqentry_text_start = .;				\
		*(.irqentry.text)					\
		__irqentry_text_end = .;

#define SOFTIRQENTRY_TEXT						\
		ALIGN_FUNCTION();					\
		__softirqentry_text_start = .;				\
		*(.softirqentry.text)					\
		__softirqentry_text_end = .;

#define STATIC_CALL_TEXT						\
		ALIGN_FUNCTION();					\
		__static_call_text_start = .;				\
		*(.static_call.text)					\
		__static_call_text_end = .;

/* Section used for early init (in .S files) */
#define HEAD_TEXT  KEEP(*(.head.text))

#define HEAD_TEXT_SECTION							\
	.head.text : AT(ADDR(.head.text) - LOAD_OFFSET) {		\
		HEAD_TEXT						\
	}

/*
 * Exception table
 */
#define EXCEPTION_TABLE(align)						\
	. = ALIGN(align);						\
	__ex_table : AT(ADDR(__ex_table) - LOAD_OFFSET) {		\
		__start___ex_table = .;					\
		KEEP(*(__ex_table))					\
		__stop___ex_table = .;					\
	}

/*
 * .BTF
 */
#ifdef CONFIG_DEBUG_INFO_BTF
#define BTF								\
	.BTF : AT(ADDR(.BTF) - LOAD_OFFSET) {				\
		__start_BTF = .;					\
		KEEP(*(.BTF))						\
		__stop_BTF = .;						\
	}								\
	. = ALIGN(4);							\
	.BTF_ids : AT(ADDR(.BTF_ids) - LOAD_OFFSET) {			\
		*(.BTF_ids)						\
	}
#else
#define BTF
#endif

/*
 * Init task
 */
#define INIT_TASK_DATA_SECTION(align)					\
	. = ALIGN(align);						\
	.data..init_task :  AT(ADDR(.data..init_task) - LOAD_OFFSET) {	\
		INIT_TASK_DATA(align)					\
	}

#ifdef CONFIG_CONSTRUCTORS
#define KERNEL_CTORS()	. = ALIGN(8);			   \
			__ctors_start = .;		   \
			KEEP(*(SORT(.ctors.*)))		   \
			KEEP(*(.ctors))			   \
			KEEP(*(SORT(.init_array.*)))	   \
			KEEP(*(.init_array))		   \
			__ctors_end = .;
#else
#define KERNEL_CTORS()
#endif

/* init and exit section handling */
#define INIT_DATA							\
	KEEP(*(SORT(___kentry+*)))					\
	*(.init.data init.data.*)					\
	MEM_DISCARD(init.data*)						\
	KERNEL_CTORS()							\
	MCOUNT_REC()							\
	*(.init.rodata .init.rodata.*)					\
	FTRACE_EVENTS()							\
	TRACE_SYSCALLS()						\
	KPROBE_BLACKLIST()						\
	ERROR_INJECT_WHITELIST()					\
	MEM_DISCARD(init.rodata)					\
	CLK_OF_TABLES()							\
	RESERVEDMEM_OF_TABLES()						\
	TIMER_OF_TABLES()						\
	CPU_METHOD_OF_TABLES()						\
	CPUIDLE_METHOD_OF_TABLES()					\
	KERNEL_DTB()							\
	IRQCHIP_OF_MATCH_TABLE()					\
	ACPI_PROBE_TABLE(irqchip)					\
	ACPI_PROBE_TABLE(timer)						\
	THERMAL_TABLE(governor)						\
	DTPM_TABLE()							\
	EARLYCON_TABLE()						\
	LSM_TABLE()							\
	EARLY_LSM_TABLE()						\
	KUNIT_TABLE()

#define INIT_TEXT							\
	*(.init.text .init.text.*)					\
	*(.text.startup)						\
	MEM_DISCARD(init.text*)

#define EXIT_DATA							\
	*(.exit.data .exit.data.*)					\
	*(.fini_array .fini_array.*)					\
	*(.dtors .dtors.*)						\
	MEM_DISCARD(exit.data*)						\
	MEM_DISCARD(exit.rodata*)

#define EXIT_TEXT							\
	*(.exit.text)							\
	*(.text.exit)							\
	MEM_DISCARD(exit.text)

#define EXIT_CALL							\
	*(.exitcall.exit)

/*
 * bss (Block Started by Symbol) - uninitialized data
 * zeroed during startup
 */
#define SBSS(sbss_align)						\
	. = ALIGN(sbss_align);						\
	.sbss : AT(ADDR(.sbss) - LOAD_OFFSET) {				\
		*(.dynsbss)						\
		*(SBSS_MAIN)						\
		*(.scommon)						\
	}

/*
 * Allow archectures to redefine BSS_FIRST_SECTIONS to add extra
 * sections to the front of bss.
 */
#ifndef BSS_FIRST_SECTIONS
#define BSS_FIRST_SECTIONS
#endif

#define BSS(bss_align)							\
	. = ALIGN(bss_align);						\
	.bss : AT(ADDR(.bss) - LOAD_OFFSET) {				\
		BSS_FIRST_SECTIONS					\
		. = ALIGN(PAGE_SIZE);					\
		*(.bss..page_aligned)					\
		. = ALIGN(PAGE_SIZE);					\
		*(.dynbss)						\
		*(BSS_MAIN)						\
		*(COMMON)						\
	}

/*
 * DWARF debug sections.
 * Symbols in the DWARF debugging sections are relative to
 * the beginning of the section so we begin them at 0.
 */
#define DWARF_DEBUG							\
		/* DWARF 1 */						\
		.debug          0 : { *(.debug) }			\
		.line           0 : { *(.line) }			\
		/* GNU DWARF 1 extensions */				\
		.debug_srcinfo  0 : { *(.debug_srcinfo) }		\
		.debug_sfnames  0 : { *(.debug_sfnames) }		\
		/* DWARF 1.1 and DWARF 2 */				\
		.debug_aranges  0 : { *(.debug_aranges) }		\
		.debug_pubnames 0 : { *(.debug_pubnames) }		\
		/* DWARF 2 */						\
		.debug_info     0 : { *(.debug_info			\
				.gnu.linkonce.wi.*) }			\
		.debug_abbrev   0 : { *(.debug_abbrev) }		\
		.debug_line     0 : { *(.debug_line) }			\
		.debug_frame    0 : { *(.debug_frame) }			\
		.debug_str      0 : { *(.debug_str) }			\
		.debug_loc      0 : { *(.debug_loc) }			\
		.debug_macinfo  0 : { *(.debug_macinfo) }		\
		.debug_pubtypes 0 : { *(.debug_pubtypes) }		\
		/* DWARF 3 */						\
		.debug_ranges	0 : { *(.debug_ranges) }		\
		/* SGI/MIPS DWARF 2 extensions */			\
		.debug_weaknames 0 : { *(.debug_weaknames) }		\
		.debug_funcnames 0 : { *(.debug_funcnames) }		\
		.debug_typenames 0 : { *(.debug_typenames) }		\
		.debug_varnames  0 : { *(.debug_varnames) }		\
		/* GNU DWARF 2 extensions */				\
		.debug_gnu_pubnames 0 : { *(.debug_gnu_pubnames) }	\
		.debug_gnu_pubtypes 0 : { *(.debug_gnu_pubtypes) }	\
		/* DWARF 4 */						\
		.debug_types	0 : { *(.debug_types) }			\
		/* DWARF 5 */						\
		.debug_addr	0 : { *(.debug_addr) }			\
		.debug_line_str	0 : { *(.debug_line_str) }		\
		.debug_loclists	0 : { *(.debug_loclists) }		\
		.debug_macro	0 : { *(.debug_macro) }			\
		.debug_names	0 : { *(.debug_names) }			\
		.debug_rnglists	0 : { *(.debug_rnglists) }		\
		.debug_str_offsets	0 : { *(.debug_str_offsets) }

/* Stabs debugging sections. */
#define STABS_DEBUG							\
		.stab 0 : { *(.stab) }					\
		.stabstr 0 : { *(.stabstr) }				\
		.stab.excl 0 : { *(.stab.excl) }			\
		.stab.exclstr 0 : { *(.stab.exclstr) }			\
		.stab.index 0 : { *(.stab.index) }			\
		.stab.indexstr 0 : { *(.stab.indexstr) }

/* Required sections not related to debugging. */
#define ELF_DETAILS							\
		.comment 0 : { *(.comment) }				\
		.symtab 0 : { *(.symtab) }				\
		.strtab 0 : { *(.strtab) }				\
		.shstrtab 0 : { *(.shstrtab) }

#ifdef CONFIG_GENERIC_BUG
#define BUG_TABLE							\
	. = ALIGN(8);							\
	__bug_table : AT(ADDR(__bug_table) - LOAD_OFFSET) {		\
		__start___bug_table = .;				\
		KEEP(*(__bug_table))					\
		__stop___bug_table = .;					\
	}
#else
#define BUG_TABLE
#endif

#ifdef CONFIG_UNWINDER_ORC
#define ORC_UNWIND_TABLE						\
	. = ALIGN(4);							\
	.orc_unwind_ip : AT(ADDR(.orc_unwind_ip) - LOAD_OFFSET) {	\
		__start_orc_unwind_ip = .;				\
		KEEP(*(.orc_unwind_ip))					\
		__stop_orc_unwind_ip = .;				\
	}								\
	. = ALIGN(2);							\
	.orc_unwind : AT(ADDR(.orc_unwind) - LOAD_OFFSET) {		\
		__start_orc_unwind = .;					\
		KEEP(*(.orc_unwind))					\
		__stop_orc_unwind = .;					\
	}								\
	. = ALIGN(4);							\
	.orc_lookup : AT(ADDR(.orc_lookup) - LOAD_OFFSET) {		\
		orc_lookup = .;						\
		. += (((SIZEOF(.text) + LOOKUP_BLOCK_SIZE - 1) /	\
			LOOKUP_BLOCK_SIZE) + 1) * 4;			\
		orc_lookup_end = .;					\
	}
#else
#define ORC_UNWIND_TABLE
#endif

#ifdef CONFIG_PM_TRACE
#define TRACEDATA							\
	. = ALIGN(4);							\
	.tracedata : AT(ADDR(.tracedata) - LOAD_OFFSET) {		\
		__tracedata_start = .;					\
		KEEP(*(.tracedata))					\
		__tracedata_end = .;					\
	}
#else
#define TRACEDATA
#endif

#define NOTES								\
	.notes : AT(ADDR(.notes) - LOAD_OFFSET) {			\
		__start_notes = .;					\
		KEEP(*(.note.*))					\
		__stop_notes = .;					\
	} NOTES_HEADERS							\
	NOTES_HEADERS_RESTORE

#define INIT_SETUP(initsetup_align)					\
		. = ALIGN(initsetup_align);				\
		__setup_start = .;					\
		KEEP(*(.init.setup))					\
		__setup_end = .;

#define INIT_CALLS_LEVEL(level)						\
		__initcall##level##_start = .;				\
		KEEP(*(.initcall##level##.init))			\
		KEEP(*(.initcall##level##s.init))			\

#define INIT_CALLS							\
		__initcall_start = .;					\
		KEEP(*(.initcallearly.init))				\
		INIT_CALLS_LEVEL(0)					\
		INIT_CALLS_LEVEL(1)					\
		INIT_CALLS_LEVEL(2)					\
		INIT_CALLS_LEVEL(3)					\
		INIT_CALLS_LEVEL(4)					\
		INIT_CALLS_LEVEL(5)					\
		INIT_CALLS_LEVEL(rootfs)				\
		INIT_CALLS_LEVEL(6)					\
		INIT_CALLS_LEVEL(7)					\
		__initcall_end = .;

#define CON_INITCALL							\
		__con_initcall_start = .;				\
		KEEP(*(.con_initcall.init))				\
		__con_initcall_end = .;

/* Alignment must be consistent with (kunit_suite *) in include/kunit/test.h */
#define KUNIT_TABLE()							\
		. = ALIGN(8);						\
		__kunit_suites_start = .;				\
		KEEP(*(.kunit_test_suites))				\
		__kunit_suites_end = .;

#ifdef CONFIG_BLK_DEV_INITRD
#define INIT_RAM_FS							\
	. = ALIGN(4);							\
	__initramfs_start = .;						\
	KEEP(*(.init.ramfs))						\
	. = ALIGN(8);							\
	KEEP(*(.init.ramfs.info))
#else
#define INIT_RAM_FS
#endif

/*
 * Memory encryption operates on a page basis. Since we need to clear
 * the memory encryption mask for this section, it needs to be aligned
 * on a page boundary and be a page-size multiple in length.
 *
 * Note: We use a separate section so that only this section gets
 * decrypted to avoid exposing more than we wish.
 */
#ifdef CONFIG_AMD_MEM_ENCRYPT
#define PERCPU_DECRYPTED_SECTION					\
	. = ALIGN(PAGE_SIZE);						\
	*(.data..decrypted)						\
	*(.data..percpu..decrypted)					\
	. = ALIGN(PAGE_SIZE);
#else
#define PERCPU_DECRYPTED_SECTION
#endif


/*
 * Default discarded sections.
 *
 * Some archs want to discard exit text/data at runtime rather than
 * link time due to cross-section references such as alt instructions,
 * bug table, eh_frame, etc.  DISCARDS must be the last of output
 * section definitions so that such archs put those in earlier section
 * definitions.
 */
#ifdef RUNTIME_DISCARD_EXIT
#define EXIT_DISCARDS
#else
#define EXIT_DISCARDS							\
	EXIT_TEXT							\
	EXIT_DATA
#endif

/*
 * Clang's -fprofile-arcs, -fsanitize=kernel-address, and
 * -fsanitize=thread produce unwanted sections (.eh_frame
 * and .init_array.*), but CONFIG_CONSTRUCTORS wants to
 * keep any .init_array.* sections.
 * https://bugs.llvm.org/show_bug.cgi?id=46478
 */
#if defined(CONFIG_GCOV_KERNEL) || defined(CONFIG_KASAN_GENERIC) || defined(CONFIG_KCSAN) || \
	defined(CONFIG_CFI_CLANG)
# ifdef CONFIG_CONSTRUCTORS
#  define SANITIZER_DISCARDS						\
	*(.eh_frame)
# else
#  define SANITIZER_DISCARDS						\
	*(.init_array) *(.init_array.*)					\
	*(.eh_frame)
# endif
#else
# define SANITIZER_DISCARDS
#endif

#define COMMON_DISCARDS							\
	SANITIZER_DISCARDS						\
	*(.discard)							\
	*(.discard.*)							\
	*(.modinfo)							\
	/* ld.bfd warns about .gnu.version* even when not emitted */	\
	*(.gnu.version*)						\

#define DISCARDS							\
	/DISCARD/ : {							\
	EXIT_DISCARDS							\
	EXIT_CALL							\
	COMMON_DISCARDS							\
	}

/**
 * PERCPU_INPUT - the percpu input sections
 * @cacheline: cacheline size
 *
 * The core percpu section names and core symbols which do not rely
 * directly upon load addresses.
 *
 * @cacheline is used to align subsections to avoid false cacheline
 * sharing between subsections for different purposes.
 */
#define PERCPU_INPUT(cacheline)						\
	__per_cpu_start = .;						\
	*(.data..percpu..first)						\
	. = ALIGN(PAGE_SIZE);						\
	*(.data..percpu..page_aligned)					\
	. = ALIGN(cacheline);						\
	*(.data..percpu..read_mostly)					\
	. = ALIGN(cacheline);						\
	*(.data..percpu)						\
	*(.data..percpu..shared_aligned)				\
	PERCPU_DECRYPTED_SECTION					\
	__per_cpu_end = .;

/**
 * PERCPU_VADDR - define output section for percpu area
 * @cacheline: cacheline size
 * @vaddr: explicit base address (optional)
 * @phdr: destination PHDR (optional)
 *
 * Macro which expands to output section for percpu area.
 *
 * @cacheline is used to align subsections to avoid false cacheline
 * sharing between subsections for different purposes.
 *
 * If @vaddr is not blank, it specifies explicit base address and all
 * percpu symbols will be offset from the given address.  If blank,
 * @vaddr always equals @laddr + LOAD_OFFSET.
 *
 * @phdr defines the output PHDR to use if not blank.  Be warned that
 * output PHDR is sticky.  If @phdr is specified, the next output
 * section in the linker script will go there too.  @phdr should have
 * a leading colon.
 *
 * Note that this macros defines __per_cpu_load as an absolute symbol.
 * If there is no need to put the percpu section at a predetermined
 * address, use PERCPU_SECTION.
 */
#define PERCPU_VADDR(cacheline, vaddr, phdr)				\
	__per_cpu_load = .;						\
	.data..percpu vaddr : AT(__per_cpu_load - LOAD_OFFSET) {	\
		PERCPU_INPUT(cacheline)					\
	} phdr								\
	. = __per_cpu_load + SIZEOF(.data..percpu);

/**
 * PERCPU_SECTION - define output section for percpu area, simple version
 * @cacheline: cacheline size
 *
 * Align to PAGE_SIZE and outputs output section for percpu area.  This
 * macro doesn't manipulate @vaddr or @phdr and __per_cpu_load and
 * __per_cpu_start will be identical.
 *
 * This macro is equivalent to ALIGN(PAGE_SIZE); PERCPU_VADDR(@cacheline,,)
 * except that __per_cpu_load is defined as a relative symbol against
 * .data..percpu which is required for relocatable x86_32 configuration.
 */
#define PERCPU_SECTION(cacheline)					\
	. = ALIGN(PAGE_SIZE);						\
	.data..percpu	: AT(ADDR(.data..percpu) - LOAD_OFFSET) {	\
		__per_cpu_load = .;					\
		PERCPU_INPUT(cacheline)					\
	}


/*
 * Definition of the high level *_SECTION macros
 * They will fit only a subset of the architectures
 */


/*
 * Writeable data.
 * All sections are combined in a single .data section.
 * The sections following CONSTRUCTORS are arranged so their
 * typical alignment matches.
 * A cacheline is typical/always less than a PAGE_SIZE so
 * the sections that has this restriction (or similar)
 * is located before the ones requiring PAGE_SIZE alignment.
 * NOSAVE_DATA starts and ends with a PAGE_SIZE alignment which
 * matches the requirement of PAGE_ALIGNED_DATA.
 *
 * use 0 as page_align if page_aligned data is not used */
#define RW_DATA(cacheline, pagealigned, inittask)			\
	. = ALIGN(PAGE_SIZE);						\
	.data : AT(ADDR(.data) - LOAD_OFFSET) {				\
		INIT_TASK_DATA(inittask)				\
		NOSAVE_DATA						\
		PAGE_ALIGNED_DATA(pagealigned)				\
		CACHELINE_ALIGNED_DATA(cacheline)			\
		READ_MOSTLY_DATA(cacheline)				\
		DATA_DATA						\
		CONSTRUCTORS						\
	}								\
	BUG_TABLE							\

#define INIT_TEXT_SECTION(inittext_align)				\
	. = ALIGN(inittext_align);					\
	.init.text : AT(ADDR(.init.text) - LOAD_OFFSET) {		\
		_sinittext = .;						\
		INIT_TEXT						\
		_einittext = .;						\
	}

#define INIT_DATA_SECTION(initsetup_align)				\
	.init.data : AT(ADDR(.init.data) - LOAD_OFFSET) {		\
		INIT_DATA						\
		INIT_SETUP(initsetup_align)				\
		INIT_CALLS						\
		CON_INITCALL						\
		INIT_RAM_FS						\
	}

#define BSS_SECTION(sbss_align, bss_align, stop_align)			\
	. = ALIGN(sbss_align);						\
	__bss_start = .;						\
	SBSS(sbss_align)						\
	BSS(bss_align)							\
	. = ALIGN(stop_align);						\
	__bss_stop = .;
