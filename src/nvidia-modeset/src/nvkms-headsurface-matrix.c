/*
 * SPDX-FileCopyrightText: Copyright (c) 2017 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "nvkms-headsurface-matrix.h"
#include "nvkms-softfloat.h"
#include "nv-float.h"

/*!
 * Multiply the matrices 'l' and 'r', and return the result.
 */
static struct NvKmsMatrixF32 MultiplyMatrix(
    const struct NvKmsMatrixF32 *l,
    const struct NvKmsMatrixF32 *r)
{
    struct NvKmsMatrixF32 d = { };
    int dx, dy;

    for (dy = 0; dy < 3; dy++) {
        for (dx = 0; dx < 3; dx++) {
            float32_t v = NvU32viewAsF32(NV_FLOAT_ZERO);
            int o;
            for (o = 0; o < 3; o++) {
                const float32_t a = f32_mul(l->m[dy][o], r->m[o][dx]);
                v = f32_add(v, a);
            }
            d.m[dy][dx] = v;
        }
    }

    return d;
}

/*!
 * Generate a matrix that performs the translation described by tx in x and ty
 * in y.
 */
static struct NvKmsMatrixF32 GenTranslationMatrix(
    float32_t tx,
    float32_t ty)
{
    struct NvKmsMatrixF32 ret = { };

    ret.m[0][0] = NvU32viewAsF32(NV_FLOAT_ONE);
    ret.m[0][1] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[0][2] = tx;
    ret.m[1][0] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[1][1] = NvU32viewAsF32(NV_FLOAT_ONE);
    ret.m[1][2] = ty;
    ret.m[2][0] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[2][1] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[2][2] = NvU32viewAsF32(NV_FLOAT_ONE);

    return ret;
}

/*!
 * Generate a matrix that performs the scaling operation described by sx in x
 * and sy in y.
 */
static struct NvKmsMatrixF32 GenScaleMatrix(
    float32_t sx,
    float32_t sy)
{
    struct NvKmsMatrixF32 ret = { };

    ret.m[0][0] = sx;
    ret.m[0][1] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[0][2] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[1][0] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[1][1] = sy;
    ret.m[1][2] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[2][0] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[2][1] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[2][2] = NvU32viewAsF32(NV_FLOAT_ONE);

    return ret;
}

/*!
 * Generate a matrix that performs the rotation operation described by cosine
 * 'c' and sine 's'.
 */
static struct NvKmsMatrixF32 GenRotationMatrix(
    float32_t c,
    float32_t s)
{
    struct NvKmsMatrixF32 ret = { };
    const float32_t negOneF32 = NvU32viewAsF32(NV_FLOAT_NEG_ONE);

    ret.m[0][0] = c;
    ret.m[0][1] = f32_mul(s, negOneF32);
    ret.m[0][2] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[1][0] = s;
    ret.m[1][1] = c;
    ret.m[1][2] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[2][0] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[2][1] = NvU32viewAsF32(NV_FLOAT_ZERO);
    ret.m[2][2] = NvU32viewAsF32(NV_FLOAT_ONE);

    return ret;
}

/*!
 * Generate the identity matrix.
 */
static struct NvKmsMatrixF32 GenIdentityMatrix(void)
{
    return GenScaleMatrix(NvU32viewAsF32(NV_FLOAT_ONE),
                          NvU32viewAsF32(NV_FLOAT_ONE));
}

/*!
 * Transform x,y,q with a 3x3 affine transformation matrix (row-major).
 */
static inline void TransformVertex(
    const struct NvKmsMatrixF32 *mat,
    float32_t *pX,
    float32_t *pY,
    float32_t *pQ)
{
    const float32_t x_in = *pX;
    const float32_t y_in = *pY;
    const float32_t oneF32 = NvU32viewAsF32(NV_FLOAT_ONE);
    float32_t w, oneOverW, x, y;

    x = F32_AxB_plus_CxD_plus_E(x_in, mat->m[0][0],
                                y_in, mat->m[0][1],
                                      mat->m[0][2]);
    y = F32_AxB_plus_CxD_plus_E(x_in, mat->m[1][0],
                                y_in, mat->m[1][1],
                                      mat->m[1][2]);
    w = F32_AxB_plus_CxD_plus_E(x_in, mat->m[2][0],
                                y_in, mat->m[2][1],
                                      mat->m[2][2]);
    oneOverW = f32_div(oneF32, w);

    x = f32_mul(x, oneOverW);
    y = f32_mul(y, oneOverW);

    *pX = x;
    *pY = y;
    *pQ = oneOverW;
}

