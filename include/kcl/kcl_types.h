/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_TYPES_H
#define AMDKCL_TYPES_H

/* Copied from v4.15-rc1-4-g8ced390c2b18 include/uapi/linux/types.h */
#ifndef HAVE_TYPE__POLL_T
#ifdef __CHECK_POLL
typedef unsigned __bitwise __poll_t;
#else
typedef unsigned __poll_t;
#endif
#endif
#endif
