/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * Data structures and interfaces used for generic falcon boot-loader.
 *
 * This generic boot-loader is designed to load both non-secure and
 * secure code taking care of signature as well. This bootloader
 * should be loaded at the end of the IMEM so that it doesnt overwrite
 * itself when it tries to load the code into IMEM starting at blk 0.
 * The data will be loaded into DMEM offset 0.
 */

#ifndef RMFLCNBL_H
#define RMFLCNBL_H

#include "flcnifcmn.h"

/*!
 * @brief Structure used by the boot-loader to load the rest of the code.
 *
 * This has to be filled by the GPU driver and copied into DMEM at offset
 * provided in the @ref _def_rm_flcn_bl_desc.blDmemDescLoadOff.
 */
typedef struct _def_rm_flcn_bl_dmem_desc
{
    /*!
     * reserved should be always first element
     */
    NvU32       reserved[4];
    /*!
     * signature should follow reserved 16B signature for secure code.
     * 0s if no secure code
     */
    NvU32       signature[4];
    /*!
     * ctxDma is used by the bootloader while loading code/data.
     */
    NvU32       ctxDma;
    /*!
     * 256B aligned physical FB address where code is located.
     */
    RM_FLCN_U64 codeDmaBase;
    /*!
     * Offset from codeDmaBase where the nonSecure code is located.
     * The offset must be multiple of 256 to help performance.
     */
    NvU32       nonSecureCodeOff;
    /*!
     * The size of the nonSecure code part.
     */
    NvU32       nonSecureCodeSize;
    /*!
     * Offset from codeDmaBase where the secure code is located.
     * The offset must be multiple of 256 to help performance.
     */
    NvU32       secureCodeOff;
    /*!
     * The size of the ecure code part.
     */
    NvU32       secureCodeSize;
    /*!
     * Code entry point which will be invoked by BL after code is loaded.
     */
    NvU32       codeEntryPoint;
    /*!
     * 256B aligned Physical FB Address where data is located.
     */
    RM_FLCN_U64 dataDmaBase;
    /*!
     * Size of data block. Should be multiple of 256B.
     */
    NvU32       dataSize;
    /*!
     * Arguments to be passed to the target firmware being loaded.
     */
    NvU32       argc;
    /*!
     * Number of arguments to be passed to the target firmware being loaded.
     */
    NvU32       argv;
} RM_FLCN_BL_DMEM_DESC, *PRM_FLCN_BL_DMEM_DESC;

/*!
 * @brief The header used by the GPU driver to figure out code and data
 * sections of bootloader.
 */
typedef struct _def_rm_flcn_bl_img_header
{
    /*!
     * Offset of code section in the image.
     */
    NvU32 blCodeOffset;
    /*!
     * Size of code section in the image.
     */
    NvU32 blCodeSize;
    /*!
     * Offset of data section in the image.
     */
    NvU32 blDataOffset;
    /*!
     * Size of data section in the image.
     */
    NvU32 blDataSize;
} RM_FLCN_BL_IMG_HEADER, *PRM_FLCN_BL_IMG_HEADER;

/*!
 * @brief The descriptor used by RM to figure out the requirements of boot loader.
 */
typedef struct _def_rm_flcn_bl_desc
{
    /*!
     *  Starting tag of bootloader
     */
    NvU32 blStartTag;
    /*!
     *  Dmem offset where _def_rm_flcn_bl_dmem_desc to be loaded
     */
    NvU32 blDmemDescLoadOff;
    /*!
     *  Description of the image
     */
    RM_FLCN_BL_IMG_HEADER blImgHeader;
} RM_FLCN_BL_DESC, *PRM_FLCN_BL_DESC;

#endif // RMFLCNBL_H