/*!
 * Transform pRectIn by the matrix, returning the result in pRectOut.
 *
 * XXX If we knew the matrix would produce a screen-aligned rectangle, we
 * wouldn't need to transform as many points.
 *
 * XXX We should probably compute the screen-aligned rect inscribed by the
 * transformed points, rather than compute the bounding box.
 */
static struct NvKmsRect TransformRect(
    const struct NvKmsRect *pRectIn,
    const struct NvKmsMatrixF32 *transform)
{
    /*
     * Get the four corners of pRectIn:
     *
     * 0            3
     *  +----------+
     *  |          |
     *  +----------+
     * 1            2
     */
    struct NvKmsPoint p[4] = {
        [0] = {
            .x = pRectIn->x,
            .y = pRectIn->y,
        },
        [1] = {
            .x = pRectIn->x,
            .y = pRectIn->y + pRectIn->height,
        },
        [2] = {
            .x = pRectIn->x + pRectIn->width,
            .y = pRectIn->y + pRectIn->height,
        },
        [3] = {
            .x = pRectIn->x + pRectIn->width,
            .y = pRectIn->y,
        },
    };

    NvU8 i;
    NvU16 minx, maxx, miny, maxy;
    struct NvKmsRect rectOut = { };

    /* Apply the matrix transform to each point. */

    for (i = 0; i < 4; i++) {

        float32_t x = ui32_to_f32(p[i].x);
        float32_t y = ui32_to_f32(p[i].y);
        float32_t unused;

        TransformVertex(transform, &x, &y, &unused);

        p[i].x = F32toNvU16(x);
        p[i].y = F32toNvU16(y);
    }

    /* Compute the screen-aligned bounding box of the transformed points. */

    minx = p[0].x;
    maxx = p[0].x;
    miny = p[0].y;
    maxy = p[0].y;

    for (i = 1; i < 4; i++) {
        minx = NV_MIN(minx, p[i].x);
        maxx = NV_MAX(maxx, p[i].x);
        miny = NV_MIN(miny, p[i].y);
        maxy = NV_MAX(maxy, p[i].y);
    }

    rectOut.x = minx;
    rectOut.y = miny;
    rectOut.width = maxx - minx;
    rectOut.height = maxy - miny;

    return rectOut;
}

/*!
 * Apply the rotation described by 'rotation' and 'viewPortOut' to the
 * transformation matrix.
 *
 * \param[in]  viewPortOut  The viewPortOut region.
 * \param[in]  transform    The current transformation matrix.
 * \param[in]  rotation     The requested screen-aligned rotation.
 *
 * \return  The resulting transformation matrix.
 */
static struct NvKmsMatrixF32 ApplyRotationToMatrix(
    const struct NvKmsRect *viewPortOut,
    const struct NvKmsMatrixF32 *transform,
    enum NvKmsRotation rotation)
{
    const float32_t zeroF32 = NvU32viewAsF32(NV_FLOAT_ZERO);
    const float32_t oneF32 = NvU32viewAsF32(NV_FLOAT_ONE);
    const float32_t negOneF32 = NvU32viewAsF32(NV_FLOAT_NEG_ONE);

    float32_t f_rot_cos, f_rot_sin, f_rot_dx, f_rot_dy;
    float32_t width, height;

    struct NvKmsMatrixF32 m = *transform;
    struct NvKmsMatrixF32 tmpMatrix;

    struct NvKmsRect transformedViewPortOut;

    if (rotation == NVKMS_ROTATION_0) {
        return m;
    }

    transformedViewPortOut = TransformRect(viewPortOut, transform);

    width = ui32_to_f32(transformedViewPortOut.width);
    height = ui32_to_f32(transformedViewPortOut.height);

    switch (rotation) {
        default:
        case NVKMS_ROTATION_90:
            f_rot_cos = zeroF32;    f_rot_sin = oneF32;
            f_rot_dx  = height;     f_rot_dy  = zeroF32;
            break;
        case NVKMS_ROTATION_180:
            f_rot_cos = negOneF32;  f_rot_sin = zeroF32;
            f_rot_dx  = width;      f_rot_dy  = height;
            break;
        case NVKMS_ROTATION_270:
            f_rot_cos = zeroF32;    f_rot_sin = negOneF32;
            f_rot_dx  = zeroF32;    f_rot_dy  = width;
            break;
    }

