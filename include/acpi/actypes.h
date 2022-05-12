/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */
/******************************************************************************
 *
 * Name: actypes.h - Common data types for the entire ACPI subsystem
 *
 * Copyright (C) 2000 - 2021, Intel Corp.
 *
 *****************************************************************************/

#ifndef __ACTYPES_H__
#define __ACTYPES_H__

/* acpisrc:struct_defs -- for acpisrc conversion */

/*
 * ACPI_MACHINE_WIDTH must be specified in an OS- or compiler-dependent
 * header and must be either 32 or 64. 16-bit ACPICA is no longer
 * supported, as of 12/2006.
 */
#ifndef ACPI_MACHINE_WIDTH
#error ACPI_MACHINE_WIDTH not defined
#endif

/*
 * Data type ranges
 * Note: These macros are designed to be compiler independent as well as
 * working around problems that some 32-bit compilers have with 64-bit
 * constants.
 */
#define ACPI_UINT8_MAX                  (u8) (~((u8)  0))	/* 0xFF               */
#define ACPI_UINT16_MAX                 (u16)(~((u16) 0))	/* 0xFFFF             */
#define ACPI_UINT32_MAX                 (u32)(~((u32) 0))	/* 0xFFFFFFFF         */
#define ACPI_UINT64_MAX                 (u64)(~((u64) 0))	/* 0xFFFFFFFFFFFFFFFF */
#define ACPI_ASCII_MAX                  0x7F

/*
 * Architecture-specific ACPICA Subsystem Data Types
 *
 * The goal of these types is to provide source code portability across
 * 16-bit, 32-bit, and 64-bit targets.
 *
 * 1) The following types are of fixed size for all targets (16/32/64):
 *
 * u8           Logical boolean
 *
 * u8           8-bit  (1 byte) unsigned value
 * u16          16-bit (2 byte) unsigned value
 * u32          32-bit (4 byte) unsigned value
 * u64          64-bit (8 byte) unsigned value
 *
 * s16          16-bit (2 byte) signed value
 * s32          32-bit (4 byte) signed value
 * s64          64-bit (8 byte) signed value
 *
 * COMPILER_DEPENDENT_UINT64/s64 - These types are defined in the
 * compiler-dependent header(s) and were introduced because there is no
 * common 64-bit integer type across the various compilation models, as
 * shown in the table below.
 *
 * Datatype  LP64 ILP64 LLP64 ILP32 LP32 16bit
 * char      8    8     8     8     8    8
 * short     16   16    16    16    16   16
 * _int32         32
 * int       32   64    32    32    16   16
 * long      64   64    32    32    32   32
 * long long            64    64
 * pointer   64   64    64    32    32   32
 *
 * Note: ILP64 and LP32 are currently not supported.
 *
 *
 * 2) These types represent the native word size of the target mode of the
 * processor, and may be 16-bit, 32-bit, or 64-bit as required. They are
 * usually used for memory allocation, efficient loop counters, and array
 * indexes. The types are similar to the size_t type in the C library and
 * are required because there is no C type that consistently represents the
 * native data width. acpi_size is needed because there is no guarantee
 * that a kernel-level C library is present.
 *
 * acpi_size        16/32/64-bit unsigned value
 * acpi_native_int  16/32/64-bit signed value
 */

/*******************************************************************************
 *
 * Common types for all compilers, all targets
 *
 ******************************************************************************/

#ifndef ACPI_USE_SYSTEM_INTTYPES

typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef COMPILER_DEPENDENT_UINT64 u64;
typedef COMPILER_DEPENDENT_INT64 s64;

#endif				/* ACPI_USE_SYSTEM_INTTYPES */

/*
 * Value returned by acpi_os_get_thread_id. There is no standard "thread_id"
 * across operating systems or even the various UNIX systems. Since ACPICA
 * only needs the thread ID as a unique thread identifier, we use a u64
 * as the only common data type - it will accommodate any type of pointer or
 * any type of integer. It is up to the host-dependent OSL to cast the
 * native thread ID type to a u64 (in acpi_os_get_thread_id).
 */
#define acpi_thread_id                  u64

/*******************************************************************************
 *
 * Types specific to 64-bit targets
 *
 ******************************************************************************/

#if ACPI_MACHINE_WIDTH == 64

#ifndef ACPI_USE_SYSTEM_INTTYPES

typedef unsigned int u32;
typedef int s32;

#endif				/* ACPI_USE_SYSTEM_INTTYPES */

typedef s64 acpi_native_int;

typedef u64 acpi_size;
typedef u64 acpi_io_address;
typedef u64 acpi_physical_address;

#define ACPI_MAX_PTR                    ACPI_UINT64_MAX
#define ACPI_SIZE_MAX                   ACPI_UINT64_MAX

#define ACPI_USE_NATIVE_DIVIDE	/* Has native 64-bit integer support */
#define ACPI_USE_NATIVE_MATH64	/* Has native 64-bit integer support */

/*
 * In the case of the Itanium Processor Family (IPF), the hardware does not
 * support misaligned memory transfers. Set the MISALIGNMENT_NOT_SUPPORTED
 * flag to indicate that special precautions must be taken to avoid alignment
 * faults. (IA64 or ia64 is currently used by existing compilers to indicate
 * IPF.)
 *
 * Note: EM64T and other X86-64 processors support misaligned transfers,
 * so there is no need to define this flag.
 */
#if defined (__IA64__) || defined (__ia64__)
#define ACPI_MISALIGNMENT_NOT_SUPPORTED
#endif

/*******************************************************************************
 *
 * Types specific to 32-bit targets
 *
 ******************************************************************************/

#elif ACPI_MACHINE_WIDTH == 32

#ifndef ACPI_USE_SYSTEM_INTTYPES

typedef unsigned int u32;
typedef int s32;

#endif				/* ACPI_USE_SYSTEM_INTTYPES */

typedef s32 acpi_native_int;

typedef u32 acpi_size;

#ifdef ACPI_32BIT_PHYSICAL_ADDRESS

/*
 * OSPMs can define this to shrink the size of the structures for 32-bit
 * none PAE environment. ASL compiler may always define this to generate
 * 32-bit OSPM compliant tables.
 */
typedef u32 acpi_io_address;
typedef u32 acpi_physical_address;

#else				/* ACPI_32BIT_PHYSICAL_ADDRESS */

/*
 * It is reported that, after some calculations, the physical addresses can
 * wrap over the 32-bit boundary on 32-bit PAE environment.
 * https://bugzilla.kernel.org/show_bug.cgi?id=87971
 */
