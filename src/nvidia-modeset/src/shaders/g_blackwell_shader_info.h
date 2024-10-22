// Generated using 'Offline GLSL Shader Compiler Version 13.0.0.0.560.00.dev/gpu_drv/bugfix_main-17009'
// WARNING: This file is auto-generated!  Do not hand-edit!
// Instead, edit the GLSL shaders and run 'unix-build nvmake @generate'.

#include "nvidia-3d-shaders.h"
#include "g_shader_names.h"

ct_assert(NUM_PROGRAMS == 34);
static const Nv3dProgramInfo BlackwellProgramInfo[NUM_PROGRAMS] = {
    // nvidia_headsurface_vertex
    { .offset = 0x00000000,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_VERTEX,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_VERTEX_B,
      .bindGroup = NV3D_HW_BIND_GROUP_VERTEX,
    },

    // nvidia_headsurface_fragment
    { .offset = 0x00000280,
      .registerCount = 12,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_customSampling
    { .offset = 0x00000480,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_overlay
    { .offset = 0x00004500,
      .registerCount = 31,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_overlay_customSampling
    { .offset = 0x00005200,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset
    { .offset = 0x00007f00,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_customSampling
    { .offset = 0x00008200,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_swapped
    { .offset = 0x0000c380,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_swapped_customSampling
    { .offset = 0x0000c680,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay
    { .offset = 0x00010800,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_customSampling
    { .offset = 0x00011580,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_swapped
    { .offset = 0x00014380,
      .registerCount = 31,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_swapped_customSampling
    { .offset = 0x00015100,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend
    { .offset = 0x00017f00,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_customSampling
    { .offset = 0x00018180,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_swapped
    { .offset = 0x0001c280,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_swapped_customSampling
    { .offset = 0x0001c580,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay
    { .offset = 0x00020700,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_customSampling
    { .offset = 0x00021480,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_swapped
    { .offset = 0x00024200,
      .registerCount = 31,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_swapped_customSampling
    { .offset = 0x00024f80,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset
    { .offset = 0x00027d80,
      .registerCount = 20,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_customSampling
    { .offset = 0x00028100,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_swapped
    { .offset = 0x0002c300,
      .registerCount = 20,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_swapped_customSampling
    { .offset = 0x0002c680,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay
    { .offset = 0x00030880,
      .registerCount = 35,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_customSampling
    { .offset = 0x00031600,
      .registerCount = 37,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_swapped
    { .offset = 0x00034400,
      .registerCount = 36,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_swapped_customSampling
    { .offset = 0x00035200,
      .registerCount = 37,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_yuv420
    { .offset = 0x00038080,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_yuv420_overlay
    { .offset = 0x00039880,
      .registerCount = 38,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_pixelShift
    { .offset = 0x0003c180,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_overlay_pixelShift
    { .offset = 0x0003cd00,
      .registerCount = 31,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_reversePrime
    { .offset = 0x0003db00,
      .registerCount = 13,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

};


static const Nv3dShaderConstBufInfo BlackwellConstBufInfo[] = {
};

static const size_t BlackwellConstBufSize = 0;
static const NvU32 BlackwellConstBufSizeAlign = 256;

// Total shader code size: 247.25 KB
static const size_t BlackwellProgramHeapSize = 253184;
static const size_t BlackwellShaderMaxLocalBytes = 0;
static const size_t BlackwellShaderMaxStackBytes = 0;
