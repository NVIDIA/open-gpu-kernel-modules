/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nv.h>
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "osapi.h"

#if defined(INCLUDE_NVLINK_LIB)
#include "nvlink.h"
//
// The functions in this file are a workaround for a significant design flaw
// where RM callbacks for the nvlink library are built with the altstack
// enabled, but the nvlink library is not built with altstack support. Whenever
// the library calls a callback, the stack switching needs to be accounted for
// or else we will observe corruption of data structures in the nvlink library
// as data is pushed onto what the callback thinks is the stack. See bug
// 1710300.
//
// This bug has also exposed other problems, such as the complete lack of
// locking awareness by these callbacks (e.g., assumption that the RMAPI and
// GPU locks are always held on entry, which is not a legitimate assumption).
// For now, we ignore that just to unblock testing.
//
extern NvlStatus knvlinkCoreAddLinkCallback(struct nvlink_link *);
extern NvlStatus knvlinkCoreRemoveLinkCallback(struct nvlink_link *);
extern NvlStatus knvlinkCoreLockLinkCallback(struct nvlink_link *);
extern void      knvlinkCoreUnlockLinkCallback(struct nvlink_link *);
extern NvlStatus knvlinkCoreQueueLinkChangeCallback(struct nvlink_link_change *);
extern NvlStatus knvlinkCoreSetDlLinkModeCallback(struct nvlink_link *, NvU64, NvU32);
extern NvlStatus knvlinkCoreGetDlLinkModeCallback(struct nvlink_link *, NvU64 *);
extern NvlStatus knvlinkCoreSetTlLinkModeCallback(struct nvlink_link *, NvU64, NvU32);
extern NvlStatus knvlinkCoreGetTlLinkModeCallback(struct nvlink_link *, NvU64 *);
extern NvlStatus knvlinkCoreGetTxSublinkModeCallback(struct nvlink_link *, NvU64 *, NvU32 *);
extern NvlStatus knvlinkCoreSetTxSublinkModeCallback(struct nvlink_link *, NvU64, NvU32);
extern NvlStatus knvlinkCoreGetRxSublinkModeCallback(struct nvlink_link *, NvU64 *, NvU32 *);
extern NvlStatus knvlinkCoreSetRxSublinkModeCallback(struct nvlink_link *, NvU64, NvU32);
extern NvlStatus knvlinkCoreReadDiscoveryTokenCallback(struct nvlink_link *, NvU64 *);
extern NvlStatus knvlinkCoreWriteDiscoveryTokenCallback(struct nvlink_link *, NvU64);
extern void      knvlinkCoreTrainingCompleteCallback(struct nvlink_link *);
extern void      knvlinkCoreGetUphyLoadCallback(struct nvlink_link *, NvBool*);
extern NvlStatus knvlinkCoreGetCciLinkModeCallback(struct nvlink_link *, NvU64 *);

/*!
 * @brief Helper to allocate an alternate stack from within core RM.
 *
 * This needs to be an NV_API_CALL (built to use the original stack instead
 * of the altstack) since it is called before we switch to using the altstack.
 */
static NV_STATUS NV_API_CALL osNvlinkAllocAltStack(nvidia_stack_t **pSp)
{
    NV_STATUS status = NV_OK;
    nvidia_stack_t *sp = NULL;
#if defined(NVCPU_X86_64) && defined(__use_altstack__)
    status = os_alloc_mem((void **)&sp, sizeof(nvidia_stack_t));
    if (status == NV_OK)
    {
        sp->size = sizeof(sp->stack);
        sp->top  = sp->stack + sp->size;
    }
#endif
    *pSp = sp;
    return status;
}

/*!
 * @brief Helper to free an alternate stack from within core RM.
 *
 * This needs to be an NV_API_CALL (built to use the original stack instead
 * of the altstack) since it is called after we've switched back to using the
 * original stack.
 */
static void NV_API_CALL osNvlinkFreeAltStack(nvidia_stack_t *sp)
{
#if defined(NVCPU_X86_64) && defined(__use_altstack__)
    os_free_mem(sp);
#endif
}

static NV_STATUS NV_API_CALL osNvlinkGetAltStack(nvidia_stack_t **sp)
{
    return osNvlinkAllocAltStack(sp);
}
static void NV_API_CALL osNvlinkPutAltStack(nvidia_stack_t *sp)
{
    osNvlinkFreeAltStack(sp);
}

