/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nv.h>
#include <nv-priv.h>
#include <nvos.h>

#if defined(DEBUG_REGISTRY)
#define DBG_REG_PRINTF(a, ...) \
    NV_PRINTF(LEVEL_INFO, a, ##__VA_ARGS__)
#else
#define DBG_REG_PRINTF(a, ...)
#endif

static NvS32 stringCaseCompare(
    const char *string1,
    const char *string2
)
{
    NvU8 c1, c2;

    do
    {
        c1 = *string1, c2 = *string2;
        if (c1 >= 'A' && c1 <= 'Z')
            c1 += ('a' - 'A');
        if (c2 >= 'A' && c2 <= 'Z')
            c2 += ('a' - 'A');
        string1++, string2++;
    }
    while ((c1 == c2) && (c1 != '\0'));

    return (c1 - c2);
}

static nv_reg_entry_t *the_registry = NULL;

static nv_reg_entry_t* regCreateNewRegistryKey(
    nv_state_t *nv,
    const char *regParmStr
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    nv_reg_entry_t *new_reg = NULL;
    char *new_ParmStr = NULL;
    NvU32 parm_size;

    if (regParmStr == NULL)
    {
        DBG_BREAKPOINT();
        return NULL;
    }

    new_reg = portMemAllocNonPaged(sizeof(nv_reg_entry_t));
    if (NULL == new_reg)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to grow registry\n");
        return NULL;
    }

    portMemSet(new_reg, 0, sizeof(nv_reg_entry_t));

    if (regParmStr != NULL)
    {
        parm_size = (portStringLength(regParmStr) + 1);
        new_ParmStr = portMemAllocNonPaged(parm_size);
        if (NULL == new_ParmStr)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to allocate registry param string\n");
            portMemFree(new_reg);
            return NULL;
        }

        NV_ASSERT(parm_size <= NVOS38_MAX_REGISTRY_STRING_LENGTH);

        if (portMemCopy(new_ParmStr, parm_size, regParmStr, parm_size) == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to copy registry param string\n");
            portMemFree(new_ParmStr);
            portMemFree(new_reg);
            return NULL;
        }
    }

    new_reg->regParmStr = new_ParmStr;
    new_reg->type       = NV_REGISTRY_ENTRY_TYPE_UNKNOWN;

    if (nvp != NULL)
    {
        new_reg->next = nvp->pRegistry;
        nvp->pRegistry = new_reg;
        DBG_REG_PRINTF("local registry now at 0x%p\n", nvp->pRegistry);
    }
    else
    {
        new_reg->next = the_registry;
        the_registry = new_reg;
        DBG_REG_PRINTF("global registry now at 0x%p\n", the_registry);
    }

    return new_reg;
}

static NV_STATUS regFreeEntry(nv_reg_entry_t *tmp)
{
    portMemFree(tmp->regParmStr);
    tmp->regParmStr = NULL;
    {
        portMemFree(tmp->pdata);
        tmp->pdata = NULL;
        tmp->len = 0;
    }
    portMemFree(tmp);

    return NV_OK;
}

static nv_reg_entry_t* regFindRegistryEntry(
    nv_state_t *nv,
    const char *regParmStr,
    NvU32       type,
    NvBool     *bGlobalEntry
)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    nv_reg_entry_t *tmp;

    DBG_REG_PRINTF("%s: %s\n", __FUNCTION__, regParmStr);

    if (nvp != NULL)
    {
        tmp = nvp->pRegistry;
        DBG_REG_PRINTF("   local registry at 0x%p\n", tmp);

        while ((tmp != NULL) && (tmp->regParmStr != NULL))
        {
            DBG_REG_PRINTF("  Testing against %s\n",
                    tmp->regParmStr);
            if ((stringCaseCompare(tmp->regParmStr, regParmStr) == 0) &&
                (type == tmp->type))
            {
                DBG_REG_PRINTF("    found a match!\n");
                if (bGlobalEntry)
                    *bGlobalEntry = NV_FALSE;
                return tmp;
            }
            tmp = tmp->next;
        }
    }

    tmp = the_registry;
    DBG_REG_PRINTF("   global registry at 0x%p\n", tmp);

    while ((tmp != NULL) && (tmp->regParmStr != NULL))
    {
        DBG_REG_PRINTF("  Testing against %s\n",
                tmp->regParmStr);
        if ((stringCaseCompare(tmp->regParmStr, regParmStr) == 0) &&
            (type == tmp->type))
        {
            DBG_REG_PRINTF("    found a match!\n");
            if (bGlobalEntry)
                *bGlobalEntry = NV_TRUE;
            return tmp;
        }
        tmp = tmp->next;
    }

    DBG_REG_PRINTF("  no match\n");
    return NULL;
}

