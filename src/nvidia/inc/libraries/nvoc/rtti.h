/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * This file is part of the NVOC runtime.
 */

#ifndef _NVOC_RTTI_H_
#define _NVOC_RTTI_H_

#include "nvtypes.h"
#include "nvoc/runtime.h"
#include "nvport/inline/util_valist.h"

typedef NV_STATUS (*NVOC_DYNAMIC_OBJ_CREATE)(Dynamic**, Dynamic *pParent, NvU32 createFlags, va_list);
typedef void (*NVOC_DYNAMIC_DTOR)(Dynamic*);

// struct NVOC_CLASS_METADATA
// {
//     // NvBool                isMixedMode;
//     // NvS32                 ring;
//     // const struct NVOC_EXPORTS  *const pExportedClasses;
// };

// MSVC warning C4200 on "NVOC_CASTINFO::relatives": zero-sized array in struct/union
// Ignore the warning on VS2013+
//! List of valid casts, needed for dynamicCast.
struct NVOC_CASTINFO
{
    const NvU32             numRelatives;
    const struct NVOC_RTTI *const relatives[];
};



//! Items unique to each NVOC class definition.  Used to identify NVOC classes.
struct NVOC_CLASS_DEF {
    const NVOC_CLASS_INFO classInfo;              // public, defined in runtime.h; contains classId, size, and name
    const NVOC_DYNAMIC_OBJ_CREATE objCreatefn;
    const struct NVOC_CASTINFO *const pCastInfo;
    const struct NVOC_EXPORT_INFO* const pExportInfo;
};

//! Items used to identify base class subobjects.
struct NVOC_RTTI                                  // one per derived-ancestor relationship pair (and every derived class is also its own ancestor)
{
    const struct NVOC_CLASS_DEF *const pClassDef; // drives dynamicCast and objCreateDynamic, one per class
    const NVOC_DYNAMIC_DTOR            dtor;      // __nvoc_destructFromBase for base substructures, real destructor for derived
    const NvU32                        offset;    // 0 for derived
};


void __nvoc_initRtti(Dynamic *pNewObject, const struct NVOC_CLASS_DEF *pClassDef);



#endif
