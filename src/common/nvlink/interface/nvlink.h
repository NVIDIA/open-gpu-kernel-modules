/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
//     nvlink.h
//

#ifndef _NVLINK_H_
#define _NVLINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nv-kernel-interface-api.h>
#include "nvlink_common.h"
#include "nvlink_lib_ctrl.h"
#include "nv_list.h"
#include "nvlink_errors.h"
#include "nvCpuUuid.h"

// Debug Prints
#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
        #define NVLINK_PRINT_ENABLED    1
        #define NVLINK_PRINT(format_and_stuff) nvlink_print format_and_stuff

        #define DBG_MODULE_NVLINK_CORE  __FILE__, __LINE__, __FUNCTION__
        #define DBG_MODULE_IBMNPU       DBG_MODULE_NVLINK_CORE
        #define DBG_MODULE_TEGRASHIM    DBG_MODULE_NVLINK_CORE
        #define DBG_MODULE_EBRIDGE      DBG_MODULE_NVLINK_CORE
        #define DBG_MODULE_NVSWITCH     DBG_MODULE_NVLINK_CORE
#else
    #define NVLINK_PRINT(format_and_stuff)  ((void)(0))
#endif

// Devices that support NVLINK
#define NVLINK_DEVICE_TYPE_EBRIDGE         0x0
#define NVLINK_DEVICE_TYPE_IBMNPU          0x1
#define NVLINK_DEVICE_TYPE_GPU             0x2
#define NVLINK_DEVICE_TYPE_NVSWITCH        0x3
#define NVLINK_DEVICE_TYPE_TEGRASHIM       0x4

// NVLink versions
#define NVLINK_DEVICE_VERSION_10           0x00000001
#define NVLINK_DEVICE_VERSION_20           0x00000002
#define NVLINK_DEVICE_VERSION_22           0x00000004
#define NVLINK_DEVICE_VERSION_30           0x00000005
#define NVLINK_DEVICE_VERSION_31           0x00000006
#define NVLINK_DEVICE_VERSION_40           0x00000007
#define NVLINK_DEVICE_VERSION_50           0x00000008

// Link Transition Timeouts in miliseconds
#define NVLINK_TRANSITION_OFF_TIMEOUT        1
#define NVLINK_TRANSITION_SAFE_TIMEOUT       70
#define NVLINK_TRANSITION_HS_TIMEOUT         7000
#define NVLINK_TRANSITION_ACTIVE_PENDING     2000
#define NVLINK_TRANSITION_POST_HS_TIMEOUT    70

// Link training seed values
#define NVLINK_MAX_SEED_NUM                6
#define NVLINK_MAX_SEED_BUFFER_SIZE        NVLINK_MAX_SEED_NUM + 1

#define NVLINK_MAX_SYSTEM_LINK_NUM         624

// Forwards
struct nvlink_device;
struct nvlink_device_handle;
struct nvlink_link;
struct nvlink_link_handlers;

// nvlink device state
struct nvlink_device
{
    NVListRec node;

    // List of links associated with this device
    NVListRec link_list;

    // Uniquely identifies a device in the core
    NvU64 deviceId;

    // Client supplied names and ids
    char *driverName;
    char *deviceName;
    NvU8 *uuid;

    // PCI Information
    struct nvlink_pci_info pciInfo;

    // Device type and status
    NvU64  type;
    NvBool initialized;
    
    // Training type: ALI or Non-ALI
    NvBool enableALI;

    // fabric node id
    NvU16  nodeId;

    // per Ioctrl data
    NvU32 numIoctrls;
    NvU32 numLinksPerIoctrl;
    NvU32 numActiveLinksPerIoctrl;

    //
    // boolean indicating if a given device
    // is a reduced nvlink config
    //
    NvBool bReducedNvlinkConfig;

    // Client private information
    void *pDevInfo;
};

// nvlink link change type
enum nvlink_link_change_type
{
    nvlink_retrain_from_off,
    nvlink_retrain_from_safe,

};

// nvlink link_change parameters 
struct nvlink_link_change
{
    struct nvlink_link *master;
    struct nvlink_link *slave;

