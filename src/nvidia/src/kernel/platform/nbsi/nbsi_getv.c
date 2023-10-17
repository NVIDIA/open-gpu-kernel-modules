/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvlimits.h"
#include "gpu/gpu.h"

//----------------------------------------------------------------------------
//  NV_STATUS rtnNbsiElement(n, moduleID, thisPathHash, thisElementHash,
//                          pNbsiElement, * pRetBuf, * pRetSize, * pErrorCode)

//  This function searches the nbsi registry entry of the matching element.
//
//  Input parameters:
//      n            - index into element array (for debug output)
//      thisPathHash - pathHash (for debug output)
//      thisElementHash - element hash (for debug output)
//      pNbsiElement - pointer to this Element
//      pRetBuf      - pointer to return buffer
//      pRetSize     - pointer to return size
//      pErrorCode   - pointer to return error code
//
//  Returns NV_STATUS of NV_OK or NV_ERR_GENERIC.
//      NV_OK:      The table is present and no table integrity issues found
//      NV_ERR_GENERIC:   The table is not present or a table integrity issue was
//                  found.
//
//  Output parameters:
//      pRetSize     - If the element is found in the table pRetSize contains
//                    the size of the element (4 for byte/word/dword) or
//                    the array size. If the element size is larger than
//                    the pRetBuf size, the element is not copied but the size
//                    is updated.
//      pErrorCode   - status of search (when status = NV_OK)
//          NV2080_CTRL_BIOS_GET_NBSI_NOT_FOUND: element not present in table
//          NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE: element larger than pRetSize
//          NV2080_CTRL_BIOS_GET_NBSI_SUCCESS: element was found and placed
//                    at pRetBuf, pRetSize updated to size of element.
//                    Note: byte, word and dword entries in the table are all
//                    converted to a dword.
//
//----------------------------------------------------------------------------
static NV_STATUS rtnNbsiElement
(
    NvU32  n,
    NvU32  moduleID,
    NvU16  thisPathHash,
    NvU32  thisElementHash,
    PNBSI_ELEMENT pNbsiElement,
    NvU8   * pRetBuf,
    NvU32  * pRetSize,
    NvU32  * pErrorCode
)
{
    NvU32 elementSize;
    NvU32 rtnData;
    NvU16 nullTerm = 0;
    NvU8 * pRetBufEnd;

    NV_ASSERT(pErrorCode);
    NV_ASSERT(pRetSize);
    NV_ASSERT(!(*pRetSize != 0 && pRetBuf == NULL));
    //
    // We've found the element. Default return status to incomplete
    // to indicate entry doesn't fit return location. Later we'll
    // change that when we find out it does fit (and make the
    // successfull copy).
    //
    *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE;
    switch (pNbsiElement->hdrPartA.type)
    {
        case NBSI_BYTE_ARRAY:
            elementSize = pNbsiElement->data.ba.size;
            if (*pRetSize >= elementSize)
            {
                portMemCopy(pRetBuf, elementSize, &pNbsiElement->data.ba.data[0], elementSize);
                *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_SUCCESS;

                //
                // Null term strings but keep length same
                // Note: RM calls expect null termination (at least on
                //       strings). Calls from NvAPI via rm control
                //       apparently don't use the NULL.
                //       So NULL terminate if they have room to be
                //       fully compatible
                //
                if (*pRetSize >= elementSize+sizeof(nullTerm))
                {
                    pRetBufEnd = pRetBuf + elementSize;
                    portMemCopy(pRetBufEnd, nullTerm, &nullTerm, nullTerm);
                }
            }
            else
            {
                *pRetSize = elementSize + sizeof(nullTerm);
                *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE;
                return NV_OK;
            }
            break;
        case NBSI_BYTE_ARRAY_EXTENDED:
            elementSize = pNbsiElement->data.bax.size;
            if (*pRetSize >= elementSize)
            {
                portMemCopy(pRetBuf, elementSize, &pNbsiElement->data.bax.data[0], elementSize);
                *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_SUCCESS;
                //
                // Null term strings but keep length same
                // Note: RM calls expect null termination (at least on
                //       strings). Calls from NvAPI via rm control
                //       apparently don't use the NULL.
                //       So NULL terminate if they have room to be
                //       fully compatible
                //
                if (*pRetSize > elementSize+sizeof(nullTerm))
                {
                    pRetBufEnd = pRetBuf + elementSize;
                    portMemCopy(pRetBufEnd, nullTerm, &nullTerm, nullTerm);
                }
            }
            else
            {
                *pRetSize = elementSize + sizeof(nullTerm);
                *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE;
                return NV_OK;
            }
            break;
        case NBSI_BYTE:
            elementSize = 4;
            if (*pRetSize >= elementSize)
            {
                // assign byte to dword for proper fill of upper 3 bytes
                rtnData = pNbsiElement->data.dataByte;
                portMemCopy(pRetBuf, elementSize, &rtnData, elementSize);
                *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_SUCCESS;
            }
            break;
        case NBSI_WORD:
            elementSize = 4;
            if (*pRetSize >= elementSize)
            {
                // assign word to dword for proper fill of upper word
                rtnData = pNbsiElement->data.dataWord;
                portMemCopy(pRetBuf, elementSize, &rtnData, elementSize);
                *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_SUCCESS;
            }
            break;
        case NBSI_DWORD:
            elementSize = 4;
            if (*pRetSize >= elementSize)
            {
                portMemCopy(pRetBuf, elementSize, &pNbsiElement->data.dataDWord, elementSize);
                *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_SUCCESS;
            }
            break;
        case NBSI_QWORD:
            elementSize = 8;
            if (*pRetSize >= elementSize)
            {
                portMemCopy(pRetBuf, elementSize, &pNbsiElement->data.dataQWord, elementSize);
                *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_SUCCESS;
            }
            break;
        default:
            *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_NOT_FOUND;
            NV_PRINTF(LEVEL_ERROR,
                      "Invalid NBSI table entry (2)\n");
            NV_PRINTF(LEVEL_ERROR,
                      "Mod/Path/Elem=%x/%x/%x, ndx=%d, type = %d.\n",
                      moduleID, thisPathHash, thisElementHash,
                      n, pNbsiElement->hdrPartA.type);
            DBG_BREAKPOINT();
            return NV_ERR_GENERIC;
    }
    //
    // on success or failure, return the correct size so caller can
    // know how big it is (or needed to be).
    //
    *pRetSize = elementSize;
    return NV_OK;
}

