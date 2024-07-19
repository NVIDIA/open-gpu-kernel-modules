/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvlink.h"
#include "nvVer.h"
#include "nvlink_os.h"
#include "nvlink_lib_ctrl.h"
#include "../nvlink_ctx.h"
#include "../nvlink_helper.h"
#include "nvlink_lock.h"
#include "nvctassert.h"

#define NVLINK_IOC_GET_BUF(ctrlParams, type) (ctrlParams)->size >= sizeof(type) ? (type *) (ctrlParams)->buf : NULL

/**
 * List of static functions
 */
static NvlStatus nvlink_lib_ioctl_ctrl_helper(nvlink_ioctrl_params *);
static NvlStatus nvlink_lib_ctrl_prologue(nvlink_ioctrl_params *);
static NvlStatus nvlink_lib_ctrl_check_version(nvlink_check_version *);
static NvlStatus nvlink_lib_ctrl_set_node_id(nvlink_set_node_id *);
static NvlStatus nvlink_lib_ctrl_all_links(nvlink_ioctrl_params *);
static NvlStatus nvlink_lib_ctrl_device_link_init_status(nvlink_device_link_init_status *);
static NvlStatus nvlink_lib_ctrl_device_write_discovery_tokens(nvlink_device_write_discovery_tokens *);
static NvlStatus nvlink_lib_ctrl_device_read_discovery_tokens(nvlink_device_read_discovery_tokens *);
static NvlStatus nvlink_lib_ctrl_device_read_sids(nvlink_device_read_sids *);
static NvlStatus nvlink_lib_ctrl_discover_intranode_conns(nvlink_discover_intranode_conns *);
static NvlStatus nvlink_lib_ctrl_device_get_intranode_conns(nvlink_device_get_intranode_conns *);
static NvlStatus nvlink_lib_ctrl_add_internode_conn(nvlink_add_internode_conn *);
static NvlStatus nvlink_lib_ctrl_remove_internode_conn(nvlink_remove_internode_conn *);
static NvlStatus nvlink_lib_ctrl_train_intranode_conn(nvlink_train_intranode_conn *);
static NvlStatus nvlink_lib_ctrl_train_intranode_conns_parallel(nvlink_train_intranode_conns_parallel *);
static NvlStatus nvlink_lib_ctrl_train_internode_conn_link(nvlink_train_internode_conn_link *);
static NvlStatus nvlink_lib_ctrl_train_internode_conn_sublink(nvlink_train_internode_conn_sublink *);
static NvlStatus nvlink_lib_ctrl_train_internode_links_initoptimize(nvlink_train_internode_links_initoptimize *);
static NvlStatus nvlink_lib_ctrl_train_internode_links_post_initoptimize(nvlink_train_internode_links_post_initoptimize *);
static NvlStatus nvlink_lib_ctrl_train_internode_conns_parallel(nvlink_train_internode_conns_parallel *);
static NvlStatus nvlink_lib_ctrl_get_devices_info(nvlink_get_devices_info *);
static NvlStatus nvlink_lib_ctrl_acquire_capability(nvlink_ioctrl_params *, nvlink_acquire_capability *);
static NvlStatus nvlink_lib_ctrl_get_link_state(nvlink_get_link_state *);
static NvlStatus nvlink_lib_ctrl_get_device_link_states(nvlink_get_device_link_states *);

/**
 * Entry point for IOCTLs into the NVLink core library
 *
 * @param[in]  ctrlParams  IOCTL params
 *
 * return NvlStatus
 */
NvlStatus
nvlink_lib_ioctl_ctrl
(
    nvlink_ioctrl_params *ctrlParams
)
{
    NvlStatus status = NVL_SUCCESS;

    status = nvlink_lib_ioctl_ctrl_helper(ctrlParams);

    return status;
}

