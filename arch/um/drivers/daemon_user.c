// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2001 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 * Copyright (C) 2001 Lennert Buytenhek (buytenh@gnu.org) and
 * James Leu (jleu@mindspring.net).
 * Copyright (C) 2001 by various other people who didn't put their name here.
 */

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include "daemon.h"
#include <net_user.h>
#include <os.h>
#include <um_malloc.h>

enum request_type { REQ_NEW_CONTROL };

#define SWITCH_MAGIC 0xfeedface

struct request_v3 {
	uint32_t magic;
	uint32_t version;
	enum request_type type;
	struct sockaddr_un sock;
};

static struct sockaddr_un *new_addr(void *name, int len)
{
	struct sockaddr_un *sun;

	sun = uml_kmalloc(sizeof(struct sockaddr_un), UM_GFP_KERNEL);
	if (sun == NULL) {
		printk(UM_KERN_ERR "new_addr: allocation of sockaddr_un "
		       "failed\n");
		return NULL;
	}
	sun->sun_family = AF_UNIX;
	memcpy(sun->sun_path, name, len);
	return sun;
}

static int connect_to_switch(struct daemon_data *pri)
{
	struct sockaddr_un *ctl_addr = pri->ctl_addr;
	struct sockaddr_un *local_addr = pri->local_addr;
	struct sockaddr_un *sun;
	struct request_v3 req;
	int fd, n, err;

	pri->control = socket(AF_UNIX, SOCK_STREAM, 0);
	if (pri->control < 0) {
		err = -errno;
		printk(UM_KERN_ERR "daemon_open : control socket failed, "
		       "errno = %d\n", -err);
		return err;
	}

	if (connect(pri->control, (struct sockaddr *) ctl_addr,
		   sizeof(*ctl_addr)) < 0) {
		err = -errno;
		printk(UM_KERN_ERR "daemon_open : control connect failed, "
		       "errno = %d\n", -err);
		goto out;
	}

	fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (fd < 0) {
		err = -errno;
		printk(UM_KERN_ERR "daemon_open : data socket failed, "
		       "errno = %d\n", -err);
		goto out;
	}
	if (bind(fd, (struct sockaddr *) local_addr, sizeof(*local_addr)) < 0) {
		err = -errno;
		printk(UM_KERN_ERR "daemon_open : data bind failed, "
		       "errno = %d\n", -err);
		goto out_close;
	}

	sun = uml_kmalloc(sizeof(struct sockaddr_un), UM_GFP_KERNEL);
	if (sun == NULL) {
		printk(UM_KERN_ERR "new_addr: allocation of sockaddr_un "
		       "failed\n");
		err = -ENOMEM;
		goto out_close;
	}

	req.magic = SWITCH_MAGIC;
	req.version = SWITCH_VERSION;
	req.type = REQ_NEW_CONTROL;
	req.sock = *local_addr;
	n = write(pri->control, &req, sizeof(req));
	if (n != sizeof(req)) {
		printk(UM_KERN_ERR "daemon_open : control setup request "
		       "failed, err = %d\n", -errno);
		err = -ENOTCONN;
		goto out_free;
	}

	n = read(pri->control, sun, sizeof(*sun));
	if (n != sizeof(*sun)) {
		printk(UM_KERN_ERR "daemon_open : read of data socket failed, "
		       "err = %d\n", -errno);
		err = -ENOTCONN;
		goto out_free;
	}

	pri->data_addr = sun;
	return fd;

 out_free:
	kfree(sun);
 out_close:
	close(fd);
 out:
	close(pri->control);
	return err;
}

static int daemon_user_init(void *data, void *dev)
{
	struct daemon_data *pri = data;
	struct timeval tv;
	struct {
		char zero;
		int pid;
		int usecs;
	} name;

	if (!strcmp(pri->sock_type, "unix"))
		pri->ctl_addr = new_addr(pri->ctl_sock,
					 strlen(pri->ctl_sock) + 1);
	name.zero = 0;
	name.pid = os_getpid();
	gettimeofday(&tv, NULL);
	name.usecs = tv.tv_usec;
	pri->local_addr = new_addr(&name, sizeof(name));
	pri->dev = dev;
	pri->fd = connect_to_switch(pri);
	if (pri->fd < 0) {
		kfree(pri->local_addr);
		pri->local_addr = NULL;
		return pri->fd;
	}

	return 0;
}

static int daemon_open(void *data)
{
	struct daemon_data *pri = data;
	return pri->fd;
}

static void daemon_remove(void *data)
{
	struct daemon_data *pri = data;

	close(pri->fd);
	pri->fd = -1;
	close(pri->control);
	pri->control = -1;

	kfree(pri->data_addr);
	pri->data_addr = NULL;
	kfree(pri->ctl_addr);
	pri->ctl_addr = NULL;
	kfree(pri->local_addr);
	pri->local_addr = NULL;
}

int daemon_user_write(int fd, void *buf, int len, struct daemon_data *pri)
{
	struct sockaddr_un *data_addr = pri->data_addr;

	return net_sendto(fd, buf, len, data_addr, sizeof(*data_addr));
}

const struct net_user_info daemon_user_info = {
	.init		= daemon_user_init,
	.open		= daemon_open,
	.close	 	= NULL,
	.remove	 	= daemon_remove,
	.add_address	= NULL,
	.delete_address = NULL,
	.mtu		= ETH_MAX_PACKET,
	.max_packet	= ETH_MAX_PACKET + ETH_HEADER_OTHER,
};
