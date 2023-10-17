/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#define  __NO_VERSION__

#include "os-interface.h"
#include "nv-linux.h"

#include "nv-time.h"

#include <linux/mmzone.h>
#include <linux/numa.h>

#include <linux/pid.h>

extern char *NVreg_TemporaryFilePath;

#define MAX_ERROR_STRING 512
static char nv_error_string[MAX_ERROR_STRING];
nv_spinlock_t nv_error_string_lock;

extern nv_linux_state_t nv_ctl_device;

extern nv_kthread_q_t nv_kthread_q;

NvU32 os_page_size  = PAGE_SIZE;
NvU64 os_page_mask  = NV_PAGE_MASK;
NvU8  os_page_shift = PAGE_SHIFT;
NvBool os_cc_enabled = 0;
NvBool os_cc_tdx_enabled = 0;

#if defined(CONFIG_DMA_SHARED_BUFFER)
NvBool os_dma_buf_enabled = NV_TRUE;
#else
NvBool os_dma_buf_enabled = NV_FALSE;
#endif // CONFIG_DMA_SHARED_BUFFER

void NV_API_CALL os_disable_console_access(void)
{
    console_lock();
}

void NV_API_CALL os_enable_console_access(void)
{
    console_unlock();
}

typedef struct semaphore os_mutex_t;

//
// os_alloc_mutex - Allocate the RM mutex
//
//  ppMutex - filled in with pointer to opaque structure to mutex data type
//
NV_STATUS NV_API_CALL os_alloc_mutex
(
    void **ppMutex
)
{
    NV_STATUS rmStatus;
    os_mutex_t *os_mutex;

    rmStatus = os_alloc_mem(ppMutex, sizeof(os_mutex_t));
    if (rmStatus != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate mutex!\n");
        return rmStatus;
    }
    os_mutex = (os_mutex_t *)*ppMutex;
    NV_INIT_MUTEX(os_mutex);

    return NV_OK;
}

//
// os_free_mutex - Free resources associated with mutex allocated
//                via os_alloc_mutex above.
//
//  pMutex - Pointer to opaque structure to mutex data type
//
void NV_API_CALL os_free_mutex
(
    void  *pMutex
)
{
    os_mutex_t *os_mutex = (os_mutex_t *)pMutex;

    if (os_mutex != NULL)
    {
        os_free_mem(pMutex);
    }
}

//
//  pMutex - Pointer to opaque structure to mutex data type
//

NV_STATUS NV_API_CALL os_acquire_mutex
(
    void  *pMutex
)
{
    os_mutex_t *os_mutex = (os_mutex_t *)pMutex;

    if (!NV_MAY_SLEEP())
    {
        return NV_ERR_INVALID_REQUEST;
    }
    down(os_mutex);

    return NV_OK;
}

NV_STATUS NV_API_CALL os_cond_acquire_mutex
(
    void * pMutex
)
{
    os_mutex_t *os_mutex = (os_mutex_t *)pMutex;
    if (!NV_MAY_SLEEP())
    {
        return NV_ERR_INVALID_REQUEST;
    }

    if (down_trylock(os_mutex))
    {
        return NV_ERR_TIMEOUT_RETRY;
    }

    return NV_OK;
}


void NV_API_CALL os_release_mutex
(
    void *pMutex
)
{
    os_mutex_t *os_mutex = (os_mutex_t *)pMutex;
    up(os_mutex);
}

typedef struct semaphore os_semaphore_t;


void* NV_API_CALL os_alloc_semaphore
(
    NvU32 initialValue
)
{
    NV_STATUS rmStatus;
    os_semaphore_t *os_sema;

    rmStatus = os_alloc_mem((void *)&os_sema, sizeof(os_semaphore_t));
    if (rmStatus != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate semaphore!\n");
        return NULL;
    }

    sema_init(os_sema, initialValue);

    return (void *)os_sema;
}

void NV_API_CALL os_free_semaphore
(
    void *pSema
)
{
    os_semaphore_t *os_sema = (os_semaphore_t *)pSema;

    os_free_mem(os_sema);
}

NV_STATUS NV_API_CALL os_acquire_semaphore
(
    void *pSema
)
{
    os_semaphore_t *os_sema = (os_semaphore_t *)pSema;

    if (!NV_MAY_SLEEP())
    {
        return NV_ERR_INVALID_REQUEST;
    }
    down(os_sema);
    return NV_OK;
}

NV_STATUS NV_API_CALL os_cond_acquire_semaphore
(
    void * pSema
)
{
    os_semaphore_t *os_sema = (os_semaphore_t *)pSema;
    //
    // NOTE: down_trylock() is safe to call from IRQ, se we don't need an
    // NV_MAY_SLEEP() check here. We do check it in os_cond_acquire_mutex(),
    // even though it is also calling down_trylock(), since that keeps it
    // in line with the kernel's 'struct mutex' API.
    //
    if (down_trylock(os_sema))
    {
        return NV_ERR_TIMEOUT_RETRY;
    }

    return NV_OK;
}

NV_STATUS NV_API_CALL os_release_semaphore
(
    void *pSema
)
{
    os_semaphore_t *os_sema = (os_semaphore_t *)pSema;
    up(os_sema);
    return NV_OK;
}

typedef struct rw_semaphore os_rwlock_t;

void* NV_API_CALL os_alloc_rwlock(void)
{
    os_rwlock_t *os_rwlock = NULL;

    NV_STATUS rmStatus = os_alloc_mem((void *)&os_rwlock, sizeof(os_rwlock_t));
    if (rmStatus != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate rw_semaphore!\n");
        return NULL;
    }

    init_rwsem(os_rwlock);

    return os_rwlock;
}

void NV_API_CALL os_free_rwlock(void *pRwLock)
{
    os_rwlock_t *os_rwlock = (os_rwlock_t *)pRwLock;
    os_free_mem(os_rwlock);
}

NV_STATUS NV_API_CALL os_acquire_rwlock_read(void *pRwLock)
{
    os_rwlock_t *os_rwlock = (os_rwlock_t *)pRwLock;

    if (!NV_MAY_SLEEP())
    {
        return NV_ERR_INVALID_REQUEST;
    }
    down_read(os_rwlock);
    return NV_OK;
}

NV_STATUS NV_API_CALL os_acquire_rwlock_write(void *pRwLock)
{
    os_rwlock_t *os_rwlock = (os_rwlock_t *)pRwLock;

    if (!NV_MAY_SLEEP())
    {
        return NV_ERR_INVALID_REQUEST;
    }
    down_write(os_rwlock);
    return NV_OK;
}

NV_STATUS NV_API_CALL os_cond_acquire_rwlock_read(void *pRwLock)
{
    os_rwlock_t *os_rwlock = (os_rwlock_t *)pRwLock;

    if (down_read_trylock(os_rwlock))
    {
        return NV_ERR_TIMEOUT_RETRY;
    }

    return NV_OK;
}

NV_STATUS NV_API_CALL os_cond_acquire_rwlock_write(void *pRwLock)
{
    os_rwlock_t *os_rwlock = (os_rwlock_t *)pRwLock;

    if (down_write_trylock(os_rwlock))
    {
        return NV_ERR_TIMEOUT_RETRY;
    }

    return NV_OK;
}

void NV_API_CALL os_release_rwlock_read(void *pRwLock)
{
    os_rwlock_t *os_rwlock = (os_rwlock_t *)pRwLock;
    up_read(os_rwlock);
}

void NV_API_CALL os_release_rwlock_write(void *pRwLock)
{
    os_rwlock_t *os_rwlock = (os_rwlock_t *)pRwLock;
    up_write(os_rwlock);
}

NvBool NV_API_CALL os_semaphore_may_sleep(void)
{
    return NV_MAY_SLEEP();
}

NvBool NV_API_CALL os_is_isr(void)
{
    return (in_irq());
}

// return TRUE if the caller is the super-user
NvBool NV_API_CALL os_is_administrator(void)
{
    return NV_IS_SUSER();
}

NvBool NV_API_CALL os_allow_priority_override(void)
{
    return capable(CAP_SYS_NICE);
}

NvU64 NV_API_CALL os_get_num_phys_pages(void)
{
    return (NvU64)NV_NUM_PHYSPAGES;
}

char* NV_API_CALL os_string_copy(
    char *dst,
    const char *src
)
{
    return strcpy(dst, src);
}

NvU32 NV_API_CALL os_string_length(
    const char* str
)
{
    return strlen(str);
}

NvU32 NV_API_CALL os_strtoul(const char *str, char **endp, NvU32 base)
{
    return (NvU32)simple_strtoul(str, endp, base);
}

NvS32 NV_API_CALL os_string_compare(const char *str1, const char *str2)
{
    return strcmp(str1, str2);
}