    enum nvlink_link_change_type change_type;
};

//
// Structure representing Nvlink Error Threshold
//
struct nvlink_link_error_threshold
{
    NvU8 thresholdMan;
    NvU8 thresholdExp;
    NvU8 timescaleMan;
    NvU8 timescaleExp;
    NvBool bInterruptEn;
    NvBool bUserConfig;
    NvBool bInterruptTrigerred; // Error threshold interrupt generated
};

// nvlink link state
struct nvlink_link
{
    NVListRec node;

    // Device the link is associated with
    struct nvlink_device *dev;

    // Lock for per link structure
    void *linkLock;

    // Uniquely identifies a link in the core
    NvU64 linkId;

    // If this link is the master of its connection
    NvBool master;

    // Client supplied link name and number
    char  *linkName;
    NvU32 linkNumber;

    NvU64 token;

    // Link state
    NvU32 state;
    NvBool inSWCFG; 

    // Sublink states
    NvU32 tx_sublink_state;
    NvU32 rx_sublink_state;

    // Has rceiver detect passed
    NvBool bRxDetected;

    // Link failed when sending InitPll to minion
    NvBool bTxCommonModeFail;

    // Link failed when transitioning to SWCFG
    NvBool bSafeTransitionFail;

    // Link failed when sending INITPHASE5 to minion 
    NvBool bInitphase5Fails;

    // IP version
    NvU32 version;

    // Has state been saved
    NvBool bStateSaved;

    // Number of retries to put link to safe
    NvU32 safe_retries;

    // Set if LINK is ac coupled
    NvBool ac_coupled;

    // Number of retries to discover the other end of the link
    NvU32 packet_injection_retries;

    // Local Sid of the link.
    NvU64 localSid;

    // Remote Sid of the link.
    NvU64 remoteSid;

    // Remote LinkId to which the current link is connected.
    NvU32 remoteLinkId;

    NvU32 remoteDeviceType;

    // Has INITNEGOTIATE received CONFIG_GOOD (NVL3.0+)
    NvBool bInitnegotiateConfigGood;

    NvBool bCciManaged;

    // Power state transition status
    enum
    {
        nvlink_power_state_in_L0,
        nvlink_power_state_entering_L2,
        nvlink_power_state_in_L2,
        nvlink_power_state_exiting_L2
    } powerStateTransitionStatus;

    // Link handlers
    const struct nvlink_link_handlers *link_handlers;

    // Client private information
    void *link_info;

    // Outstanding link change request information
    struct nvlink_link_change link_change;

    //seed data for given nvlink
    NvU32 seedData[NVLINK_MAX_SEED_BUFFER_SIZE];

    struct nvlink_link_error_threshold errorThreshold;
};

// nvlink link handler ops
struct nvlink_link_handlers
{
    NV_API_CALL NvlStatus (*add)                        (struct nvlink_link *link);
    NV_API_CALL NvlStatus (*remove)                     (struct nvlink_link *link);
    NV_API_CALL NvlStatus (*lock)                       (struct nvlink_link *link);
    NV_API_CALL void      (*unlock)                     (struct nvlink_link *link);
    NV_API_CALL NvlStatus (*queue_link_change)          (struct nvlink_link_change *link_change);
    NV_API_CALL NvlStatus (*set_dl_link_mode)           (struct nvlink_link *link, NvU64  mode, NvU32 flags);
    NV_API_CALL NvlStatus (*get_dl_link_mode)           (struct nvlink_link *link, NvU64 *mode);
    NV_API_CALL NvlStatus (*set_tl_link_mode)           (struct nvlink_link *link, NvU64  mode, NvU32 flags);
    NV_API_CALL NvlStatus (*get_tl_link_mode)           (struct nvlink_link *link, NvU64 *mode);
    NV_API_CALL NvlStatus (*set_tx_mode)                (struct nvlink_link *link, NvU64  mode, NvU32 flags);
    NV_API_CALL NvlStatus (*get_tx_mode)                (struct nvlink_link *link, NvU64 *mode, NvU32 *subMode);
    NV_API_CALL NvlStatus (*set_rx_mode)                (struct nvlink_link *link, NvU64  mode, NvU32 flags);
    NV_API_CALL NvlStatus (*get_rx_mode)                (struct nvlink_link *link, NvU64 *mode, NvU32 *subMode);
    NV_API_CALL NvlStatus (*set_rx_detect)              (struct nvlink_link *link, NvU32 flags);
    NV_API_CALL NvlStatus (*get_rx_detect)              (struct nvlink_link *link);
    NV_API_CALL NvlStatus (*write_discovery_token)      (struct nvlink_link *link, NvU64  token);
    NV_API_CALL NvlStatus (*read_discovery_token)       (struct nvlink_link *link, NvU64 *token);
    NV_API_CALL void      (*training_complete)          (struct nvlink_link *link);
    NV_API_CALL void      (*get_uphy_load)              (struct nvlink_link *link, NvBool* bUnlocked);
    NV_API_CALL NvlStatus (*get_cci_link_mode)          (struct nvlink_link *link, NvU64 *mode);
    NV_API_CALL NvlStatus (*ali_training)               (struct nvlink_link *link);
};

