/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gsp/kernel_gsp.h"

#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/sec2/kernel_sec2.h"
#include "core/bin_data.h"

/*!
 * Free a KernelGspVbiosImg structure.
 *
 * @param[in] pVbiosImg  structure to free
 */
void
kgspFreeVbiosImg
(
    KernelGspVbiosImg *pVbiosImg
)
{
    if (pVbiosImg == NULL)
    {
        return;
    }

    portMemFree(pVbiosImg->pImage);
    pVbiosImg->pImage = NULL;

    portMemFree(pVbiosImg);
}

/*!
 * Free a KernelGspFlcnUcode structure.
 *
 * @param[in] pFlcnUcode structure to free
 */
void
kgspFreeFlcnUcode
(
    KernelGspFlcnUcode *pFlcnUcode
)
{
    if (pFlcnUcode == NULL)
    {
        return;
    }

    if (pFlcnUcode->bootType == KGSP_FLCN_UCODE_BOOT_FROM_HS)
    {
        KernelGspFlcnUcodeBootFromHs *pUcode = &pFlcnUcode->ucodeBootFromHs;
        if (pUcode->pUcodeMemDesc != NULL)
        {
            memdescFree(pUcode->pUcodeMemDesc);
            memdescDestroy(pUcode->pUcodeMemDesc);
            pUcode->pUcodeMemDesc = NULL;
        }
        portMemFree(pUcode->pSignatures);
        pUcode->pSignatures = NULL;
    }
    else if (pFlcnUcode->bootType == KGSP_FLCN_UCODE_BOOT_WITH_LOADER)
    {
        KernelGspFlcnUcodeBootWithLoader *pUcode = &pFlcnUcode->ucodeBootWithLoader;
        if (pUcode->pCodeMemDesc != NULL)
        {
            memdescFree(pUcode->pCodeMemDesc);
            memdescDestroy(pUcode->pCodeMemDesc);
            pUcode->pCodeMemDesc = NULL;
        }
        if (pUcode->pDataMemDesc != NULL)
        {
            memdescFree(pUcode->pDataMemDesc);
            memdescDestroy(pUcode->pDataMemDesc);
            pUcode->pDataMemDesc = NULL;
        }
    }
    else if (pFlcnUcode->bootType == KGSP_FLCN_UCODE_BOOT_DIRECT)
    {
        KernelGspFlcnUcodeBootDirect *pUcode = &pFlcnUcode->ucodeBootDirect;
        portMemFree(pUcode->pImage);
        pUcode->pImage = NULL;
    }

    portMemFree(pFlcnUcode);
}

static NV_STATUS
s_bindataWriteToFixedSizeBuffer
(
    const BINDATA_STORAGE *pBinStorage,
    void *pBuf,  // out
    NvU32 bufSize
)
{
    NV_STATUS status = NV_OK;

    if (bindataGetBufferSize(pBinStorage) != bufSize)
    {
        status = NV_ERR_INVALID_DATA;
        return status;
    }

    status = bindataWriteToBuffer(pBinStorage, (NvU8 *) pBuf, bufSize);
    if (status != NV_OK)
    {
        return status;
    }

    return status;
}

