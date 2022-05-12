/* SPDX-License-Identifier: GPL-2.0-only */
/******************************************************************************
*******************************************************************************
**
**  Copyright (C) Sistina Software, Inc.  1997-2003  All rights reserved.
**  Copyright (C) 2004-2009 Red Hat, Inc.  All rights reserved.
**
**
*******************************************************************************
******************************************************************************/

#ifndef __LOWCOMMS_DOT_H__
#define __LOWCOMMS_DOT_H__

#define LOWCOMMS_MAX_TX_BUFFER_LEN	4096

/* switch to check if dlm is running */
extern int dlm_allow_conn;

int dlm_lowcomms_start(void);
void dlm_lowcomms_shutdown(void);
void dlm_lowcomms_stop(void);
void dlm_lowcomms_exit(void);
int dlm_lowcomms_close(int nodeid);
void *dlm_lowcomms_get_buffer(int nodeid, int len, gfp_t allocation, char **ppc);
void dlm_lowcomms_commit_buffer(void *mh);
int dlm_lowcomms_connect_node(int nodeid);
int dlm_lowcomms_nodes_set_mark(int nodeid, unsigned int mark);
int dlm_lowcomms_addr(int nodeid, struct sockaddr_storage *addr, int len);

#endif				/* __LOWCOMMS_DOT_H__ */

