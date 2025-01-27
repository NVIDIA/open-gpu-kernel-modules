/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file holds Unix-specific NVIDIA driver options
//

#ifndef _RM_REG_H_
#define _RM_REG_H_

#include "nvtypes.h"
#include "nv-firmware-registry.h"

/*
 * use NV_REG_STRING to stringify a registry key when using that registry key
 */

#define __NV_REG_STRING(regkey)  #regkey
#define NV_REG_STRING(regkey)  __NV_REG_STRING(regkey)

/*
 * use NV_DEFINE_REG_ENTRY and NV_DEFINE_PARAMS_TABLE_ENTRY to simplify definition
 * of registry keys in the kernel module source code.
 */

#define __NV_REG_VAR(regkey)  NVreg_##regkey

#if defined(NV_MODULE_PARAMETER)
#define NV_DEFINE_REG_ENTRY(regkey, default_value)        \
    static NvU32 __NV_REG_VAR(regkey) = (default_value);  \
    NV_MODULE_PARAMETER(__NV_REG_VAR(regkey))
#define NV_DEFINE_REG_ENTRY_GLOBAL(regkey, default_value) \
    NvU32 __NV_REG_VAR(regkey) = (default_value);         \
    NV_MODULE_PARAMETER(__NV_REG_VAR(regkey))
#else
#define NV_DEFINE_REG_ENTRY(regkey, default_value)        \
    static NvU32 __NV_REG_VAR(regkey) = (default_value)
#define NV_DEFINE_REG_ENTRY_GLOBAL(regkey, default_value) \
    NvU32 __NV_REG_VAR(regkey) = (default_value)
#endif

#if defined(NV_MODULE_STRING_PARAMETER)
#define NV_DEFINE_REG_STRING_ENTRY(regkey, default_value) \
    char *__NV_REG_VAR(regkey) = (default_value);         \
    NV_MODULE_STRING_PARAMETER(__NV_REG_VAR(regkey))
#else
#define NV_DEFINE_REG_STRING_ENTRY(regkey, default_value) \
    char *__NV_REG_VAR(regkey) = (default_value)
#endif

#define NV_DEFINE_PARAMS_TABLE_ENTRY(regkey) \
    { NV_REG_STRING(regkey), &__NV_REG_VAR(regkey) }

/*
 * Like NV_DEFINE_PARMS_TABLE_ENTRY, but allows a mismatch between the name of
 * the regkey and the name of the module parameter. When using this macro, the
 * name of the parameter is passed to the extra "parameter" argument, and it is
 * this name that must be used in the NV_DEFINE_REG_ENTRY() macro.
 */

#define NV_DEFINE_PARAMS_TABLE_ENTRY_CUSTOM_NAME(regkey, parameter) \
    { NV_REG_STRING(regkey), &__NV_REG_VAR(parameter)}

/*
 *----------------- registry key definitions--------------------------
 */

/*
 * Option: ModifyDeviceFiles
 *
 * Description:
 *
 * When this option is enabled, the NVIDIA driver will verify the validity
 * of the NVIDIA device files in /dev and attempt to dynamically modify
 * and/or (re-)create them, if necessary. If you don't wish for the NVIDIA
 * driver to touch the device files, you can use this registry key.
 *
 * This module parameter is only honored by the NVIDIA GPU driver and NVIDIA
 * capability driver. Furthermore, the NVIDIA capability driver provides
 * modifiable /proc file entry (DeviceFileModify=0/1) to alter the behavior of
 * this module parameter per device file.
 *
 * Possible Values:
 *  0 = disable dynamic device file management
 *  1 = enable  dynamic device file management (default)
 */

#define __NV_MODIFY_DEVICE_FILES ModifyDeviceFiles
#define NV_REG_MODIFY_DEVICE_FILES NV_REG_STRING(__NV_MODIFY_DEVICE_FILES)

/*
 * Option: DeviceFileUID
 *
 * Description:
 *
 * This registry key specifies the UID assigned to the NVIDIA device files
 * created and/or modified by the NVIDIA driver when dynamic device file
 * management is enabled.
 *
 * This module parameter is only honored by the NVIDIA GPU driver.
 *
 * The default UID is 0 ('root').
 */

#define __NV_DEVICE_FILE_UID DeviceFileUID
#define NV_REG_DEVICE_FILE_UID NV_REG_STRING(__NV_DEVICE_FILE_UID)

/*
 * Option: DeviceFileGID
 *
 * Description:
 *
 * This registry key specifies the GID assigned to the NVIDIA device files
 * created and/or modified by the NVIDIA driver when dynamic device file
 * management is enabled.
 *
 * This module parameter is only honored by the NVIDIA GPU driver.
 *
 * The default GID is 0 ('root').
 */

#define __NV_DEVICE_FILE_GID DeviceFileGID
#define NV_REG_DEVICE_FILE_GID NV_REG_STRING(__NV_DEVICE_FILE_GID)

/*
 * Option: DeviceFileMode
 *
 * Description:
 *
 * This registry key specifies the device file mode assigned to the NVIDIA
 * device files created and/or modified by the NVIDIA driver when dynamic
 * device file management is enabled.
 *
 * This module parameter is only honored by the NVIDIA GPU driver.
 *
 * The default mode is 0666 (octal, rw-rw-rw-).
 */

#define __NV_DEVICE_FILE_MODE DeviceFileMode
#define NV_REG_DEVICE_FILE_MODE NV_REG_STRING(__NV_DEVICE_FILE_MODE)

