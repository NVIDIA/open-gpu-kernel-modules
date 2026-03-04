/*
* SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
*
* Comments, prototypes and checks taken from DMTF: Copyright 2021-2022 DMTF. All rights reserved.
* License: BSD 3-Clause License. For full text see link: https://github.com/DMTF/libspdm/blob/main/LICENSE.md
*/

#include "os-interface.h"
#include "internal_crypt_lib.h"
#include "library/cryptlib.h"

bool libspdm_check_crypto_backend(void)
{
#ifdef USE_LKCA
    nv_printf(NV_DBG_INFO, "libspdm_check_crypto_backend: LKCA wrappers found.\n");
    nv_printf(NV_DBG_INFO, "libspdm_check_crypto_backend: LKCA calls may still fail if modules have not been loaded!\n");
    return true;
#else
    nv_printf(NV_DBG_ERRORS, "libspdm_check_crypto_backend: Error - libspdm expects LKCA but found stubs!\n");
    return false;
#endif
}

