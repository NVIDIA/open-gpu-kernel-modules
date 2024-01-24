/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVLINK_LIB_CTRL_H_
#define _NVLINK_LIB_CTRL_H_


#include "nvtypes.h"
#include "nvlink_errors.h"

/* List of supported capability type */
#define NVLINK_CAP_FABRIC_MANAGEMENT 0

/* 
 * Max supported capabilities count
 *
 */
#define NVLINK_CAP_COUNT 1

#define NVLINK_UUID_LEN 16

/*
 * Total number of nvlinks connections a device could have.
 */
#define NVLINK_MAX_DEVICE_CONN 64

/*
 * Total number of links to train in parallel
 */
#define NVLINK_MAX_PARALLEL_CONNS_TRAIN_COUNT 288

/*
 * Best effort to copy the driver supplied device name. Name more than
 * this size (if any) will be truncated.
 */
#define NVLINK_DEVICE_NAME_LEN_MAX 30

/*
 * Total number of nvlink devices supported. Derived based on current supported
 * limits. OBJ_MAX_GPUS = 32 and NVSWITCH_DEVICE_INSTANCE_MAX = 64
 */
#define NVLINK_DEVICE_INSTANCE_MAX 96

/*
 * Total number of nvlink endpoints core library can have
 *  This is mapped to NVLINK_MAX_SYSTEM_LINK_NUM in drivers/nvlink/interface/nvlink.h 
 */
#define NVLINK_MAX_NVLINK_ENDPOINTS 624

#define NVLINK_VERSION_STRING_LENGTH    64

#define NVLINK_CCI_TRAINING_TIMEOUT_SEC 30          

/*
 * NVLink version consists of,
 *   major - no compatibility.
 *   minor - only backwards compatible.
 */
typedef struct
{
    char version[NVLINK_VERSION_STRING_LENGTH];
} nvlink_version;

typedef struct
{
    NvU16 domain;
    NvU8  bus;
    NvU8  device;
    NvU8  function;
} nvlink_pci_dev_info;

/* uniquely identify an nvlink endpoint */
typedef struct
{
    NvU16               nodeId;
    NvU16               linkIndex;
    nvlink_pci_dev_info pciInfo;
} nvlink_endpoint;

/* uniquely identify an nvlink device */
typedef struct
{
    NvU16               nodeId;
    nvlink_pci_dev_info pciInfo;
} nvlink_device_info;

/* detailed information about an nvlink device */
typedef struct
{
    nvlink_pci_dev_info pciInfo;
    char                deviceName[NVLINK_DEVICE_NAME_LEN_MAX];
    NvU8                devUuid[NVLINK_UUID_LEN];
    NvU16               numLinks;
    NvU32               devType;
    NV_DECLARE_ALIGNED(NvU64 enabledLinkMask, 8);
    NvBool              bEnableAli;
    /* See struct definition modification guidelines at the top of this file */
} nvlink_detailed_dev_info;

/* detailed information about a remote nvlink connection endpoint */
typedef struct
{
    NvU16               nodeId;
    NvU16               linkIndex;
    nvlink_pci_dev_info pciInfo;
    NvU8                devUuid[NVLINK_UUID_LEN];
    NvU32               devType;
} nvlink_remote_endpoint_info;

/* uniquely identify an nvlink connection */
typedef struct
{
    nvlink_endpoint srcEndPoint;
    nvlink_endpoint dstEndPoint;
} nvlink_connection_info;

/* link device types */
typedef enum
{
    nvlink_device_type_unknown = 0,
    nvlink_device_type_gpu,
    nvlink_device_type_ibmnpu,
    nvlink_device_type_nvswitch
} nvlink_device_type;

/* link modes */
typedef enum
{
    nvlink_link_mode_unknown = 0,
    nvlink_link_mode_off,
    nvlink_link_mode_active,
    nvlink_link_mode_swcfg,
    nvlink_link_mode_fault,
    nvlink_link_mode_recovery,
    nvlink_link_mode_fail,
    nvlink_link_mode_detect,
    nvlink_link_mode_reset,
    nvlink_link_mode_enable_pm,
    nvlink_link_mode_disable_pm,
    nvlink_link_mode_traffic_setup,
    nvlink_link_mode_contain,
    nvlink_link_mode_training_cci
} nvlink_link_mode;