/**
 * Helper function for routing the IOCTL to the respective handlers
 *
 * Note: The handlers acquire the required core library locks before
 *       calling the core library code
 *
 * @param[in]  ctrlParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ioctl_ctrl_helper
(
    nvlink_ioctrl_params *ctrlParams
)
{
    NvlStatus status;

    status = nvlink_lib_ctrl_prologue(ctrlParams);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    switch (ctrlParams->cmd)
    {
        case CTRL_NVLINK_CHECK_VERSION:
        {
            nvlink_check_version *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_check_version);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_check_version(iocReq);
            break;
        }

        case CTRL_NVLINK_SET_NODE_ID:
        {
            nvlink_set_node_id *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_set_node_id);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_set_node_id(iocReq);
            break;
        }

        //
        // The following commands operate on all the links registered in the
        // core library. Hence, clubbing them into a group so, we don't have
        // to duplicate the lock acquire/release for each of them
        //
        case CTRL_NVLINK_INITPHASE1:
        case CTRL_NVLINK_RX_INIT_TERM:
        case CTRL_NVLINK_SET_RX_DETECT:
        case CTRL_NVLINK_GET_RX_DETECT:
        case CTRL_NVLINK_SET_TX_COMMON_MODE:
        case CTRL_NVLINK_CALIBRATE:
        case CTRL_NVLINK_ENABLE_DATA:
        case CTRL_NVLINK_LINK_INIT_ASYNC:
        case CTRL_NVLINK_INITNEGOTIATE:
        case CTRL_NVLINK_INITPHASE5:
        {
            nvlink_lib_ctrl_all_links(ctrlParams);
            break;
        }

        case CTRL_NVLINK_DEVICE_LINK_INIT_STATUS:
        {
            nvlink_device_link_init_status *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_device_link_init_status);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_device_link_init_status(iocReq);
            break;
        }

        case CTRL_NVLINK_DEVICE_WRITE_DISCOVERY_TOKENS:
        {
            nvlink_device_write_discovery_tokens *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_device_write_discovery_tokens);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_device_write_discovery_tokens(iocReq);
            break;
        }

        case CTRL_NVLINK_DEVICE_READ_DISCOVERY_TOKENS:
        {
            nvlink_device_read_discovery_tokens *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_device_read_discovery_tokens);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_device_read_discovery_tokens(iocReq);
            break;
        }

        case CTRL_NVLINK_DEVICE_READ_SIDS:
        {
            nvlink_device_read_sids *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_device_read_sids);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_device_read_sids(iocReq);
            break;
        }

        case CTRL_NVLINK_DISCOVER_INTRANODE_CONNS:
        {
            nvlink_discover_intranode_conns *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_discover_intranode_conns);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_discover_intranode_conns(iocReq);
            break;
        }

        case CTRL_NVLINK_DEVICE_GET_INTRANODE_CONNS:
        {
            nvlink_device_get_intranode_conns *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_device_get_intranode_conns);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_device_get_intranode_conns(iocReq);
            break;
        }

        case CTRL_NVLINK_ADD_INTERNODE_CONN:
        {
            nvlink_add_internode_conn *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_add_internode_conn);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_add_internode_conn(iocReq);
            break;
        }

        case CTRL_NVLINK_REMOVE_INTERNODE_CONN:
        {
            nvlink_remove_internode_conn *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_remove_internode_conn);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_remove_internode_conn(iocReq);
            break;
        }

        case CTRL_NVLINK_TRAIN_INTRANODE_CONN:
        {
            nvlink_train_intranode_conn *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_train_intranode_conn);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_train_intranode_conn(iocReq);
            break;
        }

        case CTRL_NVLINK_TRAIN_INTRANODE_CONNS_PARALLEL:
        {
            nvlink_train_intranode_conns_parallel *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_train_intranode_conns_parallel);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_train_intranode_conns_parallel(iocReq);
            break;
        }

        case CTRL_NVLINK_TRAIN_INTERNODE_CONN_LINK:
        {
            nvlink_train_internode_conn_link *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_train_internode_conn_link);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_train_internode_conn_link(iocReq);
            break;
        }

        case CTRL_NVLINK_TRAIN_INTERNODE_CONN_SUBLINK:
        {
            nvlink_train_internode_conn_sublink *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_train_internode_conn_sublink);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_train_internode_conn_sublink(iocReq);
            break;
        }

        case CTRL_NVLINK_TRAIN_INTERNODE_LINKS_INITOPTIMIZE:
        {
            nvlink_train_internode_links_initoptimize *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_train_internode_links_initoptimize);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }
            iocReq->status = nvlink_lib_ctrl_train_internode_links_initoptimize(iocReq);
            break;
        }

        case CTRL_NVLINK_TRAIN_INTERNODE_LINKS_POST_INITOPTIMIZE:
        {
            nvlink_train_internode_links_post_initoptimize *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_train_internode_links_post_initoptimize);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }
            iocReq->status = nvlink_lib_ctrl_train_internode_links_post_initoptimize(iocReq);
            break;
        }

        case CTRL_NVLINK_TRAIN_INTERNODE_CONNS_PARALLEL:
        {
            nvlink_train_internode_conns_parallel *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_train_internode_conns_parallel);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_train_internode_conns_parallel(iocReq);
            break;
        }

        case CTRL_NVLINK_GET_DEVICES_INFO:
        {
            nvlink_get_devices_info *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_get_devices_info);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_get_devices_info(iocReq);
            break;
        }

        case CTRL_NVLINK_ACQUIRE_CAPABILITY:
        {
            nvlink_acquire_capability *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_acquire_capability);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_acquire_capability(ctrlParams, iocReq);
            break;
        }

        case CTRL_NVLINK_GET_LINK_STATE:
        {
            nvlink_get_link_state *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_get_link_state);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_get_link_state(iocReq);
            break;
        }
        case CTRL_NVLINK_GET_DEVICE_LINK_STATES:
        {
            nvlink_get_device_link_states *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_get_device_link_states);
            if (!iocReq)
            {
                return NVL_BAD_ARGS;
            }

            iocReq->status = nvlink_lib_ctrl_get_device_link_states(iocReq);
            break;
        }

        case CTRL_NVLINK_RESERVED_0:
        case CTRL_NVLINK_RESERVED_1:
        case CTRL_NVLINK_RESERVED_2:
        case CTRL_NVLINK_RESERVED_3:
        case CTRL_NVLINK_RESERVED_4:
        case CTRL_NVLINK_RESERVED_5:
        case CTRL_NVLINK_RESERVED_6:
        case CTRL_NVLINK_RESERVED_7:
        case CTRL_NVLINK_RESERVED_8:
        case CTRL_NVLINK_RESERVED_9:
        case CTRL_NVLINK_RESERVED_10:
        case CTRL_NVLINK_RESERVED_11:
        {
            return NVL_SUCCESS;
            break;
        }

        default:
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: unknown ioctl command 0x%08X specified.\n",
                __FUNCTION__, ctrlParams->cmd));
            return NVL_BAD_ARGS;
        }
    }

    //
    // the IOCTL call is success. However, status of the individual IOCTL is
    // indicated in their corresponding embedded status field.
    //
    return NVL_SUCCESS;
}

/**
 * Preliminary check before passing the IOCTL to the respective handler
 *
 * @param[in]  ctrlParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus 
nvlink_lib_ctrl_prologue
(
    nvlink_ioctrl_params *ctrlParams
)
{
    NvlStatus status = NVL_SUCCESS;

    if (ctrlParams == NULL)
    {
        return NVL_BAD_ARGS;
    }

    switch (ctrlParams->cmd)
    {
        //
        // These control calls are aren't privileged. So, skip the capability
        // check.
        //
        case CTRL_NVLINK_CHECK_VERSION:
        case CTRL_NVLINK_ACQUIRE_CAPABILITY:
        {
            break;
        }
        default:
        {
            if (!nvlink_is_admin() &&
                !nvlink_is_fabric_manager(ctrlParams->osPrivate))
            {
                status = NVL_ERR_INSUFFICIENT_PERMISSIONS;
            }
            break;
        }
    }

    return status;
}

/**
 * Check if the user and kernel versions mismatch
 *
 * @param[in]  versionParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_check_version
(
    nvlink_check_version *versionParams
)
{
    const NvU32 NV_VERSION_LENGTH = nvlink_strlen(NV_VERSION_STRING);

    if (NV_VERSION_LENGTH > NVLINK_VERSION_STRING_LENGTH)
    {
        return NVL_NO_MEM;
    }

    versionParams->user.version[NVLINK_VERSION_STRING_LENGTH - 1] = '\0';

    nvlink_memset(versionParams->kernel.version, 0x0, sizeof(versionParams->kernel.version));
    nvlink_strcpy(versionParams->kernel.version, NV_VERSION_STRING);

    versionParams->kernel.version[NVLINK_VERSION_STRING_LENGTH - 1] = '\0';

    if (nvlink_strcmp(versionParams->user.version, versionParams->kernel.version))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: version mismatch, kernel version %s user version %s\n",
            __FUNCTION__,
            versionParams->kernel.version, versionParams->user.version));

        return NVL_ERR_NOT_SUPPORTED;
    }

    return NVL_SUCCESS;
}

/**
 * Assign node ID to all the registered devices
 *
 * @param[in]  idParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_set_node_id
(
    nvlink_set_node_id *idParams
)
{
    NvlStatus      status = NVL_SUCCESS;
    nvlink_device *dev    = NULL;

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return status;
    }

    // Top-level lock is now acquired

    // Return success, if an attempt is made to re-assign the same node-id.
    if (nvlinkLibCtx.nodeId == idParams->nodeId)
    {
        // Release the top-level lock
        nvlink_lib_top_lock_release();

        return  NVL_SUCCESS;
    }

    if (nvlinkLibCtx.nodeId != NV_U16_MAX)
    {
        // Don't allow to change fabric node id once it is set.
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Can't change fabric node id once it is set. "
            "Current node id is %u\n",
            __FUNCTION__, nvlinkLibCtx.nodeId));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        return  NVL_ERR_INVALID_STATE;
    }

    // Change already registered device's fabric node id.
    FOR_EACH_DEVICE_REGISTERED(dev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        dev->nodeId = idParams->nodeId;
    }

    // Store fabric node id for any future device registration.
    nvlinkLibCtx.nodeId = idParams->nodeId;

    // Release the top-level lock
    nvlink_lib_top_lock_release();

    return NVL_SUCCESS;
}

/**
 * Kick off the desired operation on registered links of all devices
 *
 * Note: This operation will acquire the per-link locks of all the
 *       registered links of all devices in the core library
 *
 * @param[in]  ctrlParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_all_links
(
    nvlink_ioctrl_params *ctrlParams
)
{
    NvlStatus      status   = NVL_SUCCESS;
    nvlink_device *dev      = NULL;
    nvlink_link   *link     = NULL;
    NvU32          numLinks = 0;

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
    {
        return NVL_NO_MEM;
    }
    
    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        nvlink_free((void *)links);
        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists
    //

    FOR_EACH_DEVICE_REGISTERED(dev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        FOR_EACH_LINK_REGISTERED(link, dev, node)
        {
            if (numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                    __FUNCTION__));

                nvlink_assert(0);

                // Release the top-level lock and free links
                nvlink_lib_top_lock_release();
                nvlink_free((void *)links);
                return NVL_ERR_INVALID_STATE;
            }
            links[numLinks] = link;
            numLinks++;
        }
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();
        nvlink_free((void *)links);
        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    nvlink_assert((links != NULL) && (numLinks > 0));

    // Kick off the desired operation on all the registered links
    switch (ctrlParams->cmd)
    {
        case CTRL_NVLINK_INITPHASE1:
        {
            nvlink_initphase1 *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_initphase1);
            if (!iocReq)
            {
                status = NVL_BAD_ARGS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            // default initialize status to NVL_SUCCESS
            iocReq->status = NVL_SUCCESS;

            if (links[0]->dev->enableALI)
            {
                status = NVL_SUCCESS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            iocReq->status = nvlink_core_initphase1(links, numLinks,
                                                    NVLINK_STATE_CHANGE_SYNC);
            break;
        }

        case CTRL_NVLINK_RX_INIT_TERM:
        {
            nvlink_rx_init_term *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_rx_init_term);
            if (!iocReq)
            {
                status = NVL_BAD_ARGS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            // default initialize status to NVL_SUCCESS
            iocReq->status = NVL_SUCCESS;

            //
            // If the current nvlink device does not support the command
            // skip using the command and return success for FM to continue on.
            //
            if (links[0]->version >= NVLINK_DEVICE_VERSION_40)
            {
                status = NVL_SUCCESS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            iocReq->status = nvlink_core_rx_init_term(links, numLinks,
                                                      NVLINK_STATE_CHANGE_ASYNC);
            break;
        }

        case CTRL_NVLINK_SET_RX_DETECT:
        {
            nvlink_set_rx_detect *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_set_rx_detect);
            if (!iocReq)
            {
                status = NVL_BAD_ARGS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            // default initialize status to NVL_SUCCESS
            iocReq->status = NVL_SUCCESS;

            if (links[0]->dev->enableALI)
            {
                status = NVL_SUCCESS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            iocReq->status = nvlink_core_set_rx_detect(links, numLinks,
                                                      NVLINK_STATE_CHANGE_ASYNC);
            break;
        }

        case CTRL_NVLINK_GET_RX_DETECT:
        {
            nvlink_get_rx_detect *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_get_rx_detect);
            if (!iocReq)
            {
                status = NVL_BAD_ARGS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            // default initialize status to NVL_SUCCESS
            iocReq->status = NVL_SUCCESS;

            if (links[0]->dev->enableALI)
            {
                status = NVL_SUCCESS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            iocReq->status = nvlink_core_get_rx_detect(links, numLinks,
                                                      NVLINK_STATE_CHANGE_ASYNC);
            break;
        }

        case CTRL_NVLINK_SET_TX_COMMON_MODE:
        {
            nvlink_set_tx_common_mode *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_set_tx_common_mode);
            if (!iocReq)
            {
                status = NVL_BAD_ARGS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            // default initialize status to NVL_SUCCESS
            iocReq->status = NVL_SUCCESS;

            if (links[0]->dev->enableALI)
            {
                status = NVL_SUCCESS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            if (iocReq->commMode)
            {
                iocReq->status = nvlink_core_enable_common_mode(links, numLinks,
                                                      NVLINK_STATE_CHANGE_SYNC);
            }
            else if(links[0]->version <= NVLINK_DEVICE_VERSION_30)
            {
                iocReq->status = nvlink_core_disable_common_mode(links, numLinks,
                                                      NVLINK_STATE_CHANGE_SYNC);
            }

            //
            // If the current nvlink device does not support disabling common mode
            // skip using the command and return success for FM to continue on.
            //
            break;
        }

        case CTRL_NVLINK_CALIBRATE:
        {
            nvlink_calibrate *iocReq;
            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_calibrate);

            if (!iocReq)
            {
                status = NVL_BAD_ARGS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            // default initialize status to NVL_SUCCESS
            iocReq->status = NVL_SUCCESS;

            //
            // If the current nvlink device does not support the command
            // skip using the command and return success for FM to continue on.
            //
            if (links[0]->version >= NVLINK_DEVICE_VERSION_40)
            {
                iocReq->status = NVL_SUCCESS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            iocReq->status = nvlink_core_calibrate_links(links, numLinks,
                                                      NVLINK_STATE_CHANGE_SYNC);
            break;
        }

        case CTRL_NVLINK_ENABLE_DATA:
        {
            nvlink_enable_data *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_enable_data);
            if (!iocReq)
            {
                goto nvlink_lib_ctrl_all_links_end;
            }

            // default initialize status to NVL_SUCCESS
            iocReq->status = NVL_SUCCESS;

            //
            // If the current nvlink device does not support the command
            // skip using the command and return success for FM to continue on.
            //
            if (links[0]->version >= NVLINK_DEVICE_VERSION_40)
            {
                status = NVL_SUCCESS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            iocReq->status = nvlink_core_enable_data(links, numLinks,
                                                      NVLINK_STATE_CHANGE_SYNC);
            break;
        }

        case  CTRL_NVLINK_LINK_INIT_ASYNC:
        {
            nvlink_link_init_async *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_link_init_async);
            if (!iocReq)
            {
                status = NVL_BAD_ARGS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            // default initialize status to NVL_SUCCESS
            iocReq->status = NVL_SUCCESS;

            iocReq->status = nvlink_core_link_init_async(links, numLinks);
            break;
        }

        case CTRL_NVLINK_INITNEGOTIATE:
        {
            nvlink_initnegotiate *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_initnegotiate);
            if (!iocReq)
            {
                status = NVL_BAD_ARGS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            // default initialize status to NVL_SUCCESS
            iocReq->status = NVL_SUCCESS;

            if (links[0]->dev->enableALI)
            {
                status = NVL_SUCCESS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            iocReq->status = nvlink_core_initnegotiate(links, numLinks,
                                                      NVLINK_STATE_CHANGE_ASYNC);
            break;
        }

        case CTRL_NVLINK_INITPHASE5:
        {
            nvlink_initphase5 *iocReq;

            iocReq = NVLINK_IOC_GET_BUF(ctrlParams, nvlink_initphase5);
            if (!iocReq)
            {
                status = NVL_BAD_ARGS;
                goto nvlink_lib_ctrl_all_links_end;
            }

            // default initialize status to NVL_SUCCESS
            iocReq->status = NVL_SUCCESS;

            //
            // If the current nvlink device does not support the command
            // skip using the command and return success for FM to continue on.
            //
            if (links[0]->version < NVLINK_DEVICE_VERSION_40 ||
                links[0]->dev->enableALI)
            {
                status = NVL_SUCCESS;
                goto nvlink_lib_ctrl_all_links_end;
            }
            iocReq->status = nvlink_core_initphase5(links, numLinks,
                                                      NVLINK_STATE_CHANGE_ASYNC);
            break;
        }

        default:
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: unknown ioctl command specified.\n",
                __FUNCTION__));

            status = NVL_BAD_ARGS;
            goto nvlink_lib_ctrl_all_links_end;
        }

    }

nvlink_lib_ctrl_all_links_end:

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }

    return status;
}

/**
 * Get the link init status on all queried links
 *
 * @param[in]  statusParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_device_link_init_status
(
    nvlink_device_link_init_status *statusParams
)
{
    NvlStatus      status   = NVL_SUCCESS;
    nvlink_device *dev      = NULL;
    nvlink_link   *link     = NULL;
    NvU32          numLinks = 0;
    NvU32          i        = 0;

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
    {
        return NVL_NO_MEM;
    }

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        nvlink_free((void *)links);
        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists
    //

    // look-up user requested nvlink device object
    nvlink_core_get_device_by_devinfo(&statusParams->devInfo, &dev);
    if (dev == NULL)
    {
        //
        // Couldn't find the device ptr in the core library. Release the 
        // top-level lock and return
        //
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return NVL_BAD_ARGS;
    }

    FOR_EACH_LINK_REGISTERED(link, dev, node)
    {
        if (numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                __FUNCTION__));

            nvlink_assert(0);

            nvlink_lib_top_lock_release();
            nvlink_free((void *)links);
            return NVL_ERR_INVALID_STATE;
        }
        links[numLinks] = link;
        numLinks++;
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    // Poll for links to reach SAFE/SWCFG and capture the status
    for (i = 0; i < numLinks; i++)
    {
        // status index should be within NVLINK_MAX_DEVICE_CONN
        if (i >= NVLINK_MAX_DEVICE_CONN)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: number of links for the device >= NVLINK_MAX_DEVICE_CONN",
                __FUNCTION__));

            nvlink_assert(0);

            nvlink_lib_link_locks_release(links, numLinks);
            nvlink_free((void *)links);
            return NVL_ERR_INVALID_STATE;
        }

        status = nvlink_core_wait_for_link_init(links[i]);

        // indicate link init state to user
        statusParams->linkStatus[i].linkIndex = links[i]->linkNumber;

        if (status == NVL_SUCCESS)
        {
            statusParams->linkStatus[i].initStatus = NV_TRUE;
        }
        else
        {
            statusParams->linkStatus[i].initStatus = NV_FALSE;
        }
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }
    return NVL_SUCCESS;
}

/**
 * Send discovery tokens on all the links for a given device
 *
 * @param[in]  writeParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_device_write_discovery_tokens
(
    nvlink_device_write_discovery_tokens *writeParams
)
{
    NvlStatus      status    = NVL_SUCCESS;
    nvlink_device *dev       = NULL;
    nvlink_link   *link      = NULL;
    NvU32          numLinks  = 0;
    NvU32          i         = 0;
    NvU32          numTokens = 0;

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
    {
        return NVL_NO_MEM;
    }

    // Initialize number of tokens written to 0
    writeParams->numTokens = 0;

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        nvlink_free((void *)links);
        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists
    //

    // look-up user requested nvlink device object
    nvlink_core_get_device_by_devinfo(&writeParams->devInfo, &dev);
    if (dev == NULL)
    {
        //
        // Couldn't find the device ptr in the core library. Release the 
        // top-level lock and return
        //
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return NVL_BAD_ARGS;
    }

    FOR_EACH_LINK_REGISTERED(link, dev, node)
    {
        nvlink_intranode_conn *conn = NULL;

        nvlink_core_get_intranode_conn(link, &conn);
        if (conn != NULL)
        {
            // skip token write if we already have a connection for the link
            continue;
        }

        if (numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                __FUNCTION__));

            nvlink_assert(0);

            // Release the top-level lock and free links
            nvlink_lib_top_lock_release();
            nvlink_free((void *)links);
            return NVL_ERR_INVALID_STATE;
        }

        links[numLinks] = link;
        numLinks++;
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    for (i = 0; i < numLinks; i++)
    {
        NvU64 writeToken = 0;

        writeToken = nvlink_core_get_link_discovery_token(links[i]);
        status     = nvlink_core_write_link_discovery_token(links[i], writeToken);

        if (status == NVL_SUCCESS)
        {
            //
            // wrote a token. copy the token and link information to user
            // which can be used for comparing tokens across nodes.
            //

            // total number of tokens should be within NVLINK_MAX_DEVICE_CONN
            if (numTokens >= NVLINK_MAX_DEVICE_CONN)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Number of tokens >= NVLINK_MAX_DEVICE_CONN\n",
                    __FUNCTION__));

                nvlink_assert(0);

                nvlink_lib_link_locks_release(links, numLinks);
                nvlink_free((void *)links);
                return NVL_ERR_INVALID_STATE;
            }

            writeParams->tokenInfo[numTokens].linkIndex  = links[i]->linkNumber;
            writeParams->tokenInfo[numTokens].tokenValue = writeToken;
            numTokens++;
        }
    }

    // update total number of tokens written
    writeParams->numTokens = numTokens;

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }
    return NVL_SUCCESS;
}

/**
 * Read discovery tokens on all the links for a given device
 *
 * @param[in]  readParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_device_read_discovery_tokens
(
    nvlink_device_read_discovery_tokens *readParams
)
{
    NvlStatus      status    = NVL_SUCCESS;
    nvlink_device *dev       = NULL;
    nvlink_link   *link      = NULL;
    NvU32          numLinks  = 0;
    NvU32          i         = 0;
    NvU32          numTokens = 0;

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
    {
        return NVL_NO_MEM;
    }

    // Initialize number of tokens read to 0
    readParams->numTokens = 0;

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        nvlink_free((void *)links);
        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists
    //

    // look-up user requested nvlink device object
    nvlink_core_get_device_by_devinfo(&readParams->devInfo, &dev);
    if (dev == NULL)
    {
        //
        // Couldn't find the device ptr in the core library. Release the 
        // top-level lock and return
        //
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return NVL_BAD_ARGS;
    }

    FOR_EACH_LINK_REGISTERED(link, dev, node)
    {
        nvlink_intranode_conn *conn = NULL;

        nvlink_core_get_intranode_conn(link, &conn);
        if (conn != NULL)
        {
            // skip token write if we already have a connection for the link
            continue;
        }

        if (numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                __FUNCTION__));

            nvlink_assert(0);

            // Release the top-level lock and free links
            nvlink_lib_top_lock_release();
            nvlink_free((void *)links);
            return NVL_ERR_INVALID_STATE;
        }

        links[numLinks] = link;
        numLinks++;
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    for (i = 0; i < numLinks; i++)
    {
        NvU64 readToken = 0;

        // query discovery token from the link
        readToken = nvlink_core_read_link_discovery_token(links[i]);

        // take non-zero tokens. token will be zero if read_discovery failed as well.
        if (readToken)
        {
            //
            // received a valid token. copy the token and link information to user
            // which can be used for comparing tokens across nodes.
            //

            // total number of tokens should be within NVLINK_MAX_DEVICE_CONN
            if (numTokens >= NVLINK_MAX_DEVICE_CONN)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Number of tokens >= NVLINK_MAX_DEVICE_CONN\n",
                    __FUNCTION__));

                nvlink_assert(0);

                nvlink_lib_link_locks_release(links, numLinks);
                nvlink_free((void *)links);
                return NVL_ERR_INVALID_STATE;
            }

            readParams->tokenInfo[numTokens].linkIndex  = links[i]->linkNumber;
            readParams->tokenInfo[numTokens].tokenValue = readToken;
            numTokens++;
        }
    }

    // update total number of tokens read
    readParams->numTokens = numTokens;

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }
    return NVL_SUCCESS;
}

/**
 * Perform peer link discovery
 *
 * @param[in]  readParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
_nvlink_lib_ctrl_device_discover_peer_link
(
    nvlink_link *link
)
{
    NvlStatus      status   = NVL_SUCCESS;

    //
    // If the link succeeds rxDet(link is in HS, SAFE, or SLEEP mode) then go through and find its
    // peer link. What is important is not actually finding the link, but making sure the corelib
    // goes through the discovery process and has endpoints cache the remote information in the corelib
    // such that FM or endpoints can query the corelib for the topology of the system.
    //
    NvU64 linkMode = NVLINK_LINKSTATE_OFF;
    status = link->link_handlers->get_dl_link_mode(link, &linkMode);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Unable to get link mode for %s:%s\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));
        return status;
    }

    if ((linkMode == NVLINK_LINKSTATE_SAFE) ||
        (linkMode == NVLINK_LINKSTATE_HS)   ||
        (linkMode == NVLINK_LINKSTATE_SLEEP))
    {
        nvlink_link   *remoteLink = NULL;
        nvlink_core_discover_and_get_remote_end(link, &remoteLink, 0, NV_FALSE);
        if (remoteLink == NULL)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: link 0x%x: couldn't find link pair! Possible that other device queries need to finish before there is a found connection in the corelib\n",
                __FUNCTION__, link->linkNumber));
        }
    }

    return NVL_SUCCESS;
}

/**
 * Read the SIDs for the the local and remote device
 *
 * @param[in]  readParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_device_read_sids
(
    nvlink_device_read_sids *readParams
)
{
    NvlStatus      status     = NVL_SUCCESS;
    nvlink_device *dev        = NULL;
    nvlink_link   *link       = NULL;
    NvU32          numLinks   = 0;
    NvU32          i          = 0;
    NvU32          numEntries = 0;

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
    {
        return NVL_NO_MEM;
    }

    // Initialize number of SIDs read to 0
    readParams->numEntries = 0;

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        nvlink_free((void *)links);
        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists
    //

    // look-up user requested nvlink device object
    nvlink_core_get_device_by_devinfo(&readParams->devInfo, &dev);
    if (dev == NULL)
    {
        //
        // Couldn't find the device ptr in the core library. Release the 
        // top-level lock and return
        //
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return NVL_BAD_ARGS;
    }

    FOR_EACH_LINK_REGISTERED(link, dev, node)
    {
        nvlink_intranode_conn *conn = NULL;

        nvlink_core_get_intranode_conn(link, &conn);
        if (conn != NULL)
        {
            // skip token write if we already have a connection for the link
            continue;
        }

        if (numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                __FUNCTION__));

            nvlink_assert(0);

            // Release the top-level lock and free links
            nvlink_lib_top_lock_release();
            nvlink_free((void *)links);
            return NVL_ERR_INVALID_STATE;
        }

        links[numLinks] = link;
        numLinks++;
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    for (i = 0; i < numLinks; i++)
    {
        // ALI specific handling to update corelib structures and verify link status
        if (dev->enableALI)
        {
            status = _nvlink_lib_ctrl_device_discover_peer_link(links[i]);
            if (status != NVL_SUCCESS)
            {
                // Release the per-link locks and free links
                nvlink_lib_link_locks_release(links, numLinks);
                nvlink_free((void *)links);
                return status;
            }
        }

        // Fill-up the local/remote link numbers and SIDs
        readParams->sidInfo[numEntries].localLinkSid  = links[i]->localSid;
        readParams->sidInfo[numEntries].remoteLinkSid = links[i]->remoteSid;
        readParams->sidInfo[numEntries].localLinkNum  = links[i]->linkNumber;
        readParams->sidInfo[numEntries].remoteLinkNum = links[i]->remoteLinkId;
        numEntries++;
    }

    // update total number of entries read
    readParams->numEntries = numEntries;

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }
    return NVL_SUCCESS;
}

/**
 * Discover all the intranode connections from the core library
 *
 * @param[in]  connParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_discover_intranode_conns
(
    nvlink_discover_intranode_conns *connParams
)
{
    NvlStatus      status   = NVL_SUCCESS;
    nvlink_device *dev      = NULL;
    nvlink_link   *link     = NULL;
    NvU32          numLinks = 0;

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
    {
        return NVL_NO_MEM;
    }

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        nvlink_free((void *)links);
        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists
    //

    FOR_EACH_DEVICE_REGISTERED(dev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        FOR_EACH_LINK_REGISTERED(link, dev, node)
        {
            if (numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                    __FUNCTION__));

                nvlink_assert(0);

                // Release the top-level lock and free links
                nvlink_lib_top_lock_release();
                nvlink_free((void *)links);
                return NVL_ERR_INVALID_STATE;
            }

            links[numLinks] = link;
            numLinks++;
        }
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Note: We will still need to hold the top-level lock, because we might have
    //       to add connections to the intranode connections list if any case new
    //       intranode connection is discovered
    //

    FOR_EACH_DEVICE_REGISTERED(dev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        FOR_EACH_LINK_REGISTERED(link, dev, node)
        {
            NvU64                  writeToken = 0;
            nvlink_intranode_conn *conn       = NULL;

            nvlink_core_get_intranode_conn(link, &conn);
            if (conn != NULL)
            {
                // skip token write if we already have a connection for the link
                continue;
            }

            if (!link->bRxDetected)
            {
                // If receiver detect has failed, then there is no connection
                continue;
            }

        // ALI specific handling to update corelib structures and verify link status
        if (dev->enableALI)
        {
            status = _nvlink_lib_ctrl_device_discover_peer_link(link);
            if (status != NVL_SUCCESS)
            {
                // Release the per-link locks
                nvlink_lib_link_locks_release(links, numLinks);

                // Release the top-level lock
                nvlink_lib_top_lock_release();
                nvlink_free((void *)links);
                return status;
            }
        }

            writeToken = nvlink_core_get_link_discovery_token(link);

            if ((link->version < NVLINK_DEVICE_VERSION_30) ||
                ((link->localSid == 0) || (link->remoteSid == 0)))
            {
                nvlink_core_write_link_discovery_token(link, writeToken);

                // wrote a token. read back tokens from all links and create connection
                nvlink_core_correlate_conn_by_token(link, writeToken, NV_FALSE);
            }
            else
            {
                // From 3.0 we rely on Sid values. So send skiptoken as true.
                nvlink_core_correlate_conn_by_token(link, writeToken, NV_TRUE);
            }
        }
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    // Release the top-level lock
    nvlink_lib_top_lock_release();

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }
    return NVL_SUCCESS;
}

/**
 * Get the intranode connections from the core library
 *
 * @param[in]  getParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_device_get_intranode_conns
(
    nvlink_device_get_intranode_conns *getParams
)
{
    NvlStatus              status   = NVL_SUCCESS;
    nvlink_device         *dev      = NULL;
    NvU32                  numConns = 0;
    nvlink_intranode_conn *conn     = NULL;

    // Initialize number of connections to 0
    getParams->numConnections = 0;

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists
    //

    // look-up user requested nvlink device object
    nvlink_core_get_device_by_devinfo(&getParams->devInfo, &dev);
    if (dev == NULL)
    {
        //
        // Couldn't find the device ptr in the core library. Release the 
        // top-level lock and return
        //
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    FOR_EACH_CONNECTION(conn, nvlinkLibCtx.nv_intraconn_head, node)
    {
        //
        // copy connection information if source or destination device of
        // this connection belong to the nvlink device specified by user
        //
        if ((conn->end0->dev == dev) || (conn->end1->dev == dev))
        {
            // total number of connections should be within NVLINK_MAX_DEVICE_CONN
            if (numConns >= NVLINK_MAX_DEVICE_CONN)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: numConns >= NVLINK_MAX_DEVICE_CONN\n",
                    __FUNCTION__));

                nvlink_assert(0);

                nvlink_lib_top_lock_release();
                return NVL_ERR_INVALID_STATE;
            }

            // copy source endpoint information
            nvlink_core_copy_endpoint_info(conn->end0, &getParams->conn[numConns].srcEndPoint);

            // copy destination endpoint information
            nvlink_core_copy_endpoint_info(conn->end1, &getParams->conn[numConns].dstEndPoint);

            numConns++;
        }
    }

    getParams->numConnections = numConns;

    // Release the top-level lock
    nvlink_lib_top_lock_release();

    return NVL_SUCCESS;
}

/**
 * Add a discovered internode connection
 *
 * @param[in]  addParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_add_internode_conn
(
    nvlink_add_internode_conn *addParams
)
{
    nvlink_link           *localLink = NULL;
    nvlink_intranode_conn *intraConn = NULL;
    NvlStatus              status    = NVL_SUCCESS;

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists
    //

    // make sure that this connection is multi-node
    if (addParams->localEndPoint.nodeId == addParams->remoteEndPoint.nodeId)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Internode connection add with same node id for local and remote endpoint\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    // validate the remote endpoint device type information
    if (!nvlink_core_is_supported_device_type(addParams->remoteEndPoint.devType))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Internode connection add with invalid remote device type\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    //
    // look-up the nvlink link objects. Look-up will fail if there is a
    // fabric node id mismatch. So an explicit check against self
    // node id is not required.
    //
    nvlink_core_get_link_by_endpoint(&addParams->localEndPoint, &localLink);
    if (localLink == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Internode connection add with no matching local endpoint\n",
            __FUNCTION__));

        //
        // Couldn't find the endpoint registered in the core library. Release the
        // top-level lock and return
        //
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    // can't add internode connection if we have an intranode connection
    nvlink_core_get_intranode_conn(localLink, &intraConn);
    if (intraConn != NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Found an intranode connection while adding internode connection\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    // all the sanity check passed, add this internode connection in our context
    status = nvlink_core_add_internode_conn(localLink, &addParams->remoteEndPoint);

    // Release the top-level lock
    nvlink_lib_top_lock_release();

    return status;
}

/**
 * Remove an internode connection from the list
 *
 * @param[in]  removeParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_remove_internode_conn
(
    nvlink_remove_internode_conn *removeParams
)
{
    nvlink_link *localLink = NULL;
    NvlStatus    status    = NVL_SUCCESS;

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists
    //

    //
    // look-up the nvlink link objects. Look-up will fail if there is a
    // fabric node id mismatch. So an explicit check against self
    // node id is not required.
    //
    nvlink_core_get_link_by_endpoint(&removeParams->localEndPoint, &localLink);
    if (localLink == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Internode connection remove with no matching local endpoint\n",
            __FUNCTION__));

        //
        // Couldn't find the endpoint registered in the core library. Release the
        // top-level lock and return
        //
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    // Acquire the per-link lock
    status = nvlink_lib_link_locks_acquire(&localLink, 1);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link lock\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        return status;
    }

    // all the sanity check passed, remove this internode connection from our context
    nvlink_core_remove_internode_conn(localLink);

    // Release the per-link lock
    nvlink_lib_link_locks_release(&localLink, 1);

    // Release the top-level lock
    nvlink_lib_top_lock_release();

    return NVL_SUCCESS;
}

/**
 * Train the intranode connection to the desired target state
 *
 * @param[in]  trainParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_train_intranode_conn
(
    nvlink_train_intranode_conn *trainParams
)
{
    nvlink_link           *srcLink      = NULL;
    nvlink_link           *dstLink      = NULL;
    nvlink_link           *initLinks[2] = {0};
    nvlink_intranode_conn *conn         = NULL;
    NvlStatus              status       = NVL_SUCCESS;
    NvU32                  count;
    NvU32                  i;

    // make sure that this call is for single node systems
    if (trainParams->srcEndPoint.nodeId != trainParams->dstEndPoint.nodeId)
    {
        return NVL_BAD_ARGS;
    }

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device
    // and link lists
    //

    //
    // look-up the nvlink link objects. Look-up will fail if there is a
    // fabric node id mismatch. So an explicit check against self
    // node id is not required.
    //
    nvlink_core_get_link_by_endpoint(&trainParams->srcEndPoint, &srcLink);
    nvlink_core_get_link_by_endpoint(&trainParams->dstEndPoint, &dstLink);

    // we can't train if both ends are not found
    if ((srcLink == NULL) || (dstLink == NULL))
    {
        //
        // Couldn't find the endpoints registered in the core library. Release
        // the top-level lock and return
        //
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    // look-up the nvlink connection object by source link
    nvlink_core_get_intranode_conn(srcLink, &conn);
    if (conn == NULL)
    {
        //
        // Couldn't find an associated connection for the 2 endpoints. Release
        // the top-level lock and return
        //
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    //
    // we found the connection by the source link. Make sure that dest link is
    // indeed, the user specified one as well
    //
    if ((conn->end0 != dstLink) && (conn->end1 != dstLink))
    {
        //
        // The dest endpoint is not the remote end for the src endpoint. Release
        // the top-level lock and return
        //
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    initLinks[0] = conn->end0;
    initLinks[1] = conn->end1;

    // If loopback then only pass in 1 link
    if (conn->end0 != conn->end1)
    {
        count = 2;
    }
    else
    {
        count = 1;
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(initLinks, 2);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    // the connection looks sane, initiate the training
    switch (trainParams->trainTo)
    {
        case nvlink_train_conn_off_to_swcfg:
        {
            if (srcLink->version >= NVLINK_DEVICE_VERSION_40)
            {
                // non-ALI training for NVLink4.0+
                if (!srcLink->dev->enableALI)
                {
                    nvlink_core_init_links_from_off_to_swcfg_non_ALI(
                                            initLinks, count, NVLINK_STATE_CHANGE_SYNC);
                }
            }
            else
            {
                // ALT training for NVLink3.0+
                nvlink_core_init_links_from_off_to_swcfg(
                                            initLinks, count, NVLINK_STATE_CHANGE_SYNC);
            } 
            break;
        }
        case nvlink_train_conn_swcfg_to_active:
        {
            if (srcLink->version >= NVLINK_DEVICE_VERSION_40)
            {
                // non-ALI training for NVLink4.0+
                if (!srcLink->dev->enableALI)
                {
                    status = nvlink_core_train_intranode_conns_from_swcfg_to_active_non_ALI(
                                                     &conn, 1, NVLINK_STATE_CHANGE_SYNC);
                }
            } 
            else if (srcLink->version >= NVLINK_DEVICE_VERSION_30)
            {
                // ALT training for NVLink3.0+
                status = nvlink_core_train_intranode_conns_from_swcfg_to_active_ALT(
                                                 &conn, 1, NVLINK_STATE_CHANGE_SYNC);
            }
            else
            {
                // Legacy training for pre-NVLink3.0
                status = nvlink_core_train_intranode_conns_from_swcfg_to_active_legacy(
                                                 &conn, 1, NVLINK_STATE_CHANGE_SYNC);
            }
            break;
        }
        case nvlink_train_conn_active_to_swcfg:
        {
            status = nvlink_core_powerdown_intranode_conns_from_active_to_swcfg(
                                                 &conn, 1, NVLINK_STATE_CHANGE_SYNC);
            break;
        }
        case nvlink_train_conn_to_off:
        case nvlink_train_conn_swcfg_to_off:
        {
            status = nvlink_core_powerdown_intranode_conns_from_active_to_off(
                                                 &conn, 1, NVLINK_STATE_CHANGE_SYNC);
            if (status == NVL_SUCCESS)
            {
                nvlink_core_reset_intranode_conns(&conn, 1, NVLINK_STATE_CHANGE_SYNC);
            }
            break;
        }
        case nvlink_train_conn_off_to_active_ali_non_blocking:
        case nvlink_train_conn_off_to_active_ali_blocking:
        {
           if (srcLink->version >= NVLINK_DEVICE_VERSION_40 &&
               srcLink->dev->enableALI)
            {
                status = nvlink_core_train_intranode_conns_from_off_to_active_ALI(initLinks, count);

                if (trainParams->trainTo == nvlink_train_conn_off_to_active_ali_blocking)
                {
                    NvU32 timeout = NVLINK_TRANSITION_HS_TIMEOUT;
                    do
                    {
                        nvlink_sleep(1);
                        status = nvlink_core_train_check_link_ready_ALI(initLinks, count);
                        if (status == NVL_SUCCESS)
                        {
                            break;
                        }

                        timeout--;
                    } while(timeout > 0);

                    if (status == NVL_SUCCESS)
                    {
                        for ( i = 0; i < count; ++i)
                        {
                            //
                            // NVLINK_LINKSTATE_TRAFFIC_SETUP will make sure a request to active completes before
                            // setting buffer ready so use the internal check to see if the request for ALI completed
                            //
                            (void)initLinks[i]->link_handlers->set_dl_link_mode(initLinks[i], NVLINK_LINKSTATE_TRAFFIC_SETUP, 0);
                        }
                }
            }
            }
            break;
        }
        default:
        {
            status = NVL_BAD_ARGS;
            break;
        }
    }

    //
    // always get the latest link state values so that
    // user has additional information other than just the return value.
    //
    nvlink_core_get_endpoint_state(conn->end0, &trainParams->srcEndState);
    nvlink_core_get_endpoint_state(conn->end1, &trainParams->dstEndState);

    // Release the per-link locks
    nvlink_lib_link_locks_release(initLinks, 2);

    return status;
}

/**
 * Train the intranode connections in parallel to the desired target state
 *
 * @param[in]  trainParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_train_intranode_conns_parallel
(
    nvlink_train_intranode_conns_parallel *trainParams
)
{
    nvlink_link            *srcLink    = NULL;
    nvlink_link            *dstLink    = NULL;
    nvlink_link           **trainLinks = NULL;
    nvlink_link           **initLinks  = NULL;
    nvlink_intranode_conn **conns      = NULL;
    NvU32                   numConns   = 0;
    NvlStatus               status     = NVL_SUCCESS;
    NvU32                   i;
    NvU32                   count = 0;

    // sanity check endPointPairsCount
    if (trainParams->endPointPairsCount > NVLINK_MAX_PARALLEL_CONNS_TRAIN_COUNT)
    {
        return NVL_BAD_ARGS;
    }

    //
    // sanity check the input parms
    // make sure that this call is for single node systems
    //
    numConns = trainParams->endPointPairsCount;
    for (i = 0; i < numConns; i++)
    {
        if (trainParams->endPointPairs[i].src.nodeId !=
            trainParams->endPointPairs[i].dst.nodeId)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Node index 0x%x with mis-match ids (src:0x%x dst:0x%x).\n",
                __FUNCTION__ , i,
                trainParams->endPointPairs[i].src.nodeId,
                trainParams->endPointPairs[i].dst.nodeId));

            return NVL_BAD_ARGS;
        }
        if ((trainParams->endPointPairs[i].src.pciInfo.bus == trainParams->endPointPairs[i].dst.pciInfo.bus) &&
            (trainParams->endPointPairs[i].src.pciInfo.device == trainParams->endPointPairs[i].dst.pciInfo.device) &&
            (trainParams->endPointPairs[i].src.pciInfo.function == trainParams->endPointPairs[i].dst.pciInfo.function) &&
            (trainParams->endPointPairs[i].src.linkIndex == trainParams->endPointPairs[i].dst.linkIndex))
        {
            count++;
        }
        else
        {
            count = count + 2;
        }
    }

    // Allocate space for the connection list
    conns = (nvlink_intranode_conn **)nvlink_malloc(
                            sizeof(nvlink_intranode_conn *) * numConns);
    if (conns == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to allocate space for connections list\n",
            __FUNCTION__));

        status = NVL_ERR_GENERIC;
        goto nvlink_lib_ctrl_train_intranode_conns_parallel_end;
    }

    // Allocate space for the links list for link initialization
    initLinks = (nvlink_link **)nvlink_malloc(sizeof(nvlink_link *) * count);
    if (initLinks == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to allocate space for links list for link initialization\n",
            __FUNCTION__));

        status = NVL_ERR_GENERIC;
        goto nvlink_lib_ctrl_train_intranode_conns_parallel_end;
    }

    // Allocate space for the links list for link training
    trainLinks = (nvlink_link **)nvlink_malloc(sizeof(nvlink_link *) * numConns);
    if (trainLinks == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to allocate space for links list for link training\n",
            __FUNCTION__));

        status = NVL_ERR_GENERIC;
        goto nvlink_lib_ctrl_train_intranode_conns_parallel_end;
    }

    nvlink_memset(conns, 0, sizeof(nvlink_intranode_conn *) * numConns);
    nvlink_memset(initLinks,  0, sizeof(nvlink_link *) * count);
    nvlink_memset(trainLinks, 0, sizeof(nvlink_link *) * numConns);

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        goto nvlink_lib_ctrl_train_intranode_conns_parallel_end;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device and
    // link lists and connections list
    //
    count = 0;
    // Get all the connections associated with the list of links
    for (i = 0; i < numConns; i++)
    {
        //
        // look-up the nvlink link objects. Look-up will fail if there is a
        // fabric node id mismatch. So an explicit check against self
        // node id is not required.
        //
        srcLink = NULL;
        dstLink = NULL;

        nvlink_core_get_link_by_endpoint(&trainParams->endPointPairs[i].src, &srcLink);
        nvlink_core_get_link_by_endpoint(&trainParams->endPointPairs[i].dst, &dstLink);

        // we can't train if both ends of a pair not found
        if ((srcLink == NULL) || (dstLink == NULL))
        {
            //
            // Couldn't find the endpoints registered in the core library. Release
            // the top-level lock and return
            //
            nvlink_lib_top_lock_release();

            status = NVL_BAD_ARGS;
            goto nvlink_lib_ctrl_train_intranode_conns_parallel_end;
        }

        // look-up the nvlink connection object by source link
        nvlink_core_get_intranode_conn(srcLink, &conns[i]);
        if (conns[i] == NULL)
        {
            //
            // Couldn't find an associated connection for the 2 endpoints. Release
            // the top-level lock and return
            //
            nvlink_lib_top_lock_release();

            status = NVL_BAD_ARGS;
            goto nvlink_lib_ctrl_train_intranode_conns_parallel_end;
        }

        //
        // we found the connection by source link. Make sure that dest link is
        // indeed, the user specified one as well
        //
        if ((conns[i]->end0 != dstLink) && (conns[i]->end1 != dstLink))
        {
            //
            // The dest endpoint is not the remote end for the src endpoint. Release
            // the top-level lock and return
            //
            nvlink_lib_top_lock_release();

            status = NVL_BAD_ARGS;
            goto nvlink_lib_ctrl_train_intranode_conns_parallel_end;
        }
        if (srcLink == dstLink)
        {
            initLinks[count] = srcLink;
            count++;
        }
        else
        {
            initLinks[count]       = srcLink;
            initLinks[count + 1]   = dstLink;
            count = count + 2;
        }
        trainLinks[i]      = srcLink;
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(initLinks, count);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        goto nvlink_lib_ctrl_train_intranode_conns_parallel_end;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    // Check all the links captured have version >= 3.0
    for (i = 0; i < numConns; i++)
    {
        // Parallel training allowed NvLink 3.0 & above
        if ((conns[i]->end0->version < NVLINK_DEVICE_VERSION_30) ||
            (conns[i]->end1->version < NVLINK_DEVICE_VERSION_30))
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Parallel training not allowed with nvlink version 0x%x indexed 0x%x\n",
                __FUNCTION__ ,
                conns[i]->end0->version, i));

            //
            // Parallel training is allowed for only NVLink 3.0 and above. Release
            // the per link locks and return
            //
            nvlink_lib_link_locks_release(initLinks, count);

            status = NVL_BAD_ARGS;
            goto nvlink_lib_ctrl_train_intranode_conns_parallel_end;
        }
    }

    // the connection looks sane, initiate the training
    switch (trainParams->trainTo)
    {
        case nvlink_train_conn_off_to_swcfg:
        {
            if (srcLink->version >= NVLINK_DEVICE_VERSION_40)
            {
                // non-ALI training for NVLink4.0+
                if (!srcLink->dev->enableALI)
                {
                    nvlink_core_init_links_from_off_to_swcfg_non_ALI(
                                            initLinks, count, NVLINK_STATE_CHANGE_SYNC);
                }
            }
            else
            {
                // ALT training for NVLink3.0+
                nvlink_core_init_links_from_off_to_swcfg(
                                            initLinks, count, NVLINK_STATE_CHANGE_SYNC);
            } 
            break;
        }
        case nvlink_train_conn_swcfg_to_active:
        {
            if (srcLink->version >= NVLINK_DEVICE_VERSION_40)
            {
                // non-ALI training for NVLink4.0+
                if (!srcLink->dev->enableALI)
                {
                    status = nvlink_core_train_intranode_conns_from_swcfg_to_active_non_ALI(
                                                        conns, numConns, NVLINK_STATE_CHANGE_SYNC);
                }
            }
            else
            {
                // ALT training for NVLink3.0+
                status = nvlink_core_train_intranode_conns_from_swcfg_to_active_ALT(
                                                        conns, numConns, NVLINK_STATE_CHANGE_SYNC);
            } 
            break;
        }
        case nvlink_train_conn_active_to_swcfg:
        {
            status = nvlink_core_powerdown_intranode_conns_from_active_to_swcfg(
                                                    conns, numConns, NVLINK_STATE_CHANGE_SYNC);
            break;
        }
        case nvlink_train_conn_to_off:
        case nvlink_train_conn_swcfg_to_off:
        {
            status = nvlink_core_powerdown_intranode_conns_from_active_to_off(
                                                    conns, numConns, NVLINK_STATE_CHANGE_SYNC);
            if (status == NVL_SUCCESS)
            {
                nvlink_core_reset_intranode_conns(conns, numConns, NVLINK_STATE_CHANGE_SYNC);
            }
            break;
        }
        case nvlink_train_conn_off_to_active_ali_non_blocking:
        case nvlink_train_conn_off_to_active_ali_blocking:
        {
            if (srcLink->version == NVLINK_DEVICE_VERSION_40 &&
                srcLink->dev->enableALI)
            {
                status = nvlink_core_train_intranode_conns_from_off_to_active_ALI(
                                                 initLinks, count);

                if (trainParams->trainTo == nvlink_train_conn_off_to_active_ali_blocking)
                {
                    NvU32 timeout = NVLINK_TRANSITION_HS_TIMEOUT;
                    do
                    {
                        nvlink_sleep(1);
                        status = nvlink_core_train_check_link_ready_ALI(initLinks, count);
                        if (status == NVL_SUCCESS)
                        {
                            break;
                        }

                        timeout--;
                    } while(timeout > 0);

                    if (status == NVL_SUCCESS)
                    {
                        for ( i = 0; i < count; ++i)
                        {
                            //
                            // NVLINK_LINKSTATE_TRAFFIC_SETUP will make sure a request to active completes before
                            // setting buffer ready so use the internal check to see if the request for ALI completed
                            //
                            (void)initLinks[i]->link_handlers->set_dl_link_mode(initLinks[i], NVLINK_LINKSTATE_TRAFFIC_SETUP, 0);
                        }
                }
            }
            }
            break;
        }
        default:
        {
            status = NVL_BAD_ARGS;
            break;
        }
    }

    //
    // always get the latest link state values when the args are verified
    // so that user has additional information other than just the return value.
    //
    for (i = 0; i < numConns; i++)
    {
        nvlink_core_get_endpoint_state(conns[i]->end0, &trainParams->endpointPairsStates[i].srcEnd);
        nvlink_core_get_endpoint_state(conns[i]->end1, &trainParams->endpointPairsStates[i].dstEnd);
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(initLinks, count);

nvlink_lib_ctrl_train_intranode_conns_parallel_end:

    if (conns != NULL)
    {
        nvlink_free((void *)conns);
    }

    if (initLinks != NULL)
    {
        nvlink_free((void *)initLinks);
    }

    if (trainLinks != NULL)
    {
        nvlink_free((void *)trainLinks);
    }

    return status;
}

/**
 * Train the internode connection link to the target state
 *
 * @param[in]  linkParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_train_internode_conn_link
(
    nvlink_train_internode_conn_link *linkParams
)
{
    nvlink_link           *localLink = NULL;
    NvlStatus              status    = NVL_SUCCESS;
    nvlink_internode_conn *interConn = NULL;

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device and
    // link lists and connections list
    //

    //
    // look-up the nvlink link objects. Look-up will fail if there is a
    // fabric node id mismatch. So an explicit check against self
    // node id is not required.
    //
    nvlink_core_get_link_by_endpoint(&linkParams->localEndPoint, &localLink);

    // user specified link is not available
    if (localLink == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Internode connection link train request with no matching local endpoint\n",
            __FUNCTION__));

        //
        // Couldn't find the endpoint registered in the core library. Release
        // the top-level lock and return
        //
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    nvlink_core_get_internode_conn(localLink, &interConn);
    if (interConn == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No Internode connection exists for local endpoint %s: %s.\n",
            __FUNCTION__, localLink->dev->deviceName, localLink->linkName));

        //
        // Couldn't find an associated connection for the endpoint. Release
        // the top-level lock and return
        //
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    // Acquire the per-link lock
    status = nvlink_lib_link_locks_acquire(&localLink, 1);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();


    switch (linkParams->trainTo)
    {
        case nvlink_train_link_off_to_swcfg:
        {
            // OFF to SAFE is part of initialization sequence as of now.
            status = NVL_BAD_ARGS;
            break;
        }
        case nvlink_train_link_swcfg_to_active:
        {
            status = nvlink_core_train_internode_conns_from_swcfg_to_active(
                        &interConn, 1, &linkParams->isMasterEnd, NVLINK_STATE_CHANGE_SYNC);
            break;
        }
        case nvlink_train_link_to_off:
        {
            // OFF state transitions are not supported/tested
            status = NVL_BAD_ARGS;
            break;
        }
        case nvlink_train_link_active_to_swcfg:
        {
            // not implemented/supported now
            status =  NVL_BAD_ARGS;
            break;
        }
        case nvlink_train_link_swcfg_to_off:
        {
            // OFF state transitions are not supported/tested
            status = NVL_BAD_ARGS;
            break;
        }
        default:
        {
            status = NVL_BAD_ARGS;
            break;
        }
    }

    //
    // always get the latest link state values so that
    // user has additional information other than just the return value.
    //
    nvlink_core_get_endpoint_state(localLink, &linkParams->localEndState);

    // Release the per-link lock
    nvlink_lib_link_locks_release(&localLink, 1);

    return status;
}

/*
 * Train the internode connection sublink to the target state
 *
 * @param[in]  subLinkParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_train_internode_conn_sublink
(
    nvlink_train_internode_conn_sublink *subLinkParams
)
{
    nvlink_link           *localLink = NULL;
    NvlStatus              status    = NVL_SUCCESS;
    nvlink_internode_conn *interConn = NULL;

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device and
    // link lists and connections list
    //

    //
    // look-up the nvlink link objects. Look-up will fail if there is a
    // fabric node id mismatch. So an explicit check against self
    // node id is not required.
    //
    nvlink_core_get_link_by_endpoint(&subLinkParams->localEndPoint, &localLink);

    // user specified link is not available
    if (localLink == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Internode connection sublink train request with no matching local endpoint\n",
            __FUNCTION__));

        //
        // Couldn't find the endpoint registered in the core library. Release
        // the top-level lock and return
        //
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    nvlink_core_get_internode_conn(localLink, &interConn);
    if (interConn == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No Internode connection exists for local endpoint %s: %s.\n",
            __FUNCTION__, localLink->dev->deviceName, localLink->linkName));

        //
        // Couldn't find an associated connection for the endpoint. Release
        // the top-level lock and return
        //
        nvlink_lib_top_lock_release();

        return NVL_BAD_ARGS;
    }

    // Acquire the per-link lock
    status = nvlink_lib_link_locks_acquire(&localLink, 1);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    switch (subLinkParams->trainTo)
    {
        case nvlink_train_sublink_off_to_safe:
        {
            // OFF to SAFE is part of initialization sequence as of now.
            status = NVL_BAD_ARGS;
            break;
        }
        case nvlink_train_sublink_safe_to_hs:
        {
            // NVLink 3.0 onwards this is handled through INITOPTIMIZE
            if (localLink->version >= NVLINK_DEVICE_VERSION_30)
            {
                return NVL_ERR_NOT_SUPPORTED;
            }
            status = nvlink_core_train_internode_conn_sublink_from_safe_to_hs(
                         interConn, NVLINK_STATE_CHANGE_SYNC);
            break;
        }
        case nvlink_train_sublink_to_off:
        {
            // OFF state transitions are not supported/tested
            status = NVL_BAD_ARGS;
            break;
        }
        case nvlink_train_sublink_hs_to_safe:
        {
            // not implemented/supported now
            status = NVL_BAD_ARGS;
            break;
        }
        case nvlink_train_sublink_safe_to_off:
        {
            // OFF state transitions are not supported/tested
            status = NVL_BAD_ARGS;
            break;
        }
        default:
        {
            status = NVL_BAD_ARGS;
            break;
        }
    }

    //
    // always get the latest link state values so that
    // user has additional information other than just the return value.
    //
    nvlink_core_get_endpoint_state(localLink, &subLinkParams->localEndState);

    // Release the per-link lock
    nvlink_lib_link_locks_release(&localLink, 1);

    return status;
}

/**
 * Send INITOPTIMIZE on the given internode links
 *
 * @param[in]  initoptimizeParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_train_internode_links_initoptimize
(
    nvlink_train_internode_links_initoptimize *initoptimizeParams
)
{
    nvlink_link  *endpoint  = NULL;
    NvlStatus     status    = NVL_SUCCESS;
    NvU32         numLinks  = 0;
    NvU32         i         = 0;

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
    {
        return NVL_NO_MEM;
    }

    if (initoptimizeParams->endPointCount > NVLINK_MAX_NVLINK_ENDPOINTS)
    {
        nvlink_free((void *)links);
        return NVL_BAD_ARGS;
    }

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        nvlink_free((void *)links);
        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device and
    // link lists and connections list
    //

    for (i = 0; i < initoptimizeParams->endPointCount; i++)
    {
        endpoint = NULL;
        nvlink_core_get_link_by_endpoint(&initoptimizeParams->endPoints[i], &endpoint);

        // we can't send INITOPTIMIZE if the endpoint is not found
        if (endpoint == NULL)
        {
            //
            // Couldn't find the endpoint registered in the core library. Release
            // the top-level lock and return
            //
            nvlink_lib_top_lock_release();

            nvlink_free((void *)links);
            return NVL_BAD_ARGS;
        }
        else if (numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                __FUNCTION__));

            nvlink_assert(0);

            // Release the top-level lock and free links
            nvlink_lib_top_lock_release();
            nvlink_free((void *)links);
            return NVL_ERR_INVALID_STATE;
        }

        links[numLinks] = endpoint;
        numLinks++;
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    for (i = 0; i < numLinks; i++)
    {
        // INITOPTIMIZE is not supported before NVLink 3.0
        if (links[i]->version < NVLINK_DEVICE_VERSION_30)
            continue;

        // Continue if the link is already active, nothing to do
        if ((nvlink_core_check_link_state(links[i], NVLINK_LINKSTATE_HS)) &&
            (nvlink_core_check_tx_sublink_state(links[i], NVLINK_SUBLINK_STATE_TX_HS)) &&
            (nvlink_core_check_rx_sublink_state(links[i], NVLINK_SUBLINK_STATE_RX_HS)))
        {
            continue;
        }

        //
        // For INITOPTIMIZE, link should be in SWCFG, else flag error and continue
        // to next link
        //
        if (!((nvlink_core_check_link_state(links[i], NVLINK_LINKSTATE_SAFE)) &&
              (nvlink_core_check_tx_sublink_state(links[i], NVLINK_SUBLINK_STATE_TX_SAFE)) &&
              (nvlink_core_check_rx_sublink_state(links[i], NVLINK_SUBLINK_STATE_RX_SAFE))))
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: INITOPTIMIZE only works for links in SAFE %s:%s.\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            continue;
        }

        status = links[i]->link_handlers->set_dl_link_mode(links[i],
                                                           NVLINK_LINKSTATE_INITOPTIMIZE,
                                                           NVLINK_STATE_CHANGE_ASYNC);

        // Although it failed we need to continue on other links.
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: INITOPTIMIZE failed on Device:Link %s:%s\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
        }
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }
    return NVL_SUCCESS;
}

/**
 * Send POSTINITOPTIMIZE on the given internode links
 *
 * @param[in]  initoptimizeParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_train_internode_links_post_initoptimize
(
    nvlink_train_internode_links_post_initoptimize *postinitoptimizeParams
)
{
    nvlink_link  *endpoint  = NULL;
    NvlStatus     status    = NVL_SUCCESS;
    NvU32         numLinks  = 0;
    NvU32         i         = 0;

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
    {
        return NVL_NO_MEM;
    }

    if (postinitoptimizeParams->endPointCount > NVLINK_MAX_NVLINK_ENDPOINTS)
    {
        nvlink_free((void *)links);
        return NVL_BAD_ARGS;
    }

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        nvlink_free((void *)links);
        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device and
    // link lists and connections list
    //

    for (i = 0; i < postinitoptimizeParams->endPointCount; i++)
    {
        endpoint = NULL;
        nvlink_core_get_link_by_endpoint(&postinitoptimizeParams->endPoints[i], &endpoint);

        // we can't send INITOPTIMIZE if the endpoint is not found
        if (endpoint == NULL)
        {
            //
            // Couldn't find the endpoint registered in the core library. Release
            // the top-level lock and return
            //
            nvlink_lib_top_lock_release();

            nvlink_free((void *)links);
            return NVL_BAD_ARGS;
        }
        else if (numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                __FUNCTION__));

            nvlink_assert(0);

            // Release the top-level lock and free links
            nvlink_lib_top_lock_release();
            nvlink_free((void *)links);
            return NVL_ERR_INVALID_STATE;
        }

        links[numLinks] = endpoint;
        numLinks++;
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    for (i = 0; i < numLinks; i++)
    {
        // POST_INITOPTIMIZE is not supported before NVLink 3.0
        if (links[i]->version < NVLINK_DEVICE_VERSION_30)
            continue;

        // Continue if the link is already active, nothing to do
        if ((nvlink_core_check_link_state(links[i], NVLINK_LINKSTATE_HS)) &&
            (nvlink_core_check_tx_sublink_state(links[i], NVLINK_SUBLINK_STATE_TX_HS)) &&
            (nvlink_core_check_rx_sublink_state(links[i], NVLINK_SUBLINK_STATE_RX_HS)))
        {
            continue;
        }

        status = links[i]->link_handlers->set_dl_link_mode(links[i],
                                                NVLINK_LINKSTATE_POST_INITOPTIMIZE,
                                                NVLINK_STATE_CHANGE_ASYNC);

        // Although it failed we need to continue on other links.
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: POST_INITOPTIMIZE failed on Device:Link %s:%s\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
        }
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }
    return NVL_SUCCESS;
}

/**
 * Train the internode connection links to the target state
 *
 * @param[in]  linkParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_train_internode_conns_parallel
(
    nvlink_train_internode_conns_parallel *linkParams
)
{
    nvlink_link            *localLink  = NULL;
    NvlStatus               status     = NVL_SUCCESS;
    NvU32                   numLinks   = 0;
    NvU32                   i          = 0;
    nvlink_link           **links      = NULL;
    nvlink_internode_conn **interConns = NULL;

    links = (nvlink_link **)nvlink_malloc(
                    sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
    {
        status = NVL_NO_MEM;
        goto nvlink_lib_ctrl_train_internode_conns_parallel_end;
    }

    interConns = (nvlink_internode_conn **)nvlink_malloc(
                    sizeof(nvlink_internode_conn *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (interConns == NULL)
    {
        status = NVL_NO_MEM;
        goto nvlink_lib_ctrl_train_internode_conns_parallel_end;
    }

    if (linkParams->localEndPointCount > NVLINK_MAX_PARALLEL_CONNS_TRAIN_COUNT)
    {
        status = NVL_BAD_ARGS;
        goto nvlink_lib_ctrl_train_internode_conns_parallel_end;
    }

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        goto nvlink_lib_ctrl_train_internode_conns_parallel_end;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device and
    // link lists and connections list
    //
    for (i = 0; i < linkParams->localEndPointCount; i++)
    {
        //
        // look-up the nvlink link objects. Look-up will fail if there is a
        // fabric node id mismatch. So an explicit check against self
        // node id is not required.
        //
        nvlink_core_get_link_by_endpoint(&linkParams->localEndPoints[i], &localLink);

        // user specified link is not available
        if (localLink == NULL)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Internode connection link train request with no matching local endpoint\n",
                __FUNCTION__));

            //
            // Couldn't find the endpoint registered in the core library. Release
            // the top-level lock and return
            //
            nvlink_lib_top_lock_release();

            status = NVL_BAD_ARGS;
            goto nvlink_lib_ctrl_train_internode_conns_parallel_end;
        }

        nvlink_core_get_internode_conn(localLink, &(interConns[i]));
        if (interConns[i] == NULL)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: No Internode connection exists for local endpoint %s: %s.\n",
                __FUNCTION__, localLink->dev->deviceName, localLink->linkName));

            //
            // Couldn't find an associated connection for the endpoint. Release
            // the top-level lock and return
            //
            nvlink_lib_top_lock_release();

            status = NVL_BAD_ARGS;
            goto nvlink_lib_ctrl_train_internode_conns_parallel_end;
        }

        links[numLinks] = localLink;
        numLinks++;
    }

    // Acquire the per-link lock
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        goto nvlink_lib_ctrl_train_internode_conns_parallel_end;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    switch (linkParams->trainTo)
    {
        case nvlink_train_link_off_to_swcfg:
        {
            // OFF to SAFE is part of initialization sequence as of now.
            status = NVL_BAD_ARGS;
            break;
        }
        case nvlink_train_link_swcfg_to_active:
        {
            status = nvlink_core_train_internode_conns_from_swcfg_to_active(
                        interConns, numLinks, linkParams->isMasterEnd, NVLINK_STATE_CHANGE_SYNC);
            break;
        }
        case nvlink_train_link_to_off:
        {
            // OFF state transitions are not supported/tested
            status = NVL_BAD_ARGS;
            break;
        }
        case nvlink_train_link_active_to_swcfg:
        {
            // not implemented/supported now
            status =  NVL_BAD_ARGS;
            break;
        }
        case nvlink_train_link_swcfg_to_off:
        {
            // OFF state transitions are not supported/tested
            status = NVL_BAD_ARGS;
            break;
        }
        default:
        {
            status = NVL_BAD_ARGS;
            break;
        }
    }

    for (i = 0; i < numLinks; i++)
    {

        //
        // always get the latest link state values so that
        // user has additional information other than just the return value.
        //
        nvlink_core_get_endpoint_state(links[i], &linkParams->localEndStates[i]);
    }

    // Release the per-link lock
    nvlink_lib_link_locks_release(links, numLinks);

nvlink_lib_ctrl_train_internode_conns_parallel_end:

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }
    if (interConns != NULL)
    {
        nvlink_free((void *)interConns);
    }

    return status;
}

/**
 * Get the device information for all registered devices
 *
 * @param[in]  infoParams  IOCTL params
 *
 * return NvlStatus
 */