/*
 * Option: ResmanDebugLevel
 *
 * Default value: ~0
 */

#define __NV_RESMAN_DEBUG_LEVEL ResmanDebugLevel
#define NV_REG_RESMAN_DEBUG_LEVEL NV_REG_STRING(__NV_RESMAN_DEBUG_LEVEL)

/*
 * Option: RmLogonRC
 *
 * Default value: 1
 */

#define __NV_RM_LOGON_RC RmLogonRC
#define NV_REG_RM_LOGON_RC NV_REG_STRING(__NV_RM_LOGON_RC)

/*
 * Option: InitializeSystemMemoryAllocations
 *
 * Description:
 *
 * The NVIDIA Linux driver normally clears system memory it allocates
 * for use with GPUs or within the driver stack. This is to ensure
 * that potentially sensitive data is not rendered accessible by
 * arbitrary user applications.
 *
 * Owners of single-user systems or similar trusted configurations may
 * choose to disable the aforementioned clears using this option and
 * potentially improve performance.
 *
 * Possible values:
 *
 *  1 = zero out system memory allocations (default)
 *  0 = do not perform memory clears
 */

#define __NV_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS \
    InitializeSystemMemoryAllocations
#define NV_REG_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS \
    NV_REG_STRING(__NV_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS)

/*
 * Option: RegistryDwords
 *
 * Description:
 *
 * This option accepts a semicolon-separated list of key=value pairs. Each
 * key name is checked against the table of static options; if a match is
 * found, the static option value is overridden, but invalid options remain
 * invalid. Pairs that do not match an entry in the static option table
 * are passed on to the RM directly.
 *
 * Format:
 *
 *  NVreg_RegistryDwords="<key=value>;<key=value>;..."
 */

#define __NV_REGISTRY_DWORDS RegistryDwords
#define NV_REG_REGISTRY_DWORDS NV_REG_STRING(__NV_REGISTRY_DWORDS)

/*
 * Option: RegistryDwordsPerDevice
 *
 * Description:
 *
 * This option allows to specify registry keys per GPU device. It helps to
 * control registry at GPU level of granularity. It accepts a semicolon
 * separated list of key=value pairs. The first key value pair MUST be
 * "pci=DDDD:BB:DD.F;" where DDDD is Domain, BB is Bus Id, DD is device slot
 * number and F is the Function. This PCI BDF is used to identify which GPU to
 * assign the registry keys that follows next.
 * If a GPU corresponding to the value specified in "pci=DDDD:BB:DD.F;" is NOT
 * found, then all the registry keys that follows are skipped, until we find next
 * valid pci identified "pci=DDDD:BB:DD.F;". Following are the valid formats for
 * the value of the "pci" string:
 * 1)  bus:slot                 : Domain and function defaults to 0.
 * 2)  domain:bus:slot          : Function defaults to 0.
 * 3)  domain:bus:slot.func     : Complete PCI dev id string.
 *
 * For each of the registry keys that follows, key name is checked against the
 * table of static options; if a match is found, the static option value is
 * overridden, but invalid options remain invalid. Pairs that do not match an
 * entry in the static option table are passed on to the RM directly.
 *
 * Format:
 *
 *  NVreg_RegistryDwordsPerDevice="pci=DDDD:BB:DD.F;<key=value>;<key=value>;..; \
 *               pci=DDDD:BB:DD.F;<key=value>;..;"
 */

#define __NV_REGISTRY_DWORDS_PER_DEVICE RegistryDwordsPerDevice
#define NV_REG_REGISTRY_DWORDS_PER_DEVICE NV_REG_STRING(__NV_REGISTRY_DWORDS_PER_DEVICE)

#define __NV_RM_MSG RmMsg
#define NV_RM_MSG NV_REG_STRING(__NV_RM_MSG)

/*
 * Option: UsePageAttributeTable
 *
 * Description:
 *
 * Enable/disable use of the page attribute table (PAT) available in
 * modern x86/x86-64 processors to set the effective memory type of memory
 * mappings to write-combining (WC).
 *
 * If enabled, an x86 processor with PAT support is present and the host
 * system's Linux kernel did not configure one of the PAT entries to
 * indicate the WC memory type, the driver will change the second entry in
 * the PAT from its default (write-through (WT)) to WC at module load
 * time. If the kernel did update one of the PAT entries, the driver will
 * not modify the PAT.
 *
 * In both cases, the driver will honor attempts to map memory with the WC
 * memory type by selecting the appropriate PAT entry using the correct
 * set of PTE flags.
 *
 * Possible values:
 *
 * ~0 = use the NVIDIA driver's default logic (default)
 *  1 = enable use of the PAT for WC mappings.
 *  0 = disable use of the PAT for WC mappings.
 */

#define __NV_USE_PAGE_ATTRIBUTE_TABLE UsePageAttributeTable
#define NV_USE_PAGE_ATTRIBUTE_TABLE NV_REG_STRING(__NV_USE_PAGE_ATTRIBUTE_TABLE)

/*
 * Option: EnableMSI
 *
 * Description:
 *
 * When this option is enabled and the host kernel supports the MSI feature,
 * the NVIDIA driver will enable the PCI-E MSI capability of GPUs with the
 * support for this feature instead of using PCI-E wired interrupt.
 *
 * Possible Values:
 *
 *  0 = disable MSI interrupt
 *  1 = enable MSI interrupt (default)
 *
 */