NV_STATUS RmWriteRegistryDword(
    nv_state_t *nv,
    const char *regParmStr,
    NvU32       Data
)
{
    nv_reg_entry_t *tmp;
    NvBool bGlobalEntry;

    if (regParmStr == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    DBG_REG_PRINTF("%s: %s -> 0x%x\n", __FUNCTION__, regParmStr, Data);

    tmp = regFindRegistryEntry(nv, regParmStr,
                               NV_REGISTRY_ENTRY_TYPE_DWORD, &bGlobalEntry);

    // If we found an entry and we were looking for a global entry and
    // found a global, or we were looking for a per-GPU entry and found a
    // per-GPU entry
    if (tmp != NULL &&
        ((nv == NULL && bGlobalEntry) ||
         (nv != NULL && !bGlobalEntry)))
    {
        tmp->data = Data;

        if (stringCaseCompare(regParmStr, "ResmanDebugLevel") == 0)
        {
            os_dbg_set_level(Data);
        }

        return NV_OK;
    }

    tmp = regCreateNewRegistryKey(nv, regParmStr);
    if (tmp == NULL)
        return NV_ERR_GENERIC;

    tmp->type = NV_REGISTRY_ENTRY_TYPE_DWORD;
    tmp->data = Data;

    return NV_OK;
}

NV_STATUS RmReadRegistryDword(
    nv_state_t *nv,
    const char *regParmStr,
    NvU32      *Data
)
{
    nv_reg_entry_t *tmp;

    if ((regParmStr == NULL) || (Data == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    DBG_REG_PRINTF("%s: %s\n", __FUNCTION__, regParmStr);

    tmp = regFindRegistryEntry(nv, regParmStr,
                               NV_REGISTRY_ENTRY_TYPE_DWORD, NULL);
    if (tmp == NULL)
    {
        tmp = regFindRegistryEntry(nv, regParmStr,
                                   NV_REGISTRY_ENTRY_TYPE_BINARY, NULL);
        if ((tmp != NULL) && (tmp->len >= sizeof(NvU32)))
        {
            *Data = *(NvU32 *)tmp->pdata;
        }
        else
        {
            DBG_REG_PRINTF("   not found\n");
            return NV_ERR_GENERIC;
        }
    }
    else
    {
        *Data = tmp->data;
    }

    DBG_REG_PRINTF("  found in the_registry: 0x%x\n", *Data);

    return NV_OK;
}

NV_STATUS RmReadRegistryBinary(
    nv_state_t *nv,
    const char *regParmStr,
    NvU8       *Data,
    NvU32      *cbLen
)
{
    nv_reg_entry_t *tmp;
    NV_STATUS status;

    if ((regParmStr == NULL) || (Data == NULL) || (cbLen == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    DBG_REG_PRINTF("%s: %s\n", __FUNCTION__, regParmStr);

    tmp = regFindRegistryEntry(nv, regParmStr,
                               NV_REGISTRY_ENTRY_TYPE_BINARY, NULL);
    if (tmp == NULL)
    {
        DBG_REG_PRINTF("   not found\n");
        return NV_ERR_GENERIC;
    }

    DBG_REG_PRINTF("   found\n");

    if (*cbLen >= tmp->len)
    {
        portMemCopy((NvU8 *)Data, *cbLen, (NvU8 *)tmp->pdata, tmp->len);
        *cbLen = tmp->len;
        status = NV_OK;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "buffer (length: %u) is too small (data length: %u)\n",
                  *cbLen, tmp->len);
        status = NV_ERR_GENERIC;
    }

    return status;
}

NV_STATUS RmWriteRegistryBinary(
    nv_state_t *nv,
    const char *regParmStr,
    NvU8       *Data,
    NvU32       cbLen
)
{
    nv_reg_entry_t *tmp;
    NvBool bGlobalEntry;

    if ((regParmStr == NULL) || (Data == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    DBG_REG_PRINTF("%s: %s\n", __FUNCTION__, regParmStr);

    tmp = regFindRegistryEntry(nv, regParmStr,
                               NV_REGISTRY_ENTRY_TYPE_BINARY, &bGlobalEntry);

    // If we found an entry and we were looking for a global entry and
    // found a global, or we were looking for a per-GPU entry and found a
    // per-GPU entry
    if (tmp != NULL &&
        ((nv == NULL && bGlobalEntry) ||
         (nv != NULL && !bGlobalEntry)))
    {
        if (tmp->pdata != NULL)
        {
            portMemFree(tmp->pdata);
            tmp->pdata = NULL;
            tmp->len = 0;
        }
    }
    else
    {
        tmp = regCreateNewRegistryKey(nv, regParmStr);
        if (tmp == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to create binary registry entry\n");
            return NV_ERR_GENERIC;
        }
    }

    tmp->pdata = portMemAllocNonPaged(cbLen);
    if (NULL == tmp->pdata)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to write binary registry entry\n");
        return NV_ERR_GENERIC;
    }

    tmp->type = NV_REGISTRY_ENTRY_TYPE_BINARY;
    tmp->len = cbLen;
    portMemCopy((NvU8 *)tmp->pdata, tmp->len, (NvU8 *)Data, cbLen);

    return NV_OK;
}

NV_STATUS RmWriteRegistryString(
    nv_state_t *nv,
    const char *regParmStr,
    const char *buffer,
    NvU32       bufferLength
)
{
    nv_reg_entry_t *tmp;
    NvBool bGlobalEntry;

    if ((regParmStr == NULL) || (buffer == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    DBG_REG_PRINTF("%s: %s\n", __FUNCTION__, regParmStr);

    tmp = regFindRegistryEntry(nv, regParmStr,
                               NV_REGISTRY_ENTRY_TYPE_STRING, &bGlobalEntry);

    // If we found an entry and we were looking for a global entry and
    // found a global, or we were looking for a per-GPU entry and found a
    // per-GPU entry
    if (tmp != NULL &&
        ((nv == NULL && bGlobalEntry) ||
         (nv != NULL && !bGlobalEntry)))
    {
        if (tmp->pdata != NULL)
        {
            portMemFree(tmp->pdata);
            tmp->len = 0;
            tmp->pdata = NULL;
        }
    }
    else
    {
        tmp = regCreateNewRegistryKey(nv, regParmStr);
        if (tmp == NULL)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to allocate a string registry entry!\n");
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }

    tmp->pdata = portMemAllocNonPaged(bufferLength);
    if (tmp->pdata == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to write a string registry entry!\n");
        return NV_ERR_NO_MEMORY;
    }

    tmp->type = NV_REGISTRY_ENTRY_TYPE_STRING;
    tmp->len = bufferLength;
    portMemCopy((void *)tmp->pdata, tmp->len, buffer, (bufferLength - 1));
    tmp->pdata[bufferLength-1] = '\0';

    return NV_OK;
}

NV_STATUS RmReadRegistryString(
    nv_state_t *nv,
    const char *regParmStr,
    NvU8       *buffer,
    NvU32      *pBufferLength
)
{
    NvU32 bufferLength;
    nv_reg_entry_t *tmp;

    if ((regParmStr == NULL) || (buffer == NULL) || (pBufferLength == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    DBG_REG_PRINTF("%s: %s\n", __FUNCTION__, regParmStr);

    bufferLength = *pBufferLength;
    *pBufferLength = 0;
    *buffer = '\0';

    tmp = regFindRegistryEntry(nv, regParmStr,
                               NV_REGISTRY_ENTRY_TYPE_STRING, NULL);
    if (tmp == NULL)
    {
        return NV_ERR_GENERIC;
    }

    if (bufferLength >= tmp->len)
    {
        portMemCopy((void *)buffer, bufferLength, (void *)tmp->pdata, tmp->len);
        *pBufferLength = tmp->len;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "buffer (length: %u) is too small (data length: %u)\n",
                  bufferLength, tmp->len);
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    return NV_OK;
}

NV_STATUS RmInitRegistry(void)
{
    NV_STATUS rmStatus;

    rmStatus = os_registry_init();
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to initialize the OS registry!\n");
    }

    return rmStatus;
}

NV_STATUS RmDestroyRegistry(nv_state_t *nv)
{
    nv_priv_t *nvp = NV_GET_NV_PRIV(nv);
    nv_reg_entry_t *tmp;

    if (nvp != NULL)
    {
        tmp = nvp->pRegistry;
        nvp->pRegistry = NULL;
    }
    else
    {
        tmp = the_registry;
        the_registry = NULL;
    }

    while (tmp != NULL)
    {
        nv_reg_entry_t *entry = tmp;
        tmp = tmp->next;
        regFreeEntry(entry);
    }

    return NV_OK;
}

static void regCountEntriesAndSize(
    NvU32           *pNumEntries,     // Pointer to number of entries
    NvU32           *pSize,           // Pointer to total size
    nv_reg_entry_t  *pRegEntry        // Pointer local or global registry
)
{
    //
    // Note that *pNumEntries and *pSize are not initialized here.  This is so
    // we can accumulate totals of both global and local registries.
    //
    NvU32 numEntries = *pNumEntries;
    NvU32 size       = *pSize;

    while ((pRegEntry != NULL) && (pRegEntry->regParmStr != NULL))
    {
        size += portStringLength(pRegEntry->regParmStr) + 1 + pRegEntry->len;
        numEntries++;
        pRegEntry = pRegEntry->next;
    }

    *pNumEntries = numEntries;
    *pSize       = size;
}

static NV_STATUS regCopyEntriesToPackedBuffer(
    PACKED_REGISTRY_TABLE  *pRegTable,        // Pointer to packed record
    nv_reg_entry_t         *pRegEntry,        // Pointer local or global registry
    NvU32                  *pEntryIndex,      // Pointer to next index
    NvU32                  *pDataOffset       // Pointer to offset of next data byte.
)
{
    NvU8      *pByte      = (NvU8 *)pRegTable; // Byte version of record pointer.
    NV_STATUS  nvStatus   = NV_OK;
    NvU32      entryIndex = *pEntryIndex;
    NvU32      dataOffset = *pDataOffset;

    // Walk the records and copy the data.
    while ((pRegEntry != NULL) && (pRegEntry->regParmStr != NULL))
    {
        PACKED_REGISTRY_ENTRY *pEntry = &pRegTable->entries[entryIndex];
        NvU32 slen = portStringLength(pRegEntry->regParmStr) + 1;

        // Sanity check the data offset and index against counted totals.
        if ((dataOffset + slen + pRegEntry->len > pRegTable->size) ||
            (entryIndex >= pRegTable->numEntries))
        {
            // Something has changed since we counted them?
            NV_PRINTF(LEVEL_ERROR, "Registry entry record is full\n");
            nvStatus = NV_ERR_INVALID_STATE;
            break;
        }

        // Copy registry entry name to data blob.
        pEntry->nameOffset = dataOffset;
        portMemCopy(&pByte[dataOffset], slen, pRegEntry->regParmStr, slen);
        dataOffset += slen;

        switch (pRegEntry->type)
        {
            case NV_REGISTRY_ENTRY_TYPE_DWORD:
                pEntry->type   = REGISTRY_TABLE_ENTRY_TYPE_DWORD;
                pEntry->length = sizeof(NvU32);
                pEntry->data   = pRegEntry->data;
                break;

            case NV_REGISTRY_ENTRY_TYPE_BINARY:
            case NV_REGISTRY_ENTRY_TYPE_STRING:
                pEntry->type = REGISTRY_TABLE_ENTRY_TYPE_STRING;
                if (pRegEntry->type == NV_REGISTRY_ENTRY_TYPE_BINARY)
                    pEntry->type = REGISTRY_TABLE_ENTRY_TYPE_BINARY;

                if (pRegEntry->len == 0)
                {
                    NV_PRINTF(LEVEL_WARNING,
                                "Registry Key not sent to GSP-RM because it has 0 DataLength\n");
                    break;
                }
                pEntry->length   = pRegEntry->len;
                pEntry->data     = dataOffset;
                portMemCopy(&pByte[dataOffset], pEntry->length,
                            pRegEntry->pdata, pRegEntry->len);
                dataOffset += pRegEntry->len;
                break;

            default:
                // We should never get here.
                pEntry->type   = REGISTRY_TABLE_ENTRY_TYPE_UNKNOWN;
                pEntry->length = 0;
                pEntry->data   = 0;
                DBG_BREAKPOINT();
                break;
        }

        pRegEntry = pRegEntry->next;
        entryIndex++;
    }

    *pEntryIndex = entryIndex;
    *pDataOffset = dataOffset;
    return nvStatus;
}

// Package registry entries
NV_STATUS RmPackageRegistry(
    nv_state_t            *nv,
    PACKED_REGISTRY_TABLE *pRegTable,
    NvU32                 *pSize
)
{
    nv_priv_t             *nvp             = NV_GET_NV_PRIV(nv);
    nv_reg_entry_t        *pLocalRegistry  = NULL;
    NV_STATUS              nvStatus        = NV_OK;
    NvU32                  totalSize;
    NvU32                  numEntries;

    if (pSize == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    // Use the local (per-device) registry if we have one.
    if (nvp != NULL)
        pLocalRegistry = nvp->pRegistry;

    numEntries = 0;
    totalSize  = NV_OFFSETOF(PACKED_REGISTRY_TABLE, entries);

    // Count the number of global entries and total size.
    regCountEntriesAndSize(&numEntries, &totalSize, the_registry);

    // Count the number of local entries and total size.
    regCountEntriesAndSize(&numEntries, &totalSize, pLocalRegistry);

    // Add table record size into total size.
    totalSize += sizeof(PACKED_REGISTRY_ENTRY) * numEntries;

    //
    // If this function is called to only compute total size of registry table,
    // then we are done here.
    //
    if (pRegTable == NULL)
    {
        *pSize = totalSize;
        return NV_OK;
    }

    // Return warning if there are no registry entries.
    if (numEntries == 0)
        return NV_WARN_NOTHING_TO_DO;

    if (totalSize > *pSize)
    {
        NV_PRINTF(LEVEL_ERROR, "Registry entries overflow RPC record\n");
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    // Fill in our new structure with the first pass (counting) values.
    pRegTable->size       = totalSize;
    *pSize                = totalSize;
    pRegTable->numEntries = numEntries;

    // Offset of first byte after the registry entry table.
    totalSize  = NV_OFFSETOF(PACKED_REGISTRY_TABLE, entries) +
                 (sizeof(PACKED_REGISTRY_ENTRY) * numEntries);

    // Starting index in the registry entry table.
    numEntries = 0;

    // Walk the global registry and copy the data.
    nvStatus = regCopyEntriesToPackedBuffer(pRegTable,
        the_registry, &numEntries, &totalSize);

    // Walk the local registry and copy the data.
    if (nvStatus == NV_OK)
    {
        nvStatus = regCopyEntriesToPackedBuffer(pRegTable,
            pLocalRegistry, &numEntries, &totalSize);
    }

    // Sanity check second pass against first pass.
    if ((numEntries != pRegTable->numEntries) || (totalSize != pRegTable->size))
    {
        NV_PRINTF(LEVEL_ERROR, "First/second pass mismatch\n");
        nvStatus = NV_ERR_INVALID_STATE;
    }

    return nvStatus;
}

