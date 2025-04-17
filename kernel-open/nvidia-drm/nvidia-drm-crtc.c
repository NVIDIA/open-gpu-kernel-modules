/*
 * Copyright (c) 2015-2022, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nvidia-drm-conftest.h" /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

#include "nvidia-drm-helper.h"
#include "nvidia-drm-priv.h"
#include "nvidia-drm-crtc.h"
#include "nvidia-drm-connector.h"
#include "nvidia-drm-encoder.h"
#include "nvidia-drm-utils.h"
#include "nvidia-drm-fb.h"
#include "nvidia-drm-ioctl.h"
#include "nvidia-drm-format.h"

#include "nvmisc.h"
#include "nv_common_utils.h"

#include <drm/drm_crtc_helper.h>
#include <drm/drm_plane_helper.h>

#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>

#if defined(NV_DRM_DRM_COLOR_MGMT_H_PRESENT)
#include <drm/drm_color_mgmt.h>
#endif

/*
 * The two arrays below specify the PQ EOTF transfer function that's used to
 * convert from PQ encoded L'M'S' fixed-point to linear LMS FP16. This transfer
 * function is the inverse of the OETF curve.
 *
 * TODO: Generate table with max number of entries for ILUT.
 */
static const NvU32 __eotf_pq_512_seg_sizes_log2[] = {
    6, 6, 4, 4, 4, 3, 4, 3, 3, 3, 2, 2, 2, 3, 3, 2,
    2, 2, 2, 2, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    6, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 1, 2,
    2, 1, 1, 2, 2, 2, 2, 1, 2, 1, 1, 2, 1, 4, 2, 2,
};
static const NvU16 __eotf_pq_512_entries[] = {
    0x0000, 0x0001, 0x0003, 0x0005, 0x0008, 0x000C, 0x0011, 0x0016, 0x001B, 0x0022, 0x0028, 0x002F, 0x0037, 0x003F, 0x0048, 0x0051,
    0x005A, 0x0064, 0x006F, 0x007A, 0x0085, 0x0091, 0x009E, 0x00AB, 0x00B8, 0x00C6, 0x00D4, 0x00E3, 0x00F3, 0x0102, 0x0113, 0x0123,
    0x0135, 0x0146, 0x0158, 0x016B, 0x017E, 0x0192, 0x01A6, 0x01BB, 0x01D0, 0x01E5, 0x01FC, 0x0212, 0x0229, 0x0241, 0x0259, 0x0272,
    0x028B, 0x02A4, 0x02BE, 0x02D9, 0x02F4, 0x0310, 0x032C, 0x0349, 0x0366, 0x0384, 0x03A2, 0x03C1, 0x03E0, 0x0400, 0x0421, 0x0442,
    0x0463, 0x0485, 0x04A8, 0x04CB, 0x04EF, 0x0513, 0x0538, 0x055D, 0x0583, 0x05AA, 0x05D1, 0x05F9, 0x0621, 0x064A, 0x0673, 0x069D,
    0x06C7, 0x06F3, 0x071E, 0x074B, 0x0777, 0x07A5, 0x07D3, 0x0801, 0x0819, 0x0830, 0x0849, 0x0861, 0x087A, 0x0893, 0x08AD, 0x08C7,
    0x08E1, 0x08FB, 0x0916, 0x0931, 0x094C, 0x0968, 0x0984, 0x09A0, 0x09BD, 0x09DA, 0x09F7, 0x0A15, 0x0A33, 0x0A51, 0x0A70, 0x0A8F,
    0x0AAE, 0x0ACE, 0x0AEE, 0x0B0E, 0x0B2F, 0x0B50, 0x0B71, 0x0B93, 0x0BB5, 0x0BD7, 0x0BFA, 0x0C0F, 0x0C20, 0x0C32, 0x0C44, 0x0C56,
    0x0C69, 0x0CB5, 0x0D03, 0x0D55, 0x0DA9, 0x0E01, 0x0E5B, 0x0EB9, 0x0F1B, 0x0F7F, 0x0FE7, 0x1029, 0x1061, 0x109A, 0x10D5, 0x1111,
    0x1150, 0x1190, 0x11D3, 0x1217, 0x125E, 0x12A6, 0x12F0, 0x133D, 0x138B, 0x13DC, 0x1417, 0x1442, 0x146D, 0x149A, 0x14C8, 0x14F7,
    0x1527, 0x1558, 0x158B, 0x15BF, 0x15F4, 0x162A, 0x1662, 0x169B, 0x16D5, 0x1711, 0x174E, 0x178C, 0x17CC, 0x1806, 0x1828, 0x184A,
    0x186D, 0x18B4, 0x18FF, 0x194D, 0x199E, 0x19F3, 0x1A4B, 0x1AA7, 0x1B06, 0x1B37, 0x1B69, 0x1B9B, 0x1BCF, 0x1C02, 0x1C1D, 0x1C38,
    0x1C54, 0x1C70, 0x1C8D, 0x1CAB, 0x1CC9, 0x1CE7, 0x1D06, 0x1D26, 0x1D46, 0x1D88, 0x1DCC, 0x1E13, 0x1E5C, 0x1EA8, 0x1EF6, 0x1F47,
    0x1F9A, 0x1FF1, 0x2025, 0x2053, 0x2082, 0x20B3, 0x20E6, 0x211A, 0x214F, 0x2187, 0x21C0, 0x21FA, 0x2237, 0x2275, 0x22B5, 0x22F7,
    0x233B, 0x23C9, 0x2430, 0x247F, 0x24D3, 0x252B, 0x2589, 0x25EB, 0x2653, 0x26C1, 0x2734, 0x27AD, 0x2817, 0x2838, 0x285A, 0x287C,
    0x28A0, 0x28C5, 0x28EA, 0x2911, 0x2938, 0x2960, 0x298A, 0x29B4, 0x29DF, 0x2A0C, 0x2A39, 0x2A68, 0x2A98, 0x2AFA, 0x2B62, 0x2BCE,
    0x2C20, 0x2C5B, 0x2C99, 0x2CDA, 0x2D1E, 0x2D65, 0x2DB0, 0x2DFD, 0x2E4E, 0x2EA3, 0x2EFC, 0x2F58, 0x2FB8, 0x300E, 0x3043, 0x307A,
    0x30B3, 0x30D0, 0x30EE, 0x310D, 0x312C, 0x314C, 0x316D, 0x318E, 0x31B0, 0x31D3, 0x31F6, 0x321A, 0x323F, 0x3265, 0x328B, 0x32B2,
    0x32DA, 0x332D, 0x3383, 0x33DC, 0x341D, 0x344D, 0x347F, 0x34B4, 0x34EA, 0x3523, 0x355E, 0x359B, 0x35DB, 0x361D, 0x3662, 0x36A9,
    0x36F3, 0x3740, 0x3791, 0x37E4, 0x381D, 0x384A, 0x3879, 0x38A9, 0x38DB, 0x3910, 0x3946, 0x397E, 0x39B8, 0x39F5, 0x3A34, 0x3A75,
    0x3AB9, 0x3AFF, 0x3B48, 0x3B94, 0x3BE2, 0x3C1A, 0x3C44, 0x3C70, 0x3C9D, 0x3CA0, 0x3CA3, 0x3CA6, 0x3CA9, 0x3CAC, 0x3CAF, 0x3CB1,
    0x3CB4, 0x3CB7, 0x3CBA, 0x3CBD, 0x3CC0, 0x3CC3, 0x3CC6, 0x3CC9, 0x3CCC, 0x3CCF, 0x3CD2, 0x3CD5, 0x3CD8, 0x3CDB, 0x3CDE, 0x3CE1,
    0x3CE4, 0x3CE7, 0x3CEA, 0x3CEE, 0x3CF1, 0x3CF4, 0x3CF7, 0x3CFA, 0x3CFD, 0x3D00, 0x3D03, 0x3D06, 0x3D09, 0x3D0D, 0x3D10, 0x3D13,
    0x3D16, 0x3D19, 0x3D1C, 0x3D20, 0x3D23, 0x3D26, 0x3D29, 0x3D2C, 0x3D30, 0x3D33, 0x3D36, 0x3D39, 0x3D3D, 0x3D40, 0x3D43, 0x3D46,
    0x3D4A, 0x3D4D, 0x3D50, 0x3D54, 0x3D57, 0x3D5A, 0x3D5D, 0x3D61, 0x3D64, 0x3D9B, 0x3DD3, 0x3E0D, 0x3E4A, 0x3E89, 0x3ECA, 0x3F0E,
    0x3F54, 0x3F9C, 0x3FE8, 0x401B, 0x4043, 0x406D, 0x4099, 0x40C6, 0x40F4, 0x4124, 0x4156, 0x418A, 0x41C0, 0x41F8, 0x4232, 0x426D,
    0x42AB, 0x42EB, 0x432E, 0x4373, 0x43BA, 0x4428, 0x4479, 0x44D0, 0x452D, 0x4591, 0x45FC, 0x466F, 0x46EB, 0x472C, 0x476F, 0x47B5,
    0x47FE, 0x4824, 0x484B, 0x4874, 0x489D, 0x48F5, 0x4954, 0x4986, 0x49B9, 0x49EF, 0x4A26, 0x4A5F, 0x4A9B, 0x4AD9, 0x4B19, 0x4B9F,
    0x4C18, 0x4C66, 0x4CBA, 0x4CE6, 0x4D13, 0x4D43, 0x4D74, 0x4DA7, 0x4DDC, 0x4E12, 0x4E4B, 0x4E86, 0x4EC3, 0x4F02, 0x4F44, 0x4F88,
    0x4FCE, 0x500C, 0x5032, 0x5082, 0x50D8, 0x5106, 0x5135, 0x5166, 0x5199, 0x5205, 0x5278, 0x52F5, 0x537C, 0x53C3, 0x5406, 0x542D,
    0x5454, 0x54A9, 0x5503, 0x550F, 0x551B, 0x5527, 0x5533, 0x5540, 0x554C, 0x5559, 0x5565, 0x5572, 0x557F, 0x558C, 0x5599, 0x55A7,
    0x55B4, 0x55C1, 0x55CF, 0x5607, 0x5641, 0x567E, 0x56BC, 0x56FE, 0x5741, 0x5788, 0x57D0,
};

/*
 * The two arrays below specify the PQ OETF transfer function that's used to
 * convert from linear LMS FP16 to PQ encoded L'M'S' fixed-point.
 *
 * TODO: Generate table with max number of entries for ILUT.
 */
static const NvU32 __oetf_pq_512_seg_sizes_log2[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3,
    3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5,
    5,
};
static const NvU16 __oetf_pq_512_entries[] = {
    0x0000, 0x000C, 0x0014, 0x001C, 0x0028, 0x003C, 0x005C, 0x008C, 0x00D0, 0x0134, 0x0184, 0x01C8, 0x0238, 0x029C, 0x033C, 0x03C4,
    0x043C, 0x04A4, 0x0504, 0x0560, 0x0600, 0x0690, 0x0714, 0x078C, 0x07FC, 0x0864, 0x08C8, 0x0924, 0x0980, 0x09D4, 0x0A24, 0x0A70,
    0x0B04, 0x0B90, 0x0C10, 0x0C88, 0x0CFC, 0x0D68, 0x0DD4, 0x0E38, 0x0EF4, 0x0FA4, 0x1048, 0x10E4, 0x1174, 0x1200, 0x1284, 0x1304,
    0x13F4, 0x14D0, 0x159C, 0x165C, 0x1714, 0x17C0, 0x1864, 0x1900, 0x1A28, 0x1B34, 0x1C30, 0x1D1C, 0x1DFC, 0x1ECC, 0x1F94, 0x2050,
    0x2104, 0x21B0, 0x2258, 0x22F8, 0x2390, 0x2424, 0x24B4, 0x2540, 0x25C4, 0x2648, 0x26C4, 0x2740, 0x27B8, 0x282C, 0x289C, 0x290C,
    0x29E0, 0x2AAC, 0x2B70, 0x2C2C, 0x2CE0, 0x2D90, 0x2E38, 0x2ED8, 0x2F74, 0x300C, 0x30A0, 0x3130, 0x31BC, 0x3244, 0x32C8, 0x3348,
    0x3440, 0x352C, 0x360C, 0x36E4, 0x37B4, 0x387C, 0x393C, 0x39F8, 0x3AA8, 0x3B58, 0x3C00, 0x3CA4, 0x3D44, 0x3DDC, 0x3E74, 0x3F04,
    0x401C, 0x4128, 0x4228, 0x431C, 0x4408, 0x44E8, 0x45C4, 0x4694, 0x475C, 0x4820, 0x48DC, 0x4994, 0x4A48, 0x4AF4, 0x4B9C, 0x4C3C,
    0x4D78, 0x4EA0, 0x4FBC, 0x50CC, 0x51D0, 0x52CC, 0x53BC, 0x54A0, 0x5580, 0x5658, 0x5728, 0x57F0, 0x58B4, 0x5974, 0x5A2C, 0x5ADC,
    0x5C34, 0x5D7C, 0x5EB4, 0x5FDC, 0x60F4, 0x6204, 0x630C, 0x6404, 0x64F8, 0x65E0, 0x66C4, 0x679C, 0x6870, 0x693C, 0x6A04, 0x6AC4,
    0x6C38, 0x6D94, 0x6EE4, 0x7020, 0x7150, 0x7274, 0x738C, 0x7498, 0x7598, 0x7694, 0x7784, 0x786C, 0x794C, 0x7A24, 0x7AF8, 0x7BC4,
    0x7D50, 0x7EC4, 0x8024, 0x8174, 0x82B4, 0x83E8, 0x850C, 0x8628, 0x8738, 0x883C, 0x8938, 0x8A2C, 0x8B18, 0x8BFC, 0x8CD8, 0x8DB0,
    0x8F4C, 0x90D0, 0x9240, 0x939C, 0x94EC, 0x962C, 0x975C, 0x9880, 0x999C, 0x9AAC, 0x9BB0, 0x9CAC, 0x9DA0, 0x9E8C, 0x9F70, 0xA04C,
    0xA1F4, 0xA384, 0xA500, 0xA664, 0xA7BC, 0xA904, 0xAA3C, 0xAB6C, 0xAC8C, 0xADA0, 0xAEAC, 0xAFAC, 0xB0A4, 0xB194, 0xB27C, 0xB360,
    0xB510, 0xB6A4, 0xB824, 0xB994, 0xBAF0, 0xBC3C, 0xBD78, 0xBEA8, 0xBFCC, 0xC0E4, 0xC1F0, 0xC2F4, 0xC3F0, 0xC4E4, 0xC5CC, 0xC6B0,
    0xC78C, 0xC860, 0xC930, 0xC9F8, 0xCABC, 0xCB7C, 0xCC38, 0xCCEC, 0xCD9C, 0xCE48, 0xCEF0, 0xCF94, 0xD034, 0xD0D4, 0xD16C, 0xD200,
    0xD294, 0xD324, 0xD3B4, 0xD43C, 0xD4C4, 0xD54C, 0xD5CC, 0xD650, 0xD6CC, 0xD748, 0xD7C4, 0xD83C, 0xD8B0, 0xD924, 0xD994, 0xDA08,
    0xDAE0, 0xDBB4, 0xDC84, 0xDD4C, 0xDE10, 0xDECC, 0xDF84, 0xE038, 0xE0E8, 0xE194, 0xE238, 0xE2DC, 0xE37C, 0xE418, 0xE4B0, 0xE544,
    0xE5D4, 0xE664, 0xE6F0, 0xE778, 0xE800, 0xE884, 0xE904, 0xE984, 0xEA00, 0xEA7C, 0xEAF4, 0xEB68, 0xEBDC, 0xEC50, 0xECC0, 0xED30,
    0xEE08, 0xEED8, 0xEFA4, 0xF068, 0xF128, 0xF1E4, 0xF298, 0xF348, 0xF3F4, 0xF49C, 0xF540, 0xF5E0, 0xF67C, 0xF714, 0xF7A8, 0xF83C,
    0xF8CC, 0xF958, 0xF9E0, 0xFA68, 0xFAEC, 0xFB6C, 0xFBE8, 0xFC64, 0xFCE0, 0xFD58, 0xFDCC, 0xFE40, 0xFEB4, 0xFF24, 0xFF90, 0xFFFF,
};

#define NUM_VSS_HEADER_ENTRIES (NVKMS_LUT_VSS_HEADER_SIZE / NVKMS_LUT_CAPS_LUT_ENTRY_SIZE)

static int
nv_drm_atomic_replace_property_blob_from_id(struct drm_device *dev,
                                            struct drm_property_blob **blob,
                                            uint64_t blob_id,
                                            ssize_t expected_size,
                                            NvBool *replaced)
{
    struct drm_property_blob *old_blob = *blob;
    struct drm_property_blob *new_blob = NULL;

    if (blob_id != 0) {
        new_blob = drm_property_lookup_blob(dev, blob_id);
        if (new_blob == NULL) {
            return -EINVAL;
        }

        if ((expected_size > 0) &&
            (new_blob->length != expected_size)) {
            nv_drm_property_blob_put(new_blob);
            return -EINVAL;
        }
    }

    if (old_blob != new_blob) {
        nv_drm_property_blob_put(old_blob);
        if (new_blob) {
            nv_drm_property_blob_get(new_blob);
        }
        *blob = new_blob;
        *replaced = true;
    } else {
        *replaced = false;
    }

    nv_drm_property_blob_put(new_blob);

    return 0;
}