void *os_mem_copy_custom(
    void       *dstPtr,
    const void *srcPtr,
    NvU32       length
)
{
    void *ret = dstPtr;
    NvU32 dwords, bytes = length;
    NvU8 *dst = dstPtr;
    const NvU8 *src = srcPtr;

    if ((length >= 128) &&
        (((NvUPtr)dst & 3) == 0) & (((NvUPtr)src & 3) == 0))
    {
        dwords = (length / sizeof(NvU32));
        bytes = (length % sizeof(NvU32));

        while (dwords != 0)
        {
            *(NvU32 *)dst = *(const NvU32 *)src;
            dst += sizeof(NvU32);
            src += sizeof(NvU32);
            dwords--;
        }
    }

    while (bytes != 0)
    {
        *dst = *src;
        dst++;
        src++;
        bytes--;
    }

    return ret;
}

void *NV_API_CALL os_mem_copy(
    void       *dst,
    const void *src,
    NvU32       length
)
{
#if defined(NVCPU_AARCH64)
    /*
     * TODO: Remove once memset/memcpy restructure is complete
     *
     * When performing memcpy for memory mapped as device, memcpy_[to/from]io
     * must be used. WAR to check the source and destination to determine the
     * correct memcpy_io to use.
     *
     * This WAR is limited to just aarch64 for now because the address range used
     * to map ioremap and vmalloc is different on ppc64le, and is_vmalloc_addr()
     * does not correctly handle this. is_ioremap_addr() is needed instead. This
     * will have to be addressed when reorganizing RM to use the new memset model.
     */
    if (is_vmalloc_addr(dst) && !is_vmalloc_addr(src))
    {
        memcpy_toio(dst, src, length);
        return dst;
    }
    else if (!is_vmalloc_addr(dst) && is_vmalloc_addr(src))
    {
        memcpy_fromio(dst, src, length);
        return dst;
    }
    else if (is_vmalloc_addr(dst) && is_vmalloc_addr(src))
    {
        return os_mem_copy_custom(dst, src, length);
    }
    else
#endif
    {
#if defined(CONFIG_CC_OPTIMIZE_FOR_SIZE)
        /*
         * When the kernel is configured with CC_OPTIMIZE_FOR_SIZE=y, Kbuild uses
         * -Os universally. With -Os, GCC will aggressively inline builtins, even
         * if -fno-builtin is specified, including memcpy with a tiny byte-copy
         * loop on x86 (rep movsb). This is horrible for performance - a strict
         * dword copy is much faster - so when we detect this case, just provide
         * our own implementation.
         */
        return os_mem_copy_custom(dst, src, length);
#else
        /*
         * Generally speaking, the kernel-provided memcpy will be the fastest,
         * (optimized much better for the target architecture than the above
         * loop), so we want to use that whenever we can get to it.
         */
        return memcpy(dst, src, length);
#endif
    }
}

NV_STATUS NV_API_CALL os_memcpy_from_user(
    void       *to,
    const void *from,
    NvU32       n
)
{
    return (NV_COPY_FROM_USER(to, from, n) ? NV_ERR_INVALID_ADDRESS : NV_OK);
}

NV_STATUS NV_API_CALL os_memcpy_to_user(
    void       *to,
    const void *from,
    NvU32       n
)
{
    return (NV_COPY_TO_USER(to, from, n) ? NV_ERR_INVALID_ADDRESS : NV_OK);
}

void* NV_API_CALL os_mem_set(
    void  *dst,
    NvU8   c,
    NvU32  length
)
{
#if defined(NVCPU_AARCH64)
    /*
     * TODO: Remove once memset/memcpy restructure is complete
     *
     * WAR to check the destination to determine if the memory is of type Device
     * or Normal, and use the correct memset.
     *
     * This WAR is limited to just aarch64 for now because the address range used
     * to map ioremap and vmalloc is different on ppc64le, and is_vmalloc_addr()
     * does not correctly handle this. is_ioremap_addr() is needed instead. This
     * will have to be addressed when reorganizing RM to use the new memset model.
     */
    if (is_vmalloc_addr(dst))
    {
        memset_io(dst, (int)c, length);
        return dst;
    }
    else
#endif
       return memset(dst, (int)c, length);
}

NvS32 NV_API_CALL os_mem_cmp(
    const NvU8 *buf0,
    const NvU8* buf1,
    NvU32 length
)
{
    return memcmp(buf0, buf1, length);
}


/*
 * Operating System Memory Functions
 *
 * There are 2 interesting aspects of resource manager memory allocations
 * that need special consideration on Linux:
 *
 * 1. They are typically very large, (e.g. single allocations of 164KB)
 *
 * 2. The resource manager assumes that it can safely allocate memory in
 *    interrupt handlers.
 *
 * The first requires that we call vmalloc, the second kmalloc. We decide
 * which one to use at run time, based on the size of the request and the
 * context. Allocations larger than 128KB require vmalloc, in the context
 * of an ISR they fail.
 */

#if defined(NV_VGX_HYPER)
/*
 * Citrix Hypervisor-8.0 Dom0 sysmem ends up getting fragmented because
 * of which high-order kmalloc allocations fail. We try to avoid it by
 * requesting allocations not larger than 8K.
 *
 * KVM will be affected low memory pressure situation a lot,
 * particularly if hugetlbfs hugepages are being used. Hence, 8K applies
 * here too.
 */
#define KMALLOC_LIMIT 8192
#else
#define KMALLOC_LIMIT 131072
#endif

#define VMALLOC_ALLOCATION_SIZE_FLAG (1 << 0)

