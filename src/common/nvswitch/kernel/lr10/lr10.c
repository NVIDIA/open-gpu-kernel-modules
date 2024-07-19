/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"
#include "bios_nvswitch.h"
#include "error_nvswitch.h"
#include "regkey_nvswitch.h"
#include "haldef_nvswitch.h"
#include "lr10/lr10.h"
#include "lr10/clock_lr10.h"
#include "lr10/minion_lr10.h"
#include "lr10/soe_lr10.h"
#include "lr10/pmgr_lr10.h"
#include "lr10/therm_lr10.h"
#include "lr10/inforom_lr10.h"
#include "lr10/smbpbi_lr10.h"
#include "flcn/flcnable_nvswitch.h"
#include "soe/soe_nvswitch.h"
#include "lr10/cci_lr10.h"

#include "nvswitch/lr10/dev_nvs_top.h"
#include "nvswitch/lr10/dev_pri_ringmaster.h"
#include "nvswitch/lr10/dev_pri_ringstation_sys.h"
#include "nvswitch/lr10/dev_nvlsaw_ip.h"
#include "nvswitch/lr10/dev_nvlsaw_ip_addendum.h"
#include "nvswitch/lr10/dev_nvs_master.h"
#include "nvswitch/lr10/dev_nvltlc_ip.h"
#include "nvswitch/lr10/dev_nvldl_ip.h"
#include "nvswitch/lr10/dev_nvlipt_lnk_ip.h"
#include "nvswitch/lr10/dev_nvlctrl_ip.h"
#include "nvswitch/lr10/dev_npg_ip.h"
#include "nvswitch/lr10/dev_npgperf_ip.h"
#include "nvswitch/lr10/dev_nport_ip.h"
#include "nvswitch/lr10/dev_ingress_ip.h"
#include "nvswitch/lr10/dev_tstate_ip.h"
#include "nvswitch/lr10/dev_egress_ip.h"
#include "nvswitch/lr10/dev_route_ip.h"
#include "nvswitch/lr10/dev_therm.h"
#include "nvswitch/lr10/dev_soe_ip.h"
#include "nvswitch/lr10/dev_route_ip_addendum.h"
#include "nvswitch/lr10/dev_minion_ip.h"
#include "nvswitch/lr10/dev_minion_ip_addendum.h"
#include "nvswitch/lr10/dev_nport_ip_addendum.h"
#include "nvswitch/lr10/dev_nxbar_tile_ip.h"
#include "nvswitch/lr10/dev_nxbar_tc_global_ip.h"
#include "nvswitch/lr10/dev_sourcetrack_ip.h"

#include "oob/smbpbi.h"

#define DMA_ADDR_WIDTH_LR10     64
#define ROUTE_GANG_TABLE_SIZE (1 << DRF_SIZE(NV_ROUTE_REG_TABLE_ADDRESS_INDEX))

static void
_nvswitch_deassert_link_resets_lr10
(
    nvswitch_device *device
)
{
    NvU32 val, i;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;

    NVSWITCH_PRINT(device, WARN,
        "%s: NVSwitch Driver is taking the links out of reset. This should only happen during forced config.\n",
        __FUNCTION__);

    for (i = 0; i < NVSWITCH_LINK_COUNT(device); i++)
    {
        if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLIPT_LNK, i)) continue;

        val = NVSWITCH_LINK_RD32_LR10(device, i,
                NVLIPT_LNK, _NVLIPT_LNK, _RESET_RSTSEQ_LINK_RESET);
        val = FLD_SET_DRF_NUM(_NVLIPT_LNK, _RESET_RSTSEQ_LINK_RESET, _LINK_RESET,
                          NV_NVLIPT_LNK_RESET_RSTSEQ_LINK_RESET_LINK_RESET_DEASSERT, val);

        NVSWITCH_LINK_WR32_LR10(device, i,
                NVLIPT_LNK, _NVLIPT_LNK, _RESET_RSTSEQ_LINK_RESET, val);
    }

    for (i = 0; i < NVSWITCH_LINK_COUNT(device); i++)
    {
        if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLIPT_LNK, i)) continue;

        // Poll for _RESET_STATUS == _DEASSERTED
        nvswitch_timeout_create(25*NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);

        do
        {
            keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

            val = NVSWITCH_LINK_RD32_LR10(device, i,
                    NVLIPT_LNK, _NVLIPT_LNK, _RESET_RSTSEQ_LINK_RESET);
            if (FLD_TEST_DRF(_NVLIPT_LNK, _RESET_RSTSEQ_LINK_RESET,
                        _LINK_RESET_STATUS, _DEASSERTED, val))
            {
                break;
            }

            nvswitch_os_sleep(1);
        }
        while (keepPolling);

        if (!FLD_TEST_DRF(_NVLIPT_LNK, _RESET_RSTSEQ_LINK_RESET,
                    _LINK_RESET_STATUS, _DEASSERTED, val))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout waiting for link %d_LINK_RESET_STATUS == _DEASSERTED\n",
                __FUNCTION__, i);
                // Bug 2974064: Review this timeout handling (fall through)
        }
    }
}

static void
_nvswitch_train_forced_config_link_lr10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    NvU32 data, i;
    nvlink_link *link;

    link = nvswitch_get_link(device, linkId);

    if ((link == NULL) ||
        !NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber) ||
        (linkId >= NVSWITCH_NVLINK_MAX_LINKS))
    {
        return;
    }

    data = NVSWITCH_LINK_RD32_LR10(device, linkId, NVLDL, _NVLDL_TOP, _LINK_TEST);
    data = FLD_SET_DRF(_NVLDL_TOP, _LINK_TEST, _AUTO_HWCFG, _ENABLE, data);
    NVSWITCH_LINK_WR32_LR10(device, linkId, NVLDL, _NVLDL_TOP, _LINK_TEST, data);

    // Add some delay to let the sim/emu go to SAFE
    NVSWITCH_NSEC_DELAY(400 * NVSWITCH_INTERVAL_1USEC_IN_NS);

    data = NVSWITCH_LINK_RD32_LR10(device, linkId, NVLDL, _NVLDL_TOP, _LINK_TEST);
    data = FLD_SET_DRF(_NVLDL_TOP, _LINK_TEST, _AUTO_NVHS, _ENABLE, data);
    NVSWITCH_LINK_WR32_LR10(device, linkId, NVLDL, _NVLDL_TOP, _LINK_TEST, data);

    // Add some delay to let the sim/emu go to HS
    NVSWITCH_NSEC_DELAY(400 * NVSWITCH_INTERVAL_1USEC_IN_NS);

    data = NVSWITCH_LINK_RD32_LR10(device, linkId, NVLDL, _NVLDL_TOP, _LINK_CHANGE);
    data = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _NEWSTATE,      _ACTIVE, data);
    data = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _OLDSTATE_MASK, _DONTCARE, data);
    data = FLD_SET_DRF(_NVLDL_TOP, _LINK_CHANGE, _ACTION,        _LTSSM_CHANGE, data);
    NVSWITCH_LINK_WR32_LR10(device, linkId, NVLDL, _NVLDL_TOP, _LINK_CHANGE, data);

    i = 0;

    // Poll until LINK_CHANGE[1:0] != 2b01.
    while (i < 5)
    {
        data = NVSWITCH_LINK_RD32_LR10(device, linkId, NVLDL, _NVLDL_TOP, _LINK_CHANGE);

        if (FLD_TEST_DRF(_NVLDL_TOP, _LINK_CHANGE, _STATUS, _BUSY, data))
        {
            NVSWITCH_PRINT(device, INFO,
                "%s : Waiting for link %d to go to ACTIVE\n",
                __FUNCTION__, linkId);
        }
        else if (FLD_TEST_DRF(_NVLDL_TOP, _LINK_CHANGE, _STATUS, _FAULT, data))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s : Fault while changing LINK to ACTIVE. Link = %d\n",
                __FUNCTION__, linkId);
            break;
        }
        else
        {
            break;
        }

        NVSWITCH_NSEC_DELAY(5 * NVSWITCH_INTERVAL_1USEC_IN_NS);
        i++;
    }

    data = NVSWITCH_LINK_RD32_LR10(device, linkId, NVLDL, _NVLDL_TOP, _LINK_STATE);

    if (FLD_TEST_DRF(_NVLDL_TOP, _LINK_STATE, _STATE, _ACTIVE, data))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s : Link %d is in ACTIVE state, setting BUFFER_READY\n",
            __FUNCTION__, linkId);

        // Set buffer ready only for nvlink TLC and not NPORT
        nvswitch_init_buffer_ready(device, link, NV_FALSE);
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s : Timeout while waiting for link %d to go to ACTIVE\n",
            __FUNCTION__, linkId);
        NVSWITCH_PRINT(device, ERROR,
            "%s : Link %d is in 0x%x state\n",
            __FUNCTION__, linkId,DRF_VAL(_NVLDL_TOP, _LINK_STATE, _STATE, data));
    }

}

void
_nvswitch_setup_chiplib_forced_config_lr10
(
    nvswitch_device *device
)
{
    NvU64 links = ((NvU64)device->regkeys.chiplib_forced_config_link_mask) +
                  ((NvU64)device->regkeys.chiplib_forced_config_link_mask2 << 32);
    NvU32 i;

    if (links == 0)
    {
        return;
    }

    //
    // First, take the links out of reset
    //
    // NOTE: On LR10, MINION will take the links out of reset during INITPHASE1
    // On platforms where MINION is not present and/or we want to run with forced
    // config, the driver must de-assert the link reset
    //
    _nvswitch_deassert_link_resets_lr10(device);

    // Next, train the links to ACTIVE/NVHS
    FOR_EACH_INDEX_IN_MASK(64, i, links)
    {
        if (device->link[i].valid)
        {
            _nvswitch_train_forced_config_link_lr10(device, i);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;
}

/*!
 * @brief Parse packed little endian data and unpack into padded structure
 *
 * @param[in]   format          Data format
 * @param[in]   packedData      Packed little endian data
 * @param[out]  unpackedData    Unpacked padded structure
 * @param[out]  unpackedSize    Unpacked data size
 * @param[out]  fieldsCount     Number of fields
 *
 * @return 'NV_OK'
 */
NV_STATUS
_nvswitch_devinit_unpack_structure
(
    const char *format,
    const NvU8 *packedData,
    NvU32      *unpackedData,
    NvU32      *unpackedSize,
    NvU32      *fieldsCount
)
{
    NvU32 unpkdSize = 0;
    NvU32 fields = 0;
    NvU32 count;
    NvU32 data;
    char fmt;

    while ((fmt = *format++))
    {
        count = 0;
        while ((fmt >= '0') && (fmt <= '9'))
        {
            count *= 10;
            count += fmt - '0';
            fmt = *format++;
        }
        if (count == 0)
            count = 1;

        while (count--)
        {
            switch (fmt)
            {
                case 'b':
                    data = *packedData++;
                    unpkdSize += 1;
                    break;

                case 's':    // signed byte
                    data = *packedData++;
                    if (data & 0x80)
                        data |= ~0xff;
                    unpkdSize += 1;
                    break;

                case 'w':
                    data  = *packedData++;
                    data |= *packedData++ << 8;
                    unpkdSize += 2;
                    break;

                case 'd':
                    data  = *packedData++;
                    data |= *packedData++ << 8;
                    data |= *packedData++ << 16;
                    data |= *packedData++ << 24;
                    unpkdSize += 4;
                    break;

                default:
                    return NV_ERR_GENERIC;
            }
            *unpackedData++ = data;
            fields++;
        }
    }

    if (unpackedSize != NULL)
        *unpackedSize = unpkdSize;

    if (fieldsCount != NULL)
        *fieldsCount = fields;

    return NV_OK;
}

/*!
 * @brief Calculate packed and unpacked data size based on given data format
 *
 * @param[in]   format          Data format
 * @param[out]  packedSize      Packed data size
 * @param[out]  unpackedSize    Unpacked data size
 *
 */
void
_nvswitch_devinit_calculate_sizes
(
    const char *format,
    NvU32      *packedSize,
    NvU32      *unpackedSize
)
{
    NvU32 unpkdSize = 0;
    NvU32 pkdSize = 0;
    NvU32 count;
    char fmt;

    while ((fmt = *format++))
    {
        count = 0;
        while ((fmt >= '0') && (fmt <= '9'))
        {
            count *= 10;
            count += fmt - '0';
            fmt = *format++;
        }
        if (count == 0)
            count = 1;

        switch (fmt)
        {
            case 'b':
                pkdSize += count * 1;
                unpkdSize += count * sizeof(bios_U008);
                break;

            case 's':    // signed byte
                pkdSize += count * 1;
                unpkdSize += count * sizeof(bios_S008);
                break;

            case 'w':
                pkdSize += count * 2;
                unpkdSize += count * sizeof(bios_U016);
                break;

            case 'd':
                pkdSize += count * 4;
                unpkdSize += count * sizeof(bios_U032);
                break;
        }
    }

    if (packedSize != NULL)
        *packedSize = pkdSize;

    if (unpackedSize != NULL)
        *unpackedSize = unpkdSize;
}

/*!
 * @brief Calculate packed and unpacked data size based on given data format
 *
 * @param[in]   format          Data format
 * @param[out]  packedSize      Packed data size
 * @param[out]  unpackedSize    Unpacked data size
 *
 */

NV_STATUS
_nvswitch_vbios_read_structure
(
    nvswitch_device *device,
    void            *structure,
    NvU32           offset,
    NvU32           *ppacked_size,
    const char      *format
)
{
    NvU32  packed_size;
    NvU8  *packed_data;
    NvU32  unpacked_bytes;

    // calculate the size of the data as indicated by its packed format.
    _nvswitch_devinit_calculate_sizes(format, &packed_size, &unpacked_bytes);

    if (ppacked_size)
        *ppacked_size = packed_size;

    //
    // is 'offset' too big?
    // happens when we read bad ptrs from fixed addrs in image frequently
    //
    if ((offset + packed_size) > device->biosImage.size)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Bad offset in bios read: 0x%x, max is 0x%x, fmt is '%s'\n",
                       __FUNCTION__, offset, device->biosImage.size, format);
        return NV_ERR_GENERIC;
    }

    packed_data = &device->biosImage.pImage[offset];
    return _nvswitch_devinit_unpack_structure(format, packed_data, structure,
                                              &unpacked_bytes, NULL);
}


NvlStatus
nvswitch_vbios_read_structure_lr10
(
    nvswitch_device *device,
    void            *structure,
    NvU32           offset,
    NvU32           *ppacked_size,
    const char      *format
)
{
    if (NV_OK == _nvswitch_vbios_read_structure(device, structure, offset, ppacked_size, format))
    {
       return NVL_SUCCESS;
    }
    else
    {
       return -NVL_ERR_GENERIC;
    }
}

NvU8
_nvswitch_vbios_read8
(
    nvswitch_device *device,
    NvU32           offset
)
{
    bios_U008 data;     // BiosReadStructure expects 'bios' types

    _nvswitch_vbios_read_structure(device, &data, offset, (NvU32 *) 0, "b");

    return (NvU8) data;
}

NvU16
_nvswitch_vbios_read16
(
    nvswitch_device *device,
    NvU32           offset
)
{
    bios_U016 data;     // BiosReadStructure expects 'bios' types

    _nvswitch_vbios_read_structure(device, &data, offset, (NvU32 *) 0, "w");

    return (NvU16) data;
}


NvU32
_nvswitch_vbios_read32
(
    nvswitch_device *device,
    NvU32           offset
)
{
    bios_U032 data;     // BiosReadStructure expects 'bios' types

    _nvswitch_vbios_read_structure(device, &data, offset, (NvU32 *) 0, "d");

    return (NvU32) data;
}

NV_STATUS
_nvswitch_perform_BIT_offset_update
(
    nvswitch_device *device,
    NVSWITCH_BIOS_NVLINK_CONFIG *bios_config
)
{
    BIT_HEADER_V1_00         bitHeader;
    BIT_TOKEN_V1_00          bitToken;
    NV_STATUS                rmStatus;
    NvU32                    dataPointerOffset;
    NvU32 i;

    rmStatus = _nvswitch_vbios_read_structure(device,
                                              (NvU8*) &bitHeader,
                                              bios_config->bit_address,
                                              (NvU32 *) 0,
                                              BIT_HEADER_V1_00_FMT);

    if(rmStatus != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
                       "%s: Failed to read BIT table structure!.\n",
                       __FUNCTION__);
        return rmStatus;
    }
   
    for(i=0; i < bitHeader.TokenEntries; i++)
    {
        NvU32 BitTokenLocation = bios_config->bit_address + bitHeader.HeaderSize + (i * bitHeader.TokenSize);
        rmStatus = _nvswitch_vbios_read_structure(device,
                                                 (NvU8*) &bitToken,
                                                 BitTokenLocation,
                                                 (NvU32 *) 0,
                                                 BIT_TOKEN_V1_00_FMT);
        if(rmStatus != NV_OK)
        {
            NVSWITCH_PRINT(device, WARN,
                "%s: Failed to read BIT token %d!\n",
                __FUNCTION__, i);
            return NV_ERR_GENERIC;
        }

        dataPointerOffset = (bios_config->pci_image_address + bitToken.DataPtr);
        switch(bitToken.TokenId)
        {
            case BIT_TOKEN_NVINIT_PTRS:
            {
                BIT_DATA_NVINIT_PTRS_V1 nvInitTablePtrs;
                rmStatus = _nvswitch_vbios_read_structure(device,
                                                          (NvU8*) &nvInitTablePtrs,
                                                          dataPointerOffset,
                                                          (NvU32 *) 0,
                                                          BIT_DATA_NVINIT_PTRS_V1_30_FMT);
                if (rmStatus != NV_OK)
                {
                    NVSWITCH_PRINT(device, WARN,
                                   "%s: Failed to read internal data structure\n",
                                   __FUNCTION__);
                    return NV_ERR_GENERIC;
                }
                // Update the retrived info with device info
                bios_config->nvlink_config_table_address = (nvInitTablePtrs.NvlinkConfigDataPtr + bios_config->pci_image_address);
            }
            break;
        }
    }

    return NV_OK;
}

NV_STATUS
_nvswitch_validate_BIT_header
(
    nvswitch_device *device,
    NvU32            bit_address
)
{
    NvU32    headerSize = 0;
    NvU32    chkSum = 0;
    NvU32    i;

    //
    // For now let's assume the Header Size is always at the same place.
    // We can create something more complex if needed later.
    //
    headerSize = (NvU32)_nvswitch_vbios_read8(device, bit_address + BIT_HEADER_SIZE_OFFSET);

    // Now perform checksum
    for (i = 0; i < headerSize; i++)
        chkSum += (NvU32)_nvswitch_vbios_read8(device, bit_address + i);

    //Byte checksum removes upper bytes
    chkSum = chkSum & 0xFF;

    if (chkSum)
        return NV_ERR_GENERIC;

    return NV_OK;
}


NV_STATUS
nvswitch_verify_header
(
    nvswitch_device *device,
    NVSWITCH_BIOS_NVLINK_CONFIG *bios_config
)
{
    NvU32       i;
    NV_STATUS   status = NV_ERR_GENERIC;

    if ((bios_config == NULL) || (!bios_config->pci_image_address))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: PCI Image offset is not identified\n",
            __FUNCTION__);
        return status;
    }

    // attempt to find the init info in the BIOS
    for (i = bios_config->pci_image_address; i < device->biosImage.size - 3; i++)
    {
        NvU16 bitheaderID = _nvswitch_vbios_read16(device, i);
        if (bitheaderID == BIT_HEADER_ID)
        {
            NvU32 signature = _nvswitch_vbios_read32(device, i + 2);
            if (signature == BIT_HEADER_SIGNATURE)
            {
                bios_config->bit_address = i;

                // Checksum BIT to prove accuracy
                if (NV_OK != _nvswitch_validate_BIT_header(device, bios_config->bit_address))
                {
                    device->biosImage.pImage = 0;
                    device->biosImage.size = 0;
                }
            }
        }
        // only if we find the bit address do we break
        if (bios_config->bit_address)
            break;
    }
    if (bios_config->bit_address)
    {
        status = NV_OK;
    }

    return status;
}

NV_STATUS
_nvswitch_vbios_update_bit_Offset
(
    nvswitch_device *device,
    NVSWITCH_BIOS_NVLINK_CONFIG *bios_config
)
{
    NV_STATUS   status = NV_OK;

    if (bios_config->bit_address)
    {
        goto vbios_update_bit_Offset_done;
    }

    status = nvswitch_verify_header(device, bios_config);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: *** BIT header is not found in vbios!\n",
            __FUNCTION__);
        goto vbios_update_bit_Offset_done;
    }

    if (bios_config->bit_address)
    {

        status = _nvswitch_perform_BIT_offset_update(device, bios_config);
        if (status != NV_OK)
            goto vbios_update_bit_Offset_done;
    }

vbios_update_bit_Offset_done:
    return status;
}


NV_STATUS
_nvswitch_vbios_identify_pci_image_loc
(
    nvswitch_device         *device,
    NVSWITCH_BIOS_NVLINK_CONFIG *bios_config
)
{
    NV_STATUS   status = NV_OK;
    NvU32       i;

    if (bios_config->pci_image_address)
    {
        goto vbios_identify_pci_image_loc_done;
    }

    // Match the PCI_EXP_ROM_SIGNATURE and followed by the PCI Data structure
    // with PCIR and matching vendor ID
    NVSWITCH_PRINT(device, SETUP,
        "%s: Verifying and extracting PCI Data.\n",
        __FUNCTION__);

    // attempt to find the init info in the BIOS
    for (i = 0; i < (device->biosImage.size - PCI_ROM_HEADER_PCI_DATA_SIZE); i++)
    {
        NvU16 pci_rom_sigature = _nvswitch_vbios_read16(device, i);

        if (pci_rom_sigature == PCI_EXP_ROM_SIGNATURE)
        {
            NvU32 pcir_data_dffSet  = _nvswitch_vbios_read16(device, i + PCI_ROM_HEADER_SIZE);  // 0x16 -> 0x18 i.e, including the ROM Signature bytes

            if (((i + pcir_data_dffSet) + PCI_DATA_STRUCT_SIZE) < device->biosImage.size)
            {
                NvU32 pcirSigature = _nvswitch_vbios_read32(device, (i + pcir_data_dffSet));

                if (pcirSigature == PCI_DATA_STRUCT_SIGNATURE)
                {
                    PCI_DATA_STRUCT pciData;
                    status = _nvswitch_vbios_read_structure(device,
                                                           (NvU8*) &pciData,
                                                            i + pcir_data_dffSet,
                                                            (NvU32 *) 0,
                                                            PCI_DATA_STRUCT_FMT);
                    if (status != NV_OK)
                    {
                        NVSWITCH_PRINT(device, WARN,
                                       "%s: Failed to PCI Data for validation\n",
                                       __FUNCTION__);
                        goto vbios_identify_pci_image_loc_done;
                    }

                    // Validate the vendor details as well
                    if (pciData.vendorID == PCI_VENDOR_ID_NVIDIA)
                    {
                        bios_config->pci_image_address = i;
                        break;
                    }
                }
            }
        }
    }

vbios_identify_pci_image_loc_done:
    return status;
}

NvU32 _nvswitch_get_nvlink_config_address
(
    nvswitch_device         *device,
    NVSWITCH_BIOS_NVLINK_CONFIG *bios_config
)
{
    return bios_config->nvlink_config_table_address;
}

NV_STATUS
_nvswitch_read_vbios_link_base_entry
(
    nvswitch_device *device,
    NvU32            tblPtr,
    NVLINK_CONFIG_DATA_BASEENTRY  *link_base_entry
)
{
    NV_STATUS status = NV_ERR_INVALID_PARAMETER;
    NVLINK_VBIOS_CONFIG_DATA_BASEENTRY vbios_link_base_entry;

    status = _nvswitch_vbios_read_structure(device, &vbios_link_base_entry, tblPtr, (NvU32 *)0, NVLINK_CONFIG_DATA_BASEENTRY_FMT);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Error on reading nvlink base entry\n",
            __FUNCTION__);
        return status;
    }

    link_base_entry->positionId = vbios_link_base_entry.positionId;

    return status;
}

NvlStatus
nvswitch_read_vbios_link_entries_lr10
(
    nvswitch_device              *device,
    NvU32                         tblPtr,
    NvU32                         expected_link_entriesCount,
    NVLINK_CONFIG_DATA_LINKENTRY *link_entries,
    NvU32                        *identified_link_entriesCount
)
{
    NV_STATUS status = NV_ERR_INVALID_PARAMETER;
    NvU32 i;
    NVLINK_VBIOS_CONFIG_DATA_LINKENTRY_20 vbios_link_entry;
    *identified_link_entriesCount = 0;

    for (i = 0; i < expected_link_entriesCount; i++)
    {
        status = _nvswitch_vbios_read_structure(device,
                                                &vbios_link_entry,
                                                tblPtr, (NvU32 *)0,
                                                NVLINK_CONFIG_DATA_LINKENTRY_FMT_20);
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Error on reading nvlink entry\n",
                __FUNCTION__);
            return status;
        }
        link_entries[i].nvLinkparam0 = (NvU8)vbios_link_entry.nvLinkparam0;
        link_entries[i].nvLinkparam1 = (NvU8)vbios_link_entry.nvLinkparam1;
        link_entries[i].nvLinkparam2 = (NvU8)vbios_link_entry.nvLinkparam2;
        link_entries[i].nvLinkparam3 = (NvU8)vbios_link_entry.nvLinkparam3;
        link_entries[i].nvLinkparam4 = (NvU8)vbios_link_entry.nvLinkparam4;
        link_entries[i].nvLinkparam5 = (NvU8)vbios_link_entry.nvLinkparam5;
        link_entries[i].nvLinkparam6 = (NvU8)vbios_link_entry.nvLinkparam6;
        tblPtr += (sizeof(NVLINK_VBIOS_CONFIG_DATA_LINKENTRY_20)/sizeof(NvU32));

        NVSWITCH_PRINT(device, NOISY,
            "<<<---- NvLink ID 0x%x ---->>>\n", i);
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 0 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam0, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam0));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 1 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam1, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam1));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 2 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam2, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam2));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 3 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam3, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam3));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 4 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam4, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam4));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 5 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam5, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam5));
        NVSWITCH_PRINT(device, NOISY,
            "NVLink Params 6 \t0x%x \tBinary:"BYTE_TO_BINARY_PATTERN"\n", vbios_link_entry.nvLinkparam6, BYTE_TO_BINARY(vbios_link_entry.nvLinkparam6));
        NVSWITCH_PRINT(device, NOISY,
            "<<<---- NvLink ID 0x%x ---->>>\n\n", i);
    }
    *identified_link_entriesCount = i;
    return status;
}

NV_STATUS
_nvswitch_vbios_fetch_nvlink_entries
(
    nvswitch_device         *device,
    NVSWITCH_BIOS_NVLINK_CONFIG *bios_config
)
{
    NvU32                       tblPtr;
    NvU8                        version;
    NvU8                        size;
    NV_STATUS                   status = NV_ERR_GENERIC;
    NVLINK_CONFIG_DATA_HEADER   header;
    NvU32                       base_entry_index;
    NvU32                       expected_base_entry_count;

    tblPtr = _nvswitch_get_nvlink_config_address(device, bios_config);
    if (!tblPtr)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: No NvLink Config table set\n",
            __FUNCTION__);
        goto vbios_fetch_nvlink_entries_done;
    }

    // Read the table version number
    version = _nvswitch_vbios_read8(device, tblPtr);
    switch (version)
    {
        case NVLINK_CONFIG_DATA_HEADER_VER_20:
        case NVLINK_CONFIG_DATA_HEADER_VER_30:
            size = _nvswitch_vbios_read8(device, tblPtr + 1);
            if (size == NVLINK_CONFIG_DATA_HEADER_20_SIZE)
            {
                // Grab Nvlink Config Data Header
                status = _nvswitch_vbios_read_structure(device, &header.ver_20, tblPtr, (NvU32 *) 0, NVLINK_CONFIG_DATA_HEADER_20_FMT);

                if (status != NV_OK)
                {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s: Error on reading the nvlink config header\n",
                        __FUNCTION__);
                }
            }
            break;
        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s: Invalid version 0x%x\n",
                __FUNCTION__, version);
    }
    if (status != NV_OK)
    {
        goto vbios_fetch_nvlink_entries_done;
    }

    NVSWITCH_PRINT(device, NOISY,
        "<<<---- NvLink Header ---->>>\n\n");
    NVSWITCH_PRINT(device, NOISY,
        "Version \t\t 0x%x\n", header.ver_20.Version);
    NVSWITCH_PRINT(device, NOISY,
        "Header Size \t0x%x\n", header.ver_20.HeaderSize);
    NVSWITCH_PRINT(device, NOISY,
        "Base Entry Size \t0x%x\n", header.ver_20.BaseEntrySize);
    NVSWITCH_PRINT(device, NOISY,
        "Base Entry count \t0x%x\n", header.ver_20.BaseEntryCount);
    NVSWITCH_PRINT(device, NOISY,
        "Link Entry Size \t0x%x\n", header.ver_20.LinkEntrySize);
    NVSWITCH_PRINT(device, NOISY,
        "Link Entry Count \t0x%x\n", header.ver_20.LinkEntryCount);
    NVSWITCH_PRINT(device, NOISY,
        "Reserved \t0x%x\n", header.ver_20.Reserved);
    NVSWITCH_PRINT(device, NOISY,
        "<<<---- NvLink Header ---->>>\n");
    if (header.ver_20.Version == NVLINK_CONFIG_DATA_HEADER_VER_20)
    {
         device->bIsNvlinkVbiosTableVersion2 = NV_TRUE;
    }
    expected_base_entry_count = header.ver_20.BaseEntryCount;
    if (expected_base_entry_count > NVSWITCH_NUM_BIOS_NVLINK_CONFIG_BASE_ENTRY)
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: Greater than expected base entry count 0x%x - Restricting to count 0x%x\n",
            __FUNCTION__, expected_base_entry_count, NVSWITCH_NUM_BIOS_NVLINK_CONFIG_BASE_ENTRY);
        expected_base_entry_count = NVSWITCH_NUM_BIOS_NVLINK_CONFIG_BASE_ENTRY;
    }

    tblPtr += header.ver_20.HeaderSize;
    for (base_entry_index = 0; base_entry_index < expected_base_entry_count; base_entry_index++)
    {
        NvU32 expected_link_entriesCount = header.ver_20.LinkEntryCount;
        if (expected_link_entriesCount > NVSWITCH_LINK_COUNT(device))
        {
            NVSWITCH_PRINT(device, WARN,
                "%s: Greater than expected link count 0x%x - Restricting to count 0x%x\n",
                __FUNCTION__, expected_link_entriesCount, NVSWITCH_LINK_COUNT(device));
            expected_link_entriesCount = NVSWITCH_LINK_COUNT(device);
        }

        // Grab Nvlink Config Data Base Entry
        _nvswitch_read_vbios_link_base_entry(device, tblPtr, &bios_config->link_vbios_base_entry[base_entry_index]);
        tblPtr += header.ver_20.BaseEntrySize;
        device->hal.nvswitch_read_vbios_link_entries(device,
                                          tblPtr,
                                          expected_link_entriesCount,
                                          bios_config->link_vbios_entry[base_entry_index],
                                          &bios_config->identified_Link_entries[base_entry_index]);

        if (device->bIsNvlinkVbiosTableVersion2)
        {
            tblPtr += (expected_link_entriesCount * (sizeof(NVLINK_VBIOS_CONFIG_DATA_LINKENTRY_20)/sizeof(NvU32)));
        }
        else
        {
            tblPtr += (expected_link_entriesCount * (sizeof(NVLINK_VBIOS_CONFIG_DATA_LINKENTRY_30)/sizeof(NvU32)));
        }
    }
vbios_fetch_nvlink_entries_done:
    return status;
}

NV_STATUS
_nvswitch_vbios_assign_base_entry
(
    nvswitch_device         *device,
    NVSWITCH_BIOS_NVLINK_CONFIG    *bios_config
)
{
    NvU32 physical_id;
    NvU32 entry_index;

    physical_id = nvswitch_read_physical_id(device);

    for (entry_index = 0; entry_index < NVSWITCH_NUM_BIOS_NVLINK_CONFIG_BASE_ENTRY; entry_index++)
    {
        if (physical_id == bios_config->link_vbios_base_entry[entry_index].positionId)
        {
            bios_config->link_base_entry_assigned = entry_index;
            return NV_OK;
        }
    }

    // TODO: Bug 3507948
    NVSWITCH_PRINT(device, ERROR,
            "%s: Error on assigning base entry. Setting base entry index = 0\n",
            __FUNCTION__);
    bios_config->link_base_entry_assigned = 0;

    return NV_OK;
}