//
// Represents an intranode connections in single/multi-node system.
// Both endpoints of the connection is visible from same node.
//
struct nvlink_intranode_conn
{
    NVListRec node;
    struct nvlink_link *end0;
    struct nvlink_link *end1;
};

//
// Represents internode connections in a multi-node system.
// One of the endpoint of the connection must be a local link.
//
struct nvlink_internode_conn
{
    NVListRec                    node;
    struct nvlink_link          *local_end;
    nvlink_remote_endpoint_info  remote_end;
};


// Typedefs
typedef struct nvlink_device           nvlink_device;
typedef struct nvlink_device_handle    nvlink_device_handle;
typedef struct nvlink_link             nvlink_link;
typedef struct nvlink_link_change      nvlink_link_change;
typedef struct nvlink_device_handlers  nvlink_device_handlers;
typedef struct nvlink_link_handlers    nvlink_link_handlers;
typedef struct nvlink_intranode_conn   nvlink_intranode_conn;
typedef struct nvlink_internode_conn   nvlink_internode_conn;
typedef enum   nvlink_link_change_type nvlink_link_change_type;
typedef struct nvlink_inband_data      nvlink_inband_data;


#define NVLINK_MAX_NUM_SAFE_RETRIES                 7
#define NVLINK_MAX_NUM_PACKET_INJECTION_RETRIES     4


