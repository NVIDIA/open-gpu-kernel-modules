/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_LINUX_SOCKET_H
#define _UAPI_LINUX_SOCKET_H

/*
 * Desired design of maximum size and alignment (see RFC2553)
 */
#define _K_SS_MAXSIZE	128	/* Implementation specific max size */

typedef unsigned short __kernel_sa_family_t;

/*
 * The definition uses anonymous union and struct in order to control the
 * default alignment.
 */
struct __kernel_sockaddr_storage {
	union {
		struct {
			__kernel_sa_family_t	ss_family; /* address family */
			/* Following field(s) are implementation specific */
			char __data[_K_SS_MAXSIZE - sizeof(unsigned short)];
				/* space to achieve desired size, */
				/* _SS_MAXSIZE value minus size of ss_family */
		};
		void *__align; /* implementation specific desired alignment */
	};
};

#endif /* _UAPI_LINUX_SOCKET_H */