    tmpMatrix = GenRotationMatrix(f_rot_cos, f_rot_sin);
    m = MultiplyMatrix(&tmpMatrix, &m);

    tmpMatrix = GenTranslationMatrix(f_rot_dx, f_rot_dy);
    m = MultiplyMatrix(&tmpMatrix, &m);

    return m;
}

/*!
 * Apply the reflection described by 'reflection[XY]', 'rotation' and
 * 'viewPortOut' to the transformation matrix.
 *
 * \param[in]  viewPortOut  The viewPortOut region.
 * \param[in]  transform    The current transformation matrix.
 * \param[in]  reflectionX  Whether to reflect along the X axis.
 * \param[in]  reflectionY  Whether to reflect along the Y axis.
 *
 * \return  The resulting transformation matrix.
 */
static struct NvKmsMatrixF32 ApplyReflectionToMatrix(
    const struct NvKmsRect *viewPortOut,
    const struct NvKmsMatrixF32 *transform,
    NvBool reflectionX,
    NvBool reflectionY)
{
    const float32_t zeroF32 = NvU32viewAsF32(NV_FLOAT_ZERO);
    const float32_t oneF32 = NvU32viewAsF32(NV_FLOAT_ONE);
    const float32_t negOneF32 = NvU32viewAsF32(NV_FLOAT_NEG_ONE);

    float32_t f_scale_x, f_scale_y, f_scale_dx, f_scale_dy;
    float32_t width, height;

    struct NvKmsMatrixF32 m = *transform;
    struct NvKmsMatrixF32 tmpMatrix;

    struct NvKmsRect transformedViewPortOut;

    if (!reflectionX && !reflectionY) {
        return m;
    }

    transformedViewPortOut = TransformRect(viewPortOut, transform);

    width = ui32_to_f32(transformedViewPortOut.width);
    height = ui32_to_f32(transformedViewPortOut.height);

    f_scale_x  = oneF32;
    f_scale_dx = zeroF32;
    f_scale_y  = oneF32;
    f_scale_dy = zeroF32;

    if (reflectionX) {
        f_scale_x = negOneF32;
        f_scale_dx = width;
    }

    if (reflectionY) {
        f_scale_y = negOneF32;
        f_scale_dy = height;
    }

    tmpMatrix = GenScaleMatrix(f_scale_x, f_scale_y);
    m = MultiplyMatrix(&tmpMatrix, &m);

    tmpMatrix = GenTranslationMatrix(f_scale_dx, f_scale_dy);
    m = MultiplyMatrix(&tmpMatrix, &m);

    return m;
}

/*!
 * Apply the scaling described by 'viewPortIn' and 'viewPortOut' to the
 * transformation matrix.
 *
 * \param[in]  viewPortIn   The viewPortIn region.
 * \param[in]  viewPortOut  The viewPortOut region.
 * \param[in]  transform    The current transformation matrix.
 *
 * \return  The resulting transformation matrix.
 */
static struct NvKmsMatrixF32 ScaleMatrixForViewPorts(
    const struct NvKmsRect *viewPortIn,
    const struct NvKmsRect *viewPortOut,
    const struct NvKmsMatrixF32 *transform)
{
    const struct NvKmsRect transformedViewPortOut =
        TransformRect(viewPortOut, transform);

    const float32_t inWidth = ui32_to_f32(viewPortIn->width);
    const float32_t inHeight = ui32_to_f32(viewPortIn->height);
    const float32_t outWidth = ui32_to_f32(transformedViewPortOut.width);
    const float32_t outHeight = ui32_to_f32(transformedViewPortOut.height);

    struct NvKmsMatrixF32 tmpMatrix;

    const float32_t sx = f32_div(inWidth, outWidth);
    const float32_t sy = f32_div(inHeight, outHeight);

    tmpMatrix = GenScaleMatrix(sx, sy);

    return MultiplyMatrix(&tmpMatrix, transform);
}

/*!
 * Translate the matrix for the ViewPortOut position.
 *
 * When headSurface state == FULL, the headSurface surface is the size of the
 * visible region of the mode and headSurface rendering simulates the
 * client-requested viewPortOut.  Translate the headSurface transformation
 * matrix for the ViewPortOut position.
 *
 * \param[in]  state        The headSurface state; we only translate for FULL.
 * \param[in]  viewPortOut  The viewPortOut region.
 * \param[in]  transform    The current transformation matrix.
 */