// NVLINK LINK states
#define NVLINK_LINKSTATE_OFF                            0x00   // OFF
#define NVLINK_LINKSTATE_HS                             0x01   // High Speed
#define NVLINK_LINKSTATE_SAFE                           0x02   // Safe/Discovery State
#define NVLINK_LINKSTATE_FAULT                          0x03   // Faulty
#define NVLINK_LINKSTATE_RECOVERY                       0x04   // Recovery
#define NVLINK_LINKSTATE_FAIL                           0x05   // Unconnected/Fail
#define NVLINK_LINKSTATE_DETECT                         0x06   // Detect mode
#define NVLINK_LINKSTATE_RESET                          0x07   // Reset
#define NVLINK_LINKSTATE_ENABLE_PM                      0x08   // Enable Link Power Management
#define NVLINK_LINKSTATE_DISABLE_PM                     0x09   // Disable Link Power Management
#define NVLINK_LINKSTATE_SLEEP                          0x0A   // Sleep (L2)
#define NVLINK_LINKSTATE_SAVE_STATE                     0x0B   // Save state while entering L2
#define NVLINK_LINKSTATE_RESTORE_STATE                  0x0C   // Restore state while exiting L2
#define NVLINK_LINKSTATE_PRE_HS                         0x0E   // Settings before moving to High Speed
#define NVLINK_LINKSTATE_DISABLE_ERR_DETECT             0x0F   // Disable Error detection (interrupt)
#define NVLINK_LINKSTATE_LANE_DISABLE                   0x10   // Disable Lanes
#define NVLINK_LINKSTATE_LANE_SHUTDOWN                  0x11   // Shutdown Lanes in PHY
#define NVLINK_LINKSTATE_TRAFFIC_SETUP                  0x12   // Setup traffic flow after ACTIVE
#define NVLINK_LINKSTATE_INITPHASE1                     0x13   // INITPHASE1
#define NVLINK_LINKSTATE_INITNEGOTIATE                  0x14   // Initialize the negotiation (Ampere And Later)
#define NVLINK_LINKSTATE_POST_INITNEGOTIATE             0x15   // Sends DL stat
#define NVLINK_LINKSTATE_INITOPTIMIZE                   0x16   // INITOPTIMIZE
#define NVLINK_LINKSTATE_POST_INITOPTIMIZE              0x17   // POST INITOPTIMIZE DL stat check
#define NVLINK_LINKSTATE_DISABLE_HEARTBEAT              0x18   // Disables the heartbeat errors
#define NVLINK_LINKSTATE_CONTAIN                        0x19   // TL is in contain mode
#define NVLINK_LINKSTATE_INITTL                         0x1A   // INITTL
#define NVLINK_LINKSTATE_INITPHASE5                     0x1B   // INITPHASE5
#define NVLINK_LINKSTATE_ALI                            0x1C   // ALI 
#define NVLINK_LINKSTATE_ACTIVE_PENDING                 0x1D   // Intermediate state for a link going to active
#define NVLINK_LINKSTATE_TRAINING_CCI                   0x1E   // Intermediate state for a link that is still training
#define NVLINK_LINKSTATE_INVALID                        0xFF   // Invalid state

// NVLINK TX SUBLINK states
#define NVLINK_SUBLINK_STATE_TX_HS                      0x0   // TX High Speed
#define NVLINK_SUBLINK_STATE_TX_SINGLE_LANE             0x4   // TX Single Lane (1/8th or 1/4th) Mode (Deprecated)
#define NVLINK_SUBLINK_STATE_TX_LOW_POWER               0x4   // TX Single Lane Mode / L1
#define NVLINK_SUBLINK_STATE_TX_TRAIN                   0x5   // TX training
#define NVLINK_SUBLINK_STATE_TX_SAFE                    0x6   // TX Safe Mode
#define NVLINK_SUBLINK_STATE_TX_OFF                     0x7   // TX OFF
#define NVLINK_SUBLINK_STATE_TX_COMMON_MODE             0x8   // TX common mode enable
#define NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE     0x9   // TX common mode disable
#define NVLINK_SUBLINK_STATE_TX_DATA_READY              0xA   // Do Data Ready and Data Enable
#define NVLINK_SUBLINK_STATE_TX_EQ                      0xB   // TX equalization
#define NVLINK_SUBLINK_STATE_TX_PRBS_EN                 0xC   // TX IOBIST PRBS generator enable
#define NVLINK_SUBLINK_STATE_TX_POST_HS                 0xD   // TX Post High Speed settings

// NVLINK RX SUBLINK states
#define NVLINK_SUBLINK_STATE_RX_HS                      0x0   // RX High Speed
#define NVLINK_SUBLINK_STATE_RX_SINGLE_LANE             0x4   // RX Single Lane (1/8th or 1/4th) Mode (Deprecated)
#define NVLINK_SUBLINK_STATE_RX_LOW_POWER               0x4   // RX Single Lane Mode / L1
#define NVLINK_SUBLINK_STATE_RX_TRAIN                   0x5   // RX training
#define NVLINK_SUBLINK_STATE_RX_SAFE                    0x6   // RX Safe Mode
#define NVLINK_SUBLINK_STATE_RX_OFF                     0x7   // RX OFF
#define NVLINK_SUBLINK_STATE_RX_RXCAL                   0x8   // RX in calibration
#define NVLINK_SUBLINK_STATE_RX_INIT_TERM               0x9   // Enable RX termination

// NVLINK TX SUBLINK sub-states
#define NVLINK_SUBLINK_SUBSTATE_TX_STABLE               0x0   // TX Stable