NV_STATUS NV_API_CALL os_alloc_mem(
    void **address,
    NvU64 size
)
{
    NvU64 original_size = size;
    unsigned long alloc_size;

    if (address == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    *address = NULL;
    NV_MEM_TRACKING_PAD_SIZE(size);

    // check for integer overflow on size
    if (size < original_size)
        return NV_ERR_INVALID_ARGUMENT;

    //
    // NV_KMALLOC, nv_vmalloc take an input of 4 bytes in x86. To avoid
    // truncation and wrong allocation, below check is required.
    //
    alloc_size = size;

    if (alloc_size != size)
        return NV_ERR_INVALID_PARAMETER;

    if (!NV_MAY_SLEEP())
    {
        if (alloc_size <= KMALLOC_LIMIT)
            NV_KMALLOC_ATOMIC(*address, alloc_size);
    }
    else
    {
        if (alloc_size <= KMALLOC_LIMIT)
        {
            NV_KMALLOC_NO_OOM(*address, alloc_size);
        }
        if (*address == NULL)
        {
            *address = nv_vmalloc(alloc_size);
            alloc_size |= VMALLOC_ALLOCATION_SIZE_FLAG;
        }
    }

    NV_MEM_TRACKING_HIDE_SIZE(address, alloc_size);

    return ((*address != NULL) ? NV_OK : NV_ERR_NO_MEMORY);
}

void NV_API_CALL os_free_mem(void *address)
{
    NvU64 size;

    NV_MEM_TRACKING_RETRIEVE_SIZE(address, size);

    if (size & VMALLOC_ALLOCATION_SIZE_FLAG)
    {
        size &= ~VMALLOC_ALLOCATION_SIZE_FLAG;
        nv_vfree(address, size);
    }
    else
        NV_KFREE(address, size);
}


/*****************************************************************************
*
*   Name: osGetCurrentTime
*
*****************************************************************************/

NV_STATUS NV_API_CALL os_get_current_time(
    NvU32 *seconds,
    NvU32 *useconds
)
{
    struct timespec64 tm;

    ktime_get_real_ts64(&tm);

    *seconds = tm.tv_sec;
    *useconds = tm.tv_nsec / NSEC_PER_USEC;

    return NV_OK;
}

//
// Get the High resolution tick count of the system uptime
//
NvU64 NV_API_CALL os_get_current_tick_hr(void)
{
    struct timespec64 tm;
    ktime_get_raw_ts64(&tm);
    return (NvU64) timespec64_to_ns(&tm);
}

#if BITS_PER_LONG >= 64

NvU64 NV_API_CALL os_get_current_tick(void)
{
#if defined(NV_JIFFIES_TO_TIMESPEC_PRESENT)
    struct timespec ts;
    jiffies_to_timespec(jiffies, &ts);
    return (NvU64) timespec_to_ns(&ts);
#else
    struct timespec64 ts;
    jiffies_to_timespec64(jiffies, &ts);
    return (NvU64) timespec64_to_ns(&ts);
#endif
}

NvU64 NV_API_CALL os_get_tick_resolution(void)
{
    return (NvU64)jiffies_to_usecs(1) * NSEC_PER_USEC;
}

#else

NvU64 NV_API_CALL os_get_current_tick(void)
{
    /*
     * 'jiffies' overflows regularly on 32-bit builds (unsigned long is 4 bytes
     * instead of 8 bytes), so it's unwise to build a tick counter on it, since
     * the rest of the Resman assumes the 'tick' returned from this function is
     * monotonically increasing and never overflows.
     *
     * Instead, use the previous implementation that we've lived with since the
     * beginning, which uses system clock time to calculate the tick. This is
     * subject to problems if the system clock time changes dramatically
     * (more than a second or so) while the Resman is actively tracking a
     * timeout.
     */
    NvU32 seconds, useconds;

    (void) os_get_current_time(&seconds, &useconds);

    return ((NvU64)seconds * NSEC_PER_SEC +
                 (NvU64)useconds * NSEC_PER_USEC);
}

NvU64 NV_API_CALL os_get_tick_resolution(void)
{
    /*
     * os_get_current_tick() uses os_get_current_time(), which has
     * microsecond resolution.
     */
    return 1000ULL;
}

#endif

//---------------------------------------------------------------------------
//
//  Misc services.
//
//---------------------------------------------------------------------------

NV_STATUS NV_API_CALL os_delay_us(NvU32 MicroSeconds)
{
    return nv_sleep_us(MicroSeconds);
}

NV_STATUS NV_API_CALL os_delay(NvU32 MilliSeconds)
{
    return nv_sleep_ms(MilliSeconds);
}

NvU64 NV_API_CALL os_get_cpu_frequency(void)
{
    NvU64 cpu_hz = 0;
#if defined(CONFIG_CPU_FREQ)
    cpu_hz = (cpufreq_get(0) * 1000);
#elif defined(NVCPU_X86_64)
    NvU64 tsc[2];

    tsc[0] = nv_rdtsc();
    mdelay(250);
    tsc[1] = nv_rdtsc();

    cpu_hz = ((tsc[1] - tsc[0]) * 4);
#endif
    return cpu_hz;
}

NvU32 NV_API_CALL os_get_current_process(void)
{
    return NV_GET_CURRENT_PROCESS();
}

void NV_API_CALL os_get_current_process_name(char *buf, NvU32 len)
{
    task_lock(current);
    strncpy(buf, current->comm, len - 1);
    buf[len - 1] = '\0';
    task_unlock(current);
}

NV_STATUS NV_API_CALL os_get_current_thread(NvU64 *threadId)
{
    if (in_interrupt())
        *threadId = 0;
    else
        *threadId = (NvU64) current->pid;

    return NV_OK;
}

/*******************************************************************************/
/*                                                                             */
/* Debug and logging utilities follow                                          */
/*                                                                             */
/*******************************************************************************/

// The current debug display level (default to maximum debug level)
NvU32 cur_debuglevel = 0xffffffff;

/*
 * The binary core of RM (nv-kernel.o) calls both out_string, and nv_printf.
 */
inline void NV_API_CALL out_string(const char *str)
{
    printk("%s", str);
}

#define NV_PRINT_LOCAL_BUFF_LEN_MAX      530

/*
 * nv_printf() prints to the kernel log for the driver.
 * Returns the number of characters written.
 */
int NV_API_CALL nv_printf(NvU32 debuglevel, const char *printf_format, ...)
{
    va_list arglist;
    int chars_written = 0;
    NvBool bForced = (NV_DBG_FORCE_LEVEL(debuglevel) == debuglevel);
    debuglevel = debuglevel & 0xff;

    // This function is protected by the "_nv_dbg_lock" lock, so it is still
    // thread-safe to store the print buffer in a static variable, thus
    // avoiding a problem with kernel stack size.
    static char buff[NV_PRINT_LOCAL_BUFF_LEN_MAX];

    /*
     * Print a message if:
     * 1. Caller indicates that filtering should be skipped, or
     * 2. debuglevel is at least cur_debuglevel for DBG_MODULE_OS (bits 4:5). Support for print
     * modules has been removed with DBG_PRINTF, so this check should be cleaned up.
     */
    if (bForced ||
        (debuglevel >= ((cur_debuglevel >> 4) & 0x3)))
    {
        size_t loglevel_length = 0, format_length = strlen(printf_format);
        size_t length = 0;
        const char *loglevel = "";

        switch (debuglevel)
        {
            case NV_DBG_INFO:       loglevel = KERN_DEBUG; break;
            case NV_DBG_SETUP:      loglevel = KERN_NOTICE; break;
            case NV_DBG_WARNINGS:   loglevel = KERN_WARNING; break;
            case NV_DBG_ERRORS:     loglevel = KERN_ERR; break;
            case NV_DBG_HW_ERRORS:  loglevel = KERN_CRIT; break;
            case NV_DBG_FATAL:      loglevel = KERN_CRIT; break;
        }

        loglevel_length = strlen(loglevel);

        // When printk is called to extend the output of the previous line
        // (i.e. when the previous line did not end in \n), the printk call
        // must contain KERN_CONT.  Older kernels still print the line
        // correctly, but KERN_CONT was technically always required.

        // This means that every call to printk() needs to have a KERN_xxx
        // prefix.  The only way to get this is to rebuild the format string
        // into a new buffer, with a KERN_xxx prefix prepended.

        // Unfortunately, we can't guarantee that two calls to nv_printf()
        // won't be interrupted by a printk from another driver.  So to be
        // safe, we always append KERN_CONT.  It's still technically wrong,
        // but it works.

        // The long-term fix is to modify all NV_PRINTF-ish calls so that the
        // string always contains only one \n (at the end) and NV_PRINTF_EX
        // is deleted.  But that is unlikely to ever happen.

        length = loglevel_length + format_length + sizeof(KERN_CONT);
        if (length < 1)
            return 0;

        // KERN_CONT changed in the 3.6 kernel, so we can't assume its
        // composition or size.
        memcpy(buff, KERN_CONT, sizeof(KERN_CONT) - 1);
        memcpy(buff + sizeof(KERN_CONT) - 1, loglevel, loglevel_length);
        memcpy(buff + sizeof(KERN_CONT) - 1 + loglevel_length, printf_format, length + 1);

        va_start(arglist, printf_format);
        chars_written = vprintk(buff, arglist);
        va_end(arglist);
    }

    return chars_written;
}

NvS32 NV_API_CALL os_snprintf(char *buf, NvU32 size, const char *fmt, ...)
{
    va_list arglist;
    int chars_written;

    va_start(arglist, fmt);
    chars_written = vsnprintf(buf, size, fmt, arglist);
    va_end(arglist);

    return chars_written;
}

NvS32 NV_API_CALL os_vsnprintf(char *buf, NvU32 size, const char *fmt, va_list arglist)
{
    return vsnprintf(buf, size, fmt, arglist);
}

void NV_API_CALL os_log_error(const char *fmt, va_list ap)
{
    unsigned long flags;

    NV_SPIN_LOCK_IRQSAVE(&nv_error_string_lock, flags);

    vsnprintf(nv_error_string, MAX_ERROR_STRING, fmt, ap);
    nv_error_string[MAX_ERROR_STRING - 1] = 0;
    printk(KERN_ERR "%s", nv_error_string);

    NV_SPIN_UNLOCK_IRQRESTORE(&nv_error_string_lock, flags);
}

void NV_API_CALL os_io_write_byte(
    NvU32 address,
    NvU8 value
)
{
    outb(value, address);
}

void NV_API_CALL os_io_write_word(
    NvU32 address,
    NvU16 value
)
{
    outw(value, address);
}

void NV_API_CALL os_io_write_dword(
    NvU32 address,
    NvU32 value
)
{
    outl(value, address);
}

NvU8 NV_API_CALL os_io_read_byte(
    NvU32 address
)
{
    return inb(address);
}

NvU16 NV_API_CALL os_io_read_word(
    NvU32 address
)
{
    return inw(address);
}

NvU32 NV_API_CALL os_io_read_dword(
    NvU32 address
)
{
    return inl(address);
}


static NvBool NV_API_CALL xen_support_fully_virtualized_kernel(void)
{
#if defined(NV_XEN_SUPPORT_FULLY_VIRTUALIZED_KERNEL)
    return (os_is_vgx_hyper());
#endif
    return NV_FALSE;
}

void* NV_API_CALL os_map_kernel_space(
    NvU64 start,
    NvU64 size_bytes,
    NvU32 mode
)
{
    void *vaddr;

    if (!xen_support_fully_virtualized_kernel() && start == 0)
    {
        if (mode != NV_MEMORY_CACHED)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: os_map_kernel_space: won't map address 0x%0llx UC!\n", start);
            return NULL;
        }
        else
            return (void *)PAGE_OFFSET;
    }

    if (!NV_MAY_SLEEP())
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: os_map_kernel_space: can't map 0x%0llx, invalid context!\n", start);
        os_dbg_breakpoint();
        return NULL;
    }

    switch (mode)
    {
        case NV_MEMORY_CACHED:
            vaddr = nv_ioremap_cache(start, size_bytes);
            break;
        case NV_MEMORY_WRITECOMBINED:
            vaddr = rm_disable_iomap_wc() ?
                    nv_ioremap_nocache(start, size_bytes) :
                    nv_ioremap_wc(start, size_bytes);
            break;
        case NV_MEMORY_UNCACHED:
        case NV_MEMORY_DEFAULT:
            vaddr = nv_ioremap_nocache(start, size_bytes);
            break;
        default:
            nv_printf(NV_DBG_ERRORS,
                "NVRM: os_map_kernel_space: unsupported mode!\n");
            return NULL;
    }

    return vaddr;
}

