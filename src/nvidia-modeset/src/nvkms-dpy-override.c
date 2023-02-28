/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "nvkms-utils.h"
#include "nvkms-dpy-override.h"

#include "nv_list.h"

static NVListRec dpyOverrideList = NV_LIST_INIT(&dpyOverrideList);

#define FOR_ALL_DPY_OVERRIDES(_pDpyOverride) \
    nvListForEachEntry(_pDpyOverride, &dpyOverrideList, entry)
#define FOR_ALL_DPY_OVERRIDES_SAFE(_pDpyOverride, _next) \
    nvListForEachEntry_safe(_pDpyOverride, _next, &dpyOverrideList, entry)

#define DPY_OVERRIDE_MATCHES(_pDpyOverride, _gpuId, _name) \
    ((_pDpyOverride->gpuId == _gpuId) &&                   \
     !nvkms_strcmp(_pDpyOverride->name, _name))

static NvBool DpyOverrideReadEdid(NVDpyOverrideRec *dpy,
                                  const char *buffer,
                                  size_t size)
{
    if ((dpy->edid.length != size) || (dpy->edid.buffer == NULL)) {
        NvU8 *newbuf = nvRealloc(dpy->edid.buffer, size);
        if (newbuf == NULL) {
            return FALSE;
        }
        dpy->edid.buffer = newbuf;
        dpy->edid.length = size;
    }

    nvkms_memcpy(dpy->edid.buffer, buffer, size);
    return TRUE;
}


/*
 * Creates a display override on the given GPU for the given display name.
 * If the override already exists it will be overwritten.
 *
 * \param[in]   gpuId       The ID of the GPU on which to create the display
 *                          override, as returned in nvkms_enumerate_gpus()
 * \param[in]   name        The name of the display to override in
 *                          PROTOCOL-Index format, e.g. HDMI-0.
 * \param[in]   edid        A buffer containing EDID data for the override.
 * \param[in]   edidSize    The size of the edid buffer.
 *
 * \return  A pointer to the created or edited NVDpyOverrideRec, or NULL if
 *          creation failed.
 */
NVDpyOverrideRec *nvCreateDpyOverride(NvU32 gpuId,
                                      const char *name,
                                      NvBool connected,
                                      const char *edid,
                                      size_t edidSize)
{
    NVDpyOverridePtr pDpyOverride;
    size_t namelen, cpsz;
    NvBool found = FALSE;

    /* if such a display override already exists, let it be changed */
    FOR_ALL_DPY_OVERRIDES(pDpyOverride) {
        if (DPY_OVERRIDE_MATCHES(pDpyOverride, gpuId, name)) {

            found = TRUE;
            break;
        }
    }

    /* if such a display override doesn't exist, create a new one */
    if (!found) {
        pDpyOverride = nvCalloc(1, sizeof(*pDpyOverride));
        if (pDpyOverride == NULL) {
            nvEvoLog(EVO_LOG_WARN, "Failed allocating data for display override");
            return NULL;
        }

        nvListAdd(&pDpyOverride->entry, &dpyOverrideList);

        namelen = nvkms_strlen(name);
        cpsz = namelen > NVKMS_DPY_NAME_SIZE - 1 ? NVKMS_DPY_NAME_SIZE - 1
                                                 : namelen;

        nvkms_memcpy(pDpyOverride->name, name, cpsz);
        pDpyOverride->gpuId = gpuId;
    }

    pDpyOverride->connected = connected;
    if (connected && !DpyOverrideReadEdid(pDpyOverride, edid, edidSize)) {
        nvEvoLog(EVO_LOG_WARN, "Failed reading EDID");
        nvListDel(&pDpyOverride->entry);
        nvFree(pDpyOverride);
        return NULL;
    }

    return pDpyOverride;
}

/*
 * Deletes a display override on the given GPU for the given display name.
 *
 * \param[in]   gpuId       The ID of the GPU on which to delete the display
 *                          override, as returned in nvkms_enumerate_gpus()
 * \param[in]   name        The name of the display whose override to delete in
 *                          PROTOCOL-Index format, e.g. HDMI-0.
 */
void nvDeleteDpyOverride(NvU32 gpuId, const char *name)
{
    NVDpyOverridePtr pDpyOverride;

    /* If such a display override already exists, delete it */
    FOR_ALL_DPY_OVERRIDES(pDpyOverride) {
        if (DPY_OVERRIDE_MATCHES(pDpyOverride, gpuId, name)) {

            nvListDel(&pDpyOverride->entry);
            nvFree(pDpyOverride);
            return; /* This makes using nvListForEachEntry safe. */
        }
    }
}

