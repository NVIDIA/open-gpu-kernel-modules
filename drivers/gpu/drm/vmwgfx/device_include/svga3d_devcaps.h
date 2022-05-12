/* SPDX-License-Identifier: GPL-2.0 OR MIT */
/**********************************************************
 * Copyright 1998-2019 VMware, Inc.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 **********************************************************/

/*
 * svga3d_devcaps.h --
 *
 *       SVGA 3d caps definitions
 */

#ifndef _SVGA3D_DEVCAPS_H_
#define _SVGA3D_DEVCAPS_H_

#define INCLUDE_ALLOW_MODULE
#define INCLUDE_ALLOW_USERLEVEL
#define INCLUDE_ALLOW_VMCORE

#include "includeCheck.h"

#include "svga3d_types.h"

/*
 * 3D Hardware Version
 *
 *   The hardware version is stored in the SVGA_FIFO_3D_HWVERSION fifo
 *   register.   Is set by the host and read by the guest.  This lets
 *   us make new guest drivers which are backwards-compatible with old
 *   SVGA hardware revisions.  It does not let us support old guest
 *   drivers.  Good enough for now.
 *
 */

#define SVGA3D_MAKE_HWVERSION(major, minor)      (((major) << 16) | ((minor) & 0xFF))
#define SVGA3D_MAJOR_HWVERSION(version)          ((version) >> 16)
#define SVGA3D_MINOR_HWVERSION(version)          ((version) & 0xFF)

typedef enum {
   SVGA3D_HWVERSION_WS5_RC1   = SVGA3D_MAKE_HWVERSION(0, 1),
   SVGA3D_HWVERSION_WS5_RC2   = SVGA3D_MAKE_HWVERSION(0, 2),
   SVGA3D_HWVERSION_WS51_RC1  = SVGA3D_MAKE_HWVERSION(0, 3),
   SVGA3D_HWVERSION_WS6_B1    = SVGA3D_MAKE_HWVERSION(1, 1),
   SVGA3D_HWVERSION_FUSION_11 = SVGA3D_MAKE_HWVERSION(1, 4),
   SVGA3D_HWVERSION_WS65_B1   = SVGA3D_MAKE_HWVERSION(2, 0),
   SVGA3D_HWVERSION_WS8_B1    = SVGA3D_MAKE_HWVERSION(2, 1),
   SVGA3D_HWVERSION_CURRENT   = SVGA3D_HWVERSION_WS8_B1,
} SVGA3dHardwareVersion;

/*
 * DevCap indexes.
 */

typedef uint32 SVGA3dDevCapIndex;

#define SVGA3D_DEVCAP_INVALID                              ((uint32)-1)
#define SVGA3D_DEVCAP_3D                                   0
#define SVGA3D_DEVCAP_MAX_LIGHTS                           1

/*
 * SVGA3D_DEVCAP_MAX_TEXTURES reflects the maximum number of
 * fixed-function texture units available. Each of these units
 * work in both FFP and Shader modes, and they support texture
 * transforms and texture coordinates. The host may have additional
 * texture image units that are only usable with shaders.
 */