static void nv_drm_plane_destroy(struct drm_plane *plane)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);

    /* plane->state gets freed here */
    drm_plane_cleanup(plane);

    nv_drm_free(nv_plane);
}

static inline void
plane_config_clear(struct NvKmsKapiLayerConfig *layerConfig)
{
    if (layerConfig == NULL) {
        return;
    }

    memset(layerConfig, 0, sizeof(*layerConfig));
    layerConfig->csc = NVKMS_IDENTITY_CSC_MATRIX;
}

static inline void
plane_req_config_disable(struct NvKmsKapiLayerRequestedConfig *req_config)
{
    /* Clear layer config */
    plane_config_clear(&req_config->config);

    /* Set flags to get cleared layer config applied */
    req_config->flags.surfaceChanged = NV_TRUE;
    req_config->flags.srcXYChanged = NV_TRUE;
    req_config->flags.srcWHChanged = NV_TRUE;
    req_config->flags.dstXYChanged = NV_TRUE;
    req_config->flags.dstWHChanged = NV_TRUE;
}

static inline void
cursor_req_config_disable(struct NvKmsKapiCursorRequestedConfig *req_config)
{
    req_config->surface = NULL;
    req_config->flags.surfaceChanged = NV_TRUE;
}

static NvU64 ctm_val_to_csc_val(NvU64 ctm_val)
{
    /*
     * Values in the CTM are encoded in S31.32 sign-magnitude fixed-
     * point format, while NvKms CSC values are signed 2's-complement
     * S15.16 (Ssign-extend12-3.16?) fixed-point format.
     */
    NvU64 sign_bit = ctm_val & (1ULL << 63);
    NvU64 magnitude = ctm_val & ~sign_bit;

    /*
     * Drop the low 16 bits of the fractional part and the high 17 bits
     * of the integral part. Drop 17 bits to avoid corner cases where
     * the highest resulting bit is a 1, causing the `cscVal = -cscVal`
     * line to result in a positive number.
     *
     * NOTE: Upstream precedent is to clamp to the range supported by hardware.
     * Here, we truncate the integral part to 14 bits, and will later truncate
     * further to the 3-5 bits supported by hardware within the display HAL.
     *
     * TODO: Clamping would be better, in the rare event that we receive
     * coefficients that are large enough for it to matter.
     */
    NvS32 csc_val = (magnitude >> 16) & ((1ULL << 31) - 1);
    if (sign_bit) {
        csc_val = -csc_val;
    }

    return csc_val;
}

#if defined(NV_DRM_COLOR_MGMT_AVAILABLE)
static void ctm_to_csc(struct NvKmsCscMatrix *nvkms_csc,
                       struct drm_color_ctm  *drm_ctm)
{
    int y;

    /* CTM is a 3x3 matrix while ours is 3x4. Zero out the last column. */
    nvkms_csc->m[0][3] = nvkms_csc->m[1][3] = nvkms_csc->m[2][3] = 0;

    for (y = 0; y < 3; y++) {
        int x;

        for (x = 0; x < 3; x++) {
            nvkms_csc->m[y][x] = ctm_val_to_csc_val(drm_ctm->matrix[y*3 + x]);
        }
    }
}
#endif /* NV_DRM_COLOR_MGMT_AVAILABLE */

static void ctm_3x4_to_csc(struct NvKmsCscMatrix    *nvkms_csc,
                           struct drm_color_ctm_3x4 *drm_ctm_3x4)
{
    int y;

    for (y = 0; y < 3; y++) {
        int x;

        for (x = 0; x < 4; x++) {
            nvkms_csc->m[y][x] =
                ctm_val_to_csc_val(drm_ctm_3x4->matrix[y*4 + x]);
        }
    }
}

static void
cursor_plane_req_config_update(struct drm_plane *plane,
                               struct drm_plane_state *plane_state,
                               struct NvKmsKapiCursorRequestedConfig *req_config)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    struct NvKmsKapiCursorRequestedConfig old_config = *req_config;

    if (plane_state->fb == NULL) {
        cursor_req_config_disable(req_config);
        return;
    }

    memset(req_config, 0, sizeof(*req_config));
    req_config->surface = to_nv_framebuffer(plane_state->fb)->pSurface;
    req_config->dstX = plane_state->crtc_x;
    req_config->dstY = plane_state->crtc_y;

#if defined(NV_DRM_ALPHA_BLENDING_AVAILABLE)
    if (plane->blend_mode_property != NULL && plane->alpha_property != NULL) {

        switch (plane_state->pixel_blend_mode) {
            case DRM_MODE_BLEND_PREMULTI:
                req_config->compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA;
                break;
            case DRM_MODE_BLEND_COVERAGE:
                req_config->compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA;
                break;
            default:
                /*
                 * We should not hit this, because
                 * plane_state->pixel_blend_mode should only have values
                 * registered in
                 * __nv_drm_plane_create_alpha_blending_properties().
                 */
                WARN_ON("Unsupported blending mode");
                break;

        }

        req_config->compParams.surfaceAlpha =
            plane_state->alpha >> 8;

    } else if (plane->blend_mode_property != NULL) {

        switch (plane_state->pixel_blend_mode) {
            case DRM_MODE_BLEND_PREMULTI:
                req_config->compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA;
                break;
            case DRM_MODE_BLEND_COVERAGE:
                req_config->compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA;
                break;
            default:
                /*
                 * We should not hit this, because
                 * plane_state->pixel_blend_mode should only have values
                 * registered in
                 * __nv_drm_plane_create_alpha_blending_properties().
                 */
                WARN_ON("Unsupported blending mode");
                break;

        }

    } else {
        req_config->compParams.compMode =
            nv_plane->defaultCompositionMode;
    }
#else
    req_config->compParams.compMode = nv_plane->defaultCompositionMode;
#endif

    /*
     * Unconditionally mark the surface as changed, even if nothing changed,
     * so that we always get a flip event: a DRM client may flip with
     * the same surface and wait for a flip event.
     */
    req_config->flags.surfaceChanged = NV_TRUE;

    if (old_config.surface == NULL &&
        old_config.surface != req_config->surface) {
        req_config->flags.dstXYChanged = NV_TRUE;
        return;
    }

    req_config->flags.dstXYChanged =
        old_config.dstX != req_config->dstX ||
        old_config.dstY != req_config->dstY;
}

static void release_drm_nvkms_surface(struct nv_drm_nvkms_surface *drm_nvkms_surface)
{
    struct NvKmsKapiDevice *pDevice = drm_nvkms_surface->pDevice;

    BUG_ON(drm_nvkms_surface->nvkms_surface == NULL);
    BUG_ON(drm_nvkms_surface->nvkms_memory == NULL);
    BUG_ON(drm_nvkms_surface->buffer == NULL);

    nvKms->destroySurface(pDevice, drm_nvkms_surface->nvkms_surface);
    nvKms->unmapMemory(pDevice, drm_nvkms_surface->nvkms_memory,
                       NVKMS_KAPI_MAPPING_TYPE_KERNEL,
                       drm_nvkms_surface->buffer);
    nvKms->freeMemory(pDevice, drm_nvkms_surface->nvkms_memory);
}

static int init_drm_nvkms_surface(struct nv_drm_device *nv_dev,
    struct nv_drm_nvkms_surface *drm_nvkms_surface,
    struct nv_drm_nvkms_surface_params *surface_params)
{
    struct NvKmsKapiDevice *pDevice = nv_dev->pDevice;
    NvU8 compressible = 0; // No compression

    struct NvKmsKapiCreateSurfaceParams params = {};
    struct NvKmsKapiMemory *surface_mem;
    struct NvKmsKapiSurface *surface;
    void *buffer;

    params.format = surface_params->format;
    params.width = surface_params->width;
    params.height = surface_params->height;

    /* Allocate displayable memory. */
    if (nv_dev->hasVideoMemory) {
        surface_mem =
            nvKms->allocateVideoMemory(pDevice,
                                       NvKmsSurfaceMemoryLayoutPitch,
                                       NVKMS_KAPI_ALLOCATION_TYPE_SCANOUT,
                                       surface_params->surface_size,
                                       &compressible);
    } else {
        surface_mem =
            nvKms->allocateSystemMemory(pDevice,
                                        NvKmsSurfaceMemoryLayoutPitch,
                                        NVKMS_KAPI_ALLOCATION_TYPE_SCANOUT,
                                        surface_params->surface_size,
                                        &compressible);
    }
    if (surface_mem == NULL) {
        return -ENOMEM;
    }

    /* Map memory in order to populate it. */
    if (!nvKms->mapMemory(pDevice, surface_mem,
                          NVKMS_KAPI_MAPPING_TYPE_KERNEL,
                          &buffer)) {
        nvKms->freeMemory(pDevice, surface_mem);
        return -ENOMEM;
    }

    params.planes[0].memory = surface_mem;
    params.planes[0].offset = 0;
    params.planes[0].pitch = surface_params->surface_size;

    /* Create surface. */
    surface = nvKms->createSurface(pDevice, &params);
    if (surface == NULL) {
        nvKms->unmapMemory(pDevice, surface_mem,
                           NVKMS_KAPI_MAPPING_TYPE_KERNEL, buffer);
        nvKms->freeMemory(pDevice, surface_mem);
        return -ENOMEM;
    }

    /* Pack into struct nv_drm_nvkms_surface. */
    drm_nvkms_surface->pDevice = pDevice;
    drm_nvkms_surface->nvkms_memory = surface_mem;
    drm_nvkms_surface->nvkms_surface = surface;
    drm_nvkms_surface->buffer = buffer;

    /* Init refcount. */
    kref_init(&drm_nvkms_surface->refcount);

    return 0;
}

static struct nv_drm_lut_surface *alloc_drm_lut_surface(
    struct nv_drm_device *nv_dev,
    enum NvKmsLUTFormat entry_format,
    enum NvKmsLUTVssType vss_type,
    NvU32 num_vss_header_segments,
    NvU32 num_vss_header_entries,
    NvU32 num_entries)
{
    struct nv_drm_lut_surface *drm_lut_surface;
    const size_t surface_size =
        (((num_vss_header_entries + num_entries) *
          NVKMS_LUT_CAPS_LUT_ENTRY_SIZE) + 255) & ~255; // 256-byte aligned

    struct nv_drm_nvkms_surface_params params = {};

    params.format = NvKmsSurfaceMemoryFormatR16G16B16A16;
    params.width = num_vss_header_entries + num_entries;
    params.height = 1;
    params.surface_size = surface_size;

    drm_lut_surface = nv_drm_calloc(1, sizeof(struct nv_drm_lut_surface));
    if (drm_lut_surface == NULL) {
        return NULL;
    }

    if (init_drm_nvkms_surface(nv_dev, &drm_lut_surface->base, &params) != 0) {
        nv_drm_free(drm_lut_surface);
        return NULL;
    }

    drm_lut_surface->properties.vssSegments = num_vss_header_segments;
    drm_lut_surface->properties.vssType = vss_type;
    drm_lut_surface->properties.lutEntries = num_entries;
    drm_lut_surface->properties.entryFormat = entry_format;

    return drm_lut_surface;
}

static void free_drm_lut_surface(struct kref *ref)
{
    struct nv_drm_nvkms_surface *drm_nvkms_surface =
        container_of(ref, struct nv_drm_nvkms_surface, refcount);
    struct nv_drm_lut_surface *drm_lut_surface =
        container_of(drm_nvkms_surface, struct nv_drm_lut_surface, base);

    // Clean up base
    release_drm_nvkms_surface(drm_nvkms_surface);

    nv_drm_free(drm_lut_surface);
}

static NvU32 fp32_lut_interp(
    NvU16 entry0,
    NvU16 entry1,
    NvU32 interp,
    NvU32 interp_max)
{
    NvU32 fp32_entry0 = nvKmsKapiUI32ToF32((NvU32) entry0);
    NvU32 fp32_entry1 = nvKmsKapiUI32ToF32((NvU32) entry1);

    NvU32 fp32_num0  = nvKmsKapiUI32ToF32(interp_max - interp);
    NvU32 fp32_num1  = nvKmsKapiUI32ToF32(interp);
    NvU32 fp32_denom = nvKmsKapiUI32ToF32(interp_max);

    fp32_entry0 = nvKmsKapiF32Mul(fp32_entry0, fp32_num0);
    fp32_entry0 = nvKmsKapiF32Div(fp32_entry0, fp32_denom);

    fp32_entry1 = nvKmsKapiF32Mul(fp32_entry1, fp32_num1);
    fp32_entry1 = nvKmsKapiF32Div(fp32_entry1, fp32_denom);

    return nvKmsKapiF32Add(fp32_entry0, fp32_entry1);
}

static struct nv_drm_lut_surface *create_drm_ilut_surface_vss(
    struct nv_drm_device *nv_dev,
    struct nv_drm_plane *nv_plane,
    struct nv_drm_plane_state *nv_drm_plane_state)
{
    static const NvU32 fp_norm  = 0x42FA0000; // FP32 125.0
    static const NvU32 u10_norm = 0x447FC000; // FP32 1023.0
    static const NvU32 u16_norm = 0x477FFF00; // FP32 UINT16_MAX
    // FP32 UINT32_MAX (Precision limited to 2^32)
    static const NvU32 u32_norm = 0x4F800000;

    struct nv_drm_lut_surface *drm_lut_surface;

    NvU32 entry_idx;
    NvU32 num_entries;
    NvU16 *lut_data;

    const NvU32 *vss_header_seg_sizes = NULL;
    NvU32 num_vss_header_segments = 0;
    const NvU16 *vss_entries = NULL;
    enum NvKmsLUTVssType vss_type = NVKMS_LUT_VSS_TYPE_NONE;

    NvBool multiply = false;
    NvU32 fp32_multiplier;

    WARN_ON(!nv_plane->ilut_caps.supported);
    WARN_ON(nv_plane->ilut_caps.entryFormat != NVKMS_LUT_FORMAT_FP16);
    WARN_ON(nv_plane->ilut_caps.vssSupport != NVKMS_LUT_VSS_SUPPORTED);
    WARN_ON(nv_plane->ilut_caps.vssType != NVKMS_LUT_VSS_TYPE_LINEAR);

    /* Convert multiplier from S31.32 Sign-Magnitude format to FP32. */
    if (nv_drm_plane_state->degamma_multiplier != (((NvU64) 1) << 32)) {
        NvU32 upper = (NvU32) (nv_drm_plane_state->degamma_multiplier >> 32);
        NvU32 lower = (NvU32) nv_drm_plane_state->degamma_multiplier;

        /* Range property is configured to ensure sign bit = 0. */
        WARN_ON(nv_drm_plane_state->degamma_multiplier & (((NvU64) 1) << 63));

        fp32_multiplier =
            nvKmsKapiF32Add(
                nvKmsKapiUI32ToF32(upper),
                nvKmsKapiF32Div(nvKmsKapiUI32ToF32(lower), u32_norm));

        multiply = true;
    }

    /* Determine configuration based on specified EOTF. */
    if (nv_drm_plane_state->degamma_tf == NV_DRM_TRANSFER_FUNCTION_PQ) {
        /* Need VSS for PQ. */
        vss_header_seg_sizes = __eotf_pq_512_seg_sizes_log2;
        num_vss_header_segments = ARRAY_LEN(__eotf_pq_512_seg_sizes_log2);
        vss_type = NVKMS_LUT_VSS_TYPE_LINEAR;

        vss_entries = __eotf_pq_512_entries;
        num_entries = ARRAY_LEN(__eotf_pq_512_entries) + 1;
    } else {
        WARN_ON((nv_drm_plane_state->degamma_tf != NV_DRM_TRANSFER_FUNCTION_DEFAULT) &&
                (nv_drm_plane_state->degamma_tf != NV_DRM_TRANSFER_FUNCTION_LINEAR));

        num_entries = NVKMS_LUT_ARRAY_SIZE + 1;
    }
    WARN_ON((vss_entries != NULL) &&
            (num_vss_header_segments != nv_plane->ilut_caps.vssSegments));
    WARN_ON((vss_entries != NULL) && (num_entries > nv_plane->ilut_caps.lutEntries));
    WARN_ON((vss_entries == NULL) && (num_entries != nv_plane->ilut_caps.lutEntries));

    /*
     * Allocate displayable LUT surface.
     * Space for the VSS header must be included even for non-VSS LUTs.
     */
    drm_lut_surface =
        alloc_drm_lut_surface(nv_dev,
                              NVKMS_LUT_FORMAT_FP16,
                              vss_type,
                              num_vss_header_segments,
                              NUM_VSS_HEADER_ENTRIES,
                              num_entries);
    if (!drm_lut_surface) {
        return NULL;
    }

    lut_data = (NvU16 *) drm_lut_surface->base.buffer;

