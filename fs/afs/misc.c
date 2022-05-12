// SPDX-License-Identifier: GPL-2.0-or-later
/* miscellaneous bits
 *
 * Copyright (C) 2002, 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include "internal.h"
#include "afs_fs.h"
#include "protocol_uae.h"

/*
 * convert an AFS abort code to a Linux error number
 */
int afs_abort_to_error(u32 abort_code)
{
	switch (abort_code) {
		/* Low errno codes inserted into abort namespace */
	case 13:		return -EACCES;
	case 27:		return -EFBIG;
	case 30:		return -EROFS;

		/* VICE "special error" codes; 101 - 111 */
	case VSALVAGE:		return -EIO;
	case VNOVNODE:		return -ENOENT;
	case VNOVOL:		return -ENOMEDIUM;
	case VVOLEXISTS:	return -EEXIST;
	case VNOSERVICE:	return -EIO;
	case VOFFLINE:		return -ENOENT;
	case VONLINE:		return -EEXIST;
	case VDISKFULL:		return -ENOSPC;
	case VOVERQUOTA:	return -EDQUOT;
	case VBUSY:		return -EBUSY;
	case VMOVED:		return -ENXIO;

		/* Volume Location server errors */
	case AFSVL_IDEXIST:		return -EEXIST;
	case AFSVL_IO:			return -EREMOTEIO;
	case AFSVL_NAMEEXIST:		return -EEXIST;
	case AFSVL_CREATEFAIL:		return -EREMOTEIO;
	case AFSVL_NOENT:		return -ENOMEDIUM;
	case AFSVL_EMPTY:		return -ENOMEDIUM;
	case AFSVL_ENTDELETED:		return -ENOMEDIUM;
	case AFSVL_BADNAME:		return -EINVAL;
	case AFSVL_BADINDEX:		return -EINVAL;
	case AFSVL_BADVOLTYPE:		return -EINVAL;
	case AFSVL_BADSERVER:		return -EINVAL;
	case AFSVL_BADPARTITION:	return -EINVAL;
	case AFSVL_REPSFULL:		return -EFBIG;
	case AFSVL_NOREPSERVER:		return -ENOENT;
	case AFSVL_DUPREPSERVER:	return -EEXIST;
	case AFSVL_RWNOTFOUND:		return -ENOENT;
	case AFSVL_BADREFCOUNT:		return -EINVAL;
	case AFSVL_SIZEEXCEEDED:	return -EINVAL;
	case AFSVL_BADENTRY:		return -EINVAL;
	case AFSVL_BADVOLIDBUMP:	return -EINVAL;
	case AFSVL_IDALREADYHASHED:	return -EINVAL;
	case AFSVL_ENTRYLOCKED:		return -EBUSY;
	case AFSVL_BADVOLOPER:		return -EBADRQC;
	case AFSVL_BADRELLOCKTYPE:	return -EINVAL;
	case AFSVL_RERELEASE:		return -EREMOTEIO;
	case AFSVL_BADSERVERFLAG:	return -EINVAL;
	case AFSVL_PERM:		return -EACCES;
	case AFSVL_NOMEM:		return -EREMOTEIO;

		/* Unified AFS error table */
	case UAEPERM:			return -EPERM;
	case UAENOENT:			return -ENOENT;
	case UAEACCES:			return -EACCES;
	case UAEBUSY:			return -EBUSY;
	case UAEEXIST:			return -EEXIST;
	case UAENOTDIR:			return -ENOTDIR;
	case UAEISDIR:			return -EISDIR;
	case UAEFBIG:			return -EFBIG;
	case UAENOSPC:			return -ENOSPC;
	case UAEROFS:			return -EROFS;
	case UAEMLINK:			return -EMLINK;
	case UAEDEADLK:			return -EDEADLK;
	case UAENAMETOOLONG:		return -ENAMETOOLONG;
	case UAENOLCK:			return -ENOLCK;
	case UAENOTEMPTY:		return -ENOTEMPTY;
	case UAELOOP:			return -ELOOP;
	case UAEOVERFLOW:		return -EOVERFLOW;
	case UAENOMEDIUM:		return -ENOMEDIUM;
	case UAEDQUOT:			return -EDQUOT;

		/* RXKAD abort codes; from include/rxrpc/packet.h.  ET "RXK" == 0x1260B00 */
	case RXKADINCONSISTENCY: return -EPROTO;
	case RXKADPACKETSHORT:	return -EPROTO;
	case RXKADLEVELFAIL:	return -EKEYREJECTED;
	case RXKADTICKETLEN:	return -EKEYREJECTED;
	case RXKADOUTOFSEQUENCE: return -EPROTO;
	case RXKADNOAUTH:	return -EKEYREJECTED;
	case RXKADBADKEY:	return -EKEYREJECTED;
	case RXKADBADTICKET:	return -EKEYREJECTED;
	case RXKADUNKNOWNKEY:	return -EKEYREJECTED;
	case RXKADEXPIRED:	return -EKEYEXPIRED;
	case RXKADSEALEDINCON:	return -EKEYREJECTED;
	case RXKADDATALEN:	return -EKEYREJECTED;
	case RXKADILLEGALLEVEL:	return -EKEYREJECTED;

	case RXGEN_OPCODE:	return -ENOTSUPP;

	default:		return -EREMOTEIO;
	}
}

/*
 * Select the error to report from a set of errors.
 */
void afs_prioritise_error(struct afs_error *e, int error, u32 abort_code)
{
	switch (error) {
	case 0:
		return;
	default:
		if (e->error == -ETIMEDOUT ||
		    e->error == -ETIME)
			return;
		fallthrough;
	case -ETIMEDOUT:
	case -ETIME:
		if (e->error == -ENOMEM ||
		    e->error == -ENONET)
			return;
		fallthrough;
	case -ENOMEM:
	case -ENONET:
		if (e->error == -ERFKILL)
			return;
		fallthrough;
	case -ERFKILL:
		if (e->error == -EADDRNOTAVAIL)
			return;
		fallthrough;
	case -EADDRNOTAVAIL:
		if (e->error == -ENETUNREACH)
			return;
		fallthrough;
	case -ENETUNREACH:
		if (e->error == -EHOSTUNREACH)
			return;
		fallthrough;
	case -EHOSTUNREACH:
		if (e->error == -EHOSTDOWN)
			return;
		fallthrough;
	case -EHOSTDOWN:
		if (e->error == -ECONNREFUSED)
			return;
		fallthrough;
	case -ECONNREFUSED:
		if (e->error == -ECONNRESET)
			return;
		fallthrough;
	case -ECONNRESET: /* Responded, but call expired. */
		if (e->responded)
			return;
		e->error = error;
		return;

	case -ECONNABORTED:
		e->responded = true;
		e->error = afs_abort_to_error(abort_code);
		return;
	}
}
