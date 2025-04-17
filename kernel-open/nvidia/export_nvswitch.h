/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVSWITCH_EXPORT_H_
#define _NVSWITCH_EXPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "nv_stdarg.h"
#include "nvlink_common.h"
#include "ioctl_common_nvswitch.h"

#define NVSWITCH_DRIVER_NAME            "nvidia-nvswitch"

#define NVSWITCH_MAX_BARS               1

#define NVSWITCH_DEVICE_INSTANCE_MAX    64

#define PCI_CLASS_BRIDGE_NVSWITCH       0x0680

#ifndef PCI_VENDOR_ID_NVIDIA
#define PCI_VENDOR_ID_NVIDIA            0x10DE
#endif

#define PCI_ADDR_OFFSET_VENDOR          0
#define PCI_ADDR_OFFSET_DEVID           2

#define NVSWITCH_NSEC_PER_SEC           1000000000ULL

#define NVSWITCH_DBG_LEVEL_MMIO         0x0
#define NVSWITCH_DBG_LEVEL_NOISY        0x1
#define NVSWITCH_DBG_LEVEL_INFO         0x2
#define NVSWITCH_DBG_LEVEL_SETUP        0x3
#define NVSWITCH_DBG_LEVEL_WARN         0x4
#define NVSWITCH_DBG_LEVEL_ERROR        0x5

#define NVSWITCH_LOG_BUFFER_SIZE         512

#define NVSWITCH_DMA_DIR_TO_SYSMEM      0
#define NVSWITCH_DMA_DIR_FROM_SYSMEM    1
#define NVSWITCH_DMA_DIR_BIDIRECTIONAL  2

#define NVSWITCH_I2C_CMD_READ               0
#define NVSWITCH_I2C_CMD_WRITE              1
#define NVSWITCH_I2C_CMD_SMBUS_READ         2
#define NVSWITCH_I2C_CMD_SMBUS_WRITE        3
#define NVSWITCH_I2C_CMD_SMBUS_QUICK_READ   4
#define NVSWITCH_I2C_CMD_SMBUS_QUICK_WRITE  5

typedef struct nvswitch_device nvswitch_device;
typedef struct NVSWITCH_CLIENT_EVENT NVSWITCH_CLIENT_EVENT;

/*
 * @Brief : The interface will check if the client's version is supported by the
 *          driver.
 *
 * @param[in] user_version        Version of the interface that the client is
 *                                compiled with.
 * @param[out] kernel_version     Version of the interface that the kernel driver
 *                                is compiled with. This information will be
 *                                filled even if the CTRL call returns
 *                                -NVL_ERR_NOT_SUPPORTED due to version mismatch.
 * @param[in] length              Version string buffer length
 *
 * @returns                       NVL_SUCCESS if the client is using compatible
 *                                interface.
 *                                -NVL_ERR_NOT_SUPPORTED if the client is using
 *                                incompatible interface.
 *                                Or, Other NVL_XXX status value.
 */
NvlStatus
nvswitch_lib_check_api_version
(
    const char *user_version,
    char *kernel_version,
    NvU32 length
);

/*
 * @Brief : Allocate a new nvswitch lib device instance.
 *
 * @Description : Creates and registers a new nvswitch device and registers
 *   with the nvlink library.  This only initializes software state,
 *   it does not initialize the hardware state.
 *
 * @param[in] pci_domain    pci domain of the device
 * @param[in] pci_bus       pci bus of the device
 * @param[in] pci_device    pci device of the device
 * @param[in] pci_func      pci function of the device
 * @param[in] device_id     pci device ID of the device
 * @param[in] os_handle     Device handle used to interact with OS layer
 * @param[in] os_instance   instance number of this device
 * @param[out] device       return device handle for interfacing with library
 *
 * @returns                 NVL_SUCCESS if the action succeeded
 *                          an NVL error code otherwise
 */
NvlStatus
nvswitch_lib_register_device
(
    NvU16 pci_domain,
    NvU8 pci_bus,
    NvU8 pci_device,
    NvU8 pci_func,
    NvU16 device_id,
    void *os_handle,
    NvU32 os_instance,
    nvswitch_device **device
);

