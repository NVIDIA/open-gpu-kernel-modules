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

#ifndef NVHWMON_FAN_H
#define NVHWMON_FAN_H

#include "hwmon-main.h"

/* Probe caches static fan topology; read/write helpers assume gpu->lock. */
void nvhwmon_fan_probe(struct nvhwmon_gpu *gpu);
void nvhwmon_fan_restore_all(struct nvhwmon_gpu *gpu);

bool nvhwmon_fan_has_max_rpm(const struct nvhwmon_gpu *gpu, u32 channel);
int nvhwmon_fan_read_max_rpm(struct nvhwmon_gpu *gpu, u32 channel, long *val);
bool nvhwmon_fan_has_pwm(const struct nvhwmon_gpu *gpu, u32 channel);
int nvhwmon_fan_read_pwm(struct nvhwmon_gpu *gpu, u32 channel, long *val);
int nvhwmon_fan_write_pwm(struct nvhwmon_gpu *gpu, u32 channel, long val);
int nvhwmon_fan_read_pwm_enable(struct nvhwmon_gpu *gpu, u32 channel, long *val);
int nvhwmon_fan_write_pwm_enable(struct nvhwmon_gpu *gpu, u32 channel, long val);
int nvhwmon_fan_read_pwm_mode(struct nvhwmon_gpu *gpu, u32 channel, long *val);
int nvhwmon_fan_write_pwm_mode(struct nvhwmon_gpu *gpu, u32 channel, long val);

#endif
