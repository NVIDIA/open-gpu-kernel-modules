// SPDX-License-Identifier: GPL-2.0
/*
 * This file contains all the stubs needed when communicating with lockd.
 * This level of indirection is necessary so we can run nfsd+lockd without
 * requiring the nfs client to be compiled in/loaded, and vice versa.
 *
 * Copyright (C) 1996, Olaf Kirch <okir@monad.swb.de>
 */

#include <linux/file.h>
#include <linux/lockd/bind.h>
#include "nfsd.h"
#include "vfs.h"

#define NFSDDBG_FACILITY		NFSDDBG_LOCKD

#ifdef CONFIG_LOCKD_V4
#define nlm_stale_fh	nlm4_stale_fh
#define nlm_failed	nlm4_failed
#else
#define nlm_stale_fh	nlm_lck_denied_nolocks
#define nlm_failed	nlm_lck_denied_nolocks
#endif
/*
 * Note: we hold the dentry use count while the file is open.
 */
static __be32
nlm_fopen(struct svc_rqst *rqstp, struct nfs_fh *f, struct file **filp)
{
	__be32		nfserr;
	struct svc_fh	fh;

	/* must initialize before using! but maxsize doesn't matter */
	fh_init(&fh,0);
	fh.fh_handle.fh_size = f->size;
	memcpy((char*)&fh.fh_handle.fh_base, f->data, f->size);
	fh.fh_export = NULL;

	nfserr = nfsd_open(rqstp, &fh, S_IFREG, NFSD_MAY_LOCK, filp);
	fh_put(&fh);
 	/* We return nlm error codes as nlm doesn't know
	 * about nfsd, but nfsd does know about nlm..
	 */
	switch (nfserr) {
	case nfs_ok:
		return 0;
	case nfserr_dropit:
		return nlm_drop_reply;
	case nfserr_stale:
		return nlm_stale_fh;
	default:
		return nlm_failed;
	}
}

static void
nlm_fclose(struct file *filp)
{
	fput(filp);
}

static const struct nlmsvc_binding nfsd_nlm_ops = {
	.fopen		= nlm_fopen,		/* open file for locking */
	.fclose		= nlm_fclose,		/* close file */
};

void
nfsd_lockd_init(void)
{
	dprintk("nfsd: initializing lockd\n");
	nlmsvc_ops = &nfsd_nlm_ops;
}

void
nfsd_lockd_shutdown(void)
{
	nlmsvc_ops = NULL;
}
