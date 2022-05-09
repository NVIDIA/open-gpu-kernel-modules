/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nv-reg.h"
#include "nv-pat.h"

int nv_pat_mode = NV_PAT_MODE_DISABLED;

#if defined(NV_ENABLE_PAT_SUPPORT)
/*
 * Private PAT support for use by the NVIDIA driver. This is used on
 * kernels that do not modify the PAT to include a write-combining
 * entry.
 *
 * On kernels that have CONFIG_X86_PAT, the NVIDIA driver still checks that the
 * WC entry is as expected before using PAT.
 */

#if defined(CONFIG_X86_PAT)
#define NV_ENABLE_BUILTIN_PAT_SUPPORT 0
#else
#define NV_ENABLE_BUILTIN_PAT_SUPPORT 1
#endif


#define NV_READ_PAT_ENTRIES(pat1, pat2)   rdmsr(0x277, (pat1), (pat2))
#define NV_WRITE_PAT_ENTRIES(pat1, pat2)  wrmsr(0x277, (pat1), (pat2))
#define NV_PAT_ENTRY(pat, index) \
    (((pat) & (0xff << ((index)*8))) >> ((index)*8))

#if NV_ENABLE_BUILTIN_PAT_SUPPORT

static unsigned long orig_pat1, orig_pat2;

static inline void nv_disable_caches(unsigned long *cr4)
{
    unsigned long cr0 = read_cr0();
    write_cr0(((cr0 & (0xdfffffff)) | 0x40000000));
    wbinvd();
    *cr4 = NV_READ_CR4();
    if (*cr4 & 0x80) NV_WRITE_CR4(*cr4 & ~0x80);
    __flush_tlb();
}

static inline void nv_enable_caches(unsigned long cr4)
{
    unsigned long cr0 = read_cr0();
    wbinvd();
    __flush_tlb();
    write_cr0((cr0 & 0x9fffffff));
    if (cr4 & 0x80) NV_WRITE_CR4(cr4);
}

static void nv_setup_pat_entries(void *info)
{
    unsigned long pat1, pat2, cr4;
    unsigned long eflags;

#if defined(NV_ENABLE_HOTPLUG_CPU)
    int cpu = (NvUPtr)info;
    if ((cpu != 0) && (cpu != (int)smp_processor_id()))
        return;
#endif

    NV_SAVE_FLAGS(eflags);
    NV_CLI();
    nv_disable_caches(&cr4);

    NV_READ_PAT_ENTRIES(pat1, pat2);

    pat1 &= 0xffff00ff;
    pat1 |= 0x00000100;

    NV_WRITE_PAT_ENTRIES(pat1, pat2);

    nv_enable_caches(cr4);
    NV_RESTORE_FLAGS(eflags);
}

static void nv_restore_pat_entries(void *info)
{
    unsigned long cr4;
    unsigned long eflags;

#if defined(NV_ENABLE_HOTPLUG_CPU)
    int cpu = (NvUPtr)info;
    if ((cpu != 0) && (cpu != (int)smp_processor_id()))
        return;
#endif

    NV_SAVE_FLAGS(eflags);
    NV_CLI();
    nv_disable_caches(&cr4);

    NV_WRITE_PAT_ENTRIES(orig_pat1, orig_pat2);

    nv_enable_caches(cr4);
    NV_RESTORE_FLAGS(eflags);
}

/*
 * NOTE 1:
 * Functions register_cpu_notifier(), unregister_cpu_notifier(),
 * macros register_hotcpu_notifier, register_hotcpu_notifier,
 * and CPU states CPU_DOWN_FAILED, CPU_DOWN_PREPARE
 * were removed by the following commit:
 *   2016 Dec 25: b272f732f888d4cf43c943a40c9aaa836f9b7431
 *
 * NV_REGISTER_CPU_NOTIFIER_PRESENT is true when
 * register_cpu_notifier() is present.
 *
 * The functions cpuhp_setup_state() and cpuhp_remove_state() should be
 * used as an alternative to register_cpu_notifier() and
 * unregister_cpu_notifier() functions. The following
 * commit introduced these functions as well as the enum cpuhp_state.
 *   2016 Feb 26: 5b7aa87e0482be768486e0c2277aa4122487eb9d
 *
 * NV_CPUHP_CPUHP_STATE_PRESENT is true when cpuhp_setup_state() is present.
 *
 * For kernels where both cpuhp_setup_state() and register_cpu_notifier()
 * are present, we still use register_cpu_notifier().
 */

static int
nvidia_cpu_teardown(unsigned int cpu)
{
#if defined(NV_ENABLE_HOTPLUG_CPU)
    unsigned int this_cpu = get_cpu();

    if (this_cpu == cpu)
        nv_restore_pat_entries(NULL);
    else
        smp_call_function(nv_restore_pat_entries, &cpu, 1);

    put_cpu();
#endif
    return 0;
}