#define SVGA3D_DEVCAP_MAX_TEXTURES                         2
#define SVGA3D_DEVCAP_MAX_CLIP_PLANES                      3
#define SVGA3D_DEVCAP_VERTEX_SHADER_VERSION                4
#define SVGA3D_DEVCAP_VERTEX_SHADER                        5
#define SVGA3D_DEVCAP_FRAGMENT_SHADER_VERSION              6
#define SVGA3D_DEVCAP_FRAGMENT_SHADER                      7
#define SVGA3D_DEVCAP_MAX_RENDER_TARGETS                   8
#define SVGA3D_DEVCAP_S23E8_TEXTURES                       9
#define SVGA3D_DEVCAP_S10E5_TEXTURES                       10
#define SVGA3D_DEVCAP_MAX_FIXED_VERTEXBLEND                11
#define SVGA3D_DEVCAP_D16_BUFFER_FORMAT                    12
#define SVGA3D_DEVCAP_D24S8_BUFFER_FORMAT                  13
#define SVGA3D_DEVCAP_D24X8_BUFFER_FORMAT                  14
#define SVGA3D_DEVCAP_QUERY_TYPES                          15
#define SVGA3D_DEVCAP_TEXTURE_GRADIENT_SAMPLING            16
#define SVGA3D_DEVCAP_MAX_POINT_SIZE                       17
#define SVGA3D_DEVCAP_MAX_SHADER_TEXTURES                  18
#define SVGA3D_DEVCAP_MAX_TEXTURE_WIDTH                    19
#define SVGA3D_DEVCAP_MAX_TEXTURE_HEIGHT                   20
#define SVGA3D_DEVCAP_MAX_VOLUME_EXTENT                    21
#define SVGA3D_DEVCAP_MAX_TEXTURE_REPEAT                   22
#define SVGA3D_DEVCAP_MAX_TEXTURE_ASPECT_RATIO             23
#define SVGA3D_DEVCAP_MAX_TEXTURE_ANISOTROPY               24
#define SVGA3D_DEVCAP_MAX_PRIMITIVE_COUNT                  25
#define SVGA3D_DEVCAP_MAX_VERTEX_INDEX                     26
#define SVGA3D_DEVCAP_MAX_VERTEX_SHADER_INSTRUCTIONS       27
#define SVGA3D_DEVCAP_MAX_FRAGMENT_SHADER_INSTRUCTIONS     28
#define SVGA3D_DEVCAP_MAX_VERTEX_SHADER_TEMPS              29
#define SVGA3D_DEVCAP_MAX_FRAGMENT_SHADER_TEMPS            30
#define SVGA3D_DEVCAP_TEXTURE_OPS                          31
#define SVGA3D_DEVCAP_SURFACEFMT_X8R8G8B8                  32
#define SVGA3D_DEVCAP_SURFACEFMT_A8R8G8B8                  33
#define SVGA3D_DEVCAP_SURFACEFMT_A2R10G10B10               34
#define SVGA3D_DEVCAP_SURFACEFMT_X1R5G5B5                  35
#define SVGA3D_DEVCAP_SURFACEFMT_A1R5G5B5                  36
#define SVGA3D_DEVCAP_SURFACEFMT_A4R4G4B4                  37
#define SVGA3D_DEVCAP_SURFACEFMT_R5G6B5                    38
#define SVGA3D_DEVCAP_SURFACEFMT_LUMINANCE16               39
#define SVGA3D_DEVCAP_SURFACEFMT_LUMINANCE8_ALPHA8         40
#define SVGA3D_DEVCAP_SURFACEFMT_ALPHA8                    41
#define SVGA3D_DEVCAP_SURFACEFMT_LUMINANCE8                42
#define SVGA3D_DEVCAP_SURFACEFMT_Z_D16                     43
#define SVGA3D_DEVCAP_SURFACEFMT_Z_D24S8                   44
#define SVGA3D_DEVCAP_SURFACEFMT_Z_D24X8                   45
#define SVGA3D_DEVCAP_SURFACEFMT_DXT1                      46
#define SVGA3D_DEVCAP_SURFACEFMT_DXT2                      47
#define SVGA3D_DEVCAP_SURFACEFMT_DXT3                      48
#define SVGA3D_DEVCAP_SURFACEFMT_DXT4                      49
#define SVGA3D_DEVCAP_SURFACEFMT_DXT5                      50
#define SVGA3D_DEVCAP_SURFACEFMT_BUMPX8L8V8U8              51
#define SVGA3D_DEVCAP_SURFACEFMT_A2W10V10U10               52
#define SVGA3D_DEVCAP_SURFACEFMT_BUMPU8V8                  53
#define SVGA3D_DEVCAP_SURFACEFMT_Q8W8V8U8                  54
#define SVGA3D_DEVCAP_SURFACEFMT_CxV8U8                    55
#define SVGA3D_DEVCAP_SURFACEFMT_R_S10E5                   56
#define SVGA3D_DEVCAP_SURFACEFMT_R_S23E8                   57
#define SVGA3D_DEVCAP_SURFACEFMT_RG_S10E5                  58
#define SVGA3D_DEVCAP_SURFACEFMT_RG_S23E8                  59
#define SVGA3D_DEVCAP_SURFACEFMT_ARGB_S10E5                60
#define SVGA3D_DEVCAP_SURFACEFMT_ARGB_S23E8                61

/*
 * There is a hole in our devcap definitions for
 * historical reasons.
 *
 * Define a constant just for completeness.
 */
#define SVGA3D_DEVCAP_MISSING62                            62

#define SVGA3D_DEVCAP_MAX_VERTEX_SHADER_TEXTURES           63

