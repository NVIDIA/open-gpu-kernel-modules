/*******************************************************************************
    Copyright (c) 2013-2023 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#ifndef __UVM_COMMON_H__
#define __UVM_COMMON_H__

#ifdef DEBUG
    #define UVM_IS_DEBUG() 1
#else
    #define UVM_IS_DEBUG() 0
#endif

// NVIDIA_UVM_DEVELOP implies DEBUG, but not vice-versa
// TODO Bug 1773100: Figure out the right distinction between develop and debug
// builds.
#ifdef NVIDIA_UVM_DEVELOP
    #define UVM_IS_DEVELOP() 1
#else
    #define UVM_IS_DEVELOP() 0
#endif

#include "uvm_types.h"
#include "uvm_linux.h"

enum {
    NVIDIA_UVM_PRIMARY_MINOR_NUMBER = 0,
    NVIDIA_UVM_TOOLS_MINOR_NUMBER   = 1,
    // to ensure backward-compatiblity and correct counting, please insert any
    // new minor devices just above the following field:
    NVIDIA_UVM_NUM_MINOR_DEVICES
};

#define UVM_GPU_UUID_TEXT_BUFFER_LENGTH (8+16*2+4+1)

int format_uuid_to_buffer(char *buffer, unsigned bufferLength, const NvProcessorUuid *pGpuUuid);

#define UVM_PRINT_FUNC_PREFIX(func, prefix, fmt, ...) \
    func(prefix "%s:%u %s[pid:%d]" fmt,               \
         kbasename(__FILE__),                         \
         __LINE__,                                    \
         __FUNCTION__,                                \
         current->pid,                                \
         ##__VA_ARGS__)

#define UVM_PRINT_FUNC(func, fmt, ...)  \
    UVM_PRINT_FUNC_PREFIX(func, "", fmt, ##__VA_ARGS__)

// Check whether UVM_{ERR,DBG,INFO)_PRINT* should be enabled
bool uvm_debug_prints_enabled(void);

// A printing helper like UVM_PRINT_FUNC_PREFIX that only prints if
// uvm_debug_prints_enabled() returns true.
#define UVM_PRINT_FUNC_PREFIX_CHECK(func, prefix, fmt, ...)             \
    do {                                                                \
        if (uvm_debug_prints_enabled()) {                               \
            UVM_PRINT_FUNC_PREFIX(func, prefix, fmt, ##__VA_ARGS__);    \
        }                                                               \
    } while (0)

#define UVM_ASSERT_PRINT(fmt, ...) \
    UVM_PRINT_FUNC_PREFIX(printk, KERN_ERR NVIDIA_UVM_PRETTY_PRINTING_PREFIX, " " fmt, ##__VA_ARGS__)

#define UVM_ASSERT_PRINT_RL(fmt, ...) \
    UVM_PRINT_FUNC_PREFIX(printk_ratelimited, KERN_ERR NVIDIA_UVM_PRETTY_PRINTING_PREFIX, " " fmt, ##__VA_ARGS__)

#define UVM_ERR_PRINT(fmt, ...) \
    UVM_PRINT_FUNC_PREFIX_CHECK(printk, KERN_ERR NVIDIA_UVM_PRETTY_PRINTING_PREFIX, " " fmt, ##__VA_ARGS__)

#define UVM_ERR_PRINT_RL(fmt, ...) \
    UVM_PRINT_FUNC_PREFIX_CHECK(printk_ratelimited, KERN_ERR NVIDIA_UVM_PRETTY_PRINTING_PREFIX, " " fmt, ##__VA_ARGS__)

#define UVM_DBG_PRINT(fmt, ...) \
    UVM_PRINT_FUNC_PREFIX_CHECK(printk, KERN_DEBUG NVIDIA_UVM_PRETTY_PRINTING_PREFIX, " " fmt, ##__VA_ARGS__)

#define UVM_DBG_PRINT_RL(fmt, ...)                              \
    UVM_PRINT_FUNC_PREFIX_CHECK(printk_ratelimited, KERN_DEBUG NVIDIA_UVM_PRETTY_PRINTING_PREFIX, " " fmt, ##__VA_ARGS__)

#define UVM_INFO_PRINT(fmt, ...) \
    UVM_PRINT_FUNC_PREFIX_CHECK(printk, KERN_INFO NVIDIA_UVM_PRETTY_PRINTING_PREFIX, " " fmt, ##__VA_ARGS__)

//
// Please see the documentation of format_uuid_to_buffer, for details on what
// this routine prints for you.
//
#define UVM_DBG_PRINT_UUID(msg, uuidPtr)                                \
    do {                                                                \
        char uuidBuffer[UVM_GPU_UUID_TEXT_BUFFER_LENGTH];               \
        format_uuid_to_buffer(uuidBuffer, sizeof(uuidBuffer), uuidPtr); \
        UVM_DBG_PRINT("%s: %s\n", msg, uuidBuffer);                     \
    } while (0)

#define UVM_ERR_PRINT_NV_STATUS(msg, rmStatus, ...)                        \
    UVM_ERR_PRINT("ERROR: %s : " msg "\n", nvstatusToString(rmStatus), ##__VA_ARGS__)

#define UVM_ERR_PRINT_UUID(msg, uuidPtr, ...)                              \
    do {                                                                   \
        char uuidBuffer[UVM_GPU_UUID_TEXT_BUFFER_LENGTH];                  \
        format_uuid_to_buffer(uuidBuffer, sizeof(uuidBuffer), uuidPtr);    \
        UVM_ERR_PRINT("ERROR: %s : " msg "\n", uuidBuffer, ##__VA_ARGS__); \
    } while (0)

#define UVM_PANIC()             UVM_PRINT_FUNC(panic, "\n")
#define UVM_PANIC_MSG(fmt, ...) UVM_PRINT_FUNC(panic, ": " fmt, ##__VA_ARGS__)

#define UVM_PANIC_ON_MSG(cond, fmt, ...)        \
    do {                                        \
        if (unlikely(cond))                     \
            UVM_PANIC_MSG(fmt, ##__VA_ARGS__);  \
    } while (0)

#define UVM_PANIC_ON(cond)  UVM_PANIC_ON_MSG(cond, "failed cond %s\n", #cond)

// expr may include function calls. Use sizeof to prevent it from being
// evaluated while also preventing unused variable warnings. sizeof() can't be
// used on a bitfield however, so use ! to force the expression to evaluate as
// an int.
#define UVM_IGNORE_EXPR(expr) ((void)sizeof(!(expr)))

#define UVM_IGNORE_EXPR2(expr1, expr2)  \
    do {                                \
        UVM_IGNORE_EXPR(expr1);         \
        UVM_IGNORE_EXPR(expr2);         \
    } while (0)

// NO-OP function to break on_uvm_test_fail - that is just to set a breakpoint
void on_uvm_test_fail(void);

// NO-OP function to break on_uvm_assert - that is just to set a breakpoint
// Unlike on_uvm_test_fail it provides 'panic' coverity semantics
void on_uvm_assert(void);

#define _UVM_ASSERT_MSG(expr, cond, fmt, ...)                                                   \
    do {                                                                                        \
        if (unlikely(!(expr))) {                                                                \
            UVM_ASSERT_PRINT("Assert failed, condition %s not true" fmt, cond, ##__VA_ARGS__);  \
            dump_stack();                                                                       \
            on_uvm_assert();                                                                    \
        }                                                                                       \
    } while (0)

// Prevent function calls in expr and the print argument list from being
// evaluated.
#define UVM_ASSERT_MSG_IGNORE(expr, fmt, ...)   \
    do {                                        \
        UVM_IGNORE_EXPR(expr);                  \
        UVM_NO_PRINT(fmt, ##__VA_ARGS__);       \
    } while (0)

// UVM_ASSERT and UVM_ASSERT_MSG are only enabled on non-release and Coverity builds
#if UVM_IS_DEBUG() || defined __COVERITY__
    #define UVM_ASSERT_MSG(expr, fmt, ...)  _UVM_ASSERT_MSG(expr, #expr, ": " fmt, ##__VA_ARGS__)
    #define UVM_ASSERT(expr)                _UVM_ASSERT_MSG(expr, #expr, "\n")
#else
    #define UVM_ASSERT_MSG(expr, fmt, ...)  UVM_ASSERT_MSG_IGNORE(expr, fmt, ##__VA_ARGS__)
    #define UVM_ASSERT(expr)                UVM_ASSERT_MSG_IGNORE(expr, "\n")
#endif

// UVM_ASSERT_RELEASE and UVM_ASSERT_MSG_RELEASE are always included in the
// build, even on release builds. They are skipped at runtime if
// uvm_release_asserts is 0.

// Whether release asserts are enabled and whether they should dump the stack
// and set the global error.
extern int uvm_release_asserts;
extern int uvm_release_asserts_dump_stack;
extern int uvm_release_asserts_set_global_error;
extern bool uvm_release_asserts_set_global_error_for_tests;

// Given these are enabled for release builds, we need to be more cautious than
// in UVM_ASSERT(). Use a ratelimited print and only dump the stack if a module
// param is enabled.
#define _UVM_ASSERT_MSG_RELEASE(expr, cond, fmt, ...)                                                   \
    do {                                                                                                \
        if (uvm_release_asserts && unlikely(!(expr))) {                                                 \
            UVM_ASSERT_PRINT_RL("Assert failed, condition %s not true" fmt, cond, ##__VA_ARGS__);       \
            if (uvm_release_asserts_set_global_error || uvm_release_asserts_set_global_error_for_tests) \
                uvm_global_set_fatal_error(NV_ERR_INVALID_STATE);                                       \
            if (uvm_release_asserts_dump_stack)                                                         \
                dump_stack();                                                                           \
            on_uvm_assert();                                                                            \
        }                                                                                               \
    } while (0)

#define UVM_ASSERT_MSG_RELEASE(expr, fmt, ...)  _UVM_ASSERT_MSG_RELEASE(expr, #expr, ": " fmt, ##__VA_ARGS__)
#define UVM_ASSERT_RELEASE(expr)                _UVM_ASSERT_MSG_RELEASE(expr, #expr, "\n")

#define UVM_SIZE_1KB (1024ULL)
#define UVM_SIZE_1MB (1024 * UVM_SIZE_1KB)
#define UVM_SIZE_1GB (1024 * UVM_SIZE_1MB)
#define UVM_SIZE_1TB (1024 * UVM_SIZE_1GB)
#define UVM_SIZE_1PB (1024 * UVM_SIZE_1TB)

#define UVM_ALIGN_DOWN(x, a) ({         \
        typeof(x) _a = a;               \
        UVM_ASSERT(is_power_of_2(_a));  \
        (x) & ~(_a - 1);                \
    })

#define UVM_ALIGN_UP(x, a) ({           \
        typeof(x) _a = a;               \
        UVM_ASSERT(is_power_of_2(_a));  \
        ((x) + _a - 1) & ~(_a - 1);     \
    })

#define UVM_PAGE_ALIGN_UP(value) UVM_ALIGN_UP(value, PAGE_SIZE)
#define UVM_PAGE_ALIGN_DOWN(value) UVM_ALIGN_DOWN(value, PAGE_SIZE)

// These macros provide a convenient way to string-ify enum values.
#define UVM_ENUM_STRING_CASE(value) case value: return #value
#define UVM_ENUM_STRING_DEFAULT() default: return "UNKNOWN"

// Divide by a dynamic value known at runtime to be a power of 2. ilog2 is
// optimized as a single instruction in many processors, whereas integer
// division is always slow.
static inline NvU32 uvm_div_pow2_32(NvU32 numerator, NvU32 denominator_pow2)
{
    UVM_ASSERT(is_power_of_2(denominator_pow2));
    UVM_ASSERT(denominator_pow2);
    return numerator >> ilog2(denominator_pow2);
}

static inline NvU64 uvm_div_pow2_64(NvU64 numerator, NvU64 denominator_pow2)
{
    UVM_ASSERT(is_power_of_2(denominator_pow2));
    UVM_ASSERT(denominator_pow2);
    return numerator >> ilog2(denominator_pow2);
}

#define SUM_FROM_0_TO_N(n) (((n) * ((n) + 1)) / 2)

// Start and end are inclusive
static inline NvBool uvm_ranges_overlap(NvU64 a_start, NvU64 a_end, NvU64 b_start, NvU64 b_end)
{
    // De Morgan's of: !(a_end < b_start || b_end < a_start)
    return a_end >= b_start && b_end >= a_start;
}

static int debug_mode(void)
{
#ifdef DEBUG
    return 1;
#else
    return 0;
#endif
}

static inline void kmem_cache_destroy_safe(struct kmem_cache **ppCache)
{
    if (ppCache)
    {
        if (*ppCache)
            kmem_cache_destroy(*ppCache);

        *ppCache = NULL;
    }
}

typedef struct
{
    NvU64 start_time_ns;
    NvU64 print_time_ns;
} uvm_spin_loop_t;

static inline void uvm_spin_loop_init(uvm_spin_loop_t *spin)
{
    NvU64 curr = NV_GETTIME();
    spin->start_time_ns = curr;
    spin->print_time_ns = curr;
}

// Periodically yields the CPU when not called from interrupt context. Returns
// NV_ERR_TIMEOUT_RETRY if the caller should print a warning that we've been
// waiting too long, and NV_OK otherwise.
NV_STATUS uvm_spin_loop(uvm_spin_loop_t *spin);

static NvU64 uvm_spin_loop_elapsed(const uvm_spin_loop_t *spin)
{
    NvU64 curr = NV_GETTIME();
    return curr - spin->start_time_ns;
}

#define UVM_SPIN_LOOP(__spin) ({                                                        \
    NV_STATUS __status = uvm_spin_loop(__spin);                                         \
    if (__status == NV_ERR_TIMEOUT_RETRY) {                                             \
        UVM_DBG_PRINT("Warning: stuck waiting for %llus\n",                             \
                      uvm_spin_loop_elapsed(__spin) / (1000*1000*1000));                \
                                                                                        \
        if (uvm_debug_prints_enabled())                                                 \
            dump_stack();                                                               \
    }                                                                                   \
    __status;                                                                           \
})

// Execute the loop code while cond is true. Invokes uvm_spin_loop_iter at the
// end of each iteration.
#define UVM_SPIN_WHILE(cond, spin)                                                \
    if (cond)                                                                     \
        for (uvm_spin_loop_init(spin); (cond); UVM_SPIN_LOOP(spin))

//
// Documentation for the internal routines listed below may be found in the
// implementation file(s).
//
NV_STATUS errno_to_nv_status(int errnoCode);
int nv_status_to_errno(NV_STATUS status);
unsigned uvm_get_stale_process_id(void);
unsigned uvm_get_stale_thread_id(void);

extern int uvm_enable_builtin_tests;

static inline void uvm_init_character_device(struct cdev *cdev, const struct file_operations *fops)
{
    cdev_init(cdev, fops);
    cdev->owner = THIS_MODULE;
}

typedef struct
{
    int rm_control_fd;
    NvHandle user_client;
    NvHandle user_object;
} uvm_rm_user_object_t;

typedef enum
{
    UVM_FD_UNINITIALIZED,
    UVM_FD_INITIALIZING,
    UVM_FD_VA_SPACE,
    UVM_FD_MM,
    UVM_FD_COUNT
} uvm_fd_type_t;

// This should be large enough to fit the valid values from uvm_fd_type_t above.
// Note we can't use order_base_2(UVM_FD_COUNT) to define this because our code
// coverage tool fails due when the preprocessor expands that to a huge mess of
// ternary operators.
#define UVM_FD_TYPE_BITS 2
#define UVM_FD_TYPE_MASK ((1UL << UVM_FD_TYPE_BITS) - 1)

// Macro used to compare two values for types that support less than operator.
// It returns -1 if a < b, 1 if a > b and 0 if a == 0
#define UVM_CMP_DEFAULT(a,b)              \
({                                        \
    typeof(a) _a = a;                     \
    typeof(b) _b = b;                     \
    int __ret;                            \
    BUILD_BUG_ON(sizeof(a) != sizeof(b)); \
    if (_a < _b)                          \
        __ret = -1;                       \
    else if (_b < _a)                     \
        __ret = 1;                        \
    else                                  \
        __ret = 0;                        \
                                          \
    __ret;                                \
})

// Returns whether the input file was opened against the UVM character device
// file. A NULL input returns false.
bool uvm_file_is_nvidia_uvm(struct file *filp);

// Returns the type of data filp->private_data contains to and if ptr_val !=
// NULL returns the value of the pointer.
uvm_fd_type_t uvm_fd_type(struct file *filp, void **ptr_val);

// Returns the pointer stored in filp->private_data if the type
// matches, otherwise returns NULL.
void *uvm_fd_get_type(struct file *filp, uvm_fd_type_t type);

// Reads the first word in the supplied struct page.
static inline void uvm_touch_page(struct page *page)
{
    char *mapping;

    UVM_ASSERT(page);

    mapping = (char *) kmap(page);
    (void)UVM_READ_ONCE(*mapping);
    kunmap(page);
}

// Return true if the VMA is one used by UVM managed allocations.
bool uvm_vma_is_managed(struct vm_area_struct *vma);

static bool uvm_platform_uses_canonical_form_address(void)
{
    if (NVCPU_IS_PPC64LE)
        return false;

    return true;
}

// Similar to the GPU MMU HAL num_va_bits(), it returns the CPU's num_va_bits().
static NvU32 uvm_cpu_num_va_bits(void)
{
    return fls64(TASK_SIZE - 1) + 1;
}

// Return the unaddressable range in a num_va_bits-wide VA space, [first, outer)
static void uvm_get_unaddressable_range(NvU32 num_va_bits, NvU64 *first, NvU64 *outer)
{
    UVM_ASSERT(num_va_bits < 64);
    UVM_ASSERT(first);
    UVM_ASSERT(outer);

    if (uvm_platform_uses_canonical_form_address()) {
        *first = 1ULL << (num_va_bits - 1);
        *outer = (NvU64)((NvS64)(1ULL << 63) >> (64 - num_va_bits));
    }
    else {
        *first = 1ULL << num_va_bits;
        *outer = ~0Ull;
    }
}

static void uvm_cpu_get_unaddressable_range(NvU64 *first, NvU64 *outer)
{
    return uvm_get_unaddressable_range(uvm_cpu_num_va_bits(), first, outer);
}

#endif /* __UVM_COMMON_H__ */
