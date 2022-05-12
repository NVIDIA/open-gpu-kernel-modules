// SPDX-License-Identifier: GPL-2.0-or-later
/** -*- linux-c -*- ***********************************************************
 * Linux PPP over X/Ethernet (PPPoX/PPPoE) Sockets
 *
 * PPPoX --- Generic PPP encapsulation socket family
 * PPPoE --- PPP over Ethernet (RFC 2516)
 *
 * Version:	0.5.2
 *
 * Author:	Michal Ostrowski <mostrows@speakeasy.net>
 *
 * 051000 :	Initialization cleanup
 *
 * License:
 */

#include <linux/string.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/compat.h>
#include <linux/errno.h>
#include <linux/netdevice.h>
#include <linux/net.h>
#include <linux/init.h>
#include <linux/if_pppox.h>
#include <linux/ppp_defs.h>
#include <linux/ppp-ioctl.h>
#include <linux/ppp_channel.h>
#include <linux/kmod.h>

#include <net/sock.h>

#include <linux/uaccess.h>

static const struct pppox_proto *pppox_protos[PX_MAX_PROTO + 1];

int register_pppox_proto(int proto_num, const struct pppox_proto *pp)
{
	if (proto_num < 0 || proto_num > PX_MAX_PROTO)
		return -EINVAL;
	if (pppox_protos[proto_num])
		return -EALREADY;
	pppox_protos[proto_num] = pp;
	return 0;
}

void unregister_pppox_proto(int proto_num)
{
	if (proto_num >= 0 && proto_num <= PX_MAX_PROTO)
		pppox_protos[proto_num] = NULL;
}

void pppox_unbind_sock(struct sock *sk)
{
	/* Clear connection to ppp device, if attached. */

	if (sk->sk_state & (PPPOX_BOUND | PPPOX_CONNECTED)) {
		ppp_unregister_channel(&pppox_sk(sk)->chan);
		sk->sk_state = PPPOX_DEAD;
	}
}

EXPORT_SYMBOL(register_pppox_proto);
EXPORT_SYMBOL(unregister_pppox_proto);
EXPORT_SYMBOL(pppox_unbind_sock);

int pppox_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk = sock->sk;
	struct pppox_sock *po = pppox_sk(sk);
	int rc;

	lock_sock(sk);

	switch (cmd) {
	case PPPIOCGCHAN: {
		int index;
		rc = -ENOTCONN;
		if (!(sk->sk_state & PPPOX_CONNECTED))
			break;

		rc = -EINVAL;
		index = ppp_channel_index(&po->chan);
		if (put_user(index , (int __user *) arg))
			break;

		rc = 0;
		sk->sk_state |= PPPOX_BOUND;
		break;
	}
	default:
		rc = pppox_protos[sk->sk_protocol]->ioctl ?
			pppox_protos[sk->sk_protocol]->ioctl(sock, cmd, arg) : -ENOTTY;
	}

	release_sock(sk);
	return rc;
}

EXPORT_SYMBOL(pppox_ioctl);

#ifdef CONFIG_COMPAT
int pppox_compat_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	if (cmd == PPPOEIOCSFWD32)
		cmd = PPPOEIOCSFWD;

	return pppox_ioctl(sock, cmd, (unsigned long)compat_ptr(arg));
}

EXPORT_SYMBOL(pppox_compat_ioctl);
#endif

static int pppox_create(struct net *net, struct socket *sock, int protocol,
			int kern)
{
	int rc = -EPROTOTYPE;

	if (protocol < 0 || protocol > PX_MAX_PROTO)
		goto out;

	rc = -EPROTONOSUPPORT;
	if (!pppox_protos[protocol])
		request_module("net-pf-%d-proto-%d", PF_PPPOX, protocol);
	if (!pppox_protos[protocol] ||
	    !try_module_get(pppox_protos[protocol]->owner))
		goto out;

	rc = pppox_protos[protocol]->create(net, sock, kern);

	module_put(pppox_protos[protocol]->owner);
out:
	return rc;
}

static const struct net_proto_family pppox_proto_family = {
	.family	= PF_PPPOX,
	.create	= pppox_create,
	.owner	= THIS_MODULE,
};

static int __init pppox_init(void)
{
	return sock_register(&pppox_proto_family);
}

static void __exit pppox_exit(void)
{
	sock_unregister(PF_PPPOX);
}

module_init(pppox_init);
module_exit(pppox_exit);

MODULE_AUTHOR("Michal Ostrowski <mostrows@speakeasy.net>");
MODULE_DESCRIPTION("PPP over Ethernet driver (generic socket layer)");
MODULE_LICENSE("GPL");
MODULE_ALIAS_NETPROTO(PF_PPPOX);
