/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvmisc.h"
#include "os-interface.h"
#include "nv-linux.h"
#include "nv-p2p.h"
#include "nv-reg.h"
#include "nv-msi.h"
#include "nv-pci-table.h"

#if defined(NV_UVM_ENABLE)
#include "nv_uvm_interface.h"
#endif

#if defined(NV_VGPU_KVM_BUILD)
#include "nv-vgpu-vfio-interface.h"
#endif


#include "nvlink_proto.h"
#include "nvlink_caps.h"


#include "nv-frontend.h"
#include "nv-hypervisor.h"
#include "nv-ibmnpu.h"
#include "nv-rsync.h"
#include "nv-kthread-q.h"
#include "nv-pat.h"
#include "nv-dmabuf.h"

#if !defined(CONFIG_RETPOLINE)
#include "nv-retpoline.h"
#endif

#include <linux/firmware.h>

#include <sound/core.h>             /* HDA struct snd_card */

#include <asm/cache.h>

#if defined(NV_SOUND_HDAUDIO_H_PRESENT)
#include "sound/hdaudio.h"
#endif

#if defined(NV_SOUND_HDA_CODEC_H_PRESENT)
#include <sound/core.h>
#include <sound/hda_codec.h>
#include <sound/hda_verbs.h>
#endif

#if defined(NV_SEQ_READ_ITER_PRESENT)
#include <linux/uio.h>
#include <linux/seq_file.h>
#include <linux/kernfs.h>
#endif

#include <linux/dmi.h>              /* System DMI info */

#include <linux/ioport.h>

#include "conftest/patches.h"

#define RM_THRESHOLD_TOTAL_IRQ_COUNT     100000
#define RM_THRESHOLD_UNAHNDLED_IRQ_COUNT 99900
#define RM_UNHANDLED_TIMEOUT_US          100000

const NvBool nv_is_rm_firmware_supported_os = NV_TRUE;

// Deprecated, use NV_REG_ENABLE_GPU_FIRMWARE instead
char *rm_firmware_active = NULL;
NV_MODULE_STRING_PARAMETER(rm_firmware_active);

#define NV_FIRMWARE_GSP_FILENAME     "nvidia/" NV_VERSION_STRING "/gsp.bin"
#define NV_FIRMWARE_GSP_LOG_FILENAME "nvidia/" NV_VERSION_STRING "/gsp_log.bin"

MODULE_FIRMWARE(NV_FIRMWARE_GSP_FILENAME);

/*
 * Global NVIDIA capability state, for GPU driver
 */
nv_cap_t *nvidia_caps_root = NULL;

/*
 * our global state; one per device
 */
NvU32 num_nv_devices = 0;
NvU32 num_probed_nv_devices = 0;

nv_linux_state_t *nv_linux_devices;

/*
 * And one for the control device
 */
nv_linux_state_t nv_ctl_device = { { 0 } };
extern NvU32 nv_dma_remap_peer_mmio;

nv_kthread_q_t nv_kthread_q;
nv_kthread_q_t nv_deferred_close_kthread_q;

struct rw_semaphore nv_system_pm_lock;

#if defined(CONFIG_PM)
static nv_power_state_t nv_system_power_state;
static nv_pm_action_depth_t nv_system_pm_action_depth;
struct semaphore nv_system_power_state_lock;
#endif

void *nvidia_p2p_page_t_cache;
static void *nvidia_pte_t_cache;
void *nvidia_stack_t_cache;
static nvidia_stack_t *__nv_init_sp;

static int nv_tce_bypass_mode = NV_TCE_BYPASS_MODE_DEFAULT;

struct semaphore nv_linux_devices_lock;

static NvTristate nv_chipset_is_io_coherent = NV_TRISTATE_INDETERMINATE;

// True if all the successfully probed devices support ATS
// Assigned at device probe (module init) time
NvBool nv_ats_supported = NVCPU_IS_PPC64LE





;

// allow an easy way to convert all debug printfs related to events
// back and forth between 'info' and 'errors'
#if defined(NV_DBG_EVENTS)
#define NV_DBG_EVENTINFO NV_DBG_ERRORS
#else
#define NV_DBG_EVENTINFO NV_DBG_INFO
#endif

#if defined(HDA_MAX_CODECS)
#define NV_HDA_MAX_CODECS HDA_MAX_CODECS
#else
#define NV_HDA_MAX_CODECS 8
#endif

/***
 *** STATIC functions, only in this file
 ***/

/* nvos_ functions.. do not take a state device parameter  */
static int      nvos_count_devices(void);

static nv_alloc_t  *nvos_create_alloc(struct device *, int);
static int          nvos_free_alloc(nv_alloc_t *);

/***
 *** EXPORTS to Linux Kernel
 ***/

static irqreturn_t   nvidia_isr_common_bh   (void *);
static void          nvidia_isr_bh_unlocked (void *);
static int           nvidia_ctl_open        (struct inode *, struct file *);
static int           nvidia_ctl_close       (struct inode *, struct file *);

const char *nv_device_name = MODULE_NAME;
static const char *nvidia_stack_cache_name = MODULE_NAME "_stack_cache";
static const char *nvidia_pte_cache_name = MODULE_NAME "_pte_cache";
static const char *nvidia_p2p_page_cache_name = MODULE_NAME "_p2p_page_cache";

static int           nvidia_open           (struct inode *, struct file *);
static int           nvidia_close          (struct inode *, struct file *);
static unsigned int  nvidia_poll           (struct file *, poll_table *);
static int           nvidia_ioctl          (struct inode *, struct file *, unsigned int, unsigned long);

/* character device entry points*/
nvidia_module_t nv_fops = {
    .owner       = THIS_MODULE,
    .module_name = MODULE_NAME,
    .instance    = MODULE_INSTANCE_NUMBER,
    .open        = nvidia_open,
    .close       = nvidia_close,
    .ioctl       = nvidia_ioctl,
    .mmap        = nvidia_mmap,
    .poll        = nvidia_poll,
};

#if defined(CONFIG_PM)
static int           nv_pmops_suspend          (struct device *dev);
static int           nv_pmops_resume           (struct device *dev);
static int           nv_pmops_freeze           (struct device *dev);
static int           nv_pmops_thaw             (struct device *dev);
static int           nv_pmops_restore          (struct device *dev);
static int           nv_pmops_poweroff         (struct device *dev);
static int           nv_pmops_runtime_suspend  (struct device *dev);
static int           nv_pmops_runtime_resume   (struct device *dev);

struct dev_pm_ops nv_pm_ops = {
    .suspend         = nv_pmops_suspend,
    .resume          = nv_pmops_resume,
    .freeze          = nv_pmops_freeze,
    .thaw            = nv_pmops_thaw,
    .poweroff        = nv_pmops_poweroff,
    .restore         = nv_pmops_restore,
    .runtime_suspend = nv_pmops_runtime_suspend,
    .runtime_resume  = nv_pmops_runtime_resume,
};
#endif

/***
 *** see nv.h for functions exported to other parts of resman
 ***/

/***
 *** STATIC functions
 ***/

#if defined(NVCPU_X86_64)
#define NV_AMD_SEV_BIT BIT(1)

static
NvBool nv_is_sev_supported(
    void
)
{
    unsigned int eax, ebx, ecx, edx;

    /* Check for the SME/SEV support leaf */
    eax = 0x80000000;
    ecx = 0;
    native_cpuid(&eax, &ebx, &ecx, &edx);
    if (eax < 0x8000001f)
        return NV_FALSE;

    eax = 0x8000001f;
    ecx = 0;
    native_cpuid(&eax, &ebx, &ecx, &edx);
    /* Check whether SEV is supported */
    if (!(eax & NV_AMD_SEV_BIT))
        return NV_FALSE;

    return NV_TRUE;
}
#endif

static
void nv_sev_init(
    void
)
{
#if defined(MSR_AMD64_SEV) && defined(NVCPU_X86_64)
    NvU32 lo_val, hi_val;

    if (!nv_is_sev_supported())
        return;

    rdmsr(MSR_AMD64_SEV, lo_val, hi_val);

    os_sev_status = lo_val;
#if defined(MSR_AMD64_SEV_ENABLED)
    os_sev_enabled = (os_sev_status & MSR_AMD64_SEV_ENABLED);
#endif
#endif
}

static
nv_alloc_t *nvos_create_alloc(
    struct device *dev,
    int num_pages
)
{
    nv_alloc_t *at;
    unsigned int pt_size, i;

    NV_KMALLOC(at, sizeof(nv_alloc_t));
    if (at == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate alloc info\n");
        return NULL;
    }

    memset(at, 0, sizeof(nv_alloc_t));

    at->dev = dev;
    pt_size = num_pages *  sizeof(nvidia_pte_t *);
    if (os_alloc_mem((void **)&at->page_table, pt_size) != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate page table\n");
        NV_KFREE(at, sizeof(nv_alloc_t));
        return NULL;
    }

    memset(at->page_table, 0, pt_size);
    at->num_pages = num_pages;
    NV_ATOMIC_SET(at->usage_count, 0);

    for (i = 0; i < at->num_pages; i++)
    {
        at->page_table[i] = NV_KMEM_CACHE_ALLOC(nvidia_pte_t_cache);
        if (at->page_table[i] == NULL)
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: failed to allocate page table entry\n");
            nvos_free_alloc(at);
            return NULL;
        }
        memset(at->page_table[i], 0, sizeof(nvidia_pte_t));
    }

    at->pid = os_get_current_process();

    return at;
}

static
int nvos_free_alloc(
    nv_alloc_t *at
)
{
    unsigned int i;

    if (at == NULL)
        return -1;

    if (NV_ATOMIC_READ(at->usage_count))
        return 1;

    for (i = 0; i < at->num_pages; i++)
    {
        if (at->page_table[i] != NULL)
            NV_KMEM_CACHE_FREE(at->page_table[i], nvidia_pte_t_cache);
    }
    os_free_mem(at->page_table);

    NV_KFREE(at, sizeof(nv_alloc_t));

    return 0;
}

static void
nv_module_resources_exit(nv_stack_t *sp)
{
    nv_kmem_cache_free_stack(sp);

    NV_KMEM_CACHE_DESTROY(nvidia_p2p_page_t_cache);
    NV_KMEM_CACHE_DESTROY(nvidia_pte_t_cache);
    NV_KMEM_CACHE_DESTROY(nvidia_stack_t_cache);
}

static int __init
nv_module_resources_init(nv_stack_t **sp)
{
    int rc = -ENOMEM;

    nvidia_stack_t_cache = NV_KMEM_CACHE_CREATE(nvidia_stack_cache_name,
                                                nvidia_stack_t);
    if (nvidia_stack_t_cache == NULL)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: nvidia_stack_t cache allocation failed.\n");
        goto exit;
    }

    nvidia_pte_t_cache = NV_KMEM_CACHE_CREATE(nvidia_pte_cache_name,
                                              nvidia_pte_t);
    if (nvidia_pte_t_cache == NULL)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: nvidia_pte_t cache allocation failed.\n");
        goto exit;
    }

    nvidia_p2p_page_t_cache = NV_KMEM_CACHE_CREATE(nvidia_p2p_page_cache_name,
                                                   nvidia_p2p_page_t);
    if (nvidia_p2p_page_t_cache == NULL)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: nvidia_p2p_page_t cache allocation failed.\n");
        goto exit;
    }

    rc = nv_kmem_cache_alloc_stack(sp);
    if (rc < 0)
    {
        goto exit;
    }

exit:
    if (rc < 0)
    {
        nv_kmem_cache_free_stack(*sp);

        NV_KMEM_CACHE_DESTROY(nvidia_p2p_page_t_cache);
        NV_KMEM_CACHE_DESTROY(nvidia_pte_t_cache);
        NV_KMEM_CACHE_DESTROY(nvidia_stack_t_cache);
    }

    return rc;
}


static void
nvlink_drivers_exit(void)
{

#if NVCPU_IS_64_BITS
    nvswitch_exit();
#endif


#if defined(NVCPU_PPC64LE)
    ibmnpu_exit();
#endif

    nvlink_core_exit();
}



static int __init
nvlink_drivers_init(void)
{
    int rc = 0;

    rc = nvlink_core_init();
    if (rc < 0)
    {
        nv_printf(NV_DBG_INFO, "NVRM: NVLink core init failed.\n");
        return rc;
    }

#if defined(NVCPU_PPC64LE)
    rc = ibmnpu_init();
    if (rc < 0)
    {
        nv_printf(NV_DBG_INFO, "NVRM: IBM NPU init failed.\n");
        nvlink_core_exit();
        return rc;
    }
#endif


#if NVCPU_IS_64_BITS
    rc = nvswitch_init();
    if (rc < 0)
    {
        nv_printf(NV_DBG_INFO, "NVRM: NVSwitch init failed.\n");
#if defined(NVCPU_PPC64LE)
        ibmnpu_exit();
#endif
        nvlink_core_exit();
    }
#endif


    return rc;
}


static void
nv_module_state_exit(nv_stack_t *sp)
{
    nv_state_t *nv = NV_STATE_PTR(&nv_ctl_device);

    nv_teardown_pat_support();

    nv_kthread_q_stop(&nv_deferred_close_kthread_q);
    nv_kthread_q_stop(&nv_kthread_q);

    nv_lock_destroy_locks(sp, nv);
}

static int
nv_module_state_init(nv_stack_t *sp)
{
    int rc;
    nv_state_t *nv = NV_STATE_PTR(&nv_ctl_device);

    nv->os_state = (void *)&nv_ctl_device;

    if (!nv_lock_init_locks(sp, nv))
    {
        return -ENOMEM;
    }

    rc = nv_kthread_q_init(&nv_kthread_q, "nv_queue");
    if (rc != 0)
    {
        goto exit;
    }

    rc = nv_kthread_q_init(&nv_deferred_close_kthread_q, "nv_queue");
    if (rc != 0)
    {
        nv_kthread_q_stop(&nv_kthread_q);
        goto exit;
    }

    rc = nv_init_pat_support(sp);
    if (rc < 0)
    {
        nv_kthread_q_stop(&nv_deferred_close_kthread_q);
        nv_kthread_q_stop(&nv_kthread_q);
        goto exit;
    }

    nv_linux_devices = NULL;
    NV_INIT_MUTEX(&nv_linux_devices_lock);
    init_rwsem(&nv_system_pm_lock);

#if defined(CONFIG_PM)
    NV_INIT_MUTEX(&nv_system_power_state_lock);
    nv_system_power_state = NV_POWER_STATE_RUNNING;
    nv_system_pm_action_depth = NV_PM_ACTION_DEPTH_DEFAULT;
#endif

    NV_SPIN_LOCK_INIT(&nv_ctl_device.snapshot_timer_lock);

exit:
    if (rc < 0)
    {
        nv_lock_destroy_locks(sp, nv);
    }

    return rc;
}

static void __init
nv_registry_keys_init(nv_stack_t *sp)
{
    NV_STATUS status;
    nv_state_t *nv = NV_STATE_PTR(&nv_ctl_device);
    NvU32 data;

    /*
     * Determine the TCE bypass mode here so it can be used during
     * device probe.  Also determine whether we should allow
     * user-mode NUMA onlining of device memory.
     */
    if (NVCPU_IS_PPC64LE)
    {
        status = rm_read_registry_dword(sp, nv,
                                        NV_REG_TCE_BYPASS_MODE,
                                        &data);
        if ((status == NV_OK) && ((int)data != NV_TCE_BYPASS_MODE_DEFAULT))
        {
            nv_tce_bypass_mode = data;
        }

        if (NVreg_EnableUserNUMAManagement)
        {
            /* Force on the core RM registry key to match. */
            status = rm_write_registry_dword(sp, nv, "RMNumaOnlining", 1);
            WARN_ON(status != NV_OK);
        }
    }

    status = rm_read_registry_dword(sp, nv, NV_DMA_REMAP_PEER_MMIO, &data);
    if (status == NV_OK)
    {
        nv_dma_remap_peer_mmio = data;
    }
}

static void __init
nv_report_applied_patches(void)
{
    unsigned i;

    for (i = 0; __nv_patches[i].short_description; i++)
    {
        if (i == 0)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: Applied patches:\n");
        }

        nv_printf(NV_DBG_ERRORS,
            "NVRM:    Patch #%d: %s\n", i + 1, __nv_patches[i].short_description);
    }
}

static void
nv_drivers_exit(void)
{



    nv_pci_unregister_driver();

    nvidia_unregister_module(&nv_fops);
}

static int __init
nv_drivers_init(void)
{
    int rc;

    rc = nvidia_register_module(&nv_fops);
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: failed to register character device.\n");
        return rc;
    }

    rc = nv_pci_register_driver();
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: No NVIDIA PCI devices found.\n");
        rc = -ENODEV;
        goto exit;
    }











exit:
    if (rc < 0)
    {
        nvidia_unregister_module(&nv_fops);
    }

    return rc;
}

static void
nv_module_exit(nv_stack_t *sp)
{
    nv_module_state_exit(sp);

    rm_shutdown_rm(sp);

    nv_destroy_rsync_info();

    nvlink_drivers_exit();


    nv_cap_drv_exit();

    nv_module_resources_exit(sp);
}

static int __init
nv_module_init(nv_stack_t **sp)
{
    int rc;

    rc = nv_module_resources_init(sp);
    if (rc < 0)
    {
        return rc;
    }

    rc = nv_cap_drv_init();
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: nv-cap-drv init failed.\n");
        goto cap_drv_exit;
    }


    rc = nvlink_drivers_init();
    if (rc < 0)
    {
        goto cap_drv_exit;
    }


    nv_init_rsync_info(); 
    nv_sev_init();

    if (!rm_init_rm(*sp))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: rm_init_rm() failed!\n");
        rc = -EIO;
        goto nvlink_exit;
    }

    rc = nv_module_state_init(*sp);
    if (rc < 0)
    {
        goto init_rm_exit;
    }

    return rc;

init_rm_exit:
    rm_shutdown_rm(*sp);

nvlink_exit:
    nv_destroy_rsync_info();

    nvlink_drivers_exit();


cap_drv_exit:
    nv_cap_drv_exit();
    nv_module_resources_exit(*sp);

    return rc;
}

/*
 * In this function we check for the cases where GPU exclusion is not
 * honored, and issue a warning.
 *
 * Only GPUs that support a mechanism to query UUID prior to
 * initializing the GPU can be excluded, so that we can detect and
 * exclude them during device probe.  This function checks that an
 * initialized GPU was not specified in the exclusion list, and issues a
 * warning if so.
 */
static void
nv_assert_not_in_gpu_exclusion_list(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    char *uuid = rm_get_gpu_uuid(sp, nv);

    if (uuid == NULL)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "Unable to read UUID");
        return;
    }

    if (nv_is_uuid_in_gpu_exclusion_list(uuid))
    {
        NV_DEV_PRINTF(NV_DBG_WARNINGS, nv,
                      "Could not exclude GPU %s because PBI is not supported\n",
                      uuid);
        WARN_ON(1);
    }

    os_free_mem(uuid);

    return;
}

static int __init nv_caps_root_init(void)
{
    nvidia_caps_root = os_nv_cap_init("driver/" MODULE_NAME);

    return (nvidia_caps_root == NULL) ? -ENOENT : 0;
}