static NvlStatus NV_API_CALL rm_nvlink_ops_add_link
(
    struct nvlink_link *link
)
{
    void *fp;
    NvlStatus status;
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    status = knvlinkCoreAddLinkCallback(link);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_remove_link
(
    struct nvlink_link *link
)
{
    void *fp;
    NvlStatus status;
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    status = knvlinkCoreRemoveLinkCallback(link);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_lock_link
(
    struct nvlink_link *link
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreLockLinkCallback(link);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static void NV_API_CALL rm_nvlink_ops_unlock_link
(
    struct nvlink_link *link
)
{
    void *fp;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    knvlinkCoreUnlockLinkCallback(link);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);
}

static NvlStatus NV_API_CALL rm_nvlink_ops_queue_link_change
(
    struct nvlink_link_change *link_change
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreQueueLinkChangeCallback(link_change);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_set_dl_link_mode
(
    struct nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreSetDlLinkModeCallback(link, mode, flags);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_get_dl_link_mode
(
    struct nvlink_link *link,
    NvU64 *mode
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreGetDlLinkModeCallback(link, mode);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_set_tl_link_mode
(
    struct nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreSetTlLinkModeCallback(link, mode, flags);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);
    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_get_tl_link_mode
(
    struct nvlink_link *link,
    NvU64 *mode
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreGetTlLinkModeCallback(link, mode);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_set_link_tx_mode
(
    struct nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreSetTxSublinkModeCallback(link, mode, flags);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_get_link_tx_mode
(
    struct nvlink_link *link,
    NvU64 *mode,
    NvU32 *subMode
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreGetTxSublinkModeCallback(link, mode, subMode);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_set_link_rx_mode
(
    struct nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreSetRxSublinkModeCallback(link, mode, flags);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_get_link_rx_mode
(
    struct nvlink_link *link,
    NvU64 *mode,
    NvU32 *subMode
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreGetRxSublinkModeCallback(link, mode, subMode);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_set_link_rx_detect
(
    struct nvlink_link *link,
    NvU32 flags
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreSetRxSublinkDetectCallback(link, flags);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_get_link_rx_detect
(
    struct nvlink_link *link
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreGetRxSublinkDetectCallback(link);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static void NV_API_CALL rm_nvlink_get_uphy_load
(
    struct nvlink_link *link,
    NvBool *bUnlocked
)
{
    void *fp;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    knvlinkCoreGetUphyLoadCallback(link, bUnlocked);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);
}

static NvlStatus NV_API_CALL rm_nvlink_ops_read_link_discovery_token
(
    struct nvlink_link *link,
    NvU64 *token
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreReadDiscoveryTokenCallback(link, token);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_write_link_discovery_token
(
    struct nvlink_link *link,
    NvU64 token
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreWriteDiscoveryTokenCallback(link, token);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);

    return status;
}

static void NV_API_CALL rm_nvlink_ops_training_complete
(
    struct nvlink_link *link
)
{
    void *fp;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t *sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    knvlinkCoreTrainingCompleteCallback(link);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);
}

static NvlStatus NV_API_CALL rm_nvlink_ops_ali_training
(
    struct nvlink_link *link
)
{
    void *fp;
    NvlStatus status;
    THREAD_STATE_NODE threadState = {0};
    nvidia_stack_t * sp;

    if (osNvlinkGetAltStack(&sp) != NV_OK)
    {
        return NVL_ERR_GENERIC;
    }

    NV_ENTER_RM_RUNTIME(sp, fp);

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);
    status = knvlinkCoreAliTrainingCallback(link);
    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    NV_EXIT_RM_RUNTIME(sp, fp);

    osNvlinkPutAltStack(sp);
    return status;
}

static NvlStatus NV_API_CALL rm_nvlink_ops_get_cci_link_mode
(
    struct nvlink_link *link,
    NvU64 *mode
)
{
    return NVL_SUCCESS;
}

#endif /* defined(INCLUDE_NVLINK_LIB) */

const struct nvlink_link_handlers* osGetNvlinkLinkCallbacks(void)
{
#if defined(INCLUDE_NVLINK_LIB)
    static const struct nvlink_link_handlers rm_nvlink_link_ops =
    {
        .add                        = rm_nvlink_ops_add_link,
        .remove                     = rm_nvlink_ops_remove_link,
        .lock                       = rm_nvlink_ops_lock_link,
        .unlock                     = rm_nvlink_ops_unlock_link,
        .queue_link_change          = rm_nvlink_ops_queue_link_change,
        .set_dl_link_mode           = rm_nvlink_ops_set_dl_link_mode,
        .get_dl_link_mode           = rm_nvlink_ops_get_dl_link_mode,
        .set_tl_link_mode           = rm_nvlink_ops_set_tl_link_mode,
        .get_tl_link_mode           = rm_nvlink_ops_get_tl_link_mode,
        .set_tx_mode                = rm_nvlink_ops_set_link_tx_mode,
        .get_tx_mode                = rm_nvlink_ops_get_link_tx_mode,
        .set_rx_mode                = rm_nvlink_ops_set_link_rx_mode,
        .get_rx_mode                = rm_nvlink_ops_get_link_rx_mode,
        .set_rx_detect              = rm_nvlink_ops_set_link_rx_detect,
        .get_rx_detect              = rm_nvlink_ops_get_link_rx_detect,
        .write_discovery_token      = rm_nvlink_ops_write_link_discovery_token,
        .read_discovery_token       = rm_nvlink_ops_read_link_discovery_token,
        .training_complete          = rm_nvlink_ops_training_complete,
        .get_uphy_load              = rm_nvlink_get_uphy_load,
        .get_cci_link_mode          = rm_nvlink_ops_get_cci_link_mode,
        .ali_training               = rm_nvlink_ops_ali_training,
    };

    return &rm_nvlink_link_ops;
#else
    return NULL;
#endif
}

/*
 * @brief Verif only function to get the chiplib overrides for link connection
 * state for all NVLINKs.
 *
 * If chiplib overrides exist, each link can either be enabled (1) or disabled (0)
 *
 * @param[in]   pGpu                GPU object pointer
 * @param[in]   maxLinks            Size of pLinkConnection array
 * @param[out]  pLinkConnection     array of pLinkConnection values to be populated by MODS
 *
 * @return      NV_OK or NV_ERR_NOT_SUPPORTED (no overrides available)
 */
NV_STATUS
osGetForcedNVLinkConnection
(
    OBJGPU *pGpu,
    NvU32   maxLinks,
    NvU32   *pLinkConnection
)
{
    int i, ret;
    NV_STATUS status;
    char path[64];

    NV_ASSERT_OR_RETURN((pLinkConnection != NULL), NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN((maxLinks > 0), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN((pGpu != NULL), NV_ERR_INVALID_ARGUMENT);

    for (i = 0; i < maxLinks; i++)
    {
        ret = os_snprintf(path, sizeof(path), "CPU_MODEL|CM_ATS_ADDRESS|NVLink%u", i);
        NV_ASSERT((ret > 0) && (ret < (sizeof(path) - 1)));

        status = gpuSimEscapeRead(pGpu, path, 0, 4, &pLinkConnection[i]);
        if (status == NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "%s: %s=0x%X\n", __FUNCTION__,
                      path, pLinkConnection[i]);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "%s: gpuSimEscapeRead for '%s' failed (%u)\n",
                      __FUNCTION__, path, status);
            return NV_ERR_NOT_SUPPORTED;
        }
    }
    return NV_OK;
}

/*
 * @brief Get Platform suggested NVLink linerate
 *
 * NVLink will use this function to get the platform suggested linerate
 * if available in FRU or device tree.
 *
 * @param[in]   pGpu                GPU object pointer
 * @param[out]  NvU32 *             Suggested datarate
 *
 * @return      NV_OK or NV_ERR_NOT_SUPPORTED (platform linerate data not available)
 */
NV_STATUS
osGetPlatformNvlinkLinerate
(
    OBJGPU *pGpu,
    NvU32   *lineRate
)
{
#if defined(NVCPU_PPC64LE)
    nv_state_t *nv      = NV_GET_NV_STATE(pGpu);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    if (!pKernelNvlink)
        return NV_ERR_INVALID_ARGUMENT;

    return nv_get_nvlink_line_rate(nv, lineRate);
#else
    //TODO : FRU based method to be filled out by Bug 200285656
    //*lineRate = 0;
    //return NV_OK;
    return NV_ERR_NOT_SUPPORTED;
#endif
}

void
osSetNVLinkSysmemLinkState
(
    OBJGPU *pGpu,
    NvBool enabled
)
{
    nv_state_t *nv = NV_GET_NV_STATE(pGpu);
    NV_ASSERT(enabled);
    if (enabled)
        nv_dma_enable_nvlink(nv->dma_dev);
}