void NV_API_CALL os_unmap_kernel_space(
    void *addr,
    NvU64 size_bytes
)
{
    if (addr == (void *)PAGE_OFFSET)
        return;

    nv_iounmap(addr, size_bytes);
}

// flush the cpu's cache, uni-processor version
NV_STATUS NV_API_CALL os_flush_cpu_cache(void)
{
    CACHE_FLUSH();
    return NV_OK;
}

// flush the cache of all cpus
NV_STATUS NV_API_CALL os_flush_cpu_cache_all(void)
{
#if defined(NVCPU_AARCH64)
    CACHE_FLUSH_ALL();
    return NV_OK;
#endif
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL os_flush_user_cache(void)
{
#if defined(NVCPU_AARCH64)
    if (!NV_MAY_SLEEP())
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // The Linux kernel does not export an interface for flushing a range,
    // although it is possible. For now, just flush the entire cache to be
    // safe.
    //
    CACHE_FLUSH_ALL();
    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

void NV_API_CALL os_flush_cpu_write_combine_buffer(void)
{
    WRITE_COMBINE_FLUSH();
}

// override initial debug level from registry
void NV_API_CALL os_dbg_init(void)
{
    NvU32 new_debuglevel;
    nvidia_stack_t *sp = NULL;

    NV_SPIN_LOCK_INIT(&nv_error_string_lock);

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return;
    }

    if (NV_OK == rm_read_registry_dword(sp, NULL,
                                        "ResmanDebugLevel",
                                        &new_debuglevel))
    {
        if (new_debuglevel != (NvU32)~0)
            cur_debuglevel = new_debuglevel;
    }

    nv_kmem_cache_free_stack(sp);
}

void NV_API_CALL os_dbg_set_level(NvU32 new_debuglevel)
{
    nv_printf(NV_DBG_SETUP, "NVRM: Changing debuglevel from 0x%x to 0x%x\n",
        cur_debuglevel, new_debuglevel);
    cur_debuglevel = new_debuglevel;
}

NvU64 NV_API_CALL os_get_max_user_va(void)
{
	return TASK_SIZE;
}

NV_STATUS NV_API_CALL os_schedule(void)
{
    if (NV_MAY_SLEEP())
    {
        set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(1);
        return NV_OK;
    }
    else
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: os_schedule: Attempted to yield"
                                 " the CPU while in atomic or interrupt"
                                 " context\n");
        return NV_ERR_ILLEGAL_ACTION;
    }
}

typedef struct {
    nv_kthread_q_item_t item;
    void *data;
} os_queue_data_t;

static void os_execute_work_item(void *_oqd)
{
    os_queue_data_t *oqd = _oqd;
    nvidia_stack_t *sp = NULL;
    void *data = oqd->data;

    NV_KFREE(oqd, sizeof(os_queue_data_t));

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return;
    }

    rm_execute_work_item(sp, data);

    nv_kmem_cache_free_stack(sp);
}

NV_STATUS NV_API_CALL os_queue_work_item(struct os_work_queue *queue, void *data)
{
    os_queue_data_t *oqd;
    nv_kthread_q_t *kthread;

    /* Use the global queue unless a valid queue was provided */
    kthread = queue ? &queue->nvk : &nv_kthread_q;

    /* Make sure the kthread is active */
    if (unlikely(!kthread->q_kthread)) {
        nv_printf(NV_DBG_ERRORS, "NVRM: queue is not enabled\n");
        return NV_ERR_NOT_READY;
    }

    /* Allocate atomically just in case we're called in atomic context. */
    NV_KMALLOC_ATOMIC(oqd, sizeof(os_queue_data_t));
    if (!oqd)
        return NV_ERR_NO_MEMORY;

    nv_kthread_q_item_init(&oqd->item, os_execute_work_item, oqd);
    oqd->data = data;

    nv_kthread_q_schedule_q_item(kthread, &oqd->item);

    return NV_OK;
}

NV_STATUS NV_API_CALL os_flush_work_queue(struct os_work_queue *queue)
{
    nv_kthread_q_t *kthread;

    /* Use the global queue unless a valid queue was provided */
    kthread = queue ? &queue->nvk : &nv_kthread_q;

    if (NV_MAY_SLEEP())
    {
        if (kthread->q_kthread)
            nv_kthread_q_flush(kthread);

        return NV_OK;
    }
    else
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: os_flush_work_queue: attempted to execute passive"
                  "work from an atomic or interrupt context.\n");
        return NV_ERR_ILLEGAL_ACTION;
    }
}

extern NvU32 NVreg_EnableDbgBreakpoint;

void NV_API_CALL os_dbg_breakpoint(void)
{
    if (NVreg_EnableDbgBreakpoint == 0)
    {
        return;
    }

#if defined(CONFIG_X86_REMOTE_DEBUG) || defined(CONFIG_KGDB) || defined(CONFIG_XMON)
  #if defined(NVCPU_X86_64)
    __asm__ __volatile__ ("int $3");
  #elif defined(NVCPU_ARM)
    __asm__ __volatile__ (".word %c0" :: "i" (KGDB_COMPILED_BREAK));
  #elif defined(NVCPU_AARCH64)
    # warning "Need to implement os_dbg_breakpoint() for aarch64"
  #elif defined(NVCPU_PPC64LE)
    __asm__ __volatile__ ("trap");
  #endif // NVCPU_*
#elif defined(CONFIG_KDB)
    KDB_ENTER();
#endif // CONFIG_X86_REMOTE_DEBUG || CONFIG_KGDB || CONFIG_XMON
}

NvU32 NV_API_CALL os_get_cpu_number(void)
{
    NvU32 cpu_id = get_cpu();
    put_cpu();
    return cpu_id;
}

NvU32 NV_API_CALL os_get_cpu_count(void)
{
    return NV_NUM_CPUS();
}

NvBool NV_API_CALL os_pat_supported(void)
{
    return (nv_pat_mode != NV_PAT_MODE_DISABLED);
}

NvBool NV_API_CALL os_is_efi_enabled(void)
{
    return efi_enabled(EFI_BOOT);
}

void NV_API_CALL os_get_screen_info(
    NvU64 *pPhysicalAddress,
    NvU32 *pFbWidth,
    NvU32 *pFbHeight,
    NvU32 *pFbDepth,
    NvU32 *pFbPitch,
    NvU64 consoleBar1Address,
    NvU64 consoleBar2Address
)
{
    *pPhysicalAddress = 0;
    *pFbWidth = *pFbHeight = *pFbDepth = *pFbPitch = 0;

#if defined(CONFIG_FB) && defined(NV_NUM_REGISTERED_FB_PRESENT)
    if (num_registered_fb > 0)
    {
        int i;

        for (i = 0; i < num_registered_fb; i++)
        {
            if (!registered_fb[i])
                continue;

            /* Make sure base address is mapped to GPU BAR */
            if ((registered_fb[i]->fix.smem_start == consoleBar1Address) ||
                (registered_fb[i]->fix.smem_start == consoleBar2Address))
            {
                *pPhysicalAddress = registered_fb[i]->fix.smem_start;
                *pFbWidth = registered_fb[i]->var.xres;
                *pFbHeight = registered_fb[i]->var.yres;
                *pFbDepth = registered_fb[i]->var.bits_per_pixel;
                *pFbPitch = registered_fb[i]->fix.line_length;
                return;
            }
        }
    }
#endif

    /*
     * If the screen info is not found in the registered FBs then fallback
     * to the screen_info structure.
     *
     * The SYSFB_SIMPLEFB option, if enabled, marks VGA/VBE/EFI framebuffers as
     * generic framebuffers so the new generic system-framebuffer drivers can
     * be used instead. DRM_SIMPLEDRM drives the generic system-framebuffers
     * device created by SYSFB_SIMPLEFB.
     *
     * SYSFB_SIMPLEFB registers a dummy framebuffer which does not contain the
     * information required by os_get_screen_info(), therefore you need to
     * fall back onto the screen_info structure.
     */

#if NV_IS_EXPORT_SYMBOL_PRESENT_screen_info
    /*
     * If there is not a framebuffer console, return 0 size.
     *
     * orig_video_isVGA is set to 1 during early Linux kernel
     * initialization, and then will be set to a value, such as
     * VIDEO_TYPE_VLFB or VIDEO_TYPE_EFI if an fbdev console is used.
     */
    if (screen_info.orig_video_isVGA > 1)
    {
        NvU64 physAddr = screen_info.lfb_base;
#if defined(VIDEO_CAPABILITY_64BIT_BASE)
        physAddr |= (NvU64)screen_info.ext_lfb_base << 32;
#endif

        /* Make sure base address is mapped to GPU BAR */
        if ((physAddr == consoleBar1Address) ||
            (physAddr == consoleBar2Address))
        {
            *pPhysicalAddress = physAddr;
            *pFbWidth = screen_info.lfb_width;
            *pFbHeight = screen_info.lfb_height;
            *pFbDepth = screen_info.lfb_depth;
            *pFbPitch = screen_info.lfb_linelength;
        }
    }
#endif
}