static void nv_caps_root_exit(void)
{
    os_nv_cap_destroy_entry(nvidia_caps_root);
    nvidia_caps_root = NULL;
}

int __init nvidia_init_module(void)
{
    int rc;
    NvU32 count;
    nvidia_stack_t *sp = NULL;
    const NvBool is_nvswitch_present = os_is_nvswitch_present();

    nv_memdbg_init();

    rc = nv_procfs_init();
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to initialize procfs.\n");
        return rc;
    }

    rc = nv_caps_root_init();
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to initialize capabilities.\n");
        goto procfs_exit;
    }

    rc = nv_module_init(&sp);
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to initialize module.\n");
        goto caps_root_exit;
    }

    count = nvos_count_devices();
    if ((count == 0) && (!is_nvswitch_present))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: No NVIDIA GPU found.\n");
        rc = -ENODEV;
        goto module_exit;
    }

    rc = nv_drivers_init();
    if (rc < 0)
    {
        goto module_exit;
    }

    if (num_probed_nv_devices != count)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: The NVIDIA probe routine was not called for %d device(s).\n",
            count - num_probed_nv_devices);
        nv_printf(NV_DBG_ERRORS,
            "NVRM: This can occur when a driver such as: \n"
            "NVRM: nouveau, rivafb, nvidiafb or rivatv "
            "\nNVRM: was loaded and obtained ownership of the NVIDIA device(s).\n");
        nv_printf(NV_DBG_ERRORS,
            "NVRM: Try unloading the conflicting kernel module (and/or\n"
            "NVRM: reconfigure your kernel without the conflicting\n"
            "NVRM: driver(s)), then try loading the NVIDIA kernel module\n"
            "NVRM: again.\n");
    }

    if ((num_probed_nv_devices == 0) && (!is_nvswitch_present))
    {
        rc = -ENODEV;
        nv_printf(NV_DBG_ERRORS, "NVRM: No NVIDIA devices probed.\n");
        goto drivers_exit;
    }

    if (num_probed_nv_devices != num_nv_devices)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: The NVIDIA probe routine failed for %d device(s).\n",
            num_probed_nv_devices - num_nv_devices);
    }

    if ((num_nv_devices == 0) && (!is_nvswitch_present))
    {
        rc = -ENODEV;
        nv_printf(NV_DBG_ERRORS,
            "NVRM: None of the NVIDIA devices were initialized.\n");
        goto drivers_exit;
    }

    /*
     * Initialize registry keys after PCI driver registration has
     * completed successfully to support per-device module
     * parameters.
     */
    nv_registry_keys_init(sp);

    nv_report_applied_patches();

    nv_printf(NV_DBG_ERRORS, "NVRM: loading %s\n", pNVRM_ID);

#if defined(NV_UVM_ENABLE)
    rc = nv_uvm_init();
    if (rc != 0)
    {
        goto drivers_exit;
    }
#endif

    __nv_init_sp = sp;

    return 0;

drivers_exit:
    nv_drivers_exit();

module_exit:
    nv_module_exit(sp);

caps_root_exit:
    nv_caps_root_exit();

procfs_exit:
    nv_procfs_exit();

    return rc;
}

void nvidia_exit_module(void)
{
    nvidia_stack_t *sp = __nv_init_sp;

#if defined(NV_UVM_ENABLE)
    nv_uvm_exit();
#endif

    nv_drivers_exit();

    nv_module_exit(sp);

    nv_caps_root_exit();

    nv_procfs_exit();

    nv_memdbg_exit();
}

static void *nv_alloc_file_private(void)
{
    nv_linux_file_private_t *nvlfp;
    unsigned int i;

    NV_KMALLOC(nvlfp, sizeof(nv_linux_file_private_t));
    if (!nvlfp)
        return NULL;

    memset(nvlfp, 0, sizeof(nv_linux_file_private_t));

    for (i = 0; i < NV_FOPS_STACK_INDEX_COUNT; ++i)
    {
        NV_INIT_MUTEX(&nvlfp->fops_sp_lock[i]);
    }
    init_waitqueue_head(&nvlfp->waitqueue);
    NV_SPIN_LOCK_INIT(&nvlfp->fp_lock);

    return nvlfp;
}

static void nv_free_file_private(nv_linux_file_private_t *nvlfp)
{
    nvidia_event_t *nvet;

    if (nvlfp == NULL)
        return;

    for (nvet = nvlfp->event_data_head; nvet != NULL; nvet = nvlfp->event_data_head)
    {
        nvlfp->event_data_head = nvlfp->event_data_head->next;
        NV_KFREE(nvet, sizeof(nvidia_event_t));
    }

    if (nvlfp->mmap_context.page_array != NULL)
    {
        os_free_mem(nvlfp->mmap_context.page_array);
    }

    NV_KFREE(nvlfp, sizeof(nv_linux_file_private_t));
}


static int nv_is_control_device(
    struct inode *inode
)
{
    return (minor((inode)->i_rdev) == NV_CONTROL_DEVICE_MINOR);
}

/*
 * Search the global list of nv devices for the one with the given minor device
 * number. If found, nvl is returned with nvl->ldata_lock taken.
 */
static nv_linux_state_t *find_minor(NvU32 minor)
{
    nv_linux_state_t *nvl;

    LOCK_NV_LINUX_DEVICES();
    nvl = nv_linux_devices;
    while (nvl != NULL)
    {
        if (nvl->minor_num == minor)
        {
            down(&nvl->ldata_lock);
            break;
        }
        nvl = nvl->next;
    }

    UNLOCK_NV_LINUX_DEVICES();
    return nvl;
}

/*
 * Search the global list of nv devices for the one with the given gpu_id.
 * If found, nvl is returned with nvl->ldata_lock taken.
 */
static nv_linux_state_t *find_gpu_id(NvU32 gpu_id)
{
    nv_linux_state_t *nvl;

    LOCK_NV_LINUX_DEVICES();
    nvl = nv_linux_devices;
    while (nvl != NULL)
    {
        nv_state_t *nv = NV_STATE_PTR(nvl);
        if (nv->gpu_id == gpu_id)
        {
            down(&nvl->ldata_lock);
            break;
        }
        nvl = nvl->next;
    }

    UNLOCK_NV_LINUX_DEVICES();
    return nvl;
}

/*
 * Search the global list of nv devices for the one with the given UUID. Devices
 * with missing UUID information are ignored. If found, nvl is returned with
 * nvl->ldata_lock taken.
 */
nv_linux_state_t *find_uuid(const NvU8 *uuid)
{
    nv_linux_state_t *nvl = NULL;
    nv_state_t *nv;
    const NvU8 *dev_uuid;

    LOCK_NV_LINUX_DEVICES();

    for (nvl = nv_linux_devices; nvl; nvl = nvl->next)
    {
        nv = NV_STATE_PTR(nvl);
        down(&nvl->ldata_lock);
        dev_uuid = nv_get_cached_uuid(nv);
        if (dev_uuid && memcmp(dev_uuid, uuid, GPU_UUID_LEN) == 0)
            goto out;
        up(&nvl->ldata_lock);
    }

out:
    UNLOCK_NV_LINUX_DEVICES();
    return nvl;
}

/*
 * Search the global list of nv devices. The search logic is:
 *
 * 1) If any device has the given UUID, return it
 *
 * 2) If no device has the given UUID but at least one device is missing
 *    its UUID (for example because rm_init_adapter has not run on it yet),
 *    return that device.
 *
 * 3) If no device has the given UUID and all UUIDs are present, return NULL.
 *
 * In cases 1 and 2, nvl is returned with nvl->ldata_lock taken.
 *
 * The reason for this weird logic is because UUIDs aren't always available. See
 * bug 1642200.
 */
static nv_linux_state_t *find_uuid_candidate(const NvU8 *uuid)
{
    nv_linux_state_t *nvl = NULL;
    nv_state_t *nv;
    const NvU8 *dev_uuid;
    int use_missing;
    int has_missing = 0;

    LOCK_NV_LINUX_DEVICES();

    /*
     * Take two passes through the list. The first pass just looks for the UUID.
     * The second looks for the target or missing UUIDs. It would be nice if
     * this could be done in a single pass by remembering which nvls are missing
     * UUIDs, but we have to hold the nvl lock after we check for the UUID.
     */
    for (use_missing = 0; use_missing <= 1; use_missing++)
    {
        for (nvl = nv_linux_devices; nvl; nvl = nvl->next)
        {
            nv = NV_STATE_PTR(nvl);
            down(&nvl->ldata_lock);
            dev_uuid = nv_get_cached_uuid(nv);
            if (dev_uuid)
            {
                /* Case 1: If a device has the given UUID, return it */
                if (memcmp(dev_uuid, uuid, GPU_UUID_LEN) == 0)
                    goto out;
            }
            else
            {
                /* Case 2: If no device has the given UUID but at least one
                 * device is missing its UUID, return that device. */
                if (use_missing)
                    goto out;
                has_missing = 1;
            }
            up(&nvl->ldata_lock);
        }

        /* Case 3: If no device has the given UUID and all UUIDs are present,
         * return NULL. */
        if (!has_missing)
            break;
    }

out:
    UNLOCK_NV_LINUX_DEVICES();
    return nvl;
}

void nv_dev_free_stacks(nv_linux_state_t *nvl)
{
    NvU32 i;
    for (i = 0; i < NV_DEV_STACK_COUNT; i++)
    {
        if (nvl->sp[i])
        {
            nv_kmem_cache_free_stack(nvl->sp[i]);
            nvl->sp[i] = NULL;
        }
    }
}

static int nv_dev_alloc_stacks(nv_linux_state_t *nvl)
{
    NvU32 i;
    int rc;

    for (i = 0; i < NV_DEV_STACK_COUNT; i++)
    {
        rc = nv_kmem_cache_alloc_stack(&nvl->sp[i]);
        if (rc != 0)
        {
            nv_dev_free_stacks(nvl);
            return rc;
        }
    }

    return 0;
}

static int validate_numa_start_state(nv_linux_state_t *nvl)
{
    int rc = 0;
    int numa_status = nv_get_numa_status(nvl);

    if (numa_status != NV_IOCTL_NUMA_STATUS_DISABLED)
    {
        if (nv_ctl_device.numa_memblock_size == 0)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: numa memblock size of zero "
                      "found during device start");
            rc = -EINVAL;
        }
        else
        {
            /* Keep the individual devices consistent with the control device */
            nvl->numa_memblock_size = nv_ctl_device.numa_memblock_size;
        }
    }

    return rc;
}

NV_STATUS NV_API_CALL nv_get_num_dpaux_instances(nv_state_t *nv, NvU32 *num_instances)
{
    *num_instances = nv->num_dpaux_instance;
    return NV_OK;
}

void NV_API_CALL
nv_schedule_uvm_isr(nv_state_t *nv)
{
#if defined(NV_UVM_ENABLE)
    nv_uvm_event_interrupt(nv_get_cached_uuid(nv));
#endif
}

/*
 * Brings up the device on the first file open. Assumes nvl->ldata_lock is held.
 */
static int nv_start_device(nv_state_t *nv, nvidia_stack_t *sp)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
    NvU32 msi_config = 0;
#endif
    int rc = 0;
    NvBool kthread_init = NV_FALSE;
    NvBool power_ref = NV_FALSE;

    rc = nv_get_rsync_info();
    if (rc != 0)
    {
        return rc;
    }

    rc = validate_numa_start_state(nvl);
    if (rc != 0)
    {
        goto failed;
    }

    if (nv_dev_is_pci(nvl->dev) && (nv->pci_info.device_id == 0))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: open of non-existent GPU with minor number %d\n", nvl->minor_num);
        rc = -ENXIO;
        goto failed;
    }

    if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
    {
        if (rm_ref_dynamic_power(sp, nv, NV_DYNAMIC_PM_COARSE) != NV_OK)
        {
            rc = -EINVAL;
            goto failed;
        }
        power_ref = NV_TRUE;
    }
    else
    {
        if (rm_ref_dynamic_power(sp, nv, NV_DYNAMIC_PM_FINE) != NV_OK)
        {
            rc = -EINVAL;
            goto failed;
        }
        power_ref = NV_TRUE;
    }

    rc = nv_init_ibmnpu_devices(nv);
    if (rc != 0)
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: failed to initialize ibmnpu devices attached to GPU with minor number %d\n",
            nvl->minor_num);
        goto failed;
    }

    if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
    {
        rc = nv_dev_alloc_stacks(nvl);
        if (rc != 0)
            goto failed;
    }

#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
    if (nv_dev_is_pci(nvl->dev))
    {
        if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
        {
            rm_read_registry_dword(sp, nv, NV_REG_ENABLE_MSI, &msi_config);
            if (msi_config == 1)
            {
                if (pci_find_capability(nvl->pci_dev, PCI_CAP_ID_MSIX))
                {
                    nv_init_msix(nv);
                }
                if (pci_find_capability(nvl->pci_dev, PCI_CAP_ID_MSI) &&
                    !(nv->flags & NV_FLAG_USES_MSIX))
                {
                    nv_init_msi(nv);
                }
            }
        }
    }
#endif

    if (((!(nv->flags & NV_FLAG_USES_MSI)) && (!(nv->flags & NV_FLAG_USES_MSIX)))
        && (nv->interrupt_line == 0) && !(nv->flags & NV_FLAG_SOC_DISPLAY))
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "No interrupts of any type are available. Cannot use this GPU.\n");
        rc = -EIO;
        goto failed;
    }

    rc = 0;
    if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
    {
        if (nv->flags & NV_FLAG_SOC_DISPLAY)
        {



        }
        else if (!(nv->flags & NV_FLAG_USES_MSIX))
        {
            rc = request_threaded_irq(nv->interrupt_line, nvidia_isr,
                                  nvidia_isr_kthread_bh, nv_default_irq_flags(nv),
                                  nv_device_name, (void *)nvl);
        }
#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
        else
        {
            rc = nv_request_msix_irq(nvl);
        }
#endif
    }
    if (rc != 0)
    {
        if ((nv->interrupt_line != 0) && (rc == -EBUSY))
        {
            NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                "Tried to get IRQ %d, but another driver\n",
                (unsigned int) nv->interrupt_line);
            nv_printf(NV_DBG_ERRORS, "NVRM: has it and is not sharing it.\n");
            nv_printf(NV_DBG_ERRORS, "NVRM: You may want to verify that no audio driver");
            nv_printf(NV_DBG_ERRORS, " is using the IRQ.\n");
        }
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "request_irq() failed (%d)\n", rc);
        goto failed;
    }

    if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
    {
        rc = os_alloc_mutex(&nvl->isr_bh_unlocked_mutex);
        if (rc != 0)
            goto failed;
        nv_kthread_q_item_init(&nvl->bottom_half_q_item, nvidia_isr_bh_unlocked, (void *)nv);
        rc = nv_kthread_q_init(&nvl->bottom_half_q, nv_device_name);
        if (rc != 0)
            goto failed;
        kthread_init = NV_TRUE;

        rc = nv_kthread_q_init(&nvl->queue.nvk, "nv_queue");
        if (rc)
            goto failed;
        nv->queue = &nvl->queue;
    }

    if (!rm_init_adapter(sp, nv))
    {
        if (!(nv->flags & NV_FLAG_USES_MSIX) &&
            !(nv->flags & NV_FLAG_SOC_DISPLAY))
        {
            free_irq(nv->interrupt_line, (void *) nvl);
        }
        else if (nv->flags & NV_FLAG_SOC_DISPLAY)
        {



        }
#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
        else
        {
            nv_free_msix_irq(nvl);
        }
#endif
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "rm_init_adapter failed, device minor number %d\n",
                      nvl->minor_num);
        rc = -EIO;
        goto failed;
    }

    {
        const NvU8 *uuid = rm_get_gpu_uuid_raw(sp, nv);

        if (uuid != NULL)
        {
#if defined(NV_UVM_ENABLE)
            nv_uvm_notify_start_device(uuid);
#endif
        }
    }

    if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
    {
        nv_acpi_register_notifier(nvl);
    }

    nv->flags |= NV_FLAG_OPEN;

    /*
     * Now that RM init is done, allow dynamic power to control the GPU in FINE
     * mode, if enabled.  (If the mode is COARSE, this unref will do nothing
     * which will cause the GPU to remain powered up.)
     * This is balanced by a FINE ref increment at the beginning of
     * nv_stop_device().
     */
    rm_unref_dynamic_power(sp, nv, NV_DYNAMIC_PM_FINE);

    return 0;

failed:
#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
    if (nv->flags & NV_FLAG_USES_MSI)
    {
        nv->flags &= ~NV_FLAG_USES_MSI;
        NV_PCI_DISABLE_MSI(nvl->pci_dev);
        if(nvl->irq_count)
            NV_KFREE(nvl->irq_count, nvl->num_intr * sizeof(nv_irq_count_info_t));
    }
    if (nv->flags & NV_FLAG_USES_MSIX)
    {
        nv->flags &= ~NV_FLAG_USES_MSIX;
        pci_disable_msix(nvl->pci_dev);
        NV_KFREE(nvl->irq_count, nvl->num_intr*sizeof(nv_irq_count_info_t));
        NV_KFREE(nvl->msix_entries, nvl->num_intr*sizeof(struct msix_entry));
    }

    if (nvl->msix_bh_mutex)
    {
        os_free_mutex(nvl->msix_bh_mutex);
        nvl->msix_bh_mutex = NULL;
    }
#endif

    if (nv->queue && !(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
    {
        nv->queue = NULL;
        nv_kthread_q_stop(&nvl->queue.nvk);
    }

    if (kthread_init && !(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
        nv_kthread_q_stop(&nvl->bottom_half_q);

    if (nvl->isr_bh_unlocked_mutex)
    {
        os_free_mutex(nvl->isr_bh_unlocked_mutex);
        nvl->isr_bh_unlocked_mutex = NULL;
    }

    nv_dev_free_stacks(nvl);

    nv_unregister_ibmnpu_devices(nv);

    if (power_ref)
    {
        rm_unref_dynamic_power(sp, nv, NV_DYNAMIC_PM_COARSE);
    }

    nv_put_rsync_info();

    return rc;
}

/*
 * Makes sure the device is ready for operations and increases nvl->usage_count.
 * Assumes nvl->ldata_lock is held.
 */
static int nv_open_device(nv_state_t *nv, nvidia_stack_t *sp)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    int rc;
    NV_STATUS status;

    if (os_is_vgx_hyper())
    {
        /* fail open if GPU is being unbound */
        if (nv->flags & NV_FLAG_UNBIND_LOCK)
        {
            NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                          "Open failed as GPU is locked for unbind operation\n");
            return -ENODEV;
        }
    }

    NV_DEV_PRINTF(NV_DBG_INFO, nv, "Opening GPU with minor number %d\n",
                  nvl->minor_num);

    status = nv_check_gpu_state(nv);
    if (status == NV_ERR_GPU_IS_LOST)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "Device in removal process\n");
        return -ENODEV;
    }






    if ( ! (nv->flags & NV_FLAG_OPEN))
    {
        /* Sanity check: !NV_FLAG_OPEN requires usage_count == 0 */
        if (NV_ATOMIC_READ(nvl->usage_count) != 0)
        {
            NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                          "Minor device %u is referenced without being open!\n",
                          nvl->minor_num);
            WARN_ON(1);
            return -EBUSY;
        }

        rc = nv_start_device(nv, sp);
        if (rc != 0)
            return rc;
    }
    else if (rm_is_device_sequestered(sp, nv))
    {
        /* Do not increment the usage count of sequestered devices. */
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv, "Device is currently unavailable\n");
        return -EBUSY;
    }

    NV_ATOMIC_INC(nvl->usage_count);
    return 0;
}

