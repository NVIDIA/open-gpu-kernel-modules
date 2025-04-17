/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/locks.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/ce/kernel_ce_private.h"
#include "gpu/eng_desc.h"
#include "gpu/mem_mgr/ce_utils.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/intr/intr_service.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/common_nvlink.h"
#include "vgpu/sdk-structures.h"
#include "nvrm_registry.h"

#include "gpu/spdm/spdm.h"
#include "gpu/conf_compute/conf_compute.h"
#include "ctrl/ctrl2080/ctrl2080spdm.h"
#include "gpu/conf_compute/ccsl.h"

#define IV_NUM_DWORDS 3

NV_STATUS kceConstructEngine_IMPL(OBJGPU *pGpu, KernelCE *pKCe, ENGDESCRIPTOR engDesc)
{
    NV_ASSERT_OR_RETURN(!RMCFG_FEATURE_PLATFORM_GSP, NV_ERR_NOT_SUPPORTED);

    NvU32 thisPublicID = GET_CE_IDX(engDesc);

    NV_PRINTF(LEVEL_INFO, "KernelCE: thisPublicID = %d\n", thisPublicID);

    pKCe->publicID = thisPublicID;
    pKCe->bShimOwner = NV_FALSE;

    pKCe->shimInstance = 0;
    kceSetShimInstance_HAL(pGpu, pKCe);

    pKCe->bIsAutoConfigEnabled = NV_TRUE;
    pKCe->bUseGen4Mapping = NV_FALSE;
    pKCe->ceCapsMask = 0;
    NvU32 data32 = 0;
    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_CE_ENABLE_AUTO_CONFIG, &data32) == NV_OK) &&
        (data32 == NV_REG_STR_RM_CE_ENABLE_AUTO_CONFIG_FALSE))
    {
        NV_PRINTF(LEVEL_INFO, "Disable CE Auto PCE-LCE Config\n");
        pKCe->bIsAutoConfigEnabled = NV_FALSE;
    }

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_CE_USE_GEN4_MAPPING, &data32) == NV_OK) &&
        (data32 == NV_REG_STR_RM_CE_USE_GEN4_MAPPING_TRUE))
    {
        NV_PRINTF(LEVEL_INFO, "GEN4 mapping will use a HSHUB PCE (if available) for PCIe!\n");
        pKCe->bUseGen4Mapping = NV_TRUE;
    }

    // OBJCE::isPresent would compute this first
    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu))
    {
        pGpu->numCEs++;
    }

    return NV_OK;
}

NvBool kceIsPresent_IMPL(OBJGPU *pGpu, KernelCE *pKCe)
{
    NvBool present = NV_FALSE;

    present = gpuCheckEngine_HAL(pGpu, ENG_CE(pKCe->publicID));

    NV_PRINTF(LEVEL_INFO, "KCE %d / %d: present=%d\n", pKCe->publicID,
        pGpu->numCEs > 0 ? pGpu->numCEs - 1 : pGpu->numCEs, present);

    return present;
}

NvBool kceIsNewMissingEngineRemovalSequenceEnabled_IMPL(OBJGPU *pGpu, KernelCE *pKCe)
{
    return NV_TRUE;
}

static NV_STATUS
spdmSendTestCommand
(
    OBJGPU       *pGpu,
    pCcslContext pCcslCtx,
    NvU8         *text,
    size_t       textSize,
    NvU8         *authTag,
    size_t       authTagSize,
    NvU32        *iv,
    size_t       ivSize,
    NvU8         *encData,
    size_t       encDataSize,
    NvU8         isEnc
)
{
    NV_STATUS           status   = NV_OK;
    RM_API              *pRmApi  = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    RMTIMEOUT           timeout;
    Spdm                *pSpdm = GPU_GET_SPDM(pGpu);

    NVC56F_CTRL_CMD_GET_KMB_PARAMS             getKmbParams = {0};
    NV2080_CTRL_INTERNAL_SPDM_PARTITION_PARAMS params = {0};
    status = pRmApi->Control(pRmApi, pCcslCtx->hClient, pCcslCtx->hChannel,
            NVC56F_CTRL_CMD_GET_KMB, &getKmbParams,
            sizeof(getKmbParams));

    NV_ASSERT_OK_OR_RETURN(status);

    if (pGpu == NULL || pSpdm == NULL || !pSpdm->bSessionEstablished)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    portMemCopy(&params.cmd.ccFipsTest.kmb, sizeof(params.cmd.ccFipsTest.kmb), &getKmbParams.kmb, sizeof(getKmbParams.kmb));

    if (!isEnc)
    {
        portMemCopy(&params.cmd.ccFipsTest.kmb.decryptBundle.iv, ivSize, iv, ivSize);
        portMemCopy(&params.cmd.ccFipsTest.text, textSize, text, textSize);
        portMemCopy(&params.cmd.ccFipsTest.authTag, authTagSize, authTag, authTagSize);
    }
    else
    {
        portMemCopy(&params.cmd.ccFipsTest.text, textSize, text, textSize);
    }

    params.cmd.cmdType          = RM_GSP_SPDM_CMD_ID_FIPS_SELFTEST;
    params.cmd.ccFipsTest.isEnc = isEnc;
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    status = spdmSendCtrlCall(pGpu, pSpdm, &params);

    NV_ASSERT_OK_OR_RETURN(status);

    // Perform basic validation of header returned.
    status = params.msg.status;

    NV_ASSERT_OK_OR_RETURN(status);

    if (isEnc)
    {
        portMemCopy(encData, encDataSize, params.cmd.ccFipsTest.text, encDataSize);
        portMemCopy(authTag, authTagSize, params.cmd.ccFipsTest.authTag, authTagSize);
    }

    return NV_OK;
}