typedef u64 acpi_io_address;
typedef u64 acpi_physical_address;

#endif				/* ACPI_32BIT_PHYSICAL_ADDRESS */

#define ACPI_MAX_PTR                    ACPI_UINT32_MAX
#define ACPI_SIZE_MAX                   ACPI_UINT32_MAX

#else

/* ACPI_MACHINE_WIDTH must be either 64 or 32 */

#error unknown ACPI_MACHINE_WIDTH
#endif

/*******************************************************************************
 *
 * OS-dependent types
 *
 * If the defaults below are not appropriate for the host system, they can
 * be defined in the OS-specific header, and this will take precedence.
 *
 ******************************************************************************/

/* Flags for acpi_os_acquire_lock/acpi_os_release_lock */

#ifndef acpi_cpu_flags
#define acpi_cpu_flags			acpi_size
#endif

/* Object returned from acpi_os_create_cache */

#ifndef acpi_cache_t
#ifdef ACPI_USE_LOCAL_CACHE
#define acpi_cache_t                    struct acpi_memory_list
#else
#define acpi_cache_t                    void *
#endif
#endif

/*
 * Synchronization objects - Mutexes, Semaphores, and spin_locks
 */
#if (ACPI_MUTEX_TYPE == ACPI_BINARY_SEMAPHORE)
/*
 * These macros are used if the host OS does not support a mutex object.
 * Map the OSL Mutex interfaces to binary semaphores.
 */
#define acpi_mutex                      acpi_semaphore
#define acpi_os_create_mutex(out_handle) acpi_os_create_semaphore (1, 1, out_handle)
#define acpi_os_delete_mutex(handle)    (void) acpi_os_delete_semaphore (handle)
#define acpi_os_acquire_mutex(handle,time) acpi_os_wait_semaphore (handle, 1, time)
#define acpi_os_release_mutex(handle)   (void) acpi_os_signal_semaphore (handle, 1)
#endif

/* Configurable types for synchronization objects */

#ifndef acpi_spinlock
#define acpi_spinlock                   void *
#endif

#ifndef acpi_raw_spinlock
#define acpi_raw_spinlock		acpi_spinlock
#endif

#ifndef acpi_semaphore
#define acpi_semaphore                  void *
#endif

#ifndef acpi_mutex
#define acpi_mutex                      void *
#endif

/*******************************************************************************
 *
 * Compiler-dependent types
 *
 * If the defaults below are not appropriate for the host compiler, they can
 * be defined in the compiler-specific header, and this will take precedence.
 *
 ******************************************************************************/

/* Use C99 uintptr_t for pointer casting if available, "void *" otherwise */

#ifndef acpi_uintptr_t
#define acpi_uintptr_t                  void *
#endif

/*
 * ACPI_PRINTF_LIKE is used to tag functions as "printf-like" because
 * some compilers can catch printf format string problems
 */
#ifndef ACPI_PRINTF_LIKE
#define ACPI_PRINTF_LIKE(c)
#endif

/*
 * Some compilers complain about unused variables. Sometimes we don't want
 * to use all the variables (for example, _acpi_module_name). This allows us
 * to tell the compiler in a per-variable manner that a variable
 * is unused
 */
#ifndef ACPI_UNUSED_VAR
#define ACPI_UNUSED_VAR
#endif

/*
 * All ACPICA external functions that are available to the rest of the
 * kernel are tagged with these macros which can be defined as appropriate
 * for the host.
 *
 * Notes:
 * ACPI_EXPORT_SYMBOL_INIT is used for initialization and termination
 * interfaces that may need special processing.
 * ACPI_EXPORT_SYMBOL is used for all other public external functions.
 */
#ifndef ACPI_EXPORT_SYMBOL_INIT
#define ACPI_EXPORT_SYMBOL_INIT(symbol)
#endif

#ifndef ACPI_EXPORT_SYMBOL
#define ACPI_EXPORT_SYMBOL(symbol)
#endif

/*
 * Compiler/Clibrary-dependent debug initialization. Used for ACPICA
 * utilities only.
 */
#ifndef ACPI_DEBUG_INITIALIZE
#define ACPI_DEBUG_INITIALIZE()
#endif

/*******************************************************************************
 *
 * Configuration
 *
 ******************************************************************************/

#ifdef ACPI_NO_MEM_ALLOCATIONS

#define ACPI_ALLOCATE(a)                NULL
#define ACPI_ALLOCATE_ZEROED(a)         NULL
#define ACPI_FREE(a)
#define ACPI_MEM_TRACKING(a)

#else				/* ACPI_NO_MEM_ALLOCATIONS */

#ifdef ACPI_DBG_TRACK_ALLOCATIONS
/*
 * Memory allocation tracking (used by acpi_exec to detect memory leaks)
 */
#define ACPI_MEM_PARAMETERS             _COMPONENT, _acpi_module_name, __LINE__
#define ACPI_ALLOCATE(a)                acpi_ut_allocate_and_track ((acpi_size) (a), ACPI_MEM_PARAMETERS)
#define ACPI_ALLOCATE_ZEROED(a)         acpi_ut_allocate_zeroed_and_track ((acpi_size) (a), ACPI_MEM_PARAMETERS)
#define ACPI_FREE(a)                    acpi_ut_free_and_track (a, ACPI_MEM_PARAMETERS)
#define ACPI_MEM_TRACKING(a)            a

#else
/*
 * Normal memory allocation directly via the OS services layer
 */
#define ACPI_ALLOCATE(a)                acpi_os_allocate ((acpi_size) (a))
#define ACPI_ALLOCATE_ZEROED(a)         acpi_os_allocate_zeroed ((acpi_size) (a))
#define ACPI_FREE(a)                    acpi_os_free (a)
#define ACPI_MEM_TRACKING(a)

#endif				/* ACPI_DBG_TRACK_ALLOCATIONS */

#endif				/* ACPI_NO_MEM_ALLOCATIONS */

/******************************************************************************
 *
 * ACPI Specification constants (Do not change unless the specification
 * changes)
 *
 *****************************************************************************/

/* Number of distinct FADT-based GPE register blocks (GPE0 and GPE1) */

#define ACPI_MAX_GPE_BLOCKS             2

/* Default ACPI register widths */

#define ACPI_GPE_REGISTER_WIDTH         8
#define ACPI_PM1_REGISTER_WIDTH         16
#define ACPI_PM2_REGISTER_WIDTH         8
#define ACPI_PM_TIMER_WIDTH             32
#define ACPI_RESET_REGISTER_WIDTH       8

/* Names within the namespace are 4 bytes long */

