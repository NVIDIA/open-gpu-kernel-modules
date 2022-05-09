/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _RS_DOMAIN_H_
#define _RS_DOMAIN_H_

#include "resserv/resserv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RsDomain
 * @addtogroup RsDomain
 * @{*/

/**
 * @brief Domains are being re-worked
 */
struct RsDomain
{
    NvU32 dummy;
};

/**
 * Construct a domain instance
 * @param[in]   pDomain This domain
 * @param[in]   pAllocator
 * @param[in]   hDomain The handle for this domain
 * @param[in]   hParentDomain The handle for the parent domain
 * @param[in]   pAccessControl The privileges of the domain
 */
NV_STATUS
domainConstruct
(
    RsDomain *pDomain,
    PORT_MEM_ALLOCATOR *pAllocator,
    NvHandle hDomain,
    NvHandle hParentDomain,
    ACCESS_CONTROL *pAccessControl
);

/**
 * Destruct a domain instance
 * @param[in]   pDomain The domain to destruct
 */
NV_STATUS
domainDestruct
(
    RsDomain *pDomain
);

/* @} */

#ifdef __cplusplus
}
#endif

#endif