#define __NV_ENABLE_MSI EnableMSI
#define NV_REG_ENABLE_MSI NV_REG_STRING(__NV_ENABLE_MSI)

/*
 * Option: EnablePCIeGen3
 *
 * Description:
 *
 * Due to interoperability problems seen with Kepler PCIe Gen3 capable GPUs
 * when configured on SandyBridge E desktop platforms, NVIDIA feels that
 * delivering a reliable, high-quality experience is not currently possible in
 * PCIe Gen3 mode on all PCIe Gen3 platforms. Therefore, Quadro, Tesla and
 * NVS Kepler products operate in PCIe Gen2 mode by default. You may use this
 * option to enable PCIe Gen3 support.
 *
 * This is completely unsupported!
 *
 * Possible Values:
 *
 *  0: disable PCIe Gen3 support (default)
 *  1: enable PCIe Gen3 support
 */

#define __NV_ENABLE_PCIE_GEN3 EnablePCIeGen3
#define NV_REG_ENABLE_PCIE_GEN3 NV_REG_STRING(__NV_ENABLE_PCIE_GEN3)

/*
 * Option: MemoryPoolSize
 *
 * Description:
 *
 * When set to a non-zero value, this option specifies the size of the
 * memory pool, given as a multiple of 1 GB, created on VMware ESXi to
 * satisfy any system memory allocations requested by the NVIDIA kernel
 * module.
 */

#define __NV_MEMORY_POOL_SIZE MemoryPoolSize
#define NV_REG_MEMORY_POOL_SIZE NV_REG_STRING(__NV_MEMORY_POOL_SIZE)

/*
 * Option: KMallocHeapMaxSize
 *
 * Description:
 *
 * When set to a non-zero value, this option specifies the maximum size of the
 * heap memory space reserved for kmalloc operations. Given as a
 * multiple of 1 MB created on VMware ESXi to satisfy any system memory
 * allocations requested by the NVIDIA kernel module.
 */

#define __NV_KMALLOC_HEAP_MAX_SIZE KMallocHeapMaxSize
#define NV_KMALLOC_HEAP_MAX_SIZE NV_REG_STRING(__NV_KMALLOC_HEAP_MAX_SIZE)

/*
 * Option: VMallocHeapMaxSize
 *
 * Description:
 *
 * When set to a non-zero value, this option specifies the maximum size of the
 * heap memory space reserved for vmalloc operations. Given as a
 * multiple of 1 MB created on VMware ESXi to satisfy any system memory
 * allocations requested by the NVIDIA kernel module.
 */

#define __NV_VMALLOC_HEAP_MAX_SIZE VMallocHeapMaxSize
#define NV_VMALLOC_HEAP_MAX_SIZE NV_REG_STRING(__NV_VMALLOC_HEAP_MAX_SIZE)

/*
 * Option: IgnoreMMIOCheck
 *
 * Description:
 *
 * When this option is enabled, the NVIDIA kernel module will ignore
 * MMIO limit check during device probe on VMWare ESXi kernel. This is
 * typically necessary when VMware ESXi MMIO limit differs between any
 * base version and its updates. Customer using updates can set regkey
 * to avoid probe failure.
 */

#define __NV_IGNORE_MMIO_CHECK IgnoreMMIOCheck
#define NV_REG_IGNORE_MMIO_CHECK NV_REG_STRING(__NV_IGNORE_MMIO_CHECK)

/*
 * Option: pci
 *
 * Description:
 *
 * On Unix platforms, per GPU based registry key can be specified as:
 * NVreg_RegistryDwordsPerDevice="pci=DDDD:BB:DD.F,<per-gpu registry keys>".
 * where DDDD:BB:DD.F refers to Domain:Bus:Device.Function.
 * We need this key "pci" to identify what follows next is a PCI BDF identifier,
 * for which the registry keys are to be applied.
 *
 * This define is not used on non-UNIX platforms.
 *
 * Possible Formats for value:
 *
 * 1)  bus:slot                 : Domain and function defaults to 0.
 * 2)  domain:bus:slot          : Function defaults to 0.
 * 3)  domain:bus:slot.func     : Complete PCI BDF identifier string.
 */
#define __NV_PCI_DEVICE_BDF pci
#define NV_REG_PCI_DEVICE_BDF NV_REG_STRING(__NV_PCI_DEVICE_BDF)

/*
 * Option: EnableStreamMemOPs
 *
 * Description:
 *
 * When this option is enabled, the CUDA driver will enable support for
 * CUDA Stream Memory Operations in user-mode applications, which are so
 * far required to be disabled by default due to limited support in
 * devtools.
 *
 * Note: this is treated as a hint. MemOPs may still be left disabled by CUDA
 * driver for other reasons.
 *
 * Possible Values:
 *
 *  0 = disable feature (default)
 *  1 = enable feature
 */
#define __NV_ENABLE_STREAM_MEMOPS EnableStreamMemOPs
#define NV_REG_ENABLE_STREAM_MEMOPS NV_REG_STRING(__NV_ENABLE_STREAM_MEMOPS)

/*
 * Option: EnableUserNUMAManagement
 *
 * Description:
 *
 * When this option is enabled, the NVIDIA kernel module will require the
 * user-mode NVIDIA Persistence daemon to manage the onlining and offlining
 * of its NUMA device memory.
 *
 * This option has no effect on platforms that do not support onlining
 * device memory to a NUMA node (this feature is only supported on certain
 * POWER9 systems).
 *
 * Possible Values:
 *
 *  0: disable user-mode NUMA management
 *  1: enable user-mode NUMA management (default)
 */