/*
 * Note that MAX_SIMULTANEOUS_RENDER_TARGETS is a maximum count of color
 * render targets.  This does not include the depth or stencil targets.
 */
#define SVGA3D_DEVCAP_MAX_SIMULTANEOUS_RENDER_TARGETS      64

#define SVGA3D_DEVCAP_SURFACEFMT_V16U16                    65
#define SVGA3D_DEVCAP_SURFACEFMT_G16R16                    66
#define SVGA3D_DEVCAP_SURFACEFMT_A16B16G16R16              67
#define SVGA3D_DEVCAP_SURFACEFMT_UYVY                      68
#define SVGA3D_DEVCAP_SURFACEFMT_YUY2                      69

/*
 * Deprecated.
 */
#define SVGA3D_DEVCAP_DEAD4                                70
#define SVGA3D_DEVCAP_DEAD5                                71
#define SVGA3D_DEVCAP_DEAD7                                72
#define SVGA3D_DEVCAP_DEAD6                                73

#define SVGA3D_DEVCAP_AUTOGENMIPMAPS                       74
#define SVGA3D_DEVCAP_SURFACEFMT_NV12                      75
#define SVGA3D_DEVCAP_DEAD10                               76

/*
 * This is the maximum number of SVGA context IDs that the guest
 * can define using SVGA_3D_CMD_CONTEXT_DEFINE.
 */
#define SVGA3D_DEVCAP_MAX_CONTEXT_IDS                      77

/*
 * This is the maximum number of SVGA surface IDs that the guest
 * can define using SVGA_3D_CMD_SURFACE_DEFINE*.
 */
#define SVGA3D_DEVCAP_MAX_SURFACE_IDS                      78

#define SVGA3D_DEVCAP_SURFACEFMT_Z_DF16                    79
#define SVGA3D_DEVCAP_SURFACEFMT_Z_DF24                    80
#define SVGA3D_DEVCAP_SURFACEFMT_Z_D24S8_INT               81

#define SVGA3D_DEVCAP_SURFACEFMT_ATI1                      82
#define SVGA3D_DEVCAP_SURFACEFMT_ATI2                      83

/*
 * Deprecated.
 */
#define SVGA3D_DEVCAP_DEAD1                                84
#define SVGA3D_DEVCAP_DEAD8                                85
#define SVGA3D_DEVCAP_DEAD9                                86

#define SVGA3D_DEVCAP_LINE_AA                              87  /* boolean */
#define SVGA3D_DEVCAP_LINE_STIPPLE                         88  /* boolean */
#define SVGA3D_DEVCAP_MAX_LINE_WIDTH                       89  /* float */
#define SVGA3D_DEVCAP_MAX_AA_LINE_WIDTH                    90  /* float */

#define SVGA3D_DEVCAP_SURFACEFMT_YV12                      91

/*
 * Deprecated.
 */
#define SVGA3D_DEVCAP_DEAD3                                92

/*
 * Are TS_CONSTANT, TS_COLOR_KEY, and TS_COLOR_KEY_ENABLE supported?
 */
#define SVGA3D_DEVCAP_TS_COLOR_KEY                         93 /* boolean */

/*
 * Deprecated.
 */
#define SVGA3D_DEVCAP_DEAD2                                94

/*
 * Does the device support DXContexts?
 */
#define SVGA3D_DEVCAP_DXCONTEXT                            95

/*
 * Deprecated.
 */
#define SVGA3D_DEVCAP_DEAD11                               96

/*
 * What is the maximum number of vertex buffers or vertex input registers
 * that can be expected to work correctly with a DXContext?
 *
 * The guest is allowed to set up to SVGA3D_DX_MAX_VERTEXBUFFERS, but
 * anything in excess of this cap is not guaranteed to render correctly.
 *
 * Similarly, the guest can set up to SVGA3D_DX_MAX_VERTEXINPUTREGISTERS
 * input registers without the SVGA3D_DEVCAP_SM4_1 cap, or
 * SVGA3D_DX_SM41_MAX_VERTEXINPUTREGISTERS with the SVGA3D_DEVCAP_SM4_1,
 * but only the registers up to this cap value are guaranteed to render
 * correctly.
 *
 * If guest-drivers are able to expose a lower-limit, it's recommended
 * that they clamp to this value.  Otherwise, the host will make a
 * best-effort on case-by-case basis if guests exceed this.
 */
#define SVGA3D_DEVCAP_DX_MAX_VERTEXBUFFERS                 97

