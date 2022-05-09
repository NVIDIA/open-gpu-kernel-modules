/*
 * SPDX-FileCopyrightText: Copyright (c) 2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NV_CAPS_H_
#define _NV_CAPS_H_

#include <nv-kernel-interface-api.h>

/*
 * Opaque OS-specific struct; on Linux, this has member
 * 'struct proc_dir_entry'.
 */
typedef struct nv_cap nv_cap_t;

/*
 * Creates directory named "capabilities" under the provided path.
 *
 * @param[in]   path        Absolute path
 *
 * Returns a valid nv_cap_t upon success. Otherwise, returns NULL.
 */
nv_cap_t* NV_API_CALL nv_cap_init(const char *path);

/*
 * Creates capability directory entry
 *
 * @param[in]   parent_cap     Parent capability directory
 * @param[in]   name           Capability directory's name
 * @param[in]   mode           Capability directory's access mode
 *
 * Returns a valid nv_cap_t upon success. Otherwise, returns NULL.
 */
nv_cap_t* NV_API_CALL nv_cap_create_dir_entry(nv_cap_t *parent_cap, const char *name, int mode);

/*
 * Creates capability file entry
 *
 * @param[in]   parent_cap     Parent capability directory
 * @param[in]   name           Capability file's name
 * @param[in]   mode           Capability file's access mode
 *
 * Returns a valid nv_cap_t upon success. Otherwise, returns NULL.
 */
nv_cap_t* NV_API_CALL nv_cap_create_file_entry(nv_cap_t *parent_cap, const char *name, int mode);

/*
 * Destroys capability entry
 *
 * @param[in]   cap              Capability entry
 */
void NV_API_CALL nv_cap_destroy_entry(nv_cap_t *cap);

/*
 * Validates and duplicates the provided file descriptor
 *
 * @param[in]   cap         Capability entry
 * @param[in]   fd          File descriptor to be validated
 *
 * Returns duplicate fd upon success. Otherwise, returns -1.
 */
int NV_API_CALL nv_cap_validate_and_dup_fd(const nv_cap_t *cap, int fd);

/*
 * Closes file descriptor
 *
 * This function should be used to close duplicate file descriptors
 * returned by nv_cap_validate_and_dup_fd.
 *
 * @param[in]   fd          File descriptor to be validated
 *
 */
void NV_API_CALL nv_cap_close_fd(int fd);

#endif  /* _NV_CAPS_H_ */