NV_STATUS
_nvswitch_setup_link_vbios_overrides
(
    nvswitch_device *device,
    NVSWITCH_BIOS_NVLINK_CONFIG *bios_config
)
{
    NV_STATUS    status         = NV_OK;

    if (bios_config == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
                "%s: BIOS config override not supported\n",
                __FUNCTION__);
         return -NVL_ERR_NOT_SUPPORTED;
    }

    bios_config->vbios_disabled_link_mask = 0;

    bios_config->bit_address                 = 0;
    bios_config->pci_image_address           = 0;
    bios_config->nvlink_config_table_address = 0;

    if ((device->biosImage.size == 0) || (device->biosImage.pImage == NULL))
    {
        NVSWITCH_PRINT(device, ERROR,
                "%s: VBIOS not exist size:0x%x\n",
                __FUNCTION__, device->biosImage.size);
         return -NVL_ERR_NOT_SUPPORTED;
    }

    //
    // Locate the PCI ROM Image
    //
    if (_nvswitch_vbios_identify_pci_image_loc(device, bios_config)  != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Error on identifying pci image loc\n",
            __FUNCTION__);
        status = NV_ERR_GENERIC;
        goto setup_link_vbios_overrides_done;
    }

    //
    // Locate and fetch BIT offset
    //
    if (_nvswitch_vbios_update_bit_Offset(device, bios_config) != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Error on identifying pci image loc\n",
            __FUNCTION__);
        status = NV_ERR_GENERIC;
        goto setup_link_vbios_overrides_done;
    }

    //
    // Fetch NvLink Entries
    //
    if (_nvswitch_vbios_fetch_nvlink_entries(device, bios_config) != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Error on fetching nvlink entries\n",
            __FUNCTION__);
        status = NV_ERR_GENERIC;
        goto setup_link_vbios_overrides_done;
    }

    //
    // Assign Base Entry for this device
    //
    if (_nvswitch_vbios_assign_base_entry(device, bios_config) != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Error on assigning base entry\n",
            __FUNCTION__);
        status = NV_ERR_GENERIC;
        goto setup_link_vbios_overrides_done;
    }

setup_link_vbios_overrides_done:
    if (status != NV_OK)
    {
        bios_config->bit_address                = 0;
        bios_config->pci_image_address          = 0;
        bios_config->nvlink_config_table_address =0;
    }
    return status;
}

/*
 * @Brief : Setting up system registers after device initialization
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 */
NvlStatus
nvswitch_setup_system_registers_lr10
(
    nvswitch_device *device
)
{
    nvlink_link *link;
    NvU8 i;
    NvU64 enabledLinkMask;

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);

    FOR_EACH_INDEX_IN_MASK(64, i, enabledLinkMask)
    {
        NVSWITCH_ASSERT(i < NVSWITCH_LINK_COUNT(device));

        link = nvswitch_get_link(device, i);

        if ((link == NULL) ||
            !NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber) ||
            (i >= NVSWITCH_NVLINK_MAX_LINKS))
        {
            continue;
        }

        nvswitch_setup_link_system_registers(device, link);
        nvswitch_load_link_disable_settings(device, link);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_deassert_link_reset_lr10
(
    nvswitch_device *device,
    nvlink_link     *link
)
{
    NvU64 mode;
    NvlStatus status = NVL_SUCCESS;

    status = device->hal.nvswitch_corelib_get_dl_link_mode(link, &mode);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                "%s:DL link mode failed on link %d\n",
                __FUNCTION__, link->linkNumber);
        return status;
    }

    // Check if the link is RESET
    if (mode != NVLINK_LINKSTATE_RESET)
    {
        return NVL_SUCCESS;
    }

    // Send INITPHASE1 to bring link out of reset
    status = link->link_handlers->set_dl_link_mode(link,
                                        NVLINK_LINKSTATE_INITPHASE1,
                                        NVLINK_STATE_CHANGE_ASYNC);

    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                "%s: INITPHASE1 failed on link %d\n",
                __FUNCTION__, link->linkNumber);
    }

    return status;
}

static NvU32
_nvswitch_get_num_vcs_lr10
(
    nvswitch_device *device
)
{
    return NVSWITCH_NUM_VCS_LR10;
}

void
nvswitch_determine_platform_lr10
(
    nvswitch_device *device
)
{
    NvU32 value;

    //
    // Determine which model we are using SMC_BOOT_2 and OS query
    //
    value = NVSWITCH_REG_RD32(device, _PSMC, _BOOT_2);
    device->is_emulation = FLD_TEST_DRF(_PSMC, _BOOT_2, _EMULATION, _YES, value);

    if (!IS_EMULATION(device))
    {
        // If we are not on fmodel, we must be on RTL sim or silicon
        if (FLD_TEST_DRF(_PSMC, _BOOT_2, _FMODEL, _YES, value))
        {
            device->is_fmodel = NV_TRUE;
        }
        else
        {
            device->is_rtlsim = NV_TRUE;

            // Let OS code finalize RTL sim vs silicon setting
            nvswitch_os_override_platform(device->os_handle, &device->is_rtlsim);
        }
    }

#if defined(NVLINK_PRINT_ENABLED)
    {
        const char *build;
        const char *mode;

        build = "HW";
        if (IS_FMODEL(device))
            mode = "fmodel";
        else if (IS_RTLSIM(device))
            mode = "rtlsim";
        else if (IS_EMULATION(device))
            mode = "emulation";
        else
            mode = "silicon";

        NVSWITCH_PRINT(device, SETUP,
            "%s: build: %s platform: %s\n",
             __FUNCTION__, build, mode);
    }
#endif // NVLINK_PRINT_ENABLED
}

static void
_nvswitch_portstat_reset_latency_counters
(
    nvswitch_device *device
)
{
    // Set SNAPONDEMAND from 0->1 to reset the counters
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _NPORT, _PORTSTAT_SNAP_CONTROL,
        DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _STARTCOUNTER, _ENABLE) |
        DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _SNAPONDEMAND, _ENABLE));

    // Set SNAPONDEMAND back to 0.
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _NPORT, _PORTSTAT_SNAP_CONTROL,
        DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _STARTCOUNTER, _ENABLE) |
        DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _SNAPONDEMAND, _DISABLE));
}

//
// Data collector which runs on a background thread, collecting latency stats.
//
// The latency counters have a maximum window period of 3.299 seconds
// (2^32 clk cycles). The counters reset after this period. So SW snaps
// the bins and records latencies every 3 seconds. Setting SNAPONDEMAND from 0->1
// snaps the  latency counters and updates them to PRI registers for
// the SW to read. It then resets the counters to start collecting fresh latencies.
//

void
nvswitch_internal_latency_bin_log_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 idx_nport;
    NvU32 idx_vc;
    NvBool vc_valid;
    NvU32 latency;
    NvU64 time_nsec;
    NvU32 link_type;    // Access or trunk link
    NvU64 last_visited_time_nsec;

    if (chip_device->latency_stats == NULL)
    {
        // Latency stat buffers not allocated yet
        return;
    }

    time_nsec = nvswitch_os_get_platform_time();
    last_visited_time_nsec = chip_device->latency_stats->last_visited_time_nsec;

    // Update last visited time
    chip_device->latency_stats->last_visited_time_nsec = time_nsec;

    // Compare time stamp and reset the counters if the snap is missed
    if (!IS_RTLSIM(device) || !IS_FMODEL(device))
    {
        if ((last_visited_time_nsec != 0) &&
            ((time_nsec - last_visited_time_nsec) > 3 * NVSWITCH_INTERVAL_1SEC_IN_NS))
        {
            NVSWITCH_PRINT(device, ERROR,
                "Latency metrics recording interval missed.  Resetting counters.\n");
            _nvswitch_portstat_reset_latency_counters(device);
            return;
        }
    }

    for (idx_nport=0; idx_nport < NVSWITCH_LINK_COUNT(device); idx_nport++)
    {
        if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, idx_nport))
        {
            continue;
        }

        // Setting SNAPONDEMAND from 0->1 snaps the latencies and resets the counters
        NVSWITCH_LINK_WR32_LR10(device, idx_nport, NPORT, _NPORT, _PORTSTAT_SNAP_CONTROL,
            DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _STARTCOUNTER, _ENABLE) |
            DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _SNAPONDEMAND, _ENABLE));

        //
        // TODO: Check _STARTCOUNTER and don't log if counter not enabled.
        // Currently all counters are always enabled
        //

        link_type = NVSWITCH_LINK_RD32_LR10(device, idx_nport, NPORT, _NPORT, _CTRL);
        for (idx_vc = 0; idx_vc < NVSWITCH_NUM_VCS_LR10; idx_vc++)
        {
            vc_valid = NV_FALSE;

            // VC's CREQ0(0) and RSP0(5) are relevant on access links.
            if (FLD_TEST_DRF(_NPORT, _CTRL, _TRUNKLINKENB, _ACCESSLINK, link_type) &&
                ((idx_vc == NV_NPORT_VC_MAPPING_CREQ0) ||
                (idx_vc == NV_NPORT_VC_MAPPING_RSP0)))
            {
                vc_valid = NV_TRUE;
            }

            // VC's CREQ0(0), RSP0(5), CREQ1(6) and RSP1(7) are relevant on trunk links.
            if (FLD_TEST_DRF(_NPORT, _CTRL, _TRUNKLINKENB, _TRUNKLINK, link_type) &&
                ((idx_vc == NV_NPORT_VC_MAPPING_CREQ0)  ||
                 (idx_vc == NV_NPORT_VC_MAPPING_RSP0)   ||
                 (idx_vc == NV_NPORT_VC_MAPPING_CREQ1)  ||
                 (idx_vc == NV_NPORT_VC_MAPPING_RSP1)))
            {
                vc_valid = NV_TRUE;
            }

            // If the VC is not being used, skip reading it
            if (!vc_valid)
            {
                continue;
            }

            latency = NVSWITCH_NPORT_PORTSTAT_RD32_LR10(device, idx_nport, _COUNT, _LOW, idx_vc);
            chip_device->latency_stats->latency[idx_vc].accum_latency[idx_nport].low += latency;

            latency = NVSWITCH_NPORT_PORTSTAT_RD32_LR10(device, idx_nport, _COUNT, _MEDIUM, idx_vc);
            chip_device->latency_stats->latency[idx_vc].accum_latency[idx_nport].medium += latency;

            latency = NVSWITCH_NPORT_PORTSTAT_RD32_LR10(device, idx_nport, _COUNT, _HIGH, idx_vc);
            chip_device->latency_stats->latency[idx_vc].accum_latency[idx_nport].high += latency;

            latency = NVSWITCH_NPORT_PORTSTAT_RD32_LR10(device, idx_nport, _COUNT, _PANIC, idx_vc);
            chip_device->latency_stats->latency[idx_vc].accum_latency[idx_nport].panic += latency;

            latency = NVSWITCH_NPORT_PORTSTAT_RD32_LR10(device, idx_nport, _PACKET, _COUNT, idx_vc);
            chip_device->latency_stats->latency[idx_vc].accum_latency[idx_nport].count += latency;

            // Note the time of this snap
            chip_device->latency_stats->latency[idx_vc].last_read_time_nsec = time_nsec;
            chip_device->latency_stats->latency[idx_vc].count++;
        }

        // Disable SNAPONDEMAND after fetching the latencies
        NVSWITCH_LINK_WR32_LR10(device, idx_nport, NPORT, _NPORT, _PORTSTAT_SNAP_CONTROL,
            DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _STARTCOUNTER, _ENABLE) |
            DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _SNAPONDEMAND, _DISABLE));
    }
}

void
nvswitch_ecc_writeback_task_lr10
(
    nvswitch_device *device
)
{
}

void
nvswitch_set_ganged_link_table_lr10
(
    nvswitch_device *device,
    NvU32            firstIndex,
    NvU64           *ganged_link_table,
    NvU32            numEntries
)
{
    NvU32 i;

    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _ROUTE, _REG_TABLE_ADDRESS,
        DRF_NUM(_ROUTE, _REG_TABLE_ADDRESS, _INDEX, firstIndex) |
        DRF_NUM(_ROUTE, _REG_TABLE_ADDRESS, _AUTO_INCR, 1));

    for (i = 0; i < numEntries; i++)
    {
        NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _ROUTE, _REG_TABLE_DATA0,
            NvU64_LO32(ganged_link_table[i]));

        NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _ROUTE, _REG_TABLE_DATA0,
            NvU64_HI32(ganged_link_table[i]));
    }
}

static NvlStatus
_nvswitch_init_ganged_link_routing
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32        gang_index, gang_size;
    NvU64        gang_entry;
    NvU32        block_index;
    NvU32        block_count = 16;
    NvU32        glt_entries = 16;
    NvU32        glt_size = ROUTE_GANG_TABLE_SIZE / 2;
    NvU64        *ganged_link_table = NULL;
    NvU32        block_size = ROUTE_GANG_TABLE_SIZE / block_count;
    NvU32        table_index = 0;
    NvU32        i;

    //
    // Refer to switch IAS 11.2 Figure 82. Limerock Ganged RAM Table Format
    //
    // The ganged link routing table is composed of 512 entries divided into 16 sections.
    // Each section specifies how requests should be routed through the ganged links.
    // Each 32-bit entry is composed of eight 4-bit fields specifying the set of of links
    // to distribute through.  More complex spray patterns could be constructed, but for
    // now initialize it with a uniform distribution pattern.
    //
    // The ganged link routing table will be loaded with following values:
    // Typically the first section would be filled with (0,1,2,3,4,5,6,7), (8,9,10,11,12,13,14,15),...
    // Typically the second section would be filled with (0,0,0,0,0,0,0,0), (0,0,0,0,0,0,0,0),...
    // Typically the third section would be filled with (0,1,0,1,0,1,0,1), (0,1,0,1,0,1,0,1),...
    // Typically the third section would be filled with (0,1,2,0,1,2,0,1), (2,0,1,2,0,1,2,0),...
    //  :
    // The last section would typically be filled with (0,1,2,3,4,5,6,7), (8,9,10,11,12,13,14,0),...
    //
    // Refer table 20: Definition of size bits used with Ganged Link Number Table.
    // Note that section 0 corresponds with 16 ganged links.  Section N corresponds with
    // N ganged links.
    //

    //Alloc memory for Ganged Link Table
    ganged_link_table = nvswitch_os_malloc(glt_size * sizeof(gang_entry));
    if (ganged_link_table == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to allocate memory for GLT!!\n");
        return -NVL_NO_MEM;
    }

    for (block_index = 0; block_index < block_count; block_index++)
    {
        gang_size = ((block_index==0) ? 16 : block_index);

        for (gang_index = 0; gang_index < block_size/2; gang_index++)
        {
            gang_entry = 0;
            NVSWITCH_ASSERT(table_index < glt_size);

            for (i = 0; i < glt_entries; i++)
            {
                gang_entry |=
                    DRF_NUM64(_ROUTE, _REG_TABLE_DATA0, _GLX(i), (16 * gang_index + i) % gang_size);
            }

            ganged_link_table[table_index++] = gang_entry;
        }
    }

    nvswitch_set_ganged_link_table_lr10(device, 0, ganged_link_table, glt_size);

    chip_device->ganged_link_table = ganged_link_table;

    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_initialize_ip_wrappers_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 engine_enable_mask;
    NvU32 engine_disable_mask;
    NvU32 i, j;
    NvU32 idx_link;

    //
    // Now that software knows the devices and addresses, it must take all
    // the wrapper modules out of reset.  It does this by writing to the
    // PMC module enable registers.
    //

// Temporary - bug 2069764
//    NVSWITCH_REG_WR32(device, _PSMC, _ENABLE,
//        DRF_DEF(_PSMC, _ENABLE, _SAW, _ENABLE) |
//        DRF_DEF(_PSMC, _ENABLE, _PRIV_RING, _ENABLE) |
//        DRF_DEF(_PSMC, _ENABLE, _PERFMON, _ENABLE));

    NVSWITCH_SAW_WR32_LR10(device, _NVLSAW_NVSPMC, _ENABLE,
        DRF_DEF(_NVLSAW_NVSPMC, _ENABLE, _NXBAR, _ENABLE));

    //
    // At this point the list of discovered devices has been cross-referenced
    // with the ROM configuration, platform configuration, and regkey override.
    // The NVLIPT & NPORT enable filtering done here further updates the MMIO
    // information based on KVM.
    //

    // Enable the NVLIPT units that have been discovered
    engine_enable_mask = 0;
    for (i = 0; i < NVSWITCH_ENG_COUNT(device, NVLW, ); i++)
    {
        if (NVSWITCH_ENG_IS_VALID(device, NVLW, i))
        {
            engine_enable_mask |= NVBIT(i);
        }
    }
    NVSWITCH_SAW_WR32_LR10(device, _NVLSAW_NVSPMC, _ENABLE_NVLIPT, engine_enable_mask);

    //
    // In bare metal we write ENABLE_NVLIPT to enable the units that aren't
    // disabled by ROM configuration, platform configuration, or regkey override.
    // If we are running inside a VM, the hypervisor has already set ENABLE_NVLIPT
    // and write protected it.  Reading ENABLE_NVLIPT tells us which units we
    // are allowed to use inside this VM.
    //
    engine_disable_mask = ~NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _ENABLE_NVLIPT);
    if (engine_enable_mask != ~engine_disable_mask)
    {
        NVSWITCH_PRINT(device, WARN,
            "NV_NVLSAW_NVSPMC_ENABLE_NVLIPT mismatch: wrote 0x%x, read 0x%x\n",
            engine_enable_mask,
            ~engine_disable_mask);
        NVSWITCH_PRINT(device, WARN,
            "Ignoring NV_NVLSAW_NVSPMC_ENABLE_NVLIPT readback until supported on fmodel\n");
        engine_disable_mask = ~engine_enable_mask;
    }
    engine_disable_mask &= NVBIT(NVSWITCH_ENG_COUNT(device, NVLW, )) - 1;
    FOR_EACH_INDEX_IN_MASK(32, i, engine_disable_mask)
    {
        chip_device->engNVLW[i].valid = NV_FALSE;
        for (j = 0; j < NVSWITCH_LINKS_PER_NVLW; j++)
        {
            idx_link = i * NVSWITCH_LINKS_PER_NVLW + j;
            if (idx_link < NVSWITCH_LINK_COUNT(device))
            {
                device->link[idx_link].valid = NV_FALSE;
                //
                // TODO: This invalidate used to also invalidate all the
                // associated NVLW engFOO units. This is probably not necessary
                // but code that bypasses the link valid check might touch the
                // underlying units when they are not supposed to.
                //
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Enable the NPORT units that have been discovered
    engine_enable_mask = 0;
    for (i = 0; i < NVSWITCH_ENG_COUNT(device, NPG, ); i++)
    {
        if (NVSWITCH_ENG_IS_VALID(device, NPG, i))
        {
            engine_enable_mask |= NVBIT(i);
        }
    }
    NVSWITCH_SAW_WR32_LR10(device, _NVLSAW_NVSPMC, _ENABLE_NPG, engine_enable_mask);

    //
    // In bare metal we write ENABLE_NPG to enable the units that aren't
    // disabled by ROM configuration, platform configuration, or regkey override.
    // If we are running inside a VM, the hypervisor has already set ENABLE_NPG
    // and write protected it.  Reading ENABLE_NPG tells us which units we
    // are allowed to use inside this VM.
    //
    engine_disable_mask = ~NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _ENABLE_NPG);
    if (engine_enable_mask != ~engine_disable_mask)
    {
        NVSWITCH_PRINT(device, WARN,
            "NV_NVLSAW_NVSPMC_ENABLE_NPG mismatch: wrote 0x%x, read 0x%x\n",
            engine_enable_mask,
            ~engine_disable_mask);
        NVSWITCH_PRINT(device, WARN,
            "Ignoring NV_NVLSAW_NVSPMC_ENABLE_NPG readback until supported on fmodel\n");
        engine_disable_mask = ~engine_enable_mask;
    }
    engine_disable_mask &= NVBIT(NVSWITCH_ENG_COUNT(device, NPG, )) - 1;
    FOR_EACH_INDEX_IN_MASK(32, i, engine_disable_mask)
    {
        chip_device->engNPG[i].valid = NV_FALSE;
        for (j = 0; j < NVSWITCH_LINKS_PER_NPG; j++)
        {
            idx_link = i * NVSWITCH_LINKS_PER_NPG + j;

            if (idx_link < NVSWITCH_LINK_COUNT(device))
            {
                device->link[idx_link].valid = NV_FALSE;
                //
                // TODO: This invalidate used to also invalidate all the
                // associated NPG engFOO units. This is probably not necessary
                // but code that bypasses the link valid check might touch the
                // underlying units when they are not supposed to.
                //
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

//
// Bring units out of warm reset on boot.  Used by driver load.
//
void
nvswitch_init_warm_reset_lr10
(
    nvswitch_device *device
)
{
    NvU32 idx_npg;
    NvU32 idx_nport;
    NvU32 nport_mask;
    NvU32 nport_disable = 0;

#if defined(NV_NPG_WARMRESET_NPORTDISABLE)
    nport_disable = DRF_NUM(_NPG, _WARMRESET, _NPORTDISABLE, ~nport_mask);
#endif

    //
    // Walk the NPGs and build the mask of extant NPORTs
    //
    for (idx_npg = 0; idx_npg < NVSWITCH_ENG_COUNT(device, NPG, ); idx_npg++)
    {
        if (NVSWITCH_ENG_IS_VALID(device, NPG, idx_npg))
        {
            nport_mask = 0;
            for (idx_nport = 0; idx_nport < NVSWITCH_NPORT_PER_NPG; idx_nport++)
            {
                nport_mask |=
                    (NVSWITCH_ENG_IS_VALID(device, NPORT, idx_npg*NVSWITCH_NPORT_PER_NPG + idx_nport) ?
                    NVBIT(idx_nport) : 0x0);
            }

            NVSWITCH_NPG_WR32_LR10(device, idx_npg,
                _NPG, _WARMRESET,
                nport_disable |
                DRF_NUM(_NPG, _WARMRESET, _NPORTWARMRESET, nport_mask));
        }
    }
}

/*
 * CTRL_NVSWITCH_SET_REMAP_POLICY
 */

NvlStatus
nvswitch_get_remap_table_selector_lr10
(
    nvswitch_device *device,
    NVSWITCH_TABLE_SELECT_REMAP table_selector,
    NvU32 *remap_ram_sel
)
{
    NvU32 ram_sel = 0;

    switch (table_selector)
    {
        case NVSWITCH_TABLE_SELECT_REMAP_PRIMARY:
            ram_sel = NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSREMAPPOLICYRAM;
            break;
        default:
            // Unsupported remap table selector
            return -NVL_ERR_NOT_SUPPORTED;
            break;
    }

    if (remap_ram_sel)
    {
        *remap_ram_sel = ram_sel;
    }

    return NVL_SUCCESS;
}

NvU32
nvswitch_get_ingress_ram_size_lr10
(
    nvswitch_device *device,
    NvU32 ingress_ram_selector      // NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECT*
)
{
    NvU32 ram_size = 0;

    switch (ingress_ram_selector)
    {
        case NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSREMAPPOLICYRAM:
            ram_size = NV_INGRESS_REQRSPMAPADDR_RAM_ADDRESS_REMAPTAB_DEPTH + 1;
            break;
        case NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRIDROUTERAM:
            ram_size = NV_INGRESS_REQRSPMAPADDR_RAM_ADDRESS_RID_TAB_DEPTH + 1;
            break;
        case NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRLANROUTERAM:
            ram_size = NV_INGRESS_REQRSPMAPADDR_RAM_ADDRESS_RLAN_TAB_DEPTH + 1;
            break;
        default:
            // Unsupported ingress RAM selector
            break;
    }

    return ram_size;
}

static void
_nvswitch_set_remap_policy_lr10
(
    nvswitch_device *device,
    NvU32 portNum,
    NvU32 firstIndex,
    NvU32 numEntries,
    NVSWITCH_REMAP_POLICY_ENTRY *remap_policy
)
{
    NvU32 i;
    NvU32 remap_address;
    NvU32 address_offset;
    NvU32 address_base;
    NvU32 address_limit;

    NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _REQRSPMAPADDR,
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, firstIndex) |
        DRF_DEF(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, _SELECTSREMAPPOLICYRAM) |
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, 1));

    for (i = 0; i < numEntries; i++)
    {
        // Set each field if enabled, else set it to 0.
        remap_address = DRF_VAL64(_INGRESS, _REMAP, _ADDR_PHYS_LR10, remap_policy[i].address);
        address_offset = DRF_VAL64(_INGRESS, _REMAP, _ADR_OFFSET_PHYS_LR10, remap_policy[i].addressOffset);
        address_base = DRF_VAL64(_INGRESS, _REMAP, _ADR_BASE_PHYS_LR10, remap_policy[i].addressBase);
        address_limit = DRF_VAL64(_INGRESS, _REMAP, _ADR_LIMIT_PHYS_LR10, remap_policy[i].addressLimit);

        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _REMAPTABDATA1,
            DRF_NUM(_INGRESS, _REMAPTABDATA1, _REQCTXT_MSK, remap_policy[i].reqCtxMask) |
            DRF_NUM(_INGRESS, _REMAPTABDATA1, _REQCTXT_CHK, remap_policy[i].reqCtxChk));
        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _REMAPTABDATA2,
            DRF_NUM(_INGRESS, _REMAPTABDATA2, _REQCTXT_REP, remap_policy[i].reqCtxRep) |
            DRF_NUM(_INGRESS, _REMAPTABDATA2, _ADR_OFFSET, address_offset));
        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _REMAPTABDATA3,
            DRF_NUM(_INGRESS, _REMAPTABDATA3, _ADR_BASE, address_base) |
            DRF_NUM(_INGRESS, _REMAPTABDATA3, _ADR_LIMIT, address_limit));
        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _REMAPTABDATA4,
            DRF_NUM(_INGRESS, _REMAPTABDATA4, _TGTID, remap_policy[i].targetId) |
            DRF_NUM(_INGRESS, _REMAPTABDATA4, _RFUNC, remap_policy[i].flags));

        // Write last and auto-increment
        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _REMAPTABDATA0,
            DRF_NUM(_INGRESS, _REMAPTABDATA0, _RMAP_ADDR, remap_address) |
            DRF_NUM(_INGRESS, _REMAPTABDATA0, _IRL_SEL, remap_policy[i].irlSelect) |
            DRF_NUM(_INGRESS, _REMAPTABDATA0, _ACLVALID, remap_policy[i].entryValid));
    }
}

NvlStatus
nvswitch_ctrl_set_remap_policy_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_REMAP_POLICY *p
)
{
    NvU32 i;
    NvU32 rfunc;
    NvU32 ram_size;
    NvlStatus retval = NVL_SUCCESS;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, p->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "NPORT port #%d not valid\n",
            p->portNum);
        return -NVL_BAD_ARGS;
    }

    if (p->tableSelect != NVSWITCH_TABLE_SELECT_REMAP_PRIMARY)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Remap table #%d not supported\n",
            p->tableSelect);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    ram_size = nvswitch_get_ingress_ram_size(device, NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSREMAPPOLICYRAM);
    if ((p->firstIndex >= ram_size) ||
        (p->numEntries > NVSWITCH_REMAP_POLICY_ENTRIES_MAX) ||
        (p->firstIndex + p->numEntries > ram_size))
    {
        NVSWITCH_PRINT(device, ERROR,
            "remapPolicy[%d..%d] overflows range %d..%d or size %d.\n",
            p->firstIndex, p->firstIndex + p->numEntries - 1,
            0, ram_size - 1,
            NVSWITCH_REMAP_POLICY_ENTRIES_MAX);
        return -NVL_BAD_ARGS;
    }

    for (i = 0; i < p->numEntries; i++)
    {
        if (p->remapPolicy[i].targetId &
            ~DRF_MASK(NV_INGRESS_REMAPTABDATA4_TGTID))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].targetId 0x%x out of valid range (0x%x..0x%x)\n",
                i, p->remapPolicy[i].targetId,
                0, DRF_MASK(NV_INGRESS_REMAPTABDATA4_TGTID));
            return -NVL_BAD_ARGS;
        }

        if (p->remapPolicy[i].irlSelect &
            ~DRF_MASK(NV_INGRESS_REMAPTABDATA0_IRL_SEL))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].irlSelect 0x%x out of valid range (0x%x..0x%x)\n",
                i, p->remapPolicy[i].irlSelect,
                0, DRF_MASK(NV_INGRESS_REMAPTABDATA0_IRL_SEL));
            return -NVL_BAD_ARGS;
        }

        rfunc = p->remapPolicy[i].flags &
            (
                NVSWITCH_REMAP_POLICY_FLAGS_REMAP_ADDR |
                NVSWITCH_REMAP_POLICY_FLAGS_REQCTXT_CHECK |
                NVSWITCH_REMAP_POLICY_FLAGS_REQCTXT_REPLACE |
                NVSWITCH_REMAP_POLICY_FLAGS_ADR_BASE |
                NVSWITCH_REMAP_POLICY_FLAGS_ADR_OFFSET
            );
        if (rfunc != p->remapPolicy[i].flags)
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].flags 0x%x has undefined flags (0x%x)\n",
                i, p->remapPolicy[i].flags,
                p->remapPolicy[i].flags ^ rfunc);
            return -NVL_BAD_ARGS;
        }

        // Validate that only bits 46:36 are used
        if (p->remapPolicy[i].address &
            ~DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADDR_PHYS_LR10))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].address 0x%llx & ~0x%llx != 0\n",
                i, p->remapPolicy[i].address,
                DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADDR_PHYS_LR10));
            return -NVL_BAD_ARGS;
        }

        if (p->remapPolicy[i].reqCtxMask &
           ~DRF_MASK(NV_INGRESS_REMAPTABDATA1_REQCTXT_MSK))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].reqCtxMask 0x%x out of valid range (0x%x..0x%x)\n",
                i, p->remapPolicy[i].reqCtxMask,
                0, DRF_MASK(NV_INGRESS_REMAPTABDATA1_REQCTXT_MSK));
            return -NVL_BAD_ARGS;
        }

        if (p->remapPolicy[i].reqCtxChk &
            ~DRF_MASK(NV_INGRESS_REMAPTABDATA1_REQCTXT_CHK))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].reqCtxChk 0x%x out of valid range (0x%x..0x%x)\n",
                i, p->remapPolicy[i].reqCtxChk,
                0, DRF_MASK(NV_INGRESS_REMAPTABDATA1_REQCTXT_CHK));
            return -NVL_BAD_ARGS;
        }

        if (p->remapPolicy[i].reqCtxRep &
            ~DRF_MASK(NV_INGRESS_REMAPTABDATA2_REQCTXT_REP))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].reqCtxRep 0x%x out of valid range (0x%x..0x%x)\n",
                i, p->remapPolicy[i].reqCtxRep,
                0, DRF_MASK(NV_INGRESS_REMAPTABDATA2_REQCTXT_REP));
            return -NVL_BAD_ARGS;
        }

        if ((p->remapPolicy[i].flags & NVSWITCH_REMAP_POLICY_FLAGS_ADR_OFFSET) &&
            !(p->remapPolicy[i].flags & NVSWITCH_REMAP_POLICY_FLAGS_ADR_BASE))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].flags: _FLAGS_ADR_OFFSET should not be set if "
                "_FLAGS_ADR_BASE is not set\n",
                i);
            return -NVL_BAD_ARGS;
        }

        // Validate that only bits 35:20 are used
        if (p->remapPolicy[i].addressBase &
            ~DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_BASE_PHYS_LR10))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].addressBase 0x%llx & ~0x%llx != 0\n",
                i, p->remapPolicy[i].addressBase,
                DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_BASE_PHYS_LR10));
            return -NVL_BAD_ARGS;
        }

        // Validate that only bits 35:20 are used
        if (p->remapPolicy[i].addressLimit &
            ~DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_LIMIT_PHYS_LR10))
        {
            NVSWITCH_PRINT(device, ERROR,
                 "remapPolicy[%d].addressLimit 0x%llx & ~0x%llx != 0\n",
                 i, p->remapPolicy[i].addressLimit,
                 DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_LIMIT_PHYS_LR10));
            return -NVL_BAD_ARGS;
        }

        // Validate base & limit describe a region
        if (p->remapPolicy[i].addressBase > p->remapPolicy[i].addressLimit)
        {
            NVSWITCH_PRINT(device, ERROR,
                 "remapPolicy[%d].addressBase/Limit invalid: 0x%llx > 0x%llx\n",
                 i, p->remapPolicy[i].addressBase, p->remapPolicy[i].addressLimit);
            return -NVL_BAD_ARGS;
        }

        // Validate that only bits 35:20 are used
        if (p->remapPolicy[i].addressOffset &
            ~DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_OFFSET_PHYS_LR10))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].addressOffset 0x%llx & ~0x%llx != 0\n",
                i, p->remapPolicy[i].addressOffset,
                DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_OFFSET_PHYS_LR10));
            return -NVL_BAD_ARGS;
        }

        // Validate limit - base + offset doesn't overflow 64G
        if ((p->remapPolicy[i].addressLimit - p->remapPolicy[i].addressBase +
                p->remapPolicy[i].addressOffset) &
            ~DRF_SHIFTMASK64(NV_INGRESS_REMAP_ADR_OFFSET_PHYS_LR10))
        {
            NVSWITCH_PRINT(device, ERROR,
                "remapPolicy[%d].addressLimit 0x%llx - addressBase 0x%llx + "
                "addressOffset 0x%llx overflows 64GB\n",
                i, p->remapPolicy[i].addressLimit, p->remapPolicy[i].addressBase,
                p->remapPolicy[i].addressOffset);
            return -NVL_BAD_ARGS;
        }
    }

    _nvswitch_set_remap_policy_lr10(device, p->portNum, p->firstIndex, p->numEntries, p->remapPolicy);

    return retval;
}

/*
 * CTRL_NVSWITCH_GET_REMAP_POLICY
 */

#define NVSWITCH_NUM_REMAP_POLICY_REGS_LR10 5