//----------------------------------------------------------------------------
//  NV_STATUS getNbsiValue(pGpu, moduleID, PathHash, elementHash,
//                         pElementHashArray, pRetBuf,
//                         pRetSize, pErrorCode);
//
//  This function searches the nbsi registry table for an matching element.
//
//  Input parameters:
//      pGpu         - Gpu pointer
//      moduleID     - module ID for module match
//      PathHash     - 16 bit hash for path match
//      pElementHashArray - array of 20 bit hashes for search, ndx 0 is count
//      pRetBuf      - pointer to return buffer
//      pRetSize     - pointer to size of return buffer storage.
//
//  Returns NV_STATUS of NV_OK, NV_ERR_NOT_SUPPORTED, or NV_ERR_GENERIC.
//      NV_OK:      The table is present and no table integrity issues found
//      NV_ERR_NOT_SUPPORTED: The table is not present
//      NV_ERR_GENERIC:       A table integrity issue was found.
//
//  Output parameters:
//      pRetSize     - If the element is found in the table pRetSize contains
//                    the size of the element (4 for byte/word/dword) or
//                    the array size. If the element size is larger than
//                    the pRetBuf size, the element is not copied but the size
//                    is updated.
//      pErrorCode   - status of search (when status = NV_OK)
//           NV2080_CTRL_BIOS_GET_NBSI_NOT_FOUND: element not present in table
//           NV2080_CTRL_BIOS_GET_NBSI_INCOMPLETE: element larger than pRetSize
//           NV2080_CTRL_BIOS_GET_NBSI_SUCCESS: element was found and placed
//                    at pRetBuf, pRetSize updated to size of element.
//                    Note: byte, word and dword entries in the table are all
//                    converted to a dword.
//
//----------------------------------------------------------------------------

