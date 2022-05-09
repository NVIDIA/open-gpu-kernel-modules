/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#ifndef CHIPSET_INFO_H
#define CHIPSET_INFO_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
*       Chipset and Root Port information                                   *
*                                                                           *
\***************************************************************************/

#include "core/core.h"
#include "platform/chipset/chipset.h"

typedef struct CSINFO CSINFO;
typedef CSINFO *PCSINFO;

typedef struct VENDORNAME VENDORNAME;
typedef VENDORNAME *PVENDORNAME;

typedef struct RPINFO RPINFO;
typedef RPINFO *PRPINFO;

typedef struct BRINFO BRINFO;
typedef RPINFO *PBRINFO;

typedef struct ARMCSALLOWLISTINFO ARMCSALLOWLISTINFO;
typedef ARMCSALLOWLISTINFO *PARMCSALLOWLISTINFO;

struct CSINFO
{
    NvU16 vendorID, deviceID;
    NvU32 chipset;
    const char *name;
    NV_STATUS (*setupFunc)(OBJCL *);
};

extern CSINFO chipsetInfo[];

struct VENDORNAME
{
    NvU32 vendorID;
    const char *name;
};

extern VENDORNAME vendorName[];

struct RPINFO
{
    NvU16 vendorID, deviceID;
    NvU32 rpID;
    NV_STATUS (*setupFunc)(OBJGPU *, OBJCL *);
};

extern RPINFO rootPortInfo[];

struct BRINFO
{
    NvU16 vendorID, deviceID;
    NV_STATUS (*setupFunc)(OBJGPU *, OBJCL *);
};

extern BRINFO upstreamPortInfo[];

struct ARMCSALLOWLISTINFO
{
    NvU64 vendorID, deviceID;
    NvU32 chipset;
};

extern ARMCSALLOWLISTINFO armChipsetAllowListInfo[];

void csGetInfoStrings(OBJCL *, NvU8 *, NvU8 *, NvU8 *, NvU8 *, NvU32);

#endif // CHIPSET_INFO_H