static struct NvKmsMatrixF32 TranslateMatrixForViewPortOut(
    const NVHsConfigState state,
    const struct NvKmsRect *viewPortOut,
    const struct NvKmsMatrixF32 *transform)
{
    if (state != NVKMS_HEAD_SURFACE_CONFIG_STATE_FULL_HEAD_SURFACE) {
        return *transform;
    }

    const struct NvKmsRect transformedViewPortOut =
        TransformRect(viewPortOut, transform);

    const float32_t x = ui32_to_f32(transformedViewPortOut.x);
    const float32_t y = ui32_to_f32(transformedViewPortOut.y);
    const float32_t negX = f32_mul(x, NvU32viewAsF32(NV_FLOAT_NEG_ONE));
    const float32_t negY = f32_mul(y, NvU32viewAsF32(NV_FLOAT_NEG_ONE));

    const struct NvKmsMatrixF32 translationMatrix =
        GenTranslationMatrix(negX, negY);

    return MultiplyMatrix(&translationMatrix, transform);
}

/*!
 * For pixelShift modes, bloat by x2 (pixelShift takes a 2x2 quad of input
 * pixels for each output pixel).
 *
 * \param[in]  transform   The current transformation matrix.
 * \param[in]  pixelShift  The pixelShift mode requested.
 */
static struct NvKmsMatrixF32 TransformMatrixForPixelShift(
    const struct NvKmsMatrixF32 *transform,
    enum NvKmsPixelShiftMode pixelShift)
{
    const float32_t twoF32 = NvU32viewAsF32(NV_FLOAT_TWO);
    const struct NvKmsMatrixF32 pixelShiftBloatTransform =
        GenScaleMatrix(twoF32, twoF32);
    struct NvKmsMatrixF32 m = *transform;

    if (pixelShift == NVKMS_PIXEL_SHIFT_NONE) {
        return m;
    }

    return MultiplyMatrix(&pixelShiftBloatTransform, &m);
}

/*!
 * Multiply the client-specified transform with the current transform.
 *
 * \param[in]  transform  The current transformation matrix.
 * \param[in]  p          The client request parameter structure.
 */
static struct NvKmsMatrixF32 ApplyClientTransformToMatrix(
    const struct NvKmsMatrixF32 *transform,
    const struct NvKmsSetModeHeadSurfaceParams *p)
{
    const struct NvKmsMatrixF32 clientTransform =
        NvKmsMatrixToNvKmsMatrixF32(p->transform);

    nvAssert(p->transformSpecified);

    return MultiplyMatrix(&clientTransform, transform);
}

static NvBool InvertMatrix(
    struct NvKmsMatrixF32 *dst,
    const struct NvKmsMatrixF32 *src)
{
    float64_t det;
    int i, j;
    static int a[3] = { 2, 2, 1 };
    static int b[3] = { 1, 0, 0 };
    const float64_t zeroF64 = i32_to_f64(0);
    const float64_t oneF64 = i32_to_f64(1);

    det = zeroF64;
    for (i = 0; i < 3; i++) {
        float64_t p;
        const int ai = a[i];
        const int bi = b[i];

        float32_t tmp = F32_AxB_minus_CxD(src->m[ai][2], src->m[bi][1],
                                          src->m[ai][1], src->m[bi][2]);
        tmp = f32_mul(src->m[i][0], tmp);

        p = f32_to_f64(tmp);

        if (i == 1) {
            p = F64_negate(p);
        }

        det = f64_add(det, p);
    }

    if (f64_eq(det, zeroF64)) {
        return FALSE;
    }

    det = f64_div(oneF64, det);

    for (j = 0; j < 3; j++) {
        for (i = 0; i < 3; i++) {
            float64_t p;
            const int ai = a[i];
            const int aj = a[j];
            const int bi = b[i];
            const int bj = b[j];

            const float32_t tmp =
                F32_AxB_minus_CxD(src->m[ai][aj], src->m[bi][bj],
                                  src->m[ai][bj], src->m[bi][aj]);
            p = f32_to_f64(tmp);

            if (((i + j) & 1) != 0) {
                p = F64_negate(p);
            }

            p = f64_mul(det, p);

            dst->m[j][i] = f64_to_f32(p);
        }
    }

    return TRUE;
}

/*!
 * Calculate a warp mesh for this head.
 *
 * This constructs a simple 4-vertex "mesh" that renders a single quad using a
 * triangle strip.  The vertices are transformed from the viewPortIn region
 * using the inversed of the HS transform to normalized headSurface space.
 */
