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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __NVIDIA_3D_TYPES_H__
#define __NVIDIA_3D_TYPES_H__


#include "nvtypes.h"
#include "nvlimits.h"
#include "nvidia-push-methods.h"

#include "nvidia-3d-shaders.h"

enum Nv3dBlendOperation {
    NV3D_BLEND_OP_CLEAR,
    NV3D_BLEND_OP_SRC,
    NV3D_BLEND_OP_DST,
    NV3D_BLEND_OP_OVER,
    NV3D_BLEND_OP_OVER_REVERSE,
    NV3D_BLEND_OP_IN,
    NV3D_BLEND_OP_IN_REVERSE,
    NV3D_BLEND_OP_OUT,
    NV3D_BLEND_OP_OUT_REVERSE,
    NV3D_BLEND_OP_ATOP,
    NV3D_BLEND_OP_ATOP_REVERSE,
    NV3D_BLEND_OP_XOR,
    NV3D_BLEND_OP_ADD,
    NV3D_BLEND_OP_SATURATE,
};

// We use two vertex streams: one for static attributes (values that are the
// same for all vertices) and one for dynamic attributes.
enum Nv3dVertexAttributeStreamType {
    NV3D_VERTEX_ATTRIBUTE_STREAM_FIRST   = 0,
    NV3D_VERTEX_ATTRIBUTE_STREAM_STATIC  = 0,
    NV3D_VERTEX_ATTRIBUTE_STREAM_DYNAMIC = 1,
    NV3D_VERTEX_ATTRIBUTE_STREAM_COUNT,
} __attribute__ ((__packed__));

/* The data type of a vertex attribute. */
/* Names of enum Nv3dVertexAttributeDataType members follow
 * "NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_{N_elements}_{element_size}_{NUMERICAL_TYPE}" convention
 * where {NUMERICAL_TYPE} gives information about NV9097_SET_VERTEX_ATTRIBUTE_A_NUMERICAL_TYPE
 */
enum Nv3dVertexAttributeDataType {
    NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_2_32_FLOAT,  /* two floats */
    NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_4_32_FLOAT,  /* four floats */
    NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_4_16_UNORM,  /* four unsigned shorts mapped to floats: [0,65535] => [0.0f,1.0f] */
    NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_4_8_UNORM,   /* four unsigned bytes mapped to floats: [0,255] => [0.0f,1.0f] */
    NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_2_16_SSCALED,/* two shorts mapped to floats: [-32768,32767] => [-32768.0f,32767.0f] */
} __attribute__ ((__packed__));

/* The possible vertex attributes. */
enum Nv3dVertexAttributeType {
    NV3D_VERTEX_ATTRIBUTE_POSITION        = 0,
    NV3D_VERTEX_ATTRIBUTE_VERTEX_WEIGHT   = 1,
    NV3D_VERTEX_ATTRIBUTE_NORMAL          = 2,
    NV3D_VERTEX_ATTRIBUTE_COLOR           = 3,
    NV3D_VERTEX_ATTRIBUTE_SECONDARY_COLOR = 4,
    NV3D_VERTEX_ATTRIBUTE_FOG_COORD       = 5,
    NV3D_VERTEX_ATTRIBUTE_POINT_SIZE      = 6,
    NV3D_VERTEX_ATTRIBUTE_MATRIX_INDEX    = 7,
    NV3D_VERTEX_ATTRIBUTE_TEXCOORD0       = 8,
    NV3D_VERTEX_ATTRIBUTE_TEXCOORD1       = 9,
    NV3D_VERTEX_ATTRIBUTE_TEXCOORD2       = 10,
    NV3D_VERTEX_ATTRIBUTE_TEXCOORD3       = 11,
    /*
     * The _END enum value is used as a sentinel to terminate arrays of
     * Nv3dVertexAttributeInfoRec (see Nv3dVertexAttributeInfoRec, below).
     */
    NV3D_VERTEX_ATTRIBUTE_END             = 255,
} __attribute__ ((__packed__));

/*
 * Nv3dVertexAttributeInfoRec stores the triplet attribute, stream type, and
 * data type.  Arrays of Nv3dVertexAttributeInfoRec are used to describe vertex
 * attribute configurations to FermiSetupVertexArrays().
 *
 * The NV3D_ATTRIB_ENTRY() and NV3D_ATTRIB_END macros can be used to make
 * Nv3dVertexAttributeInfoRec assignment more succinct.  E.g.,
 *
 *     Nv3dVertexAttributeInfoRec attribs[] = {
 *         NV3D_ATTRIB_ENTRY(COLOR, STATIC, 4UB),
 *         NV3D_ATTRIB_END,
 *     };
 */