static int
nvidia_cpu_online(unsigned int cpu)
{
#if defined(NV_ENABLE_HOTPLUG_CPU)
    unsigned int this_cpu = get_cpu();

    if (this_cpu == cpu)
        nv_setup_pat_entries(NULL);
    else
        smp_call_function(nv_setup_pat_entries, &cpu, 1);

    put_cpu();
#endif
    return 0;
}

static int nv_enable_builtin_pat_support(void)
{
    unsigned long pat1, pat2;

    NV_READ_PAT_ENTRIES(orig_pat1, orig_pat2);
    nv_printf(NV_DBG_SETUP, "saved orig pats as 0x%lx 0x%lx\n", orig_pat1, orig_pat2);

    on_each_cpu(nv_setup_pat_entries, NULL, 1);

    NV_READ_PAT_ENTRIES(pat1, pat2);
    nv_printf(NV_DBG_SETUP, "changed pats to 0x%lx 0x%lx\n", pat1, pat2);
    return 1;
}

static void nv_disable_builtin_pat_support(void)
{
    unsigned long pat1, pat2;

    on_each_cpu(nv_restore_pat_entries, NULL, 1);

    nv_pat_mode = NV_PAT_MODE_DISABLED;

    NV_READ_PAT_ENTRIES(pat1, pat2);
    nv_printf(NV_DBG_SETUP, "restored orig pats as 0x%lx 0x%lx\n", pat1, pat2);
}

static int
nvidia_cpu_callback(struct notifier_block *nfb, unsigned long action, void *hcpu)
{
/* CPU_DOWN_FAILED was added by the following commit
 *   2004 Oct 18: 71da3667be80d30121df3972caa0bf5684228379
 *
 * CPU_DOWN_PREPARE was added by the following commit
 *   2004 Oct 18: d13d28de21d913aacd3c91e76e307fa2eb7835d8
 *
 * We use one ifdef for both macros since they were added on the same day.
 */
#if defined(CPU_DOWN_FAILED)
    switch (action)
    {
        case CPU_DOWN_FAILED:
        case CPU_ONLINE:
            nvidia_cpu_online((NvUPtr)hcpu);
            break;
        case CPU_DOWN_PREPARE:
            nvidia_cpu_teardown((NvUPtr)hcpu);
            break;
    }
#endif
    return NOTIFY_OK;
}

/*
 * See NOTE 1.
 * In order to avoid warnings for unused variable when compiling against
 * kernel versions which include changes of commit id
 * b272f732f888d4cf43c943a40c9aaa836f9b7431, we have to protect declaration
 * of nv_hotcpu_nfb with #if.
 *
 * NV_REGISTER_CPU_NOTIFIER_PRESENT is checked before
 * NV_CPUHP_SETUP_STATE_PRESENT to avoid compilation warnings for unused
 * variable nvidia_pat_online for kernels where both
 * NV_REGISTER_CPU_NOTIFIER_PRESENT and NV_CPUHP_SETUP_STATE_PRESENT
 * are true.
 */
#if defined(NV_REGISTER_CPU_NOTIFIER_PRESENT) && defined(CONFIG_HOTPLUG_CPU)
static struct notifier_block nv_hotcpu_nfb = {
    .notifier_call = nvidia_cpu_callback,
    .priority = 0
};
#elif defined(NV_CPUHP_SETUP_STATE_PRESENT)
static enum cpuhp_state nvidia_pat_online;
#endif

static int
nvidia_register_cpu_hotplug_notifier(void)
{
    int ret;
/* See NOTE 1 */
#if defined(NV_REGISTER_CPU_NOTIFIER_PRESENT) && defined(CONFIG_HOTPLUG_CPU)
    /* register_hotcpu_notiifer() returns 0 on success or -ENOENT on failure */
    ret = register_hotcpu_notifier(&nv_hotcpu_nfb);
#elif defined(NV_CPUHP_SETUP_STATE_PRESENT)
    /*
     *  cpuhp_setup_state() returns positive number on success when state is
     *  CPUHP_AP_ONLINE_DYN. On failure, it returns a negative number.
     */
    ret = cpuhp_setup_state(CPUHP_AP_ONLINE_DYN,
                                        "nvidia/pat:online",
                                        nvidia_cpu_online,
                                        nvidia_cpu_teardown);
    if (ret < 0)
    {
        /*
         * If cpuhp_setup_state() fails, the cpuhp_remove_state()
         * should never be called. If it gets called, we might remove
         * some other state. Hence, explicitly set
         * nvidia_pat_online to zero. This will trigger a BUG()
         * in cpuhp_remove_state().
         */
        nvidia_pat_online = 0;
    }
    else
    {
        nvidia_pat_online = ret;
    }
#else

    /*
     * This function should be a no-op for kernels which
     * - do not have CONFIG_HOTPLUG_CPU enabled,
     * - do not have PAT support,
     * - do not have the cpuhp_setup_state() function.
     *
     * On such kernels, returning an error here would result in module init
     * failure. Hence, return 0 here.
     */
    if (nv_pat_mode == NV_PAT_MODE_BUILTIN)
    {
        ret = 0;
    }
    else
    {
        ret = -EIO;
    }
#endif

    if (ret < 0)
    {
        nv_disable_pat_support();
        nv_printf(NV_DBG_ERRORS,
            "NVRM: CPU hotplug notifier registration failed!\n");
        return -EIO;
    }
    return 0;
}