/*
 * @Brief : Clean-up the software state for a nvswitch device.
 *
 * @Description :
 *
 * @param[in] device        device handle to destroy
 *
 * @returns                 none
 */
void
nvswitch_lib_unregister_device
(
    nvswitch_device *device
);

/*
 * @Brief : Initialize the hardware for a nvswitch device.
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 *
 * @returns                 NVL_SUCCESS if the action succeeded
 *                          -NVL_BAD_ARGS if bad arguments provided
 *                          -NVL_PCI_ERROR if bar info unable to be retrieved
 */
NvlStatus
nvswitch_lib_initialize_device
(
    nvswitch_device *device
);

/*
 * @Brief : Shutdown the hardware for a nvswitch device.
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 *
 * @returns                 NVL_SUCCESS if the action succeeded
 *                          -NVL_BAD_ARGS if bad arguments provided
 *                          -NVL_PCI_ERROR if bar info unable to be retrieved
 */
NvlStatus
nvswitch_lib_shutdown_device
(
    nvswitch_device *device
);

/*
 * @Brief Control call (ioctl) interface.
 *
 * @param[in] device        device to operate on
 * @param[in] cmd           Enumerated command to execute.
 * @param[in] params        Params structure to pass to the command.
 * @param[in] params_size   Size of the parameter structure.
 * @param[in] osPrivate     The private data structure for OS.
 *
 * @return                  NVL_SUCCESS on a successful command
 *                          -NVL_NOT_FOUND if target device unable to be found
 *                          -NVL_BAD_ARGS if an invalid cmd is provided
 *                          -NVL_BAD_ARGS if a null arg is provided
 *                          -NVL_ERR_GENERIC otherwise
 */
NvlStatus nvswitch_lib_ctrl
(
    nvswitch_device *device,
    NvU32 cmd,
    void *params,
    NvU64 size,
    void *osPrivate
);

/*
 * @Brief: Retrieve PCI information for a switch based from device instance
 *
 * @Description :
 *
 * @param[in]  lib_handle   device to query
 * @param[out] pciInfo      return pointer to nvswitch lib copy of device info
 */
void nvswitch_lib_get_device_info
(
    nvswitch_device *lib_handle,
    struct nvlink_pci_info **pciInfo
);

/*
 * @Brief: Retrieve BIOS version for an nvswitch device
 *
 * @Description: For devices with a BIOS, this retrieves the BIOS version.
 *
 * @param[in]  device  device to query
 * @param[out] version BIOS version is stored here
 *
 * @returns NVL_SUCCESS                 BIOS version was retrieved successfully
 *          -NVL_BAD_ARGS               an invalid device is provided
 *          -NVL_ERR_INVALID_STATE      an error occurred reading BIOS info
 *          -NVL_ERR_NOT_SUPPORTED      device doesn't support this feature
 */

NvlStatus
nvswitch_lib_get_bios_version
(
    nvswitch_device *device,
    NvU64 *version
);


/*
 * @Brief: Retrieve whether the device supports PCI pin interrupts
 *
 * @Description: Returns whether the device can use PCI pin IRQs
 *
 *
 * @returns NV_TRUE                 device can use PCI pin IRQs
 *          NV_FALSE                device cannot use PCI pin IRQs
 */

NvlStatus
nvswitch_lib_use_pin_irq
(
    nvswitch_device *device
);


/*
 * @Brief: Load platform information (emulation, simulation etc.).
 *
 * @param[in]  lib_handle   device
 *
 * @return                  NVL_SUCCESS on a successful command
 *                          -NVL_BAD_ARGS if an invalid device is provided
 */
NvlStatus nvswitch_lib_load_platform_info
(
    nvswitch_device *lib_handle
);

/*
 * @Brief : Enable interrupts for this device
 *
 * @Description :
 *
 * @param[in] device        device to enable
 *
 * @returns                 NVL_SUCCESS
 *                          -NVL_PCI_ERROR if there was a register access error
 */
void
nvswitch_lib_enable_interrupts
(
    nvswitch_device *device
);

