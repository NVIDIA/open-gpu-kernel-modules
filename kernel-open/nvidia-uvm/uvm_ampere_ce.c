/*******************************************************************************
    Copyright (c) 2018-2023 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include "uvm_hal.h"
#include "uvm_hal_types.h"
#include "clc6b5.h"
#include "clc7b5.h"
#include "clc56f.h" // Needed because HAL ce_init pushes SET_OBJECT

bool uvm_hal_ampere_ce_method_is_valid_c6b5(uvm_push_t *push, NvU32 method_address, NvU32 method_data)
{
    if (!uvm_channel_is_proxy(push->channel))
        return true;

    switch (method_address) {
        case NVC56F_SET_OBJECT:
        case NVC6B5_SET_SEMAPHORE_A:
        case NVC6B5_SET_SEMAPHORE_B:
        case NVC6B5_SET_SEMAPHORE_PAYLOAD:
        case NVC6B5_SET_SRC_PHYS_MODE:
        case NVC6B5_SET_DST_PHYS_MODE:
        case NVC6B5_LAUNCH_DMA:
        case NVC6B5_OFFSET_IN_UPPER:
        case NVC6B5_OFFSET_IN_LOWER:
        case NVC6B5_OFFSET_OUT_UPPER:
        case NVC6B5_OFFSET_OUT_LOWER:
        case NVC6B5_LINE_LENGTH_IN:
        case NVC6B5_SET_REMAP_CONST_A:
        case NVC6B5_SET_REMAP_CONST_B:
        case NVC6B5_SET_REMAP_COMPONENTS:
            return true;
    }

    UVM_ERR_PRINT("Unsupported CE method 0x%x\n", method_address);
    return false;
}

static NvU32 ce_aperture(uvm_aperture_t aperture)
{
    BUILD_BUG_ON(HWCONST(C6B5, SET_SRC_PHYS_MODE, TARGET, LOCAL_FB) !=
                 HWCONST(C6B5, SET_DST_PHYS_MODE, TARGET, LOCAL_FB));
    BUILD_BUG_ON(HWCONST(C6B5, SET_SRC_PHYS_MODE, TARGET, COHERENT_SYSMEM) !=
                 HWCONST(C6B5, SET_DST_PHYS_MODE, TARGET, COHERENT_SYSMEM));
    BUILD_BUG_ON(HWCONST(C6B5, SET_SRC_PHYS_MODE, TARGET, PEERMEM) !=
                 HWCONST(C6B5, SET_DST_PHYS_MODE, TARGET, PEERMEM));

    if (aperture == UVM_APERTURE_SYS) {
        return HWCONST(C6B5, SET_SRC_PHYS_MODE, TARGET, COHERENT_SYSMEM);
    }
    else if (aperture == UVM_APERTURE_VID) {
        return HWCONST(C6B5, SET_SRC_PHYS_MODE, TARGET, LOCAL_FB);
    }
    else {
        return HWCONST(C6B5, SET_SRC_PHYS_MODE, TARGET, PEERMEM) |
               HWVALUE(C6B5, SET_SRC_PHYS_MODE, FLA, 0) |
               HWVALUE(C6B5, SET_SRC_PHYS_MODE, PEER_ID, UVM_APERTURE_PEER_ID(aperture));
    }
}

// Push SET_{SRC,DST}_PHYS mode if needed and return LAUNCH_DMA_{SRC,DST}_TYPE
// flags
NvU32 uvm_hal_ampere_ce_phys_mode(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src)
{
    NvU32 launch_dma_src_dst_type = 0;

    if (src.is_virtual)
        launch_dma_src_dst_type |= HWCONST(C6B5, LAUNCH_DMA, SRC_TYPE, VIRTUAL);
    else
        launch_dma_src_dst_type |= HWCONST(C6B5, LAUNCH_DMA, SRC_TYPE, PHYSICAL);

    if (dst.is_virtual)
        launch_dma_src_dst_type |= HWCONST(C6B5, LAUNCH_DMA, DST_TYPE, VIRTUAL);
    else
        launch_dma_src_dst_type |= HWCONST(C6B5, LAUNCH_DMA, DST_TYPE, PHYSICAL);

    if (!src.is_virtual && !dst.is_virtual) {
        NV_PUSH_2U(C6B5, SET_SRC_PHYS_MODE, ce_aperture(src.aperture),
                         SET_DST_PHYS_MODE, ce_aperture(dst.aperture));
    }
    else if (!src.is_virtual) {
        NV_PUSH_1U(C6B5, SET_SRC_PHYS_MODE, ce_aperture(src.aperture));
    }
    else if (!dst.is_virtual) {
        NV_PUSH_1U(C6B5, SET_DST_PHYS_MODE, ce_aperture(dst.aperture));
    }

    return launch_dma_src_dst_type;
}

NvU32 uvm_hal_ampere_ce_plc_mode_c7b5(void)
{
    return HWCONST(C7B5, LAUNCH_DMA, DISABLE_PLC, TRUE);
}

bool uvm_hal_ampere_ce_memcopy_is_valid_c6b5(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src)
{
    NvU64 push_begin_gpu_va;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    if (!uvm_parent_gpu_is_virt_mode_sriov_heavy(gpu->parent))
        return true;

    if (uvm_channel_is_proxy(push->channel)) {
        uvm_pushbuffer_t *pushbuffer;

        if (dst.is_virtual) {
            UVM_ERR_PRINT("Destination address of memcopy must be physical, not virtual\n");
            return false;
        }

        if (dst.aperture != UVM_APERTURE_VID) {
            UVM_ERR_PRINT("Destination address of memcopy must be in vidmem\n");
            return false;
        }

        // The source address is irrelevant, since it is a pushbuffer offset
        if (!IS_ALIGNED(dst.address, 8)){
            UVM_ERR_PRINT("Destination address of memcopy is not 8-byte aligned");
            return false;
        }

        if (!src.is_virtual) {
            UVM_ERR_PRINT("Source address of memcopy must be virtual\n");
            return false;
        }

        pushbuffer = uvm_channel_get_pushbuffer(push->channel);
        push_begin_gpu_va = uvm_pushbuffer_get_gpu_va_for_push(pushbuffer, push);

        if ((src.address < push_begin_gpu_va) || (src.address >= push_begin_gpu_va + uvm_push_get_size(push))) {
            UVM_ERR_PRINT("Source address of memcopy must point to pushbuffer\n");
            return false;
        }
    }
    else {
        // TODO: Bug 3429418: When in SR-IOV heavy, a memcopy/memset pushed to a
        // UVM internal channel cannot use peer physical addresses.
        if (!dst.is_virtual && !uvm_aperture_is_peer(dst.aperture)) {
            UVM_ERR_PRINT("Destination address of memcopy must be virtual, not physical (aperture: %s)\n",
                          uvm_gpu_address_aperture_string(dst));
            return false;
        }

        if (!src.is_virtual && !uvm_aperture_is_peer(src.aperture)) {
            UVM_ERR_PRINT("Source address of memcopy must be virtual, not physical (aperture: %s)\n",
                          uvm_gpu_address_aperture_string(src));
            return false;
        }
    }

    return true;
}

// In SR-IOV heavy (GA100 only), the UVM driver is expected to push a patched
// version of an inlined memcopy to the proxy channels. The patching consists in
// passing the offset of the inlined data within the push as the source virtual
// address, instead of passing its GPU VA.
//
// Copies pushed to internal channels use the GPU VA of the inlined data,
// irrespective of the virtualization mode.
void uvm_hal_ampere_ce_memcopy_patch_src_c6b5(uvm_push_t *push, uvm_gpu_address_t *src)
{
    uvm_pushbuffer_t *pushbuffer;

    if (!uvm_channel_is_proxy(push->channel))
        return;

    pushbuffer = uvm_channel_get_pushbuffer(push->channel);
    src->address -= uvm_pushbuffer_get_gpu_va_for_push(pushbuffer, push);
}

bool uvm_hal_ampere_ce_memset_is_valid_c6b5(uvm_push_t *push,
                                            uvm_gpu_address_t dst,
                                            size_t num_elements,
                                            size_t element_size)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    if (!uvm_parent_gpu_is_virt_mode_sriov_heavy(gpu->parent))
        return true;

    if (uvm_channel_is_proxy(push->channel)) {
        if (dst.is_virtual) {
            UVM_ERR_PRINT("Destination address of memset must be physical, not virtual\n");
            return false;
        }

        if (dst.aperture != UVM_APERTURE_VID) {
            UVM_ERR_PRINT("Destination address of memset must be in vidmem\n");
            return false;
        }

        if (!IS_ALIGNED(dst.address, 8)){
            UVM_ERR_PRINT("Destination address of memset is not 8-byte aligned");
            return false;
        }

        // Disallow memsets that don't match the page table/directory entry
        // size. PDE0 entries are 16 bytes wide, but those are written using a
        // memcopy.
        //
        // The memset size is not checked to be a multiple of the element size
        // because the check is not exclusive of SR-IOV heavy, and it is already
        // present in the uvm_hal_*_memset_* functions.
        if (element_size != 8) {
            UVM_ERR_PRINT("Memset data must be 8 bytes wide, but found %zu instead\n", element_size);
            return false;
        }
    }
    // TODO: Bug 3429418: When in SR-IOV heavy, a memcopy/memset pushed to a
    // UVM internal channel cannot use peer physical addresses.
    else if (!dst.is_virtual && !uvm_aperture_is_peer(dst.aperture)) {
        UVM_ERR_PRINT("Destination address of memset must be virtual, not physical (aperture: %s)\n",
                      uvm_gpu_address_aperture_string(dst));
        return false;
    }

    return true;
}