static void nv_init_mapping_revocation(nv_linux_state_t *nvl,
                                       struct file *file,
                                       nv_linux_file_private_t *nvlfp,
                                       struct inode *inode)
{
    down(&nvl->mmap_lock);

    /* Set up struct address_space for use with unmap_mapping_range() */
    nv_address_space_init_once(&nvlfp->mapping);
    nvlfp->mapping.host = inode;
    nvlfp->mapping.a_ops = inode->i_mapping->a_ops;
#if defined(NV_ADDRESS_SPACE_HAS_BACKING_DEV_INFO)
    nvlfp->mapping.backing_dev_info = inode->i_mapping->backing_dev_info;
#endif
    file->f_mapping = &nvlfp->mapping;

    /* Add nvlfp to list of open files in nvl for mapping revocation */
    list_add(&nvlfp->entry, &nvl->open_files);

    up(&nvl->mmap_lock);
}

/*
** nvidia_open
**
** nv driver open entry point.  Sessions are created here.
*/
int
nvidia_open(
    struct inode *inode,
    struct file *file
)
{
    nv_state_t *nv = NULL;
    nv_linux_state_t *nvl = NULL;
    int rc = 0;
    nv_linux_file_private_t *nvlfp = NULL;
    nvidia_stack_t *sp = NULL;
    unsigned int i;
    unsigned int k;

    nv_printf(NV_DBG_INFO, "NVRM: nvidia_open...\n");

    nvlfp = nv_alloc_file_private();
    if (nvlfp == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate file private!\n");
        return -ENOMEM;
    }

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc != 0)
    {
        nv_free_file_private(nvlfp);
        return rc;
    }

    for (i = 0; i < NV_FOPS_STACK_INDEX_COUNT; ++i)
    {
        rc = nv_kmem_cache_alloc_stack(&nvlfp->fops_sp[i]);
        if (rc != 0)
        {
            nv_kmem_cache_free_stack(sp);
            for (k = 0; k < i; ++k)
            {
                nv_kmem_cache_free_stack(nvlfp->fops_sp[k]);
            }
            nv_free_file_private(nvlfp);
            return rc;
        }
    }

    NV_SET_FILE_PRIVATE(file, nvlfp);
    nvlfp->sp = sp;

    /* for control device, just jump to its open routine */
    /* after setting up the private data */
    if (nv_is_control_device(inode))
    {
        rc = nvidia_ctl_open(inode, file);
        if (rc != 0)
            goto failed;
        return rc;
    }

    rc = nv_down_read_interruptible(&nv_system_pm_lock);
    if (rc < 0)
        goto failed;

    /* Takes nvl->ldata_lock */
    nvl = find_minor(NV_DEVICE_MINOR_NUMBER(inode));
    if (!nvl)
    {
        rc = -ENODEV;
        up_read(&nv_system_pm_lock);
        goto failed;
    }

    nvlfp->nvptr = nvl;
    nv = NV_STATE_PTR(nvl);

    if ((nv->flags & NV_FLAG_EXCLUDE) != 0)
    {
        char *uuid = rm_get_gpu_uuid(sp, nv);
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "open() not permitted for excluded %s\n",
                      (uuid != NULL) ? uuid : "GPU");
        if (uuid != NULL)
            os_free_mem(uuid);
        rc = -EPERM;
        goto failed1;
    }

    rc = nv_open_device(nv, sp);
    /* Fall-through on error */

    nv_assert_not_in_gpu_exclusion_list(sp, nv);

failed1:
    up(&nvl->ldata_lock);

    up_read(&nv_system_pm_lock);
failed:
    if (rc != 0)
    {
        if (nvlfp != NULL)
        {
            nv_kmem_cache_free_stack(sp);
            for (i = 0; i < NV_FOPS_STACK_INDEX_COUNT; ++i)
            {
                nv_kmem_cache_free_stack(nvlfp->fops_sp[i]);
            }
            nv_free_file_private(nvlfp);
            NV_SET_FILE_PRIVATE(file, NULL);
        }
    }
    else
    {
        nv_init_mapping_revocation(nvl, file, nvlfp, inode);
    }

    return rc;
}

static void validate_numa_shutdown_state(nv_linux_state_t *nvl)
{
    int numa_status = nv_get_numa_status(nvl);
    WARN_ON((numa_status != NV_IOCTL_NUMA_STATUS_OFFLINE) &&
            (numa_status != NV_IOCTL_NUMA_STATUS_DISABLED));
}

void nv_shutdown_adapter(nvidia_stack_t *sp,
                         nv_state_t *nv,
                         nv_linux_state_t *nvl)
{
    validate_numa_shutdown_state(nvl);

    rm_disable_adapter(sp, nv);

    // It's safe to call nv_kthread_q_stop even if queue is not initialized
    nv_kthread_q_stop(&nvl->bottom_half_q);

    if (nv->queue != NULL)
    {
        nv->queue = NULL;
        nv_kthread_q_stop(&nvl->queue.nvk);
    }

    if (nvl->isr_bh_unlocked_mutex)
    {
        os_free_mutex(nvl->isr_bh_unlocked_mutex);
        nvl->isr_bh_unlocked_mutex = NULL;
    }

    if (!(nv->flags & NV_FLAG_USES_MSIX) &&
        !(nv->flags & NV_FLAG_SOC_DISPLAY))
    {
        free_irq(nv->interrupt_line, (void *)nvl);
        if (nv->flags & NV_FLAG_USES_MSI)
        {
            NV_PCI_DISABLE_MSI(nvl->pci_dev);
            if(nvl->irq_count)
                NV_KFREE(nvl->irq_count, nvl->num_intr * sizeof(nv_irq_count_info_t));
        }
    }
    else if (nv->flags & NV_FLAG_SOC_DISPLAY)
    {



    }
#if defined(NV_LINUX_PCIE_MSI_SUPPORTED)
    else
    {
        nv_free_msix_irq(nvl);
        pci_disable_msix(nvl->pci_dev);
        nv->flags &= ~NV_FLAG_USES_MSIX;
        NV_KFREE(nvl->msix_entries, nvl->num_intr*sizeof(struct msix_entry));
        NV_KFREE(nvl->irq_count, nvl->num_intr*sizeof(nv_irq_count_info_t));
    }
#endif

    if (nvl->msix_bh_mutex)
    {
        os_free_mutex(nvl->msix_bh_mutex);
        nvl->msix_bh_mutex = NULL;
    }

    rm_shutdown_adapter(sp, nv);
}

/*
 * Tears down the device on the last file close. Assumes nvl->ldata_lock is
 * held.
 */
static void nv_stop_device(nv_state_t *nv, nvidia_stack_t *sp)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    static int persistence_mode_notice_logged;

    /*
     * The GPU needs to be powered on to go through the teardown sequence.
     * This balances the FINE unref at the end of nv_start_device().
     */
    rm_ref_dynamic_power(sp, nv, NV_DYNAMIC_PM_FINE);

#if defined(NV_UVM_ENABLE)
    {
        const NvU8* uuid;
        // Inform UVM before disabling adapter. Use cached copy
        uuid = nv_get_cached_uuid(nv);
        if (uuid != NULL)
        {
            // this function cannot fail
            nv_uvm_notify_stop_device(uuid);
        }
    }
#endif
    /* Adapter is already shutdown as part of nvidia_pci_remove */
    if (!nv->removed)
    {
        if (nv->flags & NV_FLAG_PERSISTENT_SW_STATE)
        {
            rm_disable_adapter(sp, nv);
        }
        else
        {
            nv_acpi_unregister_notifier(nvl);
            nv_shutdown_adapter(sp, nv, nvl);
        }
    }

    if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
    {
        nv_dev_free_stacks(nvl);
    }

    if ((nv->flags & NV_FLAG_PERSISTENT_SW_STATE) &&
        (!persistence_mode_notice_logged) && (!os_is_vgx_hyper()))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Persistence mode is deprecated and"
                  " will be removed in a future release. Please use"
                  " nvidia-persistenced instead.\n");
        persistence_mode_notice_logged  = 1;
    }

    /* leave INIT flag alone so we don't reinit every time */
    nv->flags &= ~NV_FLAG_OPEN;

    nv_unregister_ibmnpu_devices(nv);

    if (!(nv->flags & NV_FLAG_PERSISTENT_SW_STATE))
    {
        rm_unref_dynamic_power(sp, nv, NV_DYNAMIC_PM_COARSE);
    }
    else
    {
        /* If in legacy persistence mode, only unref FINE refcount. */
        rm_unref_dynamic_power(sp, nv, NV_DYNAMIC_PM_FINE);
    }

    nv_put_rsync_info();
}

/*
 * Decreases nvl->usage_count, stopping the device when it reaches 0. Assumes
 * nvl->ldata_lock is held.
 */
static void nv_close_device(nv_state_t *nv, nvidia_stack_t *sp)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (NV_ATOMIC_READ(nvl->usage_count) == 0)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: Attempting to close unopened minor device %u!\n",
                  nvl->minor_num);
        WARN_ON(1);
        return;
    }

    if (NV_ATOMIC_DEC_AND_TEST(nvl->usage_count))
        nv_stop_device(nv, sp);
}

/*
** nvidia_close
**
** Primary driver close entry point.
*/

static void
nvidia_close_callback(
   nv_linux_file_private_t *nvlfp
)
{
    nv_linux_state_t *nvl = nvlfp->nvptr;
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nvidia_stack_t *sp = nvlfp->sp;
    unsigned int i;
    NvBool bRemove = NV_FALSE;

    rm_cleanup_file_private(sp, nv, &nvlfp->nvfp);

    down(&nvl->mmap_lock);
    list_del(&nvlfp->entry);
    up(&nvl->mmap_lock);

    down(&nvl->ldata_lock);
    nv_close_device(nv, sp);

    bRemove = (!NV_IS_DEVICE_IN_SURPRISE_REMOVAL(nv)) &&
              (NV_ATOMIC_READ(nvl->usage_count) == 0) &&
              rm_get_device_remove_flag(sp, nv->gpu_id);

    for (i = 0; i < NV_FOPS_STACK_INDEX_COUNT; ++i)
    {
        nv_kmem_cache_free_stack(nvlfp->fops_sp[i]);
    }

    nv_free_file_private(nvlfp);

    /*
     * In case of surprise removal of device, we have 2 cases as below:
     *
     * 1> When nvidia_pci_remove is scheduled prior to nvidia_close.
     * nvidia_pci_remove will not destroy linux layer locks & nv linux state
     * struct but will set variable nv->removed for nvidia_close.
     * Once all the clients are closed, last nvidia_close will clean up linux
     * layer locks and nv linux state struct.
     *
     * 2> When nvidia_close is scheduled prior to nvidia_pci_remove.
     * This will be treated as normal working case. nvidia_close will not do
     * any cleanup related to linux layer locks and nv linux state struct.
     * nvidia_pci_remove when scheduled will do necessary cleanup.
     */
    if ((NV_ATOMIC_READ(nvl->usage_count) == 0) && nv->removed)
    {
        nvidia_frontend_remove_device((void *)&nv_fops, nvl);
        nv_lock_destroy_locks(sp, nv);
        NV_KFREE(nvl, sizeof(nv_linux_state_t));
    }
    else
    {
        up(&nvl->ldata_lock);

#if defined(NV_PCI_STOP_AND_REMOVE_BUS_DEVICE)
        if (bRemove)
        {
            NV_PCI_STOP_AND_REMOVE_BUS_DEVICE(nvl->pci_dev);
        }
#endif
    }

    nv_kmem_cache_free_stack(sp);
}

static void nvidia_close_deferred(void *data)
{
    nv_linux_file_private_t *nvlfp = data;

    down_read(&nv_system_pm_lock);

    nvidia_close_callback(nvlfp);

    up_read(&nv_system_pm_lock);
}

int
nvidia_close(
    struct inode *inode,
    struct file *file
)
{
    int rc;
    nv_linux_file_private_t *nvlfp = NV_GET_LINUX_FILE_PRIVATE(file);
    nv_linux_state_t *nvl = nvlfp->nvptr;
    nv_state_t *nv = NV_STATE_PTR(nvl);

    NV_DEV_PRINTF(NV_DBG_INFO, nv, "nvidia_close on GPU with minor number %d\n", NV_DEVICE_MINOR_NUMBER(inode));

    if (nv_is_control_device(inode))
    {
        return nvidia_ctl_close(inode, file);
    }

    NV_SET_FILE_PRIVATE(file, NULL);

    rc = nv_down_read_interruptible(&nv_system_pm_lock);
    if (rc == 0)
    {
        nvidia_close_callback(nvlfp);
        up_read(&nv_system_pm_lock);
    }
    else
    {
        nv_kthread_q_item_init(&nvlfp->deferred_close_q_item,
                               nvidia_close_deferred,
                               nvlfp);
        rc = nv_kthread_q_schedule_q_item(&nv_deferred_close_kthread_q,
                                          &nvlfp->deferred_close_q_item);
        WARN_ON(rc == 0);
    }

    return 0;
}

unsigned int
nvidia_poll(
    struct file *file,
    poll_table  *wait
)
{
    unsigned int mask = 0;
    nv_linux_file_private_t *nvlfp = NV_GET_LINUX_FILE_PRIVATE(file);
    unsigned long eflags;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_FILEP(file);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    NV_STATUS status;

    status = nv_check_gpu_state(nv);
    if (status == NV_ERR_GPU_IS_LOST)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "GPU is lost, skipping nvidia_poll\n");
        return POLLHUP;
    }

    if ((file->f_flags & O_NONBLOCK) == 0)
        poll_wait(file, &nvlfp->waitqueue, wait);

    NV_SPIN_LOCK_IRQSAVE(&nvlfp->fp_lock, eflags);

    if ((nvlfp->event_data_head != NULL) || nvlfp->dataless_event_pending)
    {
        mask = (POLLPRI | POLLIN);
        nvlfp->dataless_event_pending = NV_FALSE;
    }

    NV_SPIN_UNLOCK_IRQRESTORE(&nvlfp->fp_lock, eflags);

    return mask;
}

#define NV_CTL_DEVICE_ONLY(nv)                 \
{                                              \
    if (((nv)->flags & NV_FLAG_CONTROL) == 0)  \
    {                                          \
        status = -EINVAL;                      \
        goto done;                             \
    }                                          \
}

#define NV_ACTUAL_DEVICE_ONLY(nv)              \
{                                              \
    if (((nv)->flags & NV_FLAG_CONTROL) != 0)  \
    {                                          \
        status = -EINVAL;                      \
        goto done;                             \
    }                                          \
}

/*
 * Fills the ci array with the state of num_entries devices. Returns -EINVAL if
 * num_entries isn't big enough to hold all available devices.
 */
static int nvidia_read_card_info(nv_ioctl_card_info_t *ci, size_t num_entries)
{
    nv_state_t *nv;
    nv_linux_state_t *nvl;
    size_t i = 0;
    int rc = 0;

    /* Clear each card's flags field the lazy way */
    memset(ci, 0, num_entries * sizeof(ci[0]));

    LOCK_NV_LINUX_DEVICES();

    if (num_entries < num_nv_devices)
    {
        rc = -EINVAL;
        goto out;
    }

    for (nvl = nv_linux_devices; nvl && i < num_entries; nvl = nvl->next)
    {
        nv = NV_STATE_PTR(nvl);

        /* We do not include excluded GPUs in the list... */
        if ((nv->flags & NV_FLAG_EXCLUDE) != 0)
            continue;

        ci[i].valid              = NV_TRUE;
        ci[i].pci_info.domain    = nv->pci_info.domain;
        ci[i].pci_info.bus       = nv->pci_info.bus;
        ci[i].pci_info.slot      = nv->pci_info.slot;
        ci[i].pci_info.vendor_id = nv->pci_info.vendor_id;
        ci[i].pci_info.device_id = nv->pci_info.device_id;
        ci[i].gpu_id             = nv->gpu_id;
        ci[i].interrupt_line     = nv->interrupt_line;
        ci[i].reg_address        = nv->regs->cpu_address;
        ci[i].reg_size           = nv->regs->size;
        ci[i].minor_number       = nvl->minor_num;
        if (nv_dev_is_pci(nvl->dev))
        {
            ci[i].fb_address         = nv->fb->cpu_address;
            ci[i].fb_size            = nv->fb->size;
        }
        i++;
    }

out:
    UNLOCK_NV_LINUX_DEVICES();
    return rc;
}

