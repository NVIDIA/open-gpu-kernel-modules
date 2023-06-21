/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "platform/chipset/pci_pbi.h"
#include "rmpbicmdif.h"
#include "os/os.h"
#include "gpu/gpu_uuid.h"

// Max number of retries when polling for command completion (1ms per poll)
#define PCI_PBI_POLL_STATIC              100        // Poll iterations for static PBI
// Longer poll time if we know PBI is supported by device.  GA100 can take 1.2sec
#define PCI_PBI_POLL_ENABLED            1300        // Poll iterations if Linked/Enabled

//
// Going forward we expect to find PBI in the PCI capability list if PBI is
// supported by the device. However, there are GPUs using a fixed offset at
// 0xb4, and if the PCI capability is not found then we still want to support
// devices that use the static offset.
//
#define PCI_CAPABILITY_LIST_BASE        0x34       // base of the PCI capability list
#define PCI_PBI_STATIC_CAP_BASE         0xb4       // Base of static PBI capability
#define PCI_PBI_CAP_ADDR(base, offset) ((base == 0 ? PCI_PBI_STATIC_CAP_BASE : base) + offset)

// PBI register offsets, from the base of the PCI capability
#define PCI_PBI_REG_PBCI                0x00       // PBI cap ID register
#define PCI_PBI_REG_PBLS                0x02       // PBI cap list size register
#define PCI_PBI_REG_PBCC                0x03       // PBI cap control register
#define PCI_PBI_REG_COMMAND             0x04       // PBI command register
#define PCI_PBI_REG_DATAIN              0x08       // PBI data in register
#define PCI_PBI_REG_DATAOUT             0x0c       // PBI data out register
#define PCI_PBI_REG_MUTEX               0x10       // PBI mutex register

// Expected PCI capability values if the Post-Box interface exists for this card
#define PCI_PBI_CAP_ID                  0x09       // PCI Capability ID for PBI
#define PCI_PBI_REG_PBLS_LENGTH         0x14       // Expected length of the capability
#define PCI_PBI_REG_PBCC_ENABLED        0x01       // Set when PBI is supported

//
// Check to see if Post-box interface is found in PCI config space at the
// specified base address
//
// If the PBI capability was found in the PCI cap list then cap_base is the
// PCI config offset of the capability.
// If the capability was not found in the PCI cap list then cap_base is zero, but
// we still check for PBI at a static location to support certain GPUs in the field.
//
static NV_STATUS pciPbiCheck(void *handle, NvU32 cap_base)
{
    if ((osPciReadByte(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_PBCI)) == PCI_PBI_CAP_ID) &&
        (osPciReadByte(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_PBLS)) == PCI_PBI_REG_PBLS_LENGTH) &&
        (osPciReadByte(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_PBCC)) == PCI_PBI_REG_PBCC_ENABLED))
        return NV_OK;

    return NV_ERR_NOT_SUPPORTED;
}

//
// Find the base of the PCI PBI capability and return the base.
// Returns 0 if PBI is not found in the PCI cap list.
//
static NvU32 pciPbiFindCapability(void *handle)
{
    // Start at the beginning of the PCI capability list
    NvU32 cap_base = osPciReadByte(handle, PCI_CAPABILITY_LIST_BASE);

    // Walk the PCI capability list looking for a match for PBI
    while (cap_base != 0 && pciPbiCheck(handle, cap_base) != NV_OK)
        cap_base = osPciReadByte(handle, cap_base + 1);

    return cap_base;
}

//
// Attempt to acquire Post-box interface mutex
//
static NV_STATUS pciPbiAcquireMutex(void *handle, NvU32 cap_base)
{
    NvU32 id;

    // check to see if mutex is available
    id = osPciReadDword(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_MUTEX));
    if (id == PBI_CLIENT_NONE)
    {
        // attempt to acquire
        osPciWriteDword(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_MUTEX),
                        PBI_CLIENT_DRIVER_PCIPBI_SHIM);

        // check to see if acquired
        id = osPciReadDword(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_MUTEX));
        if (id == PBI_CLIENT_DRIVER_PCIPBI_SHIM)
            return NV_OK;
    }

    return NV_ERR_IN_USE;
}