static NV_STATUS
kceRunFipsSelfTestDecrypt
(
    OBJGPU   *pGpu,
    void     *pArg
)
{
    KernelCE          *pKCe              = pArg;
    MemoryManager     *pMemoryManager    = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMIGManager  *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    MEMORY_DESCRIPTOR *pSrcMemDesc       = NULL;
    MEMORY_DESCRIPTOR *pDstMemDesc       = NULL;
    MEMORY_DESCRIPTOR *pAuthMemDesc      = NULL;
    MEMORY_DESCRIPTOR *pIvMemDesc        = NULL;
    CeUtils           *pCeUtils          = NULL;
    pCcslContext       pCcslCtx          = NULL;
    NV_STATUS          status;
    NV0050_ALLOCATION_PARAMETERS ceUtilsParams = {0};
    CEUTILS_MEMCOPY_PARAMS params = {0};
    NvU32 Ivl[IV_NUM_DWORDS] = {0};

    NvU8 ceTestPlaintext[CE_FIPS_SELF_TEST_DATA_SIZE] = {
        0x2d, 0x71, 0xbc, 0xfa, 0x91, 0x4e, 0x4a, 0xc0,
        0x45, 0xb2, 0xaa, 0x60, 0x95, 0x5f, 0xad, 0x24
    };
    NvU8 encryptedData[CE_FIPS_SELF_TEST_DATA_SIZE] = { 0 };
    NvU8 dataAuth[CE_FIPS_SELF_TEST_AUTH_TAG_SIZE] = { 0 };

    NV_ASSERT_OR_RETURN(gpuIsCCFeatureEnabled(pGpu), NV_ERR_NOT_SUPPORTED);

    if (!gpuCheckEngineTable(pGpu, RM_ENGINE_TYPE_COPY(pKCe->publicID)) ||
        ceIsCeGrce(pGpu, RM_ENGINE_TYPE_COPY(pKCe->publicID)) ||
        kceIsDecompLce(pGpu, pKCe->publicID))
    {
        // CE doesn't support encryption
        return NV_OK;
    }

    if (kmigmgrIsMIGSupported(pGpu, pKernelMIGManager) &&
        kmigmgrGetStaticInfo(pGpu, pKernelMIGManager) == NULL)
    {
        // Wait for KernelMigManager, as it might remap CEs
        return NV_WARN_MORE_PROCESSING_REQUIRED;
    }

    NV_PRINTF(LEVEL_INFO, "Running FIPS test for CE%u\n", pKCe->publicID);

    ceUtilsParams.flags |= DRF_DEF(0050_CEUTILS, _FLAGS, _FORCE_CE_ID, _TRUE);
    ceUtilsParams.flags |= DRF_DEF(0050_CEUTILS, _FLAGS, _CC_SECURE, _TRUE);
    ceUtilsParams.forceCeId = pKCe->publicID;

    NV_ASSERT_OK_OR_GOTO(status,
        objCreate(&pCeUtils, pMemoryManager, CeUtils, ENG_GET_GPU(pMemoryManager), NULL, &ceUtilsParams), failed);

    NV_ASSERT_OK_OR_GOTO(status,
        ccslContextInitViaChannel_HAL(&pCcslCtx, pCeUtils->pChannel->hClient,
                                      pCeUtils->pChannel->subdeviceId,
                                      pCeUtils->pChannel->channelId),
        failed);

    NV_ASSERT_OK_OR_GOTO(status, memdescCreate(&pSrcMemDesc, pGpu, sizeof ceTestPlaintext, 0, NV_TRUE, ADDR_FBMEM,
                         NV_MEMORY_UNCACHED, MEMDESC_ALLOC_FLAGS_PROTECTED), failed);
    NV_ASSERT_OK_OR_GOTO(status, memdescAlloc(pSrcMemDesc), failed);

    NV_ASSERT_OK_OR_GOTO(status, memdescCreate(&pDstMemDesc, pGpu, sizeof encryptedData, 0, NV_TRUE, ADDR_SYSMEM,
                         NV_MEMORY_UNCACHED, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY), failed);
    NV_ASSERT_OK_OR_GOTO(status, memdescAlloc(pDstMemDesc), failed);

    NV_ASSERT_OK_OR_GOTO(status, memdescCreate(&pAuthMemDesc, pGpu, sizeof dataAuth, 0, NV_TRUE, ADDR_SYSMEM,
                         NV_MEMORY_UNCACHED, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY), failed);
    NV_ASSERT_OK_OR_GOTO(status, memdescAlloc(pAuthMemDesc), failed);

    NV_ASSERT_OK_OR_GOTO(status, memdescCreate(&pIvMemDesc, pGpu, CE_FIPS_SELF_TEST_IV_SIZE, 0, NV_TRUE, ADDR_SYSMEM,
                         NV_MEMORY_UNCACHED, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY), failed);
    NV_ASSERT_OK_OR_GOTO(status, memdescAlloc(pIvMemDesc), failed);

    TRANSFER_SURFACE srcSurface  = { .pMemDesc = pSrcMemDesc,  .offset = 0 };
    TRANSFER_SURFACE dstSurface  = { .pMemDesc = pDstMemDesc,  .offset = 0 };
    TRANSFER_SURFACE authSurface = { .pMemDesc = pAuthMemDesc, .offset = 0 };
    TRANSFER_SURFACE ivSurface   = { .pMemDesc = pIvMemDesc,   .offset = 0 };

    // Write data to allocations, encrypt using CE, and read back the results
    NV_ASSERT_OK_OR_GOTO(status, memmgrMemDescMemSet(pMemoryManager, pDstMemDesc, 0, 0), failed);
    NV_ASSERT_OK_OR_GOTO(status, memmgrMemDescMemSet(pMemoryManager, pAuthMemDesc, 0, 0), failed);
    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemWrite(pMemoryManager, &srcSurface, ceTestPlaintext, sizeof ceTestPlaintext, TRANSFER_FLAGS_NONE),
        failed);

    params.bSecureCopy    = NV_TRUE;
    params.authTagAddr    = memdescGetPhysAddr(pAuthMemDesc, AT_GPU, 0);
    params.encryptIvAddr  = memdescGetPhysAddr(pIvMemDesc, AT_GPU, 0);
    params.pDstMemDesc    = pDstMemDesc;
    params.dstOffset      = 0;
    params.pSrcMemDesc    = pSrcMemDesc;
    params.srcOffset      = 0;
    params.length         = sizeof ceTestPlaintext;
    params.bEncrypt       = NV_TRUE;
    NV_ASSERT_OK_OR_GOTO(status, ceutilsMemcopy(pCeUtils, &params), failed);

    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemRead(pMemoryManager, &dstSurface, encryptedData, sizeof encryptedData, TRANSFER_FLAGS_NONE), failed);
    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemRead(pMemoryManager, &authSurface, dataAuth, sizeof dataAuth, TRANSFER_FLAGS_NONE), failed);

    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemRead(pMemoryManager, &ivSurface, Ivl, sizeof Ivl, TRANSFER_FLAGS_NONE), failed);

    NV_ASSERT_OK_OR_GOTO(status, spdmSendTestCommand(pGpu, pCcslCtx, encryptedData, sizeof encryptedData,
                       dataAuth, sizeof dataAuth, Ivl, sizeof Ivl, NULL, 0, NV_FALSE), failed);