int
nvidia_ioctl(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long i_arg)
{
    NV_STATUS rmStatus;
    int status = 0;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_FILEP(file);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nv_linux_file_private_t *nvlfp = NV_GET_LINUX_FILE_PRIVATE(file);
    nvidia_stack_t *sp = NULL;
    nv_ioctl_xfer_t ioc_xfer;
    void *arg_ptr = (void *) i_arg;
    void *arg_copy = NULL;
    size_t arg_size = 0;
    int arg_cmd;

    nv_printf(NV_DBG_INFO, "NVRM: ioctl(0x%x, 0x%x, 0x%x)\n",
        _IOC_NR(cmd), (unsigned int) i_arg, _IOC_SIZE(cmd));

    status = nv_down_read_interruptible(&nv_system_pm_lock);
    if (status < 0)
        return status;

    down(&nvlfp->fops_sp_lock[NV_FOPS_STACK_INDEX_IOCTL]);
    sp = nvlfp->fops_sp[NV_FOPS_STACK_INDEX_IOCTL];

    rmStatus = nv_check_gpu_state(nv);
    if (rmStatus == NV_ERR_GPU_IS_LOST)
    {
        nv_printf(NV_DBG_INFO, "NVRM: GPU is lost, skipping nvidia_ioctl\n");
        status = -EINVAL;
        goto done;
    }

    arg_size = _IOC_SIZE(cmd);
    arg_cmd  = _IOC_NR(cmd);

    if (arg_cmd == NV_ESC_IOCTL_XFER_CMD)
    {
        if (arg_size != sizeof(nv_ioctl_xfer_t))
        {
            nv_printf(NV_DBG_ERRORS,
                    "NVRM: invalid ioctl XFER structure size!\n");
            status = -EINVAL;
            goto done;
        }

        if (NV_COPY_FROM_USER(&ioc_xfer, arg_ptr, sizeof(ioc_xfer)))
        {
            nv_printf(NV_DBG_ERRORS,
                    "NVRM: failed to copy in ioctl XFER data!\n");
            status = -EFAULT;
            goto done;
        }

        arg_cmd  = ioc_xfer.cmd;
        arg_size = ioc_xfer.size;
        arg_ptr  = NvP64_VALUE(ioc_xfer.ptr);

        if (arg_size > NV_ABSOLUTE_MAX_IOCTL_SIZE)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: invalid ioctl XFER size!\n");
            status = -EINVAL;
            goto done;
        }
    }

    NV_KMALLOC(arg_copy, arg_size);
    if (arg_copy == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate ioctl memory\n");
        status = -ENOMEM;
        goto done;
    }

    if (NV_COPY_FROM_USER(arg_copy, arg_ptr, arg_size))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to copy in ioctl data!\n");
        status = -EFAULT;
        goto done;
    }

    switch (arg_cmd)
    {
        case NV_ESC_QUERY_DEVICE_INTR:
        {
            nv_ioctl_query_device_intr *query_intr = arg_copy;

            NV_ACTUAL_DEVICE_ONLY(nv);

            if ((arg_size < sizeof(*query_intr)) ||
                (!nv->regs->map))
            {
                status = -EINVAL;
                goto done;
            }

            query_intr->intrStatus =
                *(nv->regs->map + (NV_RM_DEVICE_INTR_ADDRESS >> 2));
            query_intr->status = NV_OK;
            break;
        }

        /* pass out info about the card */
        case NV_ESC_CARD_INFO:
        {
            size_t num_arg_devices = arg_size / sizeof(nv_ioctl_card_info_t);

            NV_CTL_DEVICE_ONLY(nv);

            status = nvidia_read_card_info(arg_copy, num_arg_devices);
            break;
        }

        case NV_ESC_ATTACH_GPUS_TO_FD:
        {
            size_t num_arg_gpus = arg_size / sizeof(NvU32);
            size_t i;

            NV_CTL_DEVICE_ONLY(nv);

            if (num_arg_gpus == 0 || nvlfp->num_attached_gpus != 0 ||
                arg_size % sizeof(NvU32) != 0)
            {
                status = -EINVAL;
                goto done;
            }

            NV_KMALLOC(nvlfp->attached_gpus, arg_size);
            if (nvlfp->attached_gpus == NULL)
            {
                status = -ENOMEM;
                goto done;
            }
            memcpy(nvlfp->attached_gpus, arg_copy, arg_size);
            nvlfp->num_attached_gpus = num_arg_gpus;

            for (i = 0; i < nvlfp->num_attached_gpus; i++)
            {
                if (nvlfp->attached_gpus[i] == 0)
                {
                    continue;
                }

                if (nvidia_dev_get(nvlfp->attached_gpus[i], sp))
                {
                    while (i--)
                    {
                        if (nvlfp->attached_gpus[i] != 0)
                            nvidia_dev_put(nvlfp->attached_gpus[i], sp);
                    }
                    NV_KFREE(nvlfp->attached_gpus, arg_size);
                    nvlfp->num_attached_gpus = 0;

                    status = -EINVAL;
                    break;
                }
            }

            break;
        }

        case NV_ESC_CHECK_VERSION_STR:
        {
            NV_CTL_DEVICE_ONLY(nv);

            rmStatus = rm_perform_version_check(sp, arg_copy, arg_size);
            status = ((rmStatus == NV_OK) ? 0 : -EINVAL);
            break;
        }

        case NV_ESC_SYS_PARAMS:
        {
            nv_ioctl_sys_params_t *api = arg_copy;

            NV_CTL_DEVICE_ONLY(nv);

            if (arg_size != sizeof(nv_ioctl_sys_params_t))
            {
                status = -EINVAL;
                goto done;
            }

            /* numa_memblock_size should only be set once */
            if (nvl->numa_memblock_size == 0)
            {
                nvl->numa_memblock_size = api->memblock_size;
            }
            else
            {
                status = (nvl->numa_memblock_size == api->memblock_size) ?
                    0 : -EBUSY;
                goto done;
            }
            break;
        }

        case NV_ESC_NUMA_INFO:
        {
            nv_ioctl_numa_info_t *api = arg_copy;
            rmStatus = NV_OK;

            NV_ACTUAL_DEVICE_ONLY(nv);

            if (arg_size != sizeof(nv_ioctl_numa_info_t))
            {
                status = -EINVAL;
                goto done;
            }

            api->offline_addresses.numEntries = 
                ARRAY_SIZE(api->offline_addresses.addresses),

            rmStatus = rm_get_gpu_numa_info(sp, nv,
                &(api->nid),
                &(api->numa_mem_addr),
                &(api->numa_mem_size),
                 (api->offline_addresses.addresses),
                &(api->offline_addresses.numEntries));
            if (rmStatus != NV_OK)
            {
                status = -EBUSY;
                goto done;
            }

            api->status = nv_get_numa_status(nvl);
            api->memblock_size = nv_ctl_device.numa_memblock_size;
            break;
        }

        case NV_ESC_SET_NUMA_STATUS:
        {
            nv_ioctl_set_numa_status_t *api = arg_copy;
            rmStatus = NV_OK;

            if (!NV_IS_SUSER())
            {
                status = -EACCES;
                goto done;
            }

            NV_ACTUAL_DEVICE_ONLY(nv);

            if (arg_size != sizeof(nv_ioctl_set_numa_status_t))
            {
                status = -EINVAL;
                goto done;
            }

            /*
             * The nv_linux_state_t for the device needs to be locked
             * in order to prevent additional open()/close() calls from
             * manipulating the usage count for the device while we
             * determine if NUMA state can be changed.
             */
            down(&nvl->ldata_lock);

            if (nv_get_numa_status(nvl) != api->status)
            {
                if (api->status == NV_IOCTL_NUMA_STATUS_OFFLINE_IN_PROGRESS)
                {
                    /*
                     * Only the current client should have an open file
                     * descriptor for the device, to allow safe offlining.
                     */
                    if (NV_ATOMIC_READ(nvl->usage_count) > 1)
                    {
                        status = -EBUSY;
                        goto unlock;
                    }
                    else
                    {
                        /*
                         * If this call fails, it indicates that RM
                         * is not ready to offline memory, and we should keep
                         * the current NUMA status of ONLINE.
                         */
                        rmStatus = rm_gpu_numa_offline(sp, nv);
                        if (rmStatus != NV_OK)
                        {
                            status = -EBUSY;
                            goto unlock;
                        }
                    }
                }

                status = nv_set_numa_status(nvl, api->status);
                if (status < 0)
                {
                    if (api->status == NV_IOCTL_NUMA_STATUS_OFFLINE_IN_PROGRESS)
                        (void) rm_gpu_numa_online(sp, nv);
                    goto unlock;
                }

                if (api->status == NV_IOCTL_NUMA_STATUS_ONLINE)
                {
                    rmStatus = rm_gpu_numa_online(sp, nv);
                    if (rmStatus != NV_OK)
                    {
                        status = -EBUSY;
                        goto unlock;
                    }
                }
            }

unlock:
            up(&nvl->ldata_lock);

            break;
        }

        case NV_ESC_EXPORT_TO_DMABUF_FD:
        {
            nv_ioctl_export_to_dma_buf_fd_t *params = arg_copy;

            if (arg_size != sizeof(nv_ioctl_export_to_dma_buf_fd_t))
            {
                status = -EINVAL;
                goto done;
            }

            NV_ACTUAL_DEVICE_ONLY(nv);

            params->status = nv_dma_buf_export(nv, params);

            break;
        }

        default:
            rmStatus = rm_ioctl(sp, nv, &nvlfp->nvfp, arg_cmd, arg_copy, arg_size);
            status = ((rmStatus == NV_OK) ? 0 : -EINVAL);
            break;
    }

done:
    up(&nvlfp->fops_sp_lock[NV_FOPS_STACK_INDEX_IOCTL]);

    up_read(&nv_system_pm_lock);

    if (arg_copy != NULL)
    {
        if (status != -EFAULT)
        {
            if (NV_COPY_TO_USER(arg_ptr, arg_copy, arg_size))
            {
                nv_printf(NV_DBG_ERRORS, "NVRM: failed to copy out ioctl data\n");
                status = -EFAULT;
            }
        }
        NV_KFREE(arg_copy, arg_size);
    }

    return status;
}

irqreturn_t
nvidia_isr_msix(
    int   irq,
    void *arg
)
{
    irqreturn_t ret;
    nv_linux_state_t *nvl = (void *) arg;

    // nvidia_isr_msix() is called for each of the MSI-X vectors and they can
    // run in parallel on different CPUs (cores), but this is not currently
    // supported by nvidia_isr() and its children. As a big hammer fix just
    // spinlock around the nvidia_isr() call to serialize them.
    //
    // At this point interrupts are disabled on the CPU running our ISR (see
    // comments for nv_default_irq_flags()) so a plain spinlock is enough.
    NV_SPIN_LOCK(&nvl->msix_isr_lock);

    ret = nvidia_isr(irq, arg);

    NV_SPIN_UNLOCK(&nvl->msix_isr_lock);

    return ret;
}

/*
 * driver receives an interrupt
 *    if someone waiting, then hand it off.
 */
irqreturn_t
nvidia_isr(
    int   irq,
    void *arg
)
{
    nv_linux_state_t *nvl = (void *) arg;
    nv_state_t *nv = NV_STATE_PTR(nvl);
    NvU32 need_to_run_bottom_half_gpu_lock_held = 0;
    NvBool rm_handled = NV_FALSE, uvm_handled = NV_FALSE, rm_fault_handling_needed = NV_FALSE;
    NvU32 rm_serviceable_fault_cnt = 0;
    NvU32 sec, usec;
    NvU16 index = 0;
    NvU64 currentTime = 0;
    NvBool found_irq = NV_FALSE;

    rm_gpu_copy_mmu_faults_unlocked(nvl->sp[NV_DEV_STACK_ISR], nv, &rm_serviceable_fault_cnt);
    rm_fault_handling_needed = (rm_serviceable_fault_cnt != 0);

#if defined (NV_UVM_ENABLE)
    //
    // Returns NV_OK if the UVM driver handled the interrupt
    //
    // Returns NV_ERR_NO_INTR_PENDING if the interrupt is not for
    // the UVM driver.
    //
    // Returns NV_WARN_MORE_PROCESSING_REQUIRED if the UVM top-half ISR was
    // unable to get its lock(s), due to other (UVM) threads holding them.
    //
    // RM can normally treat NV_WARN_MORE_PROCESSING_REQUIRED the same as
    // NV_ERR_NO_INTR_PENDING, but in some cases the extra information may
    // be helpful.
    //
    if (nv_uvm_event_interrupt(nv_get_cached_uuid(nv)) == NV_OK)
        uvm_handled = NV_TRUE;
#endif

    rm_handled = rm_isr(nvl->sp[NV_DEV_STACK_ISR], nv,
                        &need_to_run_bottom_half_gpu_lock_held);

    /* Replicating the logic in linux kernel to track unhandled interrupt crossing a threshold */
    if ((nv->flags & NV_FLAG_USES_MSI) || (nv->flags & NV_FLAG_USES_MSIX))
    {
        if (nvl->irq_count != NULL)
        {
            for (index = 0; index < nvl->current_num_irq_tracked; index++)
            {
                if (nvl->irq_count[index].irq == irq)
                {
                    found_irq = NV_TRUE;
                    break;
                }

                found_irq = NV_FALSE;
            }

            if (!found_irq && nvl->current_num_irq_tracked < nvl->num_intr)
            {
                index = nvl->current_num_irq_tracked;
                nvl->irq_count[index].irq = irq;
                nvl->current_num_irq_tracked++;
                found_irq = NV_TRUE;
            }

            if (found_irq)
            {
                nvl->irq_count[index].total++;

                if(rm_handled == NV_FALSE)
                {
                    os_get_current_time(&sec, &usec);
                    currentTime = ((NvU64)sec) * 1000000 + (NvU64)usec;

                    /* Reset unhandled count if it's been more than 0.1 seconds since the last unhandled IRQ */
                    if ((currentTime - nvl->irq_count[index].last_unhandled) > RM_UNHANDLED_TIMEOUT_US)
                        nvl->irq_count[index].unhandled = 1;
                    else
                        nvl->irq_count[index].unhandled++;

                    nvl->irq_count[index].last_unhandled = currentTime;
                    rm_handled = NV_TRUE;
                }

                if (nvl->irq_count[index].total >= RM_THRESHOLD_TOTAL_IRQ_COUNT)
                {
                    if (nvl->irq_count[index].unhandled > RM_THRESHOLD_UNAHNDLED_IRQ_COUNT)
                        nv_printf(NV_DBG_ERRORS,"NVRM: Going over RM unhandled interrupt threshold for irq %d\n", irq);

                    nvl->irq_count[index].total = 0;
                    nvl->irq_count[index].unhandled = 0;
                    nvl->irq_count[index].last_unhandled = 0;
                }
            }
            else
                nv_printf(NV_DBG_ERRORS,"NVRM: IRQ number out of valid range\n");
        }
    }

    if (need_to_run_bottom_half_gpu_lock_held)
    {
        return IRQ_WAKE_THREAD;
    }
    else
    {
        //
        // If rm_isr does not need to run a bottom half and mmu_faults_copied
        // indicates that bottom half is needed, then we enqueue a kthread based
        // bottom half, as this specific bottom_half will acquire the GPU lock
        //
        if (rm_fault_handling_needed)
            nv_kthread_q_schedule_q_item(&nvl->bottom_half_q, &nvl->bottom_half_q_item);
    }

    return IRQ_RETVAL(rm_handled || uvm_handled || rm_fault_handling_needed);
}

irqreturn_t
nvidia_isr_kthread_bh(
    int irq,
    void *data
)
{
    return nvidia_isr_common_bh(data);
}

irqreturn_t
nvidia_isr_msix_kthread_bh(
    int irq,
    void *data
)
{
    NV_STATUS status;
    irqreturn_t ret;
    nv_state_t *nv = (nv_state_t *) data;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    //
    // Synchronize kthreads servicing bottom halves for different MSI-X vectors
    // as they share same pre-allocated alt-stack.
    //
    status = os_acquire_mutex(nvl->msix_bh_mutex);
    // os_acquire_mutex can only fail if we cannot sleep and we can
    WARN_ON(status != NV_OK);

    ret = nvidia_isr_common_bh(data);

    os_release_mutex(nvl->msix_bh_mutex);

    return ret;
}

static irqreturn_t
nvidia_isr_common_bh(
    void *data
)
{
    nv_state_t *nv = (nv_state_t *) data;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    nvidia_stack_t *sp = nvl->sp[NV_DEV_STACK_ISR_BH];
    NV_STATUS status;

    status = nv_check_gpu_state(nv);
    if (status == NV_ERR_GPU_IS_LOST)
    {
        nv_printf(NV_DBG_INFO, "NVRM: GPU is lost, skipping ISR bottom half\n");
    }
    else
    {
        rm_isr_bh(sp, nv);
    }

    return IRQ_HANDLED;
}

static void
nvidia_isr_bh_unlocked(
    void * args
)
{
    nv_state_t *nv = (nv_state_t *) args;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    nvidia_stack_t *sp;
    NV_STATUS status;

    //
    // Synchronize kthreads servicing unlocked bottom half as they
    // share same pre-allocated stack for alt-stack
    //
    status = os_acquire_mutex(nvl->isr_bh_unlocked_mutex);
    if (status != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: %s: Unable to take bottom_half mutex!\n",
                  __FUNCTION__);
        WARN_ON(1);
    }

    sp = nvl->sp[NV_DEV_STACK_ISR_BH_UNLOCKED];

    status = nv_check_gpu_state(nv);
    if (status == NV_ERR_GPU_IS_LOST)
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: GPU is lost, skipping unlocked ISR bottom half\n");
    }
    else
    {
        rm_isr_bh_unlocked(sp, nv);
    }

    os_release_mutex(nvl->isr_bh_unlocked_mutex);
}

static void
nvidia_rc_timer_callback(
    struct nv_timer *nv_timer
)
{
    nv_linux_state_t *nvl = container_of(nv_timer, nv_linux_state_t, rc_timer);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nvidia_stack_t *sp = nvl->sp[NV_DEV_STACK_TIMER];
    NV_STATUS status;

    status = nv_check_gpu_state(nv);
    if (status == NV_ERR_GPU_IS_LOST)
    {
        nv_printf(NV_DBG_INFO,
            "NVRM: GPU is lost, skipping device timer callbacks\n");
        return;
    }

    if (rm_run_rc_callback(sp, nv) == NV_OK)
    {
        // set another timeout 1 sec in the future:
        mod_timer(&nvl->rc_timer.kernel_timer, jiffies + HZ);
    }
}

/*
** nvidia_ctl_open
**
** nv control driver open entry point.  Sessions are created here.
*/
static int
nvidia_ctl_open(
    struct inode *inode,
    struct file *file
)
{
    nv_linux_state_t *nvl = &nv_ctl_device;
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nv_linux_file_private_t *nvlfp = NV_GET_LINUX_FILE_PRIVATE(file);

    nv_printf(NV_DBG_INFO, "NVRM: nvidia_ctl_open\n");

    down(&nvl->ldata_lock);

    /* save the nv away in file->private_data */
    nvlfp->nvptr = nvl;

    if (NV_ATOMIC_READ(nvl->usage_count) == 0)
    {
        nv->flags |= (NV_FLAG_OPEN | NV_FLAG_CONTROL);
    }

    NV_ATOMIC_INC(nvl->usage_count);
    up(&nvl->ldata_lock);

    return 0;
}


/*
** nvidia_ctl_close
*/
static int
nvidia_ctl_close(
    struct inode *inode,
    struct file *file
)
{
    nv_alloc_t *at, *next;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_FILEP(file);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nv_linux_file_private_t *nvlfp = NV_GET_LINUX_FILE_PRIVATE(file);
    nvidia_stack_t *sp = nvlfp->sp;
    unsigned int i;

    nv_printf(NV_DBG_INFO, "NVRM: nvidia_ctl_close\n");

    down(&nvl->ldata_lock);
    if (NV_ATOMIC_DEC_AND_TEST(nvl->usage_count))
    {
        nv->flags &= ~NV_FLAG_OPEN;
    }
    up(&nvl->ldata_lock);

    rm_cleanup_file_private(sp, nv, &nvlfp->nvfp);

    if (nvlfp->free_list != NULL)
    {
        at = nvlfp->free_list;
        while (at != NULL)
        {
            next = at->next;
            if (at->pid == os_get_current_process())
                NV_PRINT_AT(NV_DBG_MEMINFO, at);
            nv_free_pages(nv, at->num_pages,
                          at->flags.contig,
                          at->cache_type,
                          (void *)at);
            at = next;
        }
    }

    if (nvlfp->num_attached_gpus != 0)
    {
        size_t i;

        for (i = 0; i < nvlfp->num_attached_gpus; i++)
        {
            if (nvlfp->attached_gpus[i] != 0)
                nvidia_dev_put(nvlfp->attached_gpus[i], sp);
        }

        NV_KFREE(nvlfp->attached_gpus, sizeof(NvU32) * nvlfp->num_attached_gpus);
        nvlfp->num_attached_gpus = 0;
    }

    for (i = 0; i < NV_FOPS_STACK_INDEX_COUNT; ++i)
    {
        nv_kmem_cache_free_stack(nvlfp->fops_sp[i]);
    }

    nv_free_file_private(nvlfp);
    NV_SET_FILE_PRIVATE(file, NULL);

    nv_kmem_cache_free_stack(sp);

    return 0;
}