NvlStatus
nvswitch_ctrl_get_remap_policy_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_REMAP_POLICY_PARAMS *params
)
{
    NVSWITCH_REMAP_POLICY_ENTRY *remap_policy;
    NvU32 remap_policy_data[NVSWITCH_NUM_REMAP_POLICY_REGS_LR10]; // 5 REMAP tables
    NvU32 table_index;
    NvU32 remap_count;
    NvU32 remap_address;
    NvU32 address_offset;
    NvU32 address_base;
    NvU32 address_limit;
    NvU32 ram_size;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, params->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "NPORT port #%d not valid\n",
            params->portNum);
        return -NVL_BAD_ARGS;
    }

    if (params->tableSelect != NVSWITCH_TABLE_SELECT_REMAP_PRIMARY)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Remap table #%d not supported\n",
            params->tableSelect);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    ram_size = nvswitch_get_ingress_ram_size(device, NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSREMAPPOLICYRAM);
    if ((params->firstIndex >= ram_size))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: remapPolicy first index %d out of range[%d..%d].\n",
            __FUNCTION__, params->firstIndex, 0, ram_size - 1);
        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(params->entry, 0, (NVSWITCH_REMAP_POLICY_ENTRIES_MAX *
        sizeof(NVSWITCH_REMAP_POLICY_ENTRY)));

    table_index = params->firstIndex;
    remap_policy = params->entry;
    remap_count = 0;

    /* set table offset */
    NVSWITCH_LINK_WR32_LR10(device, params->portNum, NPORT, _INGRESS, _REQRSPMAPADDR,
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, params->firstIndex) |
        DRF_DEF(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, _SELECTSREMAPPOLICYRAM) |
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, 1));

    while (remap_count < NVSWITCH_REMAP_POLICY_ENTRIES_MAX &&
        table_index < ram_size)
    {
        remap_policy_data[0] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _REMAPTABDATA0);
        remap_policy_data[1] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _REMAPTABDATA1);
        remap_policy_data[2] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _REMAPTABDATA2);
        remap_policy_data[3] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _REMAPTABDATA3);
        remap_policy_data[4] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _REMAPTABDATA4);

        /* add to remap_entries list if nonzero */
        if (remap_policy_data[0] || remap_policy_data[1] || remap_policy_data[2] ||
            remap_policy_data[3] || remap_policy_data[4])
        {
            remap_policy[remap_count].irlSelect =
                DRF_VAL(_INGRESS, _REMAPTABDATA0, _IRL_SEL, remap_policy_data[0]);

            remap_policy[remap_count].entryValid =
                DRF_VAL(_INGRESS, _REMAPTABDATA0, _ACLVALID, remap_policy_data[0]);

            remap_address =
                DRF_VAL(_INGRESS, _REMAPTABDATA0, _RMAP_ADDR, remap_policy_data[0]);

            remap_policy[remap_count].address =
                DRF_NUM64(_INGRESS, _REMAP, _ADDR_PHYS_LR10, remap_address);

            remap_policy[remap_count].reqCtxMask =
                DRF_VAL(_INGRESS, _REMAPTABDATA1, _REQCTXT_MSK, remap_policy_data[1]);

            remap_policy[remap_count].reqCtxChk =
                DRF_VAL(_INGRESS, _REMAPTABDATA1, _REQCTXT_CHK, remap_policy_data[1]);

            remap_policy[remap_count].reqCtxRep =
                DRF_VAL(_INGRESS, _REMAPTABDATA2, _REQCTXT_REP, remap_policy_data[2]);

            address_offset =
                DRF_VAL(_INGRESS, _REMAPTABDATA2, _ADR_OFFSET, remap_policy_data[2]);

            remap_policy[remap_count].addressOffset =
                DRF_NUM64(_INGRESS, _REMAP, _ADR_OFFSET_PHYS_LR10, address_offset);

            address_base =
                DRF_VAL(_INGRESS, _REMAPTABDATA3, _ADR_BASE, remap_policy_data[3]);

            remap_policy[remap_count].addressBase =
                DRF_NUM64(_INGRESS, _REMAP, _ADR_BASE_PHYS_LR10, address_base);

            address_limit =
                DRF_VAL(_INGRESS, _REMAPTABDATA3, _ADR_LIMIT, remap_policy_data[3]);

            remap_policy[remap_count].addressLimit =
                DRF_NUM64(_INGRESS, _REMAP, _ADR_LIMIT_PHYS_LR10, address_limit);

            remap_policy[remap_count].targetId =
                DRF_VAL(_INGRESS, _REMAPTABDATA4, _TGTID, remap_policy_data[4]);

            remap_policy[remap_count].flags =
                DRF_VAL(_INGRESS, _REMAPTABDATA4, _RFUNC, remap_policy_data[4]);

            remap_count++;
        }

        table_index++;
    }

    params->nextIndex = table_index;
    params->numEntries = remap_count;

    return NVL_SUCCESS;
}

/*
 * CTRL_NVSWITCH_SET_REMAP_POLICY_VALID
 */
NvlStatus
nvswitch_ctrl_set_remap_policy_valid_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_REMAP_POLICY_VALID *p
)
{
    NvU32 remap_ram;
    NvU32 ram_address = p->firstIndex;
    NvU32 remap_policy_data[NVSWITCH_NUM_REMAP_POLICY_REGS_LR10]; // 5 REMAP tables
    NvU32 i;
    NvU32 ram_size;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, p->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NPORT port #%d not valid\n",
            __FUNCTION__, p->portNum);
        return -NVL_BAD_ARGS;
    }

    if (p->tableSelect != NVSWITCH_TABLE_SELECT_REMAP_PRIMARY)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Remap table #%d not supported\n",
            p->tableSelect);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    ram_size = nvswitch_get_ingress_ram_size(device, NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSREMAPPOLICYRAM);
    if ((p->firstIndex >= ram_size) ||
        (p->numEntries > NVSWITCH_REMAP_POLICY_ENTRIES_MAX) ||
        (p->firstIndex + p->numEntries > ram_size))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: remapPolicy[%d..%d] overflows range %d..%d or size %d.\n",
            __FUNCTION__, p->firstIndex, p->firstIndex + p->numEntries - 1,
            0, ram_size - 1,
            NVSWITCH_REMAP_POLICY_ENTRIES_MAX);
        return -NVL_BAD_ARGS;
    }

    // Select REMAPPOLICY RAM and disable Auto Increament.
    remap_ram =
        DRF_DEF(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, _SELECTSREMAPPOLICYRAM) |
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, 0);

    for (i = 0; i < p->numEntries; i++)
    {
        /* set the ram address */
        remap_ram = FLD_SET_DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, ram_address++, remap_ram);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _REQRSPMAPADDR, remap_ram);

        remap_policy_data[0] = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA0);
        remap_policy_data[1] = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA1);
        remap_policy_data[2] = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA2);
        remap_policy_data[3] = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA3);
        remap_policy_data[4] = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA4);

        // Set valid bit in REMAPTABDATA0.
        remap_policy_data[0] = FLD_SET_DRF_NUM(_INGRESS, _REMAPTABDATA0, _ACLVALID, p->entryValid[i], remap_policy_data[0]);

        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA4, remap_policy_data[4]);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA3, remap_policy_data[3]);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA2, remap_policy_data[2]);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA1, remap_policy_data[1]);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _REMAPTABDATA0, remap_policy_data[0]);
    }

    return NVL_SUCCESS;
}

//
// Programming invalid entries to 0x3F causes Route block to detect an invalid port number
// and flag a PRIV error to the FM. (See Table 14.RID RAM Programming, IAS 3.3.4)
//

#define NVSWITCH_INVALID_PORT_VAL_LR10   0x3F
#define NVSWITCH_INVALID_VC_VAL_LR10     0x0

#define NVSWITCH_PORTLIST_PORT_LR10(_entry, _idx) \
    ((_idx < _entry.numEntries) ? _entry.portList[_idx].destPortNum : NVSWITCH_INVALID_PORT_VAL_LR10)

#define NVSWITCH_PORTLIST_VC_LR10(_entry, _idx) \
    ((_idx < _entry.numEntries) ? _entry.portList[_idx].vcMap : NVSWITCH_INVALID_VC_VAL_LR10)

/*
 * CTRL_NVSWITCH_SET_ROUTING_ID
 */

static void
_nvswitch_set_routing_id_lr10
(
    nvswitch_device *device,
    NvU32 portNum,
    NvU32 firstIndex,
    NvU32 numEntries,
    NVSWITCH_ROUTING_ID_ENTRY *routing_id
)
{
    NvU32 i;
    NvU32 rmod;

    NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _REQRSPMAPADDR,
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, firstIndex) |
        DRF_DEF(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, _SELECTSRIDROUTERAM) |
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, 1));

    for (i = 0; i < numEntries; i++)
    {
        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RIDTABDATA1,
            DRF_NUM(_INGRESS, _RIDTABDATA1, _PORT3,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 3)) |
            DRF_NUM(_INGRESS, _RIDTABDATA1, _VC_MODE3, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 3))   |
            DRF_NUM(_INGRESS, _RIDTABDATA1, _PORT4,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 4)) |
            DRF_NUM(_INGRESS, _RIDTABDATA1, _VC_MODE4, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 4))   |
            DRF_NUM(_INGRESS, _RIDTABDATA1, _PORT5,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 5)) |
            DRF_NUM(_INGRESS, _RIDTABDATA1, _VC_MODE5, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 5)));

        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RIDTABDATA2,
            DRF_NUM(_INGRESS, _RIDTABDATA2, _PORT6,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 6)) |
            DRF_NUM(_INGRESS, _RIDTABDATA2, _VC_MODE6, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 6))   |
            DRF_NUM(_INGRESS, _RIDTABDATA2, _PORT7,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 7)) |
            DRF_NUM(_INGRESS, _RIDTABDATA2, _VC_MODE7, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 7))   |
            DRF_NUM(_INGRESS, _RIDTABDATA2, _PORT8,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 8)) |
            DRF_NUM(_INGRESS, _RIDTABDATA2, _VC_MODE8, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 8)));

        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RIDTABDATA3,
            DRF_NUM(_INGRESS, _RIDTABDATA3, _PORT9,     NVSWITCH_PORTLIST_PORT_LR10(routing_id[i],  9)) |
            DRF_NUM(_INGRESS, _RIDTABDATA3, _VC_MODE9,  NVSWITCH_PORTLIST_VC_LR10(routing_id[i],  9))   |
            DRF_NUM(_INGRESS, _RIDTABDATA3, _PORT10,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 10)) |
            DRF_NUM(_INGRESS, _RIDTABDATA3, _VC_MODE10, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 10))   |
            DRF_NUM(_INGRESS, _RIDTABDATA3, _PORT11,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 11)) |
            DRF_NUM(_INGRESS, _RIDTABDATA3, _VC_MODE11, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 11)));

        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RIDTABDATA4,
            DRF_NUM(_INGRESS, _RIDTABDATA4, _PORT12,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 12)) |
            DRF_NUM(_INGRESS, _RIDTABDATA4, _VC_MODE12, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 12))   |
            DRF_NUM(_INGRESS, _RIDTABDATA4, _PORT13,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 13)) |
            DRF_NUM(_INGRESS, _RIDTABDATA4, _VC_MODE13, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 13))   |
            DRF_NUM(_INGRESS, _RIDTABDATA4, _PORT14,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 14)) |
            DRF_NUM(_INGRESS, _RIDTABDATA4, _VC_MODE14, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 14)));

        rmod =
            (routing_id[i].useRoutingLan ? NVBIT(6) : 0) |
            (routing_id[i].enableIrlErrResponse ? NVBIT(9) : 0);

        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RIDTABDATA5,
            DRF_NUM(_INGRESS, _RIDTABDATA5, _PORT15,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 15)) |
            DRF_NUM(_INGRESS, _RIDTABDATA5, _VC_MODE15, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 15))   |
            DRF_NUM(_INGRESS, _RIDTABDATA5, _RMOD,      rmod)                                           |
            DRF_NUM(_INGRESS, _RIDTABDATA5, _ACLVALID,  routing_id[i].entryValid));

        NVSWITCH_ASSERT(routing_id[i].numEntries <= 16);
        // Write last and auto-increment
        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RIDTABDATA0,
            DRF_NUM(_INGRESS, _RIDTABDATA0, _GSIZE,
                (routing_id[i].numEntries == 16) ? 0x0 : routing_id[i].numEntries) |
            DRF_NUM(_INGRESS, _RIDTABDATA0, _PORT0,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 0)) |
            DRF_NUM(_INGRESS, _RIDTABDATA0, _VC_MODE0, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 0))   |
            DRF_NUM(_INGRESS, _RIDTABDATA0, _PORT1,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 1)) |
            DRF_NUM(_INGRESS, _RIDTABDATA0, _VC_MODE1, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 1))   |
            DRF_NUM(_INGRESS, _RIDTABDATA0, _PORT2,    NVSWITCH_PORTLIST_PORT_LR10(routing_id[i], 2)) |
            DRF_NUM(_INGRESS, _RIDTABDATA0, _VC_MODE2, NVSWITCH_PORTLIST_VC_LR10(routing_id[i], 2)));
    }
}

#define NVSWITCH_NUM_RIDTABDATA_REGS_LR10 6

NvlStatus
nvswitch_ctrl_get_routing_id_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_ROUTING_ID_PARAMS *params
)
{
    NVSWITCH_ROUTING_ID_IDX_ENTRY *rid_entries;
    NvU32 table_index;
    NvU32 rid_tab_data[NVSWITCH_NUM_RIDTABDATA_REGS_LR10]; // 6 RID tables
    NvU32 rid_count;
    NvU32 rmod;
    NvU32 gsize;
    NvU32 ram_size;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, params->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NPORT port #%d not valid\n",
            __FUNCTION__, params->portNum);
        return -NVL_BAD_ARGS;
    }

    ram_size = nvswitch_get_ingress_ram_size(device, NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRIDROUTERAM);
    if (params->firstIndex >= ram_size)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: routingId first index %d out of range[%d..%d].\n",
            __FUNCTION__, params->firstIndex, 0, ram_size - 1);
        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(params->entries, 0, sizeof(params->entries));

    table_index = params->firstIndex;
    rid_entries = params->entries;
    rid_count = 0;

    /* set table offset */
    NVSWITCH_LINK_WR32_LR10(device, params->portNum, NPORT, _INGRESS, _REQRSPMAPADDR,
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, params->firstIndex) |
        DRF_DEF(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, _SELECTSRIDROUTERAM) |
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, 1));

    while (rid_count < NVSWITCH_ROUTING_ID_ENTRIES_MAX &&
           table_index < ram_size)
    {
        rid_tab_data[0] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RIDTABDATA0);
        rid_tab_data[1] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RIDTABDATA1);
        rid_tab_data[2] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RIDTABDATA2);
        rid_tab_data[3] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RIDTABDATA3);
        rid_tab_data[4] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RIDTABDATA4);
        rid_tab_data[5] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RIDTABDATA5);

        /* add to rid_entries list if nonzero */
        if (rid_tab_data[0] || rid_tab_data[1] || rid_tab_data[2] ||
            rid_tab_data[3] || rid_tab_data[4] || rid_tab_data[5])
        {
            rid_entries[rid_count].entry.portList[0].destPortNum  = DRF_VAL(_INGRESS, _RIDTABDATA0, _PORT0, rid_tab_data[0]);
            rid_entries[rid_count].entry.portList[0].vcMap        = DRF_VAL(_INGRESS, _RIDTABDATA0, _VC_MODE0, rid_tab_data[0]);

            rid_entries[rid_count].entry.portList[1].destPortNum  = DRF_VAL(_INGRESS, _RIDTABDATA0, _PORT1, rid_tab_data[0]);
            rid_entries[rid_count].entry.portList[1].vcMap        = DRF_VAL(_INGRESS, _RIDTABDATA0, _VC_MODE1, rid_tab_data[0]);

            rid_entries[rid_count].entry.portList[2].destPortNum  = DRF_VAL(_INGRESS, _RIDTABDATA0, _PORT2, rid_tab_data[0]);
            rid_entries[rid_count].entry.portList[2].vcMap        = DRF_VAL(_INGRESS, _RIDTABDATA0, _VC_MODE2, rid_tab_data[0]);

            rid_entries[rid_count].entry.portList[3].destPortNum  = DRF_VAL(_INGRESS, _RIDTABDATA1, _PORT3, rid_tab_data[1]);
            rid_entries[rid_count].entry.portList[3].vcMap        = DRF_VAL(_INGRESS, _RIDTABDATA1, _VC_MODE3, rid_tab_data[1]);

            rid_entries[rid_count].entry.portList[4].destPortNum  = DRF_VAL(_INGRESS, _RIDTABDATA1, _PORT4, rid_tab_data[1]);
            rid_entries[rid_count].entry.portList[4].vcMap        = DRF_VAL(_INGRESS, _RIDTABDATA1, _VC_MODE4, rid_tab_data[1]);

            rid_entries[rid_count].entry.portList[5].destPortNum  = DRF_VAL(_INGRESS, _RIDTABDATA1, _PORT5, rid_tab_data[1]);
            rid_entries[rid_count].entry.portList[5].vcMap        = DRF_VAL(_INGRESS, _RIDTABDATA1, _VC_MODE5, rid_tab_data[1]);

            rid_entries[rid_count].entry.portList[6].destPortNum  = DRF_VAL(_INGRESS, _RIDTABDATA2, _PORT6, rid_tab_data[2]);
            rid_entries[rid_count].entry.portList[6].vcMap        = DRF_VAL(_INGRESS, _RIDTABDATA2, _VC_MODE6, rid_tab_data[2]);

            rid_entries[rid_count].entry.portList[7].destPortNum  = DRF_VAL(_INGRESS, _RIDTABDATA2, _PORT7, rid_tab_data[2]);
            rid_entries[rid_count].entry.portList[7].vcMap        = DRF_VAL(_INGRESS, _RIDTABDATA2, _VC_MODE7, rid_tab_data[2]);

            rid_entries[rid_count].entry.portList[8].destPortNum  = DRF_VAL(_INGRESS, _RIDTABDATA2, _PORT8, rid_tab_data[2]);
            rid_entries[rid_count].entry.portList[8].vcMap        = DRF_VAL(_INGRESS, _RIDTABDATA2, _VC_MODE8, rid_tab_data[2]);

            rid_entries[rid_count].entry.portList[9].destPortNum  = DRF_VAL(_INGRESS, _RIDTABDATA3, _PORT9, rid_tab_data[3]);
            rid_entries[rid_count].entry.portList[9].vcMap        = DRF_VAL(_INGRESS, _RIDTABDATA3, _VC_MODE9, rid_tab_data[3]);

            rid_entries[rid_count].entry.portList[10].destPortNum = DRF_VAL(_INGRESS, _RIDTABDATA3, _PORT10, rid_tab_data[3]);
            rid_entries[rid_count].entry.portList[10].vcMap       = DRF_VAL(_INGRESS, _RIDTABDATA3, _VC_MODE10, rid_tab_data[3]);

            rid_entries[rid_count].entry.portList[11].destPortNum = DRF_VAL(_INGRESS, _RIDTABDATA3, _PORT11, rid_tab_data[3]);
            rid_entries[rid_count].entry.portList[11].vcMap       = DRF_VAL(_INGRESS, _RIDTABDATA3, _VC_MODE11, rid_tab_data[3]);

            rid_entries[rid_count].entry.portList[12].destPortNum = DRF_VAL(_INGRESS, _RIDTABDATA4, _PORT12, rid_tab_data[4]);
            rid_entries[rid_count].entry.portList[12].vcMap       = DRF_VAL(_INGRESS, _RIDTABDATA4, _VC_MODE12, rid_tab_data[4]);

            rid_entries[rid_count].entry.portList[13].destPortNum = DRF_VAL(_INGRESS, _RIDTABDATA4, _PORT13, rid_tab_data[4]);
            rid_entries[rid_count].entry.portList[13].vcMap       = DRF_VAL(_INGRESS, _RIDTABDATA4, _VC_MODE13, rid_tab_data[4]);

            rid_entries[rid_count].entry.portList[14].destPortNum = DRF_VAL(_INGRESS, _RIDTABDATA4, _PORT14, rid_tab_data[4]);
            rid_entries[rid_count].entry.portList[14].vcMap       = DRF_VAL(_INGRESS, _RIDTABDATA4, _VC_MODE14, rid_tab_data[4]);

            rid_entries[rid_count].entry.portList[15].destPortNum = DRF_VAL(_INGRESS, _RIDTABDATA5, _PORT15, rid_tab_data[5]);
            rid_entries[rid_count].entry.portList[15].vcMap       = DRF_VAL(_INGRESS, _RIDTABDATA5, _VC_MODE15, rid_tab_data[5]);
            rid_entries[rid_count].entry.entryValid               = DRF_VAL(_INGRESS, _RIDTABDATA5, _ACLVALID, rid_tab_data[5]);

            rmod = DRF_VAL(_INGRESS, _RIDTABDATA5, _RMOD, rid_tab_data[5]);
            rid_entries[rid_count].entry.useRoutingLan = (NVBIT(6) & rmod) ? 1 : 0;
            rid_entries[rid_count].entry.enableIrlErrResponse = (NVBIT(9) & rmod) ? 1 : 0;

            // Gsize of 16 falls into the 0th entry of GLT region. The _GSIZE field must be mapped accordingly
            // to the number of port entries (See IAS, Table 20, Sect 3.4.2.2. Packet Routing).
            gsize = DRF_VAL(_INGRESS, _RIDTABDATA0, _GSIZE, rid_tab_data[0]);
            rid_entries[rid_count].entry.numEntries = ((gsize == 0) ? 16 : gsize);

            rid_entries[rid_count].idx = table_index;
            rid_count++;
        }

        table_index++;
    }

    params->nextIndex = table_index;
    params->numEntries = rid_count;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_set_routing_id_valid_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_ROUTING_ID_VALID *p
)
{
    NvU32 rid_ctrl;
    NvU32 rid_tab_data0;
    NvU32 rid_tab_data1;
    NvU32 rid_tab_data2;
    NvU32 rid_tab_data3;
    NvU32 rid_tab_data4;
    NvU32 rid_tab_data5;
    NvU32 ram_address = p->firstIndex;
    NvU32 i;
    NvU32 ram_size;
    NvlStatus retval;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, p->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NPORT port #%d not valid\n",
            __FUNCTION__, p->portNum);
        return -NVL_BAD_ARGS;
    }

    ram_size = nvswitch_get_ingress_ram_size(device, NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRIDROUTERAM);
    if ((p->firstIndex >= ram_size) ||
        (p->numEntries > NVSWITCH_ROUTING_ID_ENTRIES_MAX) ||
        (p->firstIndex + p->numEntries > ram_size))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: routingId[%d..%d] overflows range %d..%d or size %d.\n",
            __FUNCTION__, p->firstIndex, p->firstIndex + p->numEntries - 1,
            0, ram_size - 1,
            NVSWITCH_ROUTING_ID_ENTRIES_MAX);
        return -NVL_BAD_ARGS;
    }

    // Stop traffic on the port
    retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_TRUE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to stop traffic on nport %d\n", p->portNum);
        return retval;
    }

    // Select RID RAM and disable Auto Increment.
    rid_ctrl =
        DRF_DEF(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, _SELECTSRIDROUTERAM) |
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, 0);


    for (i = 0; i < p->numEntries; i++)
    {
        /* set the ram address */
        rid_ctrl = FLD_SET_DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, ram_address++, rid_ctrl);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _REQRSPMAPADDR, rid_ctrl);

        rid_tab_data0 = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA0);
        rid_tab_data1 = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA1);
        rid_tab_data2 = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA2);
        rid_tab_data3 = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA3);
        rid_tab_data4 = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA4);
        rid_tab_data5 = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA5);

        // Set the valid bit in _RIDTABDATA5
        rid_tab_data5 = FLD_SET_DRF_NUM(_INGRESS, _RIDTABDATA5, _ACLVALID,
            p->entryValid[i], rid_tab_data5);

        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA1, rid_tab_data1);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA2, rid_tab_data2);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA3, rid_tab_data3);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA4, rid_tab_data4);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA5, rid_tab_data5);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RIDTABDATA0, rid_tab_data0);
    }

    // Allow traffic on the port
    retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_FALSE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to restart traffic on nport %d\n", p->portNum);
        return retval;
    }
    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_set_routing_id_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_ROUTING_ID *p
)
{
    NvU32 i, j;
    NvlStatus retval = NVL_SUCCESS;
    NvU32 ram_size;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, p->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "NPORT port #%d not valid\n",
            p->portNum);
        return -NVL_BAD_ARGS;
    }

    ram_size = nvswitch_get_ingress_ram_size(device, NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRIDROUTERAM);
    if ((p->firstIndex >= ram_size) ||
        (p->numEntries > NVSWITCH_ROUTING_ID_ENTRIES_MAX) ||
        (p->firstIndex + p->numEntries > ram_size))
    {
        NVSWITCH_PRINT(device, ERROR,
            "routingId[%d..%d] overflows range %d..%d or size %d.\n",
            p->firstIndex, p->firstIndex + p->numEntries - 1,
            0, ram_size - 1,
            NVSWITCH_ROUTING_ID_ENTRIES_MAX);
        return -NVL_BAD_ARGS;
    }

    for (i = 0; i < p->numEntries; i++)
    {
        if ((p->routingId[i].numEntries < 1) ||
            (p->routingId[i].numEntries > NVSWITCH_ROUTING_ID_DEST_PORT_LIST_MAX))
        {
            NVSWITCH_PRINT(device, ERROR,
                "routingId[%d].portList[] size %d overflows range %d..%d\n",
                i, p->routingId[i].numEntries,
                1, NVSWITCH_ROUTING_ID_DEST_PORT_LIST_MAX);
            return -NVL_BAD_ARGS;
        }

        for (j = 0; j < p->routingId[i].numEntries; j++)
        {
            if (p->routingId[i].portList[j].vcMap > DRF_MASK(NV_INGRESS_RIDTABDATA0_VC_MODE0))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "routingId[%d].portList[%d] vcMap 0x%x out of valid range (0x%x..0x%x)\n",
                    i, j,
                    p->routingId[i].portList[j].vcMap,
                    0, DRF_MASK(NV_INGRESS_RIDTABDATA0_VC_MODE0));
                return -NVL_BAD_ARGS;
            }

            if (p->routingId[i].portList[j].destPortNum > DRF_MASK(NV_INGRESS_RIDTABDATA0_PORT0))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "routingId[%d].portList[%d] destPortNum 0x%x out of valid range (0x%x..0x%x)\n",
                    i, j,
                    p->routingId[i].portList[j].destPortNum,
                    0, DRF_MASK(NV_INGRESS_RIDTABDATA0_PORT0));
                return -NVL_BAD_ARGS;
            }
        }
    }

    // Stop traffic on the port
    retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_TRUE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to stop traffic on nport %d\n", p->portNum);
        return retval;
    }

    _nvswitch_set_routing_id_lr10(device, p->portNum, p->firstIndex, p->numEntries, p->routingId);

    // Allow traffic on the port
    retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_FALSE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to restart traffic on nport %d\n", p->portNum);
        return retval;
    }

    return retval;
}

/*
 * CTRL_NVSWITCH_SET_ROUTING_LAN
 */

//
// Check the data field is present in the list.  Return either the data field
// or default if not present.
//
#define NVSWITCH_PORTLIST_VALID_LR10(_entry, _idx, _field, _default) \
    ((_idx < _entry.numEntries) ? _entry.portList[_idx]._field  : _default)

static void
_nvswitch_set_routing_lan_lr10
(
    nvswitch_device *device,
    NvU32 portNum,
    NvU32 firstIndex,
    NvU32 numEntries,
    NVSWITCH_ROUTING_LAN_ENTRY *routing_lan
)
{
    NvU32 i;

    NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _REQRSPMAPADDR,
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, firstIndex) |
        DRF_DEF(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, _SELECTSRLANROUTERAM) |
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, 1));

    for (i = 0; i < numEntries; i++)
    {
        //
        // NOTE: The GRP_SIZE field is 4-bits.  A subgroup is size 1 through 16
        // with encoding 0x0=16 and 0x1=1, ..., 0xF=15.
        // Programming of GRP_SIZE takes advantage of the inherent masking of
        // DRF_NUM to truncate 16 to 0.
        // See bug #3300673
        //

        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RLANTABDATA1,
            DRF_NUM(_INGRESS, _RLANTABDATA1, _GRP_SEL_3, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 3, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA1, _GRP_SIZE_3, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 3, groupSize, 1)) |
            DRF_NUM(_INGRESS, _RLANTABDATA1, _GRP_SEL_4, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 4, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA1, _GRP_SIZE_4, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 4, groupSize, 1)) |
            DRF_NUM(_INGRESS, _RLANTABDATA1, _GRP_SEL_5, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 5, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA1, _GRP_SIZE_5, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 5, groupSize, 1)));

        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RLANTABDATA2,
            DRF_NUM(_INGRESS, _RLANTABDATA2, _GRP_SEL_6, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 6, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA2, _GRP_SIZE_6, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 6, groupSize, 1)) |
            DRF_NUM(_INGRESS, _RLANTABDATA2, _GRP_SEL_7, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 7, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA2, _GRP_SIZE_7, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 7, groupSize, 1)) |
            DRF_NUM(_INGRESS, _RLANTABDATA2, _GRP_SEL_8, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 8, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA2, _GRP_SIZE_8, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 8, groupSize, 1)));

        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RLANTABDATA3,
            DRF_NUM(_INGRESS, _RLANTABDATA3, _GRP_SEL_9, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 9, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA3, _GRP_SIZE_9, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 9, groupSize, 1)) |
            DRF_NUM(_INGRESS, _RLANTABDATA3, _GRP_SEL_10, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 10, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA3, _GRP_SIZE_10, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 10, groupSize, 1)) |
            DRF_NUM(_INGRESS, _RLANTABDATA3, _GRP_SEL_11, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 11, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA3, _GRP_SIZE_11, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 11, groupSize, 1)));

        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RLANTABDATA4,
            DRF_NUM(_INGRESS, _RLANTABDATA4, _GRP_SEL_12, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 12, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA4, _GRP_SIZE_12, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 12, groupSize, 1)) |
            DRF_NUM(_INGRESS, _RLANTABDATA4, _GRP_SEL_13, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 13, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA4, _GRP_SIZE_13, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 13, groupSize, 1)) |
            DRF_NUM(_INGRESS, _RLANTABDATA4, _GRP_SEL_14, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 14, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA4, _GRP_SIZE_14, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 14, groupSize, 1)));

        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RLANTABDATA5,
            DRF_NUM(_INGRESS, _RLANTABDATA5, _GRP_SEL_15, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 15, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA5, _GRP_SIZE_15, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 15, groupSize, 1)) |
            DRF_NUM(_INGRESS, _RLANTABDATA5, _ACLVALID,  routing_lan[i].entryValid));

        // Write last and auto-increment
        NVSWITCH_LINK_WR32_LR10(device, portNum, NPORT, _INGRESS, _RLANTABDATA0,
            DRF_NUM(_INGRESS, _RLANTABDATA0, _GRP_SEL_0, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 0, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA0, _GRP_SIZE_0, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 0, groupSize, 1)) |
            DRF_NUM(_INGRESS, _RLANTABDATA0, _GRP_SEL_1, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 1, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA0, _GRP_SIZE_1, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 1, groupSize, 1)) |
            DRF_NUM(_INGRESS, _RLANTABDATA0, _GRP_SEL_2, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 2, groupSelect, 0)) |
            DRF_NUM(_INGRESS, _RLANTABDATA0, _GRP_SIZE_2, NVSWITCH_PORTLIST_VALID_LR10(routing_lan[i], 2, groupSize, 1)));
    }
}

NvlStatus
nvswitch_ctrl_set_routing_lan_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_ROUTING_LAN *p
)
{
    NvU32 i, j;
    NvlStatus retval = NVL_SUCCESS;
    NvU32 ram_size;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, p->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NPORT port #%d not valid\n",
            __FUNCTION__, p->portNum);
        return -NVL_BAD_ARGS;
    }

    ram_size = nvswitch_get_ingress_ram_size(device, NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRLANROUTERAM);
    if ((p->firstIndex >= ram_size) ||
        (p->numEntries > NVSWITCH_ROUTING_LAN_ENTRIES_MAX) ||
        (p->firstIndex + p->numEntries > ram_size))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: routingLan[%d..%d] overflows range %d..%d or size %d.\n",
            __FUNCTION__, p->firstIndex, p->firstIndex + p->numEntries - 1,
            0, ram_size - 1,
            NVSWITCH_ROUTING_LAN_ENTRIES_MAX);
        return -NVL_BAD_ARGS;
    }

    for (i = 0; i < p->numEntries; i++)
    {
        if (p->routingLan[i].numEntries > NVSWITCH_ROUTING_LAN_GROUP_SEL_MAX)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: routingLan[%d].portList[] size %d overflows range %d..%d\n",
                __FUNCTION__, i, p->routingLan[i].numEntries,
                0, NVSWITCH_ROUTING_LAN_GROUP_SEL_MAX);
            return -NVL_BAD_ARGS;
        }

        for (j = 0; j < p->routingLan[i].numEntries; j++)
        {
            if (p->routingLan[i].portList[j].groupSelect > DRF_MASK(NV_INGRESS_RLANTABDATA0_GRP_SEL_0))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: routingLan[%d].portList[%d] groupSelect 0x%x out of valid range (0x%x..0x%x)\n",
                    __FUNCTION__, i, j,
                    p->routingLan[i].portList[j].groupSelect,
                    0, DRF_MASK(NV_INGRESS_RLANTABDATA0_GRP_SEL_0));
                return -NVL_BAD_ARGS;
            }

            if ((p->routingLan[i].portList[j].groupSize == 0) ||
                (p->routingLan[i].portList[j].groupSize > DRF_MASK(NV_INGRESS_RLANTABDATA0_GRP_SIZE_0) + 1))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: routingLan[%d].portList[%d] groupSize 0x%x out of valid range (0x%x..0x%x)\n",
                    __FUNCTION__, i, j,
                    p->routingLan[i].portList[j].groupSize,
                    1, DRF_MASK(NV_INGRESS_RLANTABDATA0_GRP_SIZE_0) + 1);
                return -NVL_BAD_ARGS;
            }
        }
    }

    // Stop traffic on the port
    retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_TRUE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to stop traffic on nport %d\n", p->portNum);
        return retval;
    }

    _nvswitch_set_routing_lan_lr10(device, p->portNum, p->firstIndex, p->numEntries, p->routingLan);

    // Allow traffic on the port
    retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_FALSE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to restart traffic on nport %d\n", p->portNum);
        return retval;
    }

    return retval;
}