#define __NV_ENABLE_USER_NUMA_MANAGEMENT EnableUserNUMAManagement
#define NV_REG_ENABLE_USER_NUMA_MANAGEMENT NV_REG_STRING(__NV_ENABLE_USER_NUMA_MANAGEMENT)

/*
 * Option: GpuBlacklist
 *
 * Description:
 *
 * This option accepts a list of blacklisted GPUs, separated by commas, that
 * cannot be attached or used. Each blacklisted GPU is identified by a UUID in
 * the ASCII format with leading "GPU-". An exact match is required; no partial
 * UUIDs. This regkey is deprecated and will be removed in the future. Use
 * NV_REG_EXCLUDED_GPUS instead.
 */
#define __NV_GPU_BLACKLIST GpuBlacklist
#define NV_REG_GPU_BLACKLIST NV_REG_STRING(__NV_GPU_BLACKLIST)

/*
 * Option: ExcludedGpus
 *
 * Description:
 *
 * This option accepts a list of excluded GPUs, separated by commas, that
 * cannot be attached or used. Each excluded GPU is identified by a UUID in
 * the ASCII format with leading "GPU-". An exact match is required; no partial
 * UUIDs.
 */
#define __NV_EXCLUDED_GPUS ExcludedGpus
#define NV_REG_EXCLUDED_GPUS NV_REG_STRING(__NV_EXCLUDED_GPUS)

/*
 * Option: NvLinkDisable
 *
 * Description:
 *
 * When this option is enabled, the NVIDIA kernel module will not attempt to
 * initialize or train NVLink connections for any GPUs. System reboot is required
 * for changes to take affect.
 *
 * This option has no effect if no GPUs support NVLink.
 *
 * Possible Values:
 *
 *  0: Do not disable NVLink (default)
 *  1: Disable NVLink
 */
#define __NV_NVLINK_DISABLE NvLinkDisable
#define NV_REG_NVLINK_DISABLE NV_REG_STRING(__NV_NVLINK_DISABLE)

/*
 * Option: RestrictProfilingToAdminUsers
 *
 * Description:
 *
 * When this option is enabled, the NVIDIA kernel module will prevent users
 * without administrative access (i.e., the CAP_SYS_ADMIN capability) from
 * using GPU performance counters.
 *
 * Possible Values:
 *
 * 0: Do not restrict GPU counters (default)
 * 1: Restrict GPU counters to system administrators only
 */

#define __NV_RM_PROFILING_ADMIN_ONLY RmProfilingAdminOnly
#define __NV_RM_PROFILING_ADMIN_ONLY_PARAMETER RestrictProfilingToAdminUsers
#define NV_REG_RM_PROFILING_ADMIN_ONLY NV_REG_STRING(__NV_RM_PROFILING_ADMIN_ONLY)

/*
 * Option: TemporaryFilePath
 *
 * Description:
 *
 * When specified, this option changes the location in which the
 * NVIDIA kernel module will create unnamed temporary files (e.g. to
 * save the contents of video memory in).  The indicated file must
 * be a directory.  By default, temporary files are created in /tmp.
 */
#define __NV_TEMPORARY_FILE_PATH TemporaryFilePath
#define NV_REG_TEMPORARY_FILE_PATH NV_REG_STRING(__NV_TEMPORARY_FILE_PATH)

/*
 * Option: PreserveVideoMemoryAllocations
 *
 * If enabled, this option prompts the NVIDIA kernel module to save and
 * restore all video memory allocations across system power management
 * cycles, i.e. suspend/resume and hibernate/restore.  Otherwise,
 * only select allocations are preserved.
 *
 * Possible Values:
 *
 *  0: Preserve only select video memory allocations (default)
 *  1: Preserve all video memory allocations
 */
#define __NV_PRESERVE_VIDEO_MEMORY_ALLOCATIONS PreserveVideoMemoryAllocations
#define NV_REG_PRESERVE_VIDEO_MEMORY_ALLOCATIONS \
    NV_REG_STRING(__NV_PRESERVE_VIDEO_MEMORY_ALLOCATIONS)

/*
 * Option: EnableS0ixPowerManagement
 *
 * When this option is enabled, the NVIDIA driver will use S0ix-based
 * power management for system suspend/resume, if both the platform and
 * the GPU support S0ix.
 *
 * During system suspend, if S0ix is enabled and
 * video memory usage is above the threshold configured by
 * 'S0ixPowerManagementVideoMemoryThreshold', video memory will be kept
 * in self-refresh mode while the rest of the GPU is powered down.
 *
 * Otherwise, the driver will copy video memory contents to system memory
 * and power off the video memory along with the GPU.
 *
 * Possible Values:
 *
 *  0: Disable S0ix based power management (default)
 *  1: Enable S0ix based power management
 */

#define __NV_ENABLE_S0IX_POWER_MANAGEMENT EnableS0ixPowerManagement
#define NV_REG_ENABLE_S0IX_POWER_MANAGEMENT \
    NV_REG_STRING(__NV_ENABLE_S0IX_POWER_MANAGEMENT)