void NV_API_CALL
nv_set_dma_address_size(
    nv_state_t  *nv,
    NvU32       phys_addr_bits
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NvU64 start_addr = nv_get_dma_start_address(nv);
    NvU64 new_mask = (((NvU64)1) << phys_addr_bits) - 1;

    nvl->dma_dev.addressable_range.limit = start_addr + new_mask;

    /*
     * The only scenario in which we definitely should not update the DMA mask
     * is on POWER, when using TCE bypass mode (see nv_get_dma_start_address()
     * for details), since the meaning of the DMA mask is overloaded in that
     * case.
     */
    if (!nvl->tce_bypass_enabled)
    {
        dma_set_mask(&nvl->pci_dev->dev, new_mask);
        /* Certain kernels have a bug which causes pci_set_consistent_dma_mask
         * to call GPL sme_active symbol, this bug has already been fixed in a
         * minor release update but detect the failure scenario here to prevent
         * an installation regression */
#if !NV_IS_EXPORT_SYMBOL_GPL_sme_active
        dma_set_coherent_mask(&nvl->pci_dev->dev, new_mask);
#endif
    }
}

static NvUPtr
nv_map_guest_pages(nv_alloc_t *at,
                   NvU64 address,
                   NvU32 page_count,
                   NvU32 page_idx)
{
    struct page **pages;
    NvU32 j;
    NvUPtr virt_addr;

    NV_KMALLOC(pages, sizeof(struct page *) * page_count);
    if (pages == NULL)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: failed to allocate vmap() page descriptor table!\n");
        return 0;
    }

    for (j = 0; j < page_count; j++)
    {
        pages[j] = NV_GET_PAGE_STRUCT(at->page_table[page_idx+j]->phys_addr);
    }

    virt_addr = nv_vm_map_pages(pages, page_count,
        at->cache_type == NV_MEMORY_CACHED, at->flags.unencrypted);
    NV_KFREE(pages, sizeof(struct page *) * page_count);

    return virt_addr;
}