failed:
    ccslContextClear(pCcslCtx);
    objDelete(pCeUtils);
    memdescFree(pSrcMemDesc);
    memdescDestroy(pSrcMemDesc);
    memdescFree(pDstMemDesc);
    memdescDestroy(pDstMemDesc);
    memdescFree(pAuthMemDesc);
    memdescDestroy(pAuthMemDesc);
    memdescFree(pIvMemDesc);
    memdescDestroy(pIvMemDesc);

    NV_PRINTF(LEVEL_INFO, "Test finished with status 0x%x\n", status);

    return status;
}

static NV_STATUS
kceRunFipsSelfTestEncrypt
(
    OBJGPU   *pGpu,
    void     *pArg
)
{
    KernelCE          *pKCe              = pArg;
    MemoryManager     *pMemoryManager    = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMIGManager  *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    MEMORY_DESCRIPTOR *pSrcMemDesc       = NULL;
    MEMORY_DESCRIPTOR *pDstMemDesc       = NULL;
    MEMORY_DESCRIPTOR *pAuthMemDesc      = NULL;
    MEMORY_DESCRIPTOR *pIvMemDesc        = NULL;
    CeUtils           *pCeUtils          = NULL;
    pCcslContext       pCcslCtx          = NULL;
    NV_STATUS          status;
    NV0050_ALLOCATION_PARAMETERS ceUtilsParams = {0};
    CEUTILS_MEMCOPY_PARAMS params = {0};
    NvU32 Ivl[IV_NUM_DWORDS] = {0};

    NvU8 ceTestPlaintext[CE_FIPS_SELF_TEST_DATA_SIZE] = {
        0x2d, 0x71, 0xbc, 0xfa, 0x91, 0x4e, 0x4a, 0xc0,
        0x45, 0xb2, 0xaa, 0x60, 0x95, 0x5f, 0xad, 0x24
    };
    NvU8 decryptedData[CE_FIPS_SELF_TEST_DATA_SIZE] = { 0 };
    NvU8 encryptedData[CE_FIPS_SELF_TEST_DATA_SIZE] = { 0 };
    NvU8 dataAuth[CE_FIPS_SELF_TEST_AUTH_TAG_SIZE]  = { 0 };

    NV_ASSERT_OR_RETURN(gpuIsCCFeatureEnabled(pGpu), NV_ERR_NOT_SUPPORTED);

    if (!gpuCheckEngineTable(pGpu, RM_ENGINE_TYPE_COPY(pKCe->publicID)) ||
        ceIsCeGrce(pGpu, RM_ENGINE_TYPE_COPY(pKCe->publicID)))
    {
        // CE doesn't support encryption
        return NV_OK;
    }

    if (kmigmgrIsMIGSupported(pGpu, pKernelMIGManager) &&
        kmigmgrGetStaticInfo(pGpu, pKernelMIGManager) == NULL)
    {
        // Wait for KernelMigManager, as it might remap CEs
        return NV_WARN_MORE_PROCESSING_REQUIRED;
    }

    NV_PRINTF(LEVEL_INFO, "Running FIPS test for CE%u\n", pKCe->publicID);

    ceUtilsParams.flags |= DRF_DEF(0050_CEUTILS, _FLAGS, _FORCE_CE_ID, _TRUE);
    ceUtilsParams.flags |= DRF_DEF(0050_CEUTILS, _FLAGS, _CC_SECURE, _TRUE);
    ceUtilsParams.forceCeId = pKCe->publicID;

    NV_ASSERT_OK_OR_GOTO(status,
        objCreate(&pCeUtils, pMemoryManager, CeUtils, ENG_GET_GPU(pMemoryManager), NULL, &ceUtilsParams), failed);

    NV_ASSERT_OK_OR_GOTO(status,
        ccslContextInitViaChannel_HAL(&pCcslCtx, pCeUtils->pChannel->hClient,
                                      pCeUtils->pChannel->subdeviceId,
                                      pCeUtils->pChannel->channelId),
        failed);

    NV_ASSERT_OK_OR_GOTO(status, memdescCreate(&pSrcMemDesc, pGpu, sizeof ceTestPlaintext, 0, NV_TRUE, ADDR_FBMEM,
                         NV_MEMORY_UNCACHED, MEMDESC_ALLOC_FLAGS_PROTECTED), failed);
    NV_ASSERT_OK_OR_GOTO(status, memdescAlloc(pSrcMemDesc), failed);

    NV_ASSERT_OK_OR_GOTO(status, memdescCreate(&pDstMemDesc, pGpu, sizeof encryptedData, 0, NV_TRUE, ADDR_SYSMEM,
                         NV_MEMORY_UNCACHED, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY), failed);
    NV_ASSERT_OK_OR_GOTO(status, memdescAlloc(pDstMemDesc), failed);

    NV_ASSERT_OK_OR_GOTO(status, memdescCreate(&pAuthMemDesc, pGpu, sizeof dataAuth, 0, NV_TRUE, ADDR_SYSMEM,
                         NV_MEMORY_UNCACHED, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY), failed);
    NV_ASSERT_OK_OR_GOTO(status, memdescAlloc(pAuthMemDesc), failed);

    NV_ASSERT_OK_OR_GOTO(status, memdescCreate(&pIvMemDesc, pGpu, sizeof Ivl, 0, NV_TRUE, ADDR_SYSMEM,
                         NV_MEMORY_UNCACHED, MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY), failed);
    NV_ASSERT_OK_OR_GOTO(status, memdescAlloc(pIvMemDesc), failed);

    TRANSFER_SURFACE srcSurface  = { .pMemDesc = pSrcMemDesc,  .offset = 0 };
    TRANSFER_SURFACE dstSurface  = { .pMemDesc = pDstMemDesc,  .offset = 0 };
    TRANSFER_SURFACE authSurface = { .pMemDesc = pAuthMemDesc, .offset = 0 };
    TRANSFER_SURFACE ivSurface = { .pMemDesc = pIvMemDesc, .offset = 0 };

    // Write data to allocations, encrypt using CE, and read back the results
    NV_ASSERT_OK_OR_GOTO(status, memmgrMemDescMemSet(pMemoryManager, pDstMemDesc, 0, 0), failed);
    NV_ASSERT_OK_OR_GOTO(status, memmgrMemDescMemSet(pMemoryManager, pAuthMemDesc, 0, 0), failed);
    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemWrite(pMemoryManager, &srcSurface, ceTestPlaintext, sizeof ceTestPlaintext, TRANSFER_FLAGS_NONE),
        failed);

    params.bSecureCopy    = NV_TRUE;
    params.authTagAddr    = memdescGetPhysAddr(pAuthMemDesc, AT_GPU, 0);
    params.encryptIvAddr  = memdescGetPhysAddr(pIvMemDesc, AT_GPU, 0);
    params.pDstMemDesc    = pDstMemDesc;
    params.dstOffset      = 0;
    params.pSrcMemDesc    = pSrcMemDesc;
    params.srcOffset      = 0;
    params.length         = sizeof ceTestPlaintext;
    params.bEncrypt       = NV_TRUE;
    NV_ASSERT_OK_OR_GOTO(status, ceutilsMemcopy(pCeUtils, &params), failed);

    NV_ASSERT_OK_OR_GOTO(status, spdmSendTestCommand(pGpu, pCcslCtx, ceTestPlaintext, sizeof ceTestPlaintext, dataAuth,
                       sizeof dataAuth, Ivl, sizeof Ivl, encryptedData, sizeof encryptedData, NV_TRUE), failed);

    // Write data to allocations, decrypt using CE, read back, and validate
    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemWrite(pMemoryManager, &dstSurface, encryptedData, sizeof encryptedData, TRANSFER_FLAGS_NONE), failed);
    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemWrite(pMemoryManager, &authSurface, dataAuth, sizeof dataAuth, TRANSFER_FLAGS_NONE), failed);
    NV_ASSERT_OK_OR_GOTO(status, memmgrMemDescMemSet(pMemoryManager, pSrcMemDesc, 0, 0), failed);
    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemRead(pMemoryManager, &ivSurface, Ivl, sizeof Ivl, TRANSFER_FLAGS_NONE), failed);

    params.pDstMemDesc = pSrcMemDesc;
    params.dstOffset   = 0;
    params.pSrcMemDesc = pDstMemDesc;
    params.srcOffset   = 0;
    params.length      = sizeof ceTestPlaintext;
    params.bEncrypt    = NV_FALSE;
    NV_ASSERT_OK_OR_GOTO(status, ceutilsMemcopy(pCeUtils, &params), failed);

    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemRead(pMemoryManager, &srcSurface, decryptedData, sizeof decryptedData, TRANSFER_FLAGS_NONE), failed);

    NV_ASSERT_TRUE_OR_GOTO(status, portMemCmp(decryptedData, ceTestPlaintext, sizeof ceTestPlaintext) == 0,
        NV_ERR_INVALID_STATE, failed);