//
// Release Post-box interface mutex
//
static void pciPbiReleaseMutex(void *handle, NvU32 cap_base)
{
    NvU32 id;

    // check to see if we own the mutex
    id = osPciReadDword(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_MUTEX));
    if (id != PBI_CLIENT_DRIVER_PCIPBI_SHIM)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Attempted to release PBI mutex that does not match client ID\n");
        NV_ASSERT(0);
        return;
    }

    osPciWriteDword(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_MUTEX), PBI_CLIENT_NONE);
}

//
// Convert PBI error to NV_STATUS error
//
static NV_STATUS pciPbiError(int status)
{
    switch (status)
    {
        case NV_PBI_COMMAND_STATUS_SUCCESS:
        case NV_PBI_COMMAND_STATUS_MORE_DATA:
            return NV_OK;
        case NV_PBI_COMMAND_STATUS_INVALID_ADDRESS:
        case NV_PBI_COMMAND_STATUS_INVALID_COMMAND:
            return NV_ERR_INVALID_COMMAND;
        case NV_PBI_COMMAND_STATUS_BUSY:
        case NV_PBI_COMMAND_STATUS_PENDING:
            return NV_ERR_IN_USE;
        default:
            return NV_ERR_GENERIC;
    }
}

//
// Polling waiting for PBI command completion
//
static NV_STATUS pciPbiCheckStatusWait(void *handle, NvU32 cap_base)
{
    NvU32 cmdStatus;
    NvU32 status;
    NvU32 intr;
    NvU32 i;
    NvU32 poll_limit;
    NvU16 devid;
    // WAR 2844019
    static const NvU16 ampere_devid[] = {0x2080, 0x2081, 0x2082, 0x20B0, 0x20B1, 0x20B2,
                                         0x20B3, 0x20B4, 0x20B5, 0x20B6, 0x20B7, 0x20BB,
                                         0x20BE, 0x20BF, 0x20C0, 0x20C2, 0x20F0, 0x20F1,
                                         0x20F2, 0x20F3, 0x20FE, 0x20FF, 0};


    // Allow longer poll time when we know PBI is supported
    if (cap_base != 0)
    {
        poll_limit = PCI_PBI_POLL_ENABLED;
    }
    else
    {
        poll_limit = PCI_PBI_POLL_STATIC;

        // WAR for 2844918, extra delay is needed for early Ampere GA100 devices
        // which do not have PBI correctly linked in the PCI Capability list
        devid = osPciReadWord(handle, 0x2);
        for (i = 0; ampere_devid[i] != 0; i++)
        {
            if (devid == ampere_devid[i])
            {
                poll_limit = PCI_PBI_POLL_ENABLED;
                break;
            }
        }
        // End WAR
    }

    // poll for command completion
    for (i = 0; i < poll_limit; i++)
    {
        cmdStatus = osPciReadDword(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_COMMAND));
        status = DRF_VAL(_PBI, _COMMAND, _STATUS, cmdStatus);
        intr = DRF_VAL(_PBI, _COMMAND, _INTERRUPT, cmdStatus);
        if (intr)
        {
            osDelay(1);
            continue;
        }
        switch (status)
        {
            case NV_PBI_COMMAND_STATUS_UNDEFINED:
            case NV_PBI_COMMAND_STATUS_BUSY:
            case NV_PBI_COMMAND_STATUS_PENDING:
                osDelay(1);
                continue;
            default:
                return pciPbiError(status);
        }
    }

    return NV_ERR_TIMEOUT;
}

//
// Send PBI command and poll waiting for completion
//
static
NV_STATUS pciPbiSendCommandWait(void *handle, NvU32 cap_base, NvU32 command, NvU32 dataIn,
                                NvU32 *dataOut)
{
    NV_STATUS status;

    // send command
    osPciWriteDword(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_DATAIN), dataIn);
    osPciWriteDword(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_COMMAND), command);

    // wait for command status
    status = pciPbiCheckStatusWait(handle, cap_base);

    if (status == NV_OK)
    {
        // read dataout
        *dataOut = osPciReadDword(handle, PCI_PBI_CAP_ADDR(cap_base, PCI_PBI_REG_DATAOUT));
    }

    return status;
}

