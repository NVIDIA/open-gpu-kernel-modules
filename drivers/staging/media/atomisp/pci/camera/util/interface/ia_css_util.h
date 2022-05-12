/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Support for Intel Camera Imaging ISP subsystem.
 * Copyright (c) 2015, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef __IA_CSS_UTIL_H__
#define __IA_CSS_UTIL_H__

#include <linux/errno.h>

#include <ia_css_err.h>
#include <type_support.h>
#include <ia_css_frame_public.h>
#include <ia_css_stream_public.h>
#include <ia_css_stream_format.h>

/* @brief convert "errno" error code to "ia_css_err" error code
 *
 * @param[in]	"errno" error code
 * @return	"ia_css_err" error code
 *
 */
int ia_css_convert_errno(
    int in_err);

/* @brief check vf frame info.
 *
 * @param[in] info
 * @return	0 or error code upon error.
 *
 */
int ia_css_util_check_vf_info(
    const struct ia_css_frame_info *const info);

/* @brief check input configuration.
 *
 * @param[in] stream_config
 * @param[in] must_be_raw
 * @return	0 or error code upon error.
 *
 */
int ia_css_util_check_input(
    const struct ia_css_stream_config *const stream_config,
    bool must_be_raw,
    bool must_be_yuv);

/* @brief check vf and out frame info.
 *
 * @param[in] out_info
 * @param[in] vf_info
 * @return	0 or error code upon error.
 *
 */
int ia_css_util_check_vf_out_info(
    const struct ia_css_frame_info *const out_info,
    const struct ia_css_frame_info *const vf_info);

/* @brief check width and height
 *
 * @param[in] width
 * @param[in] height
 * @return	0 or error code upon error.
 *
 */
int ia_css_util_check_res(
    unsigned int width,
    unsigned int height);

/* ISP2401 */
/* @brief compare resolutions (less or equal)
 *
 * @param[in] a resolution
 * @param[in] b resolution
 * @return    true if both dimensions of a are less or
 *            equal than those of b, false otherwise
 *
 */
bool ia_css_util_res_leq(
    struct ia_css_resolution a,
    struct ia_css_resolution b);

/* ISP2401 */
/**
 * @brief Check if resolution is zero
 *
 * @param[in] resolution The resolution to check
 *
 * @returns true if resolution is zero
 */
bool ia_css_util_resolution_is_zero(
    const struct ia_css_resolution resolution);

/* ISP2401 */
/**
 * @brief Check if resolution is even
 *
 * @param[in] resolution The resolution to check
 *
 * @returns true if resolution is even
 */
bool ia_css_util_resolution_is_even(
    const struct ia_css_resolution resolution);

/* @brief check width and height
 *
 * @param[in] stream_format
 * @param[in] two_ppc
 * @return bits per pixel based on given parameters.
 *
 */
unsigned int ia_css_util_input_format_bpp(
    enum atomisp_input_format stream_format,
    bool two_ppc);

/* @brief check if input format it raw
 *
 * @param[in] stream_format
 * @return true if the input format is raw or false otherwise
 *
 */
bool ia_css_util_is_input_format_raw(
    enum atomisp_input_format stream_format);

/* @brief check if input format it yuv
 *
 * @param[in] stream_format
 * @return true if the input format is yuv or false otherwise
 *
 */
bool ia_css_util_is_input_format_yuv(
    enum atomisp_input_format stream_format);

#endif /* __IA_CSS_UTIL_H__ */