failed:
    ccslContextClear(pCcslCtx);
    objDelete(pCeUtils);
    memdescFree(pSrcMemDesc);
    memdescDestroy(pSrcMemDesc);
    memdescFree(pDstMemDesc);
    memdescDestroy(pDstMemDesc);
    memdescFree(pAuthMemDesc);
    memdescDestroy(pAuthMemDesc);
    memdescFree(pIvMemDesc);
    memdescDestroy(pIvMemDesc);

    NV_PRINTF(LEVEL_INFO, "Test finished with status 0x%x\n", status);

    return status;
}

NV_STATUS
kceRunFipsSelfTest
(
    OBJGPU   *pGpu,
    void     *pArg
)
{
    NV_STATUS status = NV_OK;
    status = kceRunFipsSelfTestDecrypt(pGpu, pArg);
    if (status == NV_OK)
    {
        return kceRunFipsSelfTestEncrypt(pGpu, pArg);
    }
    return status;
}

NV_STATUS
kceStateInitLocked_IMPL
(
    OBJGPU   *pGpu,
    KernelCE *pKCe
)
{
    if (!gpuIsCCFeatureEnabled(pGpu) || !IS_SILICON(pGpu))
    {
        pKCe->bCcFipsSelfTestRequired = NV_FALSE;
    }

    if (pKCe->bCcFipsSelfTestRequired)
    {
        NV_ASSERT_OK_OR_RETURN(
            kfifoAddSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu), kceRunFipsSelfTest, pKCe, NULL, NULL));
    }

    return NV_OK;
}