/* sublink tx modes */
typedef enum
{
    nvlink_tx_sublink_mode_unknown = 0,
    nvlink_tx_sublink_mode_hs,
    nvlink_tx_sublink_mode_single_lane,
    nvlink_tx_sublink_mode_train,
    nvlink_tx_sublink_mode_safe,
    nvlink_tx_sublink_mode_off,
    nvlink_tx_sublink_mode_common_mode,
    nvlink_tx_sublink_mode_common_mode_disable,
    nvlink_tx_sublink_mode_data_ready,
    nvlink_tx_sublink_mode_tx_eq,
    nvlink_tx_sublink_mode_pbrs_en,
    nvlink_tx_sublink_mode_post_hs
} nvlink_tx_sublink_mode;

/* sublink rx modes */
typedef enum
{
    nvlink_rx_sublink_mode_unknown = 0,
    nvlink_rx_sublink_mode_hs,
    nvlink_rx_sublink_mode_single_lane,
    nvlink_rx_sublink_mode_train,
    nvlink_rx_sublink_mode_safe,
    nvlink_rx_sublink_mode_off,
    nvlink_rx_sublink_mode_rxcal
} nvlink_rx_sublink_mode;

/* link and sublink state of an nvlink endpoint */
typedef struct
{
    NvU8 linkMode;
    NvU8 txSubLinkMode;
    NvU8 rxSubLinkMode;
} nvlink_link_state;

/*
 * CTRL_NVLINK_CHECK_VERSION
 *
 * The interface will check if the client's version is supported by the driver.
 *
 * Parameters:
 *   user [IN]
 *      version of the interface that the client is compiled with.
 *
 *   kernel [OUT]
 *      version of the interface that the kernel driver is compiled with. This
 *      information will be filled even if the CTRL call returns
 *      NVL_ERR_NOT_SUPPORTED due to version mismatch.
 *
 *   status [OUT]
 *      NVL_SUCCESS if the client is using compatible interface.
 *      NVL_ERR_NOT_SUPPORTED if the client is using incompatible interface.
 *      Or, Other NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */
    nvlink_version user;

    /* output parameters */
    nvlink_version kernel;
    NvlStatus      status;
} nvlink_check_version;

/*
 * CTRL_NVLINK_SET_NODE_ID
 * 
 * Assign the nvlink devices with a fabric node id.
 * This interface will enumerate all the existing registered devices (if any)
 * and update its node id. Any subsequent nvlink device registration will use
 * this node id.
 *
 * Parameters:
 *   nodeId [IN]
 *      fabric node id of the node.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */
    NvU16     nodeId;

    /* output parameters */
    NvlStatus status;
} nvlink_set_node_id;

/*
 * CTRL_NVLINK_SET_TX_COMMON_MODE
 * 
 * Set common mode for all device links.
 * A user mode module, which has no validated and stored nvlink connection should
 * do a device discovery. This interface allows all the links to enable or disable
 * common mode to facilitate such device discovery.
 *
 * Parameters:
 *   commMode [IN]
 *      Specify the desired common mode, True to enable and False to disable.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */
    NvBool    commMode;

    /* output parameters */
    NvlStatus status;
} nvlink_set_tx_common_mode;

/*
 * CTRL_NVLINK_CALIBRATE
 * 
 * Do Rx Calibration for all the links.
 * A user mode module, which has no validated and stored nvlink connection
 * should do a device discovery. This interface allows all the links to 
 * initiate Rx calibration as part of this discovery process.
 *
 * Parameters:
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */

    /* output parameters */
    NvlStatus status;
} nvlink_calibrate;

/*
 * CTRL_NVLINK_ENABLE_DATA
 * 
 * Enable TX data for all the registered links in the node.
 * A user mode module, which has no validated and stored nvlink connection should
 * do a device discovery. This interface enable data mode for all the links as part
 * of this discovery process.
 *
 * Parameters:
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */

    /* output parameters */
    NvlStatus status;
} nvlink_enable_data;

