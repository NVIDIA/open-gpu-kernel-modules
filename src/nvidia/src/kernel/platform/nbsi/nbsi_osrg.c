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

/*!
 * @file
 * @brief NBSI table initialization, search and hash routines.
 */

#include "os/os.h"
#include "platform/nbsi/nbsi_table.h"
#include "platform/nbsi/nbsi_read.h"
#include "gpu/gpu.h"

/*!
 * The following code pulled from nvString.c which is in drivers/common/src
 * but apparently not built so the rm can use them.
 *
 */
static NvU16 _nvStrLen(const char *szStr)
{
    NvU16 dwLen = 0;
    // next statements on one line so easier to step over in debugger
    while (*szStr != '\0') { dwLen ++; szStr ++; }
    return (dwLen);
}

/*!
 *  NV_STATUS nbsiReadRegistryDword(pGpu, pRegParmStr, pData);
 *
 *  This function searches both the nbsi table and the os registry table and
 *  returns the best result.
 *
 *  Input parameters:
 *      pGpu               - OBJGPU pointer
 *      pRegParmStr        - registry element string
 *      pData              - registry value
 *
 *  Returns NV_STATUS:
 *      NV_OK:      key was found and data returned in pData
 *     !NV_OK:      key was not found in either nbsi table or os registry
 *                  found.
 *
 *  Output parameters:
 *      pData     - registry value (if NV_STATUS is NV_OK)
 *
 */
NV_STATUS nbsiReadRegistryDword
(
    OBJGPU *pGpu,
    const char *pRegParmStr,
    NvU32  *pData
)
{
    NBSI_OBJ  *pNbsiObj = getNbsiObject();
    NvU32      nbsiDword;
    NvU16      pathHash;
    NvU8      *pRetBuf;
    NvU32      retSize;
    NvU32      errorCode;
    NvU16      module = NV2080_CTRL_BIOS_NBSI_MODULE_RM;
    NV_STATUS  status;
    NvU32      elementHashArray[MAX_NBSI_OS];
    NvU8       maxOSndx;

    NV_ASSERT_OR_RETURN(pRegParmStr != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pData != NULL, NV_ERR_INVALID_ARGUMENT);

    if ((pGpu == NULL) || (pNbsiObj == NULL))
    {
        return NV_ERR_INVALID_OBJECT;
    }

    // Check to make sure we are not in the power management code path
    if ((pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH))
#if defined(DEBUG)
        && (!pGpu->getProperty(pGpu, PDB_PROP_GPU_DO_NOT_CHECK_REG_ACCESS_IN_PM_CODEPATH))
#endif
       )
    {
        //
        // Registry reads should only be done during boot and in the corresponding
        // engine *initRegistryOverrides function where possible.  See bug 649189.
        //
        NV_PRINTF(LEVEL_ERROR,
                  "osReadRegistryDword called in Sleep path can cause excessive delays!\n");

        NV_ASSERT(0);
        return NV_ERR_INVALID_REQUEST;
    }

    pathHash = pNbsiObj->nbsiBlankPathHash;
    maxOSndx = pNbsiObj->curMaxNbsiOSes;

    // The RM module uses a blank (relative path) so just do element.
    fnv1Hash20Array((const NvU8 *) pRegParmStr,
                    _nvStrLen(pRegParmStr),
                    elementHashArray,
                    maxOSndx);

    retSize = sizeof(nbsiDword);
    pRetBuf = (NvU8 *) &nbsiDword;

    //
    // Read from both NBSI table and os reg tables... then decide what
    // to return.
    //
    status = getNbsiValue(pGpu,
                          module,
                          pathHash,
                          maxOSndx,
                          elementHashArray,
                          pRetBuf,
                          &retSize,
                          &errorCode);
    if (status == NV_OK)
    {
        if (errorCode == NV2080_CTRL_BIOS_GET_NBSI_SUCCESS)
        {
            *pData = nbsiDword;
        }
        else
        {
            //
            // assert if we didn't have enough room to hold the returned data.
            // Since we define the return as a dword here, this could only be
            // because the registry has the key but it's defined longer than a
            // dword. i.e. byte array or extended byte array.
            //
            NV_ASSERT(errorCode != NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE);
            status = NV_ERR_GENERIC;
        }
    }

    return status;
}

/*!
 *  NV_STATUS nbsiReadRegistryString(pGpu, pRegParmStr, pData, pCbLen);
 *
 *  This function searches both the nbsi table and the os registry table and
 *  returns the best result.
 *
 *  Input parameters:
 *      pGpu               - OBJGPU pointer
 *      pRegParmStr        - registry element string
 *      pData              - registry value
 *      pCbLen             - size of registry in bytes
 *
 *  Returns NV_STATUS:
 *      NV_OK:      key was found and data returned in pData
 *     !NV_OK:      key was not found in either nbsi table or os registry
 *                  found.
 *
 *  Output parameters:
 *      pData    - registry value (if NV_STATUS is NV_OK)
 *      pCbLen   - size of registry in bytes (if NV_STATUS is NV_OK)
 *
 */
NV_STATUS nbsiReadRegistryString
(
    OBJGPU *pGpu,
    const char *pRegParmStr,
    NvU8   *pData,
    NvU32  *pCbLen
)
{
    NBSI_OBJ  *pNbsiObj = getNbsiObject();
    NvU16      pathHash;
    NvU8      *pRetBuf;
    NvU32      retSize;
    NvU32      errorCode;
    NvU16      module = NV2080_CTRL_BIOS_NBSI_MODULE_RM;
    NV_STATUS  status;
    NvU32      elementHashArray[MAX_NBSI_OS];
    NvU8       maxOSndx;

    NV_ASSERT_OR_RETURN(pRegParmStr != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pCbLen != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(!(*pCbLen != 0 && pData == NULL), NV_ERR_INVALID_ARGUMENT);

    if ((pGpu == NULL) || (pNbsiObj == NULL))
    {
        return NV_ERR_INVALID_OBJECT;
    }

    pathHash = pNbsiObj->nbsiBlankPathHash;
    maxOSndx = pNbsiObj->curMaxNbsiOSes;

    // The RM module uses a blank (relative path) so just do element.
    fnv1Hash20Array((const NvU8 *)pRegParmStr,
                    _nvStrLen(pRegParmStr),
                    elementHashArray,
                    maxOSndx);

    // save the original length... to be used later if doing the NBSI read
    retSize = (NvU32) *pCbLen;
    pRetBuf = pData;

    // Read from NBSI and os reg tables... then decide what to return.
    status = getNbsiValue(pGpu,
                          module,
                          pathHash,
                          maxOSndx,
                          elementHashArray,
                          pRetBuf,
                          &retSize,
                          &errorCode);
    if (status == NV_OK)
    {
        if (errorCode == NV2080_CTRL_BIOS_GET_NBSI_SUCCESS)
        {
            *pCbLen = retSize;
        }
        else
        {
            // assert if we didn't have enough room to hold the returned data
            NV_ASSERT(errorCode != NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE);
            status = NV_ERR_GENERIC;
        }
    }


    return status;
}