void
kceStateDestroy_IMPL
(
    OBJGPU   *pGpu,
    KernelCE *pKCe
)
{
    if (pKCe->bCcFipsSelfTestRequired)
    {
        kfifoRemoveSchedulingHandler(pGpu, GPU_GET_KERNEL_FIFO(pGpu), kceRunFipsSelfTest, pKCe, NULL, NULL);
    }
}

static void printCaps(OBJGPU *pGpu, KernelCE *pKCe, RM_ENGINE_TYPE rmEngineType, const NvU8 *capsTbl)
{
    NV_PRINTF(LEVEL_INFO, "LCE%d caps (engineType = %d (%d))\n", pKCe->publicID,
                           gpuGetNv2080EngineType(rmEngineType), rmEngineType);
#define PRINT_CAP(cap) NV_PRINTF(LEVEL_INFO, #cap ":%d\n", (RMCTRL_GET_CAP(capsTbl, NV2080_CTRL_CE_CAPS, cap) != 0) ? 1 : 0)

    PRINT_CAP(_CE_GRCE);
    PRINT_CAP(_CE_SHARED);
    PRINT_CAP(_CE_SYSMEM_READ);
    PRINT_CAP(_CE_SYSMEM_WRITE);
    PRINT_CAP(_CE_NVLINK_P2P);
    PRINT_CAP(_CE_SYSMEM);
    PRINT_CAP(_CE_P2P);
    PRINT_CAP(_CE_BL_SIZE_GT_64K_SUPPORTED);
    PRINT_CAP(_CE_SUPPORTS_NONPIPELINED_BL);
    PRINT_CAP(_CE_SUPPORTS_PIPELINED_BL);
    PRINT_CAP(_CE_CC_SECURE);
    PRINT_CAP(_CE_DECOMP_SUPPORTED);
}

void kceGetNvlinkCaps_IMPL(OBJGPU *pGpu, KernelCE *pKCe, NvU8 *pKCeCaps)
{
    if (kceIsCeSysmemRead_HAL(pGpu, pKCe))
        RMCTRL_SET_CAP(pKCeCaps, NV2080_CTRL_CE_CAPS, _CE_SYSMEM_READ);

    if (kceIsCeSysmemWrite_HAL(pGpu, pKCe))
        RMCTRL_SET_CAP(pKCeCaps, NV2080_CTRL_CE_CAPS, _CE_SYSMEM_WRITE);

    if (kceIsCeNvlinkP2P_HAL(pGpu, pKCe))
        RMCTRL_SET_CAP(pKCeCaps, NV2080_CTRL_CE_CAPS, _CE_NVLINK_P2P);
}

NV_STATUS kceGetDeviceCaps_IMPL(OBJGPU *pGpu, KernelCE *pKCe, RM_ENGINE_TYPE rmEngineType, NvU8 *pKCeCaps)
{
    if (pKCe->bStubbed)
    {
        NV_PRINTF(LEVEL_INFO, "Skipping stubbed CE %d\n", pKCe->publicID);
        return NV_ERR_NOT_SUPPORTED;
    }

    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    //
    // Since some CE capabilities depend on the nvlink topology,
    // trigger topology detection before updating the CE caps
    //
    if ((pKernelNvlink != NULL) && !knvlinkIsForcedConfig(pGpu, pKernelNvlink) &&
         kmigmgrIsMIGNvlinkP2PSupported(pGpu, GPU_GET_KERNEL_MIG_MANAGER(pGpu)))
    {
        knvlinkCoreGetRemoteDeviceInfo(pGpu, pKernelNvlink);
    }

    portMemSet(pKCeCaps, 0, NV2080_CTRL_CE_CAPS_TBL_SIZE);

    NV2080_CTRL_CE_GET_CAPS_V2_PARAMS physicalCaps;
    portMemSet(&physicalCaps, 0, sizeof(physicalCaps));

    physicalCaps.ceEngineType = NV2080_ENGINE_TYPE_COPY(pKCe->publicID);
    NV_PRINTF(LEVEL_INFO, "Querying caps for LCE(%d)\n", pKCe->publicID);

    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_CE_GET_PHYSICAL_CAPS,
                            &physicalCaps,
                            sizeof(physicalCaps)));

    portMemCopy(pKCeCaps,
                NV2080_CTRL_CE_CAPS_TBL_SIZE,
                physicalCaps.capsTbl,
                NV2080_CTRL_CE_CAPS_TBL_SIZE);

    kceAssignCeCaps_HAL(pGpu, pKCe, pKCeCaps);
    printCaps(pGpu, pKCe, rmEngineType, pKCeCaps);

    return NV_OK;
}

/*!
 * Determine appropriate CEs for sysmem read/write and P2P over NVLINK.
 *
 * @param[in]   pGpu           OBJGPU pointer
 * @param[in]   pKCe            OBJCE pointer
 * @param[in]   gpuMask        Mask of GPUs for determining P2P CEs
 * @param[out]  pSysmemReadCE  Pointer to fill out the LCE for sysmem read
 * @param[out]  pSysmemWriteCE Pointer to fill out the LCE for sysmem write
 * @param[out]  pP2PCE         Pointer to fill out the LCE for P2P
 */