#define NVSWITCH_NUM_RLANTABDATA_REGS_LR10 6

NvlStatus
nvswitch_ctrl_get_routing_lan_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_ROUTING_LAN_PARAMS *params
)
{
    NVSWITCH_ROUTING_LAN_IDX_ENTRY *rlan_entries;
    NvU32 table_index;
    NvU32 rlan_tab_data[NVSWITCH_NUM_RLANTABDATA_REGS_LR10]; // 6 RLAN tables
    NvU32 rlan_count;
    NvU32 ram_size;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, params->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NPORT port #%d not valid\n",
            __FUNCTION__, params->portNum);
        return -NVL_BAD_ARGS;
    }

    ram_size = nvswitch_get_ingress_ram_size(device, NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRLANROUTERAM);
    if ((params->firstIndex >= ram_size))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: routingLan first index %d out of range[%d..%d].\n",
            __FUNCTION__, params->firstIndex, 0, ram_size - 1);
        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(params->entries, 0, (NVSWITCH_ROUTING_LAN_ENTRIES_MAX *
        sizeof(NVSWITCH_ROUTING_LAN_IDX_ENTRY)));

    table_index = params->firstIndex;
    rlan_entries = params->entries;
    rlan_count = 0;

    /* set table offset */
    NVSWITCH_LINK_WR32_LR10(device, params->portNum, NPORT, _INGRESS, _REQRSPMAPADDR,
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, params->firstIndex) |
        DRF_DEF(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, _SELECTSRLANROUTERAM)   |
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, 1));

    while (rlan_count < NVSWITCH_ROUTING_LAN_ENTRIES_MAX &&
           table_index < ram_size)
    {
        /* read one entry */
        rlan_tab_data[0] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RLANTABDATA0);
        rlan_tab_data[1] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RLANTABDATA1);
        rlan_tab_data[2] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RLANTABDATA2);
        rlan_tab_data[3] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RLANTABDATA3);
        rlan_tab_data[4] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RLANTABDATA4);
        rlan_tab_data[5] = NVSWITCH_LINK_RD32_LR10(device, params->portNum, NPORT, _INGRESS, _RLANTABDATA5);

        /* add to rlan_entries list if nonzero */
        if (rlan_tab_data[0] || rlan_tab_data[1] || rlan_tab_data[2] ||
            rlan_tab_data[3] || rlan_tab_data[4] || rlan_tab_data[5])
        {
            rlan_entries[rlan_count].entry.portList[0].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA0, _GRP_SEL_0, rlan_tab_data[0]);
            rlan_entries[rlan_count].entry.portList[0].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA0, _GRP_SIZE_0, rlan_tab_data[0]);
            if (rlan_entries[rlan_count].entry.portList[0].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[0].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[1].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA0, _GRP_SEL_1, rlan_tab_data[0]);
            rlan_entries[rlan_count].entry.portList[1].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA0, _GRP_SIZE_1, rlan_tab_data[0]);
            if (rlan_entries[rlan_count].entry.portList[1].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[1].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[2].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA0, _GRP_SEL_2, rlan_tab_data[0]);
            rlan_entries[rlan_count].entry.portList[2].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA0, _GRP_SIZE_2, rlan_tab_data[0]);
            if (rlan_entries[rlan_count].entry.portList[2].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[2].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[3].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA1, _GRP_SEL_3, rlan_tab_data[1]);
            rlan_entries[rlan_count].entry.portList[3].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA1, _GRP_SIZE_3, rlan_tab_data[1]);
            if (rlan_entries[rlan_count].entry.portList[3].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[3].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[4].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA1, _GRP_SEL_4, rlan_tab_data[1]);
            rlan_entries[rlan_count].entry.portList[4].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA1, _GRP_SIZE_4, rlan_tab_data[1]);
            if (rlan_entries[rlan_count].entry.portList[4].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[4].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[5].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA1, _GRP_SEL_5, rlan_tab_data[1]);
            rlan_entries[rlan_count].entry.portList[5].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA1, _GRP_SIZE_5, rlan_tab_data[1]);
            if (rlan_entries[rlan_count].entry.portList[5].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[5].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[6].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA2, _GRP_SEL_6, rlan_tab_data[2]);
            rlan_entries[rlan_count].entry.portList[6].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA2, _GRP_SIZE_6, rlan_tab_data[2]);
            if (rlan_entries[rlan_count].entry.portList[6].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[6].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[7].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA2, _GRP_SEL_7, rlan_tab_data[2]);
            rlan_entries[rlan_count].entry.portList[7].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA2, _GRP_SIZE_7, rlan_tab_data[2]);
            if (rlan_entries[rlan_count].entry.portList[7].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[7].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[8].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA2, _GRP_SEL_8, rlan_tab_data[2]);
            rlan_entries[rlan_count].entry.portList[8].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA2, _GRP_SIZE_8, rlan_tab_data[2]);
            if (rlan_entries[rlan_count].entry.portList[8].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[8].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[9].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA3, _GRP_SEL_9, rlan_tab_data[3]);
            rlan_entries[rlan_count].entry.portList[9].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA3, _GRP_SIZE_9, rlan_tab_data[3]);
            if (rlan_entries[rlan_count].entry.portList[9].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[9].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[10].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA3, _GRP_SEL_10, rlan_tab_data[3]);
            rlan_entries[rlan_count].entry.portList[10].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA3, _GRP_SIZE_10, rlan_tab_data[3]);
            if (rlan_entries[rlan_count].entry.portList[10].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[10].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[11].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA3, _GRP_SEL_11, rlan_tab_data[3]);
            rlan_entries[rlan_count].entry.portList[11].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA3, _GRP_SIZE_11, rlan_tab_data[3]);
            if (rlan_entries[rlan_count].entry.portList[11].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[11].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[12].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA4, _GRP_SEL_12, rlan_tab_data[4]);
            rlan_entries[rlan_count].entry.portList[12].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA4, _GRP_SIZE_12, rlan_tab_data[4]);
            if (rlan_entries[rlan_count].entry.portList[12].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[12].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[13].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA4, _GRP_SEL_13, rlan_tab_data[4]);
            rlan_entries[rlan_count].entry.portList[13].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA4, _GRP_SIZE_13, rlan_tab_data[4]);
            if (rlan_entries[rlan_count].entry.portList[13].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[13].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[14].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA4, _GRP_SEL_14, rlan_tab_data[4]);
            rlan_entries[rlan_count].entry.portList[14].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA4, _GRP_SIZE_14, rlan_tab_data[4]);
            if (rlan_entries[rlan_count].entry.portList[14].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[14].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.portList[15].groupSelect = DRF_VAL(_INGRESS, _RLANTABDATA5, _GRP_SEL_15, rlan_tab_data[5]);
            rlan_entries[rlan_count].entry.portList[15].groupSize   = DRF_VAL(_INGRESS, _RLANTABDATA5, _GRP_SIZE_15, rlan_tab_data[5]);
            if (rlan_entries[rlan_count].entry.portList[15].groupSize == 0)
            {
                rlan_entries[rlan_count].entry.portList[15].groupSize = 16;
            }

            rlan_entries[rlan_count].entry.entryValid               = DRF_VAL(_INGRESS, _RLANTABDATA5, _ACLVALID, rlan_tab_data[5]);
            rlan_entries[rlan_count].entry.numEntries = NVSWITCH_ROUTING_ID_DEST_PORT_LIST_MAX;
            rlan_entries[rlan_count].idx  = table_index;

            rlan_count++;
        }

        table_index++;
    }

    params->nextIndex  = table_index;
    params->numEntries = rlan_count;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_set_routing_lan_valid_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_ROUTING_LAN_VALID *p
)
{
    NvU32 rlan_ctrl;
    NvU32 rlan_tab_data[NVSWITCH_NUM_RLANTABDATA_REGS_LR10]; // 6 RLAN tables
    NvU32 ram_address = p->firstIndex;
    NvU32 i;
    NvU32 ram_size;
    NvlStatus retval;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, p->portNum))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NPORT port #%d not valid\n",
            __FUNCTION__, p->portNum);
        return -NVL_BAD_ARGS;
    }

    ram_size = nvswitch_get_ingress_ram_size(device, NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRLANROUTERAM);
    if ((p->firstIndex >= ram_size) ||
        (p->numEntries > NVSWITCH_ROUTING_LAN_ENTRIES_MAX) ||
        (p->firstIndex + p->numEntries > ram_size))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: routingLan[%d..%d] overflows range %d..%d or size %d.\n",
            __FUNCTION__, p->firstIndex, p->firstIndex + p->numEntries - 1,
            0, ram_size - 1,
            NVSWITCH_ROUTING_LAN_ENTRIES_MAX);
        return -NVL_BAD_ARGS;
    }

    // Stop traffic on the port
    retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_TRUE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to stop traffic on nport %d\n", p->portNum);
        return retval;
    }

    // Select RLAN RAM and disable Auto Increament.
    rlan_ctrl =
        DRF_DEF(_INGRESS, _REQRSPMAPADDR, _RAM_SEL, _SELECTSRLANROUTERAM) |
        DRF_NUM(_INGRESS, _REQRSPMAPADDR, _AUTO_INCR, 0);

    for (i = 0; i < p->numEntries; i++)
    {
        /* set the RAM address */
        rlan_ctrl = FLD_SET_DRF_NUM(_INGRESS, _REQRSPMAPADDR, _RAM_ADDRESS, ram_address++, rlan_ctrl);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _REQRSPMAPADDR, rlan_ctrl);

        rlan_tab_data[0] = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA0);
        rlan_tab_data[1] = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA1);
        rlan_tab_data[2] = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA2);
        rlan_tab_data[3] = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA3);
        rlan_tab_data[4] = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA4);
        rlan_tab_data[5] = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA5);

        // Set the valid bit in _RLANTABDATA5
        rlan_tab_data[5] = FLD_SET_DRF_NUM(_INGRESS, _RLANTABDATA5, _ACLVALID,
            p->entryValid[i], rlan_tab_data[5]);

        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA1, rlan_tab_data[1]);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA2, rlan_tab_data[2]);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA3, rlan_tab_data[3]);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA4, rlan_tab_data[4]);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA5, rlan_tab_data[5]);
        NVSWITCH_LINK_WR32_LR10(device, p->portNum, NPORT, _INGRESS, _RLANTABDATA0, rlan_tab_data[0]);
    }

    // Allow traffic on the port
    retval = nvswitch_soe_issue_ingress_stop(device, p->portNum, NV_FALSE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to restart traffic on nport %d\n", p->portNum);
        return retval;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Send priv ring command and wait for completion
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 * @param[in] cmd           encoded priv ring command
 */
NvlStatus
nvswitch_ring_master_cmd_lr10
(
    nvswitch_device *device,
    NvU32 cmd
)
{
    NvU32 value;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;

    NVSWITCH_REG_WR32(device, _PPRIV_MASTER, _RING_COMMAND, cmd);

    nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        value = NVSWITCH_REG_RD32(device, _PPRIV_MASTER, _RING_COMMAND);
        if (FLD_TEST_DRF(_PPRIV_MASTER, _RING_COMMAND, _CMD, _NO_CMD, value))
        {
            break;
        }

        nvswitch_os_sleep(1);
    }
    while (keepPolling);

    if (!FLD_TEST_DRF(_PPRIV_MASTER, _RING_COMMAND, _CMD, _NO_CMD, value))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Timeout waiting for RING_COMMAND == NO_CMD (cmd=0x%x).\n",
            __FUNCTION__, cmd);
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    return NVL_SUCCESS;
}

/*
 * @brief Process the information read from ROM tables and apply it to device
 * settings.
 *
 * @param[in] device    a reference to the device to query
 * @param[in] firmware  Information parsed from ROM tables
 */
static void
_nvswitch_process_firmware_info_lr10
(
    nvswitch_device *device,
    NVSWITCH_FIRMWARE *firmware
)
{
    NvU32 idx_link;
    NvU64 link_enable_mask;

    if (device->firmware.firmware_size == 0)
    {
        return;
    }

    if (device->firmware.nvlink.link_config_found)
    {
        link_enable_mask = ((NvU64)device->regkeys.link_enable_mask2 << 32 |
                            (NvU64)device->regkeys.link_enable_mask);
        //
        // If the link enables were not already overridden by regkey, then
        // apply the ROM link enables
        //
        if (link_enable_mask == NV_U64_MAX)
        {
            for (idx_link = 0; idx_link < nvswitch_get_num_links(device); idx_link++)
            {
                if ((device->firmware.nvlink.link_enable_mask & NVBIT64(idx_link)) == 0)
                {
                    device->link[idx_link].valid = NV_FALSE;
                }
            }
        }
    }
}

void
nvswitch_init_npg_multicast_lr10
(
    nvswitch_device *device
)
{
    NvU32 idx_npg;
    NvU32 idx_nport;
    NvU32 nport_mask;

    //
    // Walk the NPGs and build the mask of extant NPORTs
    //
    for (idx_npg = 0; idx_npg < NVSWITCH_ENG_COUNT(device, NPG, ); idx_npg++)
    {
        if (NVSWITCH_ENG_IS_VALID(device, NPG, idx_npg))
        {
            nport_mask = 0;
            for (idx_nport = 0; idx_nport < NVSWITCH_NPORT_PER_NPG; idx_nport++)
            {
                nport_mask |=
                    (NVSWITCH_ENG_IS_VALID(device, NPORT, idx_npg*NVSWITCH_NPORT_PER_NPG + idx_nport) ?
                    NVBIT(idx_nport) : 0x0);
            }

            NVSWITCH_NPG_WR32_LR10(device, idx_npg,
                _NPG, _CTRL_PRI_MULTICAST,
                DRF_NUM(_NPG, _CTRL_PRI_MULTICAST, _NPORT_ENABLE, nport_mask) |
                DRF_DEF(_NPG, _CTRL_PRI_MULTICAST, _READ_MODE, _AND_ALL_BUSSES));

            NVSWITCH_NPGPERF_WR32_LR10(device, idx_npg,
                _NPGPERF, _CTRL_PRI_MULTICAST,
                DRF_NUM(_NPGPERF, _CTRL_PRI_MULTICAST, _NPORT_ENABLE, nport_mask) |
                DRF_DEF(_NPGPERF, _CTRL_PRI_MULTICAST, _READ_MODE, _AND_ALL_BUSSES));
        }
    }
}

static NvlStatus
nvswitch_clear_nport_rams_lr10
(
    nvswitch_device *device
)
{
    NvU32 idx_nport;
    NvU64 nport_mask = 0;
    NvU32 zero_init_mask;
    NvU32 val;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;
    NvlStatus retval = NVL_SUCCESS;

    // Build the mask of available NPORTs
    for (idx_nport = 0; idx_nport < NVSWITCH_ENG_COUNT(device, NPORT, ); idx_nport++)
    {
        if (NVSWITCH_ENG_IS_VALID(device, NPORT, idx_nport))
        {
            nport_mask |= NVBIT64(idx_nport);
        }
    }

    // Start the HW zero init
    zero_init_mask =
        DRF_DEF(_NPORT, _INITIALIZATION, _TAGPOOLINIT_0, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _TAGPOOLINIT_1, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _TAGPOOLINIT_2, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _TAGPOOLINIT_3, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _TAGPOOLINIT_4, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _TAGPOOLINIT_5, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _TAGPOOLINIT_6, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _LINKTABLEINIT, _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _REMAPTABINIT,  _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _RIDTABINIT,    _HWINIT) |
        DRF_DEF(_NPORT, _INITIALIZATION, _RLANTABINIT,   _HWINIT);

    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _NPORT, _INITIALIZATION,
        zero_init_mask);

    nvswitch_timeout_create(25*NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);

    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        // Check each enabled NPORT that is still pending until all are done
        for (idx_nport = 0; idx_nport < NVSWITCH_ENG_COUNT(device, NPORT, ); idx_nport++)
        {
            if (NVSWITCH_ENG_IS_VALID(device, NPORT, idx_nport) && (nport_mask & NVBIT64(idx_nport)))
            {
                val = NVSWITCH_ENG_RD32_LR10(device, NPORT, idx_nport, _NPORT, _INITIALIZATION);
                if (val == zero_init_mask)
                {
                    nport_mask &= ~NVBIT64(idx_nport);
                }
            }
        }

        if (nport_mask == 0)
        {
            break;
        }

        nvswitch_os_sleep(1);
    }
    while (keepPolling);

    if (nport_mask != 0)
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: Timeout waiting for NV_NPORT_INITIALIZATION (0x%llx)\n",
            __FUNCTION__, nport_mask);
        // Bug 2974064: Review this timeout handling (fall through)
        retval = -NVL_ERR_INVALID_STATE;
    }

    //bug 2737147 requires SW To init this crumbstore setting for LR10
    val = DRF_NUM(_TSTATE, _RAM_ADDRESS, _ADDR, 0)             |
          DRF_DEF(_TSTATE, _RAM_ADDRESS, _SELECT, _CRUMBSTORE_RAM) |
          DRF_NUM(_TSTATE, _RAM_ADDRESS, _AUTO_INCR, 0)        |
          DRF_DEF(_TSTATE, _RAM_ADDRESS, _VC, _VC5_TRANSDONE);

    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _TSTATE, _RAM_ADDRESS, val);

    return retval;
}

static void
_nvswitch_init_nport_ecc_control_lr10
(
    nvswitch_device *device
)
{
    // Set ingress ECC error limits
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _INGRESS, _ERR_NCISOC_HDR_ECC_ERROR_COUNTER,
        DRF_NUM(_INGRESS, _ERR_NCISOC_HDR_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _INGRESS, _ERR_NCISOC_HDR_ECC_ERROR_COUNTER_LIMIT, 1);

    // Set egress ECC error limits
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _EGRESS, _ERR_NXBAR_ECC_ERROR_COUNTER,
        DRF_NUM(_EGRESS, _ERR_NXBAR_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _EGRESS, _ERR_NXBAR_ECC_ERROR_COUNTER_LIMIT, 1);

    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _EGRESS, _ERR_RAM_OUT_ECC_ERROR_COUNTER,
        DRF_NUM(_EGRESS, _ERR_RAM_OUT_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _EGRESS, _ERR_RAM_OUT_ECC_ERROR_COUNTER_LIMIT, 1);

    // Set route ECC error limits
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _ROUTE, _ERR_NVS_ECC_ERROR_COUNTER,
        DRF_NUM(_ROUTE, _ERR_NVS_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _ROUTE, _ERR_NVS_ECC_ERROR_COUNTER_LIMIT, 1);

    // Set tstate ECC error limits
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER,
        DRF_NUM(_TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _TSTATE, _ERR_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT, 1);

    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER,
        DRF_NUM(_TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _TSTATE, _ERR_TAGPOOL_ECC_ERROR_COUNTER_LIMIT, 1);

    // Set sourcetrack ECC error limits to _PROD value
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _SOURCETRACK, _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT,
        DRF_NUM(_SOURCETRACK, _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _SOURCETRACK, _ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT, 1);

    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _SOURCETRACK, _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT,
        DRF_NUM(_SOURCETRACK, _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_COUNTER, _ERROR_COUNT, 0x0));
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _SOURCETRACK, _ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT, 1);

    // Enable ECC/parity
    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _INGRESS, _ERR_ECC_CTRL,
        DRF_DEF(_INGRESS, _ERR_ECC_CTRL, _NCISOC_HDR_ECC_ENABLE, __PROD) |
        DRF_DEF(_INGRESS, _ERR_ECC_CTRL, _NCISOC_PARITY_ENABLE, __PROD) |
        DRF_DEF(_INGRESS, _ERR_ECC_CTRL, _REMAPTAB_ECC_ENABLE, __PROD) |
        DRF_DEF(_INGRESS, _ERR_ECC_CTRL, _RIDTAB_ECC_ENABLE, __PROD) |
        DRF_DEF(_INGRESS, _ERR_ECC_CTRL, _RLANTAB_ECC_ENABLE, __PROD));

    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _EGRESS, _ERR_ECC_CTRL,
        DRF_DEF(_EGRESS, _ERR_ECC_CTRL, _NXBAR_ECC_ENABLE, __PROD) |
        DRF_DEF(_EGRESS, _ERR_ECC_CTRL, _NXBAR_PARITY_ENABLE, __PROD) |
        DRF_DEF(_EGRESS, _ERR_ECC_CTRL, _RAM_OUT_ECC_ENABLE, __PROD) |
        DRF_DEF(_EGRESS, _ERR_ECC_CTRL, _NCISOC_ECC_ENABLE, __PROD) |
        DRF_DEF(_EGRESS, _ERR_ECC_CTRL, _NCISOC_PARITY_ENABLE, __PROD));

    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _ROUTE, _ERR_ECC_CTRL,
        DRF_DEF(_ROUTE, _ERR_ECC_CTRL, _GLT_ECC_ENABLE, __PROD) |
        DRF_DEF(_ROUTE, _ERR_ECC_CTRL, _NVS_ECC_ENABLE, __PROD));

    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _TSTATE, _ERR_ECC_CTRL,
        DRF_DEF(_TSTATE, _ERR_ECC_CTRL, _CRUMBSTORE_ECC_ENABLE, __PROD) |
        DRF_DEF(_TSTATE, _ERR_ECC_CTRL, _TAGPOOL_ECC_ENABLE, __PROD) |
        DRF_DEF(_TSTATE, _ERR_ECC_CTRL, _TD_TID_ECC_ENABLE, _DISABLE));

    NVSWITCH_BCAST_WR32_LR10(device, NPORT, _SOURCETRACK, _ERR_ECC_CTRL,
        DRF_DEF(_SOURCETRACK, _ERR_ECC_CTRL, _CREQ_TCEN0_CRUMBSTORE_ECC_ENABLE, __PROD) |
        DRF_DEF(_SOURCETRACK, _ERR_ECC_CTRL, _CREQ_TCEN0_TD_CRUMBSTORE_ECC_ENABLE, _DISABLE) |
        DRF_DEF(_SOURCETRACK, _ERR_ECC_CTRL, _CREQ_TCEN1_CRUMBSTORE_ECC_ENABLE, __PROD));
}

static void
_nvswitch_init_cmd_routing
(
    nvswitch_device *device
)
{
    NvU32 val;

    //Set Hash policy for the requests.
    val = DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE0, _RFUN1, _SPRAY) |
          DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE0, _RFUN2, _SPRAY) |
          DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE0, _RFUN4, _SPRAY) |
          DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE0, _RFUN7, _SPRAY);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _ROUTE, _CMD_ROUTE_TABLE0, val);

    // Set Random policy for reponses.
    val = DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE2, _RFUN16, _RANDOM) |
          DRF_DEF(_ROUTE, _CMD_ROUTE_TABLE2, _RFUN17, _RANDOM);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _ROUTE, _CMD_ROUTE_TABLE2, val);
}

static NvlStatus
_nvswitch_init_portstat_counters
(
    nvswitch_device *device
)
{
    NvlStatus retval;
    NvU32 idx_channel;
    NVSWITCH_SET_LATENCY_BINS default_latency_bins;
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

    chip_device->latency_stats = nvswitch_os_malloc(sizeof(NVSWITCH_LATENCY_STATS_LR10));
    if (chip_device->latency_stats == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed allocate memory for latency stats\n",
            __FUNCTION__);
        return -NVL_NO_MEM;
    }

    nvswitch_os_memset(chip_device->latency_stats, 0, sizeof(NVSWITCH_LATENCY_STATS_LR10));

    //
    // These bin thresholds are values provided by Arch based off
    // switch latency expectations.
    //
    for (idx_channel=0; idx_channel < NVSWITCH_NUM_VCS_LR10; idx_channel++)
    {
        default_latency_bins.bin[idx_channel].lowThreshold = 120;    // 120ns
        default_latency_bins.bin[idx_channel].medThreshold = 200;    // 200ns
        default_latency_bins.bin[idx_channel].hiThreshold  = 1000;   // 1us
    }

    chip_device->latency_stats->sample_interval_msec = 3000; // 3 second sample interval

    retval = nvswitch_ctrl_set_latency_bins(device, &default_latency_bins);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to set latency bins\n",
            __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return retval;
    }

    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _NPORT, _PORTSTAT_CONTROL,
        DRF_DEF(_NPORT, _PORTSTAT_CONTROL, _SWEEPMODE, _SWONDEMAND) |
        DRF_DEF(_NPORT, _PORTSTAT_CONTROL, _RANGESELECT, _BITS13TO0));

     NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _NPORT, _PORTSTAT_SOURCE_FILTER_0,
         DRF_NUM(_NPORT, _PORTSTAT_SOURCE_FILTER_0, _SRCFILTERBIT, 0xFFFFFFFF));

    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _NPORT, _PORTSTAT_SOURCE_FILTER_1,
        DRF_NUM(_NPORT, _PORTSTAT_SOURCE_FILTER_1, _SRCFILTERBIT, 0xF));

    // Set window limit to the maximum value
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _NPORT, _PORTSTAT_WINDOW_LIMIT, 0xffffffff);

     NVSWITCH_SAW_WR32_LR10(device, _NVLSAW, _GLBLLATENCYTIMERCTRL,
         DRF_DEF(_NVLSAW, _GLBLLATENCYTIMERCTRL, _ENABLE, _ENABLE));

     NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _NPORT, _PORTSTAT_SNAP_CONTROL,
         DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _STARTCOUNTER, _ENABLE) |
         DRF_DEF(_NPORT, _PORTSTAT_SNAP_CONTROL, _SNAPONDEMAND, _DISABLE));

     return NVL_SUCCESS;
}

NvlStatus
nvswitch_init_nxbar_lr10
(
    nvswitch_device *device
)
{
    NvU32 tileout;

    // Setting this bit will send error detection info to NPG.
    NVSWITCH_BCAST_WR32_LR10(device, TILE, _NXBAR, _TILE_ERR_CYA,
        DRF_DEF(_NXBAR, _TILE_ERR_CYA, _SRCID_UPDATE_AT_EGRESS_CTRL, __PROD));

    for (tileout = 0; tileout < NUM_NXBAR_TILEOUTS_PER_TC_LR10; tileout++)
    {
        NVSWITCH_BCAST_WR32_LR10(device, NXBAR, _NXBAR, _TC_TILEOUT_ERR_CYA(tileout),
            DRF_DEF(_NXBAR, _TC_TILEOUT0_ERR_CYA, _SRCID_UPDATE_AT_EGRESS_CTRL, __PROD));
    }

    // Enable idle-based clk gating and setup delay count.
    NVSWITCH_BCAST_WR32_LR10(device, TILE, _NXBAR, _TILE_PRI_NXBAR_TILE_CG,
        DRF_DEF(_NXBAR, _TILE_PRI_NXBAR_TILE_CG, _IDLE_CG_EN, __PROD) |
        DRF_DEF(_NXBAR, _TILE_PRI_NXBAR_TILE_CG, _IDLE_CG_DLY_CNT, __PROD));

    NVSWITCH_BCAST_WR32_LR10(device, NXBAR, _NXBAR, _TC_PRI_NXBAR_TC_CG,
        DRF_DEF(_NXBAR, _TC_PRI_NXBAR_TC_CG, _IDLE_CG_EN, __PROD) |
        DRF_DEF(_NXBAR, _TC_PRI_NXBAR_TC_CG, _IDLE_CG_DLY_CNT, __PROD));

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_init_nport_lr10
(
    nvswitch_device *device
)
{
    NvU32 data32, timeout;
    NvU32 idx_nport;
    NvU32 num_nports;

    num_nports = NVSWITCH_ENG_COUNT(device, NPORT, );

    for (idx_nport = 0; idx_nport < num_nports; idx_nport++)
    {
        // Find the first valid nport
        if (NVSWITCH_ENG_IS_VALID(device, NPORT, idx_nport))
        {
            break;
        }
    }

    // There were no valid nports
    if (idx_nport == num_nports)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: No valid nports found!\n", __FUNCTION__);
        return -NVL_ERR_INVALID_STATE;
    }

    _nvswitch_init_nport_ecc_control_lr10(device);

    data32 = NVSWITCH_NPORT_RD32_LR10(device, idx_nport, _ROUTE, _ROUTE_CONTROL);
    data32 = FLD_SET_DRF(_ROUTE, _ROUTE_CONTROL, _URRESPENB, __PROD, data32);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _ROUTE, _ROUTE_CONTROL, data32);

    data32 = NVSWITCH_NPORT_RD32_LR10(device, idx_nport, _EGRESS, _CTRL);
    data32 = FLD_SET_DRF(_EGRESS, _CTRL, _DESTINATIONIDCHECKENB, __PROD, data32);
    data32 = FLD_SET_DRF(_EGRESS, _CTRL, _CTO_ENB, __PROD, data32);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _EGRESS, _CTRL, data32);

    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _EGRESS, _CTO_TIMER_LIMIT,
        DRF_DEF(_EGRESS, _CTO_TIMER_LIMIT, _LIMIT, __PROD));

    if (DRF_VAL(_SWITCH_REGKEY, _ATO_CONTROL, _DISABLE, device->regkeys.ato_control) ==
        NV_SWITCH_REGKEY_ATO_CONTROL_DISABLE_TRUE)
    {
        // ATO Disable
        data32 = NVSWITCH_NPORT_RD32_LR10(device, idx_nport, _TSTATE, _TAGSTATECONTROL);
        data32 = FLD_SET_DRF(_TSTATE, _TAGSTATECONTROL, _ATO_ENB, _OFF, data32);
        NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _TSTATE, _TAGSTATECONTROL, data32);
    }
    else
    {
        // ATO Enable
        data32 = NVSWITCH_NPORT_RD32_LR10(device, idx_nport, _TSTATE, _TAGSTATECONTROL);
        data32 = FLD_SET_DRF(_TSTATE, _TAGSTATECONTROL, _ATO_ENB, _ON, data32);
        NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _TSTATE, _TAGSTATECONTROL, data32);

        // ATO Timeout value
        timeout = DRF_VAL(_SWITCH_REGKEY, _ATO_CONTROL, _TIMEOUT, device->regkeys.ato_control);
        if (timeout != NV_SWITCH_REGKEY_ATO_CONTROL_TIMEOUT_DEFAULT)
        {
            NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _TSTATE, _ATO_TIMER_LIMIT,
                DRF_NUM(_TSTATE, _ATO_TIMER_LIMIT, _LIMIT, timeout));
        }
        else
        {
            NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _TSTATE, _ATO_TIMER_LIMIT,
                DRF_DEF(_TSTATE, _ATO_TIMER_LIMIT, _LIMIT, __PROD));
        }
    }

    if (DRF_VAL(_SWITCH_REGKEY, _STO_CONTROL, _DISABLE, device->regkeys.sto_control) ==
        NV_SWITCH_REGKEY_STO_CONTROL_DISABLE_TRUE)
    {
        // STO Disable
        data32 = NVSWITCH_NPORT_RD32_LR10(device, idx_nport, _SOURCETRACK, _CTRL);
        data32 = FLD_SET_DRF(_SOURCETRACK, _CTRL, _STO_ENB, _OFF, data32);
        NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _SOURCETRACK, _CTRL, data32);
    }
    else
    {
        // STO Enable
        data32 = NVSWITCH_NPORT_RD32_LR10(device, idx_nport, _SOURCETRACK, _CTRL);
        data32 = FLD_SET_DRF(_SOURCETRACK, _CTRL, _STO_ENB, _ON, data32);
        NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _SOURCETRACK, _CTRL, data32);

        // STO Timeout value
        timeout = DRF_VAL(_SWITCH_REGKEY, _STO_CONTROL, _TIMEOUT, device->regkeys.sto_control);
        if (timeout != NV_SWITCH_REGKEY_STO_CONTROL_TIMEOUT_DEFAULT)
        {
            NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _SOURCETRACK, _MULTISEC_TIMER0,
                DRF_NUM(_SOURCETRACK, _MULTISEC_TIMER0, _TIMERVAL0, timeout));
        }
        else
        {
            NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _SOURCETRACK, _MULTISEC_TIMER0,
                DRF_DEF(_SOURCETRACK, _MULTISEC_TIMER0, _TIMERVAL0, __PROD));
        }
    }

    //
    // WAR for bug 200606509
    // Disable CAM for entry 0 to prevent false ATO trigger
    //
    data32 = NVSWITCH_NPORT_RD32_LR10(device, idx_nport, _TSTATE, _CREQ_CAM_LOCK);
    data32 = DRF_NUM(_TSTATE, _CREQ_CAM_LOCK, _ON, 0x1);
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _TSTATE, _CREQ_CAM_LOCK, data32);

    //
    // WAR for bug 3115824
    // Clear CONTAIN_AND_DRAIN during init for links in reset.
    // Since SBR does not clear CONTAIN_AND_DRAIN, this will clear the bit
    // when the driver is reloaded after an SBR. If the driver has been reloaded
    // without an SBR, then CONTAIN_AND_DRAIN will be re-triggered.
    //
    NVSWITCH_NPORT_MC_BCAST_WR32_LR10(device, _NPORT, _CONTAIN_AND_DRAIN,
        DRF_DEF(_NPORT, _CONTAIN_AND_DRAIN, _CLEAR, _ENABLE));

    return NVL_SUCCESS;
}