    /* Calculate VSS header. */
    if (vss_header_seg_sizes != NULL) {
        for (entry_idx = 0; entry_idx < NUM_VSS_HEADER_ENTRIES; entry_idx++) {
            int i;
            NvU64 vss_header_entry = 0;
            for (i = 0; (i < 16) &&
                        (((entry_idx * 16) + i) < num_vss_header_segments); i++) {
                vss_header_entry |=
                    ((NvU64) vss_header_seg_sizes[(entry_idx * 16) + i]) << (i * 3);
            }
            ((NvU64 *) lut_data)[entry_idx] = vss_header_entry;
        }
    }

    /* Calculate LUT content. */
    for (entry_idx = 0;
         entry_idx < num_entries - 1; entry_idx++) {
        NvU32 fp32_r, fp32_g, fp32_b;
        NvU32 data_idx = entry_idx + NUM_VSS_HEADER_ENTRIES;

        if (nv_drm_plane_state->degamma_lut != NULL) {
            /* Use provided Degamma LUT. */
            static const NvU32 interp_max = (((NvU32) 1) << (32 - 10)) - 1;

            const struct drm_color_lut *degamma_lut =
                (struct drm_color_lut *) nv_drm_plane_state->degamma_lut->data;

            NvU16 lut_idx;
            NvU32 interp = 0;

            if (vss_entries != NULL) {
                /* Merge with provided VSS LUT. */
                NvU16 fp16_entry = vss_entries[entry_idx];

                /* Convert from FP16 to UNORM32. */
                // TODO: Use pre-UNORM32-normalized VSS LUT table?
                NvU32 unorm32_entry =
                    nvKmsKapiF32ToUI32RMinMag(
                        nvKmsKapiF32Mul(
                            nvKmsKapiF32Div(nvKmsKapiF16ToF32(fp16_entry),
                                            fp_norm),
                            u32_norm),
                        false);

                /* Index using upper 10 bits from UNORM32 VSS LUT. */
                lut_idx = unorm32_entry >> (32 - 10);
                /* Interpolate using lower 22 bits from UNORM32 VSS LUT. */
                interp = unorm32_entry & interp_max;
            } else {
                /* Direct index. */
                lut_idx = entry_idx;
            }

            BUG_ON(lut_idx >= NVKMS_LUT_ARRAY_SIZE);

            /* Perform interpolation or direct indexing. */
            if (interp > 0 && ((lut_idx + 1) < NVKMS_LUT_ARRAY_SIZE)) {
                fp32_r =
                    fp32_lut_interp(degamma_lut[lut_idx].red,
                                    degamma_lut[lut_idx + 1].red,
                                    interp,
                                    interp_max);
                fp32_g =
                    fp32_lut_interp(degamma_lut[lut_idx].green,
                                    degamma_lut[lut_idx + 1].green,
                                    interp,
                                    interp_max);
                fp32_b =
                    fp32_lut_interp(degamma_lut[lut_idx].blue,
                                    degamma_lut[lut_idx + 1].blue,
                                    interp,
                                    interp_max);
            } else {
                fp32_r = nvKmsKapiUI32ToF32((NvU32) degamma_lut[lut_idx].red);
                fp32_g = nvKmsKapiUI32ToF32((NvU32) degamma_lut[lut_idx].green);
                fp32_b = nvKmsKapiUI32ToF32((NvU32) degamma_lut[lut_idx].blue);
            }

            /* Convert UNORM16 to 1.0-normalized FP32. */
            fp32_r = nvKmsKapiF32Div(fp32_r, u16_norm);
            fp32_g = nvKmsKapiF32Div(fp32_g, u16_norm);
            fp32_b = nvKmsKapiF32Div(fp32_b, u16_norm);
        } else if (vss_entries != NULL) {
            /* Use VSS LUT directly, but normalized to 1.0. */
            // TODO: Use pre-1.0-normalized VSS LUT table?
            NvU16 fp16_entry = vss_entries[entry_idx];
            NvU32 fp32_entry = nvKmsKapiF16ToF32(fp16_entry);

            fp32_r = fp32_g = fp32_b = nvKmsKapiF32Div(fp32_entry, fp_norm);
        } else {
            /* Use implicit identity. */
            // TODO: Use LUT table?
            fp32_r = fp32_g = fp32_b =
                nvKmsKapiF32Div(nvKmsKapiUI32ToF32(entry_idx), u10_norm);
        }

        /* Apply multiplier. */
        if (multiply) {
            fp32_r = nvKmsKapiF32Mul(fp32_r, fp32_multiplier);
            fp32_g = nvKmsKapiF32Mul(fp32_g, fp32_multiplier);
            fp32_b = nvKmsKapiF32Mul(fp32_b, fp32_multiplier);
        }

        /* Convert from FP32 to FP16 to populate LUT. */
        lut_data[(data_idx * 4) + 0] = nvKmsKapiF32ToF16(fp32_r);
        lut_data[(data_idx * 4) + 1] = nvKmsKapiF32ToF16(fp32_g);
        lut_data[(data_idx * 4) + 2] = nvKmsKapiF32ToF16(fp32_b);
    }
    ((NvU64 *) lut_data)[NUM_VSS_HEADER_ENTRIES + num_entries - 1] =
        ((NvU64 *) lut_data)[NUM_VSS_HEADER_ENTRIES + num_entries - 2];

    return drm_lut_surface;
}

#define UNORM16_TO_UNORM14_WAR_813188(u16) ((u16 >> 2) & ~7) + 0x6000

static struct nv_drm_lut_surface *create_drm_ilut_surface_legacy(
    struct nv_drm_device *nv_dev,
    struct nv_drm_plane *nv_plane,
    struct nv_drm_plane_state *nv_drm_plane_state)

{
    struct nv_drm_lut_surface *drm_lut_surface;
    NvU16 *lut_data;
    NvU32 entry_idx;

    const struct drm_color_lut *degamma_lut;

    WARN_ON(!nv_plane->ilut_caps.supported);
    WARN_ON(nv_plane->ilut_caps.entryFormat != NVKMS_LUT_FORMAT_UNORM14_WAR_813188);
    WARN_ON(nv_plane->ilut_caps.vssSupport == NVKMS_LUT_VSS_REQUIRED);
    WARN_ON((NVKMS_LUT_ARRAY_SIZE + 1) > nv_plane->ilut_caps.lutEntries);

    BUG_ON(nv_drm_plane_state->degamma_lut == NULL);

    degamma_lut =
        (struct drm_color_lut *) nv_drm_plane_state->degamma_lut->data;

    /* Allocate displayable LUT surface. */
    drm_lut_surface =
        alloc_drm_lut_surface(nv_dev,
                              NVKMS_LUT_FORMAT_UNORM14_WAR_813188,
                              NVKMS_LUT_VSS_TYPE_NONE,
                              0, 0,
                              NVKMS_LUT_ARRAY_SIZE + 1);
    if (drm_lut_surface == NULL) {
        return NULL;
    }

    lut_data = (NvU16 *) drm_lut_surface->base.buffer;

    /* Fill LUT surface. */
    for (entry_idx = 0; entry_idx < NVKMS_LUT_ARRAY_SIZE; entry_idx++) {
        lut_data[(entry_idx * 4) + 0] =
            UNORM16_TO_UNORM14_WAR_813188(degamma_lut[entry_idx].red);
        lut_data[(entry_idx * 4) + 1] =
            UNORM16_TO_UNORM14_WAR_813188(degamma_lut[entry_idx].green);
        lut_data[(entry_idx * 4) + 2] =
            UNORM16_TO_UNORM14_WAR_813188(degamma_lut[entry_idx].blue);
    }
    ((NvU64 *) lut_data)[NVKMS_LUT_ARRAY_SIZE] =
        ((NvU64 *) lut_data)[NVKMS_LUT_ARRAY_SIZE - 1];

    return drm_lut_surface;
}

static struct nv_drm_lut_surface *create_drm_tmo_surface(
    struct nv_drm_device *nv_dev,
    struct nv_drm_plane *nv_plane,
    struct nv_drm_plane_state *nv_drm_plane_state)

{
    struct nv_drm_lut_surface *drm_lut_surface;
    NvU16 *lut_data;
    NvU32 entry_idx;

    const struct drm_color_lut *tmo_lut;

    const NvU32 num_vss_header_segments = 64;
    const NvU32 tmo_seg_size_log2 = 4;

    WARN_ON(!nv_plane->tmo_caps.supported);
    WARN_ON(nv_plane->tmo_caps.entryFormat != NVKMS_LUT_FORMAT_UNORM16);
    WARN_ON(nv_plane->tmo_caps.vssSupport != NVKMS_LUT_VSS_REQUIRED);
    WARN_ON(nv_plane->tmo_caps.vssType != NVKMS_LUT_VSS_TYPE_LINEAR);
    WARN_ON(num_vss_header_segments != nv_plane->tmo_caps.vssSegments);
    WARN_ON((NVKMS_LUT_ARRAY_SIZE + 1) > nv_plane->tmo_caps.lutEntries);

    BUG_ON(nv_drm_plane_state->tmo_lut == NULL);

    tmo_lut = (struct drm_color_lut *) nv_drm_plane_state->tmo_lut->data;

    /* Verify that all channels are equal. */
    for (entry_idx = 0; entry_idx < NVKMS_LUT_ARRAY_SIZE; entry_idx++) {
        if ((tmo_lut[entry_idx].red != tmo_lut[entry_idx].green) ||
            (tmo_lut[entry_idx].red != tmo_lut[entry_idx].blue)) {
            return NULL;
        }
    }

    /*
     * Allocate displayable LUT surface.
     * The TMO LUT always uses VSS.
     */
    drm_lut_surface =
        alloc_drm_lut_surface(nv_dev,
                              NVKMS_LUT_FORMAT_UNORM16,
                              NVKMS_LUT_VSS_TYPE_LINEAR,
                              num_vss_header_segments,
                              NUM_VSS_HEADER_ENTRIES,
                              NVKMS_LUT_ARRAY_SIZE + 1);
    if (drm_lut_surface == NULL) {
        return NULL;
    }

    lut_data = (NvU16 *) drm_lut_surface->base.buffer;

    /* Calculate linear VSS header. */
    for (entry_idx = 0; entry_idx < NUM_VSS_HEADER_ENTRIES; entry_idx++) {
        int i;
        NvU64 vss_header_entry = 0;
        for (i = 0; (i < 16) &&
                    (((entry_idx * 16) + i) < num_vss_header_segments); i++) {
            vss_header_entry |=
                ((NvU64) tmo_seg_size_log2) << (i * 3);
        }
        ((NvU64 *) lut_data)[entry_idx] = vss_header_entry;
    }

    /* Fill LUT surface. */
    for (entry_idx = 0; entry_idx < NVKMS_LUT_ARRAY_SIZE; entry_idx++) {
        NvU32 data_idx = entry_idx + NUM_VSS_HEADER_ENTRIES;

        lut_data[(data_idx * 4) + 0] = tmo_lut[entry_idx].red;
        lut_data[(data_idx * 4) + 1] = tmo_lut[entry_idx].green;
        lut_data[(data_idx * 4) + 2] = tmo_lut[entry_idx].blue;
    }
    ((NvU64 *) lut_data)[NUM_VSS_HEADER_ENTRIES + NVKMS_LUT_ARRAY_SIZE] =
        ((NvU64 *) lut_data)[NUM_VSS_HEADER_ENTRIES + NVKMS_LUT_ARRAY_SIZE - 1];

    return drm_lut_surface;
}

static NvU16 unorm16_lut_interp(
    NvU16 entry0,
    NvU16 entry1,
    NvU16 interp,
    NvU16 interp_max)
{
    NvU64 u64_entry0 = (NvU64) entry0;
    NvU64 u64_entry1 = (NvU64) entry1;

    u64_entry0 *= (NvU64) (interp_max - interp);
    u64_entry0 /= (NvU64) interp_max;

    u64_entry1 *= (NvU64) interp;
    u64_entry1 /= (NvU64) interp_max;

    return (NvU16) (u64_entry0 + u64_entry1);
}

static struct nv_drm_lut_surface *create_drm_olut_surface_vss(
    struct nv_drm_device *nv_dev,
    struct nv_drm_crtc *nv_crtc,
    struct nv_drm_crtc_state *nv_drm_crtc_state)
{
    struct nv_drm_lut_surface *drm_lut_surface;

    NvU32 entry_idx;
    NvU32 num_entries;
    NvU16 *lut_data;

    const NvU32 *vss_header_seg_sizes = NULL;
    NvU32 num_vss_header_segments = 0;
    const NvU16 *vss_entries = NULL;
    enum NvKmsLUTVssType vss_type = NVKMS_LUT_VSS_TYPE_NONE;

    WARN_ON(!nv_crtc->olut_caps.supported);
    WARN_ON(nv_crtc->olut_caps.entryFormat != NVKMS_LUT_FORMAT_UNORM16);
    WARN_ON(nv_crtc->olut_caps.vssSupport != NVKMS_LUT_VSS_SUPPORTED);
    WARN_ON(nv_crtc->olut_caps.vssType != NVKMS_LUT_VSS_TYPE_LOGARITHMIC);

    /* Determine configuration based on specified OETF. */
    if (nv_drm_crtc_state->regamma_tf == NV_DRM_TRANSFER_FUNCTION_PQ) {
        /* Need VSS for PQ. */
        vss_header_seg_sizes = __oetf_pq_512_seg_sizes_log2;
        num_vss_header_segments = ARRAY_LEN(__oetf_pq_512_seg_sizes_log2);
        vss_type = NVKMS_LUT_VSS_TYPE_LOGARITHMIC;

        vss_entries = __oetf_pq_512_entries;
        num_entries = ARRAY_LEN(__oetf_pq_512_entries) + 1;
    } else {
        WARN_ON((nv_drm_crtc_state->regamma_tf != NV_DRM_TRANSFER_FUNCTION_DEFAULT) &&
                (nv_drm_crtc_state->regamma_tf != NV_DRM_TRANSFER_FUNCTION_LINEAR));

        num_entries = NVKMS_LUT_ARRAY_SIZE + 1;
    }
    WARN_ON((vss_entries != NULL) &&
            (num_vss_header_segments != nv_crtc->olut_caps.vssSegments));
    WARN_ON((vss_entries != NULL) && (num_entries > nv_crtc->olut_caps.lutEntries));
    WARN_ON((vss_entries == NULL) && (num_entries != nv_crtc->olut_caps.lutEntries));

    /*
     * Allocate displayable LUT surface.
     * Space for the VSS header must be included even for non-VSS LUTs.
     */
    drm_lut_surface =
        alloc_drm_lut_surface(nv_dev,
                              NVKMS_LUT_FORMAT_UNORM16,
                              vss_type,
                              num_vss_header_segments,
                              NUM_VSS_HEADER_ENTRIES,
                              num_entries);
    if (!drm_lut_surface) {
        return NULL;
    }

    lut_data = (NvU16 *) drm_lut_surface->base.buffer;

    /* Calculate VSS header. */
    if (vss_header_seg_sizes != NULL) {
        for (entry_idx = 0; entry_idx < NUM_VSS_HEADER_ENTRIES; entry_idx++) {
            int i;
            NvU64 vss_header_entry = 0;
            for (i = 0; (i < 16) &&
                        (((entry_idx * 16) + i) < num_vss_header_segments); i++) {
                vss_header_entry |=
                    ((NvU64) vss_header_seg_sizes[(entry_idx * 16) + i]) << (i * 3);
            }
            ((NvU64 *) lut_data)[entry_idx] = vss_header_entry;
        }
    }

    /* Calculate LUT content. */
    for (entry_idx = 0;
         entry_idx < num_entries - 1; entry_idx++) {
        NvU32 data_idx = entry_idx + NUM_VSS_HEADER_ENTRIES;

        NvU16 r, g, b = 0;

        if (nv_drm_crtc_state->regamma_lut != NULL) {
            /* Use provided Regamma LUT. */
            static const NvU16 interp_max = (((NvU16) 1) << (16 - 10)) - 1;

            const struct drm_color_lut *regamma_lut =
                (struct drm_color_lut *) nv_drm_crtc_state->regamma_lut->data;

            NvU16 lut_idx;
            NvU16 interp = 0;

            if (vss_entries != NULL) {
                /* Merge with provided VSS LUT. */
                NvU16 unorm16_entry = vss_entries[entry_idx];

                /* Index using upper 10 bits from UNORM16 VSS LUT. */
                lut_idx = unorm16_entry >> (16 - 10);
                /* Interpolate using lower 6 bits from UNORM16 VSS LUT. */
                interp = unorm16_entry & interp_max;
            } else {
                /* Direct index. */
                lut_idx = entry_idx;
            }

            BUG_ON(lut_idx >= NVKMS_LUT_ARRAY_SIZE);

            /* Perform interpolation or direct indexing. */
            if (interp > 0 && ((lut_idx + 1) < NVKMS_LUT_ARRAY_SIZE)) {
                r = unorm16_lut_interp(regamma_lut[lut_idx].red,
                                       regamma_lut[lut_idx + 1].red,
                                       interp,
                                       interp_max);
                g = unorm16_lut_interp(regamma_lut[lut_idx].green,
                                       regamma_lut[lut_idx + 1].green,
                                       interp,
                                       interp_max);
                b = unorm16_lut_interp(regamma_lut[lut_idx].blue,
                                       regamma_lut[lut_idx + 1].blue,
                                       interp,
                                       interp_max);
            } else {
                r = regamma_lut[lut_idx].red;
                g = regamma_lut[lut_idx].green;
                b = regamma_lut[lut_idx].blue;
            }
        } else if (vss_entries != NULL) {
            /* Use VSS LUT directly. */
            r = g = b = vss_entries[entry_idx];
        } else {
            /* Use implicit identity. */
            WARN_ON_ONCE(num_entries != (NVKMS_LUT_ARRAY_SIZE + 1));
            r = g = b = entry_idx << (16 - 10);
        }

        /* Populate LUT. */
        lut_data[(data_idx * 4) + 0] = r;
        lut_data[(data_idx * 4) + 1] = g;
        lut_data[(data_idx * 4) + 2] = b;
    }
    ((NvU64 *) lut_data)[NUM_VSS_HEADER_ENTRIES + num_entries - 1] =
        ((NvU64 *) lut_data)[NUM_VSS_HEADER_ENTRIES + num_entries - 2];

