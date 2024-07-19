/*******************************************************************************
    Copyright (c) 2013-2023 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#ifndef _ctrl2080mc_h_
#define _ctrl2080mc_h_

/* valid architecture values */
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_T13X                 (0xE0000013)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM000                (0x00000110)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GM200                (0x00000120)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GP100                (0x00000130)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GV100                (0x00000140)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_TU100                (0x00000160)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GA100                (0x00000170)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GH100                (0x00000180)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_AD100                (0x00000190)
#define NV2080_CTRL_MC_ARCH_INFO_ARCHITECTURE_GB100                (0x000001A0)

/* valid ARCHITECTURE_GP10x implementation values */
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GP100              (0x00000000)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GP000              (0x00000001)

#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GA100              (0x00000000)
#define NV2080_CTRL_MC_ARCH_INFO_IMPLEMENTATION_GA000              (0x00000001)
#endif /* _ctrl2080mc_h_ */
