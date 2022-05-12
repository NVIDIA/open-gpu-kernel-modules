/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * sysfile.h
 *
 * Function prototypes
 *
 * Copyright (C) 2002, 2004 Oracle.  All rights reserved.
 */

#ifndef OCFS2_SYSFILE_H
#define OCFS2_SYSFILE_H

struct inode * ocfs2_get_system_file_inode(struct ocfs2_super *osb,
					   int type,
					   u32 slot);

#endif /* OCFS2_SYSFILE_H */