/*
 * CTRL_NVLINK_LINK_INIT_ASYNC
 * 
 * Initialize all the registered links in the node.
 * Initializes all the registered links in the node and start SWCFG state
 * transition for the links. However, this interface will not wait/poll
 * for the links to finish the initialization. The caller must use
 * CTRL_NVLINK_DEVICE_LINK_INIT_STATUS to query the link status which will 
 * wait for the initialization to complete and report how many links are 
 * transitioned to SWCFG state.
 *
 * Parameters:
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */

    /* output parameters */
    NvlStatus status;
} nvlink_link_init_async;

/*
 * CTRL_NVLINK_DEVICE_LINK_INIT_STATUS
 * 
 * Query link initialization status of the specified device.
 * This interface poll/wait for the link initialization to complete and report
 * per link initialization status. The caller can initiate a link initialization
 * request using the CTRL_NVLINK_LINK_INIT_ASYNC interface.
 *
 * Parameters:
 *   devInfo [OUT]
 *      nvlink device identification information.
 * 
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   initStatus [OUT]
 *      Per link init state information. Link will be in SWCFG mode on True.
 */
typedef struct
{
    NvU16  linkIndex;
    NvBool initStatus;
} nvlink_link_init_status;

typedef struct
{
    /* input parameters */
    nvlink_device_info       devInfo;

    /* output parameters */
    NvlStatus                status;
    nvlink_link_init_status  linkStatus[NVLINK_MAX_DEVICE_CONN];
} nvlink_device_link_init_status;

/*
 * CTRL_NVLINK_DISCOVER_INTRANODE_CONNS
 * 
 * Initiate an nvlink connection discovery.
 * This interface allows the node to initiate an nvlink connection discovery
 * process by writing and reading specific discovery tokens. The discovered
 * connections are then registered in the nvlink driver context as intranode
 * connections.
 *
 * Note:
 *   1) Link has to be in SWCFG/HS mode to participate in the discovery process.
 *   2) This interface will discover only intranode connections.
 *
 * Parameters:
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */

    /* output parameters */
    NvlStatus status;
} nvlink_discover_intranode_conns;

/*
 * CTRL_NVLINK_DEVICE_GET_INTRANODE_CONNS
 * 
 * Returns all the nvlink intranode connections for a device.
 * This interface allows a user mode module to retrive endpoint details 
 * of all the intranode connections for the specified device.
 *
 * Note:
 *   1) Issue CTRL_NVLINK_DISCOVER_INTRANODE_CONNS to discover connections first.
 *
 * Parameters:
 *   devInfo [OUT]
 *      nvlink device identification information.
 * 
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   numConnections [OUT]
 *      The number of nvlink connections discovered for this device.
 *
 *   conn [OUT]
 *      Detailed device information of each connection to this device.
 */
typedef struct
{
    /* input parameters */
    nvlink_device_info     devInfo;

    /* output parameters */
    NvlStatus              status;
    NvU32                  numConnections;
    nvlink_connection_info conn[NVLINK_MAX_DEVICE_CONN];
} nvlink_device_get_intranode_conns;

/*
 * CTRL_NVLINK_ADD_INTERNODE_CONN
 * 
 * Register an internode nvlink connection with driver.
 * This interface allows a user mode module to populate internode nvlink
 * connections which are discovered through an nvlink device discovery
 * process. This is applicable only for multi-node systems where only one
 * endpoint of the connection is visible/accessible from a given node.
 *
 * Parameters:
 *   localEndPoint [IN]
 *      Local endpoint information of the internode connection.
 *
 *   remoteEndPoint [IN]
 *      Remote endpoint and device information of the internode connection.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */
    nvlink_endpoint             localEndPoint;
    nvlink_remote_endpoint_info remoteEndPoint;

    /* output parameters */
    NvlStatus                   status;
} nvlink_add_internode_conn;

/*
 * CTRL_NVLINK_REMOVE_INTERNODE_CONN
 * 
 * Remove a previously added internode nvlink connection from the driver.
 * This interface allows a user mode module to remove an internode nvlink
 * connection. This is applicable only for multi-node systems where only one
 * endpoint of the connection is visible/accessible from a given node.
 *
 * Parameters:
 *   localEndPoint [IN]
 *      Local endpoint information of the internode connection.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */
    nvlink_endpoint localEndPoint;

    /* output parameters */
    NvlStatus       status;
} nvlink_remove_internode_conn;

