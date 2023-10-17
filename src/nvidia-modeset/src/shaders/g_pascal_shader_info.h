// Generated using 'Offline GLSL Shader Compiler Version 13.0.0.0.545.00.dev/gpu_drv/dev_a-16109'
// WARNING: This file is auto-generated!  Do not hand-edit!
// Instead, edit the GLSL shaders and run 'unix-build nvmake @generate'.

#include "nvidia-3d-shaders.h"
#include "g_shader_names.h"

ct_assert(NUM_PROGRAMS == 34);
static const Nv3dProgramInfo PascalProgramInfo[NUM_PROGRAMS] = {
    // nvidia_headsurface_vertex
    { .offset = 0x00000030,
      .registerCount = 13,
      .type = NV3D_SHADER_TYPE_VERTEX,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_VERTEX_B,
      .bindGroup = NV3D_HW_BIND_GROUP_VERTEX,
    },

    // nvidia_headsurface_fragment
    { .offset = 0x000001f0,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_customSampling
    { .offset = 0x00000370,
      .registerCount = 39,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 0,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_overlay
    { .offset = 0x000030f0,
      .registerCount = 38,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_overlay_customSampling
    { .offset = 0x00003930,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset
    { .offset = 0x000057f0,
      .registerCount = 16,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_customSampling
    { .offset = 0x000059b0,
      .registerCount = 39,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 2,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_swapped
    { .offset = 0x000087b0,
      .registerCount = 16,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_swapped_customSampling
    { .offset = 0x000089b0,
      .registerCount = 39,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 3,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay
    { .offset = 0x0000b7f0,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_customSampling
    { .offset = 0x0000c0b0,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_swapped
    { .offset = 0x0000dfb0,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_swapped_customSampling
    { .offset = 0x0000e7f0,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend
    { .offset = 0x00010730,
      .registerCount = 24,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_customSampling
    { .offset = 0x000108f0,
      .registerCount = 39,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 2,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_swapped
    { .offset = 0x000136b0,
      .registerCount = 23,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_swapped_customSampling
    { .offset = 0x000138b0,
      .registerCount = 39,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 3,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay
    { .offset = 0x000166b0,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_customSampling
    { .offset = 0x00016f30,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_swapped
    { .offset = 0x00018e30,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_swapped_customSampling
    { .offset = 0x00019630,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset
    { .offset = 0x0001b570,
      .registerCount = 18,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_customSampling
    { .offset = 0x0001b770,
      .registerCount = 39,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 4,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_swapped
    { .offset = 0x0001e5b0,
      .registerCount = 18,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_swapped_customSampling
    { .offset = 0x0001e7f0,
      .registerCount = 39,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 5,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay
    { .offset = 0x00021670,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_customSampling
    { .offset = 0x00021ef0,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 6,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_swapped
    { .offset = 0x00023ef0,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_swapped_customSampling
    { .offset = 0x00024770,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = 6,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_yuv420
    { .offset = 0x000267b0,
      .registerCount = 48,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_yuv420_overlay
    { .offset = 0x00027730,
      .registerCount = 45,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_pixelShift
    { .offset = 0x00028f70,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_overlay_pixelShift
    { .offset = 0x000294f0,
      .registerCount = 38,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_reversePrime
    { .offset = 0x00029db0,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

};

static const NvU32 PascalConstantBuf0[] = {
    0x00000438, // 1.5134e-42
    0x000007f0, // 2.84744e-42
    0x00000d18, // 4.69715e-42
    0x000012f8, // 6.80471e-42
    0x00001478, // 7.3428e-42
    0x00001630, // 7.95938e-42
    0x00001a38, // 9.40552e-42
    0x00001df0, // 1.07396e-41
    0x000022f0, // 1.25332e-41
    0x00002880, // 1.45287e-41
    0x00002a00, // 1.50668e-41
    0x00002bb8, // 1.56833e-41
    0x3f400000, // 0.75
};
static const NvU32 PascalConstantBuf1[] = {
    0x000003d0, // 1.36767e-42
    0x00000770, // 2.66807e-42
    0x00000c58, // 4.4281e-42
    0x00001240, // 6.54687e-42
    0x000013c0, // 7.08497e-42
    0x00001578, // 7.70154e-42
    0x3f400000, // 0.75
};
static const NvU32 PascalConstantBuf2[] = {
    0x00000438, // 1.5134e-42
    0x000007f0, // 2.84744e-42
    0x00000d10, // 4.68594e-42
    0x000012f8, // 6.80471e-42
    0x00001478, // 7.3428e-42
    0x00001630, // 7.95938e-42
    0x00001a60, // 9.46157e-42
    0x00001e18, // 1.07956e-41
    0x00002318, // 1.25893e-41
    0x000028b0, // 1.45959e-41
    0x00002a30, // 1.5134e-41
    0x00002be0, // 1.57394e-41
    0x3f400000, // 0.75
};
static const NvU32 PascalConstantBuf3[] = {
    0x00000438, // 1.5134e-42
    0x000007f0, // 2.84744e-42
    0x00000d10, // 4.68594e-42
    0x000012f8, // 6.80471e-42
    0x00001478, // 7.3428e-42
    0x00001630, // 7.95938e-42
    0x00001aa0, // 9.55125e-42
    0x00001e58, // 1.08853e-41
    0x00002358, // 1.26789e-41
    0x000028f0, // 1.46856e-41
    0x00002a70, // 1.52237e-41
    0x00002c20, // 1.58291e-41
    0x3f400000, // 0.75
};
static const NvU32 PascalConstantBuf4[] = {
    0x00000440, // 1.52461e-42
    0x000007f8, // 2.85865e-42
    0x00000d18, // 4.69715e-42
    0x00001300, // 6.81592e-42
    0x00001480, // 7.35401e-42
    0x00001638, // 7.97059e-42
    0x00001a80, // 9.50641e-42
    0x00001e38, // 1.08404e-41
    0x00002338, // 1.26341e-41
    0x000028d0, // 1.46408e-41
    0x00002a50, // 1.51789e-41
    0x00002c00, // 1.57842e-41
    0x3f400000, // 0.75
};
static const NvU32 PascalConstantBuf5[] = {
    0x00000440, // 1.52461e-42
    0x000007f8, // 2.85865e-42
    0x00000d18, // 4.69715e-42
    0x00001300, // 6.81592e-42
    0x00001480, // 7.35401e-42
    0x00001638, // 7.97059e-42
    0x00001ac0, // 9.59609e-42
    0x00001e78, // 1.09301e-41
    0x00002378, // 1.27238e-41
    0x00002910, // 1.47304e-41
    0x00002a90, // 1.52685e-41
    0x00002c40, // 1.58739e-41
    0x3f400000, // 0.75
};
static const NvU32 PascalConstantBuf6[] = {
    0x000003f0, // 1.41251e-42
    0x000007a0, // 2.73533e-42
    0x00000ca0, // 4.529e-42
    0x00001280, // 6.63655e-42
    0x00001400, // 7.17465e-42
    0x000015b8, // 7.79122e-42
    0x3f400000, // 0.75
};

static const Nv3dShaderConstBufInfo PascalConstBufInfo[] = {
    { PascalConstantBuf0, 0, 52 },
    { PascalConstantBuf1, 256, 28 },
    { PascalConstantBuf2, 512, 52 },
    { PascalConstantBuf3, 768, 52 },
    { PascalConstantBuf4, 1024, 52 },
    { PascalConstantBuf5, 1280, 52 },
    { PascalConstantBuf6, 1536, 28 },
};

static const size_t PascalConstBufSize = 1792;
static const NvU32 PascalConstBufSizeAlign = 256;

// Total shader code size: 167.75 KB
static const size_t PascalProgramHeapSize = 171776;
static const size_t PascalShaderMaxLocalBytes = 0;
static const size_t PascalShaderMaxStackBytes = 96;
