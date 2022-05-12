/*
   CMTP implementation for Linux Bluetooth stack (BlueZ).
   Copyright (C) 2002-2003 Marcel Holtmann <marcel@holtmann.org>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation;

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) AND AUTHOR(S) BE LIABLE FOR ANY
   CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PATENTS,
   COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS, RELATING TO USE OF THIS
   SOFTWARE IS DISCLAIMED.
*/

#include <linux/export.h>

#include <linux/types.h>
#include <linux/capability.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/poll.h>
#include <linux/fcntl.h>
#include <linux/skbuff.h>
#include <linux/socket.h>
#include <linux/ioctl.h>
#include <linux/file.h>
#include <linux/compat.h>
#include <linux/gfp.h>
#include <linux/uaccess.h>
#include <net/sock.h>

#include <linux/isdn/capilli.h>


#include "cmtp.h"

static struct bt_sock_list cmtp_sk_list = {
	.lock = __RW_LOCK_UNLOCKED(cmtp_sk_list.lock)
};

static int cmtp_sock_release(struct socket *sock)
{
	struct sock *sk = sock->sk;

	BT_DBG("sock %p sk %p", sock, sk);

	if (!sk)
		return 0;

	bt_sock_unlink(&cmtp_sk_list, sk);

	sock_orphan(sk);
	sock_put(sk);

	return 0;
}

static int do_cmtp_sock_ioctl(struct socket *sock, unsigned int cmd, void __user *argp)
{
	struct cmtp_connadd_req ca;
	struct cmtp_conndel_req cd;
	struct cmtp_connlist_req cl;
	struct cmtp_conninfo ci;
	struct socket *nsock;
	int err;

	BT_DBG("cmd %x arg %p", cmd, argp);

	switch (cmd) {
	case CMTPCONNADD:
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;

		if (copy_from_user(&ca, argp, sizeof(ca)))
			return -EFAULT;

		nsock = sockfd_lookup(ca.sock, &err);
		if (!nsock)
			return err;

		if (nsock->sk->sk_state != BT_CONNECTED) {
			sockfd_put(nsock);
			return -EBADFD;
		}

		err = cmtp_add_connection(&ca, nsock);
		if (!err) {
			if (copy_to_user(argp, &ca, sizeof(ca)))
				err = -EFAULT;
		} else
			sockfd_put(nsock);

		return err;

	case CMTPCONNDEL:
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;

		if (copy_from_user(&cd, argp, sizeof(cd)))
			return -EFAULT;

		return cmtp_del_connection(&cd);

	case CMTPGETCONNLIST:
		if (copy_from_user(&cl, argp, sizeof(cl)))
			return -EFAULT;

		if (cl.cnum <= 0)
			return -EINVAL;

		err = cmtp_get_connlist(&cl);
		if (!err && copy_to_user(argp, &cl, sizeof(cl)))
			return -EFAULT;

		return err;

	case CMTPGETCONNINFO:
		if (copy_from_user(&ci, argp, sizeof(ci)))
			return -EFAULT;

		err = cmtp_get_conninfo(&ci);
		if (!err && copy_to_user(argp, &ci, sizeof(ci)))
			return -EFAULT;

		return err;
	}

	return -EINVAL;
}

static int cmtp_sock_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	return do_cmtp_sock_ioctl(sock, cmd, (void __user *)arg);
}

#ifdef CONFIG_COMPAT
static int cmtp_sock_compat_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	void __user *argp = compat_ptr(arg);
	if (cmd == CMTPGETCONNLIST) {
		struct cmtp_connlist_req cl;
		u32 __user *p = argp;
		u32 uci;
		int err;

		if (get_user(cl.cnum, p) || get_user(uci, p + 1))
			return -EFAULT;

		cl.ci = compat_ptr(uci);

		if (cl.cnum <= 0)
			return -EINVAL;

		err = cmtp_get_connlist(&cl);

		if (!err && put_user(cl.cnum, p))
			err = -EFAULT;

		return err;
	}

	return do_cmtp_sock_ioctl(sock, cmd, argp);
}
#endif

static const struct proto_ops cmtp_sock_ops = {
	.family		= PF_BLUETOOTH,
	.owner		= THIS_MODULE,
	.release	= cmtp_sock_release,
	.ioctl		= cmtp_sock_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= cmtp_sock_compat_ioctl,
#endif
	.bind		= sock_no_bind,
	.getname	= sock_no_getname,
	.sendmsg	= sock_no_sendmsg,
	.recvmsg	= sock_no_recvmsg,
	.listen		= sock_no_listen,
	.shutdown	= sock_no_shutdown,
	.connect	= sock_no_connect,
	.socketpair	= sock_no_socketpair,
	.accept		= sock_no_accept,
	.mmap		= sock_no_mmap
};

static struct proto cmtp_proto = {
	.name		= "CMTP",
	.owner		= THIS_MODULE,
	.obj_size	= sizeof(struct bt_sock)
};

static int cmtp_sock_create(struct net *net, struct socket *sock, int protocol,
			    int kern)
{
	struct sock *sk;

	BT_DBG("sock %p", sock);

	if (sock->type != SOCK_RAW)
		return -ESOCKTNOSUPPORT;

	sk = sk_alloc(net, PF_BLUETOOTH, GFP_ATOMIC, &cmtp_proto, kern);
	if (!sk)
		return -ENOMEM;

	sock_init_data(sock, sk);

	sock->ops = &cmtp_sock_ops;

	sock->state = SS_UNCONNECTED;

	sock_reset_flag(sk, SOCK_ZAPPED);

	sk->sk_protocol = protocol;
	sk->sk_state    = BT_OPEN;

	bt_sock_link(&cmtp_sk_list, sk);

	return 0;
}

static const struct net_proto_family cmtp_sock_family_ops = {
	.family	= PF_BLUETOOTH,
	.owner	= THIS_MODULE,
	.create	= cmtp_sock_create
};

int cmtp_init_sockets(void)
{
	int err;

	err = proto_register(&cmtp_proto, 0);
	if (err < 0)
		return err;

	err = bt_sock_register(BTPROTO_CMTP, &cmtp_sock_family_ops);
	if (err < 0) {
		BT_ERR("Can't register CMTP socket");
		goto error;
	}

	err = bt_procfs_init(&init_net, "cmtp", &cmtp_sk_list, NULL);
	if (err < 0) {
		BT_ERR("Failed to create CMTP proc file");
		bt_sock_unregister(BTPROTO_HIDP);
		goto error;
	}

	BT_INFO("CMTP socket layer initialized");

	return 0;

error:
	proto_unregister(&cmtp_proto);
	return err;
}

void cmtp_cleanup_sockets(void)
{
	bt_procfs_cleanup(&init_net, "cmtp");
	bt_sock_unregister(BTPROTO_CMTP);
	proto_unregister(&cmtp_proto);
}