// NVLINK RX SUBLINK sub-states
#define NVLINK_SUBLINK_SUBSTATE_RX_STABLE               0x0   // RX Stable

// State change flags
#define NVLINK_STATE_CHANGE_ASYNC                       0x0   // Don't wait for the state change to complete
#define NVLINK_STATE_CHANGE_SYNC                        0x1   // Wait for the state change to complete


/************************************************************************************************/
/***************************** NVLink library management functions ******************************/
/************************************************************************************************/

/*
 * Check if the nvlink core library is initialized
 */
NvBool nvlink_lib_is_initialized(void);

/*
 * Check if there are no devices registered
 */
NvBool nvlink_lib_is_device_list_empty(void);

/*
 * Get if a device registerd to the nvlink corelib has a reduced nvlink config
 */
NvBool nvlink_lib_is_registerd_device_with_reduced_config(void);

/************************************************************************************************/
/************************** NVLink library driver-side interface ********************************/
/***************** Manages device and link registration and un-registration *********************/
/************************************************************************************************/

/*
 * Associates device in the NVLink Core
 * During the call, the calling driver must support callbacks into the driver from Core
 */
NvlStatus nvlink_lib_register_device(nvlink_device *dev);

/*
 * Unassociates device in the NVLink Core
 * Includes removing any links related to the device if still registered
 * During the call, the calling driver must support callbacks into the driver from Core
 */
NvlStatus nvlink_lib_unregister_device(nvlink_device *dev);


/*
 * Associates link with a device in the NVLink Core
 * During the call, the calling driver must support callbacks into the driver from Core
 */
NvlStatus nvlink_lib_register_link(nvlink_device *dev, nvlink_link *link);

/*
 * Unassociates link from a device in the NVLink Core
 * During the call, the calling driver must support callbacks into the driver from Core
 */
NvlStatus nvlink_lib_unregister_link(nvlink_link *link);

/*
* Gets number of devices with type deviceType
*/
NvlStatus nvlink_lib_return_device_count_by_type(NvU32 deviceType, NvU32 *numDevices);


/************************************************************************************************/
/***************************** NVLink device management functions ******************************/
/************************************************************************************************/

/*
 * Update UUID and deviceName in core library
 */
NvlStatus nvlink_lib_update_uuid_and_device_name(nvlink_device_info *devInfo, 
                                                 NvU8 *uuid, 
                                                 char *deviceName);


/************************************************************************************************/
/******************************* NVLink link management functions *******************************/
/************************************************************************************************/

/*
 * Check if the device has no links registered
 */
NvBool nvlink_lib_is_link_list_empty(nvlink_device *dev);

/*
 * Get the link associated with the given device's link number
 */
NvlStatus nvlink_lib_get_link(nvlink_device  *device,
                              NvU32           link_id,
                              nvlink_link   **link);

/*
 * Set the link endpoint as the link master
 */
NvlStatus nvlink_lib_set_link_master(nvlink_link *link);

/*
 * Get the link master associated with this endpoint
 */
NvlStatus nvlink_lib_get_link_master(nvlink_link *link, nvlink_link **master);

/*
 * Set the training state for the given link as non-ALI or ALI
 */
NvlStatus nvlink_lib_link_set_training_mode(nvlink_link *link, NvBool enableALI);

/************************************************************************************************/
/*************************** NVLink topology discovery functions ********************************/
/************************************************************************************************/

/*
 * Get the connected remote endpoint information
 *   For a given link, return the other endpoint details it is connected
 *   to. If there is no connection associated with the given link, then
 *   conn_info.connected member will be NV_FALSE.
 *
 *   Note: This routine will not initiate any link initialization or topology
 *   discovery.
 */
NvlStatus nvlink_lib_get_remote_conn_info(nvlink_link *link, nvlink_conn_info *conn_info);

/*
 * Get the connected remote endpoint information
 *   For a given end of a link, returns the device and link information
 *   for the remote end along with a boolean variable that specifies if
 *   the topology detection was complete
 */