/*
 * Option: S0ixPowerManagementVideoMemoryThreshold
 *
 * This option controls the threshold that the NVIDIA driver will use during
 * S0ix-based system power management.
 *
 * When S0ix is enabled and the system is suspended, the driver will
 * compare the amount of video memory in use with this threshold,
 * to decide whether to keep video memory in self-refresh or copy video
 * memory content to system memory.
 *
 * See the 'EnableS0ixPowerManagement' option.
 *
 * Values are expressed in Megabytes (1048576 bytes).
 *
 * Default value for this option is 256MB.
 *
 */
#define __NV_S0IX_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD \
    S0ixPowerManagementVideoMemoryThreshold
#define NV_REG_S0IX_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD \
    NV_REG_STRING(__NV_S0IX_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD)

/*
 * Option: DynamicPowerManagement
 *
 * This option controls how aggressively the NVIDIA kernel module will manage
 * GPU power through kernel interfaces.
 *
 * Possible Values:
 *
 *  0: Never allow the GPU to be powered down (default).
 *  1: Power down the GPU when it is not initialized.
 *  2: Power down the GPU after it has been inactive for some time.
 *  3: (Default) Power down the GPU after a period of inactivity (i.e.,
 *     mode 2) on Ampere or later notebooks.  Otherwise, do not power down
 *     the GPU.
 */
#define __NV_DYNAMIC_POWER_MANAGEMENT DynamicPowerManagement
#define NV_REG_DYNAMIC_POWER_MANAGEMENT \
    NV_REG_STRING(__NV_DYNAMIC_POWER_MANAGEMENT)

#define NV_REG_DYNAMIC_POWER_MANAGEMENT_NEVER   0
#define NV_REG_DYNAMIC_POWER_MANAGEMENT_COARSE  1
#define NV_REG_DYNAMIC_POWER_MANAGEMENT_FINE    2
#define NV_REG_DYNAMIC_POWER_MANAGEMENT_DEFAULT 3

/*
 * Option: DynamicPowerManagementVideoMemoryThreshold
 *
 * This option controls the threshold that the NVIDIA driver will use
 * when selecting the dynamic power management scheme.
 *
 * When the driver detects that the GPU is idle, it will compare the amount
 * of video memory in use with this threshold.
 *
 * If the current video memory usage is less than the threshold, the
 * driver may preserve video memory contents in system memory and power off
 * the video memory along with the GPU itself, if supported.  Otherwise,
 * the video memory will be kept in self-refresh mode while powering down
 * the rest of the GPU, if supported.
 *
 * Values are expressed in Megabytes (1048576 bytes).
 *
 * If the requested value is greater than 200MB (the default), then it
 * will be capped to 200MB.
 */
#define __NV_DYNAMIC_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD \
    DynamicPowerManagementVideoMemoryThreshold
#define NV_REG_DYNAMIC_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD \
    NV_REG_STRING(__NV_DYNAMIC_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD)

/*
 * Option: RegisterPCIDriver
 *
 * Description:
 *
 * When this option is enabled, the NVIDIA driver will register with
 * PCI subsystem.
 *
 * Possible values:
 *
 *  1 - register as PCI driver (default)
 *  0 - do not register as PCI driver
 */

#define __NV_REGISTER_PCI_DRIVER  RegisterPCIDriver
#define NV_REG_REGISTER_PCI_DRIVER NV_REG_STRING(__NV_REGISTER_PCI_DRIVER)

/*
 * Option: EnablePCIERelaxedOrderingMode
 *
 * Description:
 *
 * When this option is enabled, the registry key RmSetPCIERelaxedOrdering will
 * be set to NV_REG_STR_RM_SET_PCIE_TLP_RELAXED_ORDERING_FORCE_ENABLE, causing
 * every device to set the relaxed ordering bit to 1 in all outbound MWr
 * transaction-layer packets. This is equivalent to setting the regkey to
 * FORCE_ENABLE as a non-per-device registry key.
 *
 * Possible values:
 *  0 - Do not enable PCIe TLP relaxed ordering bit-setting (default)
 *  1 - Enable PCIe TLP relaxed ordering bit-setting
 */
#define  __NV_ENABLE_PCIE_RELAXED_ORDERING_MODE EnablePCIERelaxedOrderingMode
#define NV_REG_ENABLE_PCIE_RELAXED_ORDERING_MODE \
    NV_REG_STRING(__NV_ENABLE_PCIE_RELAXED_ORDERING_MODE)

/*
 * Option: EnableResizableBar
 *
 * Description:
 *
 * When this option is enabled, the NVIDIA driver will attempt to resize
 * BAR1 to match framebuffer size, or the next largest available size on
 * supported machines. This is currently only implemented for Linux.
 *
 * Possible values:
 *  0 - Do not enable PCI BAR resizing
 *  1 - Enable PCI BAR resizing
 */
#define __NV_ENABLE_RESIZABLE_BAR EnableResizableBar
#define NV_REG_ENABLE_RESIZABLE_BAR NV_REG_STRING(__NV_ENABLE_RESIZABLE_BAR)

/*
 * Option: EnableGpuFirmware
 *
 * Description:
 *
 * When this option is enabled, the NVIDIA driver will enable use of GPU
 * firmware.
 *
 * If this key is set globally to the system, the driver may still attempt
 * to apply some policies to maintain uniform firmware modes across all
 * GPUS.  This may result in the driver failing initialization on some GPUs
 * to maintain such a policy.
 *
 * If this key is set using NVreg_RegistryDwordsPerDevice, then the driver 
 * will attempt to honor whatever configuration is specified without applying
 * additional policies.  This may also result in failed GPU initialzations if
 * the configuration is not possible (for example if the firmware is missing 
 * from the filesystem, or the GPU is not capable).
 *
 * NOTE: More details for this regkey can be found in nv-firmware-registry.h
 */
