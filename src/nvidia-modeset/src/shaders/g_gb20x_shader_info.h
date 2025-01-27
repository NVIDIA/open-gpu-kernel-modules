// Generated using 'Offline GLSL Shader Compiler Version 13.0.0.0.560.00.dev/gpu_drv/bugfix_main-17009'
// WARNING: This file is auto-generated!  Do not hand-edit!
// Instead, edit the GLSL shaders and run 'unix-build nvmake @generate'.

#include "nvidia-3d-shaders.h"
#include "g_shader_names.h"

ct_assert(NUM_PROGRAMS == 34);
static const Nv3dProgramInfo GB20xProgramInfo[NUM_PROGRAMS] = {
    // nvidia_headsurface_vertex
    { .offset = 0x00000000,
      .registerCount = 11,
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
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_overlay_customSampling
    { .offset = 0x00005200,
      .registerCount = 35,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset
    { .offset = 0x00007e80,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_customSampling
    { .offset = 0x00008180,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_swapped
    { .offset = 0x0000c300,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_swapped_customSampling
    { .offset = 0x0000c600,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay
    { .offset = 0x00010780,
      .registerCount = 36,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_customSampling
    { .offset = 0x00011500,
      .registerCount = 36,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_swapped
    { .offset = 0x00014200,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_swapped_customSampling
    { .offset = 0x00014f80,
      .registerCount = 35,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend
    { .offset = 0x00017d00,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_customSampling
    { .offset = 0x00017f80,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_swapped
    { .offset = 0x0001c080,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_swapped_customSampling
    { .offset = 0x0001c380,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay
    { .offset = 0x00020500,
      .registerCount = 36,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_customSampling
    { .offset = 0x00021280,
      .registerCount = 36,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_swapped
    { .offset = 0x00023f80,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_swapped_customSampling
    { .offset = 0x00024c80,
      .registerCount = 35,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset
    { .offset = 0x00027a00,
      .registerCount = 20,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_customSampling
    { .offset = 0x00027d80,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_swapped
    { .offset = 0x0002bf80,
      .registerCount = 20,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_swapped_customSampling
    { .offset = 0x0002c300,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay
    { .offset = 0x00030500,
      .registerCount = 34,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_customSampling
    { .offset = 0x00031280,
      .registerCount = 37,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_swapped
    { .offset = 0x00034080,
      .registerCount = 36,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_swapped_customSampling
    { .offset = 0x00034e00,
      .registerCount = 37,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_yuv420
    { .offset = 0x00037c00,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_yuv420_overlay
    { .offset = 0x00039480,
      .registerCount = 38,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_pixelShift
    { .offset = 0x0003bd80,
      .registerCount = 36,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_overlay_pixelShift
    { .offset = 0x0003c900,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_reversePrime
    { .offset = 0x0003d780,
      .registerCount = 13,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

};


static const Nv3dShaderConstBufInfo GB20xConstBufInfo[] = {
};

static const size_t GB20xConstBufSize = 0;
static const NvU32 GB20xConstBufSizeAlign = 256;

// Total shader code size: 246.375 KB
static const size_t GB20xProgramHeapSize = 252288;
static const size_t GB20xShaderMaxLocalBytes = 0;
static const size_t GB20xShaderMaxStackBytes = 0;