/*
 * What is the maximum number of constant buffers that can be expected to
 * work correctly with a DX context?
 *
 * The guest is allowed to set up to SVGA3D_DX_MAX_CONSTBUFFERS, but
 * anything in excess of this cap is not guaranteed to render correctly.
 *
 * If guest-drivers are able to expose a lower-limit, it's recommended
 * that they clamp to this value.  Otherwise, the host will make a
 * best-effort on case-by-case basis if guests exceed this.
 */
#define SVGA3D_DEVCAP_DX_MAX_CONSTANT_BUFFERS              98

/*
 * Does the device support provoking vertex control?
 *
 * If this cap is present, the provokingVertexLast field in the
 * rasterizer state is enabled.  (Guests can then set it to FALSE,
 * meaning that the first vertex is the provoking vertex, or TRUE,
 * meaning that the last verteix is the provoking vertex.)
 *
 * If this cap is FALSE, then guests should set the provokingVertexLast
 * to FALSE, otherwise rendering behavior is undefined.
 */
#define SVGA3D_DEVCAP_DX_PROVOKING_VERTEX                  99

#define SVGA3D_DEVCAP_DXFMT_X8R8G8B8                       100
#define SVGA3D_DEVCAP_DXFMT_A8R8G8B8                       101
#define SVGA3D_DEVCAP_DXFMT_R5G6B5                         102
#define SVGA3D_DEVCAP_DXFMT_X1R5G5B5                       103
#define SVGA3D_DEVCAP_DXFMT_A1R5G5B5                       104
#define SVGA3D_DEVCAP_DXFMT_A4R4G4B4                       105
#define SVGA3D_DEVCAP_DXFMT_Z_D32                          106
#define SVGA3D_DEVCAP_DXFMT_Z_D16                          107
#define SVGA3D_DEVCAP_DXFMT_Z_D24S8                        108
#define SVGA3D_DEVCAP_DXFMT_Z_D15S1                        109
#define SVGA3D_DEVCAP_DXFMT_LUMINANCE8                     110
#define SVGA3D_DEVCAP_DXFMT_LUMINANCE4_ALPHA4              111
#define SVGA3D_DEVCAP_DXFMT_LUMINANCE16                    112
#define SVGA3D_DEVCAP_DXFMT_LUMINANCE8_ALPHA8              113
#define SVGA3D_DEVCAP_DXFMT_DXT1                           114
#define SVGA3D_DEVCAP_DXFMT_DXT2                           115
#define SVGA3D_DEVCAP_DXFMT_DXT3                           116
#define SVGA3D_DEVCAP_DXFMT_DXT4                           117
#define SVGA3D_DEVCAP_DXFMT_DXT5                           118
#define SVGA3D_DEVCAP_DXFMT_BUMPU8V8                       119
#define SVGA3D_DEVCAP_DXFMT_BUMPL6V5U5                     120
#define SVGA3D_DEVCAP_DXFMT_BUMPX8L8V8U8                   121
#define SVGA3D_DEVCAP_DXFMT_FORMAT_DEAD1                   122
#define SVGA3D_DEVCAP_DXFMT_ARGB_S10E5                     123
#define SVGA3D_DEVCAP_DXFMT_ARGB_S23E8                     124
#define SVGA3D_DEVCAP_DXFMT_A2R10G10B10                    125
#define SVGA3D_DEVCAP_DXFMT_V8U8                           126
#define SVGA3D_DEVCAP_DXFMT_Q8W8V8U8                       127
#define SVGA3D_DEVCAP_DXFMT_CxV8U8                         128
#define SVGA3D_DEVCAP_DXFMT_X8L8V8U8                       129
#define SVGA3D_DEVCAP_DXFMT_A2W10V10U10                    130
#define SVGA3D_DEVCAP_DXFMT_ALPHA8                         131
#define SVGA3D_DEVCAP_DXFMT_R_S10E5                        132
#define SVGA3D_DEVCAP_DXFMT_R_S23E8                        133
#define SVGA3D_DEVCAP_DXFMT_RG_S10E5                       134
#define SVGA3D_DEVCAP_DXFMT_RG_S23E8                       135
#define SVGA3D_DEVCAP_DXFMT_BUFFER                         136
#define SVGA3D_DEVCAP_DXFMT_Z_D24X8                        137
#define SVGA3D_DEVCAP_DXFMT_V16U16                         138
#define SVGA3D_DEVCAP_DXFMT_G16R16                         139
#define SVGA3D_DEVCAP_DXFMT_A16B16G16R16                   140
#define SVGA3D_DEVCAP_DXFMT_UYVY                           141
#define SVGA3D_DEVCAP_DXFMT_YUY2                           142
#define SVGA3D_DEVCAP_DXFMT_NV12                           143
#define SVGA3D_DEVCAP_DXFMT_FORMAT_DEAD2                   144
#define SVGA3D_DEVCAP_DXFMT_R32G32B32A32_TYPELESS          145
#define SVGA3D_DEVCAP_DXFMT_R32G32B32A32_UINT              146
#define SVGA3D_DEVCAP_DXFMT_R32G32B32A32_SINT              147
#define SVGA3D_DEVCAP_DXFMT_R32G32B32_TYPELESS             148
#define SVGA3D_DEVCAP_DXFMT_R32G32B32_FLOAT                149
#define SVGA3D_DEVCAP_DXFMT_R32G32B32_UINT                 150
#define SVGA3D_DEVCAP_DXFMT_R32G32B32_SINT                 151
#define SVGA3D_DEVCAP_DXFMT_R16G16B16A16_TYPELESS          152
#define SVGA3D_DEVCAP_DXFMT_R16G16B16A16_UINT              153
#define SVGA3D_DEVCAP_DXFMT_R16G16B16A16_SNORM             154
#define SVGA3D_DEVCAP_DXFMT_R16G16B16A16_SINT              155
#define SVGA3D_DEVCAP_DXFMT_R32G32_TYPELESS                156
#define SVGA3D_DEVCAP_DXFMT_R32G32_UINT                    157
#define SVGA3D_DEVCAP_DXFMT_R32G32_SINT                    158
#define SVGA3D_DEVCAP_DXFMT_R32G8X24_TYPELESS              159
#define SVGA3D_DEVCAP_DXFMT_D32_FLOAT_S8X24_UINT           160
#define SVGA3D_DEVCAP_DXFMT_R32_FLOAT_X8X24                161
#define SVGA3D_DEVCAP_DXFMT_X32_G8X24_UINT                 162
#define SVGA3D_DEVCAP_DXFMT_R10G10B10A2_TYPELESS           163
#define SVGA3D_DEVCAP_DXFMT_R10G10B10A2_UINT               164
#define SVGA3D_DEVCAP_DXFMT_R11G11B10_FLOAT                165
#define SVGA3D_DEVCAP_DXFMT_R8G8B8A8_TYPELESS              166
#define SVGA3D_DEVCAP_DXFMT_R8G8B8A8_UNORM                 167
#define SVGA3D_DEVCAP_DXFMT_R8G8B8A8_UNORM_SRGB            168
#define SVGA3D_DEVCAP_DXFMT_R8G8B8A8_UINT                  169
#define SVGA3D_DEVCAP_DXFMT_R8G8B8A8_SINT                  170
#define SVGA3D_DEVCAP_DXFMT_R16G16_TYPELESS                171
#define SVGA3D_DEVCAP_DXFMT_R16G16_UINT                    172
#define SVGA3D_DEVCAP_DXFMT_R16G16_SINT                    173
#define SVGA3D_DEVCAP_DXFMT_R32_TYPELESS                   174
#define SVGA3D_DEVCAP_DXFMT_D32_FLOAT                      175
#define SVGA3D_DEVCAP_DXFMT_R32_UINT                       176
#define SVGA3D_DEVCAP_DXFMT_R32_SINT                       177
#define SVGA3D_DEVCAP_DXFMT_R24G8_TYPELESS                 178
#define SVGA3D_DEVCAP_DXFMT_D24_UNORM_S8_UINT              179
#define SVGA3D_DEVCAP_DXFMT_R24_UNORM_X8                   180
#define SVGA3D_DEVCAP_DXFMT_X24_G8_UINT                    181
#define SVGA3D_DEVCAP_DXFMT_R8G8_TYPELESS                  182
#define SVGA3D_DEVCAP_DXFMT_R8G8_UNORM                     183
#define SVGA3D_DEVCAP_DXFMT_R8G8_UINT                      184
#define SVGA3D_DEVCAP_DXFMT_R8G8_SINT                      185
#define SVGA3D_DEVCAP_DXFMT_R16_TYPELESS                   186
#define SVGA3D_DEVCAP_DXFMT_R16_UNORM                      187
#define SVGA3D_DEVCAP_DXFMT_R16_UINT                       188
#define SVGA3D_DEVCAP_DXFMT_R16_SNORM                      189
#define SVGA3D_DEVCAP_DXFMT_R16_SINT                       190
#define SVGA3D_DEVCAP_DXFMT_R8_TYPELESS                    191
#define SVGA3D_DEVCAP_DXFMT_R8_UNORM                       192
#define SVGA3D_DEVCAP_DXFMT_R8_UINT                        193
#define SVGA3D_DEVCAP_DXFMT_R8_SNORM                       194
#define SVGA3D_DEVCAP_DXFMT_R8_SINT                        195
#define SVGA3D_DEVCAP_DXFMT_P8                             196
#define SVGA3D_DEVCAP_DXFMT_R9G9B9E5_SHAREDEXP             197
#define SVGA3D_DEVCAP_DXFMT_R8G8_B8G8_UNORM                198
#define SVGA3D_DEVCAP_DXFMT_G8R8_G8B8_UNORM                199
#define SVGA3D_DEVCAP_DXFMT_BC1_TYPELESS                   200
#define SVGA3D_DEVCAP_DXFMT_BC1_UNORM_SRGB                 201
#define SVGA3D_DEVCAP_DXFMT_BC2_TYPELESS                   202
#define SVGA3D_DEVCAP_DXFMT_BC2_UNORM_SRGB                 203
#define SVGA3D_DEVCAP_DXFMT_BC3_TYPELESS                   204
#define SVGA3D_DEVCAP_DXFMT_BC3_UNORM_SRGB                 205
#define SVGA3D_DEVCAP_DXFMT_BC4_TYPELESS                   206
#define SVGA3D_DEVCAP_DXFMT_ATI1                           207
#define SVGA3D_DEVCAP_DXFMT_BC4_SNORM                      208
#define SVGA3D_DEVCAP_DXFMT_BC5_TYPELESS                   209
#define SVGA3D_DEVCAP_DXFMT_ATI2                           210
#define SVGA3D_DEVCAP_DXFMT_BC5_SNORM                      211
#define SVGA3D_DEVCAP_DXFMT_R10G10B10_XR_BIAS_A2_UNORM     212
#define SVGA3D_DEVCAP_DXFMT_B8G8R8A8_TYPELESS              213
#define SVGA3D_DEVCAP_DXFMT_B8G8R8A8_UNORM_SRGB            214
#define SVGA3D_DEVCAP_DXFMT_B8G8R8X8_TYPELESS              215
#define SVGA3D_DEVCAP_DXFMT_B8G8R8X8_UNORM_SRGB            216
#define SVGA3D_DEVCAP_DXFMT_Z_DF16                         217
#define SVGA3D_DEVCAP_DXFMT_Z_DF24                         218
#define SVGA3D_DEVCAP_DXFMT_Z_D24S8_INT                    219
#define SVGA3D_DEVCAP_DXFMT_YV12                           220
#define SVGA3D_DEVCAP_DXFMT_R32G32B32A32_FLOAT             221
#define SVGA3D_DEVCAP_DXFMT_R16G16B16A16_FLOAT             222
#define SVGA3D_DEVCAP_DXFMT_R16G16B16A16_UNORM             223
#define SVGA3D_DEVCAP_DXFMT_R32G32_FLOAT                   224
#define SVGA3D_DEVCAP_DXFMT_R10G10B10A2_UNORM              225
#define SVGA3D_DEVCAP_DXFMT_R8G8B8A8_SNORM                 226
#define SVGA3D_DEVCAP_DXFMT_R16G16_FLOAT                   227
#define SVGA3D_DEVCAP_DXFMT_R16G16_UNORM                   228
#define SVGA3D_DEVCAP_DXFMT_R16G16_SNORM                   229
#define SVGA3D_DEVCAP_DXFMT_R32_FLOAT                      230
#define SVGA3D_DEVCAP_DXFMT_R8G8_SNORM                     231
#define SVGA3D_DEVCAP_DXFMT_R16_FLOAT                      232
#define SVGA3D_DEVCAP_DXFMT_D16_UNORM                      233
#define SVGA3D_DEVCAP_DXFMT_A8_UNORM                       234
#define SVGA3D_DEVCAP_DXFMT_BC1_UNORM                      235
#define SVGA3D_DEVCAP_DXFMT_BC2_UNORM                      236
#define SVGA3D_DEVCAP_DXFMT_BC3_UNORM                      237
#define SVGA3D_DEVCAP_DXFMT_B5G6R5_UNORM                   238
#define SVGA3D_DEVCAP_DXFMT_B5G5R5A1_UNORM                 239
#define SVGA3D_DEVCAP_DXFMT_B8G8R8A8_UNORM                 240
#define SVGA3D_DEVCAP_DXFMT_B8G8R8X8_UNORM                 241
#define SVGA3D_DEVCAP_DXFMT_BC4_UNORM                      242
#define SVGA3D_DEVCAP_DXFMT_BC5_UNORM                      243