static NvBool AssignStaticWarpMesh(
    NVHsChannelConfig *p)
{
    int i;

    NvHsStaticWarpMesh *swm = &p->staticWarpMesh;

    const float32_t viewPortInWidthF = ui32_to_f32(p->viewPortIn.width);
    const float32_t viewPortInHeightF = ui32_to_f32(p->viewPortIn.height);
    const float32_t viewPortOutWidthF = ui32_to_f32(p->viewPortOut.width);
    const float32_t viewPortOutHeightF = ui32_to_f32(p->viewPortOut.height);

    struct NvKmsMatrixF32 invertedTransform;

    if (!InvertMatrix(&invertedTransform, &p->transform)) {
        return FALSE;
    }

    swm->vertex[0].x = swm->vertex[0].u = NV_FLOAT_ZERO;
    swm->vertex[0].y = swm->vertex[0].v = NV_FLOAT_ZERO;

    swm->vertex[1].x = swm->vertex[1].u = NV_FLOAT_ZERO;
    swm->vertex[1].y = swm->vertex[1].v = NV_FLOAT_ONE;

    swm->vertex[2].x = swm->vertex[2].u = NV_FLOAT_ONE;
    swm->vertex[2].y = swm->vertex[2].v = NV_FLOAT_ZERO;

    swm->vertex[3].x = swm->vertex[3].u = NV_FLOAT_ONE;
    swm->vertex[3].y = swm->vertex[3].v = NV_FLOAT_ONE;

    for (i = 0; i < 4; i++) {

        float32_t x = NvU32viewAsF32(swm->vertex[i].x);
        float32_t y = NvU32viewAsF32(swm->vertex[i].y);
        float32_t q;

        // Scale to input region
        x = f32_mul(x, viewPortInWidthF);
        y = f32_mul(y, viewPortInHeightF);

        // Transform
        TransformVertex(&invertedTransform, &x, &y, &q);

        // Normalize to output region
        x = f32_div(x, viewPortOutWidthF);
        y = f32_div(y, viewPortOutHeightF);

        swm->vertex[i].x = F32viewAsNvU32(x);
        swm->vertex[i].y = F32viewAsNvU32(y);
        swm->vertex[i].q = F32viewAsNvU32(q);
    }

    return TRUE;
}

/*!
 * Assign NVHsChannelConfig::transform and NVHsChannelConfig::staticWarpMesh,
 * based on the current viewports described in NVHsChannelConfig, and various
 * client-requested state in NvKmsSetModeHeadSurfaceParams.
 *
 * \param[in,out]  pChannelConfig  The headSurface channel config.
 * \param[in]      p               The NVKMS client headSurface parameters.
 *
 * \return TRUE if the NVHsChannelConfig fields could be successfully assigned.
 * Otherwise, FALSE.
 */
NvBool nvHsAssignTransformMatrix(
    NVHsChannelConfig *pChannelConfig,
    const struct NvKmsSetModeHeadSurfaceParams *p)
{
    struct NvKmsMatrixF32 transform = GenIdentityMatrix();

    transform = TranslateMatrixForViewPortOut(
                                pChannelConfig->state,
                                &pChannelConfig->viewPortOut,
                                &transform);

    transform = TransformMatrixForPixelShift(
                                &transform,
                                p->pixelShift);

    transform = ApplyRotationToMatrix(
                                &pChannelConfig->viewPortOut,
                                &transform,
                                p->rotation);

    transform = ApplyReflectionToMatrix(
                                &pChannelConfig->viewPortOut,
                                &transform,
                                p->reflectionX,
                                p->reflectionY);
    /*
     * We treat client-specified transformation matrices and viewport scaling as
     * mutually exclusive: when a client-specified transformation matrix is
     * provided, the viewPortIn is already transformed by that matrix (the
     * client needed to do that, in order to know the size of the surface).
     * Calling ScaleMatrixForViewPorts() in the transformSpecified would
     * effectively scale the viewPortIn a second time, which would be incorrect.
     */
    if (p->transformSpecified) {
        transform = ApplyClientTransformToMatrix(
                                &transform, p);
    } else {
        transform = ScaleMatrixForViewPorts(
                                &pChannelConfig->viewPortIn,
                                &pChannelConfig->viewPortOut,
                                &transform);
    }

    pChannelConfig->transform = transform;

    return AssignStaticWarpMesh(pChannelConfig);
}