    return drm_lut_surface;
}

static struct nv_drm_lut_surface *create_drm_olut_surface_legacy(
    struct nv_drm_device *nv_dev,
    struct nv_drm_crtc *nv_crtc,
    struct nv_drm_crtc_state *nv_drm_crtc_state)

{
    struct nv_drm_lut_surface *drm_lut_surface;
    NvU16 *lut_data;
    NvU32 entry_idx;

    const struct drm_color_lut *regamma_lut;

    WARN_ON(!nv_crtc->olut_caps.supported);
    WARN_ON(nv_crtc->olut_caps.entryFormat != NVKMS_LUT_FORMAT_UNORM14_WAR_813188);
    WARN_ON(nv_crtc->olut_caps.vssSupport == NVKMS_LUT_VSS_REQUIRED);
    WARN_ON((NVKMS_LUT_ARRAY_SIZE + 1) > nv_crtc->olut_caps.lutEntries);

    BUG_ON(nv_drm_crtc_state->regamma_lut == NULL);

    regamma_lut =
        (struct drm_color_lut *) nv_drm_crtc_state->regamma_lut->data;

    /* Allocate displayable LUT surface. */
    drm_lut_surface =
        alloc_drm_lut_surface(nv_dev,
                              NVKMS_LUT_FORMAT_UNORM14_WAR_813188,
                              NVKMS_LUT_VSS_TYPE_NONE,
                              0, 0,
                              NVKMS_LUT_ARRAY_SIZE + 1);
    if (drm_lut_surface == NULL) {
        return NULL;
    }

    lut_data = (NvU16 *) drm_lut_surface->base.buffer;

    /* Fill LUT surface. */
    for (entry_idx = 0; entry_idx < NVKMS_LUT_ARRAY_SIZE; entry_idx++) {
        lut_data[(entry_idx * 4) + 0] =
            UNORM16_TO_UNORM14_WAR_813188(regamma_lut[entry_idx].red);
        lut_data[(entry_idx * 4) + 1] =
            UNORM16_TO_UNORM14_WAR_813188(regamma_lut[entry_idx].green);
        lut_data[(entry_idx * 4) + 2] =
            UNORM16_TO_UNORM14_WAR_813188(regamma_lut[entry_idx].blue);
    }
    ((NvU64 *) lut_data)[NVKMS_LUT_ARRAY_SIZE] =
        ((NvU64 *) lut_data)[NVKMS_LUT_ARRAY_SIZE - 1];

    return drm_lut_surface;
}

static bool
update_matrix_override(struct drm_property_blob *blob,
                       struct NvKmsCscMatrix *new_matrix,
                       const struct NvKmsCscMatrix *old_matrix,
                       bool old_enabled,
                       bool *changed)
{
    bool enabled;
    if (blob != NULL) {
        ctm_3x4_to_csc(new_matrix, (struct drm_color_ctm_3x4 *) blob->data);
        enabled = true;
    } else {
        enabled = false;
    }
    *changed |= (enabled != old_enabled) ||
                memcmp(new_matrix, old_matrix, sizeof(*old_matrix));
    return enabled;
}

static enum NvKmsInputColorSpace nv_get_nvkms_input_colorspace(
    enum nv_drm_input_color_space colorSpace)
{
    switch (colorSpace) {
        case NV_DRM_INPUT_COLOR_SPACE_NONE:
            return NVKMS_INPUT_COLOR_SPACE_NONE;
        case NV_DRM_INPUT_COLOR_SPACE_SCRGB_LINEAR:
            return NVKMS_INPUT_COLOR_SPACE_BT709;
        case NV_DRM_INPUT_COLOR_SPACE_BT2100_PQ:
            return NVKMS_INPUT_COLOR_SPACE_BT2100;
        default:
            /* We shouldn't hit this */
            WARN_ON("Unsupported input colorspace");
            return NVKMS_INPUT_COLOR_SPACE_NONE;
    }
}

static enum NvKmsInputTf nv_get_nvkms_input_tf(
    enum nv_drm_input_color_space colorSpace)
{
    switch (colorSpace) {
        case NV_DRM_INPUT_COLOR_SPACE_NONE:
            return NVKMS_INPUT_TF_LINEAR;
        case NV_DRM_INPUT_COLOR_SPACE_SCRGB_LINEAR:
            return NVKMS_INPUT_TF_LINEAR;
        case NV_DRM_INPUT_COLOR_SPACE_BT2100_PQ:
            return NVKMS_INPUT_TF_PQ;
        default:
            /* We shouldn't hit this */
            WARN_ON("Unsupported input colorspace");
            return NVKMS_INPUT_TF_LINEAR;
    }
}

#if defined(NV_DRM_PLANE_CREATE_COLOR_PROPERTIES_PRESENT)
static enum NvKmsInputColorSpace nv_drm_color_encoding_to_nvkms_colorspace(
    enum drm_color_encoding color_encoding)
{
    switch(color_encoding) {
        case DRM_COLOR_YCBCR_BT601:
            return NVKMS_INPUT_COLOR_SPACE_BT601;
        case DRM_COLOR_YCBCR_BT709:
            return NVKMS_INPUT_COLOR_SPACE_BT709;
        case DRM_COLOR_YCBCR_BT2020:
            return NVKMS_INPUT_COLOR_SPACE_BT2020;
        default:
            /* We shouldn't hit this */
            WARN_ON("Unsupported DRM color_encoding");
            return NVKMS_INPUT_COLOR_SPACE_NONE;
    }
}

static enum NvKmsInputColorRange nv_drm_color_range_to_nvkms_color_range(
    enum drm_color_range color_range)
{
    switch(color_range) {
        case DRM_COLOR_YCBCR_FULL_RANGE:
            return NVKMS_INPUT_COLOR_RANGE_FULL;
        case DRM_COLOR_YCBCR_LIMITED_RANGE:
            return NVKMS_INPUT_COLOR_RANGE_LIMITED;
        default:
            /* We shouldn't hit this */
            WARN_ON("Unsupported DRM color_range");
            return NVKMS_INPUT_COLOR_RANGE_DEFAULT;
    }
}
#endif

static int
plane_req_config_update(struct drm_plane *plane,
                        struct drm_plane_state *plane_state,
                        struct NvKmsKapiLayerRequestedConfig *req_config)
{
    struct nv_drm_device *nv_dev = to_nv_device(plane->dev);
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    struct NvKmsKapiLayerConfig old_config = req_config->config;
    struct nv_drm_plane_state *nv_drm_plane_state =
        to_nv_drm_plane_state(plane_state);
    bool matrix_overrides_changed = 0;

    if (plane_state->fb == NULL) {
        plane_req_config_disable(req_config);
        return 0;
    }

    memset(req_config, 0, sizeof(*req_config));

    req_config->config.surface = to_nv_framebuffer(plane_state->fb)->pSurface;

    /* Source values are 16.16 fixed point */
    req_config->config.srcX = plane_state->src_x >> 16;
    req_config->config.srcY = plane_state->src_y >> 16;
    req_config->config.srcWidth  = plane_state->src_w >> 16;
    req_config->config.srcHeight = plane_state->src_h >> 16;

    req_config->config.dstX = plane_state->crtc_x;
    req_config->config.dstY = plane_state->crtc_y;
    req_config->config.dstWidth  = plane_state->crtc_w;
    req_config->config.dstHeight = plane_state->crtc_h;

    req_config->config.csc = old_config.csc;

#if defined(NV_DRM_ROTATION_AVAILABLE)
    /*
     * plane_state->rotation is only valid when plane->rotation_property
     * is non-NULL.
     */
    if (plane->rotation_property != NULL) {
        if (plane_state->rotation & DRM_MODE_REFLECT_X) {
            req_config->config.rrParams.reflectionX = true;
        }

        if (plane_state->rotation & DRM_MODE_REFLECT_Y) {
            req_config->config.rrParams.reflectionY = true;
        }

        switch (plane_state->rotation & DRM_MODE_ROTATE_MASK) {
            case DRM_MODE_ROTATE_0:
                req_config->config.rrParams.rotation = NVKMS_ROTATION_0;
                break;
            case DRM_MODE_ROTATE_90:
                req_config->config.rrParams.rotation = NVKMS_ROTATION_90;
                break;
            case DRM_MODE_ROTATE_180:
                req_config->config.rrParams.rotation = NVKMS_ROTATION_180;
                break;
            case DRM_MODE_ROTATE_270:
                req_config->config.rrParams.rotation = NVKMS_ROTATION_270;
                break;
            default:
                /*
                 * We should not hit this, because
                 * plane_state->rotation should only have values
                 * registered in
                 * __nv_drm_plane_create_rotation_property().
                 */
                WARN_ON("Unsupported rotation");
                break;
        }
    }
#endif

#if defined(NV_DRM_ALPHA_BLENDING_AVAILABLE)
    if (plane->blend_mode_property != NULL && plane->alpha_property != NULL) {

        switch (plane_state->pixel_blend_mode) {
            case DRM_MODE_BLEND_PREMULTI:
                req_config->config.compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA;
                break;
            case DRM_MODE_BLEND_COVERAGE:
                req_config->config.compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA;
                break;
            default:
                /*
                 * We should not hit this, because
                 * plane_state->pixel_blend_mode should only have values
                 * registered in
                 * __nv_drm_plane_create_alpha_blending_properties().
                 */
                WARN_ON("Unsupported blending mode");
                break;

        }

        req_config->config.compParams.surfaceAlpha =
            plane_state->alpha >> 8;

    } else if (plane->blend_mode_property != NULL) {

        switch (plane_state->pixel_blend_mode) {
            case DRM_MODE_BLEND_PREMULTI:
                req_config->config.compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA;
                break;
            case DRM_MODE_BLEND_COVERAGE:
                req_config->config.compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA;
                break;
            default:
                /*
                 * We should not hit this, because
                 * plane_state->pixel_blend_mode should only have values
                 * registered in
                 * __nv_drm_plane_create_alpha_blending_properties().
                 */
                WARN_ON("Unsupported blending mode");
                break;

        }

    } else {
        req_config->config.compParams.compMode =
            nv_plane->defaultCompositionMode;
    }
#else
    req_config->config.compParams.compMode =
        nv_plane->defaultCompositionMode;
#endif

#if defined(NV_DRM_PLANE_CREATE_COLOR_PROPERTIES_PRESENT)
    if ((nv_drm_plane_state->input_colorspace == NV_DRM_INPUT_COLOR_SPACE_NONE) &&
        nv_drm_format_is_yuv(plane_state->fb->format->format)) {

        if (nv_plane->supportsColorProperties) {
            req_config->config.inputColorSpace =
                nv_drm_color_encoding_to_nvkms_colorspace(plane_state->color_encoding);
            req_config->config.inputColorRange =
                nv_drm_color_range_to_nvkms_color_range(plane_state->color_range);
        } else {
            req_config->config.inputColorSpace = NVKMS_INPUT_COLOR_SPACE_NONE;
            req_config->config.inputColorRange = NVKMS_INPUT_COLOR_RANGE_DEFAULT;
        }
        req_config->config.inputTf = NVKMS_INPUT_TF_LINEAR;
    } else {
#endif
        req_config->config.inputColorSpace =
            nv_get_nvkms_input_colorspace(nv_drm_plane_state->input_colorspace);
        req_config->config.inputColorRange = NVKMS_INPUT_COLOR_RANGE_DEFAULT;
        req_config->config.inputTf =
            nv_get_nvkms_input_tf(nv_drm_plane_state->input_colorspace);
#if defined(NV_DRM_PLANE_CREATE_COLOR_PROPERTIES_PRESENT)
    }
#endif

    req_config->flags.inputTfChanged =
        (old_config.inputTf != req_config->config.inputTf);
    req_config->flags.inputColorSpaceChanged =
        (old_config.inputColorSpace != req_config->config.inputColorSpace);
    req_config->flags.inputColorRangeChanged =
        (old_config.inputColorRange != req_config->config.inputColorRange);

    req_config->config.syncParams.preSyncptSpecified = false;
    req_config->config.syncParams.postSyncptRequested = false;
    req_config->config.syncParams.semaphoreSpecified = false;

    if (nv_drm_plane_state->fd_user_ptr) {
        if (nv_dev->supportsSyncpts) {
            req_config->config.syncParams.postSyncptRequested = true;
        } else {
            return -1;
        }
    }

#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    if (nv_drm_plane_state->hdr_output_metadata != NULL) {
        struct hdr_output_metadata *hdr_metadata =
            nv_drm_plane_state->hdr_output_metadata->data;
        struct hdr_metadata_infoframe *info_frame =
            &hdr_metadata->hdmi_metadata_type1;
        uint32_t i;

        if (hdr_metadata->metadata_type != HDMI_STATIC_METADATA_TYPE1) {
            NV_DRM_DEV_LOG_ERR(nv_dev, "Unsupported Metadata Type");
            return -1;
        }

        for (i = 0; i < ARRAY_SIZE(info_frame->display_primaries); i ++) {
            req_config->config.hdrMetadata.val.displayPrimaries[i].x =
                info_frame->display_primaries[i].x;
            req_config->config.hdrMetadata.val.displayPrimaries[i].y =
                info_frame->display_primaries[i].y;
        }

        req_config->config.hdrMetadata.val.whitePoint.x =
            info_frame->white_point.x;
        req_config->config.hdrMetadata.val.whitePoint.y =
            info_frame->white_point.y;
        req_config->config.hdrMetadata.val.maxDisplayMasteringLuminance =
            info_frame->max_display_mastering_luminance;
        req_config->config.hdrMetadata.val.minDisplayMasteringLuminance =
            info_frame->min_display_mastering_luminance;
        req_config->config.hdrMetadata.val.maxCLL =
            info_frame->max_cll;
        req_config->config.hdrMetadata.val.maxFALL =
            info_frame->max_fall;

        switch (info_frame->eotf) {
            case HDMI_EOTF_SMPTE_ST2084:
                req_config->config.outputTf = NVKMS_OUTPUT_TF_PQ;
                break;
            case HDMI_EOTF_TRADITIONAL_GAMMA_SDR:
                req_config->config.outputTf =
                    NVKMS_OUTPUT_TF_TRADITIONAL_GAMMA_SDR;
                break;
            default:
                NV_DRM_DEV_LOG_ERR(nv_dev, "Unsupported EOTF");
                return -1;
        }

        req_config->config.hdrMetadata.enabled = true;
    } else {
        req_config->config.hdrMetadata.enabled = false;
        req_config->config.outputTf = NVKMS_OUTPUT_TF_NONE;
    }

    req_config->flags.hdrMetadataChanged =
        ((old_config.hdrMetadata.enabled !=
          req_config->config.hdrMetadata.enabled) ||
         memcmp(&old_config.hdrMetadata.val,
                &req_config->config.hdrMetadata.val,
                sizeof(struct NvKmsHDRStaticMetadata)));

    req_config->flags.outputTfChanged = (old_config.outputTf != req_config->config.outputTf);