static NvlStatus
nvlink_lib_ctrl_get_devices_info
(
    nvlink_get_devices_info *infoParams
)
{
    nvlink_device *dev        = NULL;
    NvlStatus      status     = NVL_SUCCESS;
    NvU32          numDevices = 0;

    // Initialize number of devices to 0
    infoParams->numDevice = 0;

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device and
    // link lists and connections list
    //

    FOR_EACH_DEVICE_REGISTERED(dev, nvlinkLibCtx.nv_devicelist_head, node)
    {
        // total number of devices should be within NVLINK_DEVICE_INSTANCE_MAX
        if (numDevices >= NVLINK_DEVICE_INSTANCE_MAX)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: numDevices >= NVLINK_DEVICE_INSTANCE_MAX",
                __FUNCTION__));

            nvlink_assert(0);
            nvlink_lib_top_lock_release();
            return NVL_ERR_INVALID_STATE;
        }

        // copy device information
        nvlink_core_copy_device_info(dev, &infoParams->devInfo[numDevices]);
        numDevices++;
    }

    infoParams->numDevice = numDevices;

    // Release the top-level lock
    nvlink_lib_top_lock_release();

    return status;
}

static NvlStatus
nvlink_lib_ctrl_acquire_capability
(
    nvlink_ioctrl_params      *ctrlParams,
    nvlink_acquire_capability *capParams
)
{
    NvlStatus status;

    if (capParams == NULL)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Bad ioctl capability ctrl params specified.\n",
            __FUNCTION__));
        return NVL_BAD_ARGS;
    }

    status = nvlink_acquire_fabric_mgmt_cap(ctrlParams->osPrivate,
                                            capParams->capDescriptor);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire fabric mgmt capability.\n",
            __FUNCTION__));
        return status;
    }

    return NVL_SUCCESS;
}

