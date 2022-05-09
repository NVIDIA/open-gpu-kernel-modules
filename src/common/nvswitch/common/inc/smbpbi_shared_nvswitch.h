/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SMBPBI_SHARED_NVSWITCH_H_
#define _SMBPBI_SHARED_NVSWITCH_H_

#include "inforom/types.h"
#include "inforom/ifrdem.h"

/*!
 *
 * Shared surface between nvswitch and SOE that includes
 * data from the InfoROM needed for OOB queries
 *
 */
typedef struct
{
    struct {
        NvBool  bValid;
        NvU8    boardPartNum[24];
        NvU8    serialNum[16];
        NvU8    marketingName[24];
        NvU32   buildDate;
    } OBD;

    struct {
        NvBool  bValid;
        NvU8    oemInfo[32];
    } OEM;

    struct {
        NvBool  bValid;
        NvU8    inforomVer[16];
    } IMG;

    struct {
        NvBool        bValid;
        NvU64_ALIGN32 uncorrectedTotal;
        NvU64_ALIGN32 correctedTotal;
    } ECC;

    struct _def_inforomdata_dem_object {
        NvBool                              bValid;
        NvBool                              bPresent;   // in the InfoROM image

        union {
            INFOROM_OBJECT_HEADER_V1_00     header;
            INFOROM_DEM_OBJECT_V1_00        v1;
        } object;
    } DEM;
} RM_SOE_SMBPBI_INFOROM_DATA, *PRM_SOE_SMBPBI_INFOROM_DATA;

typedef struct
{
    RM_SOE_SMBPBI_INFOROM_DATA  inforomObjects;
} SOE_SMBPBI_SHARED_SURFACE, *PSOE_SMBPBI_SHARED_SURFACE;

/*!
 * Macros to evaluate offsets into the shared surface
 */

#define SOE_SMBPBI_SHARED_OFFSET_INFOROM(obj, member)   \
                NV_OFFSETOF(SOE_SMBPBI_SHARED_SURFACE, inforomObjects.obj.member)

#endif // _SMBPBI_SHARED_NVSWITCH_H_