/*
 * CTRL_NVLINK_DEVICE_WRITE_DISCOVERY_TOKENS
 * 
 * Write discovery token on links which are not part of any connection.
 * This interface allows a user mode module, which should discover all the
 * internode connections to write a unique discovery token on all the links
 * of the specified device.
 *
 * Parameters:
 *   devInfo [OUT]
 *      nvlink device identification information.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   numTokens [OUT]
 *      Number of nvlink tokens written.
 *
 *   tokenInfo [OUT]
 *      link index, and value of each token written.
 */
typedef struct
{
    NvU16 linkIndex;
    NV_DECLARE_ALIGNED(NvU64 tokenValue, 8);
} nvlink_token_info;

typedef struct
{
    /* input parameters */
    nvlink_device_info  devInfo;

    /* output parameters */
    NvlStatus           status;
    NvU32               numTokens;
    nvlink_token_info   tokenInfo[NVLINK_MAX_DEVICE_CONN];
} nvlink_device_write_discovery_tokens;

/*
 * CTRL_NVLINK_DEVICE_READ_DISCOVERY_TOKENS
 * 
 * Read discovery token on links which are not part of any connection.
 * This interface allows a user mode module to read discovery token from all the
 * links of the specified device. To discover internode connections, a module
 * will first initiate a write discovery token operation, then read back tokens
 * from all other nodes and check for matching tokens.
 *
 * Parameters:
 *   devInfo [OUT]
 *      nvlink device identification information.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   numTokens [OUT]
 *      Number of nvlink tokens read.
 *
 *   tokenInfo [OUT]
 *      link index, and value of each token read.
 */
typedef struct
{
    /* input parameters */
    nvlink_device_info  devInfo;

    /* output parameters */
    NvlStatus           status;
    NvU32               numTokens;
    nvlink_token_info   tokenInfo[NVLINK_MAX_DEVICE_CONN];
} nvlink_device_read_discovery_tokens;

/*
 * CTRL_NVLINK_TRAIN_INTRANODE_CONN
 * 
 * Train an intranode connection.
 * This interface allows a user mode module to train an nvlink connection
 * to the desired state.
 *
 * Note:
 *   1) Source endpoint is treated as active endpoint.
 *
 * Parameters:
 *   trainTo [IN]
 *      Desired connection state as defined in nvlink_conn_train_type.
 *
 *   srcEndPoint [IN]
 *      Specify source endpoint details of the nvlink connection.
 *
 *   dstEndPoint [IN]
 *      Specify other endpoint details of the nvlink connection.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   srcEndState [OUT]
 *      Link and sublink state of the source endpoint.
 *
 *   dstEndState [OUT]
 *      Link and sublink state of the other endpoint.
 */
typedef enum
{
    nvlink_train_conn_off_to_swcfg = 0,
    nvlink_train_conn_swcfg_to_active,
    nvlink_train_conn_to_off,
    nvlink_train_conn_active_to_swcfg,
    nvlink_train_conn_swcfg_to_off,
    nvlink_train_conn_off_to_active_ali_non_blocking,
    nvlink_train_conn_off_to_active_ali_blocking,
    /* See enum modification guidelines at the top of this file */
} nvlink_conn_train_type;

typedef struct
{
    /* input parameters */
    NvU32                  trainTo;
    nvlink_endpoint        srcEndPoint;
    nvlink_endpoint        dstEndPoint;

    /* output parameters */
    NvlStatus              status;
    nvlink_link_state      srcEndState;
    nvlink_link_state      dstEndState;
} nvlink_train_intranode_conn;

/*
 * CTRL_NVLINK_TRAIN_INTERNODE_CONN_LINK
 * 
 * Train link of an internode connection.
 * This interface allows a user mode module to train the local endpoint 
 * link of an nvlink internode connection to the desired state. This is 
 * applicable only for multi-node systems where only one endpoint of the
 * connection is visible/accessible from a given node.
 *
 * Parameters:
 *   trainTo [IN]
 *      Desired link state as defined in nvlink_link_train_type.
 *
 *   isMasterEnd [IN]
 *      True if the endpoint is the master endpoint of the connection.
 *
 *   endPoint [IN]
 *      Specify endpoint details of the nvlink connection.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   endState [OUT]
 *      Link and sublink state of the endpoint.
 */