static NvlStatus nvlink_lib_ctrl_get_link_state
(
    nvlink_get_link_state *linkParams
)
{
    nvlink_link  *endpoint  = NULL;
    NvlStatus     status    = NVL_SUCCESS;
    NvU32         numLinks  = 0;
    NvU32         i         = 0;

    ct_assert(NVLINK_MAX_SYSTEM_LINK_NUM == NVLINK_MAX_NVLINK_ENDPOINTS);

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_SYSTEM_LINK_NUM);
    if (links == NULL)
    {
        return NVL_NO_MEM;
    }

    if (linkParams->endPointCount > NVLINK_MAX_NVLINK_ENDPOINTS)
    {
        nvlink_free((void *)links);
        return NVL_BAD_ARGS;
    }

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        nvlink_free((void *)links);
        return status;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the device and
    // link lists and connections list
    //

    for (i = 0; i < linkParams->endPointCount; i++)
    {
        endpoint = NULL;
        nvlink_core_get_link_by_endpoint(&linkParams->endPoints[i], &endpoint);

        // we can't send this command if the endpoint is not found
        if (endpoint == NULL)
        {
            //
            // Couldn't find the endpoint registered in the core library. Release
            // the top-level lock and return
            //
            nvlink_lib_top_lock_release();

            nvlink_free((void *)links);
            return NVL_BAD_ARGS;
        }
        else if (numLinks >= NVLINK_MAX_NVLINK_ENDPOINTS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: numLinks >= NVLINK_MAX_SYSTEM_LINK_NUM",
                __FUNCTION__));

            nvlink_assert(0);

            // Release the top-level lock and free links
            nvlink_lib_top_lock_release();
            nvlink_free((void *)links);
            return NVL_ERR_INVALID_STATE;
        }

        links[numLinks] = endpoint;
        numLinks++;
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    for (i = 0; i < numLinks; i++)
    {
        // Wait for the link state to change.
        status = nvlink_core_poll_link_state(links[i],
                                             NVLINK_LINKSTATE_HS,
                                             NVLINK_TRANSITION_POST_HS_TIMEOUT);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to set link state to ACTIVE for link"
                " %s:%s \n",
                __FUNCTION__,
                links[i]->dev->deviceName, links[i]->linkName));
        }
        else
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_SETUP,
                "%s: Successfully able to set link state to ACTIVE for link"
                " %s:%s \n",
                __FUNCTION__,
                links[i]->dev->deviceName, links[i]->linkName));
        }

        nvlink_core_get_endpoint_state(links[i], &linkParams->endState[i]);
    }

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }

    return NVL_SUCCESS;
}