#endif

    req_config->config.matrixOverrides.enabled.lmsCtm =
        update_matrix_override(nv_drm_plane_state->lms_ctm,
                               &req_config->config.matrixOverrides.lmsCtm,
                               &old_config.matrixOverrides.lmsCtm,
                               old_config.matrixOverrides.enabled.lmsCtm,
                               &matrix_overrides_changed);
    req_config->config.matrixOverrides.enabled.lmsToItpCtm =
        update_matrix_override(nv_drm_plane_state->lms_to_itp_ctm,
                               &req_config->config.matrixOverrides.lmsToItpCtm,
                               &old_config.matrixOverrides.lmsToItpCtm,
                               old_config.matrixOverrides.enabled.lmsToItpCtm,
                               &matrix_overrides_changed);
    req_config->config.matrixOverrides.enabled.itpToLmsCtm =
        update_matrix_override(nv_drm_plane_state->itp_to_lms_ctm,
                               &req_config->config.matrixOverrides.itpToLmsCtm,
                               &old_config.matrixOverrides.itpToLmsCtm,
                               old_config.matrixOverrides.enabled.itpToLmsCtm,
                               &matrix_overrides_changed);
    req_config->config.matrixOverrides.enabled.blendCtm =
        update_matrix_override(nv_drm_plane_state->blend_ctm,
                               &req_config->config.matrixOverrides.blendCtm,
                               &old_config.matrixOverrides.blendCtm,
                               old_config.matrixOverrides.enabled.blendCtm,
                               &matrix_overrides_changed);
    req_config->flags.matrixOverridesChanged = matrix_overrides_changed;

    if (nv_drm_plane_state->degamma_changed) {
        if (nv_drm_plane_state->degamma_drm_lut_surface != NULL) {
            kref_put(&nv_drm_plane_state->degamma_drm_lut_surface->base.refcount,
                     free_drm_lut_surface);
            nv_drm_plane_state->degamma_drm_lut_surface = NULL;
        }

        if (nv_plane->ilut_caps.vssSupport == NVKMS_LUT_VSS_SUPPORTED) {
            if ((nv_drm_plane_state->degamma_tf  != NV_DRM_TRANSFER_FUNCTION_DEFAULT) ||
                (nv_drm_plane_state->degamma_lut != NULL) ||
                (nv_drm_plane_state->degamma_multiplier != ((uint64_t) 1) << 32)) {

                nv_drm_plane_state->degamma_drm_lut_surface =
                    create_drm_ilut_surface_vss(nv_dev, nv_plane,
                                                nv_drm_plane_state);
                if (nv_drm_plane_state->degamma_drm_lut_surface == NULL) {
                    return -1;
                }
            }
        } else {
            WARN_ON(nv_plane->ilut_caps.vssSupport != NVKMS_LUT_VSS_NOT_SUPPORTED);
            if (nv_drm_plane_state->degamma_lut != NULL) {
                nv_drm_plane_state->degamma_drm_lut_surface =
                    create_drm_ilut_surface_legacy(nv_dev, nv_plane,
                                                   nv_drm_plane_state);
                if (nv_drm_plane_state->degamma_drm_lut_surface == NULL) {
                    return -1;
                }
            }
        }

        if (nv_drm_plane_state->degamma_drm_lut_surface != NULL) {
            req_config->config.ilut.enabled = NV_TRUE;
            req_config->config.ilut.lutSurface =
                nv_drm_plane_state->degamma_drm_lut_surface->base.nvkms_surface;
            req_config->config.ilut.offset = 0;
            req_config->config.ilut.vssSegments =
                nv_drm_plane_state->degamma_drm_lut_surface->properties.vssSegments;
            req_config->config.ilut.lutEntries =
                nv_drm_plane_state->degamma_drm_lut_surface->properties.lutEntries;
        } else {
            req_config->config.ilut.enabled = NV_FALSE;
            req_config->config.ilut.lutSurface = NULL;
            req_config->config.ilut.offset = 0;
            req_config->config.ilut.vssSegments = 0;
            req_config->config.ilut.lutEntries = 0;

        }
        req_config->flags.ilutChanged = NV_TRUE;
    }

    if (nv_drm_plane_state->tmo_changed) {
        if (nv_drm_plane_state->tmo_drm_lut_surface != NULL) {
            kref_put(&nv_drm_plane_state->tmo_drm_lut_surface->base.refcount,
                     free_drm_lut_surface);
            nv_drm_plane_state->tmo_drm_lut_surface = NULL;
        }

        if (nv_drm_plane_state->tmo_lut != NULL) {
            nv_drm_plane_state->tmo_drm_lut_surface =
                create_drm_tmo_surface(nv_dev, nv_plane,
                                       nv_drm_plane_state);
            if (nv_drm_plane_state->tmo_drm_lut_surface == NULL) {
                return -1;
            }
        }

        if (nv_drm_plane_state->tmo_drm_lut_surface != NULL) {
            req_config->config.tmo.enabled = NV_TRUE;
            req_config->config.tmo.lutSurface =
                nv_drm_plane_state->tmo_drm_lut_surface->base.nvkms_surface;
            req_config->config.tmo.offset = 0;
            req_config->config.tmo.vssSegments =
                nv_drm_plane_state->tmo_drm_lut_surface->properties.vssSegments;
            req_config->config.tmo.lutEntries =
                nv_drm_plane_state->tmo_drm_lut_surface->properties.lutEntries;
        } else {
            req_config->config.tmo.enabled = NV_FALSE;
            req_config->config.tmo.lutSurface = NULL;
            req_config->config.tmo.offset = 0;
            req_config->config.tmo.vssSegments = 0;
            req_config->config.tmo.lutEntries = 0;
        }
        req_config->flags.tmoChanged = NV_TRUE;
    }

    /*
     * Unconditionally mark the surface as changed, even if nothing changed,
     * so that we always get a flip event: a DRM client may flip with
     * the same surface and wait for a flip event.
     */
    req_config->flags.surfaceChanged = NV_TRUE;

    if (old_config.surface == NULL &&
        old_config.surface != req_config->config.surface) {
        req_config->flags.srcXYChanged = NV_TRUE;
        req_config->flags.srcWHChanged = NV_TRUE;
        req_config->flags.dstXYChanged = NV_TRUE;
        req_config->flags.dstWHChanged = NV_TRUE;
        return 0;
    }

    req_config->flags.srcXYChanged =
        old_config.srcX != req_config->config.srcX ||
        old_config.srcY != req_config->config.srcY;

    req_config->flags.srcWHChanged =
        old_config.srcWidth != req_config->config.srcWidth ||
        old_config.srcHeight != req_config->config.srcHeight;

    req_config->flags.dstXYChanged =
        old_config.dstX != req_config->config.dstX ||
        old_config.dstY != req_config->config.dstY;

    req_config->flags.dstWHChanged =
        old_config.dstWidth != req_config->config.dstWidth ||
        old_config.dstHeight != req_config->config.dstHeight;

    return 0;
}

static bool __is_async_flip_requested(const struct drm_plane *plane,
                                      const struct drm_crtc_state *crtc_state)
{
    if (plane->type == DRM_PLANE_TYPE_PRIMARY) {
#if defined(NV_DRM_CRTC_STATE_HAS_ASYNC_FLIP)
        return crtc_state->async_flip;
#elif defined(NV_DRM_CRTC_STATE_HAS_PAGEFLIP_FLAGS)
        return !!(crtc_state->pageflip_flags & DRM_MODE_PAGE_FLIP_ASYNC);
#endif
    }

    return false;
}

static int __nv_drm_cursor_atomic_check(struct drm_plane *plane,
                                        struct drm_plane_state *plane_state)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    int i;
    struct drm_crtc *crtc;
    struct drm_crtc_state *crtc_state;

    WARN_ON(nv_plane->layer_idx != NVKMS_KAPI_LAYER_INVALID_IDX);

    nv_drm_for_each_crtc_in_state(plane_state->state, crtc, crtc_state, i) {
        struct nv_drm_crtc_state *nv_crtc_state = to_nv_crtc_state(crtc_state);
        struct NvKmsKapiHeadRequestedConfig *head_req_config =
            &nv_crtc_state->req_config;
        struct NvKmsKapiCursorRequestedConfig *cursor_req_config =
            &head_req_config->cursorRequestedConfig;

        if (plane->state->crtc == crtc &&
            plane->state->crtc != plane_state->crtc) {
            cursor_req_config_disable(cursor_req_config);
            continue;
        }

        if (plane_state->crtc == crtc) {
            cursor_plane_req_config_update(plane, plane_state,
                                           cursor_req_config);
        }
    }

    return 0;
}

#if defined(NV_DRM_PLANE_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG)
static int nv_drm_plane_atomic_check(struct drm_plane *plane,
                                     struct drm_atomic_state *state)
#else
static int nv_drm_plane_atomic_check(struct drm_plane *plane,
                                     struct drm_plane_state *plane_state)
#endif
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
#if defined(NV_DRM_PLANE_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG)
    struct drm_plane_state *plane_state =
        drm_atomic_get_new_plane_state(state, plane);
#endif
    int i;
    struct drm_crtc *crtc;
    struct drm_crtc_state *crtc_state;
    int ret;

    if (plane->type == DRM_PLANE_TYPE_CURSOR) {
        return __nv_drm_cursor_atomic_check(plane, plane_state);
    }

    WARN_ON(nv_plane->layer_idx == NVKMS_KAPI_LAYER_INVALID_IDX);

    nv_drm_for_each_crtc_in_state(plane_state->state, crtc, crtc_state, i) {
        struct nv_drm_crtc_state *nv_crtc_state = to_nv_crtc_state(crtc_state);
        struct NvKmsKapiHeadRequestedConfig *head_req_config =
            &nv_crtc_state->req_config;
        struct NvKmsKapiLayerRequestedConfig *plane_requested_config =
            &head_req_config->layerRequestedConfig[nv_plane->layer_idx];

        if (plane->state->crtc == crtc &&
            plane->state->crtc != plane_state->crtc) {
            plane_req_config_disable(plane_requested_config);
            continue;
        }

        if (plane_state->crtc == crtc) {
            ret = plane_req_config_update(plane,
                                          plane_state,
                                          plane_requested_config);
            if (ret != 0) {
                return ret;
            }

#if defined(NV_DRM_COLOR_MGMT_AVAILABLE)
            if (crtc_state->color_mgmt_changed) {
                /*
                 * According to the comment in the Linux kernel's
                 * drivers/gpu/drm/drm_color_mgmt.c, if this property is NULL,
                 * the CTM needs to be changed to the identity matrix
                 */
                if (crtc_state->ctm) {
                    ctm_to_csc(&plane_requested_config->config.csc,
                               (struct drm_color_ctm *)crtc_state->ctm->data);
                } else {
                    plane_requested_config->config.csc = NVKMS_IDENTITY_CSC_MATRIX;
                }
                plane_requested_config->config.cscUseMain = NV_FALSE;
                plane_requested_config->flags.cscChanged = NV_TRUE;
            }
#endif /* NV_DRM_COLOR_MGMT_AVAILABLE */

            if (__is_async_flip_requested(plane, crtc_state)) {
                /*
                 * Async flip requests that the flip happen 'as soon as
                 * possible', meaning that it not delay waiting for vblank.
                 * This may cause tearing on the screen.
                 */
                plane_requested_config->config.minPresentInterval = 0;
                plane_requested_config->config.tearing = NV_TRUE;
            } else {
                plane_requested_config->config.minPresentInterval = 1;
                plane_requested_config->config.tearing = NV_FALSE;
            }
        }
    }

    return 0;
}

#if defined(NV_DRM_UNIVERSAL_PLANE_INIT_HAS_FORMAT_MODIFIERS_ARG)
static bool nv_drm_plane_format_mod_supported(struct drm_plane *plane,
                                              uint32_t format,
                                              uint64_t modifier)
{
    /* All supported modifiers are compatible with all supported formats */
    return true;
}
#endif

static int nv_drm_atomic_crtc_get_property(
    struct drm_crtc *crtc,
    const struct drm_crtc_state *state,
    struct drm_property *property,
    uint64_t *val)
{
    struct nv_drm_device *nv_dev = to_nv_device(crtc->dev);
    const struct nv_drm_crtc_state *nv_drm_crtc_state =
        to_nv_crtc_state_const(state);

    if (property == nv_dev->nv_crtc_regamma_tf_property) {
        *val = nv_drm_crtc_state->regamma_tf;
        return 0;
    } else if (property == nv_dev->nv_crtc_regamma_lut_property) {
        *val = nv_drm_crtc_state->regamma_lut ?
            nv_drm_crtc_state->regamma_lut->base.id : 0;
        return 0;
    } else if (property == nv_dev->nv_crtc_regamma_divisor_property) {
        *val = nv_drm_crtc_state->regamma_divisor;
        return 0;
    } else if (property == nv_dev->nv_crtc_regamma_lut_size_property) {
        /*
         * This shouldn't be necessary, because read-only properties are stored
         * in obj->properties->values[]. To be safe, check for it anyway.
         */
        *val = NVKMS_LUT_ARRAY_SIZE;
        return 0;
    }

    return -EINVAL;

}

static int nv_drm_atomic_crtc_set_property(
    struct drm_crtc *crtc,
    struct drm_crtc_state *state,
    struct drm_property *property,
    uint64_t val)
{
    struct nv_drm_device *nv_dev = to_nv_device(crtc->dev);
    struct nv_drm_crtc_state *nv_drm_crtc_state =
        to_nv_crtc_state(state);
    NvBool replaced = false;

    if (property == nv_dev->nv_crtc_regamma_tf_property) {
        if (val != nv_drm_crtc_state->regamma_tf) {
            nv_drm_crtc_state->regamma_tf = val;
            nv_drm_crtc_state->regamma_changed = true;
        }
        return 0;
    } else if (property == nv_dev->nv_crtc_regamma_lut_property) {
        int ret = nv_drm_atomic_replace_property_blob_from_id(
                   nv_dev->dev,
                   &nv_drm_crtc_state->regamma_lut,
                   val,
                   sizeof(struct drm_color_lut) * NVKMS_LUT_ARRAY_SIZE,
                   &replaced);
        if (replaced) {
            nv_drm_crtc_state->regamma_changed = true;
        }
        return ret;
    } else if (property == nv_dev->nv_crtc_regamma_divisor_property) {
        if (val != nv_drm_crtc_state->regamma_divisor) {
            nv_drm_crtc_state->regamma_divisor = val;
            nv_drm_crtc_state->regamma_changed = true;
        }
        return 0;
    }

    return -EINVAL;
}

static int nv_drm_plane_atomic_set_property(
    struct drm_plane *plane,
    struct drm_plane_state *state,
    struct drm_property *property,
    uint64_t val)
{
    struct nv_drm_device *nv_dev = to_nv_device(plane->dev);
    struct nv_drm_plane_state *nv_drm_plane_state =
        to_nv_drm_plane_state(state);
    NvBool replaced = false;

    if (property == nv_dev->nv_out_fence_property) {
        nv_drm_plane_state->fd_user_ptr = (void __user *)(uintptr_t)(val);
        return 0;
    } else if (property == nv_dev->nv_input_colorspace_property) {
        nv_drm_plane_state->input_colorspace = val;
        return 0;
    }
#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    else if (property == nv_dev->nv_hdr_output_metadata_property) {
        return nv_drm_atomic_replace_property_blob_from_id(
                nv_dev->dev,
                &nv_drm_plane_state->hdr_output_metadata,
                val,
                sizeof(struct hdr_output_metadata),
                &replaced);
    }
#endif
    else if (property == nv_dev->nv_plane_lms_ctm_property) {
        return nv_drm_atomic_replace_property_blob_from_id(
                nv_dev->dev,
                &nv_drm_plane_state->lms_ctm,
                val,
                sizeof(struct drm_color_ctm_3x4),
                &replaced);
    } else if (property == nv_dev->nv_plane_lms_to_itp_ctm_property) {
        return nv_drm_atomic_replace_property_blob_from_id(
                nv_dev->dev,
                &nv_drm_plane_state->lms_to_itp_ctm,
                val,
                sizeof(struct drm_color_ctm_3x4),
                &replaced);
    } else if (property == nv_dev->nv_plane_itp_to_lms_ctm_property) {
        return nv_drm_atomic_replace_property_blob_from_id(
                nv_dev->dev,
                &nv_drm_plane_state->itp_to_lms_ctm,
                val,
                sizeof(struct drm_color_ctm_3x4),
                &replaced);
    } else if (property == nv_dev->nv_plane_blend_ctm_property) {
        return nv_drm_atomic_replace_property_blob_from_id(
                nv_dev->dev,
                &nv_drm_plane_state->blend_ctm,
                val,
                sizeof(struct drm_color_ctm_3x4),
                &replaced);
    } else if (property == nv_dev->nv_plane_degamma_tf_property) {
        if (val != nv_drm_plane_state->degamma_tf) {
            nv_drm_plane_state->degamma_tf = val;
            nv_drm_plane_state->degamma_changed = true;
        }
        return 0;
    } else if (property == nv_dev->nv_plane_degamma_lut_property) {
        int ret = nv_drm_atomic_replace_property_blob_from_id(
                   nv_dev->dev,
                   &nv_drm_plane_state->degamma_lut,
                   val,
                   sizeof(struct drm_color_lut) * NVKMS_LUT_ARRAY_SIZE,
                   &replaced);
        if (replaced) {
            nv_drm_plane_state->degamma_changed = true;
        }
        return ret;
    } else if (property == nv_dev->nv_plane_degamma_multiplier_property) {
        if (val != nv_drm_plane_state->degamma_multiplier) {
            nv_drm_plane_state->degamma_multiplier = val;
            nv_drm_plane_state->degamma_changed = true;
        }
        return 0;
    } else if (property == nv_dev->nv_plane_tmo_lut_property) {
        int ret = nv_drm_atomic_replace_property_blob_from_id(
                   nv_dev->dev,
                   &nv_drm_plane_state->tmo_lut,
                   val,
                   sizeof(struct drm_color_lut) * NVKMS_LUT_ARRAY_SIZE,
                   &replaced);
        if (replaced) {
            nv_drm_plane_state->tmo_changed = true;
        }
        return ret;
    }

    return -EINVAL;
}

