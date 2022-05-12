/*
 * Copyright 2018 Advanced Micro Devices, Inc.
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
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef __AMDGPU_DISCOVERY__
#define __AMDGPU_DISCOVERY__

#define DISCOVERY_TMR_SIZE      (4 << 10)
#define DISCOVERY_TMR_OFFSET    (64 << 10)

void amdgpu_discovery_fini(struct amdgpu_device *adev);
int amdgpu_discovery_reg_base_init(struct amdgpu_device *adev);
void amdgpu_discovery_harvest_ip(struct amdgpu_device *adev);
int amdgpu_discovery_get_ip_version(struct amdgpu_device *adev, int hw_id, int number_instance,
                                    int *major, int *minor, int *revision);

int amdgpu_discovery_get_gfx_info(struct amdgpu_device *adev);
int amdgpu_discovery_set_ip_blocks(struct amdgpu_device *adev);

#endif /* __AMDGPU_DISCOVERY__ */