static NvlStatus
nvlink_lib_ctrl_get_device_link_states
(
    nvlink_get_device_link_states *params
)
{
    nvlink_link  *endpoint  = NULL;
    nvlink_device *dev      = NULL;
    NvlStatus     status    = NVL_SUCCESS;
    NvU32         numLinks  = 0;
    NvU32         i         = 0;
    NvU8          linkNumber;

    nvlink_link   **links = (nvlink_link **)nvlink_malloc(
                            sizeof(nvlink_link *) * NVLINK_MAX_DEVICE_CONN);

    // Get current monotonic time in seconds.nanoseconds
    params->time = nvlink_get_platform_time();

    if (links == NULL)
    {
        return NVL_NO_MEM;
    }

    nvlink_memset(params->endStates, 0x0, sizeof(params->endStates));    

    // Acquire the top-level lock
    status = nvlink_lib_top_lock_acquire();
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire top-level lock\n",
            __FUNCTION__));

        nvlink_free((void *)links);
        return status;
    }

    // look-up user requested nvlink device object
    nvlink_core_get_device_by_devinfo(&params->devInfo, &dev);
    if (dev == NULL)
    {
        //
        // Couldn't find the device ptr in the core library. Release the 
        // top-level lock and return
        //
        nvlink_lib_top_lock_release();

        nvlink_free((void *)links);
        return NVL_BAD_ARGS;
    }

    //
    // Top-level lock is now acquired. Proceed to traversing the list
    // of devices and list of links to lock all links
    //
    FOR_EACH_LINK_REGISTERED(endpoint, dev, node)
    {
        if (numLinks >= NVLINK_MAX_DEVICE_CONN)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: numLinks >= NVLINK_MAX_DEVICE_CONN",
                __FUNCTION__));

            nvlink_assert(0);

            // Release the top-level lock and free links
            nvlink_lib_top_lock_release();
            nvlink_free((void *)links);
            return NVL_ERR_INVALID_STATE;
        }
        links[numLinks] = endpoint;
        numLinks++;
    }

    // Acquire the per-link locks
    status = nvlink_lib_link_locks_acquire(links, numLinks);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Failed to acquire per-link locks\n",
            __FUNCTION__));

        // Release the top-level lock
        nvlink_lib_top_lock_release();
        nvlink_free((void *)links);
        return status;
    }

    //
    // All the required per-link locks are now successfully acquired
    // Release the top level-lock
    //
    nvlink_lib_top_lock_release();

    nvlink_assert((links != NULL) && (numLinks > 0));

    for (i = 0; i < numLinks; ++i)
    {
        linkNumber = links[i]->linkNumber;

        nvlink_assert(linkNumber < NVLINK_MAX_DEVICE_CONN);

        // Get the endpoint states of the link
        nvlink_core_get_endpoint_state(links[i], &(params->endStates[linkNumber]));

        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: link 0x%x -- linkMode 0x%x,\n",
            __FUNCTION__, linkNumber, params->endStates[linkNumber].linkMode));
    }

    // This is done to preserve client behavior that uses endStatesCount to iterate across endStates array
    params->endStatesCount = NVLINK_MAX_DEVICE_CONN;

    // Release the per-link locks
    nvlink_lib_link_locks_release(links, numLinks);

    if (links != NULL)
    {
        nvlink_free((void *)links);
    }

    return status;
}
