/* SPDX-License-Identifier: GPL-2.0+ WITH Linux-syscall-note */
/*
 * Copyright (c) 2003+ Evgeniy Polyakov <johnpol@2ka.mxt.ru>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _XT_OSF_H
#define _XT_OSF_H

#include <linux/types.h>
#include <linux/netfilter/nfnetlink_osf.h>

#define XT_OSF_GENRE		NF_OSF_GENRE
#define XT_OSF_INVERT		NF_OSF_INVERT

#define XT_OSF_TTL		NF_OSF_TTL
#define XT_OSF_LOG		NF_OSF_LOG

#define XT_OSF_LOGLEVEL_ALL		NF_OSF_LOGLEVEL_ALL
#define XT_OSF_LOGLEVEL_FIRST		NF_OSF_LOGLEVEL_FIRST
#define XT_OSF_LOGLEVEL_ALL_KNOWN	NF_OSF_LOGLEVEL_ALL_KNOWN

#define XT_OSF_TTL_TRUE		NF_OSF_TTL_TRUE
#define XT_OSF_TTL_NOCHECK	NF_OSF_TTL_NOCHECK
#define XT_OSF_TTL_LESS		NF_OSF_TTL_LESS

#define xt_osf_wc		nf_osf_wc
#define xt_osf_opt		nf_osf_opt
#define xt_osf_info		nf_osf_info
#define xt_osf_user_finger	nf_osf_user_finger
#define xt_osf_finger		nf_osf_finger
#define xt_osf_nlmsg		nf_osf_nlmsg

#define xt_osf_window_size_options	nf_osf_window_size_options
#define xt_osf_attr_type	nf_osf_attr_type
#define xt_osf_msg_types	nf_osf_msg_types

#endif				/* _XT_OSF_H */