/*
 * @Brief : Disable interrupts for this device
 *
 * @Description :
 *
 * @param[in] device        device to enable
 *
 * @returns                 NVL_SUCCESS
 *                          -NVL_PCI_ERROR if there was a register access error
 */
void
nvswitch_lib_disable_interrupts
(
    nvswitch_device *device
);

/*
 * @Brief : Check if interrupts are pending on this device
 *
 * @Description :
 *
 * @param[in] device        device to check
 *
 * @returns                 NVL_SUCCESS if there were no errors and interrupts were handled
 *                          -NVL_BAD_ARGS if bad arguments provided
 *                          -NVL_PCI_ERROR if there was a register access error
 *                          -NVL_MORE_PROCESSING_REQUIRED no interrupts were found for this device
 */
NvlStatus
nvswitch_lib_check_interrupts
(
    nvswitch_device *device
);

/*
 * @Brief : Services interrupts for this device
 *
 * @Description :
 *
 * @param[in] device        device to service
 *
 * @returns                 NVL_SUCCESS if there were no errors and interrupts were handled
 *                          -NVL_BAD_ARGS if bad arguments provided
 *                          -NVL_PCI_ERROR if there was a register access error
 *                          -NVL_MORE_PROCESSING_REQUIRED no interrupts were found for this device
 */
NvlStatus
nvswitch_lib_service_interrupts
(
    nvswitch_device *device
);

/*
 * @Brief : Get depth of error logs and port event log
 *
 * @Description :
 *
 * @param[in]  device       device to check
 *
 * @param[out] fatal        Count of fatal errors
 * @param[out] nonfatal     Count of non-fatal errors
 * @param[out] portEvent    Count of port events
 *
 * @returns                 NVL_SUCCESS if there were no errors and interrupts were handled
 *                          -NVL_NOT_FOUND if bad arguments provided
 */
NvlStatus
nvswitch_lib_get_log_count
(
    nvswitch_device *device,
    NvU32 *fatal, NvU32 *nonfatal, NvU32 *portEvent
);

/*
 * @Brief : Periodic thread-based dispatcher for kernel functions
 *
 * @Description : Its purpose is to do any background subtasks (data collection, thermal
 * monitoring, etc.  These subtasks may need to run at varying intervals, and
 * may even wish to adjust their execution period based on other factors.
 * Each subtask's entry notes the last time it was executed and its desired
 * execution period.  This function returns back to the dispatcher the desired
 * time interval before it should be called again.
 *
 * @param[in] device          The device to run background tasks on
 *
 * @returns nsec interval to wait before the next call.
 */
NvU64
nvswitch_lib_deferred_task_dispatcher
(
    nvswitch_device *device
);

/*
 * @Brief : Perform post init tasks
 *
 * @Description : Any device initialization/tests which need the device to be
 * initialized to a sane state go here.
 *
 * @param[in] device    The device to run the post-init on
 *
 * @returns             returns NvlStatus code, see nvlink_errors.h
 */
NvlStatus
nvswitch_lib_post_init_device
(
    nvswitch_device *device
);

/*
 * @Brief : Perform post init tasks for a blacklisted device
 *
 * @Description : Any initialization tasks that should be run after a
 *                blacklisted item should go here.
 *
 * @param[in] device    The device to run the post-init-blacklist on
 *
 * @returns             void
 */
void
nvswitch_lib_post_init_blacklist_device
(
    nvswitch_device *device
);

/*
 * @Brief : Get the UUID of the device
 *
 * @Description : Copies out the device's UUID into the uuid field
 *
 * @param[in] device    The device to get the UUID from
 *
 * @param[out] uuid     A pointer to a uuid struct in which the UUID is written to
 *
 * @returns             void
 */
void
nvswitch_lib_get_uuid
(
    nvswitch_device *device,
    NvUuid *uuid
);

/*
 * @Brief : Get the Physical ID of the device
 *
 * @Description : Copies out the device's Physical ID into the phys_id field
 *
 * @param[in] device    The device to get the UUID from
 *
 * @param[out] phys_id  A pointer to a NvU32 which the physical ID is written to
 *
 * @returns             NVL_SUCCESS if successful
 *                      -NVL_BAD_ARGS if bad arguments provided
 */
