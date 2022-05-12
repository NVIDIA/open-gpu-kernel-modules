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

#ifndef __TAG_H_INCLUDED__
#define __TAG_H_INCLUDED__

/*
 * This file is included on every cell {SP,ISP,host} and is system agnostic
 *
 * System and cell specific interfaces and inline code are included
 * conditionally through Makefile path settings.
 *
 *  - .        system and cell agnostic interfaces, constants and identifiers
 *	- public:  cell specific interfaces
 *	- private: cell specific inline implementations
 *	- global:  inter cell constants and identifiers
 *	- local:   cell specific constants and identifiers
 *
 */

#include "tag_local.h"

#ifndef __INLINE_TAG__
#define STORAGE_CLASS_TAG_H extern
#define STORAGE_CLASS_TAG_C
#include "tag_public.h"
#else  /* __INLINE_TAG__ */
#define STORAGE_CLASS_TAG_H static inline
#define STORAGE_CLASS_TAG_C static inline
#include "tag_private.h"
#endif /* __INLINE_TAG__ */

#endif /* __TAG_H_INCLUDED__ */