NV_STATUS
kceGetCeFromNvlinkConfig_IMPL
(
    OBJGPU   *pGpu,
    KernelCE *pKCe,
    NvU32     gpuMask,
    NvU32    *pSysmemReadCE,
    NvU32    *pSysmemWriteCE,
    NvU32    *nvlinkP2PCeMask
)
{
    NV_STATUS  rmStatus;
    NvU32      gpuCount;
    NV2080_CTRL_CMD_NVLINK_GET_NVLINK_CAPS_PARAMS nvlinkCapsParams = {0};
    NvU8      *nvlinkCaps;

    gpuCount = gpumgrGetSubDeviceCount(gpuMask);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, !IsGP100(pGpu) || gpuCount <= 2, NV_ERR_INVALID_STATE);

    rmStatus = nvlinkCtrlCmdBusGetNvlinkCaps(pGpu, &nvlinkCapsParams);
    NV_ASSERT_OK_OR_RETURN(rmStatus);

    nvlinkCaps = (NvU8*)&nvlinkCapsParams.capsTbl;

    // Check if GPU supports NVLink
    if (NV2080_CTRL_NVLINK_GET_CAP(nvlinkCaps, NV2080_CTRL_NVLINK_CAPS_SUPPORTED))
    {
        // Check if GPU supports NVLink for SYSMEM
        if (NV2080_CTRL_NVLINK_GET_CAP(nvlinkCaps, NV2080_CTRL_NVLINK_CAPS_SYSMEM_ACCESS))
            kceGetSysmemRWLCEs(pGpu, pKCe, pSysmemReadCE, pSysmemWriteCE);

        // Check if GPU supports NVLink for P2P
        if (NV2080_CTRL_NVLINK_GET_CAP(nvlinkCaps, NV2080_CTRL_NVLINK_CAPS_P2P_SUPPORTED))
            rmStatus = kceGetP2PCes(pKCe, pGpu, gpuMask, nvlinkP2PCeMask);
    }

    return rmStatus;
}

NV_STATUS kceUpdateClassDB_KERNEL(OBJGPU *pGpu, KernelCE *pKCe)
{
    RM_API *pRmApi     = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV2080_CTRL_CE_UPDATE_CLASS_DB_PARAMS params = {0};

    NV_STATUS status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_CE_UPDATE_CLASS_DB,
                             &params,
                             sizeof(params));
    NV_ASSERT_OK_OR_RETURN(status);

    // For each LCE, check if it is stubbed out in GSP-RM
    KCE_ITER_ALL_BEGIN(pGpu, pKCe, 0)
        NvBool stubbed = ((BIT(kceInst) & params.stubbedCeMask)) != 0;

        // If this CE has no PCEs assigned, remove it from classDB
        if (stubbed)
        {
            NV_PRINTF(LEVEL_INFO, "Stubbing KCE %d\n", kceInst);
            pKCe->bStubbed = NV_TRUE;

            status = gpuDeleteClassFromClassDBByEngTag(pGpu, ENG_CE(kceInst));
        }
        else
        {
            // If a new CE needs to be added because of the new mappings
            NV_PRINTF(LEVEL_INFO, "Unstubbing KCE %d\n", kceInst);
            pKCe->bStubbed = NV_FALSE;

            status = gpuAddClassToClassDBByEngTag(pGpu, ENG_CE(kceInst));
        }
    KCE_ITER_END

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, gpuUpdateEngineTable(pGpu));

    return NV_OK;
}

/**
 * @brief Provides an opportunity to register some IntrService during intrStateInit.
 */
void
kceRegisterIntrService_IMPL
(
    OBJGPU            *pGpu,
    KernelCE          *pKCe,
    IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX]
)
{
    NvU16 engineIdx = MC_ENGINE_IDX_CE(pKCe->publicID);

    NV_ASSERT(pRecords[engineIdx].pNotificationService == NULL);
    pRecords[engineIdx].bFifoWaiveNotify = NV_FALSE;
    pRecords[engineIdx].pNotificationService = staticCast(pKCe, IntrService);
}

/**
 * @brief Services the nonstall interrupt.
 *
 * @param[in] pGpu
 * @param[in] pKCe    The IntrService object registered to handle the engineIdx nonstall interrupt.
 * @param[in] pParams
 *
 */
NV_STATUS
kceServiceNotificationInterrupt_IMPL
(
    OBJGPU            *pGpu,
    KernelCE          *pKCe,
    IntrServiceServiceNotificationInterruptArguments *pParams
)
{
    NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pParams->engineIdx == MC_ENGINE_IDX_CE(pKCe->publicID), NV_ERR_GENERIC);

    NV_PRINTF(LEVEL_INFO, "for CE%d\n", pKCe->publicID);

    MODS_ARCH_REPORT(NV_ARCH_EVENT_NONSTALL_CE, "%s", "processing CE nonstall interrupt\n");

    kceNonstallIntrCheckAndClear_HAL(pGpu, pKCe, pParams->pThreadState);

    // Wake up channels waiting on this event
    engineNonStallIntrNotify(pGpu, RM_ENGINE_TYPE_COPY(pKCe->publicID));

    return NV_OK;
}