NvlStatus
nvswitch_lib_get_physid
(
    nvswitch_device *device,
    NvU32 *phys_id
);

/*
 * @Brief : Read the Fabric State for a nvswitch device.
 *
 * @Description : Returns the Fabric State for the device
 *
 * @param[in] device        a reference to the device
 * @param[in] *ptrs         references to the fabric state
 *
 * @returns                 NVL_SUCCESS if the action succeeded
 *                          -NVL_BAD_ARGS if bad arguments provided
 */
NvlStatus
nvswitch_lib_read_fabric_state
(
    nvswitch_device *device,
    NVSWITCH_DEVICE_FABRIC_STATE *device_fabric_state,
    NVSWITCH_DEVICE_BLACKLIST_REASON *device_blacklist_reason,
    NVSWITCH_DRIVER_FABRIC_STATE *driver_fabric_state
);

/*
 * @Brief : Validates PCI device id
 *
 * @Description : Validates PCI device id
 *
 * @param[in] device    The device id to be validated
 *
 * @returns             True if device id is valid
 */
NvBool
nvswitch_lib_validate_device_id
(
    NvU32 device_id
);

/*
 * @Brief : Gets an event if it exists in the Event list
 *
 * @Description : Gets an event if it is in the Device's Client
 *                Event list
 *
 * @param[in]  device         Device to operate on
 * @param[in]  osPrivate      The private data structure for the OS
 * @param[out] ppClientEvent  Double pointer to client event
 *
 * @returns                  NVL_SUCCESS if client event found
 *                           -NVL_BAD_ARGS if bad arguments provided
 *                           -NVL_NOT_FOUND if no client event found
 */
NvlStatus
nvswitch_lib_get_client_event
(
    nvswitch_device *device,
    void *osPrivate,
    NVSWITCH_CLIENT_EVENT **ppClientEvent
);

/*
 * @Brief : Adds a single entry into the Event list
 *
 * @Description : Adds an entry into the front of the Device's
 *                Client Event List
 *
 * @param[in] device     Device to operate on
 * @param[in] osPrivate  The private data structure for OS
 * @param[in] pParams    The parameters for the client event
 *
 * @returns              NVL_SUCCESS if event added
 *                       -NVL_BAD_ARGS if bad arguments provided
 *                       -NVL_NO_MEM if allocation fails
 */
NvlStatus
nvswitch_lib_add_client_event
(
    nvswitch_device *device,
    void *osPrivate,
    NvU32 eventId
);

/*
 * @Brief : Removes entries from the Event list
 *
 * @Description : Removes the entries associated with osPrivate
 *                from the Device's Client Event List
 *
 * @param[in] device     Device to operate on
 * @param[in] osPrivate  The private data structure for OS
 *
 * @returns              NVL_SUCCESS if event removed
 */
NvlStatus
nvswitch_lib_remove_client_events
(
    nvswitch_device *device,
    void *osPrivate
);

/*
 * @Brief : Notifies all events with a matching event Id in the Client Event list
 *
 * @Description : Notifies all events with a matching event Id in the Client Event list
 *
 * @param[in] device     Device to operate on
 * @param[in] eventId    The event ID to notify
 *
 * @returns              NVL_SUCCESS if arguments are valid
 *                       -NVL_BAD_ARGS if bad arguments provided
 */
NvlStatus
nvswitch_lib_notify_client_events
(
    nvswitch_device *device,
    NvU32 eventId
);

/*
 * @Brief : Gets a mask of valid I2C ports for the device
 *
 * @Description : Gets a mask of valid I2C ports for the device
 *
 * @param[in]  device          Device to operate on
 * @param[out] validPortsMask  A pointer to a mask of valid ports
 *
 * @returns              NVL_SUCCESS if successfuly
 *                       -NVL_BAD_ARGS if bad arguments provided
 */
NvlStatus
nvswitch_lib_get_valid_ports_mask
(
    nvswitch_device *device,
    NvU32 *validPortsMask
);

/*
 * @Brief : Returns a boolean if the I2C interface is supported for the device
 *
 * @Description : Returns a boolean if the I2C interface is supported for the device
 *
 * @param[in]  device         Device to operate on
 *
 * @returns NV_TRUE           device can use the I2C interface
 *          NV_FALSE          device cannot use the I2C interface
 */