static NV_STATUS
s_patchBooterUcodeSignature
(
    OBJGPU *pGpu,
    NvU32 ucodeId,
    NvU8 *pImage,
    NvU32 sigDestOffset,
    NvU32 imageSize,
    const void *pSignatures,
    NvU32 signaturesTotalSize,
    NvU32 numSigs
)
{
    NvU32 sigIndex = 0;
    NvU32 sigSize = signaturesTotalSize / numSigs;
    NvU32 fuseVer;

    NV_ASSERT_OR_RETURN(pImage != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(imageSize > sigDestOffset, NV_ERR_INVALID_DATA);
    NV_ASSERT_OR_RETURN(imageSize - sigDestOffset > sigSize, NV_ERR_INVALID_DATA);
    NV_ASSERT_OR_RETURN(pSignatures != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(numSigs > 0, NV_ERR_INVALID_DATA);

    KernelSec2 *pKernelSec2 = GPU_GET_KERNEL_SEC2(pGpu);
    NV_ASSERT_OR_RETURN(pKernelSec2 != NULL, NV_ERR_INVALID_STATE);
    fuseVer = ksec2ReadUcodeFuseVersion_HAL(pGpu, pKernelSec2, ucodeId);

    if (numSigs > 1)
    {
        if (fuseVer > numSigs - 1)
        {
            NV_PRINTF(LEVEL_ERROR, "signature for fuse version %u not present\n", fuseVer);
            return NV_ERR_OUT_OF_RANGE;
        }
        sigIndex = numSigs - 1 - fuseVer;
    }

    portMemCopy(pImage + sigDestOffset, sigSize, ((NvU8 *) pSignatures) + sigIndex * sigSize, sigSize);
    return NV_OK;
}

static NV_STATUS
s_allocateUcodeFromBinArchive
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    const BINDATA_ARCHIVE *pBinArchive,
    KernelGspFlcnUcode **ppFlcnUcode  // out
)
{
    NV_STATUS status;
    KernelGspFlcnUcode *pFlcnUcode;

    struct {
        NvU32 osCodeOffset;
        NvU32 osCodeSize;
        NvU32 osDataOffset;
        NvU32 osDataSize;
        NvU32 numApps;
        NvU32 appCodeOffset;
        NvU32 appCodeSize;
        NvU32 appDataOffset;
        NvU32 appDataSize;
    } header;

    struct {
        NvU32 fuseVer;
        NvU32 engineId;
        NvU32 ucodeId;
    } patchMeta;

    NvU32 patchLoc;
    NvU32 patchSig;
    NvU32 numSigs;
    NvU32 signaturesTotalSize;
    const void *pSignatures = NULL;

    const BINDATA_STORAGE *pBinImage;
    const BINDATA_STORAGE *pBinHeader;
    const BINDATA_STORAGE *pBinSig;
    const BINDATA_STORAGE *pBinPatchSig;
    const BINDATA_STORAGE *pBinPatchLoc;
    const BINDATA_STORAGE *pBinPatchMeta;
    const BINDATA_STORAGE *pBinNumSigs;

    if (kgspIsDebugModeEnabled_HAL(pGpu, pKernelGsp))
    {
        pBinImage = bindataArchiveGetStorage(pBinArchive, "image_dbg");
        pBinHeader = bindataArchiveGetStorage(pBinArchive, "header_dbg");
        pBinSig = bindataArchiveGetStorage(pBinArchive, "sig_dbg");
    }
    else
    {
        pBinImage = bindataArchiveGetStorage(pBinArchive, "image_prod");
        pBinHeader = bindataArchiveGetStorage(pBinArchive, "header_prod");
        pBinSig = bindataArchiveGetStorage(pBinArchive, "sig_prod");
    }

    NV_ASSERT_OR_RETURN(pBinImage != NULL, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pBinHeader != NULL, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pBinSig != NULL, NV_ERR_NOT_SUPPORTED);

    pBinPatchSig = bindataArchiveGetStorage(pBinArchive, "patch_sig");
    pBinPatchLoc = bindataArchiveGetStorage(pBinArchive, "patch_loc");
    pBinPatchMeta = bindataArchiveGetStorage(pBinArchive, "patch_meta");
    pBinNumSigs = bindataArchiveGetStorage(pBinArchive, "num_sigs");

    NV_ASSERT_OR_RETURN(pBinPatchSig != NULL, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pBinPatchLoc != NULL, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pBinPatchMeta != NULL, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pBinNumSigs != NULL, NV_ERR_NOT_SUPPORTED);

    pFlcnUcode = portMemAllocNonPaged(sizeof(*pFlcnUcode));
    if (pFlcnUcode == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(pFlcnUcode, 0, sizeof(*pFlcnUcode));

    // Retrieve header
    NV_ASSERT_OK_OR_GOTO(status,
        s_bindataWriteToFixedSizeBuffer(pBinHeader, &header, sizeof(header)),
        out);

    if (header.numApps != 1)
    {
        NV_ASSERT(0);
        status = NV_ERR_INVALID_DATA;
        goto out;
    }

    // Retrieve signature patch location
    NV_ASSERT_OK_OR_GOTO(status,
        s_bindataWriteToFixedSizeBuffer(pBinPatchLoc, &patchLoc, sizeof(patchLoc)),
        out);

    // Retrieve signature patch index
    NV_ASSERT_OK_OR_GOTO(status,
        s_bindataWriteToFixedSizeBuffer(pBinPatchSig, &patchSig, sizeof(patchSig)),
        out);

    if (patchSig != 0)
    {
        NV_ASSERT(0);
        status = NV_ERR_INVALID_DATA;
        goto out;
    }

    // Retrieve signature patch metadata
    NV_ASSERT_OK_OR_GOTO(status,
        s_bindataWriteToFixedSizeBuffer(pBinPatchMeta, &patchMeta, sizeof(patchMeta)),
        out);

    // Retrieve signatures
    NV_ASSERT_OK_OR_GOTO(status,
        s_bindataWriteToFixedSizeBuffer(pBinNumSigs, &numSigs, sizeof(numSigs)),
        out);

    if (numSigs == 0)
    {
        NV_ASSERT(0);
        status = NV_ERR_INVALID_DATA;
        goto out;
    }

    signaturesTotalSize = bindataGetBufferSize(pBinSig);

    if ((signaturesTotalSize == 0) || ((signaturesTotalSize % numSigs) != 0))
    {
        NV_ASSERT(0);
        status = NV_ERR_INVALID_DATA;
        goto out;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        bindataStorageAcquireData(pBinSig, &pSignatures),
        out);

    // Populate KernelGspFlcnUcode structure
    if (staticCast(pKernelGsp, KernelFalcon)->bBootFromHs)
    {
        KernelGspFlcnUcodeBootFromHs *pUcode = &pFlcnUcode->ucodeBootFromHs;
        NvU8 *pMappedUcodeMem;

        pFlcnUcode->bootType = KGSP_FLCN_UCODE_BOOT_FROM_HS;

        pUcode->size = bindataGetBufferSize(pBinImage);

        pUcode->codeOffset = header.appCodeOffset;
        pUcode->imemSize = header.appCodeSize;
        pUcode->imemPa = 0;
        pUcode->imemVa = header.appCodeOffset;

        pUcode->dataOffset = header.osDataOffset;
        pUcode->dmemSize = header.osDataSize;
        pUcode->dmemPa = 0;
        pUcode->dmemVa = FLCN_DMEM_VA_INVALID;

        pUcode->hsSigDmemAddr = patchLoc - pUcode->dataOffset;
        pUcode->ucodeId = patchMeta.ucodeId;
        pUcode->engineIdMask = patchMeta.engineId;

        NV_ASSERT_OK_OR_GOTO(status,
            memdescCreate(&pUcode->pUcodeMemDesc, pGpu, pUcode->size,
                          16, NV_TRUE, ADDR_SYSMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE), out);

        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_57, 
                    pUcode->pUcodeMemDesc);
        if (status != NV_OK)
        {
            goto out;
        }

        pMappedUcodeMem = memdescMapInternal(pGpu, pUcode->pUcodeMemDesc, TRANSFER_FLAGS_NONE);
        if (pMappedUcodeMem == NULL)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto out;
        }

        // Copy in the whole image
        status = bindataWriteToBuffer(pBinImage, pMappedUcodeMem, pUcode->size);
        NV_ASSERT(status == NV_OK);

        // Patch signatures (only if image copy above succeeded)
        if (status == NV_OK)
        {
            status = s_patchBooterUcodeSignature(pGpu,
                patchMeta.ucodeId,
                pMappedUcodeMem, patchLoc, pUcode->size,
                pSignatures, signaturesTotalSize, numSigs);
            NV_ASSERT(status == NV_OK);
        }

        memdescUnmapInternal(pGpu, pUcode->pUcodeMemDesc, TRANSFER_FLAGS_DESTROY_MAPPING);
        pMappedUcodeMem = NULL;

        if (status != NV_OK)
        {
            goto out;
        }
    }
    else
    {
        KernelGspFlcnUcodeBootDirect *pUcode = &pFlcnUcode->ucodeBootDirect;

        pFlcnUcode->bootType = KGSP_FLCN_UCODE_BOOT_DIRECT;

        pUcode->size = bindataGetBufferSize(pBinImage);

        pUcode->imemNsPa = header.osCodeOffset;
        pUcode->imemNsSize = header.osCodeSize;
        pUcode->imemSecPa = header.appCodeOffset;
        pUcode->imemSecSize = header.appCodeSize;

        pUcode->dataOffset = header.osDataOffset;
        pUcode->dmemPa = 0;
        pUcode->dmemSize = header.osDataSize;

        pUcode->pImage = portMemAllocNonPaged(pUcode->size);
        if (pUcode->pImage == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto out;
        }

        // We are not using zero copy api bindataStorageAcquireData here because s_patchBooterUcodeSignature
        // writes to pUcode->pImage to patch the signatures. Since this path is taken once, the risk
        // of accidentally overwriting original bindata buffer may not be worth the performance gains.
        // Copy in the whole image
        NV_ASSERT_OK_OR_GOTO(status,
            bindataWriteToBuffer(pBinImage, pUcode->pImage, pUcode->size),
            out);

        // Patch signatures
        NV_ASSERT_OK_OR_GOTO(status,
            s_patchBooterUcodeSignature(pGpu,
                patchMeta.ucodeId,
                pUcode->pImage, patchLoc, pUcode->size,
                pSignatures, signaturesTotalSize, numSigs),
            out);
    }

out:
    bindataStorageReleaseData((void*)pSignatures);
    pSignatures = NULL;

    if (status == NV_OK)
    {
        *ppFlcnUcode = pFlcnUcode;
    }
    else
    {
        kgspFreeFlcnUcode(pFlcnUcode);
        pFlcnUcode = NULL;
    }

    return status;
}