typedef enum
{
    nvlink_train_link_off_to_swcfg = 0,
    nvlink_train_link_swcfg_to_active,
    nvlink_train_link_to_off,
    nvlink_train_link_active_to_swcfg,
    nvlink_train_link_swcfg_to_off,
} nvlink_link_train_type;

typedef struct
{
    /* input parameters */
    NvU32                  trainTo;
    NvU32                  isMasterEnd;
    nvlink_endpoint        localEndPoint;

    /* output parameters */
    NvlStatus              status;
    nvlink_link_state      localEndState;
} nvlink_train_internode_conn_link;

/*
 * CTRL_NVLINK_TRAIN_INTERNODE_CONN_SUBLINK
 * 
 * Train sublink of an internode connection.
 * This interface allows a user mode module to train the local endpoint 
 * sublink of an nvlink internode connection to the desired state. This is 
 * applicable only for multi-node systems where only one endpoint of the
 * connection is visible/accessible from a given node.
 *
 * Parameters:
 *   trainTo [IN]
 *       Desired sublink state as defined in nvlink_sublink_train_type.
 *
 *   isMasterEnd [IN]
 *      True if the endpoint is master endpoint of the connection.
 *
 *   endPoint [IN]
 *      Specify endpoint details of the nvlink connection.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   endState [OUT]
 *      Link and sublink state of the endpoint.
 */
typedef enum 
{
    nvlink_train_sublink_off_to_safe = 0,
    nvlink_train_sublink_safe_to_hs,
    nvlink_train_sublink_to_off,
    nvlink_train_sublink_hs_to_safe,
    nvlink_train_sublink_safe_to_off,
} nvlink_sublink_train_type;

typedef struct
{
    /* input parameters */
    nvlink_sublink_train_type trainTo;
    NvU32                     isMasterEnd;
    nvlink_endpoint           localEndPoint;

    /* output parameters */
    NvlStatus                 status;
    nvlink_link_state         localEndState;
} nvlink_train_internode_conn_sublink;

/*
 * CTRL_NVLINK_GET_DEVICES_INFO
 * 
 * Return registered device information.
 * This interface allows a user mode module to query and retrieve detailed 
 * information about all the registered devices in the nvlink core. This 
 * device information can be later used to uniquely identify each device
 * present in the node.
 *
 * Parameters:
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   numDevice [OUT]
 *      Total number of devices registered.
 *
 *   devInfo [OUT]
 *      Detailed information of each device.
 */
typedef struct
{
    /* input parameters */

    /* output parameters */
    NvlStatus                status;
    NvU32                    numDevice;
    nvlink_detailed_dev_info devInfo[NVLINK_DEVICE_INSTANCE_MAX];
} nvlink_get_devices_info;

/*
 * CTRL_NVLINK_INITPHASE1
 * 
 * This interface allows a user mode module to send INITPHASE request to minion.
 * NOTE: This IOCTRL is supported from GA100+
 *
 * Parameters:
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */

    /* output parameters */
    NvlStatus status;
} nvlink_initphase1;

/*
 * CTRL_NVLINK_INITNEGOTIATE
 * 
 * This interface allows a user mode module to send INITNEGOTIATE request to minion.
 * NOTE: This IOCTRL is supported from GA100+
 * 
 * Parameters:
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */

    /* output parameters */
    NvlStatus status;
} nvlink_initnegotiate;

/*
 * CTRL_NVLINK_RX_INIT_TERM
 * 
 * This interface allows a user mode module to send RECEIVER TERMINATION on the endpoint.
 * NOTE: This IOCTRL is supported from GA100+
 * 
 * Parameters:
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */

    /* output parameters */
    NvlStatus status;
} nvlink_rx_init_term;

/*
 * CTRL_NVLINK_DEVICE_READ_SIDS
 * 
 * NVLink 3.0 onwards, connection detection is handled by Minion. After INITNEGOTIATE
 * completed, this interface needs to be queried to retrieve the local/remote SIDs
 * and the local/remote link number of all links associated with a device.
 *
 * On NVLink 4.0 this needs to be queried after all links in the system have been 
 * trained.  
 *
 * Parameters:
 *   devInfo [IN]
 *      nvlink device identification information.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   numEntries [OUT]
 *      Number of links for which SIDs were read
 *
 *   sidInfo [OUT]
 *      local/remote link number and local/remote sid
 */
