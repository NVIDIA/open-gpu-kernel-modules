/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 Jihong Min. All rights reserved.
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

#ifndef NVHWMON_RUSD_H
#define NVHWMON_RUSD_H

#include "hwmon-main.h"

/* RUSD init maps shared telemetry; read helpers are serialized by gpu->lock. */
int nvhwmon_rusd_init(struct nvhwmon_gpu *gpu);
void nvhwmon_rusd_fini(struct nvhwmon_gpu *gpu);

int nvhwmon_rusd_read_temp(struct nvhwmon_gpu *gpu, u32 channel, long *val);
int nvhwmon_rusd_read_power(struct nvhwmon_gpu *gpu, u32 channel, long *val);
int nvhwmon_rusd_read_power_average(struct nvhwmon_gpu *gpu, u32 channel,
				    long *val);
int nvhwmon_rusd_read_power_cap(struct nvhwmon_gpu *gpu, u32 channel, long *val);
int nvhwmon_rusd_read_fan(struct nvhwmon_gpu *gpu, u32 channel, long *val);

bool nvhwmon_rusd_power_has_average(u32 channel);
bool nvhwmon_rusd_power_has_cap(u32 channel);

const char *nvhwmon_rusd_temp_label(u32 channel);
const char *nvhwmon_rusd_power_label(u32 channel);

#endif