void *
nvswitch_alloc_chipdevice_lr10
(
    nvswitch_device *device
)
{
    void *chip_device;

    chip_device = nvswitch_os_malloc(sizeof(lr10_device));
    if (NULL != chip_device)
    {
        nvswitch_os_memset(chip_device, 0, sizeof(lr10_device));
    }

    device->chip_id = NV_PSMC_BOOT_42_CHIP_ID_LR10;
    return(chip_device);
}

static NvlStatus
nvswitch_initialize_pmgr_lr10
(
    nvswitch_device *device
)
{
    nvswitch_init_pmgr_lr10(device);
    nvswitch_init_pmgr_devices_lr10(device);

    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_initialize_route_lr10
(
    nvswitch_device *device
)
{
    NvlStatus retval;

    retval = _nvswitch_init_ganged_link_routing(device);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to initialize GLT\n",
            __FUNCTION__);
        goto nvswitch_initialize_route_exit;
    }

    _nvswitch_init_cmd_routing(device);

    // Initialize Portstat Counters
    retval = _nvswitch_init_portstat_counters(device);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to initialize portstat counters\n",
            __FUNCTION__);
        goto nvswitch_initialize_route_exit;
    }

nvswitch_initialize_route_exit:
    return retval;
}


NvlStatus
nvswitch_pri_ring_init_lr10
(
    nvswitch_device *device
)
{
    NvU32 i;
    NvU32 value;
    NvBool enumerated = NV_FALSE;
    NvlStatus retval = NVL_SUCCESS;

    //
    // Sometimes on RTL simulation we see the priv ring initialization fail.
    // Retry up to 3 times until this issue is root caused. Bug 1826216.
    //
    for (i = 0; !enumerated && (i < 3); i++)
    {
        value = DRF_DEF(_PPRIV_MASTER, _RING_COMMAND, _CMD, _ENUMERATE_AND_START_RING);
        retval = nvswitch_ring_master_cmd_lr10(device, value);
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: PRIV ring enumeration failed\n",
                __FUNCTION__);
            continue;
        }

        value = NVSWITCH_REG_RD32(device, _PPRIV_MASTER, _RING_START_RESULTS);
        if (!FLD_TEST_DRF(_PPRIV_MASTER, _RING_START_RESULTS, _CONNECTIVITY, _PASS, value))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: PRIV ring connectivity failed\n",
                __FUNCTION__);
            continue;
        }

        value = NVSWITCH_REG_RD32(device, _PPRIV_MASTER, _RING_INTERRUPT_STATUS0);
        if (value)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: NV_PPRIV_MASTER_RING_INTERRUPT_STATUS0 = %x\n",
                __FUNCTION__, value);

            if ((!FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
                    _RING_START_CONN_FAULT, 0, value)) ||
                (!FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
                    _DISCONNECT_FAULT, 0, value))      ||
                (!FLD_TEST_DRF_NUM(_PPRIV_MASTER, _RING_INTERRUPT_STATUS0,
                    _OVERFLOW_FAULT, 0, value)))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: PRIV ring error interrupt\n",
                    __FUNCTION__);
            }

            (void)nvswitch_ring_master_cmd_lr10(device,
                    DRF_DEF(_PPRIV_MASTER, _RING_COMMAND, _CMD, _ACK_INTERRUPT));

            continue;
        }

        enumerated = NV_TRUE;
    }

    if (!enumerated)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Cannot enumerate PRIV ring!\n",
            __FUNCTION__);
        retval = -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    return retval;
}

/*
 * @Brief : Initializes an NvSwitch hardware state
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 *
 * @returns                 NVL_SUCCESS if the action succeeded
 *                          -NVL_BAD_ARGS if bad arguments provided
 *                          -NVL_PCI_ERROR if bar info unable to be retrieved
 */
NvlStatus
nvswitch_initialize_device_state_lr10
(
    nvswitch_device *device
)
{
    NvlStatus retval = NVL_SUCCESS;

    // alloc chip-specific device structure
    device->chip_device = nvswitch_alloc_chipdevice(device);
    if (NULL == device->chip_device)
    {
        NVSWITCH_PRINT(device, ERROR,
            "nvswitch_os_malloc during chip_device creation failed!\n");
        retval = -NVL_NO_MEM;
        goto nvswitch_initialize_device_state_exit;
    }

    retval = nvswitch_check_io_sanity(device);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: IO sanity test failed\n",
            __FUNCTION__);
        goto nvswitch_initialize_device_state_exit;
    }

    NVSWITCH_PRINT(device, SETUP,
        "%s: MMIO discovery\n",
        __FUNCTION__);
    retval = nvswitch_device_discovery(device, NV_SWPTOP_TABLE_BASE_ADDRESS_OFFSET);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Engine discovery failed\n",
            __FUNCTION__);
        goto nvswitch_initialize_device_state_exit;
    }

    nvswitch_filter_discovery(device);

    retval = nvswitch_process_discovery(device);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Discovery processing failed\n",
            __FUNCTION__);
        goto nvswitch_initialize_device_state_exit;
    }

    // now that we have completed discovery, perform initialization steps that
    // depend on engineDescriptors being initialized
    //
    // Temporary location, really needs to be done somewhere common to all flcnables
    if (nvswitch_is_soe_supported(device))
    {
        flcnablePostDiscoveryInit(device, device->pSoe);
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "%s: Skipping SOE post discovery init.\n",
            __FUNCTION__);
    }

    // Make sure interrupts are disabled before we enable interrupts with the OS.
    nvswitch_lib_disable_interrupts(device);

    retval = nvswitch_pri_ring_init(device);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: PRI init failed\n", __FUNCTION__);
        goto nvswitch_initialize_device_state_exit;
    }

    NVSWITCH_PRINT(device, SETUP,
        "%s: Enabled links: 0x%llx\n",
        __FUNCTION__,
        ((NvU64)device->regkeys.link_enable_mask2 << 32 |
        (NvU64)device->regkeys.link_enable_mask) &
        ((~0ULL) >> (64 - NVSWITCH_LINK_COUNT(device))));

    // Detect TNVL mode
    nvswitch_detect_tnvl_mode(device);

    if (nvswitch_is_soe_supported(device))
    {
        retval = nvswitch_init_soe(device);
        if (NVL_SUCCESS != retval)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Init SOE failed\n",
                __FUNCTION__);
            goto nvswitch_initialize_device_state_exit;
        }
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "%s: Skipping SOE init.\n",
            __FUNCTION__);
    }

    // Read ROM configuration
    nvswitch_read_rom_tables(device, &device->firmware);
    _nvswitch_process_firmware_info_lr10(device, &device->firmware);

    // Init PMGR info
    retval = nvswitch_initialize_pmgr(device);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: PMGR init failed\n", __FUNCTION__);
        retval = -NVL_INITIALIZATION_TOTAL_FAILURE;
        goto nvswitch_initialize_device_state_exit;
    }

    retval = nvswitch_init_pll_config(device);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: failed\n", __FUNCTION__);
        retval = -NVL_INITIALIZATION_TOTAL_FAILURE;
        goto nvswitch_initialize_device_state_exit;
    }

    //
    // PLL init should be done *first* before other hardware init
    //
    retval = nvswitch_init_pll(device);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: PLL init failed\n",
            __FUNCTION__);
        goto nvswitch_initialize_device_state_exit;
    }

    //
    // Now that software knows the devices and addresses, it must take all
    // the wrapper modules out of reset.  It does this by writing to the
    // PMC module enable registers.
    //

    // Init IP wrappers
//    _nvswitch_init_mc_enable_lr10(device);
    retval = nvswitch_initialize_ip_wrappers(device);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: init failed\n", __FUNCTION__);
        retval = -NVL_INITIALIZATION_TOTAL_FAILURE;
        goto nvswitch_initialize_device_state_exit;
    }

    nvswitch_init_warm_reset(device);
    nvswitch_init_npg_multicast(device);
    retval = nvswitch_clear_nport_rams(device);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NPORT RAM clear failed\n",
            __FUNCTION__);
        goto nvswitch_initialize_device_state_exit;
    }

    retval = nvswitch_init_nport(device);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Init NPORTs failed\n",
            __FUNCTION__);
        goto nvswitch_initialize_device_state_exit;
    }

    retval = nvswitch_init_nxbar(device);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Init NXBARs failed\n",
            __FUNCTION__);
        goto nvswitch_initialize_device_state_exit;
    }

    if (device->regkeys.minion_disable != NV_SWITCH_REGKEY_MINION_DISABLE_YES)
    {
        NVSWITCH_PRINT(device, WARN, "%s: Entering init minion\n", __FUNCTION__);

        retval = nvswitch_init_minion(device);
        if (NVL_SUCCESS != retval)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Init MINIONs failed\n",
                __FUNCTION__);
            goto nvswitch_initialize_device_state_exit;
        }
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "MINION is disabled via regkey.\n");

        NVSWITCH_PRINT(device, INFO, "%s: Skipping MINION init\n",
            __FUNCTION__);
    }

    _nvswitch_setup_chiplib_forced_config_lr10(device);

    // Init route
    retval = nvswitch_initialize_route(device);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: route init failed\n", __FUNCTION__);
        retval = -NVL_INITIALIZATION_TOTAL_FAILURE;
        goto nvswitch_initialize_device_state_exit;
    }

    nvswitch_init_clock_gating(device);

    // Initialize SPI
    if (nvswitch_is_spi_supported(device))
    {
        retval = nvswitch_spi_init(device);
        if (NVL_SUCCESS != retval)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: SPI init failed!, rc: %d\n",
                __FUNCTION__, retval);
            goto nvswitch_initialize_device_state_exit;
        }
    }
    else
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: Skipping SPI init.\n",
            __FUNCTION__);
    }

    // Initialize SMBPBI
    if (nvswitch_is_smbpbi_supported(device))
    {
        retval = nvswitch_smbpbi_init(device);
        if (NVL_SUCCESS != retval)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: SMBPBI init failed!, rc: %d\n",
                __FUNCTION__, retval);
            goto nvswitch_initialize_device_state_exit;
        }
    }
    else
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: Skipping SMBPBI init.\n",
            __FUNCTION__);
    }

    nvswitch_initialize_interrupt_tree(device);

    // Initialize external thermal sensor
    retval = nvswitch_init_thermal(device);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: External Thermal init failed\n",
            __FUNCTION__);
    }

    return NVL_SUCCESS;

nvswitch_initialize_device_state_exit:
    nvswitch_destroy_device_state(device);

    return retval;
}

/*
 * @Brief : Destroys an NvSwitch hardware state
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 */
void
nvswitch_destroy_device_state_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

    if (nvswitch_is_soe_supported(device))
    {
        nvswitch_soe_unregister_events(device);
    }

    if (chip_device != NULL)
    {
        if ((chip_device->latency_stats) != NULL)
        {
            nvswitch_os_free(chip_device->latency_stats);
        }

        if ((chip_device->ganged_link_table) != NULL)
        {
            nvswitch_os_free(chip_device->ganged_link_table);
        }

        nvswitch_free_chipdevice(device);
    }

    nvswitch_i2c_destroy(device);

    return;
}

static void
_nvswitch_set_nvlink_caps_lr10
(
    NvU32 *pCaps
)
{
    NvU8 tempCaps[NVSWITCH_NVLINK_CAPS_TBL_SIZE];

    nvswitch_os_memset(tempCaps, 0, sizeof(tempCaps));

    NVSWITCH_SET_CAP(tempCaps, NVSWITCH_NVLINK_CAPS, _VALID);
    NVSWITCH_SET_CAP(tempCaps, NVSWITCH_NVLINK_CAPS, _SUPPORTED);
    NVSWITCH_SET_CAP(tempCaps, NVSWITCH_NVLINK_CAPS, _P2P_SUPPORTED);
    NVSWITCH_SET_CAP(tempCaps, NVSWITCH_NVLINK_CAPS, _P2P_ATOMICS);

    // Assume IBM P9 for PPC -- TODO Xavier support.
#if defined(NVCPU_PPC64LE)
    NVSWITCH_SET_CAP(tempCaps, NVSWITCH_NVLINK_CAPS, _SYSMEM_ACCESS);
    NVSWITCH_SET_CAP(tempCaps, NVSWITCH_NVLINK_CAPS, _SYSMEM_ATOMICS);
#endif

    nvswitch_os_memcpy(pCaps, tempCaps, sizeof(tempCaps));
}

/*
 * @brief Determines if a link's lanes are reversed
 *
 * @param[in] device    a reference to the device to query
 * @param[in] linkId    Target link ID
 *
 * @return NV_TRUE if a link's lanes are reversed
 */
NvBool
nvswitch_link_lane_reversed_lr10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    NvU32 regData;
    nvlink_link *link;

    link = nvswitch_get_link(device, linkId);
    if ((link == NULL) || nvswitch_is_link_in_reset(device, link))
    {
        return NV_FALSE;
    }

    regData = NVSWITCH_LINK_RD32_LR10(device, linkId, NVLDL, _NVLDL_RX, _CONFIG_RX);

    // HW may reverse the lane ordering or it may be overridden by SW.
    if (FLD_TEST_DRF(_NVLDL_RX, _CONFIG_RX, _REVERSAL_OVERRIDE, _ON, regData))
    {
        // Overridden
        if (FLD_TEST_DRF(_NVLDL_RX, _CONFIG_RX, _LANE_REVERSE, _ON, regData))
        {
            return NV_TRUE;
        }
        else
        {
            return NV_FALSE;
        }
    }
    else
    {
        // Sensed in HW
        if (FLD_TEST_DRF(_NVLDL_RX, _CONFIG_RX, _HW_LANE_REVERSE, _ON, regData))
        {
            return NV_TRUE;
        }
        else
        {
            return NV_FALSE;
        }
    }

    return NV_FALSE;
}

NvlStatus
nvswitch_ctrl_get_nvlink_status_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_STATUS_PARAMS *ret
)
{
    NvlStatus retval = NVL_SUCCESS;
    nvlink_link *link;
    NvU8 i;
    NvU32 linkState, txSublinkStatus, rxSublinkStatus;
    nvlink_conn_info conn_info = {0};
    NvU64 enabledLinkMask;
    NvU32 nvlink_caps_version;

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    ret->enabledLinkMask = enabledLinkMask;

    FOR_EACH_INDEX_IN_MASK(64, i, enabledLinkMask)
    {
        NVSWITCH_ASSERT(i < NVSWITCH_LINK_COUNT(device));

        link = nvswitch_get_link(device, i);

        if ((link == NULL) ||
            !NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber) ||
            (i >= NVSWITCH_NVLINK_MAX_LINKS))
        {
            continue;
        }

        //
        // Call the core library to get the remote end information. On the first
        // invocation this will also trigger link training, if link-training is
        // not externally managed by FM. Therefore it is necessary that this be
        // before link status on the link is populated since this call will
        // actually change link state.
        //
        if (device->regkeys.external_fabric_mgmt)
        {
            nvlink_lib_get_remote_conn_info(link, &conn_info);
        }
        else
        {
            nvlink_lib_discover_and_get_remote_conn_info(link, &conn_info,
                                                         NVLINK_STATE_CHANGE_SYNC,
                                                         NV_FALSE);
        }

        // Set NVLINK per-link caps
        _nvswitch_set_nvlink_caps_lr10(&ret->linkInfo[i].capsTbl);

        ret->linkInfo[i].phyType = NVSWITCH_NVLINK_STATUS_PHY_NVHS;
        ret->linkInfo[i].subLinkWidth = nvswitch_get_sublink_width(device, link->linkNumber);

        if (!nvswitch_is_link_in_reset(device, link))
        {
            linkState = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _LINK_STATE);
            linkState = DRF_VAL(_NVLDL_TOP, _LINK_STATE, _STATE, linkState);

            txSublinkStatus = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TX, _SLSM_STATUS_TX);
            txSublinkStatus = DRF_VAL(_NVLDL_TX, _SLSM_STATUS_TX, _PRIMARY_STATE, txSublinkStatus);

            rxSublinkStatus = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_RX, _SLSM_STATUS_RX);
            rxSublinkStatus = DRF_VAL(_NVLDL_RX, _SLSM_STATUS_RX, _PRIMARY_STATE, rxSublinkStatus);

            ret->linkInfo[i].bLaneReversal = nvswitch_link_lane_reversed_lr10(device, i);
        }
        else
        {
            linkState       = NVSWITCH_NVLINK_STATUS_LINK_STATE_INIT;
            txSublinkStatus = NVSWITCH_NVLINK_STATUS_SUBLINK_TX_STATE_OFF;
            rxSublinkStatus = NVSWITCH_NVLINK_STATUS_SUBLINK_RX_STATE_OFF;
        }

        ret->linkInfo[i].linkState       = linkState;
        ret->linkInfo[i].txSublinkStatus = txSublinkStatus;
        ret->linkInfo[i].rxSublinkStatus = rxSublinkStatus;

        nvlink_caps_version = nvswitch_get_caps_nvlink_version(device);
        if (nvlink_caps_version == NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_3_0)
        {
            ret->linkInfo[i].nvlinkVersion = NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_3_0;
            ret->linkInfo[i].nciVersion = NVSWITCH_NVLINK_STATUS_NCI_VERSION_3_0;
        }
        else if (nvlink_caps_version == NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_4_0)
        {
            ret->linkInfo[i].nvlinkVersion = NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_4_0;
            ret->linkInfo[i].nciVersion = NVSWITCH_NVLINK_STATUS_NCI_VERSION_4_0;
        }
        else
        {
            NVSWITCH_PRINT(device, WARN,
                "%s WARNING: Unknown NVSWITCH_NVLINK_CAPS_NVLINK_VERSION 0x%x\n",
                __FUNCTION__, nvlink_caps_version);
            ret->linkInfo[i].nvlinkVersion = NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_INVALID;
            ret->linkInfo[i].nciVersion = NVSWITCH_NVLINK_STATUS_NCI_VERSION_INVALID;
        }

        ret->linkInfo[i].phyVersion = NVSWITCH_NVLINK_STATUS_NVHS_VERSION_1_0;

        if (conn_info.bConnected)
        {
            ret->linkInfo[i].connected = NVSWITCH_NVLINK_STATUS_CONNECTED_TRUE;
            ret->linkInfo[i].remoteDeviceLinkNumber = (NvU8)conn_info.linkNumber;

            ret->linkInfo[i].remoteDeviceInfo.domain = conn_info.domain;
            ret->linkInfo[i].remoteDeviceInfo.bus = conn_info.bus;
            ret->linkInfo[i].remoteDeviceInfo.device = conn_info.device;
            ret->linkInfo[i].remoteDeviceInfo.function = conn_info.function;
            ret->linkInfo[i].remoteDeviceInfo.pciDeviceId = conn_info.pciDeviceId;
            ret->linkInfo[i].remoteDeviceInfo.deviceType = conn_info.deviceType;

            ret->linkInfo[i].localLinkSid  = link->localSid;
            ret->linkInfo[i].remoteLinkSid = link->remoteSid;

            if (0 != conn_info.pciDeviceId)
            {
                ret->linkInfo[i].remoteDeviceInfo.deviceIdFlags =
                    FLD_SET_DRF(SWITCH_NVLINK, _DEVICE_INFO, _DEVICE_ID_FLAGS,
                         _PCI, ret->linkInfo[i].remoteDeviceInfo.deviceIdFlags);
            }

            // Does not use loopback
            ret->linkInfo[i].loopProperty =
                NVSWITCH_NVLINK_STATUS_LOOP_PROPERTY_NONE;
        }
        else
        {
            ret->linkInfo[i].connected =
                NVSWITCH_NVLINK_STATUS_CONNECTED_FALSE;
            ret->linkInfo[i].remoteDeviceInfo.deviceType =
                NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_NONE;
        }

        // Set the device information for the local end of the link
        ret->linkInfo[i].localDeviceInfo.domain = device->nvlink_device->pciInfo.domain;
        ret->linkInfo[i].localDeviceInfo.bus = device->nvlink_device->pciInfo.bus;
        ret->linkInfo[i].localDeviceInfo.device = device->nvlink_device->pciInfo.device;
        ret->linkInfo[i].localDeviceInfo.function = device->nvlink_device->pciInfo.function;
        ret->linkInfo[i].localDeviceInfo.pciDeviceId = 0xdeadbeef; // TODO
        ret->linkInfo[i].localDeviceLinkNumber = i;
        ret->linkInfo[i].laneRxdetStatusMask = device->link[i].lane_rxdet_status_mask;
        ret->linkInfo[i].localDeviceInfo.deviceType =
            NVSWITCH_NVLINK_DEVICE_INFO_DEVICE_TYPE_SWITCH;

        // Clock data
        ret->linkInfo[i].nvlinkLineRateMbps = nvswitch_minion_get_line_rate_Mbps_lr10(device, i);
        ret->linkInfo[i].nvlinkLinkDataRateKiBps = nvswitch_minion_get_data_rate_KiBps_lr10(device, i);
        ret->linkInfo[i].nvlinkLinkClockMhz = ret->linkInfo[i].nvlinkLineRateMbps / 32;
        ret->linkInfo[i].nvlinkRefClkSpeedMhz = 156;
        ret->linkInfo[i].nvlinkRefClkType = NVSWITCH_NVLINK_REFCLK_TYPE_NVHS;

    }
    FOR_EACH_INDEX_IN_MASK_END;

//    NVSWITCH_ASSERT(ret->enabledLinkMask == enabledLinkMask);

    return retval;
}

NvlStatus
nvswitch_ctrl_get_counters_lr10
(
    nvswitch_device *device,
    NVSWITCH_NVLINK_GET_COUNTERS_PARAMS *ret
)
{
    nvlink_link *link;
    NvU8   i;
    NvU32  counterMask;
    NvU32  data;
    NvU32  val;
    NvU64  tx0TlCount;
    NvU64  tx1TlCount;
    NvU64  rx0TlCount;
    NvU64  rx1TlCount;
    NvU32  laneId;
    NvBool bLaneReversed;
    NvlStatus status;
    NvBool minion_enabled;

    ct_assert(NVSWITCH_NUM_LANES_LR10 <= NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE__SIZE);

    link = nvswitch_get_link(device, ret->linkId);
    if ((link == NULL) ||
        !NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber))
    {
        return -NVL_BAD_ARGS;
    }

    minion_enabled = nvswitch_is_minion_initialized(device, NVSWITCH_GET_LINK_ENG_INST(device, link->linkNumber, MINION));

    counterMask = ret->counterMask;

    // Common usage allows one of these to stand for all of them
    if (counterMask & (NVSWITCH_NVLINK_COUNTER_TL_TX0 |
                       NVSWITCH_NVLINK_COUNTER_TL_TX1 |
                       NVSWITCH_NVLINK_COUNTER_TL_RX0 |
                       NVSWITCH_NVLINK_COUNTER_TL_RX1))
    {
        tx0TlCount = nvswitch_read_64bit_counter(device,
            NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_LO(0)),
            NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_HI(0)));
        if (NVBIT64(63) & tx0TlCount)
        {
            ret->bTx0TlCounterOverflow = NV_TRUE;
            tx0TlCount &= ~(NVBIT64(63));
        }

        tx1TlCount = nvswitch_read_64bit_counter(device,
            NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_LO(1)),
            NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_HI(1)));
        if (NVBIT64(63) & tx1TlCount)
        {
            ret->bTx1TlCounterOverflow = NV_TRUE;
            tx1TlCount &= ~(NVBIT64(63));
        }

        rx0TlCount = nvswitch_read_64bit_counter(device,
            NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_LO(0)),
            NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_HI(0)));
        if (NVBIT64(63) & rx0TlCount)
        {
            ret->bRx0TlCounterOverflow = NV_TRUE;
            rx0TlCount &= ~(NVBIT64(63));
        }

        rx1TlCount = nvswitch_read_64bit_counter(device,
            NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_LO(1)),
            NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_HI(1)));
        if (NVBIT64(63) & rx1TlCount)
        {
            ret->bRx1TlCounterOverflow = NV_TRUE;
            rx1TlCount &= ~(NVBIT64(63));
        }

        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_TL_TX0)] = tx0TlCount;
        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_TL_TX1)] = tx1TlCount;
        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_TL_RX0)] = rx0TlCount;
        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_TL_RX1)] = rx1TlCount;
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT)
    {
        if (minion_enabled)
        {
            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                                    NV_NVLSTAT_RX01, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
            data = DRF_VAL(_NVLSTAT, _RX01, _FLIT_CRC_ERRORS_VALUE, data);
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT)]
            = data;
    }

    data = 0x0;
    bLaneReversed = nvswitch_link_lane_reversed_lr10(device, link->linkNumber);

    for (laneId = 0; laneId < NVSWITCH_NUM_LANES_LR10; laneId++)
    {
        //
        // HW may reverse the lane ordering or it may be overridden by SW.
        // If so, invert the interpretation of the lane CRC errors.
        //
        i = (NvU8)((bLaneReversed) ? (NVSWITCH_NUM_LANES_LR10 - 1) - laneId : laneId);

        if (minion_enabled)
        {
            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                                    NV_NVLSTAT_DB01, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L(laneId))
        {
            val = BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L(laneId));

            switch (i)
            {
                case 0:
                    ret->nvlinkCounters[val]
                        = DRF_VAL(_NVLSTAT, _DB01, _ERROR_COUNT_ERR_LANECRC_L0, data);
                    break;
                case 1:
                    ret->nvlinkCounters[val]
                        = DRF_VAL(_NVLSTAT, _DB01, _ERROR_COUNT_ERR_LANECRC_L1, data);
                    break;
                case 2:
                    ret->nvlinkCounters[val]
                        = DRF_VAL(_NVLSTAT, _DB01, _ERROR_COUNT_ERR_LANECRC_L2, data);
                    break;
                case 3:
                    ret->nvlinkCounters[val]
                        = DRF_VAL(_NVLSTAT, _DB01, _ERROR_COUNT_ERR_LANECRC_L3, data);
                    break;
            }
        }
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY)
    {
        if (minion_enabled)
        {
            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                                    NV_NVLSTAT_TX09, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
            data = DRF_VAL(_NVLSTAT, _TX09, _REPLAY_EVENTS_VALUE, data);
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY)]
            = data;
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY)
    {
        if (minion_enabled)
        {
            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                                    NV_NVLSTAT_LNK1, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
            data = DRF_VAL(_NVLSTAT, _LNK1, _ERROR_COUNT1_RECOVERY_EVENTS_VALUE, data);
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY)]
            = data;
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_REPLAY)
    {
        if (minion_enabled)
        {
            status = nvswitch_minion_get_dl_status(device, link->linkNumber,
                                    NV_NVLSTAT_RX00, 0, &data);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
            data = DRF_VAL(_NVLSTAT, _RX00, _REPLAY_EVENTS_VALUE, data);
        }
        else
        {
            // MINION disabled
            data = 0;
        }

        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_REPLAY)]
            = data;
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_PASS)
    {
        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_PASS)] = 0;
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_FAIL)
    {
        ret->nvlinkCounters[BIT_IDX_32(NVSWITCH_NVLINK_COUNTER_PHY_REFRESH_FAIL)] = 0;
    }

    return NVL_SUCCESS;
}

static void
nvswitch_ctrl_clear_throughput_counters_lr10
(
    nvswitch_device *device,
    nvlink_link     *link,
    NvU32            counterMask
)
{
    NvU32 data;

    // TX
    data = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL);
    if (counterMask & NVSWITCH_NVLINK_COUNTER_TL_TX0)
    {
        data = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL, _RESETTX0, 0x1, data);
    }
    if (counterMask & NVSWITCH_NVLINK_COUNTER_TL_TX1)
    {
        data = FLD_SET_DRF_NUM(_NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL, _RESETTX1, 0x1, data);
    }
    NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_CTRL, data);

    // RX
    data = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL);
    if (counterMask & NVSWITCH_NVLINK_COUNTER_TL_RX0)
    {
        data = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL, _RESETRX0, 0x1, data);
    }
    if (counterMask & NVSWITCH_NVLINK_COUNTER_TL_RX1)
    {
        data = FLD_SET_DRF_NUM(_NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL, _RESETRX1, 0x1, data);
    }
    NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_CTRL, data);
}

static NvlStatus
nvswitch_ctrl_clear_dl_error_counters_lr10
(
    nvswitch_device *device,
    nvlink_link     *link,
    NvU32            counterMask
)
{
    NvU32           data;

    if ((!counterMask) ||
        (!(counterMask & (NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L0 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L1 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L2 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L3 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L4 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L5 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L6 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L7 |
                          NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_ECC_COUNTS |
                          NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY |
                          NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY))))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Link%d: No error count clear request, counterMask (0x%x). Returning!\n",
            __FUNCTION__, link->linkNumber, counterMask);
        return NVL_SUCCESS;
    }

    // With Minion initialized, send command to minion
    if (nvswitch_is_minion_initialized(device, NVSWITCH_GET_LINK_ENG_INST(device, link->linkNumber, MINION)))
    {
        return nvswitch_minion_clear_dl_error_counters_lr10(device, link->linkNumber);
    }

    // With Minion not-initialized, perform with the registers
    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT)
    {
        data = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_RX, _ERROR_COUNT_CTRL);
        data = FLD_SET_DRF(_NVLDL_RX, _ERROR_COUNT_CTRL, _CLEAR_FLIT_CRC, _CLEAR, data);
        data = FLD_SET_DRF(_NVLDL_RX, _ERROR_COUNT_CTRL, _CLEAR_RATES, _CLEAR, data);
        NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLDL_RX, _ERROR_COUNT_CTRL, data);
    }

    if (counterMask & (NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L0 |
               NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L1 |
               NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L2 |
               NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L3 |
               NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L4 |
               NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L5 |
               NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L6 |
               NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L7 |
               NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_ECC_COUNTS))
    {
        data = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_RX, _ERROR_COUNT_CTRL);
        data = FLD_SET_DRF(_NVLDL_RX, _ERROR_COUNT_CTRL, _CLEAR_LANE_CRC, _CLEAR, data);
        data = FLD_SET_DRF(_NVLDL_RX, _ERROR_COUNT_CTRL, _CLEAR_RATES, _CLEAR, data);
        if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_ECC_COUNTS)
        {
            data = FLD_SET_DRF(_NVLDL_RX, _ERROR_COUNT_CTRL, _CLEAR_ECC_COUNTS, _CLEAR, data);
        }
        NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLDL_RX, _ERROR_COUNT_CTRL, data);
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY)
    {
        data = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TX, _ERROR_COUNT_CTRL);
        data = FLD_SET_DRF(_NVLDL_TX, _ERROR_COUNT_CTRL, _CLEAR_REPLAY, _CLEAR, data);
        NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TX, _ERROR_COUNT_CTRL, data);
    }

    if (counterMask & NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY)
    {
        data = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _ERROR_COUNT_CTRL);
        data = FLD_SET_DRF(_NVLDL_TOP, _ERROR_COUNT_CTRL, _CLEAR_RECOVERY, _CLEAR, data);
        NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _ERROR_COUNT_CTRL, data);
    }
    return NVL_SUCCESS;
}

/*
 * CTRL_NVSWITCH_GET_INFO
 *
 * Query for miscellaneous information analogous to NV2080_CTRL_GPU_INFO
 * This provides a single API to query for multiple pieces of miscellaneous
 * information via a single call.
 *
 */

static NvU32
_nvswitch_get_info_chip_id
(
    nvswitch_device *device
)
{
    NvU32 val = NVSWITCH_REG_RD32(device, _PSMC, _BOOT_42);

    return (DRF_VAL(_PSMC, _BOOT_42, _CHIP_ID, val));
}

static NvU32
_nvswitch_get_info_revision_major
(
    nvswitch_device *device
)
{
    NvU32 val = NVSWITCH_REG_RD32(device, _PSMC, _BOOT_42);

    return (DRF_VAL(_PSMC, _BOOT_42, _MAJOR_REVISION, val));
}

static NvU32
_nvswitch_get_info_revision_minor
(
    nvswitch_device *device
)
{
    NvU32 val = NVSWITCH_REG_RD32(device, _PSMC, _BOOT_42);

    return (DRF_VAL(_PSMC, _BOOT_42, _MINOR_REVISION, val));
}

static NvU32
_nvswitch_get_info_revision_minor_ext
(
    nvswitch_device *device
)
{
    NvU32 val = NVSWITCH_REG_RD32(device, _PSMC, _BOOT_42);

    return (DRF_VAL(_PSMC, _BOOT_42, _MINOR_EXTENDED_REVISION, val));
}

static NvBool
_nvswitch_inforom_bbx_supported
(
    nvswitch_device *device
)
{
    return NV_FALSE;
}

/*
 * CTRL_NVSWITCH_GET_INFO
 *
 * Query for miscellaneous information analogous to NV2080_CTRL_GPU_INFO
 * This provides a single API to query for multiple pieces of miscellaneous
 * information via a single call.
 *
 */

