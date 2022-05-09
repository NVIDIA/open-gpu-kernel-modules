/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/gpu_uuid.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#include "os/os.h"
#include "nvSha1.h"

/**
 * @brief Transforms a raw GPU ID into an ASCII string of the form
 * "GPU-%08x-%04x-%04x-%04x-%012x" (SHA-1)
 *
 * @param[in]  pGidData    Raw GID from OBJPMU/OBJBIF
 * @param[in]  gidSize     Size of the raw ID
 * @param[out] ppGidString Return pointer for the GID string
 * @param[out] pGidStrlen  Return pointer for the GID string length
 * @param[in]  gidFlags    NV2080_GPU_CMD_GPU_GET_GID_FLAGS values: selects
 *                         SHA-1 only
 *
 * @returns matching mapping, or NULL if not found.
 */
NV_STATUS
transformGidToUserFriendlyString
(
    const NvU8 *pGidData,
    NvU32       gidSize,
    NvU8      **ppGidString,
    NvU32      *pGidStrlen,
    NvU32       gidFlags
)
{
    NvUuid uuid;

    if (!FLD_TEST_DRF(2080_GPU_CMD,_GPU_GET_GID_FLAGS,_TYPE,_SHA1,gidFlags))
    {
        return NV_ERR_INVALID_FLAGS;
    }

    NV_ASSERT(NV_UUID_LEN == gidSize);

    portMemCopy(uuid.uuid, NV_UUID_LEN, pGidData, gidSize);

    *ppGidString = portMemAllocNonPaged(NV_UUID_STR_LEN);
    if (*ppGidString == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    nvGetGpuUuidString(&uuid, (char*)*ppGidString);
    *pGidStrlen = NV_UUID_STR_LEN;

    return NV_OK;
}

static NvU32
_nvCopyUuid
(
    NvU8  *pBuff,
    NvU32  index,
    NvU32  size,
    void  *pInfo
)
{
    NvU8 *pBytes = pInfo;
    portMemCopy(pBuff, size, pBytes + index, size);
    return size;
}

/**
 * @brief Generates SHA1 UUID for a GPU or a MIG instance.
 *
 * The UUID will be computed as SHA1(message) where the message is as follows:
 *
 * offset  00  01  02  03  04  05  06  07  08  09  10  11  12  13  14  15
 * value   c8  16  c9  a3  52  24  56  bf  9d  9a  ac  7e  a7  03  fb  5b
 *
 * offset  16  17  18  19  20  21  22  23  24  25  26  27  28  29  30  31
 * value    N   V   I   D   I   A  ''   G   P   U  02   x   x  08   y   y
 *
 * offset  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47
 * value    y   y   y   y   y   y  0b   S   M   C   z   z   z   z   p   p
 *
 * offset  48  49
 * value    p   p
 */
/**
 * where,
 * Char is the byte value in ASCII encoding ('' is space = 0x20)
 * Number is the numeric byte value in hex (0x02)
 * xx is the chip id in little endian format.
 *    The chip ID ARCH+IMPL. For example: 0x017B for GA10B
 * yyyyyyyy is the 64-bit PDI in little endian. PDI = (PDI_1 << 32) OR PDI_0.
 *
 * Additionally, when fractional GPU with MIG is used, and the MIG
 * configurations are exposed as separate logical devices, the following bytes
 * are appended in the message:
 *
 * zzzz is the numeric value of the swizzle id (32-bit little-endian)
 * pppp is the numeric value of the graphics engine physical
 * sys pipe ID (32-bit little-endian)
 *
 * See bug 3028068 for more details.
 *
 * @param[in]   bMIG       "MIG" or "GPU" UUID prefix
 * @param[in]   chipId     GPU chip ID
 * @param[in]   pdi        GPU PDI
 * @param[in]   swizzId    MIG GPU instance swizz ID (only needed for MIG)
 * @param[in]   syspipeId  MIG GPU instance syspipe ID (only needed for MIG)
 * @param[out]  pUuid      UUID
 *
 * @returns NV_OK upon success, otherwise returns NV_ERR_*
 */

#define UUID_MESSAGE_SIZE 50
#define GPU_UUID_MESSAGE_SIZE 38
#define SMC_UUID_MESSAGE_SIZE UUID_MESSAGE_SIZE

static const NvU8 uuidMessage[UUID_MESSAGE_SIZE] =
{
    0xc8, 0x16, 0xc9, 0xa3, 0x52, 0x24, 0x56, 0xbf, 0x9d, 0x9a, 0xac,
    0x7e, 0xa7, 0x03, 0xfb, 0x5b,  'N',  'V',  'I',  'D',  'I',  'A',
     ' ',  'G',  'P',  'U', 0x02,  'x',  'x', 0x08,  'y',  'y',  'y',
     'y',  'y',  'y',  'y',  'y', 0x0b,  'S',  'M',  'C',  'z',  'z',
     'z',  'z',  'p',  'p',  'p',  'p'
};

static NV_STATUS
_nvGenerateUuid
(
    NvBool  bMIG,
    NvU16   chipId,
    NvU64   pdi,
    NvU32   swizzId,
    NvU32   syspipeId,
    NvUuid *pUuid
)
{
    NvU8 *pSha1Digest;
    NvU8 *pMessage;
    NvU32 messageSize = GPU_UUID_MESSAGE_SIZE;

    pSha1Digest = portMemAllocNonPaged(NV_SHA1_DIGEST_LENGTH +
                                       UUID_MESSAGE_SIZE);
    if (pSha1Digest == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    pMessage = pSha1Digest + NV_SHA1_DIGEST_LENGTH;

    portMemCopy(pMessage, UUID_MESSAGE_SIZE, uuidMessage, UUID_MESSAGE_SIZE);

    portUtilWriteLittleEndian16(&pMessage[27], chipId);
    portUtilWriteLittleEndian64(&pMessage[30], pdi);

    if (bMIG)
    {
        portUtilWriteLittleEndian32(&pMessage[42], swizzId);
        portUtilWriteLittleEndian32(&pMessage[46], syspipeId);

        messageSize = SMC_UUID_MESSAGE_SIZE;
    }

    // UUID strings only use the first 16 bytes of the 20-byte SHA-1 digest.
    sha1Generate(pSha1Digest, pMessage, messageSize, _nvCopyUuid);
    portMemCopy(pUuid->uuid, NV_UUID_LEN, pSha1Digest, NV_UUID_LEN);

    // version 5 - SHA1-based
    pUuid->uuid[6] = (pUuid->uuid[6] & 0x0f) | 0x50;
    // variant 1 - network byte ordering
    pUuid->uuid[8] = (pUuid->uuid[8] & 0x3f) | 0x80;

    portMemFree(pSha1Digest);

    return NV_OK;
}

/**
 * @brief Generates SHA1 UUID for GPU.
 *
 * @param[in]   chipId     GPU chip ID
 * @param[in]   pdi        GPU PDI
 * @param[out]  pUuid      UUID
 *
 * @returns NV_OK upon success, otherwise returns NV_ERR_*
 */
NV_STATUS
nvGenerateGpuUuid
(
    NvU16   chipId,
    NvU64   pdi,
    NvUuid *pUuid
)
{
    return _nvGenerateUuid(NV_FALSE, chipId, pdi, 0, 0, pUuid);
}

/**
 * @brief Generates SHA1 UUID for MIG instance.
 *
 * @param[in]   chipId     GPU chip ID
 * @param[in]   pdi        GPU PDI
 * @param[in]   swizzId    MIG GPU instance swizz ID (only needed for _TYPE_SMC)
 * @param[in]   syspipeId  MIG GPU instance syspipe ID (only needed for _TYPE_SMC)
 * @param[out]  pUuid      UUID
 *
 * @returns NV_OK upon success, otherwise returns NV_ERR_*
 */
NV_STATUS
nvGenerateSmcUuid
(
    NvU16   chipId,
    NvU64   pdi,
    NvU32   swizzId,
    NvU32   syspipeId,
    NvUuid *pUuid
)
{
    return _nvGenerateUuid(NV_TRUE, chipId, pdi, swizzId, syspipeId, pUuid);
}

/**
 * @brief Gets UUID ASCII string, "GPU-%08x-%04x-%04x-%04x-%012x"
 *        (SHA-1) or "MIG-%08x-%04x-%04x-%04x-%012x" (SHA-1)
 *
 * @param[in]   bMIG         "MIG" or "GPU" UUID prefix
 * @param[in]   pUuid        UUID
 * @param[out]  pUuidStr     Returns UUID string
 *
 * @returns void
 */
static void
_nvGetUuidString
(
    NvBool        bMIG,
    const NvUuid *pUuid,
    char         *pUuidStr
)
{
    const NvU32 sha1GroupEntryNum[] = { 8, 4, 4, 4, 12 };
    const NvU32 *pGroupEntryNum;
    const NvU32 extraSymbolLen = 9; // 'G' 'P' 'U' '-'(x5), '\0x0', total = 9
    const NvU8 prefixLen = 4;
    const char *pPrefix;
    NvU32 groupCount;
    NvU32 expectedStringLength = (NV_UUID_LEN << 1) + extraSymbolLen;

    pGroupEntryNum = sha1GroupEntryNum;
    groupCount = NV_ARRAY_ELEMENTS(sha1GroupEntryNum);

    pPrefix = bMIG ? "MIG-" : "GPU-";
    portMemCopy(pUuidStr, prefixLen, pPrefix, prefixLen);
    pUuidStr += prefixLen;

    portStringBufferToHexGroups(pUuidStr, (expectedStringLength - prefixLen),
                                pUuid->uuid, NV_UUID_LEN,
                                groupCount, pGroupEntryNum, "-");
}

/**
 * @brief Gets UUID ASCII string, "GPU-%08x-%04x-%04x-%04x-%012x"
 *        (SHA-1)
 *
 * @param[in]   pUuid        UUID
 * @param[out]  pUuidStr     Returns UUID string
 *
 * @returns void
 */
void
nvGetGpuUuidString
(
    const NvUuid *pUuid,
    char         *pUuidStr
)
{
    _nvGetUuidString(NV_FALSE, pUuid, pUuidStr);
}

/**
 * @brief Gets UUID ASCII string, "MIG-%08x-%04x-%04x-%04x-%012x"
 *        (SHA-1)
 *
 * @param[in]   pUuid        UUID
 * @param[out]  pUuidStr     Returns UUID string
 *
 * @returns void
 */
void
nvGetSmcUuidString
(
    const NvUuid *pUuid,
    char         *pUuidStr
)
{
    _nvGetUuidString(NV_TRUE, pUuid, pUuidStr);
}