#define ACPI_NAMESEG_SIZE               4	/* Fixed by ACPI spec */
#define ACPI_PATH_SEGMENT_LENGTH        5	/* 4 chars for name + 1 char for separator */
#define ACPI_PATH_SEPARATOR             '.'

/* Sizes for ACPI table headers */

#define ACPI_OEM_ID_SIZE                6
#define ACPI_OEM_TABLE_ID_SIZE          8

/* ACPI/PNP hardware IDs */

#define PCI_ROOT_HID_STRING             "PNP0A03"
#define PCI_EXPRESS_ROOT_HID_STRING     "PNP0A08"

/* PM Timer ticks per second (HZ) */

#define ACPI_PM_TIMER_FREQUENCY         3579545

/*******************************************************************************
 *
 * Independent types
 *
 ******************************************************************************/

/* Logical defines and NULL */

#ifdef FALSE
#undef FALSE
#endif
#define FALSE                           (1 == 0)

#ifdef TRUE
#undef TRUE
#endif
#define TRUE                            (1 == 1)

#ifndef NULL
#define NULL                            (void *) 0
#endif

/*
 * Miscellaneous types
 */
typedef u32 acpi_status;	/* All ACPI Exceptions */
typedef u32 acpi_name;		/* 4-byte ACPI name */
typedef char *acpi_string;	/* Null terminated ASCII string */
typedef void *acpi_handle;	/* Actually a ptr to a NS Node */

/* Time constants for timer calculations */

#define ACPI_MSEC_PER_SEC               1000L

#define ACPI_USEC_PER_MSEC              1000L
#define ACPI_USEC_PER_SEC               1000000L

#define ACPI_100NSEC_PER_USEC           10L
#define ACPI_100NSEC_PER_MSEC           10000L
#define ACPI_100NSEC_PER_SEC            10000000L

#define ACPI_NSEC_PER_USEC              1000L
#define ACPI_NSEC_PER_MSEC              1000000L
#define ACPI_NSEC_PER_SEC               1000000000L

#define ACPI_TIME_AFTER(a, b)           ((s64)((b) - (a)) < 0)

/* Owner IDs are used to track namespace nodes for selective deletion */

typedef u16 acpi_owner_id;
#define ACPI_OWNER_ID_MAX               0xFFF	/* 4095 possible owner IDs */

#define ACPI_INTEGER_BIT_SIZE           64
#define ACPI_MAX_DECIMAL_DIGITS         20	/* 2^64 = 18,446,744,073,709,551,616 */
#define ACPI_MAX64_DECIMAL_DIGITS       20
#define ACPI_MAX32_DECIMAL_DIGITS       10
#define ACPI_MAX16_DECIMAL_DIGITS        5
#define ACPI_MAX8_DECIMAL_DIGITS         3

/*
 * Constants with special meanings
 */
#define ACPI_ROOT_OBJECT                ((acpi_handle) ACPI_TO_POINTER (ACPI_MAX_PTR))
#define ACPI_WAIT_FOREVER               0xFFFF	/* u16, as per ACPI spec */
#define ACPI_DO_NOT_WAIT                0

/*
 * Obsolete: Acpi integer width. In ACPI version 1 (1996), integers are
 * 32 bits. In ACPI version 2 (2000) and later, integers are max 64 bits.
 * Note that this pertains to the ACPI integer type only, not to other
 * integers used in the implementation of the ACPICA subsystem.
 *
 * 01/2010: This type is obsolete and has been removed from the entire ACPICA
 * code base. It remains here for compatibility with device drivers that use
 * the type. However, it will be removed in the future.
 */
typedef u64 acpi_integer;
#define ACPI_INTEGER_MAX                ACPI_UINT64_MAX

/*******************************************************************************
 *
 * Commonly used macros
 *
 ******************************************************************************/

/* Data manipulation */

#define ACPI_LOBYTE(integer)            ((u8)   (u16)(integer))
#define ACPI_HIBYTE(integer)            ((u8) (((u16)(integer)) >> 8))
#define ACPI_LOWORD(integer)            ((u16)  (u32)(integer))
#define ACPI_HIWORD(integer)            ((u16)(((u32)(integer)) >> 16))
#define ACPI_LODWORD(integer64)         ((u32)  (u64)(integer64))
#define ACPI_HIDWORD(integer64)         ((u32)(((u64)(integer64)) >> 32))

#define ACPI_SET_BIT(target,bit)        ((target) |= (bit))
#define ACPI_CLEAR_BIT(target,bit)      ((target) &= ~(bit))
#define ACPI_MIN(a,b)                   (((a)<(b))?(a):(b))
#define ACPI_MAX(a,b)                   (((a)>(b))?(a):(b))

/* Size calculation */

#define ACPI_ARRAY_LENGTH(x)            (sizeof(x) / sizeof((x)[0]))

/* Pointer manipulation */

#define ACPI_CAST_PTR(t, p)             ((t *) (acpi_uintptr_t) (p))
#define ACPI_CAST_INDIRECT_PTR(t, p)    ((t **) (acpi_uintptr_t) (p))
#define ACPI_ADD_PTR(t, a, b)           ACPI_CAST_PTR (t, (ACPI_CAST_PTR (u8, (a)) + (acpi_size)(b)))
#define ACPI_SUB_PTR(t, a, b)           ACPI_CAST_PTR (t, (ACPI_CAST_PTR (u8, (a)) - (acpi_size)(b)))
#define ACPI_PTR_DIFF(a, b)             ((acpi_size) (ACPI_CAST_PTR (u8, (a)) - ACPI_CAST_PTR (u8, (b))))

/* Pointer/Integer type conversions */

#define ACPI_TO_POINTER(i)              ACPI_CAST_PTR (void, (acpi_size) (i))
#define ACPI_TO_INTEGER(p)              ACPI_PTR_DIFF (p, (void *) 0)
#define ACPI_OFFSET(d, f)               ACPI_PTR_DIFF (&(((d *) 0)->f), (void *) 0)
#define ACPI_PHYSADDR_TO_PTR(i)         ACPI_TO_POINTER(i)
#define ACPI_PTR_TO_PHYSADDR(i)         ACPI_TO_INTEGER(i)

/* Optimizations for 4-character (32-bit) acpi_name manipulation */