NvlStatus
nvswitch_ctrl_get_info_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_INFO *p
)
{
    NvlStatus retval = NVL_SUCCESS;
    NvU32 i;

    if (p->count > NVSWITCH_GET_INFO_COUNT_MAX)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Invalid args\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(p->info, 0, sizeof(NvU32)*NVSWITCH_GET_INFO_COUNT_MAX);

    for (i = 0; i < p->count; i++)
    {
        switch (p->index[i])
        {
            case NVSWITCH_GET_INFO_INDEX_ARCH:
                p->info[i] = device->chip_arch;
                break;
            case NVSWITCH_GET_INFO_INDEX_PLATFORM:
                if (IS_RTLSIM(device))
                {
                    p->info[i] = NVSWITCH_GET_INFO_INDEX_PLATFORM_RTLSIM;
                }
                else if (IS_FMODEL(device))
                {
                    p->info[i] = NVSWITCH_GET_INFO_INDEX_PLATFORM_FMODEL;
                }
                else if (IS_EMULATION(device))
                {
                    p->info[i] = NVSWITCH_GET_INFO_INDEX_PLATFORM_EMULATION;
                }
                else
                {
                    p->info[i] = NVSWITCH_GET_INFO_INDEX_PLATFORM_SILICON;
                }
                break;
            case NVSWITCH_GET_INFO_INDEX_IMPL:
                p->info[i] = device->chip_impl;
                break;
            case NVSWITCH_GET_INFO_INDEX_CHIPID:
                p->info[i] = _nvswitch_get_info_chip_id(device);
                break;
            case NVSWITCH_GET_INFO_INDEX_REVISION_MAJOR:
                p->info[i] = _nvswitch_get_info_revision_major(device);
                break;
            case NVSWITCH_GET_INFO_INDEX_REVISION_MINOR:
                p->info[i] = _nvswitch_get_info_revision_minor(device);
                break;
            case NVSWITCH_GET_INFO_INDEX_REVISION_MINOR_EXT:
                p->info[i] = _nvswitch_get_info_revision_minor_ext(device);
                break;
            case NVSWITCH_GET_INFO_INDEX_DEVICE_ID:
                p->info[i] = device->nvlink_device->pciInfo.pciDeviceId;
                break;
            case NVSWITCH_GET_INFO_INDEX_NUM_PORTS:
                p->info[i] = NVSWITCH_LINK_COUNT(device);
                break;
            case NVSWITCH_GET_INFO_INDEX_ENABLED_PORTS_MASK_31_0:
                p->info[i] = NvU64_LO32(nvswitch_get_enabled_link_mask(device));
                break;
            case NVSWITCH_GET_INFO_INDEX_ENABLED_PORTS_MASK_63_32:
                p->info[i] = NvU64_HI32(nvswitch_get_enabled_link_mask(device));
                break;
            case NVSWITCH_GET_INFO_INDEX_NUM_VCS:
                p->info[i] = _nvswitch_get_num_vcs_lr10(device);
                break;
            case NVSWITCH_GET_INFO_INDEX_REMAP_POLICY_TABLE_SIZE:
                {
                    NvU32 remap_ram_sel;
                    NvlStatus status;

                    status = nvswitch_get_remap_table_selector(device, NVSWITCH_TABLE_SELECT_REMAP_PRIMARY, &remap_ram_sel);
                    if (status == NVL_SUCCESS)
                    {
                        p->info[i] = nvswitch_get_ingress_ram_size(device, remap_ram_sel);
                    }
                    else
                    {
                        p->info[i] = 0;
                    }
                }
                break;
            case NVSWITCH_GET_INFO_INDEX_REMAP_POLICY_EXTA_TABLE_SIZE:
                {
                    NvU32 remap_ram_sel;
                    NvlStatus status;

                    status = nvswitch_get_remap_table_selector(device, NVSWITCH_TABLE_SELECT_REMAP_EXTA, &remap_ram_sel);
                    if (status == NVL_SUCCESS)
                    {
                        p->info[i] = nvswitch_get_ingress_ram_size(device, remap_ram_sel);
                    }
                    else
                    {
                        p->info[i] = 0;
                    }
                }
                break;
            case NVSWITCH_GET_INFO_INDEX_REMAP_POLICY_EXTB_TABLE_SIZE:
                {
                    NvU32 remap_ram_sel;
                    NvlStatus status;

                    status = nvswitch_get_remap_table_selector(device, NVSWITCH_TABLE_SELECT_REMAP_EXTB, &remap_ram_sel);
                    if (status == NVL_SUCCESS)
                    {
                        p->info[i] = nvswitch_get_ingress_ram_size(device, remap_ram_sel);
                    }
                    else
                    {
                        p->info[i] = 0;
                    }
                }
                break;
            case NVSWITCH_GET_INFO_INDEX_REMAP_POLICY_MULTICAST_TABLE_SIZE:
                {
                    NvU32 remap_ram_sel;
                    NvlStatus status;

                    status = nvswitch_get_remap_table_selector(device, NVSWITCH_TABLE_SELECT_REMAP_MULTICAST, &remap_ram_sel);
                    if (status == NVL_SUCCESS)
                    {
                        p->info[i] = nvswitch_get_ingress_ram_size(device, remap_ram_sel);
                    }
                    else
                    {
                        p->info[i] = 0;
                    }
                }
                break;
            case NVSWITCH_GET_INFO_INDEX_ROUTING_ID_TABLE_SIZE:
                p->info[i] = nvswitch_get_ingress_ram_size(device, NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRIDROUTERAM);
                break;
            case NVSWITCH_GET_INFO_INDEX_ROUTING_LAN_TABLE_SIZE:
                p->info[i] = nvswitch_get_ingress_ram_size(device, NV_INGRESS_REQRSPMAPADDR_RAM_SEL_SELECTSRLANROUTERAM);
                break;
            case NVSWITCH_GET_INFO_INDEX_FREQ_KHZ:
                p->info[i] = device->switch_pll.freq_khz;
                break;
            case NVSWITCH_GET_INFO_INDEX_VCOFREQ_KHZ:
                p->info[i] = device->switch_pll.vco_freq_khz;
                break;
            case NVSWITCH_GET_INFO_INDEX_VOLTAGE_MVOLT:
                retval = -NVL_ERR_NOT_SUPPORTED;
                break;
            case NVSWITCH_GET_INFO_INDEX_PHYSICAL_ID:
                p->info[i] = nvswitch_read_physical_id(device);
                break;
            case NVSWITCH_GET_INFO_INDEX_PCI_DOMAIN:
                p->info[i] = device->nvlink_device->pciInfo.domain;
                break;
            case NVSWITCH_GET_INFO_INDEX_PCI_BUS:
                p->info[i] = device->nvlink_device->pciInfo.bus;
                break;
            case NVSWITCH_GET_INFO_INDEX_PCI_DEVICE:
                p->info[i] = device->nvlink_device->pciInfo.device;
                break;
            case NVSWITCH_GET_INFO_INDEX_PCI_FUNCTION:
                p->info[i] = device->nvlink_device->pciInfo.function;
                break;
            default:
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Undefined NVSWITCH_GET_INFO_INDEX 0x%x\n",
                    __FUNCTION__,
                    p->index[i]);
                retval = -NVL_BAD_ARGS;
                break;
        }
    }

    return retval;
}

NvlStatus
nvswitch_set_nport_port_config_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_SWITCH_PORT_CONFIG *p
)
{
    NvU32   val;

    if (p->requesterLinkID > DRF_MASK(NV_NPORT_REQLINKID_REQROUTINGID))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Invalid requester RID 0x%x\n",
            __FUNCTION__, p->requesterLinkID);
        return -NVL_BAD_ARGS;
    }

    if (p->requesterLanID > DRF_MASK(NV_NPORT_REQLINKID_REQROUTINGLAN))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Invalid requester RLAN 0x%x\n",
            __FUNCTION__, p->requesterLanID);
        return -NVL_BAD_ARGS;
    }

    val = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _NPORT, _CTRL);
    switch (p->type)
    {
        case CONNECT_ACCESS_GPU:
        case CONNECT_ACCESS_CPU:
        case CONNECT_ACCESS_SWITCH:
            val = FLD_SET_DRF(_NPORT, _CTRL, _TRUNKLINKENB, _ACCESSLINK, val);
            break;
        case CONNECT_TRUNK_SWITCH:
            val = FLD_SET_DRF(_NPORT, _CTRL, _TRUNKLINKENB, _TRUNKLINK, val);
            break;
        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s: invalid type #%d\n",
                __FUNCTION__, p->type);
            return -NVL_BAD_ARGS;
    }

    switch(p->count)
    {
        case CONNECT_COUNT_512:
            val = FLD_SET_DRF(_NPORT, _CTRL, _ENDPOINT_COUNT, _512, val);
            break;
        case CONNECT_COUNT_1024:
            val = FLD_SET_DRF(_NPORT, _CTRL, _ENDPOINT_COUNT, _1024, val);
            break;
        case CONNECT_COUNT_2048:
            val = FLD_SET_DRF(_NPORT, _CTRL, _ENDPOINT_COUNT, _2048, val);
            break;
        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s: invalid count #%d\n",
                __FUNCTION__, p->count);
            return -NVL_BAD_ARGS;
    }
    NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _NPORT, _CTRL, val);

    NVSWITCH_LINK_WR32(device, p->portNum, NPORT, _NPORT, _REQLINKID,
        DRF_NUM(_NPORT, _REQLINKID, _REQROUTINGID, p->requesterLinkID) |
        DRF_NUM(_NPORT, _REQLINKID, _REQROUTINGLAN, p->requesterLanID));

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_set_switch_port_config_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_SWITCH_PORT_CONFIG *p
)
{
    nvlink_link *link;
    NvU32 val;
    NvlStatus status;

    if (!NVSWITCH_IS_LINK_ENG_VALID(device, p->portNum, NPORT))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: invalid link #%d\n",
            __FUNCTION__, p->portNum);
        return -NVL_BAD_ARGS;
    }

    if (p->enableVC1 && (p->type != CONNECT_TRUNK_SWITCH))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: VC1 only allowed on trunk links\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    // Validate chip-specific NPORT settings and program port config settings.
    status = nvswitch_set_nport_port_config(device, p);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    link = nvswitch_get_link(device, (NvU8)p->portNum);
    if (link == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: invalid link\n",
            __FUNCTION__);
        return -NVL_ERR_INVALID_STATE;
    }

    //
    // If ac_coupled_mask is configured during nvswitch_create_link,
    // give preference to it.
    //
    if (device->regkeys.ac_coupled_mask  ||
        device->regkeys.ac_coupled_mask2 ||
        device->firmware.nvlink.link_ac_coupled_mask)
    {
        if (link->ac_coupled != p->acCoupled)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: port[%d]: Unsupported AC coupled change (%s)\n",
                __FUNCTION__, p->portNum, p->acCoupled ? "AC" : "DC");
            return -NVL_BAD_ARGS;
        }
    }

    link->ac_coupled = p->acCoupled;

    // AC vs DC mode SYSTEM register
    if (link->ac_coupled)
    {
        //
        // In NVL3.0, ACMODE is handled by MINION in the INITPHASE1 command
        // Here we just setup the register with the proper info
        //
        val = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLIPT_LNK,
                _NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL);
        val = FLD_SET_DRF(_NVLIPT_LNK,
                _CTRL_SYSTEM_LINK_CHANNEL_CTRL, _AC_DC_MODE, _AC, val);
        NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLIPT_LNK,
                _NVLIPT_LNK, _CTRL_SYSTEM_LINK_CHANNEL_CTRL, val);
    }

    // If _BUFFER_RDY is asserted, credits are locked.
    val = NVSWITCH_LINK_RD32_LR10(device, p->portNum, NPORT, _NPORT, _CTRL_BUFFER_READY);
    if (FLD_TEST_DRF(_NPORT, _CTRL_BUFFER_READY, _BUFFERRDY, _ENABLE, val))
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: port[%d]: BUFFERRDY already enabled.\n",
            __FUNCTION__, p->portNum);
        return NVL_SUCCESS;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_set_ingress_request_table_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_INGRESS_REQUEST_TABLE *p
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_ctrl_get_ingress_request_table_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_INGRESS_REQUEST_TABLE_PARAMS *params
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_ctrl_set_ingress_request_valid_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_INGRESS_REQUEST_VALID *p
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_ctrl_get_ingress_response_table_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_INGRESS_RESPONSE_TABLE_PARAMS *params
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}


NvlStatus
nvswitch_ctrl_set_ingress_response_table_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_INGRESS_RESPONSE_TABLE *p
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

static NvlStatus
nvswitch_ctrl_set_ganged_link_table_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_GANGED_LINK_TABLE *p
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

static NvlStatus
nvswitch_ctrl_get_internal_latency_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_INTERNAL_LATENCY *pLatency
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 vc_selector = pLatency->vc_selector;
    NvU32 idx_nport;

    // Validate VC selector
    if (vc_selector >= NVSWITCH_NUM_VCS_LR10)
    {
        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(pLatency, 0, sizeof(*pLatency));
    pLatency->vc_selector = vc_selector;

    for (idx_nport=0; idx_nport < NVSWITCH_LINK_COUNT(device); idx_nport++)
    {
        if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, idx_nport))
        {
            continue;
        }

        pLatency->egressHistogram[idx_nport].low =
            chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].low;
        pLatency->egressHistogram[idx_nport].medium =
            chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].medium;
        pLatency->egressHistogram[idx_nport].high =
           chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].high;
        pLatency->egressHistogram[idx_nport].panic =
           chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].panic;
        pLatency->egressHistogram[idx_nport].count =
           chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].count;
    }

    pLatency->elapsed_time_msec =
      (chip_device->latency_stats->latency[vc_selector].last_read_time_nsec -
       chip_device->latency_stats->latency[vc_selector].start_time_nsec)/1000000ULL;

    chip_device->latency_stats->latency[vc_selector].start_time_nsec =
        chip_device->latency_stats->latency[vc_selector].last_read_time_nsec;

    chip_device->latency_stats->latency[vc_selector].count = 0;

    // Clear accum_latency[]
    for (idx_nport = 0; idx_nport < NVSWITCH_LINK_COUNT(device); idx_nport++)
    {
        chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].low = 0;
        chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].medium = 0;
        chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].high = 0;
        chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].panic = 0;
        chip_device->latency_stats->latency[vc_selector].accum_latency[idx_nport].count = 0;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_set_latency_bins_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_LATENCY_BINS *pLatency
)
{
    NvU32 vc_selector;
    const NvU32 freq_mhz = 1330;
    const NvU32 switchpll_hz = freq_mhz * 1000000ULL; // TODO: Update this with device->switch_pll.freq_khz after LR10 PLL update
    const NvU32 min_threshold = 10;   // Must be > zero to avoid div by zero
    const NvU32 max_threshold = 10000;

    // Quick input validation and ns to register value conversion
    for (vc_selector = 0; vc_selector < NVSWITCH_NUM_VCS_LR10; vc_selector++)
    {
        if ((pLatency->bin[vc_selector].lowThreshold > max_threshold)                           ||
            (pLatency->bin[vc_selector].lowThreshold < min_threshold)                           ||
            (pLatency->bin[vc_selector].medThreshold > max_threshold)                           ||
            (pLatency->bin[vc_selector].medThreshold < min_threshold)                           ||
            (pLatency->bin[vc_selector].hiThreshold  > max_threshold)                           ||
            (pLatency->bin[vc_selector].hiThreshold  < min_threshold)                           ||
            (pLatency->bin[vc_selector].lowThreshold > pLatency->bin[vc_selector].medThreshold) ||
            (pLatency->bin[vc_selector].medThreshold > pLatency->bin[vc_selector].hiThreshold))
        {
            return -NVL_BAD_ARGS;
        }

        pLatency->bin[vc_selector].lowThreshold =
            switchpll_hz / (1000000000 / pLatency->bin[vc_selector].lowThreshold);
        pLatency->bin[vc_selector].medThreshold =
            switchpll_hz / (1000000000 / pLatency->bin[vc_selector].medThreshold);
        pLatency->bin[vc_selector].hiThreshold =
            switchpll_hz / (1000000000 / pLatency->bin[vc_selector].hiThreshold);

        NVSWITCH_PORTSTAT_BCAST_WR32_LR10(device, _LIMIT, _LOW,    vc_selector, pLatency->bin[vc_selector].lowThreshold);
        NVSWITCH_PORTSTAT_BCAST_WR32_LR10(device, _LIMIT, _MEDIUM, vc_selector, pLatency->bin[vc_selector].medThreshold);
        NVSWITCH_PORTSTAT_BCAST_WR32_LR10(device, _LIMIT, _HIGH,   vc_selector, pLatency->bin[vc_selector].hiThreshold);
    }

    return NVL_SUCCESS;
}

#define NV_NPORT_REQLINKID_REQROUTINGLAN_1024  18:18
#define NV_NPORT_REQLINKID_REQROUTINGLAN_2048  18:17

/*
 * @brief Returns the ingress requester link id.
 *
 * On LR10, REQROUTINGID only gives the endpoint but not the specific port of the response packet.
 * To identify the specific port, the routing_ID must be appended with the upper bits of REQROUTINGLAN.
 *
 * When NV_NPORT_CTRL_ENDPOINT_COUNT = 1024, the upper bit of NV_NPORT_REQLINKID_REQROUTINGLAN become REQROUTINGID[9].
 * When NV_NPORT_CTRL_ENDPOINT_COUNT = 2048, the upper two bits of NV_NPORT_REQLINKID_REQROUTINGLAN become REQROUTINGID[10:9].
 *
 * @param[in] device            nvswitch device
 * @param[in] params            NVSWITCH_GET_INGRESS_REQLINKID_PARAMS
 *
 * @returns                     NVL_SUCCESS if action succeeded,
 *                              -NVL_ERR_INVALID_STATE invalid link
 */
NvlStatus
nvswitch_ctrl_get_ingress_reqlinkid_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_INGRESS_REQLINKID_PARAMS *params
)
{
    NvU32 regval;
    NvU32 reqRid;
    NvU32 reqRlan;
    NvU32 rlan_shift = DRF_SHIFT_RT(NV_NPORT_REQLINKID_REQROUTINGID) + 1;

    if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, params->portNum))
    {
        return -NVL_BAD_ARGS;
    }

    regval = NVSWITCH_NPORT_RD32_LR10(device, params->portNum, _NPORT, _REQLINKID);
    reqRid = DRF_VAL(_NPORT, _REQLINKID, _REQROUTINGID, regval);
    reqRlan = regval;

    regval = NVSWITCH_NPORT_RD32_LR10(device, params->portNum, _NPORT, _CTRL);
    if (FLD_TEST_DRF(_NPORT, _CTRL, _ENDPOINT_COUNT, _1024, regval))
    {
        reqRlan = DRF_VAL(_NPORT, _REQLINKID, _REQROUTINGLAN_1024, reqRlan);
        params->requesterLinkID = (reqRid | (reqRlan << rlan_shift));
    }
    else if (FLD_TEST_DRF(_NPORT, _CTRL, _ENDPOINT_COUNT, _2048, regval))
    {
        reqRlan = DRF_VAL(_NPORT, _REQLINKID, _REQROUTINGLAN_2048, reqRlan);
        params->requesterLinkID = (reqRid | (reqRlan << rlan_shift));
    }
    else
    {
        params->requesterLinkID = reqRid;
    }

    return NVL_SUCCESS;
}

/*
 * REGISTER_READ/_WRITE
 * Provides direct access to the MMIO space for trusted clients like MODS.
 * This API should not be exposed to unsecure clients.
 */

/*
 * _nvswitch_get_engine_base
 * Used by REGISTER_READ/WRITE API.  Looks up an engine based on device/instance
 * and returns the base address in BAR0.
 *
 * register_rw_engine   [in] REGISTER_RW_ENGINE_*
 * instance             [in] physical instance of device
 * bcast                [in] FALSE: find unicast base address
 *                           TRUE:  find broadcast base address
 * base_addr            [out] base address in BAR0 of requested device
 *
 * Returns              NVL_SUCCESS: Device base address successfully found
 *                      else device lookup failed
 */

static NvlStatus
_nvswitch_get_engine_base_lr10
(
    nvswitch_device *device,
    NvU32   register_rw_engine,     // REGISTER_RW_ENGINE_*
    NvU32   instance,               // device instance
    NvBool  bcast,
    NvU32   *base_addr
)
{
    NvU32 base = 0;
    ENGINE_DESCRIPTOR_TYPE_LR10  *engine = NULL;
    NvlStatus retval = NVL_SUCCESS;
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

    // Find the engine descriptor matching the request
    engine = NULL;

    switch (register_rw_engine)
    {
        case REGISTER_RW_ENGINE_RAW:
            // Special case raw IO
            if ((instance != 0) ||
                (bcast != NV_FALSE))
            {
                retval = -NVL_BAD_ARGS;
            }
        break;

        case REGISTER_RW_ENGINE_CLKS:
        case REGISTER_RW_ENGINE_FUSE:
        case REGISTER_RW_ENGINE_JTAG:
        case REGISTER_RW_ENGINE_PMGR:
        case REGISTER_RW_ENGINE_XP3G:
            //
            // Legacy devices are always single-instance, unicast-only.
            // These manuals are BAR0 offset-based, not IP-based.  Treat them
            // the same as RAW.
            //
            if ((instance != 0) ||
                (bcast != NV_FALSE))
            {
                retval = -NVL_BAD_ARGS;
            }
            register_rw_engine = REGISTER_RW_ENGINE_RAW;
        break;

        case REGISTER_RW_ENGINE_SAW:
            if (bcast)
            {
                retval = -NVL_BAD_ARGS;
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, SAW, instance))
                {
                    engine = &chip_device->engSAW[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_XVE:
            if (bcast)
            {
                retval = -NVL_BAD_ARGS;
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, XVE, instance))
                {
                    engine = &chip_device->engXVE[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_SOE:
            if (bcast)
            {
                retval = -NVL_BAD_ARGS;
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, SOE, instance))
                {
                    engine = &chip_device->engSOE[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_SE:
            if (bcast)
            {
                retval = -NVL_BAD_ARGS;
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, SE, instance))
                {
                    engine = &chip_device->engSE[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLW:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLW_BCAST, instance))
                {
                    engine = &chip_device->engNVLW_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLW, instance))
                {
                    engine = &chip_device->engNVLW[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_MINION:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, MINION_BCAST, instance))
                {
                    engine = &chip_device->engMINION_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, MINION, instance))
                {
                    engine = &chip_device->engMINION[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLIPT:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLIPT_BCAST, instance))
                {
                    engine = &chip_device->engNVLIPT_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLIPT, instance))
                {
                    engine = &chip_device->engNVLIPT[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLTLC:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLTLC_BCAST, instance))
                {
                    engine = &chip_device->engNVLTLC_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLTLC, instance))
                {
                    engine = &chip_device->engNVLTLC[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLTLC_MULTICAST:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLTLC_MULTICAST_BCAST, instance))
                {
                    engine = &chip_device->engNVLTLC_MULTICAST_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLTLC_MULTICAST, instance))
                {
                    engine = &chip_device->engNVLTLC_MULTICAST[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NPG:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NPG_BCAST, instance))
                {
                    engine = &chip_device->engNPG_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NPG, instance))
                {
                    engine = &chip_device->engNPG[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NPORT:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NPORT_BCAST, instance))
                {
                    engine = &chip_device->engNPORT_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NPORT, instance))
                {
                    engine = &chip_device->engNPORT[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NPORT_MULTICAST:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NPORT_MULTICAST_BCAST, instance))
                {
                    engine = &chip_device->engNPORT_MULTICAST_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NPORT_MULTICAST, instance))
                {
                    engine = &chip_device->engNPORT_MULTICAST[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLIPT_LNK:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLIPT_LNK_BCAST, instance))
                {
                    engine = &chip_device->engNVLIPT_LNK_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLIPT_LNK, instance))
                {
                    engine = &chip_device->engNVLIPT_LNK[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLIPT_LNK_MULTICAST:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLIPT_LNK_MULTICAST_BCAST, instance))
                {
                    engine = &chip_device->engNVLIPT_LNK_MULTICAST_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLIPT_LNK_MULTICAST, instance))
                {
                    engine = &chip_device->engNVLIPT_LNK_MULTICAST[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_PLL:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, PLL_BCAST, instance))
                {
                    engine = &chip_device->engPLL_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, PLL, instance))
                {
                    engine = &chip_device->engPLL[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLDL:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLDL_BCAST, instance))
                {
                    engine = &chip_device->engNVLDL_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLDL, instance))
                {
                    engine = &chip_device->engNVLDL[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NVLDL_MULTICAST:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLDL_MULTICAST_BCAST, instance))
                {
                    engine = &chip_device->engNVLDL_MULTICAST_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NVLDL_MULTICAST, instance))
                {
                    engine = &chip_device->engNVLDL_MULTICAST[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_NXBAR:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NXBAR_BCAST, instance))
                {
                    engine = &chip_device->engNXBAR_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, NXBAR, instance))
                {
                    engine = &chip_device->engNXBAR[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_TILE:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, TILE_BCAST, instance))
                {
                    engine = &chip_device->engTILE_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, TILE, instance))
                {
                    engine = &chip_device->engTILE[instance];
                }
            }
        break;

        case REGISTER_RW_ENGINE_TILE_MULTICAST:
            if (bcast)
            {
                if (NVSWITCH_ENG_VALID_LR10(device, TILE_MULTICAST_BCAST, instance))
                {
                    engine = &chip_device->engTILE_MULTICAST_BCAST[instance];
                }
            }
            else
            {
                if (NVSWITCH_ENG_VALID_LR10(device, TILE_MULTICAST, instance))
                {
                    engine = &chip_device->engTILE_MULTICAST[instance];
                }
            }
        break;

        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s: unknown REGISTER_RW_ENGINE 0x%x\n",
                __FUNCTION__,
                register_rw_engine);
            engine = NULL;
        break;
    }

    if (register_rw_engine == REGISTER_RW_ENGINE_RAW)
    {
        // Raw IO -- client provides full BAR0 offset
        base = 0;
    }
    else
    {
        // Check engine descriptor was found and valid
        if (engine == NULL)
        {
            retval = -NVL_BAD_ARGS;
            NVSWITCH_PRINT(device, ERROR,
                "%s: invalid REGISTER_RW_ENGINE/instance 0x%x(%d)\n",
                __FUNCTION__,
                register_rw_engine,
                instance);
        }
        else if (!engine->valid)
        {
            retval = -NVL_UNBOUND_DEVICE;
            NVSWITCH_PRINT(device, ERROR,
                "%s: REGISTER_RW_ENGINE/instance 0x%x(%d) disabled or invalid\n",
                __FUNCTION__,
                register_rw_engine,
                instance);
        }
        else
        {
            if (bcast && (engine->disc_type == DISCOVERY_TYPE_BROADCAST))
            {
                //
                // Caveat emptor: A read of a broadcast register is
                // implementation-specific.
                //
                base = engine->info.bc.bc_addr;
            }
            else if ((!bcast) && (engine->disc_type == DISCOVERY_TYPE_UNICAST))
            {
                base = engine->info.uc.uc_addr;
            }

            if (base == 0)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: REGISTER_RW_ENGINE/instance 0x%x(%d) has %s base address 0!\n",
                    __FUNCTION__,
                    register_rw_engine,
                    instance,
                    (bcast ? "BCAST" : "UNICAST" ));
                retval = -NVL_IO_ERROR;
            }
        }
    }

    *base_addr = base;
    return retval;
}

/*
 * CTRL_NVSWITCH_REGISTER_READ
 *
 * This provides direct access to the MMIO space for trusted clients like
 * MODS.
 * This API should not be exposed to unsecure clients.
 */

static NvlStatus
nvswitch_ctrl_register_read_lr10
(
    nvswitch_device *device,
    NVSWITCH_REGISTER_READ *p
)
{
    NvU32 base;
    NvU32 data;
    NvlStatus retval = NVL_SUCCESS;

    retval = _nvswitch_get_engine_base_lr10(device, p->engine, p->instance, NV_FALSE, &base);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // Make sure target offset isn't out-of-range
    if ((base + p->offset) >= device->nvlink_device->pciInfo.bars[0].barSize)
    {
        return -NVL_IO_ERROR;
    }

    //
    // Some legacy device manuals are not 0-based (IP style).
    //
    data = NVSWITCH_OFF_RD32(device, base + p->offset);
    p->val = data;

    return NVL_SUCCESS;
}

/*
 * CTRL_NVSWITCH_REGISTER_WRITE
 *
 * This provides direct access to the MMIO space for trusted clients like
 * MODS.
 * This API should not be exposed to unsecure clients.
 */

static NvlStatus
nvswitch_ctrl_register_write_lr10
(
    nvswitch_device *device,
    NVSWITCH_REGISTER_WRITE *p
)
{
    NvU32 base;
    NvlStatus retval = NVL_SUCCESS;

    retval = _nvswitch_get_engine_base_lr10(device, p->engine, p->instance, p->bcast, &base);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // Make sure target offset isn't out-of-range
    if ((base + p->offset) >= device->nvlink_device->pciInfo.bars[0].barSize)
    {
        return -NVL_IO_ERROR;
    }

    //
    // Some legacy device manuals are not 0-based (IP style).
    //
    NVSWITCH_OFF_WR32(device, base + p->offset, p->val);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_get_bios_info_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_BIOS_INFO_PARAMS *p
)
{
    NvU32 biosVersionBytes;
    NvU32 biosOemVersionBytes;
    NvU32 biosMagic = 0x9210;

    //
    // Example: 92.10.09.00.00 is the formatted version string
    //          |         |  |
    //          |         |  |__ BIOS OEM version byte
    //          |         |
    //          |_________|_____ BIOS version bytes
    //
    biosVersionBytes = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_SW, _SCRATCH_6);
    biosOemVersionBytes = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_SW, _SCRATCH_7);

    //
    // LR10 is built out of core92 and the BIOS version will always begin with
    // 92.10.xx.xx.xx
    //
    if ((biosVersionBytes >> 16) != biosMagic)
    {
        NVSWITCH_PRINT(device, ERROR,
                "BIOS version not found in scratch register\n");
        return -NVL_ERR_INVALID_STATE;
    }

    p->version = (((NvU64)biosVersionBytes) << 8) | (biosOemVersionBytes & 0xff);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_get_inforom_version_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_INFOROM_VERSION_PARAMS *p
)
{

    struct inforom *pInforom = device->pInforom;

    if ((pInforom == NULL) || (!pInforom->IMG.bValid))
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (NV_ARRAY_ELEMENTS(pInforom->IMG.object.version) <
        NVSWITCH_INFOROM_VERSION_LEN)
    {
        NVSWITCH_PRINT(device, ERROR,
                       "Inforom IMG object struct smaller than expected\n");
        return -NVL_ERR_INVALID_STATE;
    }

    nvswitch_inforom_string_copy(pInforom->IMG.object.version, p->version,
                                 NVSWITCH_INFOROM_VERSION_LEN);

    return NVL_SUCCESS;
}

void
nvswitch_corelib_clear_link_state_lr10
(
    nvlink_link *link
)
{
    // Receiver Detect needs to happen again
    link->bRxDetected = NV_FALSE;

    // INITNEGOTIATE needs to happen again
    link->bInitnegotiateConfigGood = NV_FALSE;

    // TxCommonMode needs to happen again
    link->bTxCommonModeFail = NV_FALSE;

    // SAFE transition needs to happen again
    link->bSafeTransitionFail = NV_FALSE;

    // Reset the SW state tracking the link and sublink states
    link->state            = NVLINK_LINKSTATE_OFF;
    link->tx_sublink_state = NVLINK_SUBLINK_STATE_TX_OFF;
    link->rx_sublink_state = NVLINK_SUBLINK_STATE_RX_OFF;
}

