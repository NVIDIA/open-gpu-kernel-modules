/*******************************************************************************
    Copyright (c) 2025 NVIDIA Corporation

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
#include "uvm_global.h"
#include "uvm_push.h"
#include "uvm_mem.h"
#include "uvm_conf_computing.h"

bool uvm_hal_blackwell_ce_memcopy_is_valid(uvm_push_t *push, uvm_gpu_address_t dst, uvm_gpu_address_t src)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    if (uvm_gpu_address_is_peer(gpu, src)) {
        UVM_ERR_PRINT("Peer copy from peer address (0x%llx) is not allowed!", src.address);
        return false;
    }

    if (push->channel && uvm_gpu_address_is_peer(gpu, dst) && !uvm_channel_is_p2p(push->channel)) {
        UVM_ERR_PRINT("Peer copy from address (0x%llx) to address (0x%llx) should use designated p2p channels!",
                      src.address,
                      dst.address);
        return false;
    }

    if (g_uvm_global.conf_computing_enabled) {
        // Blackwell+ GPUs support secure P2P. In that case, memcopy using
        // physical addresses is valid.
        if (!uvm_aperture_is_peer(dst.aperture)) {
            // In Confidential Computing, if a non-p2p memcopy uses physical
            // addressing for either the destination or the source, then the
            // corresponding aperture must be vidmem. If virtual addressing
            // is used, and the backing storage is sysmem the access is only
            // legal if the copy type is NONPROT2NONPROT, but the validation
            // does not detect it.
            if (!src.is_virtual && (src.aperture != UVM_APERTURE_VID))
                return false;

            if (!dst.is_virtual && dst.aperture != UVM_APERTURE_VID)
                return false;
        }

        // The source and destination must be both unprotected, for sysmem
        // copy, or both protected for p2p copy.
        if (dst.is_unprotected != src.is_unprotected)
            return false;
    }

    if (!gpu->parent->ce_phys_vidmem_write_supported && !dst.is_virtual && dst.aperture == UVM_APERTURE_VID) {
        UVM_ERR_PRINT("Destination address of vidmem memcopy must be virtual, not physical: {%s, 0x%llx}\n",
                      uvm_gpu_address_aperture_string(dst),
                      dst.address);
        return false;
    }

    return true;
}