#ifndef ACPI_MISALIGNMENT_NOT_SUPPORTED
#define ACPI_COMPARE_NAMESEG(a,b)       (*ACPI_CAST_PTR (u32, (a)) == *ACPI_CAST_PTR (u32, (b)))
#define ACPI_COPY_NAMESEG(dest,src)     (*ACPI_CAST_PTR (u32, (dest)) = *ACPI_CAST_PTR (u32, (src)))
#else
#define ACPI_COMPARE_NAMESEG(a,b)       (!strncmp (ACPI_CAST_PTR (char, (a)), ACPI_CAST_PTR (char, (b)), ACPI_NAMESEG_SIZE))
#define ACPI_COPY_NAMESEG(dest,src)     (strncpy (ACPI_CAST_PTR (char, (dest)), ACPI_CAST_PTR (char, (src)), ACPI_NAMESEG_SIZE))
#endif

/* Support for the special RSDP signature (8 characters) */

#define ACPI_VALIDATE_RSDP_SIG(a)       (!strncmp (ACPI_CAST_PTR (char, (a)), ACPI_SIG_RSDP, 8))
#define ACPI_MAKE_RSDP_SIG(dest)        (memcpy (ACPI_CAST_PTR (char, (dest)), ACPI_SIG_RSDP, 8))

/* Support for OEMx signature (x can be any character) */
#define ACPI_IS_OEM_SIG(a)        (!strncmp (ACPI_CAST_PTR (char, (a)), ACPI_OEM_NAME, 3) &&\
	 strnlen (a, ACPI_NAMESEG_SIZE) == ACPI_NAMESEG_SIZE)

/*
 * Algorithm to obtain access bit or byte width.
 * Can be used with access_width of struct acpi_generic_address and access_size of
 * struct acpi_resource_generic_register.
 */
#define ACPI_ACCESS_BIT_WIDTH(size)     (1 << ((size) + 2))
#define ACPI_ACCESS_BYTE_WIDTH(size)    (1 << ((size) - 1))

/*******************************************************************************
 *
 * Miscellaneous constants
 *
 ******************************************************************************/

/*
 * Initialization sequence options
 */
#define ACPI_FULL_INITIALIZATION        0x0000
#define ACPI_NO_FACS_INIT               0x0001
#define ACPI_NO_ACPI_ENABLE             0x0002
#define ACPI_NO_HARDWARE_INIT           0x0004
#define ACPI_NO_EVENT_INIT              0x0008
#define ACPI_NO_HANDLER_INIT            0x0010
#define ACPI_NO_OBJECT_INIT             0x0020
#define ACPI_NO_DEVICE_INIT             0x0040
#define ACPI_NO_ADDRESS_SPACE_INIT      0x0080

/*
 * Initialization state
 */
#define ACPI_SUBSYSTEM_INITIALIZE       0x01
#define ACPI_INITIALIZED_OK             0x02

/*
 * Power state values
 */
#define ACPI_STATE_UNKNOWN              (u8) 0xFF

#define ACPI_STATE_S0                   (u8) 0
#define ACPI_STATE_S1                   (u8) 1
#define ACPI_STATE_S2                   (u8) 2
#define ACPI_STATE_S3                   (u8) 3
#define ACPI_STATE_S4                   (u8) 4
#define ACPI_STATE_S5                   (u8) 5
#define ACPI_S_STATES_MAX               ACPI_STATE_S5
#define ACPI_S_STATE_COUNT              6

#define ACPI_STATE_D0                   (u8) 0
#define ACPI_STATE_D1                   (u8) 1
#define ACPI_STATE_D2                   (u8) 2
#define ACPI_STATE_D3_HOT               (u8) 3
#define ACPI_STATE_D3                   (u8) 4
#define ACPI_STATE_D3_COLD              ACPI_STATE_D3
#define ACPI_D_STATES_MAX               ACPI_STATE_D3
#define ACPI_D_STATE_COUNT              5

#define ACPI_STATE_C0                   (u8) 0
#define ACPI_STATE_C1                   (u8) 1
#define ACPI_STATE_C2                   (u8) 2
#define ACPI_STATE_C3                   (u8) 3
#define ACPI_C_STATES_MAX               ACPI_STATE_C3
#define ACPI_C_STATE_COUNT              4

/*
 * Sleep type invalid value
 */
#define ACPI_SLEEP_TYPE_MAX             0x7
#define ACPI_SLEEP_TYPE_INVALID         0xFF

/*
 * Standard notify values
 */
#define ACPI_NOTIFY_BUS_CHECK           (u8) 0x00
#define ACPI_NOTIFY_DEVICE_CHECK        (u8) 0x01
#define ACPI_NOTIFY_DEVICE_WAKE         (u8) 0x02
#define ACPI_NOTIFY_EJECT_REQUEST       (u8) 0x03
#define ACPI_NOTIFY_DEVICE_CHECK_LIGHT  (u8) 0x04
#define ACPI_NOTIFY_FREQUENCY_MISMATCH  (u8) 0x05
#define ACPI_NOTIFY_BUS_MODE_MISMATCH   (u8) 0x06
#define ACPI_NOTIFY_POWER_FAULT         (u8) 0x07
#define ACPI_NOTIFY_CAPABILITIES_CHECK  (u8) 0x08
#define ACPI_NOTIFY_DEVICE_PLD_CHECK    (u8) 0x09
#define ACPI_NOTIFY_RESERVED            (u8) 0x0A
#define ACPI_NOTIFY_LOCALITY_UPDATE     (u8) 0x0B
#define ACPI_NOTIFY_SHUTDOWN_REQUEST    (u8) 0x0C
#define ACPI_NOTIFY_AFFINITY_UPDATE     (u8) 0x0D
#define ACPI_NOTIFY_MEMORY_UPDATE       (u8) 0x0E
#define ACPI_NOTIFY_DISCONNECT_RECOVER  (u8) 0x0F

#define ACPI_GENERIC_NOTIFY_MAX         0x0F
#define ACPI_SPECIFIC_NOTIFY_MAX        0x84

/*
 * Types associated with ACPI names and objects. The first group of
 * values (up to ACPI_TYPE_EXTERNAL_MAX) correspond to the definition
 * of the ACPI object_type() operator (See the ACPI Spec). Therefore,
 * only add to the first group if the spec changes.
 *
 * NOTE: Types must be kept in sync with the global acpi_ns_properties
 * and acpi_ns_type_names arrays.
 */
typedef u32 acpi_object_type;