/*
 * Logs a list of currently active override names to pInfoStr for a given
 * GPU.
 *
 * \param[in]   gpuId       The ID of the GPU whose overrides to print, as
 *                          returned in nvkms_enumerate_gpus()
 * \param[in]   pInfoStr    A pointer to the NVEvoInfoString to log to.
 */
void nvLogDpyOverrides(NvU32 gpuId, NVEvoInfoStringPtr pInfoStr)
{
    NVDpyOverridePtr pDpyOverride;
    FOR_ALL_DPY_OVERRIDES(pDpyOverride) {
        if (pDpyOverride->gpuId == gpuId) {
            nvEvoLogInfoString(pInfoStr, "%s", pDpyOverride->name);
        }
    }
}

/*
 * Checks if there is a matching, valid, and enabled NVDpyOverrideRec for the
 * pDpyEvo in the global display override list and returns it if it is found.
 * O(N) in length of display override list
 *
 * \param[in]   pDpyEvo         The display to check for an override.
 *
 * \return  The NVDpyOverrideRec override for the pDpyEvo, or NULL if it isn't
 *          found.
 */
NVDpyOverridePtr nvDpyEvoGetOverride(const NVDpyEvoRec *pDpyEvo)
{
    NVDevEvoPtr pDevEvo;
    NVDispEvoPtr pDispEvo;
    NVSubDeviceEvoPtr pSubDevice;
    NVDpyOverridePtr it;

    if (pDpyEvo == NULL) {
        return NULL;
    }

    /*
     * Don't override DP MST displays, because there could be multiple attached
     * to the single connector, which would result in the number of displays
     * this override creates being dependent on the number of plugged in
     * displays, which seems incorrect for this feature
     */
    if (nvDpyEvoIsDPMST(pDpyEvo)) {
        return NULL;
    }
    pDispEvo = pDpyEvo->pDispEvo;
    pDevEvo = pDispEvo->pDevEvo;

    pSubDevice = pDevEvo->pSubDevices[pDispEvo->displayOwner];

    FOR_ALL_DPY_OVERRIDES(it) {
        /* Ensure valid and enabled override */
        if ((it->edid.length == 0) || (it->edid.buffer == NULL)) {
            continue;
        }

        /*
         * Both NVDpyOverrideRec.gpuId and NVSubDeviceEvoRec.gpuId ultimately
         * derive from nvRmApiControl(NV2080_CTRL_CMD_GPU_GET_ID), so we can
         * use them to match GPUs. Additionally, NVConnectorEvo.name is of the
         * format TYPE-N, e.g. HDMI-0, but pDpyEvo.name may have additional
         * qualifiers (e.g., an existing EDID-derived name).
         */
        if (DPY_OVERRIDE_MATCHES(it, pSubDevice->gpuId, pDpyEvo->pConnectorEvo->name)) {

            nvEvoLogDebug(EVO_LOG_INFO, "NVDpyOverrideRec found: %s\n",
                          it->name);
            return it;
        }
    }

    return NULL;
}

/*
 * Reads the EDID data from a given NVDpyOverrideRec into the buffer buff.
 * Does not write to the buffer if the operation fails.
 *
 * \param[in]   pDpyOverride    The override from which to read the EDID data.
 * \param[out]  buff            A pointer to a buffer into which to read
 *                              the override's EDID data.
 * \param[in]   len             The length of the buffer.
 *
 * \return  The number of bytes written into the buffer, or 0 if the operation
 *          failed.
 */
size_t nvReadDpyOverrideEdid(const NVDpyOverrideRec *pDpyOverride,
                             NvU8 *buff, size_t len)
{
    if ((pDpyOverride == NULL) ||
        (buff == NULL) ||
        (pDpyOverride->edid.length == 0) ||
        (pDpyOverride->edid.length > len)) {
        return 0;
    }

    nvkms_memcpy(buff, pDpyOverride->edid.buffer,
                 pDpyOverride->edid.length);
    return pDpyOverride->edid.length;
}

/*
 * Delete all display overrides. This should only ever be called during shutdown
 * of NVKMS or to cleanup when display override initialization fails.
 */
void nvClearDpyOverrides(void)
{
    NVDpyOverridePtr pDpyOverride, tmp;
    FOR_ALL_DPY_OVERRIDES_SAFE(pDpyOverride, tmp) {
        nvListDel(&pDpyOverride->entry);
        if (pDpyOverride->edid.buffer != NULL) {
            nvFree(pDpyOverride->edid.buffer);
        }
        nvFree(pDpyOverride);
    }

    nvAssert(nvListIsEmpty(&dpyOverrideList));
}
