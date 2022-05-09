/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @brief: Allocate and initialize an IO_APERTURE instance.
 * 
 * @param[out] ppAperture       pointer to the new IO_APERTURE.
 * @param[in]  pParentAperture  pointer to the parent of the new IO_APERTURE.
 * @param[in]  pDevice          pointer to IO_DEVICE of the APERTURE.
 * @param[in]  offset           offset from the parent APERTURE's baseAddress.
 * @param[in]  length           length of the APERTURE.
 * 
 * @return NV_OK upon success
 *         NV_ERR* otherwise.
 */
NV_STATUS
ioaccessCreateIOAperture
(
    IO_APERTURE **ppAperture,
    IO_APERTURE  *pParentAperture,
    IO_DEVICE    *pDevice,
    NvU32         offset,
    NvU32         length
)
{
    NV_STATUS status = NV_OK;
    IO_APERTURE *pAperture = portMemAllocNonPaged(sizeof(IO_APERTURE));

    if (pAperture == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pAperture, 0, sizeof(IO_APERTURE));

    status = ioaccessInitIOAperture(pAperture, pParentAperture, pDevice, offset, length);
    if (status != NV_OK)
    {
        portMemFree(pAperture);
    }
    else
    {
        *ppAperture = pAperture;
    }

    return status;
}


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
 
void
ioaccessDestroyIOAperture
(
    IO_APERTURE *pAperture
)
{
    portMemFree(pAperture);
}
