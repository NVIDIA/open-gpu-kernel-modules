/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
/*
 * Copyright (c) 2003-2007 Network Appliance, Inc. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the BSD-type
 * license below:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *      Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *      Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *
 *      Neither the name of the Network Appliance, Inc. nor the names of
 *      its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written
 *      permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LINUX_SUNRPC_XPRTRDMA_H
#define _LINUX_SUNRPC_XPRTRDMA_H

/*
 * Constants. Max RPC/NFS header is big enough to account for
 * additional marshaling buffers passed down by Linux client.
 *
 * RDMA header is currently fixed max size, and is big enough for a
 * fully-chunked NFS message (read chunks are the largest). Note only
 * a single chunk type per message is supported currently.
 */
#define RPCRDMA_MIN_SLOT_TABLE	(4U)
#define RPCRDMA_DEF_SLOT_TABLE	(128U)
#define RPCRDMA_MAX_SLOT_TABLE	(16384U)

#define RPCRDMA_MIN_INLINE  (1024)	/* min inline thresh */
#define RPCRDMA_DEF_INLINE  (4096)	/* default inline thresh */
#define RPCRDMA_MAX_INLINE  (65536)	/* max inline thresh */

/* Memory registration strategies, by number.
 * This is part of a kernel / user space API. Do not remove. */
enum rpcrdma_memreg {
	RPCRDMA_BOUNCEBUFFERS = 0,
	RPCRDMA_REGISTER,
	RPCRDMA_MEMWINDOWS,
	RPCRDMA_MEMWINDOWS_ASYNC,
	RPCRDMA_MTHCAFMR,
	RPCRDMA_FRWR,
	RPCRDMA_ALLPHYSICAL,
	RPCRDMA_LAST
};

#endif /* _LINUX_SUNRPC_XPRTRDMA_H */