#define ACPI_TYPE_ANY                   0x00
#define ACPI_TYPE_INTEGER               0x01	/* Byte/Word/Dword/Zero/One/Ones */
#define ACPI_TYPE_STRING                0x02
#define ACPI_TYPE_BUFFER                0x03
#define ACPI_TYPE_PACKAGE               0x04	/* byte_const, multiple data_term/Constant/super_name */
#define ACPI_TYPE_FIELD_UNIT            0x05
#define ACPI_TYPE_DEVICE                0x06	/* Name, multiple Node */
#define ACPI_TYPE_EVENT                 0x07
#define ACPI_TYPE_METHOD                0x08	/* Name, byte_const, multiple Code */
#define ACPI_TYPE_MUTEX                 0x09
#define ACPI_TYPE_REGION                0x0A
#define ACPI_TYPE_POWER                 0x0B	/* Name,byte_const,word_const,multi Node */
#define ACPI_TYPE_PROCESSOR             0x0C	/* Name,byte_const,Dword_const,byte_const,multi nm_o */
#define ACPI_TYPE_THERMAL               0x0D	/* Name, multiple Node */
#define ACPI_TYPE_BUFFER_FIELD          0x0E
#define ACPI_TYPE_DDB_HANDLE            0x0F
#define ACPI_TYPE_DEBUG_OBJECT          0x10

#define ACPI_TYPE_EXTERNAL_MAX          0x10
#define ACPI_NUM_TYPES                  (ACPI_TYPE_EXTERNAL_MAX + 1)

/*
 * These are object types that do not map directly to the ACPI
 * object_type() operator. They are used for various internal purposes
 * only. If new predefined ACPI_TYPEs are added (via the ACPI
 * specification), these internal types must move upwards. (There
 * is code that depends on these values being contiguous with the
 * external types above.)
 */
#define ACPI_TYPE_LOCAL_REGION_FIELD    0x11
#define ACPI_TYPE_LOCAL_BANK_FIELD      0x12
#define ACPI_TYPE_LOCAL_INDEX_FIELD     0x13
#define ACPI_TYPE_LOCAL_REFERENCE       0x14	/* Arg#, Local#, Name, Debug, ref_of, Index */
#define ACPI_TYPE_LOCAL_ALIAS           0x15
#define ACPI_TYPE_LOCAL_METHOD_ALIAS    0x16
#define ACPI_TYPE_LOCAL_NOTIFY          0x17
#define ACPI_TYPE_LOCAL_ADDRESS_HANDLER 0x18
#define ACPI_TYPE_LOCAL_RESOURCE        0x19
#define ACPI_TYPE_LOCAL_RESOURCE_FIELD  0x1A
#define ACPI_TYPE_LOCAL_SCOPE           0x1B	/* 1 Name, multiple object_list Nodes */

#define ACPI_TYPE_NS_NODE_MAX           0x1B	/* Last typecode used within a NS Node */
#define ACPI_TOTAL_TYPES                (ACPI_TYPE_NS_NODE_MAX + 1)

/*
 * These are special object types that never appear in
 * a Namespace node, only in an object of union acpi_operand_object
 */
#define ACPI_TYPE_LOCAL_EXTRA           0x1C
#define ACPI_TYPE_LOCAL_DATA            0x1D

#define ACPI_TYPE_LOCAL_MAX             0x1D

/* All types above here are invalid */

#define ACPI_TYPE_INVALID               0x1E
#define ACPI_TYPE_NOT_FOUND             0xFF

#define ACPI_NUM_NS_TYPES               (ACPI_TYPE_INVALID + 1)

/*
 * All I/O
 */
#define ACPI_READ                       0
#define ACPI_WRITE                      1
#define ACPI_IO_MASK                    1

/*
 * Event Types: Fixed & General Purpose
 */
typedef u32 acpi_event_type;

/*
 * Fixed events
 */
#define ACPI_EVENT_PMTIMER              0
#define ACPI_EVENT_GLOBAL               1
#define ACPI_EVENT_POWER_BUTTON         2
#define ACPI_EVENT_SLEEP_BUTTON         3
#define ACPI_EVENT_RTC                  4
#define ACPI_EVENT_MAX                  4
#define ACPI_NUM_FIXED_EVENTS           ACPI_EVENT_MAX + 1

/*
 * Event status - Per event
 * -------------
 * The encoding of acpi_event_status is illustrated below.
 * Note that a set bit (1) indicates the property is TRUE
 * (e.g. if bit 0 is set then the event is enabled).
 * +-------------+-+-+-+-+-+-+
 * |   Bits 31:6 |5|4|3|2|1|0|
 * +-------------+-+-+-+-+-+-+
 *          |     | | | | | |
 *          |     | | | | | +- Enabled?
 *          |     | | | | +--- Enabled for wake?
 *          |     | | | +----- Status bit set?
 *          |     | | +------- Enable bit set?
 *          |     | +--------- Has a handler?
 *          |     +----------- Masked?
 *          +----------------- <Reserved>
 */
typedef u32 acpi_event_status;

#define ACPI_EVENT_FLAG_DISABLED        (acpi_event_status) 0x00
#define ACPI_EVENT_FLAG_ENABLED         (acpi_event_status) 0x01
#define ACPI_EVENT_FLAG_WAKE_ENABLED    (acpi_event_status) 0x02
#define ACPI_EVENT_FLAG_STATUS_SET      (acpi_event_status) 0x04
#define ACPI_EVENT_FLAG_ENABLE_SET      (acpi_event_status) 0x08
#define ACPI_EVENT_FLAG_HAS_HANDLER     (acpi_event_status) 0x10
#define ACPI_EVENT_FLAG_MASKED          (acpi_event_status) 0x20
#define ACPI_EVENT_FLAG_SET             ACPI_EVENT_FLAG_STATUS_SET

/* Actions for acpi_set_gpe, acpi_gpe_wakeup, acpi_hw_low_set_gpe */

#define ACPI_GPE_ENABLE                 0
#define ACPI_GPE_DISABLE                1
#define ACPI_GPE_CONDITIONAL_ENABLE     2

/*
 * GPE info flags - Per GPE
 * +---+-+-+-+---+
 * |7:6|5|4|3|2:0|
 * +---+-+-+-+---+
 *   |  | | |  |
 *   |  | | |  +-- Type of dispatch:to method, handler, notify, or none
 *   |  | | +----- Interrupt type: edge or level triggered
 *   |  | +------- Is a Wake GPE
 *   |  +--------- Has been enabled automatically at init time
 *   +------------ <Reserved>
 */
#define ACPI_GPE_DISPATCH_NONE          (u8) 0x00
#define ACPI_GPE_DISPATCH_METHOD        (u8) 0x01
#define ACPI_GPE_DISPATCH_HANDLER       (u8) 0x02
#define ACPI_GPE_DISPATCH_NOTIFY        (u8) 0x03
#define ACPI_GPE_DISPATCH_RAW_HANDLER   (u8) 0x04
#define ACPI_GPE_DISPATCH_MASK          (u8) 0x07
#define ACPI_GPE_DISPATCH_TYPE(flags)   ((u8) ((flags) & ACPI_GPE_DISPATCH_MASK))