#define __NV_ENABLE_GPU_FIRMWARE  EnableGpuFirmware
#define NV_REG_ENABLE_GPU_FIRMWARE NV_REG_STRING(__NV_ENABLE_GPU_FIRMWARE)

/*
 * Option: EnableGpuFirmwareLogs
 *
 * When this option is enabled, the NVIDIA driver will send GPU firmware logs
 * to the system log, when possible.
 *
 * NOTE: More details for this regkey can be found in nv-firmware-registry.h
 */
#define __NV_ENABLE_GPU_FIRMWARE_LOGS  EnableGpuFirmwareLogs
#define NV_REG_ENABLE_GPU_FIRMWARE_LOGS NV_REG_STRING(__NV_ENABLE_GPU_FIRMWARE_LOGS)

/*
 * Option: EnableDbgBreakpoint
 *
 * When this option is set to a non-zero value, and the kernel is configured
 * appropriately, assertions within resman will trigger a CPU breakpoint (e.g.,
 * INT3 on x86_64), assumed to be caught by an attached debugger.
 *
 * When this option is set to the value zero (the default), assertions within
 * resman will print to the system log, but no CPU breakpoint will be triggered.
 */
#define __NV_ENABLE_DBG_BREAKPOINT EnableDbgBreakpoint


/*
 * Option: OpenRmEnableUnsupportedGpus
 *
 * This option to require opt in for use of Open RM on non-Data Center
 * GPUs is deprecated and no longer required. The kernel module parameter
 * is left here, though ignored, for backwards compatibility.
 */
#define __NV_OPENRM_ENABLE_UNSUPPORTED_GPUS OpenRmEnableUnsupportedGpus

/*
 * Option: NVreg_DmaRemapPeerMmio
 *
 * Description:
 *
 * When this option is enabled, the NVIDIA driver will use device driver
 * APIs provided by the Linux kernel for DMA-remapping part of a device's
 * MMIO region to another device, creating e.g., IOMMU mappings as necessary.
 * When this option is disabled, the NVIDIA driver will instead only apply a
 * fixed offset, which may be zero, to CPU physical addresses to produce the
 * DMA address for the peer's MMIO region, and no IOMMU mappings will be
 * created.
 *
 * This option only affects peer MMIO DMA mappings, and not system memory
 * mappings.
 *
 * Possible Values:
 *  0 = disable dynamic DMA remapping of peer MMIO regions
 *  1 = enable dynamic DMA remapping of peer MMIO regions (default)
 */
#define __NV_DMA_REMAP_PEER_MMIO DmaRemapPeerMmio
#define NV_DMA_REMAP_PEER_MMIO NV_REG_STRING(__NV_DMA_REMAP_PEER_MMIO)
#define NV_DMA_REMAP_PEER_MMIO_DISABLE  0x00000000
#define NV_DMA_REMAP_PEER_MMIO_ENABLE   0x00000001

/*
 * Option: NVreg_RmNvlinkBandwidthLinkCount
 *
 * Description:
 *
 * This option allows user to reduce the GPU nvlink bandwidth to save power.
 *
 * This option is only for Blackwell+ GPU with NVLINK version 5.0.
 */
#define __NV_RM_NVLINK_BW_LINK_COUNT RmNvlinkBandwidthLinkCount
#define NV_RM_NVLINK_BW_LINK_COUNT NV_REG_STRING(__NV_RM_NVLINK_BW_LINK_COUNT)

/*
 * Option: NVreg_RmNvlinkBandwidth
 *
 * Description:
 *
 * This option allows user to reduce the NVLINK P2P bandwidth to save power.
 * The option is in the string format.
 *
 * Possible string values:
 *   OFF:      0% bandwidth
 *   MIN:      15%-25% bandwidth depending on the system's NVLink topology
 *   HALF:     50% bandwidth
 *   3QUARTER: 75% bandwidth
 *   FULL:     100% bandwidth (default)
 *
 * This option is only for Hopper+ GPU with NVLINK version 4.0.
 */
#define __NV_RM_NVLINK_BW RmNvlinkBandwidth
#define NV_RM_NVLINK_BW NV_REG_STRING(__NV_RM_NVLINK_BW)

/*
 * Option: NVreg_EnableNonblockingOpen
 *
 * Description:
 *
 * When this option is enabled, the NVIDIA driver will try to perform any
 * required device initialization in the background when /dev/nvidiaN devices
 * are opened with the flag O_NONBLOCK.
 *
 * Possible Values:
 *  0 = O_NONBLOCK flag when opening devices is ignored
 *  1 = O_NONBLOCK flag when opening devices results in background device
 *      initialization (default)
 */
#define __NV_ENABLE_NONBLOCKING_OPEN EnableNonblockingOpen
#define NV_ENABLE_NONBLOCKING_OPEN NV_REG_STRING(__NV_ENABLE_NONBLOCKING_OPEN)