NV_STATUS NV_API_CALL
nv_alias_pages(
    nv_state_t *nv,
    NvU32 page_cnt,
    NvU32 contiguous,
    NvU32 cache_type,
    NvU64 guest_id,
    NvU64 *pte_array,
    void **priv_data
)
{
    nv_alloc_t *at;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NvU32 i=0;
    nvidia_pte_t *page_ptr = NULL;

    at = nvos_create_alloc(nvl->dev, page_cnt);

    if (at == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    at->cache_type = cache_type;
    if (contiguous)
        at->flags.contig = NV_TRUE;
#if defined(NVCPU_AARCH64)
    if (at->cache_type != NV_MEMORY_CACHED)
        at->flags.aliased = NV_TRUE;
#endif

    at->flags.guest = NV_TRUE;

    at->order = get_order(at->num_pages * PAGE_SIZE);

    for (i=0; i < at->num_pages; ++i)
    {
        page_ptr = at->page_table[i];

        if (contiguous && i>0)
        {
            page_ptr->dma_addr = pte_array[0] + (i << PAGE_SHIFT);
        }
        else
        {
            page_ptr->dma_addr  = pte_array[i];
        }

        page_ptr->phys_addr = page_ptr->dma_addr;

        /* aliased pages will be mapped on demand. */
        page_ptr->virt_addr = 0x0;
    }

    at->guest_id = guest_id;
    *priv_data = at;
    NV_ATOMIC_INC(at->usage_count);

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    return NV_OK;
}

/*
 *   This creates a dummy nv_alloc_t for peer IO mem, so that it can
 *   be mapped using NvRmMapMemory.
 */
NV_STATUS NV_API_CALL nv_register_peer_io_mem(
    nv_state_t *nv,
    NvU64      *phys_addr,
    NvU64       page_count,
    void      **priv_data
)
{
    nv_alloc_t *at;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NvU64 i;
    NvU64 addr;

    at = nvos_create_alloc(nvl->dev, page_count);

    if (at == NULL)
        return NV_ERR_NO_MEMORY;

    // IO regions should be uncached and contiguous
    at->cache_type = NV_MEMORY_UNCACHED;
    at->flags.contig = NV_TRUE;
#if defined(NVCPU_AARCH64)
    at->flags.aliased = NV_TRUE;
#endif
    at->flags.peer_io = NV_TRUE;

    at->order = get_order(at->num_pages * PAGE_SIZE);

    addr = phys_addr[0];

    for (i = 0; i < page_count; i++)
    {
        at->page_table[i]->phys_addr = addr;
        addr += PAGE_SIZE;
    }

    // No struct page array exists for this memory.
    at->user_pages = NULL;

    *priv_data = at;

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    return NV_OK;
}

void NV_API_CALL nv_unregister_peer_io_mem(
    nv_state_t *nv,
    void       *priv_data
)
{
    nv_alloc_t *at = priv_data;

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    nvos_free_alloc(at);
}

/*
 * By registering user pages, we create a dummy nv_alloc_t for it, so that the
 * rest of the RM can treat it like any other alloc.
 *
 * This also converts the page array to an array of physical addresses.
 */
NV_STATUS NV_API_CALL nv_register_user_pages(
    nv_state_t *nv,
    NvU64       page_count,
    NvU64      *phys_addr,
    void       *import_priv,
    void      **priv_data
)
{
    nv_alloc_t *at;
    NvU64 i;
    struct page **user_pages;
    nv_linux_state_t *nvl;
    nvidia_pte_t *page_ptr;

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM: nv_register_user_pages: 0x%x\n", page_count);
    user_pages = *priv_data;
    nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    at = nvos_create_alloc(nvl->dev, page_count);

    if (at == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    /*
     * Anonymous memory currently must be write-back cacheable, and we can't
     * enforce contiguity.
     */
    at->cache_type = NV_MEMORY_UNCACHED;
#if defined(NVCPU_AARCH64)
    at->flags.aliased = NV_TRUE;
#endif

    at->flags.user = NV_TRUE;

    at->order = get_order(at->num_pages * PAGE_SIZE);

    for (i = 0; i < page_count; i++)
    {
        /*
         * We only assign the physical address and not the DMA address, since
         * this allocation hasn't been DMA-mapped yet.
         */
        page_ptr = at->page_table[i];
        page_ptr->phys_addr = page_to_phys(user_pages[i]);

        phys_addr[i] = page_ptr->phys_addr;
    }

    /* Save off the user pages array to be restored later */
    at->user_pages = user_pages;

    /* Save off the import private data to be returned later */
    if (import_priv != NULL)
    {
        at->import_priv = import_priv;
    }

    *priv_data = at;

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    return NV_OK;
}

void NV_API_CALL nv_unregister_user_pages(
    nv_state_t *nv,
    NvU64       page_count,
    void      **import_priv,
    void      **priv_data
)
{
    nv_alloc_t *at = *priv_data;

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM: nv_unregister_user_pages: 0x%x\n", page_count);

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    WARN_ON(!at->flags.user);

    /* Restore the user pages array for the caller to handle */
    *priv_data = at->user_pages;

    /* Return the import private data for the caller to handle */
    if (import_priv != NULL)
    {
        *import_priv = at->import_priv;
    }

    nvos_free_alloc(at);
}

/*
 * This creates a dummy nv_alloc_t for existing physical allocations, so
 * that it can be mapped using NvRmMapMemory and BAR2 code path.
 */
NV_STATUS NV_API_CALL nv_register_phys_pages(
    nv_state_t *nv,
    NvU64      *phys_addr,
    NvU64       page_count,
    NvU32       cache_type,
    void      **priv_data
)
{
    nv_alloc_t *at;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NvU64 i;
    NvU64 addr;

    at = nvos_create_alloc(nvl->dev, page_count);

    if (at == NULL)
        return NV_ERR_NO_MEMORY;
    /*
     * Setting memory flags to cacheable and discontiguous.
     */
    at->cache_type = cache_type;

    /*
     * Only physical address is available so we don't try to reuse existing
     * mappings
     */
    at->flags.physical = NV_TRUE;

    at->order = get_order(at->num_pages * PAGE_SIZE);

    for (i = 0, addr = phys_addr[0]; i < page_count; addr = phys_addr[++i])
    {
        at->page_table[i]->phys_addr = addr;
    }

    at->user_pages = NULL;
    *priv_data = at;

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    return NV_OK;
}

NV_STATUS NV_API_CALL nv_register_sgt(
    nv_state_t *nv,
    NvU64      *phys_addr,
    NvU64       page_count,
    NvU32       cache_type,
    void      **priv_data,
    struct sg_table *import_sgt,
    void       *import_priv
)
{
    nv_alloc_t *at;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    unsigned int i, j = 0;
    NvU64 sg_addr, sg_off, sg_len;
    struct scatterlist *sg;

    at = nvos_create_alloc(nvl->dev, page_count);

    if (at == NULL)
        return NV_ERR_NO_MEMORY;

    /* Populate phys addrs with DMA addrs from SGT */
    for_each_sg(import_sgt->sgl, sg, import_sgt->nents, i)
    {
        /*
         * It is possible for dma_map_sg() to merge scatterlist entries, so
         * make sure we account for that here.
         */
        for (sg_addr = sg_dma_address(sg), sg_len = sg_dma_len(sg), sg_off = 0;
             (sg_off < sg_len) && (j < page_count);
             sg_off += PAGE_SIZE, j++)
        {
            phys_addr[j] = sg_addr + sg_off;
        }
    }

    /*
     * Setting memory flags to cacheable and discontiguous.
     */
    at->cache_type = cache_type;

    at->import_sgt = import_sgt;

    /* Save off the import private data to be returned later */
    if (import_priv != NULL)
    {
        at->import_priv = import_priv;
    }

    at->order = get_order(at->num_pages * PAGE_SIZE);

    *priv_data = at;

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    return NV_OK;
}

void NV_API_CALL nv_unregister_sgt(
    nv_state_t *nv,
    struct sg_table **import_sgt,
    void **import_priv,
    void  *priv_data
)
{
    nv_alloc_t *at = priv_data;

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM: nv_unregister_sgt\n");

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    /* Restore the imported SGT for the caller to handle */
    *import_sgt = at->import_sgt;

    /* Return the import private data for the caller to handle */
    if (import_priv != NULL)
    {
        *import_priv = at->import_priv;
    }

    nvos_free_alloc(at);
}

void NV_API_CALL nv_unregister_phys_pages(
    nv_state_t *nv,
    void       *priv_data
)
{
    nv_alloc_t *at = priv_data;
    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    nvos_free_alloc(at);
}

NV_STATUS NV_API_CALL nv_get_num_phys_pages(
    void    *pAllocPrivate,
    NvU32   *pNumPages
)
{
    nv_alloc_t *at = pAllocPrivate;

    if (!pNumPages) {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *pNumPages = at->num_pages;

    return NV_OK;
}

NV_STATUS NV_API_CALL nv_get_phys_pages(
    void    *pAllocPrivate,
    void    *pPages,
    NvU32   *pNumPages
)
{
    nv_alloc_t *at = pAllocPrivate;
    struct page **pages = (struct page **)pPages;
    NvU32 page_count;
    int i;

    if (!pNumPages || !pPages) {
        return NV_ERR_INVALID_ARGUMENT;
    }

    page_count = NV_MIN(*pNumPages, at->num_pages);

    for (i = 0; i < page_count; i++) {
        pages[i] = NV_GET_PAGE_STRUCT(at->page_table[i]->phys_addr);
    }

    *pNumPages = page_count;

    return NV_OK;
}

void* NV_API_CALL nv_alloc_kernel_mapping(
    nv_state_t *nv,
    void       *pAllocPrivate,
    NvU64       pageIndex,
    NvU32       pageOffset,
    NvU64       size,
    void      **pPrivate
)
{
    nv_alloc_t *at = pAllocPrivate;
    NvU32 j, page_count;
    NvUPtr virt_addr;
    struct page **pages;
    NvBool isUserAllocatedMem;

    //
    // For User allocated memory (like ErrorNotifier's) which is NOT allocated
    // nor owned by RM, the RM driver just stores the physical address
    // corresponding to that memory and does not map it until required.
    // In that case, in page tables the virt_addr == 0, so first we need to map
    // those pages to obtain virtual address.
    //
    isUserAllocatedMem = at->flags.user &&
                        !at->page_table[pageIndex]->virt_addr &&
                         at->page_table[pageIndex]->phys_addr;

    //
    // User memory may NOT have kernel VA. So check this and fallback to else
    // case to create one.
    //
    if (((size + pageOffset) <= PAGE_SIZE) &&
         !at->flags.guest && !at->flags.aliased &&
         !isUserAllocatedMem && !at->flags.physical)
    {
        *pPrivate = NULL;
        return (void *)(at->page_table[pageIndex]->virt_addr + pageOffset);
    }
    else
    {
        size += pageOffset;
        page_count = (size >> PAGE_SHIFT) + ((size & ~NV_PAGE_MASK) ? 1 : 0);

        if (at->flags.guest)
        {
            virt_addr = nv_map_guest_pages(at,
                                           nv->bars[NV_GPU_BAR_INDEX_REGS].cpu_address,
                                           page_count, pageIndex);
        }
        else
        {
            NV_KMALLOC(pages, sizeof(struct page *) * page_count);
            if (pages == NULL)
            {
                nv_printf(NV_DBG_ERRORS,
                          "NVRM: failed to allocate vmap() page descriptor table!\n");
                return NULL;
            }

            for (j = 0; j < page_count; j++)
                pages[j] = NV_GET_PAGE_STRUCT(at->page_table[pageIndex+j]->phys_addr);

            virt_addr = nv_vm_map_pages(pages, page_count,
                at->cache_type == NV_MEMORY_CACHED, at->flags.unencrypted);
            NV_KFREE(pages, sizeof(struct page *) * page_count);
        }

        if (virt_addr == 0)
        {
            nv_printf(NV_DBG_ERRORS, "NVRM: failed to map pages!\n");
            return NULL;
        }

        *pPrivate = (void *)(NvUPtr)page_count;
        return (void *)(virt_addr + pageOffset);
    }

    return NULL;
}

NV_STATUS NV_API_CALL nv_free_kernel_mapping(
    nv_state_t *nv,
    void       *pAllocPrivate,
    void       *address,
    void       *pPrivate
)
{
    nv_alloc_t *at = pAllocPrivate;
    NvUPtr virt_addr;
    NvU32 page_count;

    virt_addr = ((NvUPtr)address & NV_PAGE_MASK);
    page_count = (NvUPtr)pPrivate;

    if (at->flags.guest)
    {
        nv_iounmap((void *)virt_addr, (page_count * PAGE_SIZE));
    }
    else if (pPrivate != NULL)
    {
        nv_vm_unmap_pages(virt_addr, page_count);
    }

    return NV_OK;
}

NV_STATUS NV_API_CALL nv_alloc_pages(
    nv_state_t *nv,
    NvU32       page_count,
    NvBool      contiguous,
    NvU32       cache_type,
    NvBool      zeroed,
    NvBool      unencrypted,
    NvU64      *pte_array,
    void      **priv_data
)
{
    nv_alloc_t *at;
    NV_STATUS status = NV_ERR_NO_MEMORY;
    nv_linux_state_t *nvl = NULL;
    NvBool will_remap = NV_FALSE;
    NvU32 i;
    struct device *dev = NULL;

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM: nv_alloc_pages: %d pages\n", page_count);
    nv_printf(NV_DBG_MEMINFO, "NVRM: VM:    contig %d  cache_type %d\n",
        contiguous, cache_type);

    //
    // system memory allocation can be associated with a client instead of a gpu
    // handle the case where per device state is NULL
    //
    if(nv)
    {
       nvl = NV_GET_NVL_FROM_NV_STATE(nv);
       will_remap = nv_requires_dma_remap(nv);
       dev = nvl->dev;
    }

    if (nv_encode_caching(NULL, cache_type, NV_MEMORY_TYPE_SYSTEM))
        return NV_ERR_NOT_SUPPORTED;

    at = nvos_create_alloc(dev, page_count);
    if (at == NULL)
        return NV_ERR_NO_MEMORY;

    at->cache_type = cache_type;

    if (contiguous)
        at->flags.contig = NV_TRUE;
    if (zeroed)
        at->flags.zeroed = NV_TRUE;
#if defined(NVCPU_AARCH64)
    if (at->cache_type != NV_MEMORY_CACHED)
        at->flags.aliased = NV_TRUE;
#endif
    if (unencrypted)
        at->flags.unencrypted = NV_TRUE;

#if defined(NVCPU_PPC64LE)
    /*
     * Starting on Power9 systems, DMA addresses for NVLink are no longer the
     * same as used over PCIe. There is an address compression scheme required
     * for NVLink ONLY which impacts the upper address bits of the DMA address.
     *
     * This divergence between PCIe and NVLink DMA mappings breaks assumptions
     * in the driver where during initialization we allocate system memory
     * for the GPU to access over PCIe before NVLink is trained -- and some of
     * these mappings persist on the GPU. If these persistent mappings are not
     * equivalent they will cause invalid DMA accesses from the GPU once we
     * switch to NVLink.
     *
     * To work around this we limit all system memory allocations from the driver
     * during the period before NVLink is enabled to be from NUMA node 0 (CPU 0)
     * which has a CPU real address with the upper address bits (above bit 42)
     * set to 0. Effectively making the PCIe and NVLink DMA mappings equivalent
     * allowing persistent system memory mappings already programmed on the GPU
     * to remain valid after NVLink is enabled.
     *
     * See Bug 1920398 for more details.
     */
    if (nv && nvl->npu && !nvl->dma_dev.nvlink)
        at->flags.node0 = NV_TRUE;
#endif

    if (at->flags.contig)
        status = nv_alloc_contig_pages(nv, at);
    else
        status = nv_alloc_system_pages(nv, at);

    if (status != NV_OK)
        goto failed;

    for (i = 0; i < ((contiguous) ? 1 : page_count); i++)
    {
        /*
         * The contents of the pte_array[] depend on whether or not this device
         * requires DMA-remapping. If it does, it should be the phys addresses
         * used by the DMA-remapping paths, otherwise it should be the actual
         * address that the device should use for DMA (which, confusingly, may
         * be different than the CPU physical address, due to a static DMA
         * offset).
         */
        if ((nv == NULL) || will_remap)
        {
            pte_array[i] = at->page_table[i]->phys_addr;
        }
        else
        {
            pte_array[i] = nv_phys_to_dma(dev,
                at->page_table[i]->phys_addr);
        }
    }

    *priv_data = at;
    NV_ATOMIC_INC(at->usage_count);

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    return NV_OK;

failed:
    nvos_free_alloc(at);

    return status;
}

NV_STATUS NV_API_CALL nv_free_pages(
    nv_state_t *nv,
    NvU32 page_count,
    NvBool contiguous,
    NvU32 cache_type,
    void *priv_data
)
{
    NV_STATUS rmStatus = NV_OK;
    nv_alloc_t *at = priv_data;

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM: nv_free_pages: 0x%x\n", page_count);

    NV_PRINT_AT(NV_DBG_MEMINFO, at);

    /*
     * If the 'at' usage count doesn't drop to zero here, not all of
     * the user mappings have been torn down in time - we can't
     * safely free the memory. We report success back to the RM, but
     * defer the actual free operation until later.
     *
     * This is described in greater detail in the comments above the
     * nvidia_vma_(open|release)() callbacks in nv-mmap.c.
     */
    if (!NV_ATOMIC_DEC_AND_TEST(at->usage_count))
        return NV_OK;

    if (!at->flags.guest)
    {
        if (at->flags.contig)
            nv_free_contig_pages(at);
        else
            nv_free_system_pages(at);
    }

    nvos_free_alloc(at);

    return rmStatus;
}

NvBool nv_lock_init_locks
(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    nv_linux_state_t *nvl;
    nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    NV_INIT_MUTEX(&nvl->ldata_lock);
    NV_INIT_MUTEX(&nvl->mmap_lock);

    NV_ATOMIC_SET(nvl->usage_count, 0);

    if (!rm_init_event_locks(sp, nv))
        return NV_FALSE;

    return NV_TRUE;
}

void nv_lock_destroy_locks
(
    nvidia_stack_t *sp,
    nv_state_t *nv
)
{
    rm_destroy_event_locks(sp, nv);
}

void NV_API_CALL nv_post_event(
    nv_event_t *event,
    NvHandle    handle,
    NvU32       index,
    NvU32       info32,
    NvU16       info16,
    NvBool      data_valid
)
{
    nv_linux_file_private_t *nvlfp = nv_get_nvlfp_from_nvfp(event->nvfp);
    unsigned long eflags;
    nvidia_event_t *nvet;

    NV_SPIN_LOCK_IRQSAVE(&nvlfp->fp_lock, eflags);

    if (data_valid)
    {
        NV_KMALLOC_ATOMIC(nvet, sizeof(nvidia_event_t));
        if (nvet == NULL)
        {
            NV_SPIN_UNLOCK_IRQRESTORE(&nvlfp->fp_lock, eflags);
            return;
        }

        if (nvlfp->event_data_tail != NULL)
            nvlfp->event_data_tail->next = nvet;
        if (nvlfp->event_data_head == NULL)
            nvlfp->event_data_head = nvet;
        nvlfp->event_data_tail = nvet;
        nvet->next = NULL;

        nvet->event = *event;
        nvet->event.hObject = handle;
        nvet->event.index = index;
        nvet->event.info32 = info32;
        nvet->event.info16 = info16;
    }
    //
    // 'event_pending' is interpreted by nvidia_poll() and nv_get_event() to
    // mean that an event without data is pending. Therefore, only set it to
    // true here if newly posted event is dataless.
    //
    else
    {
        nvlfp->dataless_event_pending = NV_TRUE;
    }

    NV_SPIN_UNLOCK_IRQRESTORE(&nvlfp->fp_lock, eflags);

    wake_up_interruptible(&nvlfp->waitqueue);
}

NvBool NV_API_CALL nv_is_rm_firmware_active(
    nv_state_t *nv
)
{
    if (rm_firmware_active)
    {
        // "all" here means all GPUs
        if (strcmp(rm_firmware_active, "all") == 0)
            return NV_TRUE;
    }
    return NV_FALSE;
}

const char *nv_firmware_path(
    nv_firmware_t fw_type
)
{
    switch (fw_type)
    {
        case NV_FIRMWARE_GSP:
            return NV_FIRMWARE_GSP_FILENAME;
        case NV_FIRMWARE_GSP_LOG:
            return NV_FIRMWARE_GSP_LOG_FILENAME;
    }
    return "";
}

const void* NV_API_CALL nv_get_firmware(
    nv_state_t *nv,
    nv_firmware_t fw_type,
    const void **fw_buf,
    NvU32 *fw_size
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    const struct firmware *fw;

    // path is relative to /lib/firmware
    // if this fails it will print an error to dmesg
    if (request_firmware(&fw, nv_firmware_path(fw_type), nvl->dev) != 0)
        return NULL;

    *fw_size = fw->size;
    *fw_buf = fw->data;

    return fw;
}

void NV_API_CALL nv_put_firmware(
    const void *fw_handle
)
{
    release_firmware(fw_handle);
}

nv_file_private_t* NV_API_CALL nv_get_file_private(
    NvS32 fd,
    NvBool ctl,
    void **os_private
)
{
    struct file *filp = NULL;
    nv_linux_file_private_t *nvlfp = NULL;
    dev_t rdev = 0;

    filp = fget(fd);

    if (filp == NULL || !NV_FILE_INODE(filp))
    {
        goto fail;
    }

    rdev = (NV_FILE_INODE(filp))->i_rdev;

    if (MAJOR(rdev) != NV_MAJOR_DEVICE_NUMBER)
    {
        goto fail;
    }

    if (ctl)
    {
        if (MINOR(rdev) != NV_CONTROL_DEVICE_MINOR)
            goto fail;
    }
    else
    {
        NvBool found = NV_FALSE;
        int i;

        for (i = 0; i <= NV_FRONTEND_CONTROL_DEVICE_MINOR_MIN; i++)
        {
            if ((nv_minor_num_table[i] != NULL) && (MINOR(rdev) == i))
            {
                found = NV_TRUE;
                break;
            }
        }

        if (!found)
            goto fail;
    }

    nvlfp = NV_GET_LINUX_FILE_PRIVATE(filp);

    *os_private = filp;

    return &nvlfp->nvfp;

fail:

    if (filp != NULL)
    {
        fput(filp);
    }

    return NULL;
}

void NV_API_CALL nv_put_file_private(
    void *os_private
)
{
    struct file *filp = os_private;
    fput(filp);
}

int NV_API_CALL nv_get_event(
    nv_file_private_t  *nvfp,
    nv_event_t         *event,
    NvU32              *pending
)
{
    nv_linux_file_private_t *nvlfp = nv_get_nvlfp_from_nvfp(nvfp);
    nvidia_event_t *nvet;
    unsigned long eflags;

    NV_SPIN_LOCK_IRQSAVE(&nvlfp->fp_lock, eflags);

    nvet = nvlfp->event_data_head;
    if (nvet == NULL)
    {
        NV_SPIN_UNLOCK_IRQRESTORE(&nvlfp->fp_lock, eflags);
        return NV_ERR_GENERIC;
    }

    *event = nvet->event;

    if (nvlfp->event_data_tail == nvet)
        nvlfp->event_data_tail = NULL;
    nvlfp->event_data_head = nvet->next;

    *pending = (nvlfp->event_data_head != NULL);

    NV_SPIN_UNLOCK_IRQRESTORE(&nvlfp->fp_lock, eflags);

    NV_KFREE(nvet, sizeof(nvidia_event_t));

    return NV_OK;
}

int NV_API_CALL nv_start_rc_timer(
    nv_state_t *nv
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (nv->rc_timer_enabled)
        return -1;

    nv_printf(NV_DBG_INFO, "NVRM: initializing rc timer\n");

    nv_timer_setup(&nvl->rc_timer, nvidia_rc_timer_callback);

    nv->rc_timer_enabled = 1;

    // set the timeout for 1 second in the future:
    mod_timer(&nvl->rc_timer.kernel_timer, jiffies + HZ);

    nv_printf(NV_DBG_INFO, "NVRM: rc timer initialized\n");

    return 0;
}

int NV_API_CALL nv_stop_rc_timer(
    nv_state_t *nv
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    if (!nv->rc_timer_enabled)
        return -1;

    nv_printf(NV_DBG_INFO, "NVRM: stopping rc timer\n");
    nv->rc_timer_enabled = 0;
    del_timer_sync(&nvl->rc_timer.kernel_timer);
    nv_printf(NV_DBG_INFO, "NVRM: rc timer stopped\n");

    return 0;
}

#define SNAPSHOT_TIMER_FREQ (jiffies + HZ / NV_SNAPSHOT_TIMER_HZ)

static void snapshot_timer_callback(struct nv_timer *timer)
{
    nv_linux_state_t *nvl = &nv_ctl_device;
    nv_state_t *nv = NV_STATE_PTR(nvl);
    unsigned long flags;

    NV_SPIN_LOCK_IRQSAVE(&nvl->snapshot_timer_lock, flags);
    if (nvl->snapshot_callback != NULL)
    {
        nvl->snapshot_callback(nv->profiler_context);
        mod_timer(&timer->kernel_timer, SNAPSHOT_TIMER_FREQ);
    }
    NV_SPIN_UNLOCK_IRQRESTORE(&nvl->snapshot_timer_lock, flags);
}

void NV_API_CALL nv_start_snapshot_timer(void (*snapshot_callback)(void *context))
{
    nv_linux_state_t *nvl = &nv_ctl_device;

    nvl->snapshot_callback = snapshot_callback;
    nv_timer_setup(&nvl->snapshot_timer, snapshot_timer_callback);
    mod_timer(&nvl->snapshot_timer.kernel_timer, SNAPSHOT_TIMER_FREQ);
}

void NV_API_CALL nv_stop_snapshot_timer(void)
{
    nv_linux_state_t *nvl = &nv_ctl_device;
    NvBool timer_active;
    unsigned long flags;

    NV_SPIN_LOCK_IRQSAVE(&nvl->snapshot_timer_lock, flags);
    timer_active = nvl->snapshot_callback != NULL;
    nvl->snapshot_callback = NULL;
    NV_SPIN_UNLOCK_IRQRESTORE(&nvl->snapshot_timer_lock, flags);

    if (timer_active)
        del_timer_sync(&nvl->snapshot_timer.kernel_timer);
}

void NV_API_CALL nv_flush_snapshot_timer(void)
{
    nv_linux_state_t *nvl = &nv_ctl_device;
    nv_state_t *nv = NV_STATE_PTR(nvl);
    unsigned long flags;

    NV_SPIN_LOCK_IRQSAVE(&nvl->snapshot_timer_lock, flags);
    if (nvl->snapshot_callback != NULL)
        nvl->snapshot_callback(nv->profiler_context);
    NV_SPIN_UNLOCK_IRQRESTORE(&nvl->snapshot_timer_lock, flags);
}

static int __init
nvos_count_devices(void)
{
    int count;

    count = nv_pci_count_devices();




    return count;
}

NvBool nvos_is_chipset_io_coherent(void)
{
    if (nv_chipset_is_io_coherent == NV_TRISTATE_INDETERMINATE)
    {
        nvidia_stack_t *sp = NULL;
        if (nv_kmem_cache_alloc_stack(&sp) != 0)
        {
            nv_printf(NV_DBG_ERRORS,
              "NVRM: cannot allocate stack for platform coherence check callback \n");
            WARN_ON(1);
            return NV_FALSE;
        }

        nv_chipset_is_io_coherent = rm_is_chipset_io_coherent(sp);

        nv_kmem_cache_free_stack(sp);
    }

    return nv_chipset_is_io_coherent;
}

#if defined(CONFIG_PM)
static NV_STATUS
nv_power_management(
    nv_state_t *nv,
    nv_pm_action_t pm_action
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    int status = NV_OK;
    nvidia_stack_t *sp = NULL;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = nv_check_gpu_state(nv);
    if (status == NV_ERR_GPU_IS_LOST)
    {
        NV_DEV_PRINTF(NV_DBG_INFO, nv, "GPU is lost, skipping PM event\n");
        goto failure;
    }

    switch (pm_action)
    {
        case NV_PM_ACTION_STANDBY:
            /* fall through */
        case NV_PM_ACTION_HIBERNATE:
        {
            status = rm_power_management(sp, nv, pm_action);

            nv_kthread_q_stop(&nvl->bottom_half_q);

            nv_disable_pat_support();
            break;
        }
        case NV_PM_ACTION_RESUME:
        {
            nv_enable_pat_support();

            nv_kthread_q_item_init(&nvl->bottom_half_q_item,
                                   nvidia_isr_bh_unlocked, (void *)nv);

            status = nv_kthread_q_init(&nvl->bottom_half_q, nv_device_name);
            if (status != NV_OK)
                break;

            status = rm_power_management(sp, nv, pm_action);
            break;
        }
        default:
            status = NV_ERR_INVALID_ARGUMENT;
            break;
    }

failure:
    nv_kmem_cache_free_stack(sp);

    return status;
}

static NV_STATUS
nv_restore_user_channels(
    nv_state_t *nv
)
{
    NV_STATUS status = NV_OK;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    nv_stack_t *sp = NULL;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    down(&nvl->ldata_lock);

    if ((nv->flags & NV_FLAG_OPEN) == 0)
    {
        goto done;
    }

    status = rm_restart_user_channels(sp, nv);
    WARN_ON(status != NV_OK);

    down(&nvl->mmap_lock);

    nv_set_safe_to_mmap_locked(nv, NV_TRUE);

    up(&nvl->mmap_lock);

    rm_unref_dynamic_power(sp, nv, NV_DYNAMIC_PM_FINE);

done:
    up(&nvl->ldata_lock);

    nv_kmem_cache_free_stack(sp);

    return status;
}

static NV_STATUS
nv_preempt_user_channels(
    nv_state_t *nv
)
{
    NV_STATUS status = NV_OK;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    nv_stack_t *sp = NULL;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    down(&nvl->ldata_lock);

    if ((nv->flags & NV_FLAG_OPEN) == 0)
    {
        goto done;
    }

    status = rm_ref_dynamic_power(sp, nv, NV_DYNAMIC_PM_FINE);
    WARN_ON(status != NV_OK);

    down(&nvl->mmap_lock);

    nv_set_safe_to_mmap_locked(nv, NV_FALSE);
    nv_revoke_gpu_mappings_locked(nv);

    up(&nvl->mmap_lock);

    status = rm_stop_user_channels(sp, nv);
    WARN_ON(status != NV_OK);

done:
    up(&nvl->ldata_lock);

    nv_kmem_cache_free_stack(sp);

    return status;
}

static NV_STATUS
nvidia_suspend(
    struct device *dev,
    nv_pm_action_t pm_action,
    NvBool is_procfs_suspend
)
{
    NV_STATUS status = NV_OK;
    struct pci_dev *pci_dev = NULL;
    nv_linux_state_t *nvl;
    nv_state_t *nv;

    if (nv_dev_is_pci(dev))
    {
        pci_dev = to_pci_dev(dev);
        nvl = pci_get_drvdata(pci_dev);
    }
    else
    {
        nvl = dev_get_drvdata(dev);
    }
    nv = NV_STATE_PTR(nvl);

    down(&nvl->ldata_lock);

    if (((nv->flags & NV_FLAG_OPEN) == 0) &&
        ((nv->flags & NV_FLAG_PERSISTENT_SW_STATE) == 0))
    {
        goto done;
    }

    if ((nv->flags & NV_FLAG_SUSPENDED) != 0)
    {
        nvl->suspend_count++;
        goto pci_pm;
    }

    if (nv->preserve_vidmem_allocations && !is_procfs_suspend)
    {
        NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                      "PreserveVideoMemoryAllocations module parameter is set. "
                      "System Power Management attempted without driver procfs suspend interface. "
                      "Please refer to the 'Configuring Power Management Support' section in the driver README.\n");
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    nvidia_modeset_suspend(nv->gpu_id);

    status = nv_power_management(nv, pm_action);

    if (status != NV_OK)
    {
        nvidia_modeset_resume(nv->gpu_id);
        goto done;
    }
    else
    {
        nv->flags |= NV_FLAG_SUSPENDED;
    }

pci_pm:
    /*
     * Check if PCI power state should be D0 during system suspend. The PCI PM
     * core will change the power state only if the driver has not saved the
     * state in it's suspend callback.
     */
    if ((nv->d0_state_in_suspend) && (pci_dev != NULL) &&
        !is_procfs_suspend && (pm_action == NV_PM_ACTION_STANDBY))
    {
        pci_save_state(pci_dev);
    }

done:
    up(&nvl->ldata_lock);

    return status;
}

static NV_STATUS
nvidia_resume(
    struct device *dev,
    nv_pm_action_t pm_action
)
{
    NV_STATUS status = NV_OK;
    struct pci_dev *pci_dev;
    nv_linux_state_t *nvl;
    nv_state_t *nv;

    if (nv_dev_is_pci(dev))
    {
        pci_dev = to_pci_dev(dev);
        nvl = pci_get_drvdata(pci_dev);
    }
    else
    {
        nvl = dev_get_drvdata(dev);
    }
    nv = NV_STATE_PTR(nvl);

    down(&nvl->ldata_lock);

    if ((nv->flags & NV_FLAG_SUSPENDED) == 0)
    {
        goto done;
    }

    if (nvl->suspend_count != 0)
    {
        nvl->suspend_count--;
    }
    else
    {
        status = nv_power_management(nv, pm_action);

        if (status == NV_OK)
        {
            nvidia_modeset_resume(nv->gpu_id);
            nv->flags &= ~NV_FLAG_SUSPENDED;
        }
    }

done:
    up(&nvl->ldata_lock);

    return status;
}

static NV_STATUS
nv_resume_devices(
    nv_pm_action_t pm_action,
    nv_pm_action_depth_t pm_action_depth
)
{
    nv_linux_state_t *nvl;
    NvBool resume_devices = NV_TRUE;
    NV_STATUS status;

    if (pm_action_depth == NV_PM_ACTION_DEPTH_MODESET)
    {
        goto resume_modeset;
    }

    if (pm_action_depth == NV_PM_ACTION_DEPTH_UVM)
    {
        resume_devices = NV_FALSE;
    }

    LOCK_NV_LINUX_DEVICES();

    for (nvl = nv_linux_devices; nvl != NULL; nvl = nvl->next)
    {
        if (resume_devices)
        {
            status = nvidia_resume(nvl->dev, pm_action);
            WARN_ON(status != NV_OK);
        }
    }

    UNLOCK_NV_LINUX_DEVICES();

    status = nv_uvm_resume();
    WARN_ON(status != NV_OK);

    LOCK_NV_LINUX_DEVICES();

    for (nvl = nv_linux_devices; nvl != NULL; nvl = nvl->next)
    {
        status = nv_restore_user_channels(NV_STATE_PTR(nvl));
        WARN_ON(status != NV_OK);
    }

    UNLOCK_NV_LINUX_DEVICES();

resume_modeset:
    nvidia_modeset_resume(0);

    return NV_OK;
}

static NV_STATUS
nv_suspend_devices(
    nv_pm_action_t pm_action,
    nv_pm_action_depth_t pm_action_depth
)
{
    nv_linux_state_t *nvl;
    NvBool resume_devices = NV_FALSE;
    NV_STATUS status = NV_OK;

    nvidia_modeset_suspend(0);

    if (pm_action_depth == NV_PM_ACTION_DEPTH_MODESET)
    {
        return NV_OK;
    }

    LOCK_NV_LINUX_DEVICES();

    for (nvl = nv_linux_devices; nvl != NULL && status == NV_OK; nvl = nvl->next)
    {
        status = nv_preempt_user_channels(NV_STATE_PTR(nvl));
        WARN_ON(status != NV_OK);
    }

    UNLOCK_NV_LINUX_DEVICES();

    if (status == NV_OK)
    {
        status = nv_uvm_suspend();
        WARN_ON(status != NV_OK);
    }
    if (status != NV_OK)
    {
        goto done;
    }

    if (pm_action_depth == NV_PM_ACTION_DEPTH_UVM)
    {
        return NV_OK;
    }

    LOCK_NV_LINUX_DEVICES();

    for (nvl = nv_linux_devices; nvl != NULL && status == NV_OK; nvl = nvl->next)
    {
        status = nvidia_suspend(nvl->dev, pm_action, NV_TRUE);
        WARN_ON(status != NV_OK);
    }
    if (status != NV_OK)
    {
        resume_devices = NV_TRUE;
    }

    UNLOCK_NV_LINUX_DEVICES();

done:
    if (status != NV_OK)
    {
        LOCK_NV_LINUX_DEVICES();

        for (nvl = nv_linux_devices; nvl != NULL; nvl = nvl->next)
        {
            if (resume_devices)
            {
                nvidia_resume(nvl->dev, pm_action);
            }

            nv_restore_user_channels(NV_STATE_PTR(nvl));
        }

        UNLOCK_NV_LINUX_DEVICES();
    }

    return status;
}

NV_STATUS
nv_set_system_power_state(
    nv_power_state_t power_state,
    nv_pm_action_depth_t pm_action_depth
)
{
    NV_STATUS status;
    nv_pm_action_t pm_action;

    switch (power_state)
    {
        case NV_POWER_STATE_IN_HIBERNATE:
            pm_action = NV_PM_ACTION_HIBERNATE;
            break;
        case NV_POWER_STATE_IN_STANDBY:
            pm_action = NV_PM_ACTION_STANDBY;
            break;
        case NV_POWER_STATE_RUNNING:
            pm_action = NV_PM_ACTION_RESUME;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    down(&nv_system_power_state_lock);

    if (nv_system_power_state == power_state)
    {
        status = NV_OK;
        goto done;
    }

    if (power_state == NV_POWER_STATE_RUNNING)
    {
        status = nv_resume_devices(pm_action, nv_system_pm_action_depth);
        up_write(&nv_system_pm_lock);
    }
    else
    {
        if (nv_system_power_state != NV_POWER_STATE_RUNNING)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }

        nv_system_pm_action_depth = pm_action_depth;

        down_write(&nv_system_pm_lock);
        status = nv_suspend_devices(pm_action, nv_system_pm_action_depth);
        if (status != NV_OK)
        {
            up_write(&nv_system_pm_lock);
            goto done;
        }
    }

    nv_system_power_state = power_state;

done:
    up(&nv_system_power_state_lock);

    return status;
}

int nv_pmops_suspend(
    struct device *dev
)
{
    NV_STATUS status;

    status = nvidia_suspend(dev, NV_PM_ACTION_STANDBY, NV_FALSE);
    return (status == NV_OK) ? 0 : -EIO;
}

int nv_pmops_resume(
    struct device *dev
)
{
    NV_STATUS status;

    status = nvidia_resume(dev, NV_PM_ACTION_RESUME);
    return (status == NV_OK) ? 0 : -EIO;
}

int nv_pmops_freeze(
    struct device *dev
)
{
    NV_STATUS status;

    status = nvidia_suspend(dev, NV_PM_ACTION_HIBERNATE, NV_FALSE);
    return (status == NV_OK) ? 0 : -EIO;
}

int nv_pmops_thaw(
    struct device *dev
)
{
    return 0;
}

int nv_pmops_restore(
    struct device *dev
)
{
    NV_STATUS status;

    status = nvidia_resume(dev, NV_PM_ACTION_RESUME);
    return (status == NV_OK) ? 0 : -EIO;
}

int nv_pmops_poweroff(
    struct device *dev
)
{
    return 0;
}

static int
nvidia_transition_dynamic_power(
    struct device *dev,
    NvBool enter
)
{
    struct pci_dev *pci_dev = to_pci_dev(dev);
    nv_linux_state_t *nvl = pci_get_drvdata(pci_dev);
    nv_state_t *nv = NV_STATE_PTR(nvl);
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if ((nv->flags & (NV_FLAG_OPEN | NV_FLAG_PERSISTENT_SW_STATE)) == 0)
    {
        return 0;
    }

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return -ENOMEM;
    }

    status = rm_transition_dynamic_power(sp, nv, enter);

    nv_kmem_cache_free_stack(sp);

    return (status == NV_OK) ? 0 : -EIO;
}

int nv_pmops_runtime_suspend(
    struct device *dev
)
{
    return nvidia_transition_dynamic_power(dev, NV_TRUE);
}

int nv_pmops_runtime_resume(
    struct device *dev
)
{
    return nvidia_transition_dynamic_power(dev, NV_FALSE);
}
#endif /* defined(CONFIG_PM) */

nv_state_t* NV_API_CALL nv_get_adapter_state(
    NvU32 domain,
    NvU8  bus,
    NvU8  slot
)
{
    nv_linux_state_t *nvl;

    LOCK_NV_LINUX_DEVICES();
    for (nvl = nv_linux_devices; nvl != NULL;  nvl = nvl->next)
    {
        nv_state_t *nv = NV_STATE_PTR(nvl);
        if (nv->pci_info.domain == domain && nv->pci_info.bus == bus
            && nv->pci_info.slot == slot)
        {
            UNLOCK_NV_LINUX_DEVICES();
            return nv;
        }
    }
    UNLOCK_NV_LINUX_DEVICES();

    return NULL;
}

nv_state_t* NV_API_CALL nv_get_ctl_state(void)
{
    return NV_STATE_PTR(&nv_ctl_device);
}

NV_STATUS NV_API_CALL nv_log_error(
    nv_state_t *nv,
    NvU32       error_number,
    const char *format,
    va_list    ap
)
{
    NV_STATUS status = NV_OK;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    nv_report_error(nvl->pci_dev, error_number, format, ap);
#if defined(CONFIG_CRAY_XT)
    status = nvos_forward_error_to_cray(nvl->pci_dev, error_number,
                format, ap);
#endif

    return status;
}

NvU64 NV_API_CALL nv_get_dma_start_address(
    nv_state_t *nv
)
{
#if defined(NVCPU_PPC64LE)
    struct pci_dev *pci_dev;
    dma_addr_t dma_addr;
    NvU64 saved_dma_mask;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    /*
     * If TCE bypass is disabled via a module parameter, then just return
     * the default (which is 0).
     *
     * Otherwise, the DMA start address only needs to be set once, and it
     * won't change afterward. Just return the cached value if asked again,
     * to avoid the kernel printing redundant messages to the kernel
     * log when we call pci_set_dma_mask().
     */
    if ((nv_tce_bypass_mode == NV_TCE_BYPASS_MODE_DISABLE) ||
        (nvl->tce_bypass_enabled))
    {
        return nvl->dma_dev.addressable_range.start;
    }

    pci_dev = nvl->pci_dev;

    /*
     * Linux on IBM POWER8 offers 2 different DMA set-ups, sometimes
     * referred to as "windows".
     *
     * The "default window" provides a 2GB region of PCI address space
     * located below the 32-bit line. The IOMMU is used to provide a
     * "rich" mapping--any page in system memory can be mapped at an
     * arbitrary address within this window. The mappings are dynamic
     * and pass in and out of being as pci_map*()/pci_unmap*() calls
     * are made.
     *
     * Dynamic DMA Windows (sometimes "Huge DDW") provides a linear
     * mapping of the system's entire physical address space at some
     * fixed offset above the 59-bit line. IOMMU is still used, and
     * pci_map*()/pci_unmap*() are still required, but mappings are
     * static. They're effectively set up in advance, and any given
     * system page will always map to the same PCI bus address. I.e.
     *   physical 0x00000000xxxxxxxx => PCI 0x08000000xxxxxxxx
     *
     * This driver does not support the 2G default window because
     * of its limited size, and for reasons having to do with UVM.
     *
     * Linux on POWER8 will only provide the DDW-style full linear
     * mapping when the driver claims support for 64-bit DMA addressing
     * (a pre-requisite because the PCI addresses used in this case will
     * be near the top of the 64-bit range). The linear mapping
     * is not available in all system configurations.
     *
     * Detect whether the linear mapping is present by claiming
     * 64-bit support and then mapping physical page 0. For historical
     * reasons, Linux on POWER8 will never map a page to PCI address 0x0.
     * In the "default window" case page 0 will be mapped to some
     * non-zero address below the 32-bit line.  In the
     * DDW/linear-mapping case, it will be mapped to address 0 plus
     * some high-order offset.
     *
     * If the linear mapping is present and sane then return the offset
     * as the starting address for all DMA mappings.
     */
    saved_dma_mask = pci_dev->dma_mask;
    if (pci_set_dma_mask(pci_dev, DMA_BIT_MASK(64)) != 0)
    {
        goto done;
    }

    dma_addr = pci_map_single(pci_dev, NULL, 1, DMA_BIDIRECTIONAL);
    if (pci_dma_mapping_error(pci_dev, dma_addr))
    {
        pci_set_dma_mask(pci_dev, saved_dma_mask);
        goto done;
    }

    pci_unmap_single(pci_dev, dma_addr, 1, DMA_BIDIRECTIONAL);

    /*
     * From IBM: "For IODA2, native DMA bypass or KVM TCE-based implementation
     * of full 64-bit DMA support will establish a window in address-space
     * with the high 14 bits being constant and the bottom up-to-50 bits
     * varying with the mapping."
     *
     * Unfortunately, we don't have any good interfaces or definitions from
     * the kernel to get information about the DMA offset assigned by OS.
     * However, we have been told that the offset will be defined by the top
     * 14 bits of the address, and bits 40-49 will not vary for any DMA
     * mappings until 1TB of system memory is surpassed; this limitation is
     * essential for us to function properly since our current GPUs only
     * support 40 physical address bits. We are in a fragile place where we
     * need to tell the OS that we're capable of 64-bit addressing, while
     * relying on the assumption that the top 24 bits will not vary in this
     * case.
     *
     * The way we try to compute the window, then, is mask the trial mapping
     * against the DMA capabilities of the device. That way, devices with
     * greater addressing capabilities will only take the bits it needs to
     * define the window.
     */
    if ((dma_addr & DMA_BIT_MASK(32)) != 0)
    {
        /*
         * Huge DDW not available - page 0 mapped to non-zero address below
         * the 32-bit line.
         */
        nv_printf(NV_DBG_WARNINGS,
            "NVRM: DMA window limited by platform\n");
        pci_set_dma_mask(pci_dev, saved_dma_mask);
        goto done;
    }
    else if ((dma_addr & saved_dma_mask) != 0)
    {
        NvU64 memory_size = os_get_num_phys_pages() * PAGE_SIZE;
        if ((dma_addr & ~saved_dma_mask) !=
            ((dma_addr + memory_size) & ~saved_dma_mask))
        {
            /*
             * The physical window straddles our addressing limit boundary,
             * e.g., for an adapter that can address up to 1TB, the window
             * crosses the 40-bit limit so that the lower end of the range
             * has different bits 63:40 than the higher end of the range.
             * We can only handle a single, static value for bits 63:40, so
             * we must fall back here.
             */
            nv_printf(NV_DBG_WARNINGS,
                "NVRM: DMA window limited by memory size\n");
            pci_set_dma_mask(pci_dev, saved_dma_mask);
            goto done;
        }
    }

    nvl->tce_bypass_enabled = NV_TRUE;
    nvl->dma_dev.addressable_range.start = dma_addr & ~(saved_dma_mask);

    /* Update the coherent mask to match */
    dma_set_coherent_mask(&pci_dev->dev, pci_dev->dma_mask);

done:
    return nvl->dma_dev.addressable_range.start;
#else
    return 0;
#endif
}

NV_STATUS NV_API_CALL nv_set_primary_vga_status(
    nv_state_t *nv
)
{
    /* IORESOURCE_ROM_SHADOW wasn't added until 2.6.10 */
#if defined(IORESOURCE_ROM_SHADOW)
    nv_linux_state_t *nvl;
    struct pci_dev *pci_dev;

    nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    pci_dev = nvl->pci_dev;

    nv->primary_vga = ((NV_PCI_RESOURCE_FLAGS(pci_dev, PCI_ROM_RESOURCE) &
        IORESOURCE_ROM_SHADOW) == IORESOURCE_ROM_SHADOW);
    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

NV_STATUS NV_API_CALL nv_pci_trigger_recovery(
     nv_state_t *nv
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
#if defined(NV_PCI_ERROR_RECOVERY)
    nv_linux_state_t *nvl       = NV_GET_NVL_FROM_NV_STATE(nv);

    /*
     * Calling readl() on PPC64LE will allow the kernel to check its state for
     * the device and update it accordingly. This needs to be done before
     * checking if the PCI channel is offline, so that we don't check stale
     * state.
     *
     * This will also kick off the recovery process for the device.
     */
    if (NV_PCI_ERROR_RECOVERY_ENABLED())
    {
        if (readl(nv->regs->map) == 0xFFFFFFFF)
        {
            if (pci_channel_offline(nvl->pci_dev))
            {
                NV_DEV_PRINTF(NV_DBG_ERRORS, nv,
                              "PCI channel for the device is offline\n");
                status = NV_OK;
            }
        }
    }
#endif
    return status;
}

NvBool NV_API_CALL nv_requires_dma_remap(
    nv_state_t *nv
)
{
    NvBool dma_remap = NV_FALSE;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    dma_remap = !nv_dma_maps_swiotlb(nvl->dev);
    return dma_remap;
}

/*
 * Intended for use by external kernel modules to list nvidia gpu ids.
 */
NvBool nvidia_get_gpuid_list(NvU32 *gpu_ids, NvU32 *gpu_count)
{
    nv_linux_state_t *nvl;
    unsigned int count;
    NvBool ret = NV_TRUE;

    LOCK_NV_LINUX_DEVICES();

    count = 0;
    for (nvl = nv_linux_devices; nvl != NULL; nvl = nvl->next)
        count++;

    if (*gpu_count == 0)
    {
        goto done;
    }
    else if ((*gpu_count) < count)
    {
        ret = NV_FALSE;
        goto done;
    }

    count = 0;
    for (nvl = nv_linux_devices; nvl != NULL; nvl = nvl->next)
    {
        nv_state_t *nv = NV_STATE_PTR(nvl);
        gpu_ids[count++] = nv->gpu_id;
    }


done:

    *gpu_count = count;

    UNLOCK_NV_LINUX_DEVICES();

    return ret;
}

/*
 * Kernel-level analog to nvidia_open, intended for use by external
 * kernel modules. This increments the ref count of the device with
 * the given gpu_id and makes sure the device has been initialized.
 *
 * Clients of this interface are counted by the RM reset path, to ensure a
 * GPU is not reset while the GPU is active.
 *
 * Returns -ENODEV if the given gpu_id does not exist.
 */
int nvidia_dev_get(NvU32 gpu_id, nvidia_stack_t *sp)
{
    nv_linux_state_t *nvl;
    int rc;

    /* Takes nvl->ldata_lock */
    nvl = find_gpu_id(gpu_id);
    if (!nvl)
        return -ENODEV;

    rc = nv_open_device(NV_STATE_PTR(nvl), sp);

    if (rc == 0)
        WARN_ON(rm_set_external_kernel_client_count(sp, NV_STATE_PTR(nvl), NV_TRUE) != NV_OK);

    up(&nvl->ldata_lock);
    return rc;
}

/*
 * Kernel-level analog to nvidia_close, intended for use by external
 * kernel modules. This decrements the ref count of the device with
 * the given gpu_id, potentially tearing it down.
 */
void nvidia_dev_put(NvU32 gpu_id, nvidia_stack_t *sp)
{
    nv_linux_state_t *nvl;

    /* Takes nvl->ldata_lock */
    nvl = find_gpu_id(gpu_id);
    if (!nvl)
        return;

    nv_close_device(NV_STATE_PTR(nvl), sp);

    WARN_ON(rm_set_external_kernel_client_count(sp, NV_STATE_PTR(nvl), NV_FALSE) != NV_OK);

    up(&nvl->ldata_lock);
}

/*
 * Like nvidia_dev_get but uses UUID instead of gpu_id. Note that this may
 * trigger initialization and teardown of unrelated devices to look up their
 * UUIDs.
 *
 * Clients of this interface are counted by the RM reset path, to ensure a
 * GPU is not reset while the GPU is active.
 */
int nvidia_dev_get_uuid(const NvU8 *uuid, nvidia_stack_t *sp)
{
    nv_state_t *nv = NULL;
    nv_linux_state_t *nvl = NULL;
    const NvU8 *dev_uuid;
    int rc = 0;

    /* Takes nvl->ldata_lock */
    nvl = find_uuid_candidate(uuid);
    while (nvl)
    {
        nv = NV_STATE_PTR(nvl);

        /*
         * If the device is missing its UUID, this call exists solely so
         * rm_get_gpu_uuid_raw will be called and we can inspect the UUID.
         */
        rc = nv_open_device(nv, sp);
        if (rc != 0)
            goto out;

        /* The UUID should always be present following nv_open_device */
        dev_uuid = nv_get_cached_uuid(nv);
        WARN_ON(!dev_uuid);
        if (dev_uuid && memcmp(dev_uuid, uuid, GPU_UUID_LEN) == 0)
            break;

        /* No match, try again. */
        nv_close_device(nv, sp);
        up(&nvl->ldata_lock);
        nvl = find_uuid_candidate(uuid);
    }

    if (nvl)
    {
        rc = 0;
        WARN_ON(rm_set_external_kernel_client_count(sp, NV_STATE_PTR(nvl), NV_TRUE) != NV_OK);
    }
    else
        rc = -ENODEV;

out:
    if (nvl)
        up(&nvl->ldata_lock);
    return rc;
}

/*
 * Like nvidia_dev_put but uses UUID instead of gpu_id.
 */
void nvidia_dev_put_uuid(const NvU8 *uuid, nvidia_stack_t *sp)
{
    nv_linux_state_t *nvl;

    /* Callers must already have called nvidia_dev_get_uuid() */

    /* Takes nvl->ldata_lock */
    nvl = find_uuid(uuid);
    if (!nvl)
        return;

    nv_close_device(NV_STATE_PTR(nvl), sp);

    WARN_ON(rm_set_external_kernel_client_count(sp, NV_STATE_PTR(nvl), NV_FALSE) != NV_OK);

    up(&nvl->ldata_lock);
}

int nvidia_dev_block_gc6(const NvU8 *uuid, nvidia_stack_t *sp)

{
    nv_linux_state_t *nvl;

    /* Callers must already have called nvidia_dev_get_uuid() */

    /* Takes nvl->ldata_lock */
    nvl = find_uuid(uuid);
    if (!nvl)
        return -ENODEV;

    if (rm_ref_dynamic_power(sp, NV_STATE_PTR(nvl), NV_DYNAMIC_PM_FINE) != NV_OK)
    {
        up(&nvl->ldata_lock);
        return -EINVAL;
    }

    up(&nvl->ldata_lock);

    return 0;
}

int nvidia_dev_unblock_gc6(const NvU8 *uuid, nvidia_stack_t *sp)

{
    nv_linux_state_t *nvl;

    /* Callers must already have called nvidia_dev_get_uuid() */

    /* Takes nvl->ldata_lock */
    nvl = find_uuid(uuid);
    if (!nvl)
        return -ENODEV;

    rm_unref_dynamic_power(sp, NV_STATE_PTR(nvl), NV_DYNAMIC_PM_FINE);

    up(&nvl->ldata_lock);

    return 0;
}

NV_STATUS NV_API_CALL nv_get_device_memory_config(
    nv_state_t *nv,
    NvU64 *compr_addr_sys_phys,
    NvU64 *addr_guest_phys,
    NvU32 *addr_width,
    NvU32 *granularity,
    NvS32 *node_id
)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;

    if (!nv_platform_supports_numa(nvl))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

#if defined(NVCPU_PPC64LE)
    nv_npu_numa_info_t *numa_info;

    numa_info = &nvl->npu->numa_info;

    if (node_id != NULL)
    {
        *node_id = nvl->numa_info.node_id;
    }

    if (compr_addr_sys_phys != NULL)
    {
        *compr_addr_sys_phys =
            numa_info->compr_sys_phys_addr;
    }

    if (addr_guest_phys != NULL)
    {
        *addr_guest_phys =
            numa_info->guest_phys_addr;
    }

    if (addr_width != NULL)
    {
        *addr_width = nv_volta_dma_addr_size - nv_volta_addr_space_width;
    }

    if (granularity != NULL)
    {
        *granularity = nv_volta_addr_space_width;
    }

    status = NV_OK;
#endif






























    return status;
}

#if defined(NVCPU_PPC64LE)

NV_STATUS NV_API_CALL nv_get_nvlink_line_rate(
    nv_state_t *nvState,
    NvU32      *linerate
)
{
#if defined(NV_PNV_PCI_GET_NPU_DEV_PRESENT) && defined(NV_OF_GET_PROPERTY_PRESENT)

    nv_linux_state_t *nvl;
    struct pci_dev   *npuDev;
    NvU32            *pSpeedPtr = NULL;
    NvU32            speed;
    int              len;

    if (nvState != NULL)
        nvl = NV_GET_NVL_FROM_NV_STATE(nvState);
    else
        return NV_ERR_INVALID_ARGUMENT;

    if (!nvl->npu)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    npuDev = nvl->npu->devs[0];
    if (!npuDev->dev.of_node)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: %s: OF Node not found in IBM-NPU device node\n",
                  __FUNCTION__);
        return NV_ERR_NOT_SUPPORTED;
    }

    pSpeedPtr = (NvU32 *) of_get_property(npuDev->dev.of_node, "ibm,nvlink-speed", &len);

    if (pSpeedPtr)
    {
        speed = (NvU32) be32_to_cpup(pSpeedPtr);
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!speed)
    {
        return NV_ERR_NOT_SUPPORTED;
    }
    else
    {
        *linerate = speed;
    }

    return NV_OK;

#endif

    return NV_ERR_NOT_SUPPORTED;
}