#define ACPI_GPE_LEVEL_TRIGGERED        (u8) 0x08
#define ACPI_GPE_EDGE_TRIGGERED         (u8) 0x00
#define ACPI_GPE_XRUPT_TYPE_MASK        (u8) 0x08

#define ACPI_GPE_CAN_WAKE               (u8) 0x10
#define ACPI_GPE_AUTO_ENABLED           (u8) 0x20
#define ACPI_GPE_INITIALIZED            (u8) 0x40

/*
 * Flags for GPE and Lock interfaces
 */
#define ACPI_NOT_ISR                    0x1
#define ACPI_ISR                        0x0

/* Notify types */

#define ACPI_SYSTEM_NOTIFY              0x1
#define ACPI_DEVICE_NOTIFY              0x2
#define ACPI_ALL_NOTIFY                 (ACPI_SYSTEM_NOTIFY | ACPI_DEVICE_NOTIFY)
#define ACPI_MAX_NOTIFY_HANDLER_TYPE    0x3
#define ACPI_NUM_NOTIFY_TYPES           2

#define ACPI_MAX_SYS_NOTIFY             0x7F
#define ACPI_MAX_DEVICE_SPECIFIC_NOTIFY 0xBF

#define ACPI_SYSTEM_HANDLER_LIST        0	/* Used as index, must be SYSTEM_NOTIFY -1 */
#define ACPI_DEVICE_HANDLER_LIST        1	/* Used as index, must be DEVICE_NOTIFY -1 */

/* Address Space (Operation Region) Types */

typedef u8 acpi_adr_space_type;

#define ACPI_ADR_SPACE_SYSTEM_MEMORY    (acpi_adr_space_type) 0
#define ACPI_ADR_SPACE_SYSTEM_IO        (acpi_adr_space_type) 1
#define ACPI_ADR_SPACE_PCI_CONFIG       (acpi_adr_space_type) 2
#define ACPI_ADR_SPACE_EC               (acpi_adr_space_type) 3
#define ACPI_ADR_SPACE_SMBUS            (acpi_adr_space_type) 4
#define ACPI_ADR_SPACE_CMOS             (acpi_adr_space_type) 5
#define ACPI_ADR_SPACE_PCI_BAR_TARGET   (acpi_adr_space_type) 6
#define ACPI_ADR_SPACE_IPMI             (acpi_adr_space_type) 7
#define ACPI_ADR_SPACE_GPIO             (acpi_adr_space_type) 8
#define ACPI_ADR_SPACE_GSBUS            (acpi_adr_space_type) 9
#define ACPI_ADR_SPACE_PLATFORM_COMM    (acpi_adr_space_type) 10
#define ACPI_ADR_SPACE_PLATFORM_RT      (acpi_adr_space_type) 11

#define ACPI_NUM_PREDEFINED_REGIONS     12

/*
 * Special Address Spaces
 *
 * Note: A Data Table region is a special type of operation region
 * that has its own AML opcode. However, internally, the AML
 * interpreter simply creates an operation region with an address
 * space type of ACPI_ADR_SPACE_DATA_TABLE.
 */
#define ACPI_ADR_SPACE_DATA_TABLE       (acpi_adr_space_type) 0x7E	/* Internal to ACPICA only */
#define ACPI_ADR_SPACE_FIXED_HARDWARE   (acpi_adr_space_type) 0x7F

/* Values for _REG connection code */

#define ACPI_REG_DISCONNECT             0
#define ACPI_REG_CONNECT                1

/*
 * bit_register IDs
 *
 * These values are intended to be used by the hardware interfaces
 * and are mapped to individual bitfields defined within the ACPI
 * registers. See the acpi_gbl_bit_register_info global table in utglobal.c
 * for this mapping.
 */

/* PM1 Status register */

#define ACPI_BITREG_TIMER_STATUS                0x00
#define ACPI_BITREG_BUS_MASTER_STATUS           0x01
#define ACPI_BITREG_GLOBAL_LOCK_STATUS          0x02
#define ACPI_BITREG_POWER_BUTTON_STATUS         0x03
#define ACPI_BITREG_SLEEP_BUTTON_STATUS         0x04
#define ACPI_BITREG_RT_CLOCK_STATUS             0x05
#define ACPI_BITREG_WAKE_STATUS                 0x06
#define ACPI_BITREG_PCIEXP_WAKE_STATUS          0x07

/* PM1 Enable register */

#define ACPI_BITREG_TIMER_ENABLE                0x08
#define ACPI_BITREG_GLOBAL_LOCK_ENABLE          0x09
#define ACPI_BITREG_POWER_BUTTON_ENABLE         0x0A
#define ACPI_BITREG_SLEEP_BUTTON_ENABLE         0x0B
#define ACPI_BITREG_RT_CLOCK_ENABLE             0x0C
#define ACPI_BITREG_PCIEXP_WAKE_DISABLE         0x0D

/* PM1 Control register */

#define ACPI_BITREG_SCI_ENABLE                  0x0E
#define ACPI_BITREG_BUS_MASTER_RLD              0x0F
#define ACPI_BITREG_GLOBAL_LOCK_RELEASE         0x10
#define ACPI_BITREG_SLEEP_TYPE                  0x11
#define ACPI_BITREG_SLEEP_ENABLE                0x12

/* PM2 Control register */

#define ACPI_BITREG_ARB_DISABLE                 0x13

#define ACPI_BITREG_MAX                         0x13
#define ACPI_NUM_BITREG                         ACPI_BITREG_MAX + 1

/* Status register values. A 1 clears a status bit. 0 = no effect */

#define ACPI_CLEAR_STATUS                       1

/* Enable and Control register values */

#define ACPI_ENABLE_EVENT                       1
#define ACPI_DISABLE_EVENT                      0

/*
 * External ACPI object definition
 */

/*
 * Note: Type == ACPI_TYPE_ANY (0) is used to indicate a NULL package
 * element or an unresolved named reference.
 */
union acpi_object {
	acpi_object_type type;	/* See definition of acpi_ns_type for values */
	struct {
		acpi_object_type type;	/* ACPI_TYPE_INTEGER */
		u64 value;	/* The actual number */
	} integer;

	struct {
		acpi_object_type type;	/* ACPI_TYPE_STRING */
		u32 length;	/* # of bytes in string, excluding trailing null */
		char *pointer;	/* points to the string value */
	} string;