/*
 * Option: NVreg_ImexChannelCount
 *
 * Description:
 *
 * This option allows users to specify the number of IMEX (import/export)
 * channels. Within an IMEX domain, the channels allow sharing memory
 * securely in a multi-user environment using the CUDA driver's fabric handle
 * based APIs.
 *
 * An IMEX domain is either an OS instance or a group of securely
 * connected OS instances using the NVIDIA IMEX daemon. The option must
 * be set to the same value on each OS instance within the IMEX domain.
 *
 * An IMEX channel is a logical entity that is represented by a /dev node.
 * The IMEX channels are global resources within the IMEX domain. When
 * exporter and importer CUDA processes have been granted access to the
 * same IMEX channel, they can securely share memory.
 *
 * Note that the NVIDIA driver will not attempt to create the /dev nodes. Thus,
 * the related CUDA APIs will fail with an insufficient permission error until
 * the /dev nodes are set up. The creation of these /dev nodes,
 * /dev/nvidia-caps-imex-channels/channelN, must be handled by the
 * administrator, where N is the minor number. The major number can be
 * queried from /proc/devices.
 *
 * nvidia-modprobe CLI support is available to set up the /dev nodes.
 * NVreg_ModifyDeviceFiles, NVreg_DeviceFileGID, NVreg_DeviceFileUID
 * and NVreg_DeviceFileMode will be honored by nvidia-modprobe.
 *
 * Also, refer to the NVreg_CreateImexChannel0 option.
 *
 * Possible values:
 *  0 - Disable IMEX using CUDA driver's fabric handles.
 *  N - N IMEX channels will be enabled in the driver to facilitate N
 *      concurrent users. Default value is 2048 channels, and the current
 *      maximum value is 20-bit, same as Linux dev_t's minor number limit.
 */
#define __NV_IMEX_CHANNEL_COUNT ImexChannelCount
#define NV_REG_IMEX_CHANNEL_COUNT NV_REG_STRING(__NV_IMEX_CHANNEL_COUNT)

/*
 * Option: NVreg_CreateImexChannel0
 *
 * Description:
 *
 * This option allows users to specify whether the NVIDIA driver must create
 * the IMEX channel 0 by default. The channel will be created automatically
 * when the NVIDIA open GPU kernel module is loaded.
 *
 * Note that users are advised to enable this option only in trusted
 * environments where it is acceptable for applications to share the same
 * IMEX channel.
 *
 * For more details on IMEX channels, refer to the NVreg_ImexChannelCount
 * option.
 *
 * Possible values:
 *  0 - Do not create IMEX channel 0 (default).
 *  1 - Create IMEX channel 0.
 */
#define __NV_CREATE_IMEX_CHANNEL_0 CreateImexChannel0
#define NV_CREATE_IMEX_CHANNEL_0 NV_REG_STRING(__CREATE_IMEX_CHANNEL_0)

/*
 * Option: NVreg_GrdmaPciTopoCheckOverride
 *
 * Description:
 *
 * This option is applicable only on coherent systems with BAR1 enabled to allow
 * maximum bandwidth between GPU and a third party device over a dedicated
 * PCIe link instead of over C2C for GPUDirect RDMA use-cases.
 * Such a config is only supported for a specific topology which is checked by
 * the GPU driver's dma-buf and nv-p2p subsystems.
 *
 * This option allows the user to override the driver's topology check.
 *
 * Possible values:
 * 0 - Do not override topology check (default).
 * 1 - Override topology check.
 */
#define __NV_GRDMA_PCI_TOPO_CHECK_OVERRIDE GrdmaPciTopoCheckOverride
#define NV_GRDMA_PCI_TOPO_CHECK_OVERRIDE NV_REG_STRING(__NV_GRDMA_PCI_TOPO_CHECK_OVERRIDE)

#if defined(NV_DEFINE_REGISTRY_KEY_TABLE)

/*
 *---------registry key parameter declarations--------------
 */

NV_DEFINE_REG_ENTRY(__NV_RESMAN_DEBUG_LEVEL, ~0);
NV_DEFINE_REG_ENTRY(__NV_RM_LOGON_RC, 1);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_MODIFY_DEVICE_FILES, 1);
NV_DEFINE_REG_ENTRY(__NV_DEVICE_FILE_UID, 0);
NV_DEFINE_REG_ENTRY(__NV_DEVICE_FILE_GID, 0);
NV_DEFINE_REG_ENTRY(__NV_DEVICE_FILE_MODE, 0666);
NV_DEFINE_REG_ENTRY(__NV_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS, 1);
NV_DEFINE_REG_ENTRY(__NV_USE_PAGE_ATTRIBUTE_TABLE, ~0);
NV_DEFINE_REG_ENTRY(__NV_ENABLE_PCIE_GEN3, 0);
NV_DEFINE_REG_ENTRY(__NV_ENABLE_MSI, 1);
NV_DEFINE_REG_ENTRY(__NV_ENABLE_STREAM_MEMOPS, 0);
NV_DEFINE_REG_ENTRY(__NV_RM_PROFILING_ADMIN_ONLY_PARAMETER, 1);
NV_DEFINE_REG_ENTRY(__NV_PRESERVE_VIDEO_MEMORY_ALLOCATIONS, 0);
NV_DEFINE_REG_ENTRY(__NV_ENABLE_S0IX_POWER_MANAGEMENT, 0);
NV_DEFINE_REG_ENTRY(__NV_S0IX_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD, 256);
NV_DEFINE_REG_ENTRY(__NV_DYNAMIC_POWER_MANAGEMENT, 3);
NV_DEFINE_REG_ENTRY(__NV_DYNAMIC_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD, 200);
NV_DEFINE_REG_ENTRY(__NV_ENABLE_GPU_FIRMWARE, NV_REG_ENABLE_GPU_FIRMWARE_DEFAULT_VALUE);
NV_DEFINE_REG_ENTRY(__NV_ENABLE_GPU_FIRMWARE_LOGS, NV_REG_ENABLE_GPU_FIRMWARE_LOGS_ENABLE_ON_DEBUG);
NV_DEFINE_REG_ENTRY(__NV_OPENRM_ENABLE_UNSUPPORTED_GPUS, 1);