static int nv_drm_plane_atomic_get_property(
    struct drm_plane *plane,
    const struct drm_plane_state *state,
    struct drm_property *property,
    uint64_t *val)
{
    struct nv_drm_device *nv_dev = to_nv_device(plane->dev);
    const struct nv_drm_plane_state *nv_drm_plane_state =
        to_nv_drm_plane_state_const(state);

    if (property == nv_dev->nv_out_fence_property) {
        return 0;
    } else if (property == nv_dev->nv_input_colorspace_property) {
        *val = nv_drm_plane_state->input_colorspace;
        return 0;
    }
#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    else if (property ==  nv_dev->nv_hdr_output_metadata_property) {
        *val = nv_drm_plane_state->hdr_output_metadata ?
            nv_drm_plane_state->hdr_output_metadata->base.id : 0;
        return 0;
    }
#endif
    else if (property == nv_dev->nv_plane_lms_ctm_property) {
        *val = nv_drm_plane_state->lms_ctm ?
            nv_drm_plane_state->lms_ctm->base.id : 0;
        return 0;
    } else if (property == nv_dev->nv_plane_lms_to_itp_ctm_property) {
        *val = nv_drm_plane_state->lms_to_itp_ctm ?
            nv_drm_plane_state->lms_to_itp_ctm->base.id : 0;
        return 0;
    } else if (property == nv_dev->nv_plane_itp_to_lms_ctm_property) {
        *val = nv_drm_plane_state->itp_to_lms_ctm ?
            nv_drm_plane_state->itp_to_lms_ctm->base.id : 0;
        return 0;
    } else if (property == nv_dev->nv_plane_blend_ctm_property) {
        *val = nv_drm_plane_state->blend_ctm ?
            nv_drm_plane_state->blend_ctm->base.id : 0;
        return 0;
    } else if (property == nv_dev->nv_plane_degamma_tf_property) {
        *val = nv_drm_plane_state->degamma_tf;
        return 0;
    } else if (property == nv_dev->nv_plane_degamma_lut_property) {
        *val = nv_drm_plane_state->degamma_lut ?
            nv_drm_plane_state->degamma_lut->base.id : 0;
        return 0;
    } else if (property == nv_dev->nv_plane_degamma_multiplier_property) {
        *val = nv_drm_plane_state->degamma_multiplier;
        return 0;
    } else if (property == nv_dev->nv_plane_tmo_lut_property) {
        *val = nv_drm_plane_state->tmo_lut ?
            nv_drm_plane_state->tmo_lut->base.id : 0;
        return 0;
    } else if ((property == nv_dev->nv_plane_degamma_lut_size_property) ||
               (property == nv_dev->nv_plane_tmo_lut_size_property)) {
        /*
         * This shouldn't be necessary, because read-only properties are stored
         * in obj->properties->values[]. To be safe, check for it anyway.
         */
        *val = NVKMS_LUT_ARRAY_SIZE;
        return 0;
    }

    return -EINVAL;
}

/**
 * nv_drm_plane_atomic_reset - plane state reset hook
 * @plane: DRM plane
 *
 * Allocate an empty DRM plane state.
 */
static void nv_drm_plane_atomic_reset(struct drm_plane *plane)
{
    struct nv_drm_plane_state *nv_plane_state =
        nv_drm_calloc(1, sizeof(*nv_plane_state));

    if (!nv_plane_state) {
        return;
    }

    drm_atomic_helper_plane_reset(plane);

    /*
     * The drm atomic helper function allocates a state object that is the wrong
     * size. Copy its contents into the one we allocated above and replace the
     * pointer.
     */
    if (plane->state) {
        nv_plane_state->base = *plane->state;
        kfree(plane->state);
        plane->state = &nv_plane_state->base;
    } else {
        kfree(nv_plane_state);
    }
}


static struct drm_plane_state *
nv_drm_plane_atomic_duplicate_state(struct drm_plane *plane)
{
    struct nv_drm_plane_state *nv_old_plane_state =
        to_nv_drm_plane_state(plane->state);
    struct nv_drm_plane_state *nv_plane_state =
        nv_drm_calloc(1, sizeof(*nv_plane_state));

    if (nv_plane_state == NULL) {
        return NULL;
    }

    __drm_atomic_helper_plane_duplicate_state(plane, &nv_plane_state->base);

    nv_plane_state->fd_user_ptr = nv_old_plane_state->fd_user_ptr;
    nv_plane_state->input_colorspace = nv_old_plane_state->input_colorspace;

#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    nv_plane_state->hdr_output_metadata = nv_old_plane_state->hdr_output_metadata;
    if (nv_plane_state->hdr_output_metadata) {
        nv_drm_property_blob_get(nv_plane_state->hdr_output_metadata);
    }
#endif

    nv_plane_state->lms_ctm = nv_old_plane_state->lms_ctm;
    if (nv_plane_state->lms_ctm) {
        nv_drm_property_blob_get(nv_plane_state->lms_ctm);
    }

    nv_plane_state->lms_to_itp_ctm = nv_old_plane_state->lms_to_itp_ctm;
    if (nv_plane_state->lms_to_itp_ctm) {
        nv_drm_property_blob_get(nv_plane_state->lms_to_itp_ctm);
    }

    nv_plane_state->itp_to_lms_ctm = nv_old_plane_state->itp_to_lms_ctm;
    if (nv_plane_state->itp_to_lms_ctm) {
        nv_drm_property_blob_get(nv_plane_state->itp_to_lms_ctm);
    }

    nv_plane_state->blend_ctm = nv_old_plane_state->blend_ctm;
    if (nv_plane_state->blend_ctm) {
        nv_drm_property_blob_get(nv_plane_state->blend_ctm);
    }

    nv_plane_state->degamma_tf = nv_old_plane_state->degamma_tf;
    nv_plane_state->degamma_lut = nv_old_plane_state->degamma_lut;
    if (nv_plane_state->degamma_lut) {
        nv_drm_property_blob_get(nv_plane_state->degamma_lut);
    }
    nv_plane_state->degamma_multiplier = nv_old_plane_state->degamma_multiplier;
    nv_plane_state->degamma_changed = false;
    nv_plane_state->degamma_drm_lut_surface =
        nv_old_plane_state->degamma_drm_lut_surface;
    if (nv_plane_state->degamma_drm_lut_surface) {
        kref_get(&nv_plane_state->degamma_drm_lut_surface->base.refcount);
    }

    nv_plane_state->tmo_lut = nv_old_plane_state->tmo_lut;
    if (nv_plane_state->tmo_lut) {
        nv_drm_property_blob_get(nv_plane_state->tmo_lut);
    }
    nv_plane_state->tmo_changed = false;
    nv_plane_state->tmo_drm_lut_surface =
        nv_old_plane_state->tmo_drm_lut_surface;
    if (nv_plane_state->tmo_drm_lut_surface) {
        kref_get(&nv_plane_state->tmo_drm_lut_surface->base.refcount);
    }

    return &nv_plane_state->base;
}

static inline void __nv_drm_plane_atomic_destroy_state(
    struct drm_plane *plane,
    struct drm_plane_state *state)
{
    struct nv_drm_plane_state *nv_drm_plane_state =
        to_nv_drm_plane_state(state);
#if defined(NV_DRM_ATOMIC_HELPER_PLANE_DESTROY_STATE_HAS_PLANE_ARG)
    __drm_atomic_helper_plane_destroy_state(plane, state);
#else
    __drm_atomic_helper_plane_destroy_state(state);
#endif

#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    nv_drm_property_blob_put(nv_drm_plane_state->hdr_output_metadata);
#endif
    nv_drm_property_blob_put(nv_drm_plane_state->lms_ctm);
    nv_drm_property_blob_put(nv_drm_plane_state->lms_to_itp_ctm);
    nv_drm_property_blob_put(nv_drm_plane_state->itp_to_lms_ctm);
    nv_drm_property_blob_put(nv_drm_plane_state->blend_ctm);

    nv_drm_property_blob_put(nv_drm_plane_state->degamma_lut);
    if (nv_drm_plane_state->degamma_drm_lut_surface != NULL) {
        kref_put(&nv_drm_plane_state->degamma_drm_lut_surface->base.refcount,
                 free_drm_lut_surface);
    }

    nv_drm_property_blob_put(nv_drm_plane_state->tmo_lut);
    if (nv_drm_plane_state->tmo_drm_lut_surface != NULL) {
        kref_put(&nv_drm_plane_state->tmo_drm_lut_surface->base.refcount,
                 free_drm_lut_surface);
    }
}

static void nv_drm_plane_atomic_destroy_state(
    struct drm_plane *plane,
    struct drm_plane_state *state)
{
    __nv_drm_plane_atomic_destroy_state(plane, state);

    nv_drm_free(to_nv_drm_plane_state(state));
}

static const struct drm_plane_funcs nv_plane_funcs = {
    .update_plane           = drm_atomic_helper_update_plane,
    .disable_plane          = drm_atomic_helper_disable_plane,
    .destroy                = nv_drm_plane_destroy,
    .reset                  = nv_drm_plane_atomic_reset,
    .atomic_get_property    = nv_drm_plane_atomic_get_property,
    .atomic_set_property    = nv_drm_plane_atomic_set_property,
    .atomic_duplicate_state = nv_drm_plane_atomic_duplicate_state,
    .atomic_destroy_state   = nv_drm_plane_atomic_destroy_state,
#if defined(NV_DRM_UNIVERSAL_PLANE_INIT_HAS_FORMAT_MODIFIERS_ARG)
    .format_mod_supported   = nv_drm_plane_format_mod_supported,
#endif
};

static const struct drm_plane_helper_funcs nv_plane_helper_funcs = {
    .atomic_check   = nv_drm_plane_atomic_check,
};

static void nv_drm_crtc_destroy(struct drm_crtc *crtc)
{
    struct nv_drm_crtc *nv_crtc = to_nv_crtc(crtc);

    drm_crtc_cleanup(crtc);

    nv_drm_free(nv_crtc);
}

static inline void
__nv_drm_atomic_helper_crtc_destroy_state(struct drm_crtc *crtc,
                                          struct drm_crtc_state *crtc_state)
{
#if defined(NV_DRM_ATOMIC_HELPER_CRTC_DESTROY_STATE_HAS_CRTC_ARG)
    __drm_atomic_helper_crtc_destroy_state(crtc, crtc_state);
#else
    __drm_atomic_helper_crtc_destroy_state(crtc_state);
#endif
}

static inline bool nv_drm_crtc_duplicate_req_head_modeset_config(
    const struct NvKmsKapiHeadRequestedConfig *old,
    struct NvKmsKapiHeadRequestedConfig *new)
{
    uint32_t i;

    /*
     * Do not duplicate fields like 'modeChanged' flags expressing delta changed
     * in new configuration with respect to previous/old configuration because
     * there is no change in new configuration yet with respect
     * to older one!
     */
    memset(new, 0, sizeof(*new));
    new->modeSetConfig = old->modeSetConfig;

    for (i = 0; i < ARRAY_SIZE(old->layerRequestedConfig); i++) {
        new->layerRequestedConfig[i].config =
            old->layerRequestedConfig[i].config;
    }

    if (old->modeSetConfig.lut.input.pRamps) {
        new->modeSetConfig.lut.input.pRamps =
            nv_drm_calloc(1, sizeof(*new->modeSetConfig.lut.input.pRamps));

        if (!new->modeSetConfig.lut.input.pRamps) {
            return false;
        }
        *new->modeSetConfig.lut.input.pRamps =
            *old->modeSetConfig.lut.input.pRamps;
    }
    if (old->modeSetConfig.lut.output.pRamps) {
        new->modeSetConfig.lut.output.pRamps =
            nv_drm_calloc(1, sizeof(*new->modeSetConfig.lut.output.pRamps));

        if (!new->modeSetConfig.lut.output.pRamps) {
            /*
             * new->modeSetConfig.lut.input.pRamps is either NULL or it was
             * just allocated
             */
            nv_drm_free(new->modeSetConfig.lut.input.pRamps);
            new->modeSetConfig.lut.input.pRamps = NULL;
            return false;
        }
        *new->modeSetConfig.lut.output.pRamps =
            *old->modeSetConfig.lut.output.pRamps;
    }
    return true;
}

static inline struct nv_drm_crtc_state *nv_drm_crtc_state_alloc(void)
{
    struct nv_drm_crtc_state *nv_state = nv_drm_calloc(1, sizeof(*nv_state));
    int i;

    if (nv_state == NULL) {
        return NULL;
    }

    nv_state->req_config.modeSetConfig.olutFpNormScale = NVKMS_OLUT_FP_NORM_SCALE_DEFAULT;
    for (i = 0; i < ARRAY_SIZE(nv_state->req_config.layerRequestedConfig); i++) {
        plane_config_clear(&nv_state->req_config.layerRequestedConfig[i].config);
    }
    return nv_state;
}


/**
 * nv_drm_atomic_crtc_reset - crtc state reset hook
 * @crtc: DRM crtc
 *
 * Allocate an empty DRM crtc state.
 */
static void nv_drm_atomic_crtc_reset(struct drm_crtc *crtc)
{
    struct nv_drm_crtc_state *nv_state = nv_drm_crtc_state_alloc();

    if (!nv_state) {
        return;
    }

    drm_atomic_helper_crtc_reset(crtc);

    /*
     * The drm atomic helper function allocates a state object that is the wrong
     * size. Copy its contents into the one we allocated above and replace the
     * pointer.
     */
    if (crtc->state) {
        nv_state->base = *crtc->state;
        kfree(crtc->state);
        crtc->state = &nv_state->base;
    } else {
        kfree(nv_state);
    }
}

/**
 * nv_drm_atomic_crtc_duplicate_state - crtc state duplicate hook
 * @crtc: DRM crtc
 *
 * Allocate and accosiate flip state with DRM crtc state, this flip state will
 * be getting consumed at the time of atomic update commit to hardware by
 * nv_drm_atomic_helper_commit_tail().
 */
static struct drm_crtc_state*
nv_drm_atomic_crtc_duplicate_state(struct drm_crtc *crtc)
{
    struct nv_drm_crtc_state *nv_old_state = to_nv_crtc_state(crtc->state);
    struct nv_drm_crtc_state *nv_state = nv_drm_crtc_state_alloc();

    if (nv_state == NULL) {
        return NULL;
    }

    if ((nv_state->nv_flip =
            nv_drm_calloc(1, sizeof(*(nv_state->nv_flip)))) == NULL) {
        nv_drm_free(nv_state);
        return NULL;
    }

    INIT_LIST_HEAD(&nv_state->nv_flip->list_entry);
    INIT_LIST_HEAD(&nv_state->nv_flip->deferred_flip_list);

    /*
     * nv_drm_crtc_duplicate_req_head_modeset_config potentially allocates
     * nv_state->req_config.modeSetConfig.lut.{in,out}put.pRamps, so they should
     * be freed in any following failure paths.
     */
    if (!nv_drm_crtc_duplicate_req_head_modeset_config(
             &nv_old_state->req_config,
             &nv_state->req_config)) {

        nv_drm_free(nv_state->nv_flip);
        nv_drm_free(nv_state);
        return NULL;
    }

    __drm_atomic_helper_crtc_duplicate_state(crtc, &nv_state->base);

    nv_state->regamma_tf = nv_old_state->regamma_tf;
    nv_state->regamma_lut = nv_old_state->regamma_lut;
    if (nv_state->regamma_lut) {
        nv_drm_property_blob_get(nv_state->regamma_lut);
    }
    nv_state->regamma_divisor = nv_old_state->regamma_divisor;
    if (nv_state->regamma_drm_lut_surface) {
        kref_get(&nv_state->regamma_drm_lut_surface->base.refcount);
    }
    nv_state->regamma_changed = false;

    return &nv_state->base;
}

/**
 * nv_drm_atomic_crtc_destroy_state - crtc state destroy hook
 * @crtc: DRM crtc
 * @state: DRM crtc state object to destroy
 *
 * Destroy flip state associated with the given crtc state if it haven't get
 * consumed because failure of atomic commit.
 */
static void nv_drm_atomic_crtc_destroy_state(struct drm_crtc *crtc,
                                             struct drm_crtc_state *state)
{
    struct nv_drm_crtc_state *nv_state = to_nv_crtc_state(state);

    if (nv_state->nv_flip != NULL) {
        nv_drm_free(nv_state->nv_flip);
        nv_state->nv_flip = NULL;
    }

    __nv_drm_atomic_helper_crtc_destroy_state(crtc, &nv_state->base);

    nv_drm_property_blob_put(nv_state->regamma_lut);
    if (nv_state->regamma_drm_lut_surface != NULL) {
        kref_put(&nv_state->regamma_drm_lut_surface->base.refcount,
                 free_drm_lut_surface);
    }

    nv_drm_free(nv_state->req_config.modeSetConfig.lut.input.pRamps);
    nv_drm_free(nv_state->req_config.modeSetConfig.lut.output.pRamps);

    nv_drm_free(nv_state);
}