static void
nvidia_unregister_cpu_hotplug_notifier(void)
{
/* See NOTE 1 */
#if defined(NV_REGISTER_CPU_NOTIFIER_PRESENT) && defined(CONFIG_HOTPLUG_CPU)
    unregister_hotcpu_notifier(&nv_hotcpu_nfb);
#elif defined(NV_CPUHP_SETUP_STATE_PRESENT)
    cpuhp_remove_state(nvidia_pat_online);
#endif
    return;
}


#else /* NV_ENABLE_BUILTIN_PAT_SUPPORT */

static int nv_enable_builtin_pat_support(void)
{
    return 0;
}
static void nv_disable_builtin_pat_support(void)
{
}
static int nvidia_register_cpu_hotplug_notifier(void)
{
    return -EIO;
}
static void nvidia_unregister_cpu_hotplug_notifier(void)
{
}

#endif /* NV_ENABLE_BUILTIN_PAT_SUPPORT */

static int nv_determine_pat_mode(void)
{
    unsigned int pat1, pat2, i;
    NvU8 PAT_WC_index;

    if (!test_bit(X86_FEATURE_PAT,
            (volatile unsigned long *)&boot_cpu_data.x86_capability))
    {
        if ((boot_cpu_data.x86_vendor != X86_VENDOR_INTEL) ||
                (boot_cpu_data.cpuid_level < 1) ||
                ((cpuid_edx(1) & (1 << 16)) == 0) ||
                (boot_cpu_data.x86 != 6) || (boot_cpu_data.x86_model >= 15))
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: CPU does not support the PAT.\n");
            return NV_PAT_MODE_DISABLED;
        }
    }

    NV_READ_PAT_ENTRIES(pat1, pat2);
    PAT_WC_index = 0xf;

    for (i = 0; i < 4; i++)
    {
        if (NV_PAT_ENTRY(pat1, i) == 0x01)
        {
            PAT_WC_index = i;
            break;
        }

        if (NV_PAT_ENTRY(pat2, i) == 0x01)
        {
            PAT_WC_index = (i + 4);
            break;
        }
    }

    if (PAT_WC_index == 1)
    {
        return NV_PAT_MODE_KERNEL;
    }
    else if (PAT_WC_index != 0xf)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: PAT configuration unsupported.\n");
        return NV_PAT_MODE_DISABLED;
    }
    else
    {
#if NV_ENABLE_BUILTIN_PAT_SUPPORT
        return NV_PAT_MODE_BUILTIN;
#else
        return NV_PAT_MODE_DISABLED;
#endif /* NV_ENABLE_BUILTIN_PAT_SUPPORT */
    }
}


int nv_enable_pat_support(void)
{
    if (nv_pat_mode != NV_PAT_MODE_DISABLED)
        return 1;

    nv_pat_mode = nv_determine_pat_mode();

    switch (nv_pat_mode)
    {
        case NV_PAT_MODE_DISABLED:
            /* avoid the PAT if unavailable/unusable */
            return 0;
        case NV_PAT_MODE_KERNEL:
            /* inherit the kernel's PAT layout */
            return 1;
        case NV_PAT_MODE_BUILTIN:
            /* use builtin code to modify the PAT layout */
            break;
    }

    return nv_enable_builtin_pat_support();
}

void nv_disable_pat_support(void)
{
    if (nv_pat_mode != NV_PAT_MODE_BUILTIN)
        return;

    nv_disable_builtin_pat_support();
}

int nv_init_pat_support(nvidia_stack_t *sp)
{
    NV_STATUS status;
    NvU32 data;
    int disable_pat = 0;
    int ret = 0;

    status = rm_read_registry_dword(sp, NULL,
                                    NV_USE_PAGE_ATTRIBUTE_TABLE, &data);
    if ((status == NV_OK) && ((int)data != ~0))
    {
        disable_pat = (data == 0);
    }

    if (!disable_pat)
    {
        nv_enable_pat_support();
        if (nv_pat_mode == NV_PAT_MODE_BUILTIN)
        {
             ret = nvidia_register_cpu_hotplug_notifier();
             return ret;
        }
    }
    else
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: builtin PAT support disabled.\n");
    }

    return 0;
}

void nv_teardown_pat_support(void)
{
    if (nv_pat_mode == NV_PAT_MODE_BUILTIN)
    {
        nv_disable_pat_support();
        nvidia_unregister_cpu_hotplug_notifier();
    }
}
#endif /* defined(NV_ENABLE_PAT_SUPPORT) */