#endif

NV_STATUS NV_API_CALL nv_indicate_idle(
    nv_state_t *nv
)
{
#if defined(NV_PM_RUNTIME_AVAILABLE)
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct device *dev = nvl->dev;
    struct file *file = nvl->sysfs_config_file;
    loff_t f_pos = 0;
    char buf;

    pm_runtime_put_noidle(dev);

#if defined(NV_SEQ_READ_ITER_PRESENT)
    {
        struct kernfs_open_file *of = ((struct seq_file *)file->private_data)->private;
        struct kernfs_node *kn;

        mutex_lock(&of->mutex);
        kn = of->kn;
        if (kn != NULL && atomic_inc_unless_negative(&kn->active))
        {
            if ((kn->attr.ops != NULL) && (kn->attr.ops->read != NULL))
            {
                kn->attr.ops->read(of, &buf, 1, f_pos);
            }
            atomic_dec(&kn->active);
        }
        mutex_unlock(&of->mutex);
    }
#else
#if defined(NV_KERNEL_READ_HAS_POINTER_POS_ARG)
    kernel_read(file, &buf, 1, &f_pos);
#else
    kernel_read(file, f_pos, &buf, 1);
#endif
#endif

    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

NV_STATUS NV_API_CALL nv_indicate_not_idle(
    nv_state_t *nv
)
{
#if defined(NV_PM_RUNTIME_AVAILABLE)
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct device *dev = nvl->dev;

    pm_runtime_get_noresume(dev);

    nvl->is_forced_shutdown = NV_TRUE;
    pci_bus_type.shutdown(dev);

    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

void NV_API_CALL nv_idle_holdoff(
    nv_state_t *nv
)
{
#if defined(NV_PM_RUNTIME_AVAILABLE)
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct device *dev = nvl->dev;

    pm_runtime_get_noresume(dev);
#endif
}

NvBool NV_API_CALL nv_dynamic_power_available(
    nv_state_t *nv
)
{
#if defined(NV_PM_RUNTIME_AVAILABLE)
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    return nvl->sysfs_config_file != NULL;
#else
    return NV_FALSE;
#endif
}

/* caller should hold nv_linux_devices_lock using LOCK_NV_LINUX_DEVICES */
void nv_linux_add_device_locked(nv_linux_state_t *nvl)
{
    if (nv_linux_devices == NULL) {
        nv_linux_devices = nvl;
    }
    else
    {
        nv_linux_state_t *tnvl;
        for (tnvl = nv_linux_devices; tnvl->next != NULL;  tnvl = tnvl->next);
        tnvl->next = nvl;
    }
}

/* caller should hold nv_linux_devices_lock using LOCK_NV_LINUX_DEVICES */
void nv_linux_remove_device_locked(nv_linux_state_t *nvl)
{
    if (nvl == nv_linux_devices) {
        nv_linux_devices = nvl->next;
    }
    else
    {
        nv_linux_state_t *tnvl;
        for (tnvl = nv_linux_devices; tnvl->next != nvl;  tnvl = tnvl->next);
        tnvl->next = nvl->next;
    }
}

void NV_API_CALL nv_control_soc_irqs(nv_state_t *nv, NvBool bEnable)
{
    int count;

    if (bEnable)
    {
        for (count = 0; count < nv->num_soc_irqs; count++)
        {
            nv->soc_irq_info[count].bh_pending = NV_FALSE;
            nv->current_soc_irq = -1;
            enable_irq(nv->soc_irq_info[count].irq_num);
        }
    }
    else
    {
        for (count = 0; count < nv->num_soc_irqs; count++)
        {
            disable_irq_nosync(nv->soc_irq_info[count].irq_num);
        }
    }
}

NvU32 NV_API_CALL nv_get_dev_minor(nv_state_t *nv)
{
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

    return nvl->minor_num;
}

NV_STATUS NV_API_CALL nv_acquire_fabric_mgmt_cap(int fd, int *duped_fd)
{

    *duped_fd = nvlink_cap_acquire(fd, NVLINK_CAP_FABRIC_MANAGEMENT);
    if (*duped_fd < 0)
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    return NV_OK;



}

/*
 * Wakes up the NVIDIA GPU HDA codec and contoller by reading
 * codec proc file.
 */
void NV_API_CALL nv_audio_dynamic_power(
    nv_state_t *nv
)
{
/*
 * The runtime power management for nvidia HDA controller can be possible
 * after commit 07f4f97d7b4b ("vga_switcheroo: Use device link for HDA
 * controller"). This commit has also moved 'PCI_CLASS_MULTIMEDIA_HD_AUDIO'
 * macro from <sound/hdaudio.h> to <linux/pci_ids.h>.
 * If 'NV_PCI_CLASS_MULTIMEDIA_HD_AUDIO_PRESENT' is not defined, then
 * this function will be stub function.
 *
 * Also, check if runtime PM is enabled in the kernel (with
 * 'NV_PM_RUNTIME_AVAILABLE') and stub this function if it is disabled. This
 * function uses kernel fields only present when the kconfig has runtime PM
 * enabled.
 */
#if defined(NV_PCI_CLASS_MULTIMEDIA_HD_AUDIO_PRESENT) && defined(NV_PM_RUNTIME_AVAILABLE)
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct device *dev = nvl->dev;
    struct pci_dev *audio_pci_dev, *pci_dev;
    struct snd_card *card;

    if (!nv_dev_is_pci(dev))
        return;

    pci_dev = to_pci_dev(dev);

    audio_pci_dev = os_pci_init_handle(NV_PCI_DOMAIN_NUMBER(pci_dev),
                                       NV_PCI_BUS_NUMBER(pci_dev),
                                       NV_PCI_SLOT_NUMBER(pci_dev),
                                       1, NULL, NULL);

    if (audio_pci_dev == NULL)
        return;

    /*
     * Check if HDA controller is in pm suspended state. The HDA contoller
     * can not be runtime resumed if this API is called during system
     * suspend/resume time and HDA controller is in pm suspended state.
     */
    if (audio_pci_dev->dev.power.is_suspended)
        return;

    card = pci_get_drvdata(audio_pci_dev);
    if (card == NULL)
        return;

    /*
     * Commit be57bfffb7b5 ("ALSA: hda: move hda_codec.h to include/sound")
     * in v4.20-rc1 moved "hda_codec.h" header file from the private sound
     * folder to include/sound.
     */
#if defined(NV_SOUND_HDA_CODEC_H_PRESENT)
    {
        struct list_head *p;
        struct hda_codec *codec = NULL;
        unsigned int cmd, res;

        /*
         * Traverse the list of devices which the sound card maintains and
         * search for HDA codec controller.
         */
        list_for_each_prev(p, &card->devices)
        {
            struct snd_device *pdev = list_entry(p, struct snd_device, list);

            if (pdev->type == SNDRV_DEV_CODEC)
            {
                codec = pdev->device_data;

                /*
                 * NVIDIA HDA codec controller uses linux kernel HDA codec
                 * driver. Commit 05852448690d ("ALSA: hda - Support indirect
                 * execution of verbs") added support for overriding exec_verb.
                 * This codec->core.exec_verb will be codec_exec_verb() for
                 * NVIDIA HDA codec driver.
                 */
                if (codec->core.exec_verb == NULL)
                {
                    return;
                }

                break;
            }
        }

        if (codec == NULL)
        {
            return;
        }

        /* If HDA codec controller is already runtime active, then return */
        if (snd_hdac_is_power_on(&codec->core))
        {
            return;
        }

        /*
         * Encode codec verb for getting vendor ID from root node.
         * Refer Intel High Definition Audio Specification for more details.
         */
        cmd = (codec->addr << 28) | (AC_NODE_ROOT << 20) |
              (AC_VERB_PARAMETERS << 8) | AC_PAR_VENDOR_ID;

        /*
         * It will internally increment the runtime PM refcount,
         * wake-up the audio codec controller and send the HW
         * command for getting vendor ID. Once the vendor ID will be
         * returned back, then it will decrement the runtime PM refcount
         * and runtime suspend audio codec controller again (If refcount is
         * zero) once auto suspend counter expires.
         */
        codec->core.exec_verb(&codec->core, cmd, 0, &res);
    }
#else
    {
        int codec_addr;

        /*
         * The filp_open() call below depends on the current task's fs_struct
         * (current->fs), which may already be NULL if this is called during
         * process teardown.
         */
        if (current->fs == NULL)
            return;

        /* If device is runtime active, then return */
        if (audio_pci_dev->dev.power.runtime_status == RPM_ACTIVE)
            return;

        for (codec_addr = 0; codec_addr < NV_HDA_MAX_CODECS; codec_addr++)
        {
            char filename[48];
            NvU8 buf;
            int ret;

            ret = snprintf(filename, sizeof(filename),
                           "/proc/asound/card%d/codec#%d",
                           card->number, codec_addr);

            if (ret > 0 && ret < sizeof(filename) &&
                (os_open_and_read_file(filename, &buf, 1) == NV_OK))
            {
                    break;
            }
        }
    }
#endif
#endif
}

static int nv_match_dev_state(const void *data, struct file *filp, unsigned fd)
{
    nv_linux_state_t *nvl = NULL;
    dev_t rdev = 0;

    if (filp == NULL ||
        filp->private_data == NULL ||
        NV_FILE_INODE(filp) == NULL)
        return 0;
 
    rdev = (NV_FILE_INODE(filp))->i_rdev;
    if (MAJOR(rdev) != NV_MAJOR_DEVICE_NUMBER)
        return 0;

    nvl = NV_GET_NVL_FROM_FILEP(filp);
    if (nvl == NULL)
        return 0;

    return (data == nvl);
}

NvBool NV_API_CALL nv_is_gpu_accessible(nv_state_t *nv)
{
    struct files_struct *files = current->files;
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

#ifdef NV_ITERATE_FD_PRESENT
    return !!iterate_fd(files, 0, nv_match_dev_state, nvl);
#else
    struct fdtable *fdtable;
    int ret_val = 0;
    int fd = 0;

    if (files == NULL)
        return 0;

    spin_lock(&files->file_lock);

    for (fdtable = files_fdtable(files); fd < fdtable->max_fds; fd++)
    {
        struct file *filp;

#ifdef READ_ONCE
        filp = READ_ONCE(fdtable->fd[fd]);
#else
        filp = ACCESS_ONCE(fdtable->fd[fd]);
        smp_read_barrier_depends();
#endif
        if (filp == NULL)
            continue;

        ret_val = nv_match_dev_state(nvl, filp, fd);
        if (ret_val)
            break;
    }

    spin_unlock(&files->file_lock);

    return !!ret_val;
#endif
}


NvBool NV_API_CALL nv_platform_supports_s0ix(void)
{
#if defined(CONFIG_ACPI)
    return (acpi_gbl_FADT.flags & ACPI_FADT_LOW_POWER_S0) != 0;
#else
    return NV_FALSE;
#endif
}

NvBool NV_API_CALL nv_s2idle_pm_configured(void)
{
    NvU8 buf[8];

#if defined(NV_SEQ_READ_ITER_PRESENT)
    struct file *file;
    ssize_t num_read;
    struct kiocb kiocb;
    struct iov_iter iter;
    struct kvec iov = {
        .iov_base = &buf,
        .iov_len = sizeof(buf),
    };

    if (os_open_readonly_file("/sys/power/mem_sleep", (void **)&file) != NV_OK)
    {
        return NV_FALSE;
    }

    /*
     * init_sync_kiocb() internally uses GPL licensed __get_task_ioprio() from
     * v5.20-rc1.
     */
#if defined(NV_GET_TASK_IOPRIO_PRESENT)
    memset(&kiocb, 0, sizeof(kiocb));
    kiocb.ki_filp = file;
    kiocb.ki_flags = iocb_flags(file);
    kiocb.ki_ioprio = IOPRIO_DEFAULT;
#else
    init_sync_kiocb(&kiocb, file);
#endif

    kiocb.ki_pos = 0;
    iov_iter_kvec(&iter, READ, &iov, 1, sizeof(buf));

    num_read = seq_read_iter(&kiocb, &iter);

    os_close_file((void *)file);

    if (num_read != sizeof(buf))
    {
        return NV_FALSE;
    }
#else
    if (os_open_and_read_file("/sys/power/mem_sleep", buf,
                              sizeof(buf)) != NV_OK)
    {
        return NV_FALSE;
    }
#endif

    return (memcmp(buf, "[s2idle]", 8) == 0);
}


/*
 * Function query system chassis info, to figure out if the platform is
 * Laptop or Notebook.
 * This function should be used when querying GPU form factor information is
 * not possible via core RM or if querying both system and GPU form factor
 * information is necessary.
 */
NvBool NV_API_CALL nv_is_chassis_notebook(void)
{
    const char *chassis_type = dmi_get_system_info(DMI_CHASSIS_TYPE);

    //
    // Return true only for Laptop & Notebook
    // As per SMBIOS spec Laptop = 9 and Notebook = 10
    //
    return (chassis_type && (!strcmp(chassis_type, "9") || !strcmp(chassis_type, "10")));
}

void NV_API_CALL nv_allow_runtime_suspend
(
    nv_state_t *nv
)
{
#if defined(NV_PM_RUNTIME_AVAILABLE)
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct device    *dev = nvl->dev;

    spin_lock_irq(&dev->power.lock);

    if (dev->power.runtime_auto == false)
    {
        dev->power.runtime_auto = true;
        atomic_add_unless(&dev->power.usage_count, -1, 0);
    }

    spin_unlock_irq(&dev->power.lock);
#endif
}

void NV_API_CALL nv_disallow_runtime_suspend
(
    nv_state_t *nv
)
{
#if defined(NV_PM_RUNTIME_AVAILABLE)
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct device    *dev = nvl->dev;

    spin_lock_irq(&dev->power.lock);

    if (dev->power.runtime_auto == true)
    {
        dev->power.runtime_auto = false;
        atomic_inc(&dev->power.usage_count);
    }

    spin_unlock_irq(&dev->power.lock);
#endif
}

NvU32 NV_API_CALL nv_get_os_type(void)
{
    return OS_TYPE_LINUX;
}

void NV_API_CALL nv_flush_coherent_cpu_cache_range(nv_state_t *nv, NvU64 cpu_virtual, NvU64 size)
{
#if NVCPU_IS_PPC64LE
    return nv_ibmnpu_cache_flush_range(nv, cpu_virtual, size);
#elif NVCPU_IS_AARCH64

    NvU64 va, cbsize;
    NvU64 end_cpu_virtual = cpu_virtual + size;
    
    nv_printf(NV_DBG_INFO,
            "Flushing CPU virtual range [0x%llx, 0x%llx)\n",
            cpu_virtual, end_cpu_virtual);

    cbsize = cache_line_size();
    // Align address to line size
    cpu_virtual = NV_ALIGN_UP(cpu_virtual, cbsize);

    // Force eviction of any cache lines from the NUMA-onlined region.
    for (va = cpu_virtual; va < end_cpu_virtual; va += cbsize)
    {
        asm volatile("dc civac, %0" : : "r" (va): "memory");
        // Reschedule if necessary to avoid lockup warnings
        cond_resched();
    }
    asm volatile("dsb sy" : : : "memory");

#endif
}

static struct resource *nv_next_resource(struct resource *p)
{
    if (p->child != NULL)
        return p->child;

    while ((p->sibling == NULL) && (p->parent != NULL))
        p = p->parent;

    return p->sibling;
}

/*
 * Function to get the correct PCI Bus memory window which can be mapped
 * in the real mode emulator (emu).
 * The function gets called during the initialization of the emu before
 * remapping it to OS.
 */
void NV_API_CALL nv_get_updated_emu_seg(
    NvU32 *start,
    NvU32 *end
)
{
    struct resource *p;

    if (*start >= *end)
        return;

    for (p = iomem_resource.child; (p != NULL); p = nv_next_resource(p))
    {
        /* If we passed the resource we are looking for, stop */
        if (p->start > *end)
        {
            p = NULL;
            break;
        }

        /* Skip until we find a range that matches what we look for */
        if (p->end < *start)
            continue;

        if ((p->end > *end) && (p->child))
            continue;

        if ((p->flags & IORESOURCE_MEM) != IORESOURCE_MEM)
            continue;

        /* Found a match, break */
        break;
    }

    if (p != NULL)
    {
        *start = max((resource_size_t)*start, p->start);
        *end = min((resource_size_t)*end, p->end);
    }
}