void NV_API_CALL os_dump_stack(void)
{
    dump_stack();
}

typedef struct os_spinlock_s
{
    nv_spinlock_t      lock;
    unsigned long      eflags;
} os_spinlock_t;

NV_STATUS NV_API_CALL os_alloc_spinlock(void **ppSpinlock)
{
    NV_STATUS rmStatus;
    os_spinlock_t *os_spinlock;

    rmStatus = os_alloc_mem(ppSpinlock, sizeof(os_spinlock_t));
    if (rmStatus != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate spinlock!\n");
        return rmStatus;
    }

    os_spinlock = (os_spinlock_t *)*ppSpinlock;
    NV_SPIN_LOCK_INIT(&os_spinlock->lock);
    os_spinlock->eflags = 0;
    return NV_OK;
}

void NV_API_CALL os_free_spinlock(void *pSpinlock)
{
    os_free_mem(pSpinlock);
}

NvU64 NV_API_CALL os_acquire_spinlock(void *pSpinlock)
{
    os_spinlock_t *os_spinlock = (os_spinlock_t *)pSpinlock;
    unsigned long eflags;

    NV_SPIN_LOCK_IRQSAVE(&os_spinlock->lock, eflags);
    os_spinlock->eflags = eflags;

#if defined(NVCPU_X86_64)
    eflags &= X86_EFLAGS_IF;
#elif defined(NVCPU_AARCH64)
    eflags &= PSR_I_BIT;
#endif
    return eflags;
}

void NV_API_CALL os_release_spinlock(void *pSpinlock, NvU64 oldIrql)
{
    os_spinlock_t *os_spinlock = (os_spinlock_t *)pSpinlock;
    unsigned long eflags;

    eflags = os_spinlock->eflags;
    os_spinlock->eflags = 0;
    NV_SPIN_UNLOCK_IRQRESTORE(&os_spinlock->lock, eflags);
}

#define NV_KERNEL_RELEASE    ((LINUX_VERSION_CODE >> 16) & 0x0ff)
#define NV_KERNEL_VERSION    ((LINUX_VERSION_CODE >> 8)  & 0x0ff)
#define NV_KERNEL_SUBVERSION ((LINUX_VERSION_CODE)       & 0x0ff)

NV_STATUS NV_API_CALL os_get_version_info(os_version_info * pOsVersionInfo)
{
    NV_STATUS status      = NV_OK;

    pOsVersionInfo->os_major_version = NV_KERNEL_RELEASE;
    pOsVersionInfo->os_minor_version = NV_KERNEL_VERSION;
    pOsVersionInfo->os_build_number  = NV_KERNEL_SUBVERSION;

#if defined(UTS_RELEASE)
    pOsVersionInfo->os_build_version_str = UTS_RELEASE;
#endif

#if defined(UTS_VERSION)
    pOsVersionInfo->os_build_date_plus_str = UTS_VERSION;
#endif

    return status;
}

NvBool NV_API_CALL os_is_xen_dom0(void)
{
#if defined(NV_DOM0_KERNEL_PRESENT)
    return NV_TRUE;
#else
    return NV_FALSE;
#endif
}

NvBool NV_API_CALL os_is_vgx_hyper(void)
{
#if defined(NV_VGX_HYPER)
    return NV_TRUE;
#else
    return NV_FALSE;
#endif
}

NV_STATUS NV_API_CALL os_inject_vgx_msi(NvU16 guestID, NvU64 msiAddr, NvU32 msiData)
{
#if defined(NV_VGX_HYPER) && defined(NV_DOM0_KERNEL_PRESENT) && \
    defined(NV_XEN_IOEMU_INJECT_MSI)
    int rc = 0;
    rc = xen_ioemu_inject_msi(guestID, msiAddr, msiData);
    if (rc)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: %s: can't inject MSI to guest:%d, addr:0x%x, data:0x%x, err:%d\n",
            __FUNCTION__, guestID, msiAddr, msiData, rc);
        return NV_ERR_OPERATING_SYSTEM;
    }
    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

NvBool NV_API_CALL os_is_grid_supported(void)
{
#if defined(NV_GRID_BUILD)
    return NV_TRUE;
#else
    return NV_FALSE;
#endif
}

NvU32 NV_API_CALL os_get_grid_csp_support(void)
{
#if defined(NV_GRID_BUILD_CSP)
    return NV_GRID_BUILD_CSP;
#else
    return 0;
#endif
}

void NV_API_CALL os_bug_check(NvU32 bugCode, const char *bugCodeStr)
{
    panic(bugCodeStr);
}

NV_STATUS NV_API_CALL os_get_euid(NvU32 *pSecToken)
{
    *pSecToken = NV_CURRENT_EUID();
    return NV_OK;
}

#if defined(NVCPU_X86_64) || defined(NVCPU_AARCH64)

static NvBool os_verify_checksum(const NvU8 *pMappedAddr, NvU32 length)
{
    NvU8 sum = 0;
    NvU32 iter = 0;

    for (iter = 0; iter < length; iter++)
        sum += pMappedAddr[iter];

    return sum == 0;
}

#define _VERIFY_SMBIOS3(_pMappedAddr)                        \
        _pMappedAddr &&                                      \
        (os_mem_cmp(_pMappedAddr, "_SM3_", 5) == 0  &&       \
        _pMappedAddr[6] < 32 &&                              \
        _pMappedAddr[6] > 0 &&                               \
        os_verify_checksum(_pMappedAddr, _pMappedAddr[6]))

#define OS_VERIFY_SMBIOS3(pMappedAddr) _VERIFY_SMBIOS3((pMappedAddr))

#define _VERIFY_SMBIOS(_pMappedAddr)                           \
        _pMappedAddr &&                                        \
        (os_mem_cmp(_pMappedAddr, "_SM_", 4) == 0  &&          \
        _pMappedAddr[5] < 32 &&                                \
        _pMappedAddr[5] > 0 &&                                 \
        os_verify_checksum(_pMappedAddr, _pMappedAddr[5]) &&   \
        os_mem_cmp((_pMappedAddr + 16), "_DMI_", 5) == 0  &&   \
        os_verify_checksum((_pMappedAddr + 16), 15))

#define OS_VERIFY_SMBIOS(pMappedAddr) _VERIFY_SMBIOS((pMappedAddr))

#define SMBIOS_LEGACY_BASE 0xF0000
#define SMBIOS_LEGACY_SIZE 0x10000

