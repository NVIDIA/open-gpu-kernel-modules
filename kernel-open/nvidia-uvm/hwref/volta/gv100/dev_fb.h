/*******************************************************************************
    Copyright (c) 2017 NVIDIA Corporation

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

// Excerpt of gv100/dev_fb.h

#ifndef __dev_fb_h__
#define __dev_fb_h__

#define NV_PFB_PRI_MMU_FAULT_BUFFER_LO(i)                     (0x00100E24+(i)*20) /* RW-4A */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_LO__SIZE_1                          2 /*       */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_LO_ADDR_MODE                      0:0 /* RW-VF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_LO_ADDR_MODE_VIRTUAL       0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_LO_ADDR_MODE_PHYSICAL      0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_LO_PHYS_APERTURE                  2:1 /* RW-VF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_LO_PHYS_APERTURE_LOCAL     0x00000000 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_LO_PHYS_APERTURE_SYS_COH   0x00000002 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_LO_PHYS_APERTURE_SYS_NCOH  0x00000003 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_LO_PHYS_VOL                       3:3 /* RW-VF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_LO_ADDR                         31:12 /* RW-VF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_HI(i)                     (0x00100E28+(i)*20) /* RW-4A */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_HI__SIZE_1                          2 /*       */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_HI_ADDR                          31:0 /* RW-VF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET(i)                         (0x00100E2C+(i)*20) /* RW-4A */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET__SIZE_1                     2 /*       */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_PTR                              19:0 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_PTR_RESET                  0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_GETPTR_CORRUPTED                30:30 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_GETPTR_CORRUPTED_NO        0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_GETPTR_CORRUPTED_YES       0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_GETPTR_CORRUPTED_CLEAR     0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_OVERFLOW                        31:31 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_OVERFLOW_NO                0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_OVERFLOW_YES               0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_GET_OVERFLOW_CLEAR             0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT(i)                         (0x00100E30+(i)*20) /* R--4A */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT__SIZE_1                    2 /*       */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_PTR                               19:0 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_PTR_RESET                   0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_GETPTR_CORRUPTED                 30:30 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_GETPTR_CORRUPTED_NO         0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_GETPTR_CORRUPTED_YES        0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_OVERFLOW                         31:31 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_OVERFLOW_NO                 0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_PUT_OVERFLOW_YES                0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE(i)                    (0x00100E34+(i)*20) /* RW-4A */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE__SIZE_1                         2 /*       */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_VAL                          19:0 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_VAL_RESET              0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_OVERFLOW_INTR               29:29 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_OVERFLOW_INTR_DISABLE  0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_OVERFLOW_INTR_ENABLE   0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_SET_DEFAULT                 30:30 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_SET_DEFAULT_NO         0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_SET_DEFAULT_YES        0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_ENABLE                      31:31 /* RWEVF */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_ENABLE_FALSE           0x00000000 /* RWE-V */
#define NV_PFB_PRI_MMU_FAULT_BUFFER_SIZE_ENABLE_TRUE            0x00000001 /* RW--V */
#define NV_PFB_PRI_MMU_FAULT_ADDR_LO                                      0x00100E4C /* R--4R */
#define NV_PFB_PRI_MMU_FAULT_ADDR_LO_PHYS_APERTURE                               1:0 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_ADDR_LO_PHYS_APERTURE_LOCAL                  0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_ADDR_LO_PHYS_APERTURE_PEER                   0x00000001 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_ADDR_LO_PHYS_APERTURE_SYS_COH                0x00000002 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_ADDR_LO_PHYS_APERTURE_SYS_NCOH               0x00000003 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_ADDR_LO_ADDR                                      31:12 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_ADDR_LO_ADDR_RESET                           0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_ADDR_HI                                      0x00100E50 /* R--4R */
#define NV_PFB_PRI_MMU_FAULT_ADDR_HI_ADDR                                       31:0 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_ADDR_HI_ADDR_RESET                           0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INST_LO                                      0x00100E54 /* R--4R */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_ENGINE_ID                                   8:0 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_ENGINE_ID_RESET                      0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_APERTURE                                  11:10 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_APERTURE_VID_MEM                     0x00000000 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_APERTURE_SYS_MEM_COHERENT            0x00000002 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_APERTURE_SYS_MEM_NONCOHERENT         0x00000003 /* R---V */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_APERTURE_RESET                       0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_ADDR                                      31:12 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INST_LO_ADDR_RESET                           0x00000000 /* R-E-V */
#define NV_PFB_PRI_MMU_FAULT_INST_HI                                      0x00100E58 /* R--4R */
#define NV_PFB_PRI_MMU_FAULT_INST_HI_ADDR                                       31:0 /* R-EVF */
#define NV_PFB_PRI_MMU_FAULT_INST_HI_ADDR_RESET                           0x00000000 /* R-E-V */

#endif