NvlStatus nvlink_lib_discover_and_get_remote_conn_info(nvlink_link      *end,
                                                       nvlink_conn_info *conn_info,
                                                       NvU32             flags,
                                                       NvBool            bForceDiscovery);


/************************************************************************************************/
/****************************** NVLink initialization functions *********************************/
/************************************************************************************************/

/*
 * Re-init a given link from OFF to SWCFG
 */
NvlStatus nvlink_lib_reinit_link_from_off_to_swcfg(nvlink_link *link,
                                                   NvU32        flags);

/************************************************************************************************/
/********************************** NVLink training functions ***********************************/
/************************************************************************************************/

/*
 * Train a given set of links from SWCFG to ACTIVE state
 *    a. For low training latency - caller passes all links as an array
 *    b. For high training latency - caller passes link one by one
 */
NvlStatus nvlink_lib_train_links_from_swcfg_to_active(nvlink_link **links,
                                                      NvU32         linkCount,
                                                      NvU32         flags);

/*
 * Train a given set of links of a device from L2 to ACTIVE state
 */
NvlStatus nvlink_lib_train_links_from_L2_to_active(nvlink_device *dev,
                                                   NvU32          linkMask,
                                                   NvU32          flags);

/*
 * Retrain a given link from SWCFG to ACTIVE
 */
NvlStatus nvlink_lib_retrain_link_from_swcfg_to_active(nvlink_link *link,
                                                       NvU32        flags);

/*
 * Save the seed Data passed in from an endpoint driver 
*/
NvlStatus nvlink_lib_save_training_seeds(nvlink_link * link,
                                         NvU32 *       seedData);
NvlStatus nvlink_lib_copy_training_seeds(nvlink_link * link,
                                         NvU32 * seedDataCopy);

/*
 * Send the endpoint driver back the seeds we have stored
*/
void nvlink_lib_restore_training_seeds(nvlink_link * link, 
                                       NvU32 *       seedData);

/*
 * Check that the requested links have trained to active 
*/
NvlStatus nvlink_lib_check_training_complete(nvlink_link **links,
                                             NvU32 linkCount);


/************************************************************************************************/
/********************************** NVLink shutdown functions ***********************************/
/************************************************************************************************/

/*
 * [CLEAN SHUTDOWN]
 * Shutdown given links of a device from active to L2 state
 */
NvlStatus nvlink_lib_powerdown_links_from_active_to_L2(nvlink_device *dev,
                                                       NvU32          linkMask,
                                                       NvU32          flags);

/*
 * [PSEUDO-CLEAN SHUTDOWN]
 * Shutdown the given array of links from ACTIVE to OFF state
 */
NvlStatus nvlink_lib_powerdown_links_from_active_to_off(nvlink_link **links,
                                                        NvU32         numLinks,
                                                        NvU32         flags);

/*
 * Power down the given array of links from ACTIVE to SWCFG state
 */
NvlStatus nvlink_lib_powerdown_links_from_active_to_swcfg(nvlink_link **links,
                                                          NvU32         numLinks,
                                                          NvU32         flags);

/*
 * Reset the given array of links
 */
NvlStatus nvlink_lib_reset_links(nvlink_link **links,
                                 NvU32         numLinks,
                                 NvU32         flags);

/*
 * Floorsweep the necessary links and set buffer ready on the active links
 */
NvlStatus nvlink_lib_powerdown_floorswept_links_to_off(nvlink_device *pDevice);


/*
 * Nvlink core library structure iterators
 */

#define FOR_EACH_DEVICE_REGISTERED(dev, head, node)  \
        nvListForEachEntry(dev, &head.node, node)

#define FOR_EACH_LINK_REGISTERED(link, dev, node)    \
        nvListForEachEntry(link, &dev->link_list, node)

#define FOR_EACH_LINK_REGISTERED_SAFE(link, next, dev, node)       \
        nvListForEachEntry_safe(link, next, &dev->link_list, node)

#define FOR_EACH_CONNECTION(conn, head, node)        \
        nvListForEachEntry(conn, &head.node, node)

#ifdef __cplusplus
}
#endif

#endif // _NVLINK_H_