NvBool
nvswitch_lib_is_i2c_supported
(
    nvswitch_device *device
);

/*
 * @Brief : Performs an I2C transaction
 *
 * @Description : Performs an I2C transaction
 *
 * @param[in]  device         Device to operate on
 * @param[in]  port           Port to issue I2C transaction
 * @param[in]  type           Type of I2C transaction
 * @param[in]  addr           Device address to perform I2C transaction on
 * @param[in]  command        I2C command to perform on
 * @param[in]  len            Length of the I2C transaction message
 * @param[in/out] pData       A pointer to the buffer containing the input/output data
 *
 * @returns              NVL_SUCCESS if I2C transaction completes
 *                       -NVL_BAD_ARGS if bad arguments provided
 *                       -NVL_ERR_INVALID_STATE if something internal went wrong
 */
NvlStatus
nvswitch_lib_i2c_transfer
(
    nvswitch_device *device,
    NvU32 port,
    NvU8 type,
    NvU8 addr,
    NvU8 command,
    NvU32 len,
    NvU8 *pData
);

/*
 * Returns count of registered NvSwitch devices.
 */
NvU32
nvswitch_os_get_device_count
(
    void
);

/*
 * Get current time in nanoseconds
 * The time is since epoch time (midnight UTC of January 1, 1970)
 */
NvU64
nvswitch_os_get_platform_time
(
    void
);

NvU64
nvswitch_os_get_platform_time_epoch
(
    void
);

#if (defined(_WIN32) || defined(_WIN64))
#define NVSWITCH_PRINT_ATTRIB(str, arg1)
#else
#define NVSWITCH_PRINT_ATTRIB(str, arg1)             \
    __attribute__ ((format (printf, (str), (arg1))))
#endif // (defined(_WIN32) || defined(_WIN64))

/*
 * printf wrapper
 */
void
NVSWITCH_PRINT_ATTRIB(2, 3)
nvswitch_os_print
(
    int         log_level,
    const char *pFormat,
    ...
);

/*
 * Log the given error code via an OS-specifric programmatic API
 */
void
NVSWITCH_PRINT_ATTRIB(3, 4)
nvswitch_os_report_error
(
    void *os_handle,
    NvU32 error_code,
    const char *fmt,
    ...
);

/*
 * "Registry" interface for dword
 */
NvlStatus
nvswitch_os_read_registry_dword
(
    void *os_handle,
    const char *name,
    NvU32 *data
);

/*
 * "Registry" interface for binary data
 */
NvlStatus
nvswitch_os_read_registery_binary
(
    void *os_handle,
    const char *name,
    NvU8 *data,
    NvU32 length
);

NvBool
nvswitch_os_is_uuid_in_blacklist
(
    NvUuid *uuid
);


/*
 * Override platform/simulation settings for cases
 */
void
nvswitch_os_override_platform
(
    void *os_handle,
    NvBool *rtlsim
);

/*
 * Memory management interface
 */
NvlStatus
nvswitch_os_alloc_contig_memory
(
    void *os_handle,
    void **virt_addr,
    NvU32 size,
    NvBool force_dma32
);

void
nvswitch_os_free_contig_memory
(
    void *os_handle,
    void *virt_addr,
    NvU32 size
);

NvlStatus
nvswitch_os_map_dma_region
(
    void *os_handle,
    void *cpu_addr,
    NvU64 *dma_handle,
    NvU32 size,
    NvU32 direction
);

NvlStatus
nvswitch_os_unmap_dma_region
(
    void *os_handle,
    void *cpu_addr,
    NvU64 dma_handle,
    NvU32 size,
    NvU32 direction
);

NvlStatus
nvswitch_os_set_dma_mask
(
    void *os_handle,
    NvU32 dma_addr_width
);

NvlStatus
nvswitch_os_sync_dma_region_for_cpu
(
    void *os_handle,
    NvU64 dma_handle,
    NvU32 size,
    NvU32 direction
);

NvlStatus
nvswitch_os_sync_dma_region_for_device
(
    void *os_handle,
    NvU64 dma_handle,
    NvU32 size,
    NvU32 direction
);