NV_STATUS kceTopLevelPceLceMappingsUpdate_IMPL(OBJGPU *pGpu, KernelCE *pKCe)
{
    NvU32        pceLceMap[NV2080_CTRL_MAX_PCES]    = {0};
    NvU32        grceConfig[NV2080_CTRL_MAX_GRCES]  = {0};
    NvU32        exposeCeMask        = 0;
    NvBool       bUpdateNvlinkPceLce = NV_FALSE;
    NV_STATUS    status              = NV_OK;
    NvU32        i;

    //
    // Sync class DB before proceeding with the algorithm.
    // This is needed if mapping update previously originated in physical RM
    //
    NV_ASSERT_OK_OR_RETURN(kceUpdateClassDB_HAL(pGpu, pKCe));

    for (i = 0; i < NV2080_CTRL_MAX_PCES; i++)
        pceLceMap[i] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;

    for (i = 0; i < NV2080_CTRL_MAX_GRCES; i++)
        grceConfig[i] = NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_INVALID_LCE;

    //
    // If not GSP-RM, get the auto-config PCE-LCE mappings for NVLink topology.
    // This should work fine on CPU-RM and monolithic RM.
    //

    // Set bUpdateNvlinkPceLce to auto-config status
    bUpdateNvlinkPceLce = pKCe->bIsAutoConfigEnabled;

    if (bUpdateNvlinkPceLce || IS_SILICON(pGpu))
    {
        status = kceGetNvlinkAutoConfigCeValues_HAL(pGpu, pKCe, pceLceMap,
                                               grceConfig, &exposeCeMask);
        if (status == NV_ERR_NOT_SUPPORTED)
        {
            NV_PRINTF(LEVEL_INFO,
                "CE AutoConfig is not supported. Skipping PCE2LCE update\n");

            bUpdateNvlinkPceLce = NV_FALSE;
        }
        else
        {
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                    "Failed to get auto-config PCE-LCE mappings. Return\n");
                return status;
            }
        }
    }

    cePauseCeUtilsScheduling(pGpu);

    //
    // Pass these values to the ceUpdatePceLceMappings_HAL.
    //
    // bUpdateNvlinkPceLce will have the following values:
    //     a. NV_FALSE when auto-config is disabled through regkey.
    //     b. NV_FALSE when NVLink does not exist or NVLink topology is forced.
    //        ceUpdatePceLceMappings_HAL still have to be called because PCE-LCE
    //        mappings can be updated for reasons other than NVLink topology.
    //     c. NV_TRUE when (a) and (b) does not hold true and platform is CPU-RM
    //        or monolithic RM. For GSP-RM, value is NV_FALSE because GSP-RM does
    //        not store the NVLink topology.
    //
    // exposeCeMask will be 0x0 when bUpdateNvlinkPceLce is NV_FALSE.
    //
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_CE_UPDATE_PCE_LCE_MAPPINGS_V2_PARAMS params = {0};

    if (bUpdateNvlinkPceLce)
    {
        portMemCopy(params.pceLceMap,
                    sizeof(params.pceLceMap),
                    pceLceMap,
                    sizeof(pceLceMap));

        portMemCopy(params.grceConfig,
                    sizeof(params.grceConfig),
                    grceConfig,
                    sizeof(grceConfig));
    }

    params.exposeCeMask = exposeCeMask;
    params.bUpdateNvlinkPceLce = bUpdateNvlinkPceLce;

    NV_ASSERT_OK_OR_RETURN(
        rmapiControlCacheFreeForControl(gpuGetInstance(pGpu),
                                        NV2080_CTRL_CMD_CE_GET_CE_PCE_MASK));

    // For GSP clients, the update needs to be routed through ctrl call
    params.shimInstance = pKCe->shimInstance;
    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_CE_UPDATE_PCE_LCE_MAPPINGS_V2,
                             &params,
                             sizeof(params));

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Failed to update PCE-LCE mappings. Return\n");
        return status;
    }

    //
    // After the mappings are updated, LCEs which do not have PCEs mapped need
    // to be stubbed out and LCEs which have PCEs need to be un-stubbed. This
    // happens as a part of ceUpdatePceLceMappings_HAL which gets executed in
    // GSP/monolithic RM. For CPU-RM, have to call this function explicitly.
    //
    status = kceUpdateClassDB_HAL(pGpu, pKCe);

    ceResumeCeUtilsScheduling(pGpu);

    return status;
}

NV_STATUS kceGetFaultMethodBufferSize_IMPL(OBJGPU *pGpu, NvU32 *size)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_CE_GET_FAULT_METHOD_BUFFER_SIZE_PARAMS params = {0};

    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

        NV_ASSERT_OR_RETURN(pVSI, NV_ERR_INVALID_STATE);

        *size = pVSI->ceFaultMethodBufferDepth;

        return NV_OK;
    }

    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
            NV2080_CTRL_CMD_CE_GET_FAULT_METHOD_BUFFER_SIZE, &params, sizeof(params)));

    *size = params.size;
    return NV_OK;
}

/*!
 * Gets the HSHUB/FBHUB PCE masks and updates NVLINK_TOPOLOGY_PARAMS.
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  pTopoParams  Pointer to NVLINK_TOPOLOGY_PARAMS
 *
 * Returns NV_OK if array is filled successfully.
 *         NV_ERR_INVALID_ARGUMENT if pTopoParams is NULL or aray size is not equal.
 */
NV_STATUS
kceGetAvailableHubPceMask_IMPL
(
    OBJGPU *pGpu,
    KernelCE *pKCe,
    NVLINK_TOPOLOGY_PARAMS *pTopoParams
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_ASSERT_OR_RETURN(pTopoParams != NULL, NV_ERR_INVALID_ARGUMENT);

    NV2080_CTRL_CE_GET_HUB_PCE_MASK_V2_PARAMS params = {0};
    params.shimInstance = pKCe->shimInstance;

    ct_assert(NV_ARRAY_ELEMENTS(pTopoParams->pceAvailableMaskPerConnectingHub) ==
              NV_ARRAY_ELEMENTS(params.connectingHubPceMasks));
    NV_ASSERT_OK_OR_RETURN(
        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_CE_GET_HUB_PCE_MASK_V2,
                        &params,
                        sizeof(params))
    );

    portMemCopy(pTopoParams->pceAvailableMaskPerConnectingHub,
                sizeof(pTopoParams->pceAvailableMaskPerConnectingHub),
                params.connectingHubPceMasks,
                sizeof(pTopoParams->pceAvailableMaskPerConnectingHub));

    pTopoParams->fbhubPceMask = params.fbhubPceMask;
    return NV_OK;
}

/*!
 * @brief return first non-NULL KCE instance
 */
