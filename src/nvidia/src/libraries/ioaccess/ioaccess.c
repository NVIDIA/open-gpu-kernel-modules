/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "ioaccess/ioaccess.h"
#include "utils/nvprintf.h"
#include "nvport/nvport.h"
#include "utils/nvassert.h"

#if !((defined(NVRM) || defined(RMCFG_FEATURE_PLATFORM_GSP)) && !defined(NVWATCH))
// Broadcast read implementation - reads from first aperture
static NvU8 swbcReadReg008(SWBC_APERTURE *pSwbcAperture, NvU32 addr)
{
    NvU8 val = REG_RD08(pSwbcAperture->pApertures[0], addr);
#if defined(DEBUG)
    NvU32 i;
    for (i = 1; i < pSwbcAperture->numApertures; i++)
        NV_ASSERT(REG_RD08(&(*pSwbcAperture->pApertures)[i], addr) == val);
#endif // defined(DEBUG)
    return val;
}

static NvU16 swbcReadReg016(SWBC_APERTURE *pSwbcAperture, NvU32 addr)
{
    NvU16 val = REG_RD16(pSwbcAperture->pApertures[0], addr);
#if defined(DEBUG)
    NvU32 i;
    for (i = 1; i < pSwbcAperture->numApertures; i++)
        NV_ASSERT(REG_RD16(&(*pSwbcAperture->pApertures)[i], addr) == val);
#endif // defined(DEBUG)
    return val;
}

static NvU32 swbcReadReg032(SWBC_APERTURE *pSwbcAperture, NvU32 addr)
{
    NvU32 val = REG_RD32(pSwbcAperture->pApertures[0], addr);
#if defined(DEBUG)
    NvU32 i;
    for (i = 1; i < pSwbcAperture->numApertures; i++)
        NV_ASSERT(REG_RD32(&(*pSwbcAperture->pApertures)[i], addr) == val);
#endif // defined(DEBUG)
    return val;
}

// Broadcast write implementations - write to all apertures
static void swbcWriteReg008(SWBC_APERTURE *pSwbcAperture, NvU32 addr, NvV8 value)
{
    NvU32 i;
    for (i = 0; i < pSwbcAperture->numApertures; i++)
    {
        REG_WR08(&(*pSwbcAperture->pApertures)[i], addr, value);
    }
}

static void swbcWriteReg016(SWBC_APERTURE *pSwbcAperture, NvU32 addr, NvV16 value)
{
    NvU32 i;
    for (i = 0; i < pSwbcAperture->numApertures; i++)
    {
        REG_WR16(&(*pSwbcAperture->pApertures)[i], addr, value);
    }
}

static void swbcWriteReg032(SWBC_APERTURE *pSwbcAperture, NvU32 addr, NvV32 value)
{
    NvU32 i;
    for (i = 0; i < pSwbcAperture->numApertures; i++)
    {
        REG_WR32(&(*pSwbcAperture->pApertures)[i], addr, value);
    }
}

static void swbcWriteReg032Uc(SWBC_APERTURE *pSwbcAperture, NvU32 addr, NvV32 value)
{
    NvU32 i;
    for (i = 0; i < pSwbcAperture->numApertures; i++)
    {
        REG_WR32_UC(&(*pSwbcAperture->pApertures)[i], addr, value);
    }
}

static NvBool swbcValidReg(SWBC_APERTURE *pSwbcAperture, NvU32 addr)
{
    NvU32 i;
    for (i = 0; i < pSwbcAperture->numApertures; i++)
    {
        if (!REG_VALID(&(*pSwbcAperture->pApertures)[i], addr))
        {
            return NV_FALSE;
        }
    }
    return NV_TRUE;
}

static SWBC_DEVICE swbcDevice = {
    .pReadReg008Fn    = (SwbcReadReg008Fn*)swbcReadReg008,
    .pReadReg016Fn    = (SwbcReadReg016Fn*)swbcReadReg016,
    .pReadReg032Fn    = (SwbcReadReg032Fn*)swbcReadReg032,
    .pWriteReg008Fn   = (SwbcWriteReg008Fn*)swbcWriteReg008,
    .pWriteReg016Fn   = (SwbcWriteReg016Fn*)swbcWriteReg016,
    .pWriteReg032Fn   = (SwbcWriteReg032Fn*)swbcWriteReg032,
    .pWriteReg032UcFn = (SwbcWriteReg032Fn*)swbcWriteReg032Uc,
    .pValidRegFn      = (SwbcValidRegFn*)swbcValidReg,
};

/*!
 * Initialize an IO_APERTURE instance. This enables initialization for derived IO_APERTURE instances
 * that are not allocated via CreateIOAperture.
 *
 * @param[in,out] pAperture        pointer to IO_APERTURE instance to be initialized.
 * @param[in]     pParentAperture  pointer to parent of the new IO_APERTURE.
 * @param[in]     pDevice          pointer to IO_DEVICE of the APERTURE.
 * @param[in]     offset           offset from the parent APERTURE's baseAddress.
 * @param[in]     length           length of the APERTURE.
 *
 * @return NV_OK when inputs are valid.
 */
NV_STATUS
ioaccessInitIOAperture
(
    IO_APERTURE *pAperture,
    IO_APERTURE *pParentAperture,
    IO_DEVICE *pDevice,
    NvU32 offset,
    NvU32 length
)
{
    if (pAperture == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // Aperture's IO device can't be set if both the parent aperture and IO device
    // input arguments are NULL.
    //
    if ((pDevice == NULL) && (pParentAperture == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pDevice != NULL)
    {
        pAperture->pDevice = pDevice;
    }

    if (pParentAperture != NULL)
    {
        pAperture->pDevice     = pParentAperture->pDevice;
        pAperture->baseAddress = pParentAperture->baseAddress;

        // Check if the child Aperture strides beyond the parent's boundary.
        if ((length + offset) > pParentAperture->length)
        {
            NV_PRINTF(LEVEL_WARNING,
                "Child aperture crosses parent's boundary, length %u offset %u, Parent's length %u\n",
                length, offset, pParentAperture->length);
        }
    }
    else
    {
        pAperture->baseAddress = 0;
    }

    pAperture->baseAddress += offset;
    pAperture->length       = length;

    return NV_OK;
}

NV_STATUS swbcInitAperture
(
    SWBC_APERTURE *pSwbcAperture, 
    IO_APERTURE  **pApertures, 
    NvU32          numApertures
)
{
    if (pSwbcAperture == NULL || pApertures == NULL || numApertures == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Initialize the IO_APERTURE-compatible fields first
    pSwbcAperture->pDevice = (SWBC_DEVICE*)&swbcDevice;
    // Initialize broadcast-specific fields
    pSwbcAperture->pApertures = pApertures;
    pSwbcAperture->numApertures = numApertures;

    return NV_OK;
}
#endif // !((defined(NVRM) || defined(RMCFG_FEATURE_PLATFORM_GSP)) && !defined(NVWATCH))