const static NvU32 nport_reg_addr[] =
{
    NV_NPORT_CTRL,
    NV_NPORT_CTRL_SLCG,
    NV_NPORT_REQLINKID,
    NV_NPORT_PORTSTAT_CONTROL,
    NV_NPORT_PORTSTAT_SNAP_CONTROL,
    NV_NPORT_PORTSTAT_WINDOW_LIMIT,
    NV_NPORT_PORTSTAT_LIMIT_LOW_0,
    NV_NPORT_PORTSTAT_LIMIT_MEDIUM_0,
    NV_NPORT_PORTSTAT_LIMIT_HIGH_0,
    NV_NPORT_PORTSTAT_LIMIT_LOW_1,
    NV_NPORT_PORTSTAT_LIMIT_MEDIUM_1,
    NV_NPORT_PORTSTAT_LIMIT_HIGH_1,
    NV_NPORT_PORTSTAT_LIMIT_LOW_2,
    NV_NPORT_PORTSTAT_LIMIT_MEDIUM_2,
    NV_NPORT_PORTSTAT_LIMIT_HIGH_2,
    NV_NPORT_PORTSTAT_LIMIT_LOW_3,
    NV_NPORT_PORTSTAT_LIMIT_MEDIUM_3,
    NV_NPORT_PORTSTAT_LIMIT_HIGH_3,
    NV_NPORT_PORTSTAT_LIMIT_LOW_4,
    NV_NPORT_PORTSTAT_LIMIT_MEDIUM_4,
    NV_NPORT_PORTSTAT_LIMIT_HIGH_4,
    NV_NPORT_PORTSTAT_LIMIT_LOW_5,
    NV_NPORT_PORTSTAT_LIMIT_MEDIUM_5,
    NV_NPORT_PORTSTAT_LIMIT_HIGH_5,
    NV_NPORT_PORTSTAT_LIMIT_LOW_6,
    NV_NPORT_PORTSTAT_LIMIT_MEDIUM_6,
    NV_NPORT_PORTSTAT_LIMIT_HIGH_6,
    NV_NPORT_PORTSTAT_LIMIT_LOW_7,
    NV_NPORT_PORTSTAT_LIMIT_MEDIUM_7,
    NV_NPORT_PORTSTAT_LIMIT_HIGH_7,
    NV_NPORT_PORTSTAT_SOURCE_FILTER_0,
    NV_NPORT_PORTSTAT_SOURCE_FILTER_1,
    NV_ROUTE_ROUTE_CONTROL,
    NV_ROUTE_CMD_ROUTE_TABLE0,
    NV_ROUTE_CMD_ROUTE_TABLE1,
    NV_ROUTE_CMD_ROUTE_TABLE2,
    NV_ROUTE_CMD_ROUTE_TABLE3,
    NV_ROUTE_ERR_LOG_EN_0,
    NV_ROUTE_ERR_CONTAIN_EN_0,
    NV_ROUTE_ERR_ECC_CTRL,
    NV_ROUTE_ERR_GLT_ECC_ERROR_COUNTER_LIMIT,
    NV_ROUTE_ERR_NVS_ECC_ERROR_COUNTER_LIMIT,
    NV_INGRESS_ERR_LOG_EN_0,
    NV_INGRESS_ERR_CONTAIN_EN_0,
    NV_INGRESS_ERR_ECC_CTRL,
    NV_INGRESS_ERR_REMAPTAB_ECC_ERROR_COUNTER_LIMIT,
    NV_INGRESS_ERR_RIDTAB_ECC_ERROR_COUNTER_LIMIT,
    NV_INGRESS_ERR_RLANTAB_ECC_ERROR_COUNTER_LIMIT,
    NV_INGRESS_ERR_NCISOC_HDR_ECC_ERROR_COUNTER_LIMIT,
    NV_EGRESS_CTRL,
    NV_EGRESS_CTO_TIMER_LIMIT,
    NV_EGRESS_ERR_LOG_EN_0,
    NV_EGRESS_ERR_CONTAIN_EN_0,
    NV_EGRESS_ERR_ECC_CTRL,
    NV_EGRESS_ERR_NXBAR_ECC_ERROR_COUNTER_LIMIT,
    NV_EGRESS_ERR_RAM_OUT_ECC_ERROR_COUNTER_LIMIT,
    NV_TSTATE_TAGSTATECONTROL,
    NV_TSTATE_ATO_TIMER_LIMIT,
    NV_TSTATE_CREQ_CAM_LOCK,
    NV_TSTATE_ERR_LOG_EN_0,
    NV_TSTATE_ERR_CONTAIN_EN_0,
    NV_TSTATE_ERR_ECC_CTRL,
    NV_TSTATE_ERR_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT,
    NV_TSTATE_ERR_TAGPOOL_ECC_ERROR_COUNTER_LIMIT,
    NV_TSTATE_ERR_TD_TID_RAM_ECC_ERROR_COUNTER_LIMIT,
    NV_SOURCETRACK_CTRL,
    NV_SOURCETRACK_MULTISEC_TIMER0,
    NV_SOURCETRACK_ERR_LOG_EN_0,
    NV_SOURCETRACK_ERR_CONTAIN_EN_0,
    NV_SOURCETRACK_ERR_ECC_CTRL,
    NV_SOURCETRACK_ERR_CREQ_TCEN0_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT,
    NV_SOURCETRACK_ERR_CREQ_TCEN0_TD_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT,
    NV_SOURCETRACK_ERR_CREQ_TCEN1_CRUMBSTORE_ECC_ERROR_COUNTER_LIMIT,
};

/*
 *  Disable interrupts comming from NPG & NVLW blocks.
 */
static void
_nvswitch_link_disable_interrupts_lr10
(
    nvswitch_device *device,
    NvU32 link
)
{
    NvU32 i;

    NVSWITCH_NPORT_WR32_LR10(device, link, _NPORT, _ERR_CONTROL_COMMON_NPORT,
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _CORRECTABLEENABLE, 0x0) |
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _FATALENABLE, 0x0) |
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _NONFATALENABLE, 0x0));

    for (i = 0; i < NV_NVLCTRL_LINK_INTR_0_STATUS__SIZE_1; i++)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLW, _NVLCTRL, _LINK_INTR_0_MASK(i),
            DRF_NUM(_NVLCTRL, _LINK_INTR_0_MASK, _FATAL, 0x0) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_0_MASK, _NONFATAL, 0x0) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_0_MASK, _CORRECTABLE, 0x0));

        NVSWITCH_LINK_WR32_LR10(device, link, NVLW, _NVLCTRL, _LINK_INTR_1_MASK(i),
            DRF_NUM(_NVLCTRL, _LINK_INTR_1_MASK, _FATAL, 0x0) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_1_MASK, _NONFATAL, 0x0) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_1_MASK, _CORRECTABLE, 0x0));

        NVSWITCH_LINK_WR32_LR10(device, link, NVLW, _NVLCTRL, _LINK_INTR_2_MASK(i),
            DRF_NUM(_NVLCTRL, _LINK_INTR_2_MASK, _FATAL, 0x0) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_2_MASK, _NONFATAL, 0x0) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_2_MASK, _CORRECTABLE, 0x0));
    }
}

/*
 *  Reset NPG & NVLW interrupt state.
 */
static void
_nvswitch_link_reset_interrupts_lr10
(
    nvswitch_device *device,
    NvU32 link
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 i;

    NVSWITCH_NPORT_WR32_LR10(device, link, _NPORT, _ERR_CONTROL_COMMON_NPORT,
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _CORRECTABLEENABLE, 0x1) |
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _FATALENABLE, 0x1) |
        DRF_NUM(_NPORT, _ERR_CONTROL_COMMON_NPORT, _NONFATALENABLE, 0x1));

    for (i = 0; i < NV_NVLCTRL_LINK_INTR_0_STATUS__SIZE_1; i++)
    {
        NVSWITCH_LINK_WR32_LR10(device, link, NVLW, _NVLCTRL, _LINK_INTR_0_MASK(i),
            DRF_NUM(_NVLCTRL, _LINK_INTR_0_MASK, _FATAL, 0x1) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_0_MASK, _NONFATAL, 0x1) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_0_MASK, _CORRECTABLE, 0x1));

        NVSWITCH_LINK_WR32_LR10(device, link, NVLW, _NVLCTRL, _LINK_INTR_1_MASK(i),
            DRF_NUM(_NVLCTRL, _LINK_INTR_1_MASK, _FATAL, 0x1) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_1_MASK, _NONFATAL, 0x1) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_1_MASK, _CORRECTABLE, 0x1));

        NVSWITCH_LINK_WR32_LR10(device, link, NVLW, _NVLCTRL, _LINK_INTR_2_MASK(i),
            DRF_NUM(_NVLCTRL, _LINK_INTR_2_MASK, _FATAL, 0x1) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_2_MASK, _NONFATAL, 0x1) |
            DRF_NUM(_NVLCTRL, _LINK_INTR_2_MASK, _CORRECTABLE, 0x1));
    }

    // Enable interrupts which are disabled to prevent interrupt storm.
    NVSWITCH_NPORT_WR32_LR10(device, link, _ROUTE, _ERR_FATAL_REPORT_EN_0, chip_device->intr_mask.route.fatal);
    NVSWITCH_NPORT_WR32_LR10(device, link, _ROUTE, _ERR_NON_FATAL_REPORT_EN_0, chip_device->intr_mask.route.nonfatal);
    NVSWITCH_NPORT_WR32_LR10(device, link, _INGRESS, _ERR_FATAL_REPORT_EN_0, chip_device->intr_mask.ingress.fatal);
    NVSWITCH_NPORT_WR32_LR10(device, link, _INGRESS, _ERR_NON_FATAL_REPORT_EN_0, chip_device->intr_mask.ingress.nonfatal);
    NVSWITCH_NPORT_WR32_LR10(device, link, _EGRESS, _ERR_FATAL_REPORT_EN_0, chip_device->intr_mask.egress.fatal);
    NVSWITCH_NPORT_WR32_LR10(device, link, _EGRESS, _ERR_NON_FATAL_REPORT_EN_0, chip_device->intr_mask.egress.nonfatal);
    NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_FATAL_REPORT_EN_0, chip_device->intr_mask.tstate.fatal);
    NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _ERR_NON_FATAL_REPORT_EN_0, chip_device->intr_mask.tstate.nonfatal);
    NVSWITCH_NPORT_WR32_LR10(device, link, _SOURCETRACK, _ERR_FATAL_REPORT_EN_0, chip_device->intr_mask.sourcetrack.fatal);
    NVSWITCH_NPORT_WR32_LR10(device, link, _SOURCETRACK, _ERR_NON_FATAL_REPORT_EN_0, chip_device->intr_mask.sourcetrack.nonfatal);

    // Clear fatal error status
    device->link[link].fatal_error_occurred = NV_FALSE;
}

/*
 * @Brief : Control to reset and drain the links.
 *
 * @param[in] device        A reference to the device to initialize
 * @param[in] linkMask      A mask of link(s) to be reset.
 *
 * @returns :               NVL_SUCCESS if there were no errors
 *                         -NVL_BAD_PARAMS if input parameters are wrong.
 *                         -NVL_ERR_INVALID_STATE if other errors are present and a full-chip reset is required.
 *                         -NVL_INITIALIZATION_TOTAL_FAILURE if NPORT initialization failed and a retry is required.
 */

NvlStatus
nvswitch_reset_and_drain_links_lr10
(
    nvswitch_device *device,
    NvU64 link_mask,
    NvBool bForced
)
{
    NvlStatus status = -NVL_ERR_GENERIC;
    nvlink_link *link_info;
    NvU32 val;
    NvU32 link;
    NvU32 idx_nport;
    NvU32 npg;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;
    NvU32 i;
    NvU64 link_mode, tx_sublink_mode, rx_sublink_mode;
    NvU32 tx_sublink_submode, rx_sublink_submode;
    NvU32 *nport_reg_val = NULL;
    NvU32 reg_count = NV_ARRAY_ELEMENTS(nport_reg_addr);
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

    if ((link_mask == 0) ||
        (link_mask >> NVSWITCH_LINK_COUNT(device)))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Invalid link_mask = 0x%llx\n",
            __FUNCTION__, link_mask);

        return -NVL_BAD_ARGS;
    }

    // Check for in-active links
    FOR_EACH_INDEX_IN_MASK(64, link, link_mask)
    {
        if (!nvswitch_is_link_valid(device, link))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: link #%d invalid\n",
                __FUNCTION__, link);

            continue;
        }
        if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NPORT, link))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: NPORT #%d invalid\n",
                __FUNCTION__, link);

            continue;
        }

        if (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLW, link))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: NVLW #%d invalid\n",
                __FUNCTION__, link);

            continue;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Buffer to backup NPORT state
    nport_reg_val = nvswitch_os_malloc(sizeof(nport_reg_addr));
    if (nport_reg_val == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to allocate memory\n",
            __FUNCTION__);

        return -NVL_NO_MEM;
    }

    FOR_EACH_INDEX_IN_MASK(64, link, link_mask)
    {
        // Unregister links to make them unusable while reset is in progress.
        link_info = nvswitch_get_link(device, link);
        if (link_info == NULL)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: invalid link %d\n",
                __FUNCTION__, link);
            continue;
        }

        nvlink_lib_unregister_link(link_info);

        //
        // Step 0 :
        // Prior to starting port reset, FM must shutdown the NVlink links
        // it wishes to reset.
        // However, with shared-virtualization, FM is unable to shut down the links
        // since the GPU is no longer attached to the service VM.
        // In this case, we must perform unilateral shutdown on the LR10 side
        // of the link.
        //
        // If links are in OFF or RESET, we don't need to perform shutdown
        // If links already went through a proper pseudo-clean shutdown sequence,
        // they'll be in SAFE + sublinks in OFF
        //

        status = nvswitch_corelib_get_dl_link_mode_lr10(link_info, &link_mode);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Unable to get link mode from link %d\n",
                __FUNCTION__, link);
            goto nvswitch_reset_and_drain_links_exit;
        }
        status = nvswitch_corelib_get_tx_mode_lr10(link_info, &tx_sublink_mode, &tx_sublink_submode);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Unable to get tx sublink mode from link %d\n",
                __FUNCTION__, link);
            goto nvswitch_reset_and_drain_links_exit;
        }
        status = nvswitch_corelib_get_rx_mode_lr10(link_info, &rx_sublink_mode, &rx_sublink_submode);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Unable to get rx sublink mode from link %d\n",
                __FUNCTION__, link);
            goto nvswitch_reset_and_drain_links_exit;
        }

        if (!((link_mode == NVLINK_LINKSTATE_RESET) ||
              (link_mode == NVLINK_LINKSTATE_OFF) ||
              ((link_mode == NVLINK_LINKSTATE_SAFE) &&
               (tx_sublink_mode == NVLINK_SUBLINK_STATE_TX_OFF) &&
               (rx_sublink_mode == NVLINK_SUBLINK_STATE_RX_OFF))))
        {
            nvswitch_execute_unilateral_link_shutdown_lr10(link_info);
            nvswitch_corelib_clear_link_state_lr10(link_info);
        }

        //
        // Step 1 : Perform surgical reset
        // Refer to switch IAS 11.5.2 Link Reset.
        //

        // Step 1.a : Backup NPORT state before reset
        for (i = 0; i < reg_count; i++)
        {
            nport_reg_val[i] = NVSWITCH_ENG_OFF_RD32(device, NPORT, _UNICAST, link,
                nport_reg_addr[i]);
        }

        // Step 1.b : Assert INGRESS_STOP / EGRESS_STOP
        val = NVSWITCH_NPORT_RD32_LR10(device, link, _NPORT, _CTRL_STOP);
        val = FLD_SET_DRF(_NPORT, _CTRL_STOP, _INGRESS_STOP, _STOP, val);
        val = FLD_SET_DRF(_NPORT, _CTRL_STOP, _EGRESS_STOP, _STOP, val);
        NVSWITCH_NPORT_WR32_LR10(device, link, _NPORT, _CTRL_STOP, val);

        // Wait for stop operation to take effect at TLC.
        // Expected a minimum of 256 clk cycles.
        nvswitch_os_sleep(1);

        //
        // Step 1.c : Disable NPG & NVLW interrupts
        //
        _nvswitch_link_disable_interrupts_lr10(device, link);

        // Step 1.d : Assert NPortWarmReset
        npg = link / NVSWITCH_LINKS_PER_NPG;
        val = NVSWITCH_NPG_RD32_LR10(device, npg, _NPG, _WARMRESET);

        idx_nport = link % NVSWITCH_LINKS_PER_NPG;
        NVSWITCH_NPG_WR32_LR10(device, npg, _NPG, _WARMRESET,
            DRF_NUM(_NPG, _WARMRESET, _NPORTWARMRESET, ~NVBIT(idx_nport)));

        // Step 1.e : Initiate Minion reset sequence.
        status = nvswitch_request_tl_link_state_lr10(link_info,
            NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_RESET, NV_TRUE);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: NvLink Reset has failed for link %d\n",
                __FUNCTION__, link);
            goto nvswitch_reset_and_drain_links_exit;
        }

        // Step 1.e : De-assert NPortWarmReset
        NVSWITCH_NPG_WR32_LR10(device, npg, _NPG, _WARMRESET, val);

        // Step 1.f : Assert and De-assert NPort debug_clear
        // to clear the error status
        NVSWITCH_NPG_WR32_LR10(device, npg, _NPG, _DEBUG_CLEAR,
            DRF_NUM(_NPG, _DEBUG_CLEAR, _CLEAR, NVBIT(idx_nport)));

        NVSWITCH_NPG_WR32_LR10(device, npg, _NPG, _DEBUG_CLEAR,
            DRF_DEF(_NPG, _DEBUG_CLEAR, _CLEAR, _DEASSERT));

        // Step 1.g : Clear CONTAIN_AND_DRAIN to clear contain state (Bug 3115824)
        NVSWITCH_NPORT_WR32_LR10(device, link, _NPORT, _CONTAIN_AND_DRAIN,
            DRF_DEF(_NPORT, _CONTAIN_AND_DRAIN, _CLEAR, _ENABLE));

        val = NVSWITCH_NPORT_RD32_LR10(device, link, _NPORT, _CONTAIN_AND_DRAIN);
        if (FLD_TEST_DRF(_NPORT, _CONTAIN_AND_DRAIN, _CLEAR, _ENABLE, val))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: NPORT Contain and Drain Clear has failed for link %d\n",
                __FUNCTION__, link);
            status = NVL_ERR_INVALID_STATE;
            goto nvswitch_reset_and_drain_links_exit;
        }

        //
        // Step 2 : Assert NPORT Reset after Control & Drain routine.
        //  Clear Tagpool, CrumbStore and CAM RAMs
        //

        // Step 2.a Clear Tagpool RAM
        NVSWITCH_NPORT_WR32_LR10(device, link, _NPORT, _INITIALIZATION,
            DRF_DEF(_NPORT, _INITIALIZATION, _TAGPOOLINIT_0, _HWINIT));

        nvswitch_timeout_create(25 * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);

        do
        {
            keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

            // Check if NPORT initialization is done
            val = NVSWITCH_NPORT_RD32_LR10(device, link, _NPORT, _INITIALIZATION);
            if (FLD_TEST_DRF(_NPORT, _INITIALIZATION, _TAGPOOLINIT_0, _HWINIT, val))
            {
                break;
            }

            nvswitch_os_sleep(1);
        }
        while (keepPolling);

        if (!FLD_TEST_DRF(_NPORT, _INITIALIZATION, _TAGPOOLINIT_0, _HWINIT, val))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout waiting for TAGPOOL Initialization on link %d)\n",
                __FUNCTION__, link);

            status = -NVL_INITIALIZATION_TOTAL_FAILURE;
            goto nvswitch_reset_and_drain_links_exit;
        }

        // Step 2.b Clear CrumbStore RAM
        val = DRF_NUM(_TSTATE, _RAM_ADDRESS, _ADDR, 0) |
              DRF_DEF(_TSTATE, _RAM_ADDRESS, _SELECT, _CRUMBSTORE_RAM) |
              DRF_NUM(_TSTATE, _RAM_ADDRESS, _AUTO_INCR, 1);

        NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _RAM_ADDRESS, val);
        NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _RAM_DATA1, 0x0);

        val = DRF_NUM(_TSTATE, _RAM_DATA0, _ECC, 0x7f);
        for (i = 0; i <= NV_TSTATE_RAM_ADDRESS_ADDR_TAGPOOL_CRUMBSTORE_TDTID_DEPTH; i++)
        {
            NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _RAM_DATA0, val);
        }

        // Step 2.c Clear CAM RAM
        val = DRF_NUM(_TSTATE, _RAM_ADDRESS, _ADDR, 0) |
              DRF_DEF(_TSTATE, _RAM_ADDRESS, _SELECT, _CREQ_CAM) |
              DRF_NUM(_TSTATE, _RAM_ADDRESS, _AUTO_INCR, 1);

        NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _RAM_ADDRESS, val);
        NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _RAM_DATA1, 0x0);
        NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _RAM_DATA2, 0x0);

        for (i = 0; i <= NV_TSTATE_RAM_ADDRESS_ADDR_CREQ_CAM_DEPTH; i++)
        {
            NVSWITCH_NPORT_WR32_LR10(device, link, _TSTATE, _RAM_DATA0, 0x0);
        }

        //
        // Step 3 : Restore link state
        //

        // Restore NPORT state after reset
        for (i = 0; i < reg_count; i++)
        {
            NVSWITCH_ENG_OFF_WR32(device, NPORT, _UNICAST, link,
                                  nport_reg_addr[i], nport_reg_val[i]);
        }

        // Initialize GLT
        nvswitch_set_ganged_link_table_lr10(device, 0, chip_device->ganged_link_table,
                                            ROUTE_GANG_TABLE_SIZE/2);

        // Initialize select scratch registers to 0x0
        nvswitch_init_scratch_lr10(device);

        // Reset NVLW and NPORT interrupt state
        _nvswitch_link_reset_interrupts_lr10(device, link);

        // Re-register links.
        status = nvlink_lib_register_link(device->nvlink_device, link_info);
        if (status != NVL_SUCCESS)
        {
            nvswitch_destroy_link(link_info);
            goto nvswitch_reset_and_drain_links_exit;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Launch ALI training if applicable
    (void)nvswitch_launch_ALI(device);

nvswitch_reset_and_drain_links_exit:
    nvswitch_os_free(nport_reg_val);
    return status;
}

NvlStatus
nvswitch_get_nvlink_ecc_errors_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ECC_ERRORS_PARAMS *params
)
{
    NvU32 statData;
    NvU8 i, j;
    NvlStatus status;
    NvBool bLaneReversed;

    nvswitch_os_memset(params->errorLink, 0, sizeof(params->errorLink));

    FOR_EACH_INDEX_IN_MASK(64, i, params->linkMask)
    {
        nvlink_link         *link;
        NVSWITCH_LANE_ERROR *errorLane;
        NvU8                offset;
        NvBool              minion_enabled;
        NvU32               sublinkWidth;

        link = nvswitch_get_link(device, i);

        if ((link == NULL) ||
            !NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber) ||
            (i >= NVSWITCH_LINK_COUNT(device)))
        {
            return -NVL_BAD_ARGS;
        }

        sublinkWidth = device->hal.nvswitch_get_sublink_width(device, i);

        minion_enabled = nvswitch_is_minion_initialized(device,
            NVSWITCH_GET_LINK_ENG_INST(device, link->linkNumber, MINION));

        bLaneReversed = nvswitch_link_lane_reversed_lr10(device, link->linkNumber);

        for (j = 0; j < NVSWITCH_NVLINK_MAX_LANES; j++)
        {
            if (minion_enabled && (j < sublinkWidth))
            {
                status = nvswitch_minion_get_dl_status(device, i,
                                        (NV_NVLSTAT_RX12 + j), 0, &statData);

                if (status != NVL_SUCCESS)
                {
                    return status;
                }
                offset = bLaneReversed ? ((sublinkWidth - 1) - j) : j;
                errorLane                = &params->errorLink[i].errorLane[offset];
                errorLane->valid         = NV_TRUE;
            }
            else
            {
                // MINION disabled
                statData                 = 0;
                offset                   = j;
                errorLane                = &params->errorLink[i].errorLane[offset];
                errorLane->valid         = NV_FALSE;
            }

            errorLane->eccErrorValue = DRF_VAL(_NVLSTAT, _RX12, _ECC_CORRECTED_ERR_L0_VALUE, statData);
            errorLane->overflowed    = DRF_VAL(_NVLSTAT, _RX12, _ECC_CORRECTED_ERR_L0_OVER, statData);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

static NvU32
nvswitch_get_num_links_lr10
(
    nvswitch_device *device
)
{
    NvU32 num_links = NVSWITCH_NUM_LINKS_LR10;
    return num_links;
}

static NvU8
nvswitch_get_num_links_per_nvlipt_lr10
(
    nvswitch_device *device
)
{
    return NVSWITCH_LINKS_PER_NVLIPT;
}

NvBool
nvswitch_is_link_valid_lr10
(
    nvswitch_device *device,
    NvU32            link_id
)
{
    if (link_id >= nvswitch_get_num_links(device))
    {
        return NV_FALSE;
    }
    return device->link[link_id].valid;
}

NvlStatus
nvswitch_ctrl_get_fom_values_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_FOM_VALUES_PARAMS *p
)
{
    NvlStatus status;
    NvU32     statData;
    nvlink_link *link;

    link = nvswitch_get_link(device, p->linkId);
    if (link == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: link #%d invalid\n",
            __FUNCTION__, p->linkId);
        return -NVL_BAD_ARGS;
    }

    status = nvswitch_minion_get_dl_status(device, p->linkId,
                                        NV_NVLSTAT_TR16, 0, &statData);
    p->figureOfMeritValues[0] = (NvU16) (statData & 0xFFFF);
    p->figureOfMeritValues[1] = (NvU16) ((statData >> 16) & 0xFFFF);

    status = nvswitch_minion_get_dl_status(device, p->linkId,
                                        NV_NVLSTAT_TR17, 0, &statData);
    p->figureOfMeritValues[2] = (NvU16) (statData & 0xFFFF);
    p->figureOfMeritValues[3] = (NvU16) ((statData >> 16) & 0xFFFF);

    p->numLanes = nvswitch_get_sublink_width(device, p->linkId);

    return status;
}

void
nvswitch_set_fatal_error_lr10
(
    nvswitch_device *device,
    NvBool           device_fatal,
    NvU32            link_id
)
{
    NvU32 reg;

    NVSWITCH_ASSERT(link_id < nvswitch_get_num_links(device));

    // On first fatal error, notify PORT_DOWN
    if (!device->link[link_id].fatal_error_occurred)
    {
        if (nvswitch_lib_notify_client_events(device,
                    NVSWITCH_DEVICE_EVENT_PORT_DOWN) != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify PORT_DOWN event\n",
                         __FUNCTION__);
        }
    }

    device->link[link_id].fatal_error_occurred = NV_TRUE;

    if (device_fatal)
    {
        reg = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW, _SW_SCRATCH_12);
        reg = FLD_SET_DRF_NUM(_NVLSAW, _SW_SCRATCH_12, _DEVICE_RESET_REQUIRED,
                              1, reg);

        NVSWITCH_SAW_WR32_LR10(device, _NVLSAW, _SW_SCRATCH_12, reg);
    }
    else
    {
        reg = NVSWITCH_LINK_RD32_LR10(device, link_id, NPORT, _NPORT, _SCRATCH_WARM);
        reg = FLD_SET_DRF_NUM(_NPORT, _SCRATCH_WARM, _PORT_RESET_REQUIRED,
                              1, reg);

        NVSWITCH_LINK_WR32_LR10(device, link_id, NPORT, _NPORT, _SCRATCH_WARM, reg);
    }
}

static NvU32
nvswitch_get_latency_sample_interval_msec_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    return chip_device->latency_stats->sample_interval_msec;
}

NvU32
nvswitch_get_swap_clk_default_lr10
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvBool
nvswitch_is_link_in_use_lr10
(
    nvswitch_device *device,
    NvU32 link_id
)
{
    NvU32 data;
    nvlink_link *link;

    link = nvswitch_get_link(device, link_id);
    if (link == NULL)
    {
        // A query on an invalid link should never occur
        NVSWITCH_ASSERT(link != NULL);
        return NV_FALSE;
    }

    if (nvswitch_is_link_in_reset(device, link))
    {
        return NV_FALSE;
    }

    data = NVSWITCH_LINK_RD32_LR10(device, link_id,
                                   NVLDL, _NVLDL_TOP, _LINK_STATE);

    return (DRF_VAL(_NVLDL_TOP, _LINK_STATE, _STATE, data) !=
            NV_NVLDL_TOP_LINK_STATE_STATE_INIT);
}

static NvU32
nvswitch_get_device_dma_width_lr10
(
    nvswitch_device *device
)
{
    return DMA_ADDR_WIDTH_LR10;
}

NvU32
nvswitch_get_link_ip_version_lr10
(
    nvswitch_device *device,
    NvU32            link_id
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NvU32 nvldl_instance;

    nvldl_instance = NVSWITCH_GET_LINK_ENG_INST(device, link_id, NVLDL);
    if (NVSWITCH_ENG_IS_VALID(device, NVLDL, nvldl_instance))
    {
        return chip_device->engNVLDL[nvldl_instance].version;
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: NVLink[0x%x] NVLDL instance invalid\n",
            __FUNCTION__, link_id);
        return 0;
    }
}

static NvlStatus
nvswitch_test_soe_dma_lr10
(
    nvswitch_device *device
)
{
    return soeTestDma_HAL(device, (PSOE)device->pSoe);
}

static NvlStatus
_nvswitch_get_reserved_throughput_counters
(
    nvswitch_device *device,
    nvlink_link     *link,
    NvU16           counter_mask,
    NvU64           *counter_values
)
{
    NvU16 counter = 0;

    //
    // LR10 to use counters 0 & 2 for monitoring
    // (Same as GPU behavior)
    // Counter 0 counts data flits
    // Counter 2 counts all flits
    //
    FOR_EACH_INDEX_IN_MASK(16, counter, counter_mask)
    {
        NvU32 counter_type = NVBIT(counter);
        NvU64 data = 0;

        switch (counter_type)
        {
            case NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_TX:
            {
                data = nvswitch_read_64bit_counter(device,
                           NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber,
                           NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_LO(0)),
                           NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber,
                           NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_HI(0)));
                break;
            }
            case NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_RX:
            {
                data = nvswitch_read_64bit_counter(device,
                           NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber,
                           NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_LO(0)),
                           NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber,
                           NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_HI(0)));
                break;
            }
            case NVSWITCH_THROUGHPUT_COUNTERS_TYPE_RAW_TX:
            {
                data = nvswitch_read_64bit_counter(device,
                           NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber,
                           NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_LO(2)),
                           NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber,
                           NVLTLC, _NVLTLC_TX_LNK, _DEBUG_TP_CNTR_HI(2)));
                break;
            }
            case NVSWITCH_THROUGHPUT_COUNTERS_TYPE_RAW_RX:
            {
                data = nvswitch_read_64bit_counter(device,
                           NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber,
                           NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_LO(2)),
                           NVSWITCH_LINK_OFFSET_LR10(device, link->linkNumber,
                           NVLTLC, _NVLTLC_RX_LNK, _DEBUG_TP_CNTR_HI(2)));
                break;
            }
            default:
            {
                return -NVL_ERR_NOT_SUPPORTED;
            }
        }
        counter_values[counter] = data;
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_get_throughput_counters_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_THROUGHPUT_COUNTERS_PARAMS *p
)
{
    NvlStatus status;
    nvlink_link *link;
    NvU16 i = 0;

    nvswitch_os_memset(p->counters, 0, sizeof(p->counters));

    FOR_EACH_INDEX_IN_MASK(64, i, p->linkMask)
    {
        link = nvswitch_get_link(device, i);
        if ((link == NULL) || (link->linkNumber >= NVSWITCH_MAX_PORTS) ||
            (!NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLTLC, link->linkNumber)))
        {
            continue;
        }

        status = _nvswitch_get_reserved_throughput_counters(device, link, p->counterMask,
                        p->counters[link->linkNumber].values);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "Failed to get reserved NVLINK throughput counters on link %d\n",
                link->linkNumber);
            return status;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

static NvBool
nvswitch_is_soe_supported_lr10
(
    nvswitch_device *device
)
{
    if (device->regkeys.soe_disable == NV_SWITCH_REGKEY_SOE_DISABLE_YES)
    {
        NVSWITCH_PRINT(device, INFO, "SOE is disabled via regkey.\n");
        return NV_FALSE;
    }

    return NV_TRUE;
}

NvBool
nvswitch_is_inforom_supported_lr10
(
    nvswitch_device *device
)
{
    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "INFOROM is not supported on non-silicon platform\n");
        return NV_FALSE;
    }

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "INFOROM is not supported since SOE is not supported\n");
        return NV_FALSE;
    }

    return NV_TRUE;
}

NvBool
nvswitch_is_spi_supported_lr10
(
    nvswitch_device *device
)
{
    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "SPI is not supported on non-silicon platforms\n");
        return NV_FALSE;
    }

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "SPI is not supported since SOE is not supported\n");
        return NV_FALSE;
    }

    return NV_TRUE;
}

NvBool
nvswitch_is_bios_supported_lr10
(
    nvswitch_device *device
)
{
    return nvswitch_is_spi_supported(device);
}

NvlStatus
nvswitch_get_bios_size_lr10
(
    nvswitch_device *device,
    NvU32 *pSize
)
{
    return nvswitch_bios_read_size(device, pSize);
}

NvBool
nvswitch_is_smbpbi_supported_lr10
(
    nvswitch_device *device
)
{
    if (IS_RTLSIM(device) || IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "SMBPBI is not supported on RTLSIM/FMODEL platforms\n");
        return NV_FALSE;
    }

    if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, INFO,
            "SMBPBI is not supported since SOE is not supported\n");
        return NV_FALSE;
    }

    return NV_TRUE;
}

/*
 * @Brief : Additional setup needed after device initialization
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 */
NvlStatus
nvswitch_post_init_device_setup_lr10
(
    nvswitch_device *device
)
{
    NvlStatus retval;

    if (device->regkeys.soe_dma_self_test ==
            NV_SWITCH_REGKEY_SOE_DMA_SELFTEST_DISABLE)
    {
        NVSWITCH_PRINT(device, INFO,
            "Skipping SOE DMA selftest as requested using regkey\n");
    }
    else if (IS_RTLSIM(device) || IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, SETUP,
            "Skipping DMA selftest on FMODEL/RTLSIM platforms\n");
    }
    else if (!nvswitch_is_soe_supported(device))
    {
        NVSWITCH_PRINT(device, SETUP,
            "Skipping DMA selftest since SOE is not supported\n");
    }
    else
    {
        retval = nvswitch_test_soe_dma_lr10(device);
        if (retval != NVL_SUCCESS)
        {
            return retval;
        }
    }

    if (nvswitch_is_inforom_supported(device))
    {
        nvswitch_inforom_post_init(device);
    }
    else
    {
        NVSWITCH_PRINT(device, SETUP, "Skipping INFOROM init\n");
    }

    nvswitch_soe_init_l2_state(device);

    return NVL_SUCCESS;
}

