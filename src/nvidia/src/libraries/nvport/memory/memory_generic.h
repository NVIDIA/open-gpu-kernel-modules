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

/**
 * @file
 * @brief MEMORY module tracking functions implementation
 *
 */

#include "nvport/nvport.h"

#if defined(PORT_MEM_USE_GENERIC_portMemSetPattern)
void *
portMemSetPattern
(
    void       *pData,
    NvLength    lengthBytes,
    const NvU8 *pPattern,
    NvLength    patternBytes
)
{
    PORT_ASSERT_CHECKED(pData       != NULL);
    PORT_ASSERT_CHECKED(pPattern    != NULL);
    PORT_ASSERT_CHECKED(patternBytes > 0);

    if (lengthBytes > 0)
    {
        void *p = pData;
        while (lengthBytes > patternBytes)
        {
            portMemCopy(p, patternBytes, pPattern, patternBytes);
            p = (NvU8*)p + patternBytes;
            lengthBytes -= patternBytes;
        }
        portMemCopy(p, lengthBytes, pPattern, lengthBytes);
    }
    return pData;
}
#endif

#if defined(PORT_MEM_USE_GENERIC_portMemMove)
void *
portMemMove
(
    void       *pDestination,
    NvLength    destSize,
    const void *pSource,
    NvLength    srcSize
)
{
    NvU32 *pDst32;
    NvU8 *pDst8;
    const NvU32 *pSrc32;
    const NvU8 *pSrc8;
    NvLength dwords = 0;
    NvLength bytes = srcSize;
    PORT_ASSERT_CHECKED(pDestination != NULL);
    PORT_ASSERT_CHECKED(pSource      != NULL);
    PORT_ASSERT_CHECKED(srcSize      <= destSize);

    if (pDestination == NULL || pSource == NULL || srcSize > destSize)
    {
        return NULL;
    }

    if (pDestination == pSource)
    {
        return pDestination;
    }

    if ((((NvUPtr)pSource & 3) == 0) && (((NvUPtr)pDestination & 3) == 0))
    {
        dwords = srcSize / sizeof(NvU32);
        bytes  = srcSize % sizeof(NvU32);
    }

    if (pDestination > pSource)
    {
        pDst8 = (NvU8*)pDestination + srcSize;
        pSrc8 = (const NvU8*)pSource + srcSize;

        while (bytes--)
        {
            PORT_MEM_WR08(--pDst8, PORT_MEM_RD08(--pSrc8));
        }
        pDst32 = (NvU32*)pDst8;
        pSrc32 = (const NvU32*)pSrc8;
        while (dwords--)
        {
            PORT_MEM_WR32(--pDst32, PORT_MEM_RD32(--pSrc32));
        }
    }
    else
    {
        pDst32 = (NvU32*)pDestination;
        pSrc32 = (const NvU32*)pSource;

        while (dwords--)
        {
            PORT_MEM_WR32(pDst32++, PORT_MEM_RD32(pSrc32++));
        }
        pDst8 = (NvU8*)pDst32;
        pSrc8 = (const NvU8*)pSrc32;
        while (bytes--)
        {
            PORT_MEM_WR08(pDst8++, PORT_MEM_RD08(pSrc8++));
        }
    }
    return pDestination;
}
#endif

#if defined(PORT_MEM_USE_GENERIC_portMemCopy)
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
        return pDestination;

    PORT_ASSERT_CHECKED(!portUtilCheckOverlap((const NvU8*)pDestination, destSize,
                                              (const NvU8*)pSource,      srcSize));
    return portMemMove(pDestination, destSize, pSource, srcSize);
}
#endif


#if defined(PORT_MEM_USE_GENERIC_portMemCmp)
NvS32
portMemCmp
(
    const void *pData0,
    const void *pData1,
    NvLength    lengthBytes
)
{
    const NvU8 *p0 = (const NvU8*)pData0;
    const NvU8 *p1 = (const NvU8*)pData1;
    PORT_ASSERT_CHECKED(pData0 != NULL);
    PORT_ASSERT_CHECKED(pData1 != NULL);
    PORT_ASSERT_CHECKED(lengthBytes > 0);
    while (lengthBytes--)
    {
        NvU8 u0 = PORT_MEM_RD08(p0++);
        NvU8 u1 = PORT_MEM_RD08(p1++);
        if (u0 != u1)
            return u0 - u1;
    }
    return 0;
}
#endif

#if defined(PORT_MEM_USE_GENERIC_portMemSet)
void *
portMemSet
(
    void *pData,
    NvU8 value,
    NvLength lengthBytes
)
{
    NvLength i;
    for (i = 0; i < lengthBytes; i++)
    {
        PORT_MEM_WR08(((NvU8 *)pData)+i, value);
    }
    return pData;
}
#endif