void *
nvswitch_os_malloc_trace
(
    NvLength size,
    const char *file,
    NvU32 line
);

void
nvswitch_os_free
(
    void *pMem
);

NvLength
nvswitch_os_strlen
(
    const char *str
);

char*
nvswitch_os_strncpy
(
    char *pDest,
    const char *pSrc,
    NvLength length
);

int
nvswitch_os_strncmp
(
    const char *s1,
    const char *s2,
    NvLength length
);

char*
nvswitch_os_strncat
(
    char *s1,
    const char *s2,
    NvLength length
);

void *
nvswitch_os_memset
(
    void *pDest,
    int value,
    NvLength size
);

void *
nvswitch_os_memcpy
(
    void *pDest,
    const void *pSrc,
    NvLength size
);

int
nvswitch_os_memcmp
(
    const void *s1,
    const void *s2,
    NvLength size
);

/*
 * Memory read / write interface
 */
NvU32
nvswitch_os_mem_read32
(
    const volatile void * pAddress
);

void
nvswitch_os_mem_write32
(
    volatile void *pAddress,
    NvU32 data
);

NvU64
nvswitch_os_mem_read64
(
    const volatile void *pAddress
);

void
nvswitch_os_mem_write64
(
    volatile void *pAddress,
    NvU64 data
);

/*
 * Interface to write formatted output to sized buffer
 */
int
nvswitch_os_snprintf
(
    char *pString,
    NvLength size,
    const char *pFormat,
    ...
);

/*
 * Interface to write formatted output to sized buffer
 */
int
nvswitch_os_vsnprintf
(
    char *buf,
    NvLength size,
    const char *fmt,
    va_list arglist
);

/*
 * Debug assert and log interface
 */
void
nvswitch_os_assert_log
(
    const char *pFormat,
    ...
);

/*
 * Interface to sleep for specified milliseconds. Yields the CPU to scheduler.
 */
void
nvswitch_os_sleep
(
    unsigned int ms
);

NvlStatus
nvswitch_os_acquire_fabric_mgmt_cap
(
    void *osPrivate,
    NvU64 capDescriptor
);

int
nvswitch_os_is_fabric_manager
(
    void *osPrivate
);

int
nvswitch_os_is_admin
(
    void
);

NvlStatus
nvswitch_os_get_os_version
(
    NvU32 *pMajorVer,
    NvU32 *pMinorVer,
    NvU32 *pBuildNum
);

NvlStatus
nvswitch_os_get_pid
(
    NvU32 *pPid
);

void
nvswitch_lib_smbpbi_log_sxid
(
    nvswitch_device *device,
    NvU32           sxid,
    const char      *pFormat,
    ...
);

/*!
 * @brief: OS Specific handling to add an event.
 */
NvlStatus
nvswitch_os_add_client_event
(
    void            *osHandle,
    void            *osPrivate,
    NvU32           eventId
);

/*!
 * @brief: OS specific handling to remove all events corresponding to osPrivate.
 */
NvlStatus
nvswitch_os_remove_client_event
(
    void            *osHandle,
    void            *osPrivate
);

/*!
 * @brief: OS specific handling to notify an event.
 */
NvlStatus
nvswitch_os_notify_client_event
(
    void *osHandle,
    void *osPrivate,
    NvU32 eventId
);

/*!
 * @brief: Gets OS specific support for the REGISTER_EVENTS ioctl
 */
NvlStatus
nvswitch_os_get_supported_register_events_params
(
    NvBool *bSupportsManyEvents,
    NvBool *bUserSuppliesOsData
);

/*
 * @Brief : Is TNVL mode enabled.
 *
 * @Description : Returns if TNVL is enabled for the device
 *
 * @param[in] device        a reference to the device
 *
 * @returns                 NV_TRUE,  if TNVL is enabled
 *                          NV_FALSE, if TNVL is disabled
 */
NvBool
nvswitch_lib_is_tnvl_enabled
(
    nvswitch_device *device
);

#ifdef __cplusplus
}
#endif
#endif //_NVSWITCH_EXPORT_H_