/*
 * @Brief : Additional setup needed after blacklisted device initialization
 *
 * @Description :
 *
 * @param[in] device        a reference to the device to initialize
 */
void
nvswitch_post_init_blacklist_device_setup_lr10
(
    nvswitch_device *device
)
{
    NvlStatus status;

    if (nvswitch_is_inforom_supported(device))
    {
        nvswitch_inforom_post_init(device);
    }

    //
    // Initialize the driver state monitoring callback.
    // This is still needed for SOE to report correct driver state.
    //
    status = nvswitch_smbpbi_post_init(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Smbpbi post init failed, rc:%d\n",
                       status);
        return;
    }

    //
    // This internally will only flush if OMS value has changed
    //
    status = device->hal.nvswitch_oms_inforom_flush(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Flushing OMS failed, rc:%d\n",
                       status);
        return;
    }
}

void
nvswitch_load_uuid_lr10
(
    nvswitch_device *device
)
{
    NvU32 regData[4];

    //
    // Read 128-bit UUID from secure scratch registers which must be
    // populated by firmware.
    //
    regData[0] = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_SW, _SCRATCH_8);
    regData[1] = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_SW, _SCRATCH_9);
    regData[2] = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_SW, _SCRATCH_10);
    regData[3] = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_SW, _SCRATCH_11);

    nvswitch_os_memcpy(&device->uuid.uuid, (NvU8 *)regData, NV_UUID_LEN);
}

NvlStatus
nvswitch_read_oob_blacklist_state_lr10
(
    nvswitch_device *device
)
{
    NvU32 reg;
    NvBool is_oob_blacklist;
    NvlStatus status;

    if (device == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Called with invalid argument\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    reg = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW, _SCRATCH_COLD);

    // Check for uninitialized SCRATCH_COLD before declaring the device blacklisted
    if (reg == NV_NVLSAW_SCRATCH_COLD_DATA_INIT)
        is_oob_blacklist = NV_FALSE;
    else
        is_oob_blacklist = DRF_VAL(_NVLSAW, _SCRATCH_COLD, _OOB_BLACKLIST_DEVICE_REQUESTED, reg);

    status = nvswitch_inforom_oms_set_device_disable(device, is_oob_blacklist);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to set device disable to %d, rc:%d\n",
            is_oob_blacklist, status);
    }

    if (is_oob_blacklist)
    {
        device->device_fabric_state = NVSWITCH_DEVICE_FABRIC_STATE_BLACKLISTED;
        device->device_blacklist_reason = NVSWITCH_DEVICE_BLACKLIST_REASON_MANUAL_OUT_OF_BAND;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_write_fabric_state_lr10
(
    nvswitch_device *device
)
{
    NvU32 reg;

    if (device == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Called with invalid argument\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    // bump the sequence number for each write
    device->fabric_state_sequence_number++;

    reg = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW, _SW_SCRATCH_12);

    reg = FLD_SET_DRF_NUM(_NVLSAW, _SW_SCRATCH_12, _DEVICE_BLACKLIST_REASON,
                          device->device_blacklist_reason, reg);
    reg = FLD_SET_DRF_NUM(_NVLSAW, _SW_SCRATCH_12, _DEVICE_FABRIC_STATE,
                          device->device_fabric_state, reg);
    reg = FLD_SET_DRF_NUM(_NVLSAW, _SW_SCRATCH_12, _DRIVER_FABRIC_STATE,
                          device->driver_fabric_state, reg);
    reg = FLD_SET_DRF_NUM(_NVLSAW, _SW_SCRATCH_12, _EVENT_MESSAGE_COUNT,
                          device->fabric_state_sequence_number, reg);

    NVSWITCH_SAW_WR32_LR10(device, _NVLSAW, _SW_SCRATCH_12, reg);

    return NVL_SUCCESS;
}

static NVSWITCH_ENGINE_DESCRIPTOR_TYPE *
_nvswitch_get_eng_descriptor_lr10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);
    NVSWITCH_ENGINE_DESCRIPTOR_TYPE  *engine = NULL;

    if (eng_id >= NVSWITCH_ENGINE_ID_SIZE)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Engine_ID 0x%x out of range 0..0x%x\n",
            __FUNCTION__,
            eng_id, NVSWITCH_ENGINE_ID_SIZE-1);
        return NULL;
    }

    engine = &(chip_device->io.common[eng_id]);
    NVSWITCH_ASSERT(eng_id == engine->eng_id);

    return engine;
}

NvU32
nvswitch_get_eng_base_lr10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id,
    NvU32 eng_bcast,
    NvU32 eng_instance
)
{
    NVSWITCH_ENGINE_DESCRIPTOR_TYPE  *engine;
    NvU32 base_addr = NVSWITCH_BASE_ADDR_INVALID;

    engine = _nvswitch_get_eng_descriptor_lr10(device, eng_id);
    if (engine == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ID 0x%x[%d] %s not found\n",
            __FUNCTION__,
            eng_id, eng_instance,
            (
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) ? "UC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST) ? "BC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) ? "MC" :
                "??"
            ));
        return NVSWITCH_BASE_ADDR_INVALID;
    }

    if ((eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) &&
        (eng_instance < engine->eng_count))
    {
        base_addr = engine->uc_addr[eng_instance];
    }
    else if (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST)
    {
        base_addr = engine->bc_addr;
    }
    else if ((eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) &&
        (eng_instance < engine->mc_addr_count))
    {
        base_addr = engine->mc_addr[eng_instance];
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unknown address space type 0x%x (not UC, BC, or MC)\n",
            __FUNCTION__,
            eng_bcast);
    }

    if (base_addr == NVSWITCH_BASE_ADDR_INVALID)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ID 0x%x[%d] %s invalid address\n",
            __FUNCTION__,
            eng_id, eng_instance,
            (
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) ? "UC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST) ? "BC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) ? "MC" :
                "??"
            ));
    }

    return base_addr;
}

NvU32
nvswitch_get_eng_count_lr10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id,
    NvU32 eng_bcast
)
{
    NVSWITCH_ENGINE_DESCRIPTOR_TYPE  *engine;
    NvU32 eng_count = 0;

    engine = _nvswitch_get_eng_descriptor_lr10(device, eng_id);
    if (engine == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ID 0x%x %s not found\n",
            __FUNCTION__,
            eng_id,
            (
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) ? "UC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST) ? "BC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) ? "MC" :
                "??"
            ));
        return 0;
    }

    if (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST)
    {
        eng_count = engine->eng_count;
    }
    else if (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST)
    {
        if (engine->bc_addr == NVSWITCH_BASE_ADDR_INVALID)
        {
            eng_count = 0;
        }
        else
        {
            eng_count = 1;
        }
    }
    else if (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST)
    {
        eng_count = engine->mc_addr_count;
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unknown address space type 0x%x (not UC, BC, or MC)\n",
            __FUNCTION__,
            eng_bcast);
    }

    return eng_count;
}

NvU32
nvswitch_eng_rd_lr10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id,
    NvU32 eng_bcast,
    NvU32 eng_instance,
    NvU32 offset
)
{
    NvU32 base_addr = NVSWITCH_BASE_ADDR_INVALID;
    NvU32 data;

    base_addr = nvswitch_get_eng_base_lr10(device, eng_id, eng_bcast, eng_instance);
    if (base_addr == NVSWITCH_BASE_ADDR_INVALID)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ID 0x%x[%d] %s invalid address\n",
            __FUNCTION__,
            eng_id, eng_instance,
            (
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) ? "UC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST) ? "BC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) ? "MC" :
                "??"
            ));
        NVSWITCH_ASSERT(base_addr != NVSWITCH_BASE_ADDR_INVALID);
        return 0xBADFBADF;
    }

    data = nvswitch_reg_read_32(device, base_addr + offset);

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
    {
        NVSWITCH_ENGINE_DESCRIPTOR_TYPE  *engine = _nvswitch_get_eng_descriptor_lr10(device, eng_id);

        NVSWITCH_PRINT(device, MMIO,
            "%s: ENG_RD %s(0x%x)[%d] @0x%08x+0x%06x = 0x%08x\n",
            __FUNCTION__,
            engine->eng_name, engine->eng_id,
            eng_instance,
            base_addr, offset,
            data);
    }
#endif  //defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)

    return data;
}

void
nvswitch_eng_wr_lr10
(
    nvswitch_device *device,
    NVSWITCH_ENGINE_ID eng_id,
    NvU32 eng_bcast,
    NvU32 eng_instance,
    NvU32 offset,
    NvU32 data
)
{
    NvU32 base_addr = NVSWITCH_BASE_ADDR_INVALID;

    base_addr = nvswitch_get_eng_base_lr10(device, eng_id, eng_bcast, eng_instance);
    if (base_addr == NVSWITCH_BASE_ADDR_INVALID)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ID 0x%x[%d] %s invalid address\n",
            __FUNCTION__,
            eng_id, eng_instance,
            (
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_UNICAST) ? "UC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_BCAST) ? "BC" :
                (eng_bcast == NVSWITCH_GET_ENG_DESC_TYPE_MULTICAST) ? "MC" :
                "??"
            ));
        NVSWITCH_ASSERT(base_addr != NVSWITCH_BASE_ADDR_INVALID);
        return;
    }

    nvswitch_reg_write_32(device, base_addr + offset,  data);

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
    {
        NVSWITCH_ENGINE_DESCRIPTOR_TYPE  *engine = _nvswitch_get_eng_descriptor_lr10(device, eng_id);

        NVSWITCH_PRINT(device, MMIO,
            "%s: ENG_WR %s(0x%x)[%d] @0x%08x+0x%06x = 0x%08x\n",
            __FUNCTION__,
            engine->eng_name, engine->eng_id,
            eng_instance,
            base_addr, offset,
            data);
    }
#endif  //defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
}

NvU32
nvswitch_get_link_eng_inst_lr10
(
    nvswitch_device *device,
    NvU32 link_id,
    NVSWITCH_ENGINE_ID eng_id
)
{
    NvU32   eng_instance = NVSWITCH_ENGINE_INSTANCE_INVALID;

    if (link_id >= NVSWITCH_LINK_COUNT(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: link ID 0x%x out-of-range [0x0..0x%x]\n",
            __FUNCTION__,
            link_id, NVSWITCH_LINK_COUNT(device)-1);
        return NVSWITCH_ENGINE_INSTANCE_INVALID;
    }

    switch (eng_id)
    {
        case NVSWITCH_ENGINE_ID_NPG:
            eng_instance = link_id / NVSWITCH_LINKS_PER_NPG;
            break;
        case NVSWITCH_ENGINE_ID_NVLIPT:
            eng_instance = link_id / NVSWITCH_LINKS_PER_NVLIPT;
            break;
        case NVSWITCH_ENGINE_ID_NVLW:
        case NVSWITCH_ENGINE_ID_NVLW_PERFMON:
            eng_instance = link_id / NVSWITCH_LINKS_PER_NVLW;
            break;
        case NVSWITCH_ENGINE_ID_MINION:
            eng_instance = link_id / NVSWITCH_LINKS_PER_MINION;
            break;
        case NVSWITCH_ENGINE_ID_NPORT:
        case NVSWITCH_ENGINE_ID_NVLTLC:
        case NVSWITCH_ENGINE_ID_NVLDL:
        case NVSWITCH_ENGINE_ID_NVLIPT_LNK:
        case NVSWITCH_ENGINE_ID_NPORT_PERFMON:
            eng_instance = link_id;
            break;
        default:
            NVSWITCH_PRINT(device, ERROR,
                "%s: link ID 0x%x has no association with EngID 0x%x\n",
                __FUNCTION__,
                link_id, eng_id);
            eng_instance = NVSWITCH_ENGINE_INSTANCE_INVALID;
            break;
    }

    return eng_instance;
}

NvU32
nvswitch_get_caps_nvlink_version_lr10
(
    nvswitch_device *device
)
{
    ct_assert(NVSWITCH_NVLINK_STATUS_NVLINK_VERSION_3_0 ==
                NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_3_0);
    return NVSWITCH_NVLINK_CAPS_NVLINK_VERSION_3_0;
}

NVSWITCH_BIOS_NVLINK_CONFIG *
nvswitch_get_bios_nvlink_config_lr10
(
    nvswitch_device *device
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

    return (chip_device != NULL) ? &chip_device->bios_config : NULL;
}

/*
 * CTRL_NVSWITCH_SET_RESIDENCY_BINS
 */
static NvlStatus
nvswitch_ctrl_set_residency_bins_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_RESIDENCY_BINS *p
)
{
    NVSWITCH_PRINT(device, ERROR,
        "SET_RESIDENCY_BINS should not be called on LR10\n");
    return -NVL_ERR_NOT_SUPPORTED;
}

/*
 * CTRL_NVSWITCH_GET_RESIDENCY_BINS
 */
static NvlStatus
nvswitch_ctrl_get_residency_bins_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_RESIDENCY_BINS *p
)
{
    NVSWITCH_PRINT(device, ERROR,
        "GET_RESIDENCY_BINS should not be called on LR10\n");
    return -NVL_ERR_NOT_SUPPORTED;
}

/*
 * CTRL_NVSWITCH_GET_RB_STALL_BUSY
 */
static NvlStatus
nvswitch_ctrl_get_rb_stall_busy_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_RB_STALL_BUSY *p
)
{
    NVSWITCH_PRINT(device, ERROR,
        "GET_RB_STALL_BUSY should not be called on LR10\n");
    return -NVL_ERR_NOT_SUPPORTED;
}

/*
 * CTRL_NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR
 */
static NvlStatus
nvswitch_ctrl_get_multicast_id_error_vector_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR *p
)
{
    NVSWITCH_PRINT(device, ERROR,
        "GET_MULTICAST_ID_ERROR_VECTOR should not be called on LR10\n");
    return -NVL_ERR_NOT_SUPPORTED;
}

/*
 * CTRL_NVSWITCH_CLEAR_MULTICAST_ID_ERROR_VECTOR
 */
static NvlStatus
nvswitch_ctrl_clear_multicast_id_error_vector_lr10
(
    nvswitch_device *device,
    NVSWITCH_CLEAR_MULTICAST_ID_ERROR_VECTOR *p
)
{
    NVSWITCH_PRINT(device, ERROR,
        "CLEAR_MULTICAST_ID_ERROR_VECTOR should not be called on LR10\n");
    return -NVL_ERR_NOT_SUPPORTED;
}

void
nvswitch_send_inband_nack_lr10
(
    nvswitch_device *device,
    NvU32 *msghdr,
    NvU32  linkId
)
{
    return;
}

NvU32
nvswitch_get_max_persistent_message_count_lr10
(
    nvswitch_device *device
)
{
    return 0;
}

/*
 * CTRL_NVSWITCH_INBAND_SEND_DATA
 */
NvlStatus
nvswitch_ctrl_inband_send_data_lr10
(
    nvswitch_device *device,
    NVSWITCH_INBAND_SEND_DATA_PARAMS *p
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

/*
 * CTRL_NVSWITCH_INBAND_RECEIVE_DATA
 */
NvlStatus
nvswitch_ctrl_inband_read_data_lr10
(
    nvswitch_device *device,
    NVSWITCH_INBAND_READ_DATA_PARAMS *p
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

/*
 * CTRL_NVSWITCH_GET_BOARD_PART_NUMBER
 */
NvlStatus
nvswitch_ctrl_get_board_part_number_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_BOARD_PART_NUMBER_VECTOR *p
)
{
    struct inforom *pInforom = device->pInforom;
    INFOROM_OBD_OBJECT_V1_XX *pOBDObj;
    int byteIdx;

    if (pInforom == NULL)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (!pInforom->OBD.bValid)
    {
        NVSWITCH_PRINT(device, ERROR, "OBD data is not available\n");
        return -NVL_ERR_GENERIC;
    }

    pOBDObj = &pInforom->OBD.object.v1;

    if (sizeof(p->data) != sizeof(pOBDObj->productPartNumber)/sizeof(inforom_U008))
    {
        NVSWITCH_PRINT(device, ERROR,
                       "board part number available size %lu is not same as the request size %lu\n",
                       sizeof(pOBDObj->productPartNumber)/sizeof(inforom_U008), sizeof(p->data));
        return -NVL_ERR_GENERIC;
    }

    nvswitch_os_memset(p, 0, sizeof(NVSWITCH_GET_BOARD_PART_NUMBER_VECTOR));

    /* Copy board type data */
    for (byteIdx = 0; byteIdx < NVSWITCH_BOARD_PART_NUMBER_SIZE_IN_BYTES; byteIdx++)
    {
        p->data[byteIdx] =(NvU8)(pOBDObj->productPartNumber[byteIdx] & 0xFF);
    }

    return NVL_SUCCESS;
}

/*
* @brief: This function retrieves the NVLIPT public ID for a given global link idx
* @params[in]  device        reference to current nvswitch device
* @params[in]  linkId        link to retrieve NVLIPT public ID from
* @params[out] publicId      Public ID of NVLIPT owning linkId
*/
NvlStatus nvswitch_get_link_public_id_lr10
(
    nvswitch_device *device,
    NvU32 linkId,
    NvU32 *publicId
)
{
    if (!device->hal.nvswitch_is_link_valid(device, linkId) ||
        (publicId == NULL))
    {
        return -NVL_BAD_ARGS;
    }

    *publicId = NVSWITCH_NVLIPT_GET_PUBLIC_ID_LR10(linkId);


    return (NVSWITCH_ENG_VALID_LR10(device, NVLIPT, *publicId)) ?
                NVL_SUCCESS : -NVL_BAD_ARGS;
}

/*
* @brief: This function retrieves the internal link idx for a given global link idx
* @params[in]  device        reference to current nvswitch device
* @params[in]  linkId        link to retrieve NVLIPT public ID from
* @params[out] localLinkIdx  Internal link index of linkId
*/
NvlStatus nvswitch_get_link_local_idx_lr10
(
    nvswitch_device *device,
    NvU32 linkId,
    NvU32 *localLinkIdx
)
{
    if (!device->hal.nvswitch_is_link_valid(device, linkId) ||
        (localLinkIdx == NULL))
    {
        return -NVL_BAD_ARGS;
    }

    *localLinkIdx = NVSWITCH_NVLIPT_GET_LOCAL_LINK_ID_LR10(linkId);

    return NVL_SUCCESS;
}

NvlStatus nvswitch_set_training_error_info_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_TRAINING_ERROR_INFO_PARAMS *pLinkTrainingErrorInfoParams
)
{
    NVSWITCH_LINK_TRAINING_ERROR_INFO linkTrainingErrorInfo;
    NVSWITCH_LINK_RUNTIME_ERROR_INFO linkRuntimeErrorInfo;

    linkTrainingErrorInfo.isValid = NV_TRUE;
    linkTrainingErrorInfo.attemptedTrainingMask0 =
        pLinkTrainingErrorInfoParams->attemptedTrainingMask0;
    linkTrainingErrorInfo.trainingErrorMask0 =
        pLinkTrainingErrorInfoParams->trainingErrorMask0;

    linkRuntimeErrorInfo.isValid = NV_FALSE;
    linkRuntimeErrorInfo.mask0   = 0;

    return nvswitch_smbpbi_set_link_error_info(device,
                                               &linkTrainingErrorInfo,
                                               &linkRuntimeErrorInfo);
}

NvlStatus nvswitch_ctrl_get_fatal_error_scope_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_FATAL_ERROR_SCOPE_PARAMS *pParams
)
{
    NvU32 linkId;
    NvU32 reg = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW, _SW_SCRATCH_12);
    pParams->device = FLD_TEST_DRF_NUM(_NVLSAW, _SW_SCRATCH_12, _DEVICE_RESET_REQUIRED,
                                       1, reg);

    for (linkId = 0; linkId < NVSWITCH_MAX_PORTS; linkId++)
    {
        if (!nvswitch_is_link_valid(device, linkId))
        {
            pParams->port[linkId] = NV_FALSE;
            continue;
        }

        reg = NVSWITCH_LINK_RD32_LR10(device, linkId, NPORT, _NPORT, _SCRATCH_WARM);
        pParams->port[linkId] = FLD_TEST_DRF_NUM(_NPORT, _SCRATCH_WARM,
                                                 _PORT_RESET_REQUIRED, 1, reg);
    }

    return NVL_SUCCESS;
}

NvlStatus nvswitch_ctrl_set_mc_rid_table_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_MC_RID_TABLE_PARAMS *p
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus nvswitch_ctrl_get_mc_rid_table_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_MC_RID_TABLE_PARAMS *p
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

void nvswitch_init_scratch_lr10
(
    nvswitch_device *device
)
{
    NvU32 linkId;
    NvU32 reg;

    for (linkId = 0; linkId < nvswitch_get_num_links(device); linkId++)
    {
        if (!nvswitch_is_link_valid(device, linkId))
        {
            continue;
        }

        reg = NVSWITCH_LINK_RD32(device, linkId, NPORT, _NPORT, _SCRATCH_WARM);
        if (reg == NV_NPORT_SCRATCH_WARM_DATA_INIT)
        {
            NVSWITCH_LINK_WR32(device, linkId, NPORT, _NPORT, _SCRATCH_WARM, 0);
        }
    }
}

NvlStatus
nvswitch_launch_ALI_lr10
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_set_training_mode_lr10
(
    nvswitch_device *device
)
{
    return NVL_SUCCESS;
}

NvlStatus
nvswitch_parse_bios_image_lr10
(
    nvswitch_device *device
)
{
    NVSWITCH_BIOS_NVLINK_CONFIG *bios_config;
    NV_STATUS status = NV_OK;

    // check if spi is supported
    if (!nvswitch_is_bios_supported(device))
    {
        NVSWITCH_PRINT(device, ERROR,
                "%s: BIOS is not supported\n",
                __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    bios_config = nvswitch_get_bios_nvlink_config(device);

    // Parse and retrieve the VBIOS info
    status = _nvswitch_setup_link_vbios_overrides(device, bios_config);
    if ((status != NV_OK) && device->pSoe)
    {
        //To enable LS10 bringup (VBIOS is not ready and SOE is disabled), fail the device init only when SOE is enabled and vbios overrides has failed
        NVSWITCH_PRINT(device, ERROR,
                "%s: error=0x%x\n",
                __FUNCTION__, status);

        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_get_nvlink_lp_counters_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_LP_COUNTERS_PARAMS *params
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_ctrl_get_sw_info_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_SW_INFO_PARAMS *p
)
{
    NvlStatus retval = NVL_SUCCESS;
    NvU32 i;

    if (p->count > NVSWITCH_GET_SW_INFO_COUNT_MAX)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Invalid args\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    nvswitch_os_memset(p->info, 0, sizeof(NvU32)*NVSWITCH_GET_SW_INFO_COUNT_MAX);

    for (i = 0; i < p->count; i++)
    {
        switch (p->index[i])
        {
            case NVSWITCH_GET_SW_INFO_INDEX_INFOROM_NVL_SUPPORTED:
                p->info[i] = NV_TRUE;
                break;
            case NVSWITCH_GET_SW_INFO_INDEX_INFOROM_BBX_SUPPORTED:
                p->info[i] = (NvU32)_nvswitch_inforom_bbx_supported(device);
                break;
            default:
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Undefined NVSWITCH_GET_SW_INFO_INDEX 0x%x\n",
                    __FUNCTION__,
                    p->index[i]);
                retval = -NVL_BAD_ARGS;
                break;
        }
    }

    return retval;
}

NvlStatus
nvswitch_ctrl_get_err_info_lr10
(
    nvswitch_device *device,
    NVSWITCH_NVLINK_GET_ERR_INFO_PARAMS *ret
)
{
    nvlink_link *link;
    NvU32 data;
    NvU8 i;

     ret->linkMask = nvswitch_get_enabled_link_mask(device);

    FOR_EACH_INDEX_IN_MASK(64, i, ret->linkMask)
    {
        link = nvswitch_get_link(device, i);

        if ((link == NULL) ||
            !NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber) ||
            (i >= NVSWITCH_NVLINK_MAX_LINKS))
        {
            continue;
        }

        // TODO NVidia TL not supported
        NVSWITCH_PRINT(device, NOISY,
            "%s WARNING: Nvidia %s register %s does not exist!\n",
            __FUNCTION__, "NVLTL", "NV_NVLTL_TL_ERRLOG_REG");

        NVSWITCH_PRINT(device, NOISY,
            "%s WARNING: Nvidia %s register %s does not exist!\n",
            __FUNCTION__, "NVLTL", "NV_NVLTL_TL_INTEN_REG");

        ret->linkErrInfo[i].TLErrlog = 0x0;
        ret->linkErrInfo[i].TLIntrEn = 0x0;

        data = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TX, _SLSM_STATUS_TX);
        ret->linkErrInfo[i].DLSpeedStatusTx =
            DRF_VAL(_NVLDL_TX, _SLSM_STATUS_TX, _PRIMARY_STATE, data);

        data = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_RX, _SLSM_STATUS_RX);
        ret->linkErrInfo[i].DLSpeedStatusRx =
            DRF_VAL(_NVLDL_RX, _SLSM_STATUS_RX, _PRIMARY_STATE, data);

        data = NVSWITCH_LINK_RD32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _INTR);
        ret->linkErrInfo[i].bExcessErrorDL =
            !!DRF_VAL(_NVLDL_TOP, _INTR, _RX_SHORT_ERROR_RATE, data);

        if (ret->linkErrInfo[i].bExcessErrorDL)
        {
            NVSWITCH_LINK_WR32_LR10(device, link->linkNumber, NVLDL, _NVLDL_TOP, _INTR,
                DRF_NUM(_NVLDL_TOP, _INTR, _RX_SHORT_ERROR_RATE, 0x1));
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_ctrl_clear_counters_lr10
(
    nvswitch_device *device,
    NVSWITCH_NVLINK_CLEAR_COUNTERS_PARAMS *ret
)
{
    nvlink_link *link;
    NvU8 i;
    NvU32 counterMask;
    NvlStatus status = NVL_SUCCESS;

    counterMask = ret->counterMask;

    // Common usage allows one of these to stand for all of them
    if ((counterMask) & ( NVSWITCH_NVLINK_COUNTER_TL_TX0
                        | NVSWITCH_NVLINK_COUNTER_TL_TX1
                        | NVSWITCH_NVLINK_COUNTER_TL_RX0
                        | NVSWITCH_NVLINK_COUNTER_TL_RX1
                        ))
    {
        counterMask |= ( NVSWITCH_NVLINK_COUNTER_TL_TX0
                       | NVSWITCH_NVLINK_COUNTER_TL_TX1
                       | NVSWITCH_NVLINK_COUNTER_TL_RX0
                       | NVSWITCH_NVLINK_COUNTER_TL_RX1
                       );
    }

    // Common usage allows one of these to stand for all of them
    if ((counterMask) & ( NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT
                        | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L0
                        | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L1
                        | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L2
                        | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L3
                        | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L4
                        | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L5
                        | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L6
                        | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L7
                        | NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY
                        | NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY
                        ))
    {
        counterMask |= ( NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_FLIT
                       | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L0
                       | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L1
                       | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L2
                       | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L3
                       | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L4
                       | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L5
                       | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L6
                       | NVSWITCH_NVLINK_COUNTER_DL_RX_ERR_CRC_LANE_L7
                       | NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_REPLAY
                       | NVSWITCH_NVLINK_COUNTER_DL_TX_ERR_RECOVERY
                       );
    }

    FOR_EACH_INDEX_IN_MASK(64, i, ret->linkMask)
    {
        link = nvswitch_get_link(device, i);
        if (link == NULL)
        {
            continue;
        }

        if (NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLTLC, link->linkNumber))
        {
            nvswitch_ctrl_clear_throughput_counters_lr10(device, link, counterMask);
        }
        if (NVSWITCH_IS_LINK_ENG_VALID_LR10(device, NVLDL, link->linkNumber))
        {
            status = nvswitch_ctrl_clear_dl_error_counters_lr10(device, link, counterMask);
            // Return early with failure on clearing through minion
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Failure on clearing link counter mask 0x%x on link %d\n",
                    __FUNCTION__, counterMask, link->linkNumber);
                break;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return status;
}

NvlStatus
nvswitch_ctrl_set_nvlink_error_threshold_lr10
(
    nvswitch_device *device,
    NVSWITCH_SET_NVLINK_ERROR_THRESHOLD_PARAMS *ret
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

static NvlStatus
nvswitch_ctrl_get_nvlink_error_threshold_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ERROR_THRESHOLD_PARAMS *ret
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_get_board_id_lr10
(
    nvswitch_device *device,
    NvU16 *pBoardId
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_ctrl_get_soe_heartbeat_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_SOE_HEARTBEAT_PARAMS *p
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

static NvlStatus
nvswitch_cci_reset_and_drain_links_lr10
(
    nvswitch_device *device,
    NvU64 link_mask,
    NvBool bForced
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

void
nvswitch_update_link_state_led_lr10
(
    nvswitch_device *device
)
{
    return;
}

void
nvswitch_led_shutdown_lr10
(
    nvswitch_device *device
)
{
    return;
}

NvlStatus
nvswitch_check_io_sanity_lr10
(
    nvswitch_device *device
)
{
    return NVL_SUCCESS;
}

void
nvswitch_fsp_update_cmdq_head_tail_lr10
(
    nvswitch_device *device,
    NvU32 queueHead,
    NvU32 queueTail
)
{
    return; // -NVL_ERR_NOT_SUPPORTED;
}

void
nvswitch_fsp_get_cmdq_head_tail_lr10
(
    nvswitch_device *device,
    NvU32 *pQueueHead,
    NvU32 *pQueueTail
)
{
    return; // -NVL_ERR_NOT_SUPPORTED;
}

void
nvswitch_fsp_update_msgq_head_tail_lr10
(
    nvswitch_device *device,
    NvU32 msgqHead,
    NvU32 msgqTail
)
{
    return; // -NVL_ERR_NOT_SUPPORTED;
}

void
nvswitch_fsp_get_msgq_head_tail_lr10
(
    nvswitch_device *device,
    NvU32 *pMsgqHead,
    NvU32 *pMsgqTail
)
{
   return; // -NVL_ERR_NOT_SUPPORTED;
}

NvU32
nvswitch_fsp_get_channel_size_lr10
(
    nvswitch_device *device
)
{
    return 0; // -NVL_ERR_NOT_SUPPORTED;
}

NvU8
nvswitch_fsp_nvdm_to_seid_lr10
(
    nvswitch_device *device,
    NvU8 nvdmType
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvU32
nvswitch_fsp_create_mctp_header_lr10
(
    nvswitch_device *device,
    NvU8 som,
    NvU8 eom,
    NvU8 seid,
    NvU8 seq
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvU32
nvswitch_fsp_create_nvdm_header_lr10
(
    nvswitch_device *device,
    NvU32 nvdmType
)
{
    return 0; // -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_fsp_get_packet_info_lr10
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size,
    NvU8 *pPacketState,
    NvU8 *pTag
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_fsp_validate_mctp_payload_header_lr10
(
    nvswitch_device  *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_fsp_process_nvdm_msg_lr10
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_fsp_process_cmd_response_lr10
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_fsp_config_ememc_lr10
(
    nvswitch_device *device,
    NvU32 offset,
    NvBool bAincw,
    NvBool bAincr
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_fsp_write_to_emem_lr10
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_fsp_read_from_emem_lr10
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_fsp_error_code_to_nvlstatus_map_lr10
(
    nvswitch_device *device,
    NvU32 errorCode
)
{
    return -NVL_ERR_NOT_SUPPORTED; 
}

NvlStatus
nvswitch_fsprpc_get_caps_lr10
(
    nvswitch_device *device,
    NVSWITCH_FSPRPC_GET_CAPS_PARAMS *params
)
{
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_detect_tnvl_mode_lr10
(
    nvswitch_device *device
)
{
    return -NVL_ERR_NOT_SUPPORTED; 
}

NvBool
nvswitch_is_tnvl_mode_enabled_lr10
(
    nvswitch_device *device
)
{
    return NV_FALSE;
}

NvBool
nvswitch_is_tnvl_mode_locked_lr10
(
    nvswitch_device *device
)
{
    return NV_FALSE;
}

NvlStatus
nvswitch_tnvl_get_attestation_certificate_chain_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_ATTESTATION_CERTIFICATE_CHAIN_PARAMS *params
)
{
    // Not supported in LR10
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_tnvl_get_attestation_report_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_ATTESTATION_REPORT_PARAMS *params
)
{
    // Not supported in LR10
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_tnvl_send_fsp_lock_config_lr10
(
    nvswitch_device *device
)
{
    // Not supported in LR10
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_tnvl_get_status_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_TNVL_STATUS_PARAMS *params
)
{
    // Not supported in LR10
    return -NVL_ERR_NOT_SUPPORTED;
}

//
// This function auto creates the lr10 HAL connectivity from the NVSWITCH_INIT_HAL
// macro in haldef_nvswitch.h
//
// Note: All hal fns must be implemented for each chip.
//       There is no automatic stubbing here.
//
void nvswitch_setup_hal_lr10(nvswitch_device *device)
{
    device->chip_arch = NVSWITCH_GET_INFO_INDEX_ARCH_LR10;

    {
        device->chip_impl = NVSWITCH_GET_INFO_INDEX_IMPL_LR10;
    }

    NVSWITCH_INIT_HAL(device, lr10);
    NVSWITCH_INIT_HAL_LS10(device, lr10);                             
}
