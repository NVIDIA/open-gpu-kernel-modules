// SPDX-License-Identifier: GPL-2.0-only
/*
 * VMware vSockets Driver
 *
 * Copyright (C) 2007-2012 VMware, Inc. All rights reserved.
 */

#include <linux/types.h>
#include <linux/socket.h>
#include <linux/stddef.h>
#include <net/sock.h>
#include <net/vsock_addr.h>

void vsock_addr_init(struct sockaddr_vm *addr, u32 cid, u32 port)
{
	memset(addr, 0, sizeof(*addr));
	addr->svm_family = AF_VSOCK;
	addr->svm_cid = cid;
	addr->svm_port = port;
}
EXPORT_SYMBOL_GPL(vsock_addr_init);

int vsock_addr_validate(const struct sockaddr_vm *addr)
{
	__u8 svm_valid_flags = VMADDR_FLAG_TO_HOST;

	if (!addr)
		return -EFAULT;

	if (addr->svm_family != AF_VSOCK)
		return -EAFNOSUPPORT;

	if (addr->svm_flags & ~svm_valid_flags)
		return -EINVAL;

	return 0;
}
EXPORT_SYMBOL_GPL(vsock_addr_validate);

bool vsock_addr_bound(const struct sockaddr_vm *addr)
{
	return addr->svm_port != VMADDR_PORT_ANY;
}
EXPORT_SYMBOL_GPL(vsock_addr_bound);

void vsock_addr_unbind(struct sockaddr_vm *addr)
{
	vsock_addr_init(addr, VMADDR_CID_ANY, VMADDR_PORT_ANY);
}
EXPORT_SYMBOL_GPL(vsock_addr_unbind);

bool vsock_addr_equals_addr(const struct sockaddr_vm *addr,
			    const struct sockaddr_vm *other)
{
	return addr->svm_cid == other->svm_cid &&
		addr->svm_port == other->svm_port;
}
EXPORT_SYMBOL_GPL(vsock_addr_equals_addr);

int vsock_addr_cast(const struct sockaddr *addr,
		    size_t len, struct sockaddr_vm **out_addr)
{
	if (len < sizeof(**out_addr))
		return -EFAULT;

	*out_addr = (struct sockaddr_vm *)addr;
	return vsock_addr_validate(*out_addr);
}
EXPORT_SYMBOL_GPL(vsock_addr_cast);