	struct {
		acpi_object_type type;	/* ACPI_TYPE_BUFFER */
		u32 length;	/* # of bytes in buffer */
		u8 *pointer;	/* points to the buffer */
	} buffer;

	struct {
		acpi_object_type type;	/* ACPI_TYPE_PACKAGE */
		u32 count;	/* # of elements in package */
		union acpi_object *elements;	/* Pointer to an array of ACPI_OBJECTs */
	} package;

	struct {
		acpi_object_type type;	/* ACPI_TYPE_LOCAL_REFERENCE */
		acpi_object_type actual_type;	/* Type associated with the Handle */
		acpi_handle handle;	/* object reference */
	} reference;

	struct {
		acpi_object_type type;	/* ACPI_TYPE_PROCESSOR */
		u32 proc_id;
		acpi_io_address pblk_address;
		u32 pblk_length;
	} processor;

	struct {
		acpi_object_type type;	/* ACPI_TYPE_POWER */
		u32 system_level;
		u32 resource_order;
	} power_resource;
};

/*
 * List of objects, used as a parameter list for control method evaluation
 */
struct acpi_object_list {
	u32 count;
	union acpi_object *pointer;
};

/*
 * Miscellaneous common Data Structures used by the interfaces
 */
#define ACPI_NO_BUFFER              0

#ifdef ACPI_NO_MEM_ALLOCATIONS

#define ACPI_ALLOCATE_BUFFER        (acpi_size) (0)
#define ACPI_ALLOCATE_LOCAL_BUFFER  (acpi_size) (0)

#else				/* ACPI_NO_MEM_ALLOCATIONS */

#define ACPI_ALLOCATE_BUFFER        (acpi_size) (-1)	/* Let ACPICA allocate buffer */
#define ACPI_ALLOCATE_LOCAL_BUFFER  (acpi_size) (-2)	/* For internal use only (enables tracking) */

#endif				/* ACPI_NO_MEM_ALLOCATIONS */

struct acpi_buffer {
	acpi_size length;	/* Length in bytes of the buffer */
	void *pointer;		/* pointer to buffer */
};

/*
 * name_type for acpi_get_name
 */
#define ACPI_FULL_PATHNAME              0
#define ACPI_SINGLE_NAME                1
#define ACPI_FULL_PATHNAME_NO_TRAILING  2
#define ACPI_NAME_TYPE_MAX              2

/*
 * Predefined Namespace items
 */
struct acpi_predefined_names {
	const char *name;
	u8 type;
	char *val;
};

/*
 * Structure and flags for acpi_get_system_info
 */
#define ACPI_SYS_MODE_UNKNOWN           0x0000
#define ACPI_SYS_MODE_ACPI              0x0001
#define ACPI_SYS_MODE_LEGACY            0x0002
#define ACPI_SYS_MODES_MASK             0x0003

/*
 * System info returned by acpi_get_system_info()
 */
struct acpi_system_info {
	u32 acpi_ca_version;
	u32 flags;
	u32 timer_resolution;
	u32 reserved1;
	u32 reserved2;
	u32 debug_level;
	u32 debug_layer;
};

/*
 * System statistics returned by acpi_get_statistics()
 */
struct acpi_statistics {
	u32 sci_count;
	u32 gpe_count;
	u32 fixed_event_count[ACPI_NUM_FIXED_EVENTS];
	u32 method_count;
};

/*
 * Types specific to the OS service interfaces
 */
typedef u32
 (ACPI_SYSTEM_XFACE * acpi_osd_handler) (void *context);

typedef void
 (ACPI_SYSTEM_XFACE * acpi_osd_exec_callback) (void *context);

/*
 * Various handlers and callback procedures
 */
typedef
u32 (*acpi_sci_handler) (void *context);

typedef
void (*acpi_gbl_event_handler) (u32 event_type,
			       acpi_handle device,
			       u32 event_number, void *context);

#define ACPI_EVENT_TYPE_GPE         0
#define ACPI_EVENT_TYPE_FIXED       1

typedef
u32(*acpi_event_handler) (void *context);

typedef
u32 (*acpi_gpe_handler) (acpi_handle gpe_device, u32 gpe_number, void *context);

typedef
void (*acpi_notify_handler) (acpi_handle device, u32 value, void *context);

typedef
void (*acpi_object_handler) (acpi_handle object, void *data);

typedef
acpi_status (*acpi_init_handler) (acpi_handle object, u32 function);

#define ACPI_INIT_DEVICE_INI        1

typedef
acpi_status (*acpi_exception_handler) (acpi_status aml_status,
				       acpi_name name,
				       u16 opcode,
				       u32 aml_offset, void *context);

/* Table Event handler (Load, load_table, etc.) and types */

typedef
acpi_status (*acpi_table_handler) (u32 event, void *table, void *context);

/* Table Event Types */

#define ACPI_TABLE_EVENT_LOAD           0x0
#define ACPI_TABLE_EVENT_UNLOAD         0x1
#define ACPI_TABLE_EVENT_INSTALL        0x2
#define ACPI_TABLE_EVENT_UNINSTALL      0x3
#define ACPI_NUM_TABLE_EVENTS           4

/* Address Spaces (For Operation Regions) */

typedef
acpi_status (*acpi_adr_space_handler) (u32 function,
				       acpi_physical_address address,
				       u32 bit_width,
				       u64 *value,
				       void *handler_context,
				       void *region_context);

#define ACPI_DEFAULT_HANDLER            NULL

/* Special Context data for generic_serial_bus/general_purpose_io (ACPI 5.0) */

struct acpi_connection_info {
	u8 *connection;
	u16 length;
	u8 access_length;
};

typedef
acpi_status (*acpi_adr_space_setup) (acpi_handle region_handle,
				     u32 function,
				     void *handler_context,
				     void **region_context);

#define ACPI_REGION_ACTIVATE    0
#define ACPI_REGION_DEACTIVATE  1

typedef
acpi_status (*acpi_walk_callback) (acpi_handle object,
				   u32 nesting_level,
				   void *context, void **return_value);

typedef
u32 (*acpi_interface_handler) (acpi_string interface_name, u32 supported);

/* Interrupt handler return values */

#define ACPI_INTERRUPT_NOT_HANDLED      0x00
#define ACPI_INTERRUPT_HANDLED          0x01

/* GPE handler return values */

#define ACPI_REENABLE_GPE               0x80

/* Length of 32-bit EISAID values when converted back to a string */

#define ACPI_EISAID_STRING_SIZE         8	/* Includes null terminator */

/* Length of UUID (string) values */

#define ACPI_UUID_LENGTH                16

/* Length of 3-byte PCI class code values when converted back to a string */

