// Generated using 'Offline GLSL Shader Compiler Version 13.0.0.0.545.00.dev/gpu_drv/dev_a-16109'
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
    { .offset = 0x00000300,
      .registerCount = 13,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_customSampling
    { .offset = 0x00000580,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_overlay
    { .offset = 0x00004680,
      .registerCount = 31,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_overlay_customSampling
    { .offset = 0x00005480,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset
    { .offset = 0x00008280,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_customSampling
    { .offset = 0x00008600,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_swapped
    { .offset = 0x0000c800,
      .registerCount = 16,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_swapped_customSampling
    { .offset = 0x0000cb80,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay
    { .offset = 0x00010d80,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_customSampling
    { .offset = 0x00011c00,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_swapped
    { .offset = 0x00014b00,
      .registerCount = 31,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_offset_overlay_swapped_customSampling
    { .offset = 0x00015980,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend
    { .offset = 0x00018880,
      .registerCount = 15,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_customSampling
    { .offset = 0x00018b80,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_swapped
    { .offset = 0x0001cd00,
      .registerCount = 17,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_swapped_customSampling
    { .offset = 0x0001d080,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay
    { .offset = 0x00021280,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_customSampling
    { .offset = 0x00022100,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_swapped
    { .offset = 0x00024f80,
      .registerCount = 31,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_overlay_swapped_customSampling
    { .offset = 0x00025e00,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset
    { .offset = 0x00028d00,
      .registerCount = 20,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_customSampling
    { .offset = 0x00029100,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_swapped
    { .offset = 0x0002d380,
      .registerCount = 20,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_swapped_customSampling
    { .offset = 0x0002d780,
      .registerCount = 40,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay
    { .offset = 0x00031a00,
      .registerCount = 35,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_customSampling
    { .offset = 0x00032880,
      .registerCount = 37,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_swapped
    { .offset = 0x00035780,
      .registerCount = 36,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_blend_offset_overlay_swapped_customSampling
    { .offset = 0x00036680,
      .registerCount = 37,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_yuv420
    { .offset = 0x00039600,
      .registerCount = 56,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_yuv420_overlay
    { .offset = 0x0003b180,
      .registerCount = 38,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_pixelShift
    { .offset = 0x0003dc80,
      .registerCount = 32,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_overlay_pixelShift
    { .offset = 0x0003e880,
      .registerCount = 31,
      .type = NV3D_SHADER_TYPE_PIXEL,
      .constIndex = -1,
      .stage = NV3D_HW_SHADER_STAGE_PIXEL,
      .bindGroup = NV3D_HW_BIND_GROUP_FRAGMENT,
    },

    // nvidia_headsurface_fragment_reversePrime
    { .offset = 0x0003f780,
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

// Total shader code size: 254.5 KB
static const size_t BlackwellProgramHeapSize = 260608;
static const size_t BlackwellShaderMaxLocalBytes = 0;
static const size_t BlackwellShaderMaxStackBytes = 0;
