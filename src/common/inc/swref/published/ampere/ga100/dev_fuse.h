/*
 * SPDX-FileCopyrightText: Copyright (c) 2003-2022 NVIDIA CORPORATION & AFFILIATES
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
#ifndef __ga100_dev_fuse_h__
#define __ga100_dev_fuse_h__

#define NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS                                                            0x0082074C /* RW-4R */
#define NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS_DATA                                                              0:0 /* RWIVF */
#define NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS_DATA_NO                                                    0x00000000 /* RW--V */
#define NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS_DATA_YES                                                   0x00000001 /* RW--V */

#define NV_FUSE_OPT_NVDEC_DISABLE                                                                   0x00820378 /* RW-4R */
#define NV_FUSE_OPT_NVDEC_DISABLE_DATA                                                                     4:0 /* RWIVF */
#define NV_FUSE_OPT_NVDEC_DISABLE_DATA_INIT                                                         0x00000000 /* RWI-V */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE1_VERSION                                                        0x00824100 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE1_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE2_VERSION                                                        0x00824104 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE2_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE3_VERSION                                                        0x00824108 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE3_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE4_VERSION                                                        0x0082410C /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE4_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE5_VERSION                                                        0x00824110 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE5_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE6_VERSION                                                        0x00824114 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE6_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE7_VERSION                                                        0x00824118 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE7_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE8_VERSION                                                        0x0082411C /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE8_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE9_VERSION                                                        0x00824120 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE9_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE10_VERSION                                                       0x00824124 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE10_VERSION_DATA                                                        15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE11_VERSION                                                       0x00824128 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE11_VERSION_DATA                                                        15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE12_VERSION                                                       0x0082412C /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE12_VERSION_DATA                                                        15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE13_VERSION                                                       0x00824130 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE13_VERSION_DATA                                                        15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE14_VERSION                                                       0x00824134 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE14_VERSION_DATA                                                        15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE15_VERSION                                                       0x00824138 /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE15_VERSION_DATA                                                        15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE16_VERSION                                                       0x0082413C /* RW-4R */
#define NV_FUSE_OPT_FPF_NVDEC_UCODE16_VERSION_DATA                                                        15:0 /* RWIVF */

#define NV_FUSE_OPT_FPF_SEC2_UCODE1_VERSION                                                         0x00824140 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE1_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE2_VERSION                                                         0x00824144 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE2_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE3_VERSION                                                         0x00824148 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE3_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE4_VERSION                                                         0x0082414C /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE4_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE5_VERSION                                                         0x00824150 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE5_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE6_VERSION                                                         0x00824154 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE6_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE7_VERSION                                                         0x00824158 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE7_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE8_VERSION                                                         0x0082415C /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE8_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE9_VERSION                                                         0x00824160 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE9_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE10_VERSION                                                        0x00824164 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE10_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE11_VERSION                                                        0x00824168 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE11_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE12_VERSION                                                        0x0082416C /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE12_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE13_VERSION                                                        0x00824170 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE13_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE14_VERSION                                                        0x00824174 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE14_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE15_VERSION                                                        0x00824178 /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE15_VERSION_DATA                                                         15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_SEC2_UCODE16_VERSION                                                        0x0082417C /* RW-4R */
#define NV_FUSE_OPT_FPF_SEC2_UCODE16_VERSION_DATA                                                         15:0 /* RWIVF */

#define NV_FUSE_OPT_FPF_GSP_UCODE1_VERSION                                                          0x008241C0 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE1_VERSION_DATA                                                           15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE2_VERSION                                                          0x008241C4 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE2_VERSION_DATA                                                           15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE3_VERSION                                                          0x008241C8 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE3_VERSION_DATA                                                           15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE4_VERSION                                                          0x008241CC /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE4_VERSION_DATA                                                           15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE5_VERSION                                                          0x008241D0 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE5_VERSION_DATA                                                           15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE6_VERSION                                                          0x008241D4 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE6_VERSION_DATA                                                           15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE7_VERSION                                                          0x008241D8 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE7_VERSION_DATA                                                           15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE8_VERSION                                                          0x008241DC /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE8_VERSION_DATA                                                           15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE9_VERSION                                                          0x008241E0 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE9_VERSION_DATA                                                           15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE10_VERSION                                                         0x008241E4 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE10_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE11_VERSION                                                         0x008241E8 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE11_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE12_VERSION                                                         0x008241EC /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE12_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE13_VERSION                                                         0x008241F0 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE13_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE14_VERSION                                                         0x008241F4 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE14_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE15_VERSION                                                         0x008241F8 /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE15_VERSION_DATA                                                          15:0 /* RWIVF */
#define NV_FUSE_OPT_FPF_GSP_UCODE16_VERSION                                                         0x008241FC /* RW-4R */
#define NV_FUSE_OPT_FPF_GSP_UCODE16_VERSION_DATA                                                          15:0 /* RWIVF */

#define NV_FUSE_STATUS_OPT_DISPLAY                                                                  0x00820C04 /* R-I4R */
#define NV_FUSE_STATUS_OPT_DISPLAY_DATA                                                                    0:0 /* R-IVF */
#define NV_FUSE_STATUS_OPT_DISPLAY_DATA_ENABLE                                                      0x00000000 /* R---V */

#endif // __ga100_dev_fuse_h__