typedef struct _Nv3dVertexAttributeInfoRec {
    enum Nv3dVertexAttributeType attributeType;
    enum Nv3dVertexAttributeStreamType streamType;
    enum Nv3dVertexAttributeDataType dataType;
} Nv3dVertexAttributeInfoRec;

#define NV3D_ATTRIB_TYPE_ENTRY(_i, _streamType, _dataType)           \
    (Nv3dVertexAttributeInfoRec)                                     \
    { .attributeType = _i,                                           \
      .streamType    = NV3D_VERTEX_ATTRIBUTE_STREAM_##_streamType,   \
      .dataType      = NV3D_VERTEX_ATTRIBUTE_DATA_TYPE_##_dataType }

#define NV3D_ATTRIB_ENTRY(_attribType, _streamType, _dataType)       \
    NV3D_ATTRIB_TYPE_ENTRY(NV3D_VERTEX_ATTRIBUTE_##_attribType, _streamType, _dataType)

#define NV3D_ATTRIB_END                                              \
    (Nv3dVertexAttributeInfoRec)                                     \
    { .attributeType = NV3D_VERTEX_ATTRIBUTE_END }

/*
 * When built into kernel code, define Nv3dFloat to be an NvU32: it is the same
 * size as a float, but the caller is responsible for storing float bit patterns
 * to Nv3dFloat.
 */
ct_assert(sizeof(float) == sizeof(NvU32));
#if NV_PUSH_ALLOW_FLOAT
typedef float Nv3dFloat;
#else
typedef NvU32 Nv3dFloat;
#endif

static inline void nv3dPushFloat(NvPushChannelPtr p, const Nv3dFloat data)
{
#if NV_PUSH_ALLOW_FLOAT
    nvPushSetMethodDataF(p, data);
#else
    nvPushSetMethodData(p, data);
#endif
}

/*
 * Vertex attribute data types.  Each of these types represents a different way
 * of specifying vertex attribute data.
 */
typedef struct __attribute__((packed)) {
    Nv3dFloat x, y;
} Nv3dVertexAttrib2F;

typedef struct __attribute__((packed)) {
    NvU32 x, y;
} Nv3dVertexAttrib2U;

typedef struct __attribute__((packed)) {
    NvS32 x, y;
} Nv3dVertexAttrib2S;

typedef struct __attribute__((packed)) {
    Nv3dFloat x, y, z;
} Nv3dVertexAttrib3F;

typedef struct __attribute__((packed)) {
    NvU32 x, y, z;
} Nv3dVertexAttrib3U;

typedef struct __attribute__((packed)) {
    Nv3dFloat x, y, z, w;
} Nv3dVertexAttrib4F;

typedef struct __attribute__((packed)) {
    NvU16 x, y, z, w;
} Nv3dVertexAttrib4US;

typedef struct __attribute__((packed)) {
    NvU8 x, y, z, w;
} Nv3dVertexAttrib4UB;

typedef struct {
    NvU32 xyzw;
} Nv3dVertexAttrib4UBPacked;

typedef struct __attribute__((packed)) {
    NvU32 xy;
} Nv3dVertexAttrib2SPacked;

// List of component sizes used for the internal representation of a
// texture header
enum Nv3dTexHeaderComponentSizes {
    NV3D_TEXHEAD_A8B8G8R8,
    NV3D_TEXHEAD_A2B10G10R10,
    NV3D_TEXHEAD_B5G6R5,
    NV3D_TEXHEAD_A1B5G5R5,
    NV3D_TEXHEAD_R8,
    NV3D_TEXHEAD_R32,
    NV3D_TEXHEAD_R16,
    NV3D_TEXHEAD_G8R8,
    NV3D_TEXHEAD_R16G16B16A16,
    NV3D_TEXHEAD_R32G32B32A32,
    NV3D_TEXHEAD_Y8_VIDEO
};

// List of component sources used for the internal representation of a
// texture header
enum Nv3dTexHeaderSource {
    NV3D_TEXHEAD_IN_A,
    NV3D_TEXHEAD_IN_R,
    NV3D_TEXHEAD_IN_G,
    NV3D_TEXHEAD_IN_B,
    NV3D_TEXHEAD_IN_ZERO,
    NV3D_TEXHEAD_IN_ONE_FLOAT
};

// List of component data types used for the internal representation of
// a texture header
enum Nv3dTexHeaderDataType {
    NV3D_TEXHEAD_NUM_UNORM,
    NV3D_TEXHEAD_NUM_UINT,
    NV3D_TEXHEAD_NUM_FLOAT,
    NV3D_TEXHEAD_NUM_SNORM,
    NV3D_TEXHEAD_NUM_SINT
};

enum Nv3dTexHeaderRepeatType {
    NV3D_TEXHEAD_REPEAT_TYPE_NONE,
    NV3D_TEXHEAD_REPEAT_TYPE_NORMAL,
    NV3D_TEXHEAD_REPEAT_TYPE_PAD,
    NV3D_TEXHEAD_REPEAT_TYPE_REFLECT
};

enum Nv3dTextureFilterType{
    NV3D_TEXHEAD_FILTER_TYPE_NEAREST,
    NV3D_TEXHEAD_FILTER_TYPE_LINEAR,
    NV3D_TEXHEAD_FILTER_TYPE_ANISO_2X,
    NV3D_TEXHEAD_FILTER_TYPE_ANISO_4X,
    NV3D_TEXHEAD_FILTER_TYPE_ANISO_8X,
    NV3D_TEXHEAD_FILTER_TYPE_ANISO_16X
};

enum Nv3dTexType {
    NV3D_TEX_TYPE_ONE_D,
    NV3D_TEX_TYPE_ONE_D_BUFFER,
    NV3D_TEX_TYPE_TWO_D_PITCH,
    NV3D_TEX_TYPE_TWO_D_BLOCKLINEAR,
};

typedef struct {
    NvU32 x;
    NvU32 y;
    NvU32 z;
} Nv3dBlockLinearLog2GobsPerBlock;

// Intermediate representation of a texture header
typedef struct {
    NvBool error;

    enum Nv3dTexHeaderComponentSizes sizes;

    // Currently, we always use the same data type for all components.
    enum Nv3dTexHeaderDataType dataType;

    struct {
        enum Nv3dTexHeaderSource x;
        enum Nv3dTexHeaderSource y;
        enum Nv3dTexHeaderSource z;
        enum Nv3dTexHeaderSource w;
    } source;

    enum Nv3dTexType texType;

    NvU64 offset;
    NvBool normalizedCoords;
    enum Nv3dTexHeaderRepeatType repeatType;
    enum Nv3dTextureFilterType filtering;
    int pitch;
    int width;
    int height;

    Nv3dBlockLinearLog2GobsPerBlock log2GobsPerBlock;
} Nv3dRenderTexInfo;

typedef NvU32 Nv3dTexSampler[8];
typedef NvU32 Nv3dTexHeader[8];

// HW representation of a texture header
typedef struct {
    Nv3dTexSampler samp;
    Nv3dTexHeader head;
} Nv3dTexture;

#define NV3D_CONSTANT_BUFFER_SIZE (4096 * 4)

#define NV3D_TEXTURE_INDEX_INVALID (-1)

#define NV3D_VERTEX_ATTRIBUTE_STREAM_SIZE (64 * 1024)

/*
 * The constant buffer alignment constraints, specifically for the methods:
 *
 *   NV*97_SET_CONSTANT_BUFFER_SELECTOR_A_SIZE
 *   NV*97_SET_CONSTANT_BUFFER_SELECTOR_C_ADDRESS_LOWER
 *
 * have evolved over GPU architectures:
 *
 *          kepler  maxwell pascal  volta   turing
 * SIZE     256     16      16      16      16
 * ADDRESS  256     256     256     256     64
 *
 * But, using an alignment of 256 all the time is simpler.
 */
#define NV3D_MIN_CONSTBUF_ALIGNMENT 256

/*
 * 3D engine pitch alignment requirements for texture surface.
 */
#define NV3D_TEXTURE_PITCH_ALIGNMENT 256

typedef struct _Nv3dStreamSurfaceRec {
    NvU64 gpuAddress;
    NvU64 size;
} Nv3dStreamSurfaceRec;

typedef struct _Nv3dVertexAttributeStreamRec {
    // Current GPU address within the stream.
    NvU64 current;
    // Terminating GPU address within the stream.
    NvU64 end;
    // Number of bytes per vertex.
    NvU32 stride;
    // Index of the next vertex to be launched.
    int nextLaunch;
} Nv3dVertexAttributeStreamRec;

typedef struct _Nv3dHal Nv3dHal;

typedef struct _Nv3dDeviceCapsRec {
    NvU32 hasSetBindlessTexture :1; /* Supports SetBindlessTexture method */
    NvU32 hasProgramRegion   :1;

    NvU32 maxDim;                /*
                                  * Maximum width or height of the
                                  * texture surface in pixels.
                                  */
} Nv3dDeviceCapsRec, *Nv3dDeviceCapsPtr;

typedef struct  _Nv3dDeviceSpaVersionRec {
    NvU16 major;
    NvU16 minor;
} Nv3dDeviceSpaVersionRec;

/*
 * Enum for each compiled shader version.
 */
enum Nv3dShaderArch {
    NV3D_SHADER_ARCH_MAXWELL,
    NV3D_SHADER_ARCH_PASCAL,
    NV3D_SHADER_ARCH_VOLTA,
    NV3D_SHADER_ARCH_TURING,
    NV3D_SHADER_ARCH_AMPERE,
    NV3D_SHADER_ARCH_HOPPER,
    NV3D_SHADER_ARCH_BLACKWELL,
    NV3D_SHADER_ARCH_COUNT,
};

typedef struct _Nv3dDeviceRec {

    NvPushDevicePtr pPushDevice;
    Nv3dDeviceCapsRec caps;
    NvU32 classNumber;
    enum Nv3dShaderArch shaderArch;

    Nv3dDeviceSpaVersionRec spaVersion;

    NvU32 maxThreadsPerWarp;
    NvU32 maxWarps;

    const Nv3dHal *hal;

} Nv3dDeviceRec, *Nv3dDevicePtr;

typedef struct _Nv3dChannelProgramsRec {
    /*
     * An array of program descriptors, and the number of elements
     * in the array.
     */
    size_t num;
    const Nv3dProgramInfo *info;

    size_t maxLocalBytes;
    size_t maxStackBytes;

    /*
     * The shader program code segment.
     *
     * The size is in bytes.
     */
    struct {
        size_t decompressedSize;
        const unsigned char *compressedStart;
        const unsigned char *compressedEnd;
    } code;

    /*
     * The constant buffers generated by the compiler for use with the above
     * code segment.
     *
     * 'size' is the total size of the surface to allocate, in bytes.
     * 'sizeAlign' is the minimum alignment required by the hardware for each
     *             particular constant buffer.  (Although we may only have
     *             N bytes of data to upload for each constant buffer, that
     *             size should be padded out with zeroes to a multiple of this
     *             value.)
     * 'count' is the number of entries in the 'info' array.
     * 'info' is a pointer to an array of Nv3dShaderConstBufInfo entries.
     */
    struct {
        size_t size;
        NvU32 sizeAlign;
        NvU32 count;
        const Nv3dShaderConstBufInfo *info;
    } constants;
} Nv3dChannelProgramsRec;

typedef struct _Nv3dChannelRec {

    Nv3dDevicePtr p3dDevice;
    NvPushChannelPtr pPushChannel;

    NvU32 handle[NV_MAX_SUBDEVICES];
    NvU16 numTextures;
    NvU16 numTextureBindings;

    Nv3dVertexAttributeStreamRec
        vertexStreams[NV3D_VERTEX_ATTRIBUTE_STREAM_COUNT];

    /*
     * Begin / end state.  ~0 if outside begin/end, or NV9097_BEGIN_OP_* if
     * inside.
     */
    NvU32 currentPrimitiveMode;

    Nv3dChannelProgramsRec programs;
    int currentProgramIndex[NV3D_HW_SHADER_STAGE_COUNT];
    NvU64 programLocalMemorySize;

    NvBool hasFrameBoundaries;

    struct {
        NvU32 handle[NV_MAX_SUBDEVICES];
        NvU64 gpuAddress;
        NvU64 programOffset;
        NvU64 programConstantsOffset;
        NvU64 programLocalMemoryOffset;
        NvU64 textureOffset;
        NvU64 bindlessTextureConstantBufferOffset;
        NvU64 constantBufferOffset;
        NvU64 vertexStreamOffset[NV3D_VERTEX_ATTRIBUTE_STREAM_COUNT];
        NvU64 totalSize;
    } surface;

} Nv3dChannelRec, *Nv3dChannelPtr;

typedef struct {
    Nv3dFloat red;
    Nv3dFloat green;
    Nv3dFloat blue;
    Nv3dFloat alpha;
} Nv3dColor;

typedef struct {
    NvU32 blendFactorSrc; /* NV9097_SET_BLEND_COLOR/ALPHA_SOURCE_COEFF_ */
    NvU32 blendFactorDst; /* NV9097_SET_BLEND_COLOR/ALPHA_DEST_COEFF_ */
    NvU32 blendEquation;  /* NV9097_SET_BLEND_COLOR/ALPHA_OP_ */
} Nv3dBlendState;
#endif /* __NVIDIA_3D_TYPES_H__ */