/*
 * Advertises shaderModel 4.1 support, independent blend-states,
 * cube-map arrays, and a higher vertex input registers limit.
 *
 * (See documentation on SVGA3D_DEVCAP_DX_MAX_VERTEXBUFFERS.)
 */
#define SVGA3D_DEVCAP_SM41                                 244
#define SVGA3D_DEVCAP_MULTISAMPLE_2X                       245
#define SVGA3D_DEVCAP_MULTISAMPLE_4X                       246

/*
 * Indicates that the device has rendering support for
 * the full multisample quality.  If this cap is not present,
 * the host may or may not support full quality rendering.
 *
 * See also SVGA_REG_MS_HINT_RESOLVED.
 */
#define SVGA3D_DEVCAP_MS_FULL_QUALITY                      247

/*
 * Advertises support for the SVGA3D LogicOps commands.
 */
#define SVGA3D_DEVCAP_LOGICOPS                             248

/*
 * Advertises support for using logicOps in the DXBlendStates.
 */
#define SVGA3D_DEVCAP_LOGIC_BLENDOPS                       249

/*
* Note DXFMT range is now non-contiguous.
*/
#define SVGA3D_DEVCAP_RESERVED_1                           250
#define SVGA3D_DEVCAP_DXFMT_BC6H_TYPELESS                  251
#define SVGA3D_DEVCAP_DXFMT_BC6H_UF16                      252
#define SVGA3D_DEVCAP_DXFMT_BC6H_SF16                      253
#define SVGA3D_DEVCAP_DXFMT_BC7_TYPELESS                   254
#define SVGA3D_DEVCAP_DXFMT_BC7_UNORM                      255
#define SVGA3D_DEVCAP_DXFMT_BC7_UNORM_SRGB                 256
#define SVGA3D_DEVCAP_RESERVED_2                           257