typedef struct
{
    NvU32 localLinkNum;
    NvU32 remoteLinkNum;

    /* Added as part of NvLink 3.0 */
    NV_DECLARE_ALIGNED(NvU64 localLinkSid,  8);
    NV_DECLARE_ALIGNED(NvU64 remoteLinkSid, 8);
} nvlink_sid_info;

typedef struct
{
    /* input parameters */
    nvlink_device_info  devInfo;

    /* output parameters */
    NvlStatus           status;
    NvU32               numEntries;
    nvlink_sid_info     sidInfo[NVLINK_MAX_DEVICE_CONN];
} nvlink_device_read_sids;

/*
 * CTRL_NVLINK_SET_RX_DETECT
 * 
 * This interface allows a user mode module to send RECEIVER DETECT on the endpoint.
 * NOTE: This IOCTRL is supported from GA100+
 *
 * Parameters:
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */

    /* output parameters */
    NvlStatus status;
} nvlink_set_rx_detect;

/*
 * CTRL_NVLINK_GET_RX_DETECT
 * 
 * This interface allows a user mode module to Poll for output of receiver 
 * detect on all the endpoints.
 * NOTE: This IOCTRL is supported from GA100+
 * 
 * Parameters:
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */

    /* output parameters */
    NvlStatus status;
} nvlink_get_rx_detect;

/*
 * CTRL_NVLINK_ACQUIRE_CAPABILITY
 *
 * Upon success, user mode would acquire the requested capability
 * to perform privilege operations. This IOCTL will acquire one
 * capability at a time.
 *
 * Parameters:
 *   capDescriptor [IN]
 *      The OS file descriptor or handle representing the capability.
 *   cap [IN]
 *      The requested capability. One of the NVLINK_CAP_*.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */
    NV_DECLARE_ALIGNED(NvU64 capDescriptor, 8);
    NvU32 cap;

    /* output parameters */
    NvlStatus status;
} nvlink_acquire_capability;

/*
 * CTRL_NVLINK_GET_LINK_STATE
 *
 * Returns link state. This is needed for trunk links 
 * which has post HS steps to get links to active.
 *
 * Parameters:
 *   endPointPairs [IN]
 *      Specify the endpoints on which the command is to be sent 
 *
 *   endPointPairsCount [IN]
 *      Specify count of endpoints passed in
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   endpointPairsStates [OUT]
 *      Link and sublink state of the endpoint
 */
typedef struct
{
    /* input parameters */
    nvlink_endpoint        endPoints[NVLINK_MAX_NVLINK_ENDPOINTS];
    NvU32                  endPointCount;

    /* output parameters */
    NvlStatus              status;
    nvlink_link_state      endState[NVLINK_MAX_NVLINK_ENDPOINTS];
} nvlink_get_link_state;

/*
 * CTRL_NVLINK_TRAIN_INTRANODE_CONNS_PARALLEL
 * 
 * Train a set of intranode connections in parallel.
 * This interface allows a user mode module to train a set of nvlink
 * connections to the desired state.
 *
 * Note:
 *   1) Source endpoint of every pair is treated as active endpoint.
 *
 * Parameters:
 *   trainTo [IN]
 *      Desired connection state as defined in nvlink_conn_train_type.
 *
 *   endPointPairs [IN]
 *      Specify endpoint pair (source and other endpoint) details of the 
 *      nvlink connections 
 *
 *   endPointPairsCount [IN]
 *      Specify count of intranode connnection passed in
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   endpointPairsStates [OUT]
 *      Link and sublink state of the endpoint pairs
 */

typedef struct
{
    nvlink_endpoint        src;
    nvlink_endpoint        dst;
} nvlink_endpoint_pair;

typedef struct
{
    nvlink_link_state      srcEnd;
    nvlink_link_state      dstEnd;
} nvlink_link_state_pair;

