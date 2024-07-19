/* 
 * Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
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

#ifndef _clc370_notification_h_
#define _clc370_notification_h_

#ifdef __cplusplus
extern "C" {
#endif

#include "class/cl5070.h"

/* event values */
#define NVC370_NOTIFIERS_SW                     NV5070_NOTIFIERS_SW
#define NVC370_NOTIFIERS_BEGIN                  NV5070_NOTIFIERS_MAXCOUNT
#define NVC370_NOTIFIERS_RG_SEM_NOTIFICATION    NVC370_NOTIFIERS_BEGIN + (0)
#define NVC370_NOTIFIERS_WIN_SEM_NOTIFICATION   NVC370_NOTIFIERS_RG_SEM_NOTIFICATION + (1)
#define NVC370_NOTIFIERS_LTM_CALC_TIMEOUT       NVC370_NOTIFIERS_WIN_SEM_NOTIFICATION + (1)
#define NVC370_NOTIFIERS_MAXCOUNT               NVC370_NOTIFIERS_LTM_CALC_TIMEOUT + (1)

#ifdef __cplusplus
};     /* extern "C" */
#endif

#endif /* _clc370_notification_h_ */