static struct drm_crtc_funcs nv_crtc_funcs = {
    .set_config             = drm_atomic_helper_set_config,
    .page_flip              = drm_atomic_helper_page_flip,
    .reset                  = nv_drm_atomic_crtc_reset,
    .destroy                = nv_drm_crtc_destroy,
    .atomic_get_property    = nv_drm_atomic_crtc_get_property,
    .atomic_set_property    = nv_drm_atomic_crtc_set_property,
    .atomic_duplicate_state = nv_drm_atomic_crtc_duplicate_state,
    .atomic_destroy_state   = nv_drm_atomic_crtc_destroy_state,
#if defined(NV_DRM_ATOMIC_HELPER_LEGACY_GAMMA_SET_PRESENT)
    .gamma_set = drm_atomic_helper_legacy_gamma_set,
#endif
};

/*
 * In kernel versions before the addition of
 * drm_crtc_state::connectors_changed, connector changes were
 * reflected in drm_crtc_state::mode_changed.
 */
static inline bool
nv_drm_crtc_state_connectors_changed(struct drm_crtc_state *crtc_state)
{
#if defined(NV_DRM_CRTC_STATE_HAS_CONNECTORS_CHANGED)
    return crtc_state->connectors_changed;
#else
    return crtc_state->mode_changed;
#endif
}

static int head_modeset_config_attach_connector(
    struct nv_drm_connector *nv_connector,
    struct NvKmsKapiHeadModeSetConfig *head_modeset_config)
{
    struct nv_drm_encoder *nv_encoder = nv_connector->nv_detected_encoder;

    if (NV_DRM_WARN(nv_encoder == NULL ||
                    head_modeset_config->numDisplays >=
                        ARRAY_SIZE(head_modeset_config->displays))) {
        return -EINVAL;
    }
    head_modeset_config->displays[head_modeset_config->numDisplays++] =
        nv_encoder->hDisplay;
    return 0;
}

#if defined(NV_DRM_COLOR_MGMT_AVAILABLE)
static int color_mgmt_config_copy_lut(struct NvKmsLutRamps *nvkms_lut,
                                      struct drm_color_lut *drm_lut,
                                      uint64_t lut_len)
{
    uint64_t i = 0;
    if (lut_len != NVKMS_LUT_ARRAY_SIZE) {
        return -EINVAL;
    }

    /*
     * Both NvKms and drm LUT values are 16-bit linear values. NvKms LUT ramps
     * are in arrays in a single struct while drm LUT ramps are an array of
     * structs.
     */
    for (i = 0; i < lut_len; i++) {
        nvkms_lut->red[i]   = drm_lut[i].red;
        nvkms_lut->green[i] = drm_lut[i].green;
        nvkms_lut->blue[i]  = drm_lut[i].blue;
    }
    return 0;
}

static int color_mgmt_config_set_luts(struct nv_drm_crtc_state *nv_crtc_state,
                                      struct NvKmsKapiHeadRequestedConfig *req_config)
{
    struct NvKmsKapiHeadModeSetConfig *modeset_config =
        &req_config->modeSetConfig;
    struct drm_crtc_state *crtc_state = &nv_crtc_state->base;
    int ret = 0;

    /*
     * According to the comment in the Linux kernel's
     * drivers/gpu/drm/drm_color_mgmt.c, if either property is NULL, that LUT
     * needs to be changed to a linear LUT
     *
     * On failure, any LUT ramps allocated in this function are freed when the
     * subsequent atomic state cleanup calls nv_drm_atomic_crtc_destroy_state.
     */

    if (crtc_state->degamma_lut) {
        struct drm_color_lut *degamma_lut = NULL;
        uint64_t degamma_len = 0;

        if (!modeset_config->lut.input.pRamps) {
            modeset_config->lut.input.pRamps =
                nv_drm_calloc(1, sizeof(*modeset_config->lut.input.pRamps));
            if (!modeset_config->lut.input.pRamps) {
                return -ENOMEM;
            }
        }

        degamma_lut = (struct drm_color_lut *)crtc_state->degamma_lut->data;
        degamma_len = crtc_state->degamma_lut->length /
                      sizeof(struct drm_color_lut);

        if ((ret = color_mgmt_config_copy_lut(modeset_config->lut.input.pRamps,
                                              degamma_lut,
                                              degamma_len)) != 0) {
            return ret;
        }

        modeset_config->lut.input.depth     = 30; /* specify the full LUT */
        modeset_config->lut.input.start     = 0;
        modeset_config->lut.input.end       = degamma_len - 1;
    } else {
        /* setting input.end to 0 is equivalent to disabling the LUT, which
         * should be equivalent to a linear LUT */
        modeset_config->lut.input.depth     = 30; /* specify the full LUT */
        modeset_config->lut.input.start     = 0;
        modeset_config->lut.input.end       = 0;

        nv_drm_free(modeset_config->lut.input.pRamps);
        modeset_config->lut.input.pRamps    = NULL;
    }
    req_config->flags.legacyIlutChanged = NV_TRUE;

    if (crtc_state->gamma_lut) {
        struct drm_color_lut *gamma_lut = NULL;
        uint64_t gamma_len = 0;

        if (!modeset_config->lut.output.pRamps) {
            modeset_config->lut.output.pRamps =
                nv_drm_calloc(1, sizeof(*modeset_config->lut.output.pRamps));
            if (!modeset_config->lut.output.pRamps) {
                return -ENOMEM;
            }
        }

        gamma_lut = (struct drm_color_lut *)crtc_state->gamma_lut->data;
        gamma_len = crtc_state->gamma_lut->length /
                    sizeof(struct drm_color_lut);

        if ((ret = color_mgmt_config_copy_lut(modeset_config->lut.output.pRamps,
                                              gamma_lut,
                                              gamma_len)) != 0) {
            return ret;
        }

        modeset_config->lut.output.enabled   = NV_TRUE;
    } else {
        /* disabling the output LUT should be equivalent to setting a linear
         * LUT */
        modeset_config->lut.output.enabled   = NV_FALSE;

        nv_drm_free(modeset_config->lut.output.pRamps);
        modeset_config->lut.output.pRamps    = NULL;
    }
    req_config->flags.legacyOlutChanged = NV_TRUE;

    return 0;
}
#endif /* NV_DRM_COLOR_MGMT_AVAILABLE */

/**
 * nv_drm_crtc_atomic_check() can fail after it has modified
 * the 'nv_drm_crtc_state::req_config', that is fine because 'nv_drm_crtc_state'
 * will be discarded if ->atomic_check() fails.
 */
#if defined(NV_DRM_CRTC_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG)
static int nv_drm_crtc_atomic_check(struct drm_crtc *crtc,
                                    struct drm_atomic_state *state)
#else
static int nv_drm_crtc_atomic_check(struct drm_crtc *crtc,
                                    struct drm_crtc_state *crtc_state)
#endif
{
#if defined(NV_DRM_CRTC_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG)
    struct drm_crtc_state *crtc_state =
        drm_atomic_get_new_crtc_state(state, crtc);
#endif
    struct nv_drm_crtc *nv_crtc = to_nv_crtc(crtc);
    struct nv_drm_device *nv_dev = to_nv_device(crtc->dev);
    struct nv_drm_crtc_state *nv_crtc_state = to_nv_crtc_state(crtc_state);
    struct NvKmsKapiHeadRequestedConfig *req_config =
        &nv_crtc_state->req_config;
    int ret = 0;

    if (crtc_state->mode_changed) {
        drm_mode_to_nvkms_display_mode(&crtc_state->mode,
                                       &req_config->modeSetConfig.mode);
        req_config->flags.modeChanged = NV_TRUE;
    }

    if (nv_drm_crtc_state_connectors_changed(crtc_state)) {
        struct NvKmsKapiHeadModeSetConfig *config = &req_config->modeSetConfig;
        struct drm_connector *connector;
        struct drm_connector_state *connector_state;
        int j;

        config->numDisplays = 0;

        memset(config->displays, 0, sizeof(config->displays));

        req_config->flags.displaysChanged = NV_TRUE;

        nv_drm_for_each_connector_in_state(crtc_state->state,
                                           connector, connector_state, j) {
            if (connector_state->crtc != crtc) {
                continue;
            }

            if ((ret = head_modeset_config_attach_connector(
                            to_nv_connector(connector),
                            config)) != 0) {
                return ret;
            }
        }
    }

    if (crtc_state->active_changed) {
        req_config->modeSetConfig.bActive = crtc_state->active;
        req_config->flags.activeChanged = NV_TRUE;
    }

#if defined(NV_DRM_CRTC_STATE_HAS_VRR_ENABLED)
    req_config->modeSetConfig.vrrEnabled = crtc_state->vrr_enabled;
#endif

#if defined(NV_DRM_COLOR_MGMT_AVAILABLE)
    if (crtc_state->color_mgmt_changed) {
        if ((ret = color_mgmt_config_set_luts(nv_crtc_state, req_config)) != 0) {
            return ret;
        }
    }
#endif

    if (nv_crtc_state->regamma_changed) {
        if (nv_crtc_state->regamma_drm_lut_surface != NULL) {
            kref_put(&nv_crtc_state->regamma_drm_lut_surface->base.refcount,
                     free_drm_lut_surface);
            nv_crtc_state->regamma_drm_lut_surface = NULL;
        }

        if (nv_crtc->olut_caps.vssSupport == NVKMS_LUT_VSS_SUPPORTED) {
            if ((nv_crtc_state->regamma_tf  != NV_DRM_TRANSFER_FUNCTION_DEFAULT) ||
                (nv_crtc_state->regamma_lut != NULL)) {

                nv_crtc_state->regamma_drm_lut_surface =
                    create_drm_olut_surface_vss(nv_dev, nv_crtc,
                                                nv_crtc_state);
                if (nv_crtc_state->regamma_drm_lut_surface == NULL) {
                    return -1;
                }
            }
        } else {
            WARN_ON(nv_crtc->olut_caps.vssSupport != NVKMS_LUT_VSS_NOT_SUPPORTED);
            if (nv_crtc_state->regamma_lut != NULL) {
                nv_crtc_state->regamma_drm_lut_surface =
                    create_drm_olut_surface_legacy(nv_dev, nv_crtc,
                                                   nv_crtc_state);
                if (nv_crtc_state->regamma_drm_lut_surface == NULL) {
                    return -1;
                }
            }
        }

        if (nv_crtc_state->regamma_drm_lut_surface != NULL) {
            req_config->modeSetConfig.olut.enabled = NV_TRUE;
            req_config->modeSetConfig.olut.lutSurface =
                nv_crtc_state->regamma_drm_lut_surface->base.nvkms_surface;
            req_config->modeSetConfig.olut.offset = 0;
            req_config->modeSetConfig.olut.vssSegments =
                nv_crtc_state->regamma_drm_lut_surface->properties.vssSegments;
            req_config->modeSetConfig.olut.lutEntries =
                nv_crtc_state->regamma_drm_lut_surface->properties.lutEntries;
        } else {
            req_config->modeSetConfig.olut.enabled = NV_FALSE;
            req_config->modeSetConfig.olut.lutSurface = NULL;
            req_config->modeSetConfig.olut.offset = 0;
            req_config->modeSetConfig.olut.vssSegments = 0;
            req_config->modeSetConfig.olut.lutEntries = 0;
        }
        req_config->flags.olutChanged = NV_TRUE;

        /*
         * Range property is configured to ensure sign bit = 0 and
         * value is >= 1, but it may still default to 0 if it's unsupported.
         */
        WARN_ON(nv_crtc_state->regamma_divisor & (((NvU64) 1) << 63));

        req_config->flags.olutFpNormScaleChanged = NV_TRUE;
        if (nv_crtc_state->regamma_divisor < (((NvU64) 1) << 32)) {
            req_config->modeSetConfig.olutFpNormScale =
                NVKMS_OLUT_FP_NORM_SCALE_DEFAULT;
        } else {
            /*
             * Since the sign bit of the regamma_divisor is unset, we treat it as
             * unsigned and do 32.32 unsigned fixed-point division to get the
             * fpNormScale.
             */
            req_config->modeSetConfig.olutFpNormScale =
                (NvU32)(((NvU64)NVKMS_OLUT_FP_NORM_SCALE_DEFAULT << 32) /
                        nv_crtc_state->regamma_divisor);
        }
    }

    return ret;
}

static bool
nv_drm_crtc_mode_fixup(struct drm_crtc *crtc,
                       const struct drm_display_mode *mode,
                       struct drm_display_mode *adjusted_mode)
{
    return true;
}

static const struct drm_crtc_helper_funcs nv_crtc_helper_funcs = {
    .atomic_check = nv_drm_crtc_atomic_check,
    .mode_fixup = nv_drm_crtc_mode_fixup,
};

static void nv_drm_crtc_install_properties(
    struct drm_crtc *crtc)
{
    struct nv_drm_device *nv_dev = to_nv_device(crtc->dev);
    struct nv_drm_crtc *nv_crtc = to_nv_crtc(crtc);
    struct nv_drm_crtc_state *nv_crtc_state = to_nv_crtc_state(crtc->state);

    if (nv_crtc->olut_caps.supported) {
        if (nv_crtc->olut_caps.vssSupport == NVKMS_LUT_VSS_SUPPORTED) {
            if (nv_dev->nv_crtc_regamma_tf_property) {
                drm_object_attach_property(
                    &crtc->base, nv_dev->nv_crtc_regamma_tf_property,
                    NV_DRM_TRANSFER_FUNCTION_DEFAULT);
            }
            if (nv_dev->nv_crtc_regamma_divisor_property) {
                /* Default to 1 */
                nv_crtc_state->regamma_divisor = (((NvU64) 1) << 32);
                drm_object_attach_property(
                    &crtc->base, nv_dev->nv_crtc_regamma_divisor_property,
                    nv_crtc_state->regamma_divisor);
            }
        }
        if (nv_dev->nv_crtc_regamma_lut_property) {
            drm_object_attach_property(
                &crtc->base, nv_dev->nv_crtc_regamma_lut_property, 0);
        }
        if (nv_dev->nv_crtc_regamma_lut_size_property) {
            drm_object_attach_property(
                &crtc->base, nv_dev->nv_crtc_regamma_lut_size_property,
                NVKMS_LUT_ARRAY_SIZE);
        }
    }
}

static void nv_drm_plane_install_properties(
    struct drm_plane *plane,
    NvBool supportsICtCp)
{
    struct nv_drm_device *nv_dev = to_nv_device(plane->dev);
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    struct nv_drm_plane_state *nv_plane_state =
        to_nv_drm_plane_state(plane->state);

    if (nv_dev->nv_out_fence_property) {
        drm_object_attach_property(
            &plane->base, nv_dev->nv_out_fence_property, 0);
    }

    if (nv_dev->nv_input_colorspace_property) {
        drm_object_attach_property(
            &plane->base, nv_dev->nv_input_colorspace_property,
            NV_DRM_INPUT_COLOR_SPACE_NONE);
    }

    if (supportsICtCp) {
#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
        if (nv_dev->nv_hdr_output_metadata_property) {
            drm_object_attach_property(
                &plane->base, nv_dev->nv_hdr_output_metadata_property, 0);
        }
#endif
    }

    /*
     * Per-plane HDR properties get us dangerously close to the 24 property
     * limit on kernels that don't support NV_DRM_USE_EXTENDED_PROPERTIES.
     */
    if (NV_DRM_USE_EXTENDED_PROPERTIES) {
        if (supportsICtCp) {
            if (nv_dev->nv_plane_lms_ctm_property) {
                drm_object_attach_property(
                    &plane->base, nv_dev->nv_plane_lms_ctm_property, 0);
            }

            if (nv_dev->nv_plane_lms_to_itp_ctm_property) {
                drm_object_attach_property(
                    &plane->base, nv_dev->nv_plane_lms_to_itp_ctm_property, 0);
            }

            if (nv_dev->nv_plane_itp_to_lms_ctm_property) {
                drm_object_attach_property(
                    &plane->base, nv_dev->nv_plane_itp_to_lms_ctm_property, 0);
            }

            WARN_ON(!nv_plane->tmo_caps.supported);
            if (nv_dev->nv_plane_tmo_lut_property) {
                drm_object_attach_property(
                    &plane->base, nv_dev->nv_plane_tmo_lut_property, 0);
            }
            if (nv_dev->nv_plane_tmo_lut_size_property) {
                drm_object_attach_property(
                    &plane->base, nv_dev->nv_plane_tmo_lut_size_property,
                    NVKMS_LUT_ARRAY_SIZE);
            }
        }

        if (nv_dev->nv_plane_blend_ctm_property) {
            drm_object_attach_property(
                &plane->base, nv_dev->nv_plane_blend_ctm_property, 0);
        }

        if (nv_plane->ilut_caps.supported) {
            if (nv_plane->ilut_caps.vssSupport == NVKMS_LUT_VSS_SUPPORTED) {
                if (nv_dev->nv_plane_degamma_tf_property) {
                    drm_object_attach_property(
                        &plane->base, nv_dev->nv_plane_degamma_tf_property,
                        NV_DRM_TRANSFER_FUNCTION_DEFAULT);
                }
                if (nv_dev->nv_plane_degamma_multiplier_property) {
                    /* Default to 1 in S31.32 Sign-Magnitude Format */
                    nv_plane_state->degamma_multiplier = ((uint64_t) 1) << 32;
                    drm_object_attach_property(
                        &plane->base, nv_dev->nv_plane_degamma_multiplier_property,
                        nv_plane_state->degamma_multiplier);
                }
            }
            if (nv_dev->nv_plane_degamma_lut_property) {
                drm_object_attach_property(
                    &plane->base, nv_dev->nv_plane_degamma_lut_property, 0);
            }
            if (nv_dev->nv_plane_degamma_lut_size_property) {
                drm_object_attach_property(
                    &plane->base, nv_dev->nv_plane_degamma_lut_size_property,
                    NVKMS_LUT_ARRAY_SIZE);
            }
        }
    }
}