//
// Read 16-byte raw UUID/GID via Post-Box interface
//
// Note: The temporary buffer 'gid' is used here to avoid the expense of
// a possible unaligned exception trap when moving the data.  The data
// returned by PBI is a DWORD, and we don't know the alignment of the
// user buffer.
//
NV_STATUS pciPbiReadUuid(void *handle, NvU8 *uuid)
{
    NV_STATUS status;
    NvU32 command;
    NvU32 capability;
    NvU32 gid[RM_SHA1_GID_SIZE/4];
    NvU32 i;
    NvU32 cap_base;

    if (uuid == NULL || handle == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    cap_base = pciPbiFindCapability(handle);

    // see if PBI exists
    status = pciPbiCheck(handle, cap_base);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Device does not support PBI\n");
        return status;
    }

    // acquire PBI mutex
    status = pciPbiAcquireMutex(handle, cap_base);
    if (status != NV_OK) {
        NV_PRINTF(LEVEL_ERROR, "Could not acquire pciPbi mutex\n");
        return status;
    }

    // get PBI capabilities
    PBI_SET_COMMAND_PARAMS(0, 0, 0, 1, NV_PBI_COMMAND_FUNC_ID_GET_CAPABILITIES,
                           0, 0, 0, 1, command);
    status = pciPbiSendCommandWait(handle, cap_base, command, 0, &capability);
    if (status != NV_OK)
    {
        //
        // A device can pretend to have PBI support, using a fake PCI config space entry,
        // even though it does not respond to PBI calls. Return NV_ERR_NOT_SUPPORTED
        // for such devices if NV_PBI_COMMAND_FUNC_ID_GET_CAPABILITIES fails to respond.
        //
        NV_PRINTF(LEVEL_INFO,
                  "Device did not respond to PBI GET_CAPABILITIES\n");
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    // see if the PBI supports Execute Routine
    if ((capability & NVBIT(NV_PBI_COMMAND_FUNC_ID_EXECUTE_ROUTINE)) == 0)
    {
        NV_PRINTF(LEVEL_INFO,
                  "Device does not support PBI Execute Routine\n");
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    // get the UUID (4 words)
    for (i = 0; i < (sizeof gid)/(sizeof gid[0]); i++)
    {
        PBI_SET_COMMAND_PARAMS(0, 0, i, 1, NV_PBI_COMMAND_FUNC_ID_EXECUTE_ROUTINE,
                               0, 0, 0, 1, command);
        status = pciPbiSendCommandWait(handle, cap_base, command,
                                       NV_PBI_EXECUTE_ROUTINE_GET_GID, &gid[i]);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failure reading GID\n");
            goto done;
        }
    }

    portMemCopy(uuid, RM_SHA1_GID_SIZE, gid, RM_SHA1_GID_SIZE);

done:
    pciPbiReleaseMutex(handle, cap_base);
    return status;
}

//
// Get Feature support via Post-Box interface
//
NV_STATUS pciPbiGetFeature(void *handle, NvU32 *feature)
{
    NV_STATUS status;
    NvU32 command;
    NvU32 cap_base;

    if (feature == NULL || handle == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    cap_base = pciPbiFindCapability(handle);

    // see if PBI exists
    status = pciPbiCheck(handle, cap_base);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Device does not support PBI\n");
        return status;
    }

    // acquire PBI mutex
    status = pciPbiAcquireMutex(handle, cap_base);
    if (status != NV_OK) {
        NV_PRINTF(LEVEL_ERROR, "Could not acquire pciPbi mutex\n");
        return status;
    }

    // get feature dword
    PBI_SET_COMMAND_PARAMS(0, 0, 0, 1, NV_PBI_COMMAND_FUNC_ID_EXECUTE_ROUTINE,
                           0, 0, 0, 1, command);
    status = pciPbiSendCommandWait(handle, cap_base, command,
                                   NV_PBI_EXECUTE_ROUTINE_GET_FEATURE, feature);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Device did not provide PBI GET FEATURE, %0x\n",
                  status);
    }

    pciPbiReleaseMutex(handle, cap_base);
    return status;
}