static NV_STATUS os_get_smbios_header_legacy(NvU64 *pSmbsAddr)
{
#if !defined(NVCPU_X86_64)
    return NV_ERR_NOT_SUPPORTED;
#else
    NV_STATUS status = NV_ERR_OPERATING_SYSTEM;
    NvU8 *pMappedAddr = NULL;
    NvU8 *pIterAddr = NULL;

    pMappedAddr = (NvU8*)os_map_kernel_space(SMBIOS_LEGACY_BASE,
                                             SMBIOS_LEGACY_SIZE,
                                             NV_MEMORY_CACHED);
    if (pMappedAddr == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pIterAddr = pMappedAddr;

    for (; pIterAddr < (pMappedAddr + SMBIOS_LEGACY_SIZE); pIterAddr += 16)
    {
        if (OS_VERIFY_SMBIOS3(pIterAddr))
        {
            *pSmbsAddr = SMBIOS_LEGACY_BASE + (pIterAddr - pMappedAddr);
            status = NV_OK;
            break;
        }

        if (OS_VERIFY_SMBIOS(pIterAddr))
        {
            *pSmbsAddr = SMBIOS_LEGACY_BASE + (pIterAddr - pMappedAddr);
            status = NV_OK;
            break;
        }
    }

    os_unmap_kernel_space(pMappedAddr, SMBIOS_LEGACY_SIZE);

    return status;
#endif
}

// This function is needed only if "efi" is enabled.
#if (defined(NV_LINUX_EFI_H_PRESENT) && defined(CONFIG_EFI))
static NV_STATUS os_verify_smbios_header_uefi(NvU64 smbsAddr)
{
    NV_STATUS status = NV_ERR_OBJECT_NOT_FOUND;
    NvU64 start= 0, offset =0 , size = 32;
    NvU8 *pMappedAddr = NULL, *pBufAddr = NULL;

    start = smbsAddr;
    offset = (start & ~os_page_mask);
    start &= os_page_mask;
    size = ((size + offset + ~os_page_mask) & os_page_mask);

    pBufAddr = (NvU8*)os_map_kernel_space(start,
                                          size,
                                          NV_MEMORY_CACHED);
    if (pBufAddr == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pMappedAddr = pBufAddr + offset;

    if (OS_VERIFY_SMBIOS3(pMappedAddr))
    {
        status = NV_OK;
        goto done;
    }

    if (OS_VERIFY_SMBIOS(pMappedAddr))
    {
        status = NV_OK;
    }

done:
    os_unmap_kernel_space(pBufAddr, size);
    return status;
}
#endif

static NV_STATUS os_get_smbios_header_uefi(NvU64 *pSmbsAddr)
{
    NV_STATUS status = NV_ERR_OPERATING_SYSTEM;

// Make sure that efi.h is present before using "struct efi".
#if (defined(NV_LINUX_EFI_H_PRESENT) && defined(CONFIG_EFI))

// Make sure that efi.h has SMBIOS3_TABLE_GUID present.
#if defined(SMBIOS3_TABLE_GUID)
    if (efi.smbios3 != EFI_INVALID_TABLE_ADDR)
    {
        status = os_verify_smbios_header_uefi(efi.smbios3);
        if (status == NV_OK)
        {
            *pSmbsAddr = efi.smbios3;
            return NV_OK;
        }
    }
#endif

    if (efi.smbios != EFI_INVALID_TABLE_ADDR)
    {
        status = os_verify_smbios_header_uefi(efi.smbios);
        if (status == NV_OK)
        {
            *pSmbsAddr = efi.smbios;
            return NV_OK;
        }
    }
#endif

    return status;
}

#endif // defined(NVCPU_X86_64) || defined(NVCPU_AARCH64)

// The function locates the SMBIOS entry point.
NV_STATUS NV_API_CALL os_get_smbios_header(NvU64 *pSmbsAddr)
{

#if !defined(NVCPU_X86_64) && !defined(NVCPU_AARCH64)
    return NV_ERR_NOT_SUPPORTED;
#else
    NV_STATUS status = NV_OK;

    if (os_is_efi_enabled())
    {
        status = os_get_smbios_header_uefi(pSmbsAddr);
    }
    else
    {
        status = os_get_smbios_header_legacy(pSmbsAddr);
    }

    return status;
#endif
}

NV_STATUS NV_API_CALL os_get_acpi_rsdp_from_uefi
(
    NvU32  *pRsdpAddr
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (pRsdpAddr == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    *pRsdpAddr = 0;

// Make sure that efi.h is present before using "struct efi".
#if (defined(NV_LINUX_EFI_H_PRESENT) && defined(CONFIG_EFI))

    if (efi.acpi20 != EFI_INVALID_TABLE_ADDR)
    {
        *pRsdpAddr = efi.acpi20;
        status = NV_OK;
    }
    else if (efi.acpi != EFI_INVALID_TABLE_ADDR)
    {
        *pRsdpAddr = efi.acpi;
        status = NV_OK;
    }
    else
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: RSDP Not found!\n");
        status = NV_ERR_OPERATING_SYSTEM;
    }
#endif

    return status;
}

void NV_API_CALL os_add_record_for_crashLog(void *pbuffer, NvU32 size)
{
}

void NV_API_CALL os_delete_record_for_crashLog(void *pbuffer)
{
}

#if !defined(NV_VGPU_KVM_BUILD)
NV_STATUS NV_API_CALL os_call_vgpu_vfio(void *pvgpu_vfio_info, NvU32 cmd_type)
{
    return NV_ERR_NOT_SUPPORTED;
}
#endif

NV_STATUS NV_API_CALL os_alloc_pages_node
(
    NvS32  nid,
    NvU32  size,
    NvU32  flag,
    NvU64 *pAddress
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

#if defined(__GFP_THISNODE) && defined(GFP_HIGHUSER_MOVABLE) && \
    defined(__GFP_COMP) && defined(__GFP_NORETRY) && defined(__GFP_NOWARN)
    gfp_t gfp_mask;
    struct page *alloc_addr;
    unsigned int order = get_order(size);

    /*
     * Explanation of flags used:
     *
     * 1. __GFP_THISNODE:           This will make sure the allocation happens
     *                              on the node specified by nid.
     *
     * 2. GFP_HIGHUSER_MOVABLE:     This makes allocations from ZONE_MOVABLE.
     *
     * 3. __GFP_COMP:               This will make allocations with compound
     *                              pages, which is needed in order to use
     *                              vm_insert_page API.
     *
     * 4. __GFP_NORETRY:            Used to avoid the Linux kernel OOM killer.
     *
     * 5. __GFP_NOWARN:             Used to avoid a WARN_ON in the slowpath if
     *                              the requested order is too large (just fail
     *                              instead).
     *
     * 6. (Optional) __GFP_RECLAIM: Used to allow/forbid reclaim.
     *                              This is part of GFP_USER and consequently 
     *                              GFP_HIGHUSER_MOVABLE.
     *
     * Some of these flags are relatively more recent, with the last of them
     * (GFP_HIGHUSER_MOVABLE) having been added with this Linux kernel commit:
     *
     * 2007-07-17 769848c03895b63e5662eb7e4ec8c4866f7d0183
     *
     * Assume that this feature will only be used on kernels that support all
     * of the needed GFP flags.
     */

    gfp_mask = __GFP_THISNODE | GFP_HIGHUSER_MOVABLE | __GFP_COMP |
               __GFP_NORETRY | __GFP_NOWARN;

#if defined(__GFP_RECLAIM)
    if (flag & NV_ALLOC_PAGES_NODE_SKIP_RECLAIM)
    {
        gfp_mask &= ~(__GFP_RECLAIM);
    }
#endif // defined(__GFP_RECLAIM)

    alloc_addr = alloc_pages_node(nid, gfp_mask, order);
    if (alloc_addr == NULL)
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: alloc_pages_node(node = %d, order = %u) failed\n",
            nid, order);
        status = NV_ERR_NO_MEMORY;
    }
    else if (page_to_nid(alloc_addr) != nid)
    {
        //
        // We can hit this case when a Linux kernel bug is not patched.
        // The needed patch is https://patchwork.kernel.org/patch/10427387/
        //
        nv_printf(NV_DBG_ERRORS,
            "NVRM: alloc_pages_node(node = %d, order = %u) wrong node ID.\n",
            nid, order);
        __free_pages(alloc_addr, order);
        status = NV_ERR_NO_MEMORY;
    }
    else
    {
        *pAddress = (NvU64)page_to_phys(alloc_addr);
        status = NV_OK;
    }
#endif // GFP flags

    return status;
}

NV_STATUS NV_API_CALL os_get_page
(
    NvU64 address
)
{
    get_page(NV_GET_PAGE_STRUCT(address));
    return NV_OK;
}

NV_STATUS NV_API_CALL os_put_page
(
    NvU64 address
)
{
    put_page(NV_GET_PAGE_STRUCT(address));
    return NV_OK;
}

NvU32 NV_API_CALL os_get_page_refcount
(
    NvU64 address
)
{
    return NV_PAGE_COUNT(NV_GET_PAGE_STRUCT(address));
}

NvU32 NV_API_CALL os_count_tail_pages
(
    NvU64 address
)
{
    NvU32 order = compound_order(compound_head(NV_GET_PAGE_STRUCT(address)));

    return 1 << order;
}

void NV_API_CALL os_free_pages_phys
(
    NvU64 address,
    NvU32 size
)
{
    __free_pages(NV_GET_PAGE_STRUCT(address), get_order(size));
}

NV_STATUS NV_API_CALL os_numa_memblock_size
(
    NvU64 *memblock_size
)
{
#if NV_IS_EXPORT_SYMBOL_PRESENT_memory_block_size_bytes
    *memblock_size = memory_block_size_bytes();
    return NV_OK;
#endif
    if (nv_ctl_device.numa_memblock_size == 0)
        return NV_ERR_INVALID_STATE;
    *memblock_size = nv_ctl_device.numa_memblock_size;
    return NV_OK;
}

NV_STATUS NV_API_CALL os_open_temporary_file
(
    void **ppFile
)
{
#if NV_FILESYSTEM_ACCESS_AVAILABLE
#if defined(O_TMPFILE)
    struct file *file;
    const char *default_path = "/tmp";
    const int flags = O_TMPFILE | O_LARGEFILE | O_RDWR;
    const char *path = NVreg_TemporaryFilePath;

    /*
     * The filp_open() call below depends on the current task's fs_struct
     * (current->fs), which may already be NULL if this is called during
     * process teardown.
     */
    if (current->fs == NULL)
    {
        return NV_ERR_OPERATING_SYSTEM;
    }

    if (!path)
    {
        path = default_path;
    }

    file = filp_open(path, flags, 0);
    if (IS_ERR(file))
    {
        if ((path != default_path) && (PTR_ERR(file) == -ENOENT))
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: The temporary file path specified via the NVreg_TemporaryFilePath\n"
                      "NVRM: module parameter does not exist. Defaulting to /tmp.\n");

            file = filp_open(default_path, flags, 0);
        }
    }

    if (IS_ERR(file))
    {
        return NV_ERR_OPERATING_SYSTEM;
    }

    *ppFile = (void *)file;

    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