NV_STATUS getNbsiValue
(
    OBJGPU  *pGpu,
    NvU32    moduleID,
    NvU16    pathHash,
    NvU8     numElementHashes,
    NvU32  * pElementHash,
    NvU8   * pRetBuf,
    NvU32  * pRetSize,
    NvU32  * pErrorCode
)
{
    NBSI_OBJ        *pNbsiObj = getNbsiObject();
    NBSI_DRIVER_OBJ *pNbsiDriverObj;
    NBSI_MODULE     *pNbsiModule;
    NBSI_SCOPES     *pNbsiScopes = NULL;
    NBSI_ELEMENTS   *pNbsiElements = NULL;
    NBSI_ELEMENT    *pNbsiElement;
    NBSI_ELEMENT    *pNbsiElementFound[MAX_NBSI_OS];
    NV_STATUS        status = NV_OK;
    NvU32            n;
    NvBool           bFound;
    NvU32            elementSize;
    NvU32            idx;
    NvU8             i;
    NvU8             j;
    NvU32            valueID;
    NvU8           * tPtr;

    NV_ASSERT(pErrorCode);
    NV_ASSERT(pRetSize);
    NV_ASSERT(!(*pRetSize != 0 && pRetBuf == NULL));

    *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_NOT_FOUND;

    if (pGpu == NULL)
    {
        return NV_ERR_GENERIC;
    }

    idx = gpuGetInstance(pGpu);

    if (idx >= NV_MAX_DEVICES)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Invalid gpu index %d. Aborting nbsi get value.\n",
                  idx);
        return NV_ERR_GENERIC;
    }

    if (pNbsiObj->nbsiDrvrTable[idx] == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pNbsiDriverObj = (PNBSI_DRIVER_OBJ) pNbsiObj->nbsiDrvrTable[idx];
    if (pNbsiDriverObj->objHdr.globType == NBSI_DRIVER)
    {
        //
        // match the input moduleID with those in the table... if found, set
        // pointers to resultant module and scopes emtries.
        //
        n = 0;
        while ((pNbsiScopes == NULL) && (n < pNbsiDriverObj->numModules))
        {
            if (pNbsiDriverObj->modules[n].moduleID == moduleID)
            {
                pNbsiModule = &pNbsiDriverObj->modules[n];

                // Ignore link if the offset is special undefined offset.
                if (pNbsiModule->offset != NBSI_UNDEFINED_OFFSET)
                {
                    tPtr = (NvU8 *) &pNbsiModule->offset;
                    tPtr = &tPtr[pNbsiModule->offset];
                    pNbsiScopes = (PNBSI_SCOPES) tPtr;
                    NV_ASSERT(pNbsiScopes);
                }
            }
            n++;
        }
    }
    if (pNbsiScopes == NULL)
    {
       return NV_OK;
    }

    // Search through the scopes array to find a matching Scope (path) hash.
    n = 0;
    while ((pNbsiElements == NULL) && (n < pNbsiScopes->numPaths))
    {
       if (pNbsiScopes->paths[n].pathID == pathHash)
       {
           tPtr = (NvU8 *) &pNbsiScopes->paths[n].offset;
           tPtr = &tPtr[pNbsiScopes->paths[n].offset];
           pNbsiElements = (PNBSI_ELEMENTS) tPtr;
           NV_ASSERT(pNbsiElements);
       }
       n++;
    }
    if (pNbsiElements == NULL)
    {
       return NV_OK;
    }

    // Now find the element they want.
    bFound = NV_FALSE;
    n = 0;
    pNbsiElement = &pNbsiElements->elements[0];
    for (i = 0; i < numElementHashes; i++)
    {
        pNbsiElementFound[i] = NULL;
    }

    while (!bFound && (n < pNbsiElements->numElements))
    {
        valueID = pNbsiElement->hdrPartA.lsnValueID |
                  ((pNbsiElement->hdrPartB.mswValueID) << 4);
        //
        // Now check for a match of the Element hashes sent to us.
        // If multiple hashes are sent, index 0 holds the least specific.
        // Highest index is most specific ie. 2=Vista64, 1=Vista 0=generic
        // When multiple OSes drop out of loop on most specific now.
        // If only one OS is specified (or we know to use the generic
        // caller can reduce the count to search to 1.
        //
        for (i = 0; i < numElementHashes; i++)
        {
            if (valueID == pElementHash[i])
            {
                pNbsiElementFound[i] = pNbsiElement;
                if (i == numElementHashes-1)
                {
                    // Fall out if a match on most specific hash.
                    bFound = NV_TRUE;
                    break;
                }
            }
        }

        if (!bFound)
        {
            //
            // current element is not the one we want so move to the next one.
            // First calculate how large the current element is.
            //
            switch (pNbsiElement->hdrPartA.type)
            {
                case NBSI_BYTE:
                    elementSize = 1;
                    break;
                case NBSI_WORD:
                    elementSize = 2;
                    break;
                case NBSI_DWORD:
                    elementSize = 4;
                    break;
                case NBSI_QWORD:
                    elementSize = 8;
                    break;
                case NBSI_BYTE_ARRAY:
                    elementSize = pNbsiElement->data.ba.size + 1;
                    break;
                case NBSI_BYTE_ARRAY_EXTENDED:
                    elementSize = pNbsiElement->data.bax.size + 2;
                    break;
                default:
                    *pErrorCode = NV2080_CTRL_BIOS_GET_NBSI_NOT_FOUND;
                    NV_PRINTF(LEVEL_ERROR,
                              "Invalid NBSI table entry (1).\n");
                    NV_PRINTF(LEVEL_ERROR,
                              "Mod/Path/El=%x/%x/%x n/hash/typ=%d/%x/%d.\n",
                              moduleID, pathHash,
                              pElementHash[0], n, valueID,
                              pNbsiElement->hdrPartA.type);
                    DBG_BREAKPOINT();
                    return NV_ERR_GENERIC;
            }
            tPtr = (NvU8 *) pNbsiElement;
            tPtr = &tPtr[elementSize + NBSI_ELEMENT_HDRSIZ];
            pNbsiElement = (PNBSI_ELEMENT) tPtr;
            NV_ASSERT(pNbsiElement);
        }
        n++;
    }

    // Check if item was found (bFound set only if first OS)
    for (i = 0; i < numElementHashes; i++)
    {
        // Search for match from most specific up.
        j = numElementHashes - i - 1;
        if (pNbsiElementFound[j] != NULL)
        {
            status = rtnNbsiElement(
                             n,
                             moduleID,
                             pathHash,
                             pElementHash[j],
                             pNbsiElementFound[j],
                             pRetBuf,
                             pRetSize,
                             pErrorCode);
            break;
        }
    }

    return status;
}

