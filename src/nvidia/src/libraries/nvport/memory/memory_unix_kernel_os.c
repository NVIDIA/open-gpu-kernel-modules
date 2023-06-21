/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
/**
 * @file
 * @brief MEMORY module implementation for Unix kernelmode
 *
 * This implementation uses the NVIDIA OS interface into the unix kernels.
 */

#if !PORT_IS_KERNEL_BUILD
#error "This file can only be compiled as part of the kernel build."
#endif
#if !NVOS_IS_UNIX
#error "This file can only be compiled on Unix."
#endif

#include "nvport/nvport.h"
#include "os-interface.h"

/**
 * @note All kernel memory in unix is non-paged.
 */
void *
_portMemAllocPagedUntracked
(
    NvLength lengthBytes
)
{
    return _portMemAllocNonPagedUntracked(lengthBytes);
}

void *
_portMemAllocNonPagedUntracked
(
    NvLength lengthBytes
)
{
    void *pMem = NULL;
    PORT_ASSERT_CHECKED(lengthBytes > 0);
    if (lengthBytes > 0)
        os_alloc_mem(&pMem, lengthBytes);
    return pMem;
}



void
_portMemFreeUntracked
(
    void *pData
)
{
    if (pData != NULL)
    {
        os_free_mem(pData);
    }
}

void *
portMemCopy
(
    void       *pDestination,
    NvLength    destSize,
    const void *pSource,
    NvLength    srcSize
)
{
    // API guarantees this is a NOP when destSize==0
    if (destSize == 0)
    {
        return pDestination;
    }

    PORT_ASSERT_CHECKED(pDestination != NULL);
    PORT_ASSERT_CHECKED(pSource      != NULL);
    PORT_ASSERT_CHECKED(srcSize      <= destSize);
    PORT_ASSERT_CHECKED(!portUtilCheckOverlap(pDestination, destSize,
                                              pSource,      srcSize));

    if ((pSource == NULL) || (pDestination == NULL) || (srcSize > destSize))
    {
        return NULL;
    }
    return os_mem_copy(pDestination, pSource, srcSize);
}


void *
portMemSet
(
    void    *pData,
    NvU8     value,
    NvLength lengthBytes
)
{
    if (lengthBytes == 0)
    {
        return pData;
    }
    if (pData == NULL)
    {
        return pData;
    }
    return os_mem_set(pData, value, lengthBytes);
}

NvS32
portMemCmp
(
    const void *pData0,
    const void *pData1,
    NvLength lengthBytes
)
{
    if (lengthBytes == 0)
    {
        return 0;
    }
    if ((pData0 == NULL) || (pData1 == NULL))
    {
        return -1;
    }
    return os_mem_cmp(pData0, pData1, lengthBytes);
}



#define PORT_MEM_USE_GENERIC_portMemSetPattern
#define PORT_MEM_USE_GENERIC_portMemMove
#include "memory_generic.h"

NV_STATUS
portMemExCopyFromUser
(
    const NvP64 pUser,
    void       *pKernel,
    NvLength    lengthBytes
)
{
    if (pKernel == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }
    if (lengthBytes == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    return os_memcpy_from_user(pKernel, NvP64_VALUE(pUser), lengthBytes);
}

NV_STATUS
portMemExCopyToUser
(
    const void *pKernel,
    NvP64       pUser,
    NvLength    lengthBytes
)
{
    if (pKernel == NULL)
    {
        return NV_ERR_INVALID_POINTER;
    }
    if (lengthBytes == 0)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    return os_memcpy_to_user(NvP64_VALUE(pUser), (void*)pKernel, lengthBytes);
}

NvLength
portMemExGetPageSize(void)
{
    return os_page_size;
}

// Large allocations (>KMALLOC_LIMIT) will fail, but it is safe to call
NvBool
portMemExSafeForPagedAlloc(void)
{
    return NV_TRUE;
}
NvBool
portMemExSafeForNonPagedAlloc(void)
{
    return NV_TRUE;
}