NV_STATUS
kceFindFirstInstance_IMPL(OBJGPU *pGpu, KernelCE **ppKCe)
{
    KernelCE *pKCe = NULL;

    KCE_ITER_ALL_BEGIN(pGpu, pKCe, 0)
        *ppKCe = pKCe;
        return NV_OK;
    KCE_ITER_END

    return NV_ERR_INSUFFICIENT_RESOURCES;
}

/*!
 * @brief Find shim owner KernelCE object
 */
NV_STATUS
kceFindShimOwner_IMPL
(
    OBJGPU    *pGpu,
    KernelCE  *pKCe,
    KernelCE **ppShimKCe
)
{
    KernelCE *pKCeLoop;

    KCE_ITER_BEGIN(pGpu, pKCe, pKCeLoop, 0)
        if (pKCeLoop->bShimOwner && pKCeLoop->shimInstance == pKCe->shimInstance)
        {
            *ppShimKCe = pKCeLoop;
            return NV_OK;
        }
    KCE_ITER_END

    return NV_ERR_INSUFFICIENT_RESOURCES;
}

/**
 * Return as mask of LCEs available
 *
 * @param[in]   pGpu     OBJGPU pointer
 *
 * @return Mask of all available LCEs
 */
NvU32
kceGetLceMask_IMPL
(
    OBJGPU *pGpu
)
{
    NvU32 lceAvailableMask = 0;
    NvU32 i;

    for (i = 0; i < GPU_MAX_CES; i++)
    {
        KernelCE *pKCe = GPU_GET_KCE(pGpu, i);
        if (pKCe != NULL)
        {
            lceAvailableMask |= NVBIT(i);
        }
    }

    return lceAvailableMask;
}

/**
 * Returns the PCE config for the specified LCE type.
 *
 * @param[in] pGpu                  OBJGPU pointer
 * @param[in] pKCe                  CE object pointer
 * @param[in] lceType               LCE type
 * @param[out] pNumPcesPerLce       Number of PCEs per LCE
 * @param[out] pNumLces             Number of LCEs
 * @param[out] pSupportedPceMask    Mask of PCEs that support the specified LCE type
 * @param[out] pSupportedLceMask    Mask of LCEs that support the specified LCE type
 * @param[out] pPcesPerHshub        Number of PCEs that can be assigned per HSHUB for the given LCE type
 *
 * @return NV_OK
 */
NV_STATUS
kceGetPceConfigForLceType_IMPL
(
    OBJGPU      *pGpu,
    KernelCE    *pKCe,
    NvU32       lceType,
    NvU32       *pNumPcesPerLce,
    NvU32       *pNumLces,
    NvU32       *pSupportedPceMask,
    NvU32       *pSupportedLceMask,
    NvU32       *pPcesPerHshub
)
{
    RM_API   *pRmApi    = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV_ASSERT_OR_RETURN(pNumPcesPerLce != NULL,     NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pNumLces != NULL,           NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pSupportedPceMask != NULL,  NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pSupportedLceMask != NULL,  NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pPcesPerHshub != NULL,      NV_ERR_INVALID_ARGUMENT);

    NV2080_CTRL_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE_PARAMS pceConfigParams;
    portMemSet(&pceConfigParams, 0, sizeof(pceConfigParams));
    pceConfigParams.lceType = lceType;

    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                           pGpu->hInternalClient,
                                           pGpu->hInternalSubdevice,
                                           NV2080_CTRL_CMD_INTERNAL_CE_GET_PCE_CONFIG_FOR_LCE_TYPE,
                                           &pceConfigParams,
                                           sizeof(pceConfigParams)));

    *pNumPcesPerLce       = pceConfigParams.numPces;
    *pNumLces             = pceConfigParams.numLces;
    *pSupportedPceMask    = pceConfigParams.supportedPceMask;
    *pSupportedLceMask    = pceConfigParams.supportedLceMask;
    *pPcesPerHshub        = pceConfigParams.pcePerHshub;

    return NV_OK;
}

/*!
 * Checks whether given LCE index is enabled for decompression workloads
 *
 * @note When MIG is active this always expect local LCEs to be passed in. The translation from local
 * to global Ids is to be handled internally.
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  lceIndex     LCE Index to be checked for Decomp Cap.
 *
 * Returns NV_TRUE if the LCE index can accept decomp workloads from clients
 *         NV_FALSE if decomp workloads are not supported on this LCE
 */
NvBool
kceIsDecompLce_IMPL
(
    OBJGPU   *pGpu,
    NvU32    lceIndex
)
{
    NV2080_CTRL_CE_IS_DECOMP_LCE_ENABLED_PARAMS params = {0};
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status = NV_OK;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RS_RES_CONTROL_PARAMS_INTERNAL *pControlParams = pCallContext->pControlParams;

    params.lceIndex = lceIndex;

    status = pRmApi->Control(pRmApi,
                             pControlParams->hClient,
                             pControlParams->hObject,
                             NV2080_CTRL_CMD_CE_IS_DECOMP_LCE_ENABLED,
                             &params,
                             sizeof(params));

    if (status != NV_OK)
        params.bDecompEnabled = NV_FALSE;

    return params.bDecompEnabled;
}

/*!
 * Checks whether given LCE index is enabled for decompression workloads
 * based on the presence of the CE_DECOMP_SUPPORTED capability. Reads the
 * capability from the static data.
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  lceIndex     LCE Index to be checked for Decomp Cap.
 *
 * Returns NV_TRUE if the LCE index can accept decomp workloads from clients
 *         NV_FALSE if decomp workloads are not supported on this LCE
 */
NvBool
kceIsDecompLce_VF
(
    OBJGPU   *pGpu,
    NvU32    lceIndex
)
{
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pVSI, NV_ERR_INVALID_STATE);

    if (RMCTRL_GET_CAP(pVSI->ceCaps[lceIndex].capsTbl, NV2080_CTRL_CE_CAPS, _CE_DECOMP_SUPPORTED))
        return NV_TRUE;

    return NV_FALSE;
}