#define ACPI_PCICLS_STRING_SIZE         7	/* Includes null terminator */

/* Structures used for device/processor HID, UID, CID */

struct acpi_pnp_device_id {
	u32 length;		/* Length of string + null */
	char *string;
};

struct acpi_pnp_device_id_list {
	u32 count;		/* Number of IDs in Ids array */
	u32 list_size;		/* Size of list, including ID strings */
	struct acpi_pnp_device_id ids[];	/* ID array */
};

/*
 * Structure returned from acpi_get_object_info.
 * Optimized for both 32-bit and 64-bit builds.
 */
struct acpi_device_info {
	u32 info_size;		/* Size of info, including ID strings */
	u32 name;		/* ACPI object Name */
	acpi_object_type type;	/* ACPI object Type */
	u8 param_count;		/* If a method, required parameter count */
	u16 valid;		/* Indicates which optional fields are valid */
	u8 flags;		/* Miscellaneous info */
	u8 highest_dstates[4];	/* _sx_d values: 0xFF indicates not valid */
	u8 lowest_dstates[5];	/* _sx_w values: 0xFF indicates not valid */
	u64 address;	/* _ADR value */
	struct acpi_pnp_device_id hardware_id;	/* _HID value */
	struct acpi_pnp_device_id unique_id;	/* _UID value */
	struct acpi_pnp_device_id class_code;	/* _CLS value */
	struct acpi_pnp_device_id_list compatible_id_list;	/* _CID list <must be last> */
};

/* Values for Flags field above (acpi_get_object_info) */

#define ACPI_PCI_ROOT_BRIDGE            0x01

/* Flags for Valid field above (acpi_get_object_info) */

#define ACPI_VALID_ADR                  0x0002
#define ACPI_VALID_HID                  0x0004
#define ACPI_VALID_UID                  0x0008
#define ACPI_VALID_CID                  0x0020
#define ACPI_VALID_CLS                  0x0040
#define ACPI_VALID_SXDS                 0x0100
#define ACPI_VALID_SXWS                 0x0200

/* Flags for _STA method */

#define ACPI_STA_DEVICE_PRESENT         0x01
#define ACPI_STA_DEVICE_ENABLED         0x02
#define ACPI_STA_DEVICE_UI              0x04
#define ACPI_STA_DEVICE_FUNCTIONING     0x08
#define ACPI_STA_DEVICE_OK              0x08	/* Synonym */
#define ACPI_STA_BATTERY_PRESENT        0x10

/* Context structs for address space handlers */

struct acpi_pci_id {
	u16 segment;
	u16 bus;
	u16 device;
	u16 function;
};

struct acpi_mem_mapping {
	acpi_physical_address physical_address;
	u8 *logical_address;
	acpi_size length;
	struct acpi_mem_mapping *next_mm;
};

struct acpi_mem_space_context {
	u32 length;
	acpi_physical_address address;
	struct acpi_mem_mapping *cur_mm;
	struct acpi_mem_mapping *first_mm;
};

/*
 * struct acpi_memory_list is used only if the ACPICA local cache is enabled
 */
struct acpi_memory_list {
	const char *list_name;
	void *list_head;
	u16 object_size;
	u16 max_depth;
	u16 current_depth;

#ifdef ACPI_DBG_TRACK_ALLOCATIONS

	/* Statistics for debug memory tracking only */

	u32 total_allocated;
	u32 total_freed;
	u32 max_occupied;
	u32 total_size;
	u32 current_total_size;
	u32 requests;
	u32 hits;
#endif
};

/* Definitions of trace event types */

typedef enum {
	ACPI_TRACE_AML_METHOD,
	ACPI_TRACE_AML_OPCODE,
	ACPI_TRACE_AML_REGION
} acpi_trace_event_type;

/* Definitions of _OSI support */

#define ACPI_VENDOR_STRINGS                 0x01
#define ACPI_FEATURE_STRINGS                0x02
#define ACPI_ENABLE_INTERFACES              0x00
#define ACPI_DISABLE_INTERFACES             0x04

#define ACPI_DISABLE_ALL_VENDOR_STRINGS     (ACPI_DISABLE_INTERFACES | ACPI_VENDOR_STRINGS)
#define ACPI_DISABLE_ALL_FEATURE_STRINGS    (ACPI_DISABLE_INTERFACES | ACPI_FEATURE_STRINGS)
#define ACPI_DISABLE_ALL_STRINGS            (ACPI_DISABLE_INTERFACES | ACPI_VENDOR_STRINGS | ACPI_FEATURE_STRINGS)
#define ACPI_ENABLE_ALL_VENDOR_STRINGS      (ACPI_ENABLE_INTERFACES | ACPI_VENDOR_STRINGS)
#define ACPI_ENABLE_ALL_FEATURE_STRINGS     (ACPI_ENABLE_INTERFACES | ACPI_FEATURE_STRINGS)
#define ACPI_ENABLE_ALL_STRINGS             (ACPI_ENABLE_INTERFACES | ACPI_VENDOR_STRINGS | ACPI_FEATURE_STRINGS)

#define ACPI_OSI_WIN_2000               0x01
#define ACPI_OSI_WIN_XP                 0x02
#define ACPI_OSI_WIN_XP_SP1             0x03
#define ACPI_OSI_WINSRV_2003            0x04
#define ACPI_OSI_WIN_XP_SP2             0x05
#define ACPI_OSI_WINSRV_2003_SP1        0x06
#define ACPI_OSI_WIN_VISTA              0x07
#define ACPI_OSI_WINSRV_2008            0x08
#define ACPI_OSI_WIN_VISTA_SP1          0x09
#define ACPI_OSI_WIN_VISTA_SP2          0x0A
#define ACPI_OSI_WIN_7                  0x0B
#define ACPI_OSI_WIN_8                  0x0C
#define ACPI_OSI_WIN_8_1                0x0D
#define ACPI_OSI_WIN_10                 0x0E
#define ACPI_OSI_WIN_10_RS1             0x0F
#define ACPI_OSI_WIN_10_RS2             0x10
#define ACPI_OSI_WIN_10_RS3             0x11
#define ACPI_OSI_WIN_10_RS4             0x12
#define ACPI_OSI_WIN_10_RS5             0x13
#define ACPI_OSI_WIN_10_19H1            0x14

/* Definitions of getopt */

#define ACPI_OPT_END                    -1

/* Definitions for explicit fallthrough */

#ifndef ACPI_FALLTHROUGH
#define ACPI_FALLTHROUGH do {} while(0)
#endif

#endif				/* __ACTYPES_H__ */