typedef struct
{
    /* input parameters */
    NvU32                  trainTo;
    nvlink_endpoint_pair   endPointPairs[NVLINK_MAX_PARALLEL_CONNS_TRAIN_COUNT];
    NvU32                  endPointPairsCount;

    /* output parameters */
    NvlStatus              status;
    nvlink_link_state_pair endpointPairsStates[NVLINK_MAX_PARALLEL_CONNS_TRAIN_COUNT];
} nvlink_train_intranode_conns_parallel;

/*
 * CTRL_NVLINK_TRAIN_INTERNODE_LINKS_INITOPTIMIZE
 * 
 * This interface allows a user mode module to send INITOPTIMIZE request to minion
 *   and poll on the training good status which implies sublinks are trained
 * NOTE: This IOCTRL is supported from GA100+
 * 
 * Parameters:
 *   endPoints [IN]
 *      Specify the endpoints on which INITOPTIMIZE is to be sent
 *
 *   endPointCount [IN]
 *      Specify count of endpoints passed in
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */
    nvlink_endpoint        endPoints[NVLINK_MAX_NVLINK_ENDPOINTS];
    NvU32                  endPointCount;

    /* output parameters */
    NvlStatus              status;
} nvlink_train_internode_links_initoptimize;

/*
 * CTRL_NVLINK_TRAIN_INTERNODE_LINKS_POST_INITOPTIMIZE
 * 
 * This interface allows a user mode module to send POST_INITOPTIMIZE request to minion
 *
 * NOTE: This IOCTRL is supported from GA100+
 * 
 * Parameters:
 *   endPoints [IN]
 *      Specify the endpoints on which POST_INITOPTIMIZE is to be sent
 *
 *   endPointCount [IN]
 *      Specify count of endpoints passed in
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */
    nvlink_endpoint        endPoints[NVLINK_MAX_NVLINK_ENDPOINTS];
    NvU32                  endPointCount;

    /* output parameters */
    NvlStatus              status;
} nvlink_train_internode_links_post_initoptimize;

/*
 * CTRL_NVLINK_TRAIN_INTERNODE_CONNS_PARALLEL
 * 
 * Train link of an internode connection.
 * This interface allows a user mode module to train the local endpoint 
 * link of an nvlink internode connection to the desired state. This is 
 * applicable only for multi-node systems where only one endpoint of the
 * connection is visible/accessible from a given node.
 *
 * Parameters:
 *   trainTo [IN]
 *      Desired link state as defined in nvlink_link_train_type.
 *
 *   isMasterEnd [IN]
 *      True if the endpoint is the master endpoint of the connection.
 *
 *   endPoints [IN]
 *      Specify endpoint details of the nvlink connections.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   endStates [OUT]
 *      Link and sublink state of the endpoints.
 */

typedef struct
{
    /* input parameters */
    NvU32                  trainTo;
    NvU32                  isMasterEnd[NVLINK_MAX_PARALLEL_CONNS_TRAIN_COUNT];
    nvlink_endpoint        localEndPoints[NVLINK_MAX_PARALLEL_CONNS_TRAIN_COUNT];
    NvU32                  localEndPointCount;

    /* output parameters */
    NvlStatus              status;
    nvlink_link_state      localEndStates[NVLINK_MAX_PARALLEL_CONNS_TRAIN_COUNT];
} nvlink_train_internode_conns_parallel;

/*
 * CTRL_NVLINK_INITPHASE5
 *
 * This interface allows a user mode module to send INITPHASE request to minion.
 * NOTE: This IOCTRL is supported from GA100+
 *
 * Parameters:
 *   status [OUT]
 *      One of the NVL_XXX status value.
 */
typedef struct
{
    /* input parameters */

    /* output parameters */
    NvlStatus status;
} nvlink_initphase5;

/*
 * CTRL_NVLINK_GET_DEVICE_LINK_STATES
 *
 * Returns the link state of all links on a given device.
 *
 * Parameters:
 *   devInfo [IN]
 *      nvlink device identification information.
 *
 *   status [OUT]
 *      One of the NVL_XXX status value.
 *
 *   endStates [OUT]
 *      Link and sublink state of links. The array
 *      is continuous (i.e. it will have all links of the device
 *      even if the link is disabled)
 *      For links that are not enabled, the index in the array
 *      will show the states as INVALID.
 *
 *   endStatesCount [OUT]
 *      count of total entries in the endStates array
 *
 */