#define SVGA3D_DEVCAP_SM5                                  258
#define SVGA3D_DEVCAP_MULTISAMPLE_8X                       259

/* This must be the last index. */
#define SVGA3D_DEVCAP_MAX                                  260

/*
 * Bit definitions for DXFMT devcaps
 *
 *
 * SUPPORTED: Can the format be defined?
 * SHADER_SAMPLE: Can the format be sampled from a shader?
 * COLOR_RENDERTARGET: Can the format be a color render target?
 * DEPTH_RENDERTARGET: Can the format be a depth render target?
 * BLENDABLE: Is the format blendable?
 * MIPS: Does the format support mip levels?
 * ARRAY: Does the format support texture arrays?
 * VOLUME: Does the format support having volume?
 * MULTISAMPLE: Does the format support multisample?
 */
#define SVGA3D_DXFMT_SUPPORTED                (1 <<  0)
#define SVGA3D_DXFMT_SHADER_SAMPLE            (1 <<  1)
#define SVGA3D_DXFMT_COLOR_RENDERTARGET       (1 <<  2)
#define SVGA3D_DXFMT_DEPTH_RENDERTARGET       (1 <<  3)
#define SVGA3D_DXFMT_BLENDABLE                (1 <<  4)
#define SVGA3D_DXFMT_MIPS                     (1 <<  5)
#define SVGA3D_DXFMT_ARRAY                    (1 <<  6)
#define SVGA3D_DXFMT_VOLUME                   (1 <<  7)
#define SVGA3D_DXFMT_DX_VERTEX_BUFFER         (1 <<  8)
#define SVGA3D_DXFMT_MULTISAMPLE              (1 <<  9)
#define SVGA3D_DXFMT_MAX                      (1 << 10)

typedef union {
   SVGA3dBool b;
   uint32 u;
   int32 i;
   float f;
} SVGA3dDevCapResult;

#endif /* _SVGA3D_DEVCAPS_H_ */