void NV_API_CALL os_close_file
(
    void *pFile
)
{
#if NV_FILESYSTEM_ACCESS_AVAILABLE
    filp_close(pFile, NULL);
#endif
}

#define NV_MAX_NUM_FILE_IO_RETRIES 10

NV_STATUS NV_API_CALL os_write_file
(
    void *pFile,
    NvU8 *pBuffer,
    NvU64 size,
    NvU64 offset
)
{
#if NV_FILESYSTEM_ACCESS_AVAILABLE
    loff_t f_pos = offset;
    ssize_t num_written;
    int num_retries = NV_MAX_NUM_FILE_IO_RETRIES;

retry:
#if defined(NV_KERNEL_WRITE_HAS_POINTER_POS_ARG)
    num_written = kernel_write(pFile, pBuffer, size, &f_pos);
#else
    num_written = kernel_write(pFile, pBuffer, size, f_pos);
#endif
    if (num_written < 0)
    {
        return NV_ERR_OPERATING_SYSTEM;
    }
    else if (num_written < size)
    {
        if (num_written > 0)
        {
            pBuffer += num_written;
            size -= num_written;
        }
        if (--num_retries > 0)
        {
            cond_resched();
            goto retry;
        }
        return NV_ERR_OPERATING_SYSTEM;
    }

    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

NV_STATUS NV_API_CALL os_read_file
(
    void *pFile,
    NvU8 *pBuffer,
    NvU64 size,
    NvU64 offset
)
{
#if NV_FILESYSTEM_ACCESS_AVAILABLE
    loff_t f_pos = offset;
    ssize_t num_read;
    int num_retries = NV_MAX_NUM_FILE_IO_RETRIES;

retry:
#if defined(NV_KERNEL_READ_HAS_POINTER_POS_ARG)
    num_read = kernel_read(pFile, pBuffer, size, &f_pos);
#else
    num_read = kernel_read(pFile, f_pos, pBuffer, size);
#endif
    if (num_read < 0)
    {
        return NV_ERR_OPERATING_SYSTEM;
    }
    else if (num_read < size)
    {
        if (num_read > 0)
        {
            pBuffer += num_read;
            size -= num_read;
        }
        if (--num_retries > 0)
        {
            cond_resched();
            goto retry;
        }
        return NV_ERR_OPERATING_SYSTEM;
    }

    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

NV_STATUS NV_API_CALL os_open_readonly_file
(
    const char  *filename,
    void       **ppFile
)
{
#if NV_FILESYSTEM_ACCESS_AVAILABLE
    struct file *file;

    /*
     * The filp_open() call below depends on the current task's fs_struct
     * (current->fs), which may already be NULL if this is called during
     * process teardown.
     */
    if (current->fs == NULL)
    {
        return NV_ERR_OPERATING_SYSTEM;
    }

    file = filp_open(filename, O_RDONLY, 0);
    if (IS_ERR(file))
    {
        return NV_ERR_OPERATING_SYSTEM;
    }

    *ppFile = (void *)file;

    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

NV_STATUS NV_API_CALL os_open_and_read_file
(
    const char *filename,
    NvU8       *buf,
    NvU64       count
)
{
    void *fileHandle;
    NV_STATUS status;

    status = os_open_readonly_file(filename, &fileHandle);
    if (status != NV_OK)
    {
        return status;
    }

    status = os_read_file(fileHandle, buf, count, 0);

    os_close_file(fileHandle);

    return status;
}

NvBool NV_API_CALL os_is_nvswitch_present(void)
{
    struct pci_device_id nvswitch_pci_table[] = {
        {
            PCI_DEVICE(PCI_VENDOR_ID_NVIDIA, PCI_ANY_ID),
            .class      = PCI_CLASS_BRIDGE_OTHER << 8,
            .class_mask = PCI_ANY_ID
        },
        {0}
    };

    return !!pci_dev_present(nvswitch_pci_table);
}

/*
 * This function may sleep (interruptible).
 */
NV_STATUS NV_API_CALL os_get_random_bytes
(
    NvU8 *bytes,
    NvU16 numBytes
)
{
#if defined NV_WAIT_FOR_RANDOM_BYTES_PRESENT
    if (wait_for_random_bytes() < 0)
        return NV_ERR_NOT_READY;
#endif

    get_random_bytes(bytes, numBytes);
    return NV_OK;
}

NV_STATUS NV_API_CALL os_alloc_wait_queue
(
    os_wait_queue **wq
)
{
    NV_KMALLOC(*wq, sizeof(os_wait_queue));
    if (*wq == NULL)
        return NV_ERR_NO_MEMORY;

    init_completion(&(*wq)->q);

    return NV_OK;
}

void NV_API_CALL os_free_wait_queue
(
    os_wait_queue *wq
)
{
    NV_KFREE(wq, sizeof(os_wait_queue));
}

void NV_API_CALL os_wait_uninterruptible
(
    os_wait_queue *wq
)
{
    wait_for_completion(&wq->q);
}

void NV_API_CALL os_wait_interruptible
(
    os_wait_queue *wq
)
{
    wait_for_completion_interruptible(&wq->q);
}

void NV_API_CALL os_wake_up
(
    os_wait_queue *wq
)
{
    complete_all(&wq->q);
}

nv_cap_t* NV_API_CALL os_nv_cap_init
(
    const char *path
)
{
    return nv_cap_init(path);
}

nv_cap_t* NV_API_CALL os_nv_cap_create_dir_entry
(
    nv_cap_t *parent_cap,
    const char *name,
    int mode
)
{
    return nv_cap_create_dir_entry(parent_cap, name, mode);
}

nv_cap_t* NV_API_CALL os_nv_cap_create_file_entry
(
    nv_cap_t *parent_cap,
    const char *name,
    int mode
)
{
    return nv_cap_create_file_entry(parent_cap, name, mode);
}

void NV_API_CALL os_nv_cap_destroy_entry
(
    nv_cap_t *cap
)
{
    nv_cap_destroy_entry(cap);
}

int NV_API_CALL os_nv_cap_validate_and_dup_fd
(
    const nv_cap_t *cap,
    int fd
)
{
    return nv_cap_validate_and_dup_fd(cap, fd);
}

void NV_API_CALL os_nv_cap_close_fd
(
    int fd
)
{
    nv_cap_close_fd(fd);
}

/*
 * Reads the total memory and free memory of a NUMA node from the kernel.
 */
NV_STATUS NV_API_CALL os_get_numa_node_memory_usage
(
    NvS32 node_id,
    NvU64 *free_memory_bytes,
    NvU64 *total_memory_bytes
)
{
    struct pglist_data *pgdat;
    struct zone *zone;
    NvU32 zone_id;

    if (node_id >= MAX_NUMNODES)
    {
        nv_printf(NV_DBG_ERRORS, "Invalid NUMA node ID\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    pgdat = NODE_DATA(node_id);

    *free_memory_bytes = 0;
    *total_memory_bytes = 0;

    for (zone_id = 0; zone_id < MAX_NR_ZONES; zone_id++)
    {
        zone = &(pgdat->node_zones[zone_id]);
        if (!populated_zone(zone))
            continue;
        *free_memory_bytes += (zone_page_state_snapshot(zone, NR_FREE_PAGES) * PAGE_SIZE);
        *total_memory_bytes += (zone->present_pages * PAGE_SIZE);
    }

    return NV_OK;
}

typedef struct os_numa_gpu_mem_hotplug_notifier_s
{
    NvU64 start_pa;
    NvU64 size;
    nv_pci_info_t pci_info;
    struct notifier_block memory_notifier;
} os_numa_gpu_mem_hotplug_notifier_t;

static int os_numa_verify_gpu_memory_zone(struct notifier_block *nb,
                                          unsigned long action, void *data)
{
    os_numa_gpu_mem_hotplug_notifier_t *notifier = container_of(nb,
        os_numa_gpu_mem_hotplug_notifier_t,
        memory_notifier);
    struct memory_notify *mhp = data;
    NvU64 start_pa = PFN_PHYS(mhp->start_pfn);
    NvU64 size = PFN_PHYS(mhp->nr_pages);

    if (action == MEM_GOING_ONLINE)
    {
        // Check if onlining memory falls in the GPU memory range
        if ((start_pa >= notifier->start_pa) &&
            (start_pa + size) <= (notifier->start_pa + notifier->size))
        {
            /*
             * Verify GPU memory NUMA node has memory only in ZONE_MOVABLE before
             * onlining the memory so that incorrect auto online setting doesn't
             * cause the memory onlined in a zone where kernel allocations
             * could happen, resulting in GPU memory hot unpluggable and requiring
             * system reboot.
             */
            if (page_zonenum((pfn_to_page(mhp->start_pfn))) != ZONE_MOVABLE)
            {
                nv_printf(NV_DBG_ERRORS, "NVRM: Failing GPU memory onlining as the onlining zone "
                          "is not movable. pa: 0x%llx size: 0x%llx\n"
                          "NVRM: The NVIDIA GPU %04x:%02x:%02x.%x installed in the system\n"
                          "NVRM: requires auto onlining mode online_movable enabled in\n"
                          "NVRM: /sys/devices/system/memory/auto_online_blocks\n",
                          start_pa, size, notifier->pci_info.domain, notifier->pci_info.bus,
                          notifier->pci_info.slot, notifier->pci_info.function);
                return NOTIFY_BAD;
            }
        }
    }
    return NOTIFY_OK;
}

NV_STATUS NV_API_CALL os_numa_add_gpu_memory
(
    void *handle,
    NvU64 offset,
    NvU64 size,
    NvU32 *nodeId
)
{
#if defined(NV_ADD_MEMORY_DRIVER_MANAGED_PRESENT)
    int node = 0;
    nv_linux_state_t *nvl = pci_get_drvdata(handle);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    NvU64 base = offset + nvl->coherent_link_info.gpu_mem_pa;
    int ret;
    os_numa_gpu_mem_hotplug_notifier_t notifier =
    {
        .start_pa = base,
        .size = size,
        .pci_info = nv->pci_info,
        .memory_notifier.notifier_call = os_numa_verify_gpu_memory_zone,
    };

    if (nodeId == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (bitmap_empty(nvl->coherent_link_info.free_node_bitmap, MAX_NUMNODES))
    {
        return NV_ERR_IN_USE;
    }
    node = find_first_bit(nvl->coherent_link_info.free_node_bitmap, MAX_NUMNODES);
    if (node == MAX_NUMNODES)
    {
        return NV_ERR_INVALID_STATE;
    }

    NV_ATOMIC_SET(nvl->numa_info.status, NV_IOCTL_NUMA_STATUS_ONLINE_IN_PROGRESS);

    ret = register_memory_notifier(&notifier.memory_notifier);
    if (ret)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Memory hotplug notifier registration failed\n");
        goto failed;
    }

#ifdef NV_ADD_MEMORY_DRIVER_MANAGED_HAS_MHP_FLAGS_ARG
    ret = add_memory_driver_managed(node, base, size, "System RAM (NVIDIA)", MHP_NONE);
#else
    ret = add_memory_driver_managed(node, base, size, "System RAM (NVIDIA)");
#endif
    unregister_memory_notifier(&notifier.memory_notifier);

    if (ret == 0)
    {
        struct zone *zone = &NODE_DATA(node)->node_zones[ZONE_MOVABLE];
        NvU64 start_pfn = base >> PAGE_SHIFT;
        NvU64 end_pfn = (base + size) >> PAGE_SHIFT;

        /* Verify the full GPU memory range passed on is onlined */
        if (zone->zone_start_pfn != start_pfn ||
            zone_end_pfn(zone) != end_pfn)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: GPU memory zone movable auto onlining failed!\n");
#ifdef NV_OFFLINE_AND_REMOVE_MEMORY_PRESENT
#ifdef NV_REMOVE_MEMORY_HAS_NID_ARG
            if (offline_and_remove_memory(node, base, size) != 0)
#else
            if (offline_and_remove_memory(base, size) != 0)
#endif
            {
                nv_printf(NV_DBG_ERRORS, "NVRM: offline_and_remove_memory failed\n");
            }
#endif
            goto failed;
        }

        *nodeId = node;
        clear_bit(node, nvl->coherent_link_info.free_node_bitmap);
        NV_ATOMIC_SET(nvl->numa_info.status, NV_IOCTL_NUMA_STATUS_ONLINE);
        return NV_OK;
    }
    nv_printf(NV_DBG_ERRORS, "NVRM: Memory add failed. base: 0x%lx size: 0x%lx ret: %d\n",
              base, size, ret);
failed:
    NV_ATOMIC_SET(nvl->numa_info.status, NV_IOCTL_NUMA_STATUS_ONLINE_FAILED);
    return NV_ERR_OPERATING_SYSTEM;
#endif
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL os_numa_remove_gpu_memory
(
    void *handle,
    NvU64 offset,
    NvU64 size,
    NvU32 nodeId
)
{
#ifdef NV_ADD_MEMORY_DRIVER_MANAGED_PRESENT
    nv_linux_state_t *nvl = pci_get_drvdata(handle);
#ifdef NV_OFFLINE_AND_REMOVE_MEMORY_PRESENT
    NvU64 base = offset + nvl->coherent_link_info.gpu_mem_pa;
    remove_numa_memory_info_t numa_info;
    nv_kthread_q_item_t remove_numa_memory_q_item;
    int ret;
#endif

    if (nodeId >= MAX_NUMNODES)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    if ((nodeId == NUMA_NO_NODE) || test_bit(nodeId, nvl->coherent_link_info.free_node_bitmap))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    NV_ATOMIC_SET(nvl->numa_info.status, NV_IOCTL_NUMA_STATUS_OFFLINE_IN_PROGRESS);

#ifdef NV_OFFLINE_AND_REMOVE_MEMORY_PRESENT
    numa_info.base   = base;
    numa_info.size   = size;
    numa_info.nodeId = nodeId;
    numa_info.ret    = 0;

    nv_kthread_q_item_init(&remove_numa_memory_q_item,
                           offline_numa_memory_callback,
                           &numa_info);
    nv_kthread_q_schedule_q_item(&nvl->remove_numa_memory_q,
                                 &remove_numa_memory_q_item);
    nv_kthread_q_flush(&nvl->remove_numa_memory_q);

    ret = numa_info.ret;

    if (ret == 0)
    {
        set_bit(nodeId, nvl->coherent_link_info.free_node_bitmap);

        NV_ATOMIC_SET(nvl->numa_info.status, NV_IOCTL_NUMA_STATUS_OFFLINE);
        return NV_OK;
    }

    nv_printf(NV_DBG_ERRORS, "NVRM: Memory remove failed. base: 0x%lx size: 0x%lx ret: %d\n",
              base, size, ret);
#endif
    NV_ATOMIC_SET(nvl->numa_info.status, NV_IOCTL_NUMA_STATUS_OFFLINE_FAILED);
    return NV_ERR_OPERATING_SYSTEM;
#endif
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS NV_API_CALL os_offline_page_at_address
(
    NvU64 address
)
{
#if defined(CONFIG_MEMORY_FAILURE)
    int flags = 0;
    int ret;
    NvU64 pfn;
    struct page *page = NV_GET_PAGE_STRUCT(address);

    if (page == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Failed to get page struct for address: 0x%llx\n",
                  address);
        return NV_ERR_INVALID_ARGUMENT;
    }

    pfn = page_to_pfn(page);

#ifdef NV_MEMORY_FAILURE_MF_SW_SIMULATED_DEFINED
    //
    // Set MF_SW_SIMULATED flag so Linux kernel can differentiate this from a HW
    // memory failure. HW memory failures cannot be unset via unpoison_memory() API.
    //
    // Currently, RM does not use unpoison_memory(), so it makes no difference
    // whether or not MF_SW_SIMULATED is set. Regardless, it is semantically more
    // correct to set MF_SW_SIMULATED.
    //
    flags |= MF_SW_SIMULATED;
#endif

#ifdef NV_MEMORY_FAILURE_HAS_TRAPNO_ARG
    ret = memory_failure(pfn, 0, flags);
#else
    ret = memory_failure(pfn, flags);
#endif

    if (ret != 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: page offlining failed. address: 0x%llx pfn: 0x%llx ret: %d\n",
                  address, pfn, ret);
        return NV_ERR_OPERATING_SYSTEM;
    }

    return NV_OK;
#else // !defined(CONFIG_MEMORY_FAILURE)
    nv_printf(NV_DBG_ERRORS, "NVRM: memory_failure() not supported by kernel. page offlining failed. address: 0x%llx\n",
              address);
    return NV_ERR_NOT_SUPPORTED;
#endif
}

void* NV_API_CALL os_get_pid_info(void)
{
    return get_task_pid(current, PIDTYPE_PID);
}

void NV_API_CALL os_put_pid_info(void *pid_info)
{
    if (pid_info != NULL)
        put_pid(pid_info);
}

NV_STATUS NV_API_CALL os_find_ns_pid(void *pid_info, NvU32 *ns_pid)
{
    if ((pid_info == NULL) || (ns_pid == NULL))
        return NV_ERR_INVALID_ARGUMENT;

    *ns_pid = pid_vnr((struct pid *)pid_info);

    // The call returns 0 if the PID is not found in the current ns
    if (*ns_pid == 0)
        return NV_ERR_OBJECT_NOT_FOUND;

    return NV_OK;
}