typedef struct
{
    /* input parameters */
    nvlink_device_info       devInfo;


    /* output parameters */
    NvlStatus              status;
    nvlink_link_state      endStates[NVLINK_MAX_DEVICE_CONN];
    NvU32                  endStatesCount;
    NvU64                  time;
} nvlink_get_device_link_states;

/* 
 * Note: Verify that new parameter structs for IOCTLs satisfy 
 *       sizing restrictions for all OSs they could be used in.
 */ 

#define CTRL_NVLINK_CHECK_VERSION                            0x01
#define CTRL_NVLINK_SET_NODE_ID                              0x02
#define CTRL_NVLINK_SET_TX_COMMON_MODE                       0x03
#define CTRL_NVLINK_CALIBRATE                                0x04
#define CTRL_NVLINK_ENABLE_DATA                              0x05
#define CTRL_NVLINK_LINK_INIT_ASYNC                          0x06
#define CTRL_NVLINK_DEVICE_LINK_INIT_STATUS                  0x07
#define CTRL_NVLINK_DISCOVER_INTRANODE_CONNS                 0x08
#define CTRL_NVLINK_DEVICE_GET_INTRANODE_CONNS               0x09
#define CTRL_NVLINK_ADD_INTERNODE_CONN                       0x0A
#define CTRL_NVLINK_REMOVE_INTERNODE_CONN                    0x0B
#define CTRL_NVLINK_DEVICE_WRITE_DISCOVERY_TOKENS            0x0C
#define CTRL_NVLINK_DEVICE_READ_DISCOVERY_TOKENS             0x0D
#define CTRL_NVLINK_TRAIN_INTRANODE_CONN                     0x0E
#define CTRL_NVLINK_TRAIN_INTERNODE_CONN_LINK                0x0F
#define CTRL_NVLINK_TRAIN_INTERNODE_CONN_SUBLINK             0x10
#define CTRL_NVLINK_GET_DEVICES_INFO                         0x11
#define CTRL_NVLINK_INITPHASE1                               0x12
#define CTRL_NVLINK_INITNEGOTIATE                            0x13
#define CTRL_NVLINK_RX_INIT_TERM                             0x14
#define CTRL_NVLINK_SET_RX_DETECT                            0x15
#define CTRL_NVLINK_GET_RX_DETECT                            0x16
#define CTRL_NVLINK_ACQUIRE_CAPABILITY                       0x17
#define CTRL_NVLINK_TRAIN_INTRANODE_CONNS_PARALLEL           0x18
#define CTRL_NVLINK_DEVICE_READ_SIDS                         0x19
#define CTRL_NVLINK_TRAIN_INTERNODE_LINKS_INITOPTIMIZE       0x1A
#define CTRL_NVLINK_TRAIN_INTERNODE_LINKS_POST_INITOPTIMIZE  0x1B
#define CTRL_NVLINK_TRAIN_INTERNODE_CONNS_PARALLEL           0x1C
#define CTRL_NVLINK_INITPHASE5                               0x1D
#define CTRL_NVLINK_GET_DEVICE_LINK_STATES                   0x1E
#define CTRL_NVLINK_GET_LINK_STATE                           0x1F
#define CTRL_NVLINK_RESERVED_0                               0x20
#define CTRL_NVLINK_RESERVED_1                               0x21
#define CTRL_NVLINK_RESERVED_2                               0x22
#define CTRL_NVLINK_RESERVED_3                               0x23
#define CTRL_NVLINK_RESERVED_4                               0x24
#define CTRL_NVLINK_RESERVED_5                               0x25
#define CTRL_NVLINK_RESERVED_6                               0x26
#define CTRL_NVLINK_RESERVED_7                               0x27
#define CTRL_NVLINK_RESERVED_8                               0x28
#define CTRL_NVLINK_RESERVED_9                               0x29
#define CTRL_NVLINK_RESERVED_10                              0x2A
#define CTRL_NVLINK_RESERVED_11                              0x2B
/* Do not add code after this line */

#endif // _NVLINK_LIB_CTRL_H_
