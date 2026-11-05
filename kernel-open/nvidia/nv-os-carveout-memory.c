/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
 *
 *   Description:
 *       Read carveout memory region from the device tree reserved-memory
 *       node. Used by mem_mgr.c on Linux.
 *
 *****************************************************************************/

#define  __NO_VERSION__

#include <nvstatus.h>
#include <nvtypes.h>

#include "nv-linux.h"

#include <linux/of.h>
#include <linux/of_reserved_mem.h>

/*
 * Device tree reserved-memory node used for the display scanout carveout.
 */
#define NV_CARVEOUT_RESERVED_MEM_NAME   "disp_scanout_carveout"

/*!
 * Reads carveout base and size from
 * /reserved-memory/<NV_CARVEOUT_RESERVED_MEM_NAME>.
 */
NV_STATUS NV_API_CALL
nv_get_carveout_info
(
    NvU64 *pBase,
    NvU64 *pSize
)
{
    struct device_node *reserved_mem_node;
    struct device_node *carveout_node;
    struct reserved_mem *rmem;

    *pBase = 0;
    *pSize = 0;

    reserved_mem_node = of_find_node_by_path("/reserved-memory");
    if (reserved_mem_node == NULL)
    {
        nv_printf(NV_DBG_INFO,
                  "NVRM: DT node \"/reserved-memory\" was not found\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    carveout_node = of_get_child_by_name(reserved_mem_node,
                                         NV_CARVEOUT_RESERVED_MEM_NAME);
    of_node_put(reserved_mem_node);
    if (carveout_node == NULL)
    {
        nv_printf(NV_DBG_INFO,
                  "NVRM: DT carveout lookup failed for node \"/reserved-memory/%s\"\n",
                  NV_CARVEOUT_RESERVED_MEM_NAME);
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!of_device_is_available(carveout_node))
    {
        nv_printf(NV_DBG_INFO,
                  "NVRM: DT carveout node \"/reserved-memory/%s\" is disabled\n",
                  NV_CARVEOUT_RESERVED_MEM_NAME);
        of_node_put(carveout_node);
        return NV_ERR_NOT_SUPPORTED;
    }

    rmem = of_reserved_mem_lookup(carveout_node);
    if (rmem == NULL)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: of_reserved_mem_lookup failed for node %s\n",
                  carveout_node->full_name ? carveout_node->full_name : "<unnamed>");
        of_node_put(carveout_node);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    *pBase = rmem->base;
    *pSize = rmem->size;

    of_node_put(carveout_node);

    if (*pSize == 0)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: DT carveout size is zero (base=0x%llx)\n",
                  *pBase);
        return NV_ERR_INVALID_DATA;
    }

    nv_printf(NV_DBG_INFO,
              "NVRM: DT carveout lookup succeeded for node \"%s\": base=0x%llx size=0x%llx\n",
              NV_CARVEOUT_RESERVED_MEM_NAME, *pBase, *pSize);

    return NV_OK;
}