static void
__nv_drm_plane_create_alpha_blending_properties(struct drm_plane *plane,
                                                 NvU32 validCompModes)
{
#if defined(NV_DRM_ALPHA_BLENDING_AVAILABLE)
    if ((validCompModes &
         NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA)) != 0x0 &&
        (validCompModes &
         NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA)) != 0x0) {

        drm_plane_create_alpha_property(plane);
        drm_plane_create_blend_mode_property(plane,
                                             NVBIT(DRM_MODE_BLEND_PREMULTI) |
                                             NVBIT(DRM_MODE_BLEND_COVERAGE));
    } else if ((validCompModes &
                NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA)) != 0x0 &&
               (validCompModes &
                NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA)) != 0x0) {

        drm_plane_create_blend_mode_property(plane,
                                             NVBIT(DRM_MODE_BLEND_PREMULTI) |
                                             NVBIT(DRM_MODE_BLEND_COVERAGE));
   }
#endif
}

static void
__nv_drm_plane_create_rotation_property(struct drm_plane *plane,
                                        NvU16 validLayerRRTransforms)
{
#if defined(NV_DRM_ROTATION_AVAILABLE)
    enum NvKmsRotation curRotation;
    NvU32 supported_rotations = 0;
    struct NvKmsRRParams rrParams = {
        .rotation = NVKMS_ROTATION_0,
        .reflectionX = true,
        .reflectionY = true,
    };

    if ((NVBIT(NvKmsRRParamsToCapBit(&rrParams)) &
        validLayerRRTransforms) != 0) {
        supported_rotations |= DRM_MODE_REFLECT_X;
        supported_rotations |= DRM_MODE_REFLECT_Y;
    }

    rrParams.reflectionX = false;
    rrParams.reflectionY = false;

    for (curRotation = NVKMS_ROTATION_MIN;
         curRotation <= NVKMS_ROTATION_MAX; curRotation++) {
        rrParams.rotation = curRotation;
        if ((NVBIT(NvKmsRRParamsToCapBit(&rrParams)) &
                    validLayerRRTransforms) == 0) {
            continue;
        }

        switch (curRotation) {
            case NVKMS_ROTATION_0:
                supported_rotations |= DRM_MODE_ROTATE_0;
                break;
            case NVKMS_ROTATION_90:
                supported_rotations |= DRM_MODE_ROTATE_90;
                break;
            case NVKMS_ROTATION_180:
                supported_rotations |= DRM_MODE_ROTATE_180;
                break;
            case NVKMS_ROTATION_270:
                supported_rotations |= DRM_MODE_ROTATE_270;
                break;
            default:
                break;
        }

    }

    if (supported_rotations != 0) {
        drm_plane_create_rotation_property(plane, DRM_MODE_ROTATE_0,
                                           supported_rotations);
    }
#endif
}

static struct drm_plane*
nv_drm_plane_create(struct drm_device *dev,
                    enum drm_plane_type plane_type,
                    uint32_t layer_idx,
                    NvU32 head,
                    const struct NvKmsKapiDeviceResourcesInfo *pResInfo)
{
#if defined(NV_DRM_UNIVERSAL_PLANE_INIT_HAS_FORMAT_MODIFIERS_ARG)
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    const NvU64 linear_modifiers[] = {
        DRM_FORMAT_MOD_LINEAR,
        DRM_FORMAT_MOD_INVALID,
    };
#endif
    enum NvKmsCompositionBlendingMode defaultCompositionMode;
    struct nv_drm_plane *nv_plane = NULL;
    struct nv_drm_plane_state *nv_plane_state = NULL;
    struct drm_plane *plane = NULL;
    int ret = -ENOMEM;
    uint32_t *formats = NULL;
    unsigned int formats_count = 0;
    const NvU32 validCompositionModes =
        (plane_type == DRM_PLANE_TYPE_CURSOR) ?
            pResInfo->caps.validCursorCompositionModes :
            pResInfo->caps.layer[layer_idx].validCompositionModes;
    const long unsigned int nvkms_formats_mask =
        (plane_type == DRM_PLANE_TYPE_CURSOR) ?
            pResInfo->caps.supportedCursorSurfaceMemoryFormats :
            pResInfo->supportedSurfaceMemoryFormats[layer_idx];
    const NvU16 validLayerRRTransforms =
        (plane_type == DRM_PLANE_TYPE_CURSOR) ?
            0x0 : pResInfo->caps.layer[layer_idx].validRRTransforms;

    if ((validCompositionModes &
         NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE)) != 0x0) {
        defaultCompositionMode = NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE;
    } else if ((validCompositionModes &
                NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA)) != 0x0) {
        defaultCompositionMode = NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA;
    } else {
        goto failed;
    }

    formats =
        nv_drm_format_array_alloc(&formats_count,
                                  nvkms_formats_mask);
    if (formats == NULL) {
        goto failed;
    }

    if ((nv_plane = nv_drm_calloc(1, sizeof(*nv_plane))) == NULL) {
        goto failed_plane_alloc;
    }
    plane = &nv_plane->base;

    nv_plane->defaultCompositionMode = defaultCompositionMode;
    nv_plane->layer_idx = layer_idx;

    if ((nv_plane_state =
            nv_drm_calloc(1, sizeof(*nv_plane_state))) == NULL) {
        goto failed_state_alloc;
    }

    plane->state = &nv_plane_state->base;
    plane->state->plane = plane;

    /*
     * Possible_crtcs for primary and cursor plane is zero because
     * drm_crtc_init_with_planes() will assign the plane's possible_crtcs
     * after the crtc is successfully initialized.
     */
    ret = drm_universal_plane_init(
        dev,
        plane,
        (plane_type == DRM_PLANE_TYPE_OVERLAY) ?
        (1 << head) : 0,
        &nv_plane_funcs,
        formats, formats_count,
#if defined(NV_DRM_UNIVERSAL_PLANE_INIT_HAS_FORMAT_MODIFIERS_ARG)
        (plane_type == DRM_PLANE_TYPE_CURSOR) ?
        linear_modifiers : nv_dev->modifiers,
#endif
        plane_type
#if defined(NV_DRM_UNIVERSAL_PLANE_INIT_HAS_NAME_ARG)
        , NULL
#endif
        );

    if (ret != 0) {
        goto failed_plane_init;
    }

#if defined(NV_DRM_PLANE_CREATE_COLOR_PROPERTIES_PRESENT)
    if (pResInfo->caps.supportsInputColorSpace &&
        pResInfo->caps.supportsInputColorRange) {

        nv_plane->supportsColorProperties = true;

        drm_plane_create_color_properties(
            plane,
            NVBIT(DRM_COLOR_YCBCR_BT601) |
            NVBIT(DRM_COLOR_YCBCR_BT709) |
            NVBIT(DRM_COLOR_YCBCR_BT2020),
            NVBIT(DRM_COLOR_YCBCR_FULL_RANGE) |
            NVBIT(DRM_COLOR_YCBCR_LIMITED_RANGE),
            DRM_COLOR_YCBCR_BT709,
            DRM_COLOR_YCBCR_FULL_RANGE
        );
    } else {
        nv_plane->supportsColorProperties = false;
    }
#else
    nv_plane->supportsColorProperties = false;
#endif

    drm_plane_helper_add(plane, &nv_plane_helper_funcs);

    if (plane_type != DRM_PLANE_TYPE_CURSOR) {
        nv_plane->ilut_caps = pResInfo->lutCaps.layer[layer_idx].ilut;
        nv_plane->tmo_caps = pResInfo->lutCaps.layer[layer_idx].tmo;

        nv_drm_plane_install_properties(
                plane,
                pResInfo->supportsICtCp[layer_idx]);
    }

    __nv_drm_plane_create_alpha_blending_properties(
            plane,
            validCompositionModes);

    __nv_drm_plane_create_rotation_property(
            plane,
            validLayerRRTransforms);

    nv_drm_free(formats);

    return plane;

failed_plane_init:
    nv_drm_free(nv_plane_state);

failed_state_alloc:
    nv_drm_free(nv_plane);

failed_plane_alloc:
    nv_drm_free(formats);

failed:
    return ERR_PTR(ret);
}

/*
 * Add drm crtc for given head and supported enum NvKmsSurfaceMemoryFormats.
 */
static struct drm_crtc *__nv_drm_crtc_create(struct nv_drm_device *nv_dev,
                                             struct drm_plane *primary_plane,
                                             struct drm_plane *cursor_plane,
                                             unsigned int head,
                                             const struct NvKmsKapiDeviceResourcesInfo *pResInfo)
{
    struct nv_drm_crtc *nv_crtc = NULL;
    struct nv_drm_crtc_state *nv_state = NULL;
    int ret = -ENOMEM;

    if ((nv_crtc = nv_drm_calloc(1, sizeof(*nv_crtc))) == NULL) {
        goto failed;
    }

    nv_state = nv_drm_crtc_state_alloc();
    if (nv_state == NULL) {
        goto failed_state_alloc;
    }

    nv_crtc->base.state = &nv_state->base;
    nv_crtc->base.state->crtc = &nv_crtc->base;

    nv_crtc->head = head;
    INIT_LIST_HEAD(&nv_crtc->flip_list);
    spin_lock_init(&nv_crtc->flip_list_lock);
    nv_crtc->modeset_permission_filep = NULL;

    ret = drm_crtc_init_with_planes(nv_dev->dev,
                                    &nv_crtc->base,
                                    primary_plane, cursor_plane,
                                    &nv_crtc_funcs
#if defined(NV_DRM_CRTC_INIT_WITH_PLANES_HAS_NAME_ARG)
                                    , NULL
#endif
                                    );

    if (ret != 0) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to init crtc for head %u with planes", head);
        goto failed_init_crtc;
    }

    /* Add crtc to drm sub-system */

    drm_crtc_helper_add(&nv_crtc->base, &nv_crtc_helper_funcs);

    nv_crtc->olut_caps = pResInfo->lutCaps.olut;

    nv_drm_crtc_install_properties(&nv_crtc->base);

#if defined(NV_DRM_COLOR_MGMT_AVAILABLE)
#if defined(NV_DRM_CRTC_ENABLE_COLOR_MGMT_PRESENT)
    drm_crtc_enable_color_mgmt(&nv_crtc->base, NVKMS_LUT_ARRAY_SIZE, true,
                               NVKMS_LUT_ARRAY_SIZE);
#else
    drm_helper_crtc_enable_color_mgmt(&nv_crtc->base, NVKMS_LUT_ARRAY_SIZE,
                                      NVKMS_LUT_ARRAY_SIZE);
#endif
    ret = drm_mode_crtc_set_gamma_size(&nv_crtc->base, NVKMS_LUT_ARRAY_SIZE);
    if (ret != 0) {
        NV_DRM_DEV_LOG_WARN(
            nv_dev,
            "Failed to initialize legacy gamma support for head %u", head);
    }
#endif

    return &nv_crtc->base;

failed_init_crtc:
    nv_drm_free(nv_state);

failed_state_alloc:
    nv_drm_free(nv_crtc);

failed:
    return ERR_PTR(ret);
}

void nv_drm_enumerate_crtcs_and_planes(
    struct nv_drm_device *nv_dev,
    const struct NvKmsKapiDeviceResourcesInfo *pResInfo)
{
    unsigned int i;

    for (i = 0; i < pResInfo->numHeads; i++) {
        struct drm_plane *primary_plane = NULL, *cursor_plane = NULL;
        NvU32 layer;

        if (pResInfo->numLayers[i] <= NVKMS_KAPI_LAYER_PRIMARY_IDX) {
            continue;
        }

        primary_plane =
            nv_drm_plane_create(nv_dev->dev,
                                DRM_PLANE_TYPE_PRIMARY,
                                NVKMS_KAPI_LAYER_PRIMARY_IDX,
                                i,
                                pResInfo);

        if (IS_ERR(primary_plane)) {
            NV_DRM_DEV_LOG_ERR(
                nv_dev,
                "Failed to create primary plane for head %u, error = %ld",
                i, PTR_ERR(primary_plane));
            continue;
        }

        cursor_plane =
            nv_drm_plane_create(nv_dev->dev,
                                DRM_PLANE_TYPE_CURSOR,
                                NVKMS_KAPI_LAYER_INVALID_IDX,
                                i,
                                pResInfo);
        if (IS_ERR(cursor_plane)) {
            NV_DRM_DEV_LOG_ERR(
                nv_dev,
                "Failed to create cursor plane for head %u, error = %ld",
                i, PTR_ERR(cursor_plane));
            cursor_plane = NULL;
        }

        /* Create crtc with the primary and cursor planes */
        {
            struct drm_crtc *crtc =
                __nv_drm_crtc_create(nv_dev,
                                     primary_plane, cursor_plane,
                                     i, pResInfo);
            if (IS_ERR(crtc)) {
                nv_drm_plane_destroy(primary_plane);

                if (cursor_plane != NULL) {
                    nv_drm_plane_destroy(cursor_plane);
                }

                NV_DRM_DEV_LOG_ERR(
                    nv_dev,
                    "Failed to add DRM CRTC for head %u, error = %ld",
                    i, PTR_ERR(crtc));
                continue;
            }
        }

        for (layer = 0; layer < pResInfo->numLayers[i]; layer++) {
            struct drm_plane *overlay_plane = NULL;

            if (layer == NVKMS_KAPI_LAYER_PRIMARY_IDX) {
                continue;
            }

            overlay_plane =
                nv_drm_plane_create(nv_dev->dev,
                                    DRM_PLANE_TYPE_OVERLAY,
                                    layer,
                                    i,
                                    pResInfo);

            if (IS_ERR(overlay_plane)) {
                NV_DRM_DEV_LOG_ERR(
                    nv_dev,
                    "Failed to create plane for layer-%u of head %u, error = %ld",
                    layer, i, PTR_ERR(overlay_plane));
            }
        }

    }
}
/*
 * Helper function to convert NvKmsKapiCrcs to drm_nvidia_crtc_crc32_out.
 */
static void NvKmsKapiCrcsToDrm(const struct NvKmsKapiCrcs *crcs,
                               struct drm_nvidia_crtc_crc32_v2_out *drmCrcs)
{
    drmCrcs->outputCrc32.value = crcs->outputCrc32.value;
    drmCrcs->outputCrc32.supported = crcs->outputCrc32.supported;
    drmCrcs->outputCrc32.__pad0 = 0;
    drmCrcs->outputCrc32.__pad1 = 0;
    drmCrcs->rasterGeneratorCrc32.value = crcs->rasterGeneratorCrc32.value;
    drmCrcs->rasterGeneratorCrc32.supported = crcs->rasterGeneratorCrc32.supported;
    drmCrcs->rasterGeneratorCrc32.__pad0 = 0;
    drmCrcs->rasterGeneratorCrc32.__pad1 = 0;
    drmCrcs->compositorCrc32.value = crcs->compositorCrc32.value;
    drmCrcs->compositorCrc32.supported = crcs->compositorCrc32.supported;
    drmCrcs->compositorCrc32.__pad0 = 0;
    drmCrcs->compositorCrc32.__pad1 = 0;
}

int nv_drm_get_crtc_crc32_v2_ioctl(struct drm_device *dev,
                                   void *data, struct drm_file *filep)
{
    struct drm_nvidia_get_crtc_crc32_v2_params *params = data;
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_crtc *crtc = NULL;
    struct nv_drm_crtc *nv_crtc = NULL;
    struct NvKmsKapiCrcs crc32;

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return -EOPNOTSUPP;
    }

    crtc = nv_drm_crtc_find(dev, filep, params->crtc_id);
    if (!crtc) {
        return -ENOENT;
    }

    nv_crtc = to_nv_crtc(crtc);

    if (!nvKms->getCRC32(nv_dev->pDevice, nv_crtc->head, &crc32)) {
        return -ENODEV;
    }
    NvKmsKapiCrcsToDrm(&crc32, &params->crc32);

    return 0;
}

int nv_drm_get_crtc_crc32_ioctl(struct drm_device *dev,
                                void *data, struct drm_file *filep)
{
    struct drm_nvidia_get_crtc_crc32_params *params = data;
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_crtc *crtc = NULL;
    struct nv_drm_crtc *nv_crtc = NULL;
    struct NvKmsKapiCrcs crc32;

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return -EOPNOTSUPP;
    }

    crtc = nv_drm_crtc_find(dev, filep, params->crtc_id);
    if (!crtc) {
        return -ENOENT;
    }

    nv_crtc = to_nv_crtc(crtc);

    if (!nvKms->getCRC32(nv_dev->pDevice, nv_crtc->head, &crc32)) {
        return -ENODEV;
    }
    params->crc32 = crc32.outputCrc32.value;

    return 0;
}

#endif