NV_DEFINE_REG_ENTRY_GLOBAL(__NV_ENABLE_USER_NUMA_MANAGEMENT, 1);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_MEMORY_POOL_SIZE, 0);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_KMALLOC_HEAP_MAX_SIZE, 0);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_VMALLOC_HEAP_MAX_SIZE, 0);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_IGNORE_MMIO_CHECK, 0);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_NVLINK_DISABLE, 0);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_ENABLE_PCIE_RELAXED_ORDERING_MODE, 0);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_REGISTER_PCI_DRIVER, 1);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_ENABLE_RESIZABLE_BAR, 0);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_ENABLE_DBG_BREAKPOINT, 0);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_ENABLE_NONBLOCKING_OPEN, 1);

NV_DEFINE_REG_STRING_ENTRY(__NV_REGISTRY_DWORDS, NULL);
NV_DEFINE_REG_STRING_ENTRY(__NV_REGISTRY_DWORDS_PER_DEVICE, NULL);
NV_DEFINE_REG_STRING_ENTRY(__NV_RM_MSG, NULL);
NV_DEFINE_REG_STRING_ENTRY(__NV_GPU_BLACKLIST, NULL);
NV_DEFINE_REG_STRING_ENTRY(__NV_TEMPORARY_FILE_PATH, NULL);
NV_DEFINE_REG_STRING_ENTRY(__NV_EXCLUDED_GPUS, NULL);
NV_DEFINE_REG_ENTRY(__NV_DMA_REMAP_PEER_MMIO, NV_DMA_REMAP_PEER_MMIO_ENABLE);
NV_DEFINE_REG_STRING_ENTRY(__NV_RM_NVLINK_BW, NULL);
NV_DEFINE_REG_ENTRY(__NV_RM_NVLINK_BW_LINK_COUNT, 0);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_IMEX_CHANNEL_COUNT, 2048);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_CREATE_IMEX_CHANNEL_0, 0);
NV_DEFINE_REG_ENTRY_GLOBAL(__NV_GRDMA_PCI_TOPO_CHECK_OVERRIDE, 0);

/*
 *----------------registry database definition----------------------
 */

/*
 * You can enable any of the registry options disabled by default by
 * editing their respective entries in the table below. The last field
 * determines if the option is considered valid - in order for the
 * changes to take effect, you need to recompile and reload the NVIDIA
 * kernel module.
 */
nv_parm_t nv_parms[] = {
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_RESMAN_DEBUG_LEVEL),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_RM_LOGON_RC),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_MODIFY_DEVICE_FILES),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_DEVICE_FILE_UID),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_DEVICE_FILE_GID),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_DEVICE_FILE_MODE),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_INITIALIZE_SYSTEM_MEMORY_ALLOCATIONS),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_USE_PAGE_ATTRIBUTE_TABLE),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_ENABLE_MSI),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_ENABLE_PCIE_GEN3),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_MEMORY_POOL_SIZE),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_KMALLOC_HEAP_MAX_SIZE),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_VMALLOC_HEAP_MAX_SIZE),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_IGNORE_MMIO_CHECK),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_ENABLE_STREAM_MEMOPS),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_ENABLE_USER_NUMA_MANAGEMENT),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_NVLINK_DISABLE),
    NV_DEFINE_PARAMS_TABLE_ENTRY_CUSTOM_NAME(__NV_RM_PROFILING_ADMIN_ONLY,
        __NV_RM_PROFILING_ADMIN_ONLY_PARAMETER),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_PRESERVE_VIDEO_MEMORY_ALLOCATIONS),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_ENABLE_S0IX_POWER_MANAGEMENT),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_S0IX_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_DYNAMIC_POWER_MANAGEMENT),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_DYNAMIC_POWER_MANAGEMENT_VIDEO_MEMORY_THRESHOLD),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_REGISTER_PCI_DRIVER),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_ENABLE_PCIE_RELAXED_ORDERING_MODE),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_ENABLE_RESIZABLE_BAR),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_ENABLE_GPU_FIRMWARE),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_ENABLE_GPU_FIRMWARE_LOGS),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_RM_NVLINK_BW_LINK_COUNT),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_ENABLE_DBG_BREAKPOINT),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_OPENRM_ENABLE_UNSUPPORTED_GPUS),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_DMA_REMAP_PEER_MMIO),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_IMEX_CHANNEL_COUNT),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_CREATE_IMEX_CHANNEL_0),
    NV_DEFINE_PARAMS_TABLE_ENTRY(__NV_GRDMA_PCI_TOPO_CHECK_OVERRIDE),
    {NULL, NULL}
};

#elif defined(NVRM)

extern nv_parm_t nv_parms[];

#endif /* NV_DEFINE_REGISTRY_KEY_TABLE */

#endif /* _RM_REG_H_ */