NV_STATUS
kgspAllocateBooterLoadUcodeImage_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspFlcnUcode **ppBooterLoadUcode  // out
)
{
    const BINDATA_ARCHIVE *pBinArchive;

    NV_ASSERT_OR_RETURN(ppBooterLoadUcode != NULL, NV_ERR_INVALID_ARGUMENT);

    pBinArchive = kgspGetBinArchiveBooterLoadUcode_HAL(pKernelGsp);
    NV_ASSERT_OR_RETURN(pBinArchive != NULL, NV_ERR_NOT_SUPPORTED);

    return s_allocateUcodeFromBinArchive(pGpu, pKernelGsp, pBinArchive, ppBooterLoadUcode);
}

NV_STATUS
kgspAllocateBooterUnloadUcodeImage_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspFlcnUcode **ppBooterUnloadUcode  // out
)
{
    const BINDATA_ARCHIVE *pBinArchive;

    NV_ASSERT_OR_RETURN(ppBooterUnloadUcode != NULL, NV_ERR_INVALID_ARGUMENT);

    pBinArchive = kgspGetBinArchiveBooterUnloadUcode_HAL(pKernelGsp);
    NV_ASSERT_OR_RETURN(pBinArchive != NULL, NV_ERR_NOT_SUPPORTED);

    return s_allocateUcodeFromBinArchive(pGpu, pKernelGsp, pBinArchive, ppBooterUnloadUcode);
}

//
// Note: Scrubber is not a Booter ucode, however it is a SEC2 ucode that uses
// a similar loading and signature patching scheme, so we may use the same
// helper functions.
//
NV_STATUS
kgspAllocateScrubberUcodeImage_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspFlcnUcode **ppScrubberUcode  // out
)
{
    KernelSec2 *pKernelSec2 = GPU_GET_KERNEL_SEC2(pGpu);
    const BINDATA_ARCHIVE *pBinArchive;

    NV_ASSERT_OR_RETURN(pKernelSec2 != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(ppScrubberUcode != NULL, NV_ERR_INVALID_ARGUMENT);

    pBinArchive = ksec2GetBinArchiveSecurescrubUcode_HAL(pGpu, pKernelSec2);
    NV_ASSERT_OR_RETURN(pBinArchive != NULL, NV_ERR_NOT_SUPPORTED);

    return s_allocateUcodeFromBinArchive(pGpu, pKernelGsp, pBinArchive, ppScrubberUcode);
}
