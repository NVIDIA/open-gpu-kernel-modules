// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2001 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 */

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <net_user.h>
#include <os.h>
#include <um_malloc.h>

int tap_open_common(void *dev, char *gate_addr)
{
	int tap_addr[4];

	if (gate_addr == NULL)
		return 0;
	if (sscanf(gate_addr, "%d.%d.%d.%d", &tap_addr[0],
		  &tap_addr[1], &tap_addr[2], &tap_addr[3]) != 4) {
		printk(UM_KERN_ERR "Invalid tap IP address - '%s'\n",
		       gate_addr);
		return -EINVAL;
	}
	return 0;
}

void tap_check_ips(char *gate_addr, unsigned char *eth_addr)
{
	int tap_addr[4];

	if ((gate_addr != NULL) &&
	    (sscanf(gate_addr, "%d.%d.%d.%d", &tap_addr[0],
		    &tap_addr[1], &tap_addr[2], &tap_addr[3]) == 4) &&
	    (eth_addr[0] == tap_addr[0]) &&
	    (eth_addr[1] == tap_addr[1]) &&
	    (eth_addr[2] == tap_addr[2]) &&
	    (eth_addr[3] == tap_addr[3])) {
		printk(UM_KERN_ERR "The tap IP address and the UML eth IP "
		       "address must be different\n");
	}
}

/* Do reliable error handling as this fails frequently enough. */
void read_output(int fd, char *output, int len)
{
	int remain, ret, expected;
	char c;
	char *str;

	if (output == NULL) {
		output = &c;
		len = sizeof(c);
	}

	*output = '\0';
	ret = read(fd, &remain, sizeof(remain));

	if (ret != sizeof(remain)) {
		if (ret < 0)
			ret = -errno;
		expected = sizeof(remain);
		str = "length";
		goto err;
	}

	while (remain != 0) {
		expected = (remain < len) ? remain : len;
		ret = read(fd, output, expected);
		if (ret != expected) {
			if (ret < 0)
				ret = -errno;
			str = "data";
			goto err;
		}
		remain -= ret;
	}

	return;

err:
	if (ret < 0)
		printk(UM_KERN_ERR "read_output - read of %s failed, "
		       "errno = %d\n", str, -ret);
	else
		printk(UM_KERN_ERR "read_output - read of %s failed, read only "
		       "%d of %d bytes\n", str, ret, expected);
}

int net_read(int fd, void *buf, int len)
{
	int n;

	n = read(fd,  buf,  len);

	if ((n < 0) && (errno == EAGAIN))
		return 0;
	else if (n == 0)
		return -ENOTCONN;
	return n;
}

int net_recvfrom(int fd, void *buf, int len)
{
	int n;

	CATCH_EINTR(n = recvfrom(fd,  buf,  len, 0, NULL, NULL));
	if (n < 0) {
		if (errno == EAGAIN)
			return 0;
		return -errno;
	}
	else if (n == 0)
		return -ENOTCONN;
	return n;
}

int net_write(int fd, void *buf, int len)
{
	int n;

	n = write(fd, buf, len);

	if ((n < 0) && (errno == EAGAIN))
		return 0;
	else if (n == 0)
		return -ENOTCONN;
	return n;
}

int net_send(int fd, void *buf, int len)
{
	int n;

	CATCH_EINTR(n = send(fd, buf, len, 0));
	if (n < 0) {
		if (errno == EAGAIN)
			return 0;
		return -errno;
	}
	else if (n == 0)
		return -ENOTCONN;
	return n;
}

int net_sendto(int fd, void *buf, int len, void *to, int sock_len)
{
	int n;

	CATCH_EINTR(n = sendto(fd, buf, len, 0, (struct sockaddr *) to,
			       sock_len));
	if (n < 0) {
		if (errno == EAGAIN)
			return 0;
		return -errno;
	}
	else if (n == 0)
		return -ENOTCONN;
	return n;
}

struct change_pre_exec_data {
	int close_me;
	int stdout_fd;
};

static void change_pre_exec(void *arg)
{
	struct change_pre_exec_data *data = arg;

	close(data->close_me);
	dup2(data->stdout_fd, 1);
}

static int change_tramp(char **argv, char *output, int output_len)
{
	int pid, fds[2], err;
	struct change_pre_exec_data pe_data;

	err = os_pipe(fds, 1, 0);
	if (err < 0) {
		printk(UM_KERN_ERR "change_tramp - pipe failed, err = %d\n",
		       -err);
		return err;
	}
	pe_data.close_me = fds[0];
	pe_data.stdout_fd = fds[1];
	pid = run_helper(change_pre_exec, &pe_data, argv);

	if (pid > 0)	/* Avoid hang as we won't get data in failure case. */
		read_output(fds[0], output, output_len);

	close(fds[0]);
	close(fds[1]);

	if (pid > 0)
		helper_wait(pid);
	return pid;
}

static void change(char *dev, char *what, unsigned char *addr,
		   unsigned char *netmask)
{
	char addr_buf[sizeof("255.255.255.255\0")];
	char netmask_buf[sizeof("255.255.255.255\0")];
	char version[sizeof("nnnnn\0")];
	char *argv[] = { "uml_net", version, what, dev, addr_buf,
			 netmask_buf, NULL };
	char *output;
	int output_len, pid;

	sprintf(version, "%d", UML_NET_VERSION);
	sprintf(addr_buf, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
	sprintf(netmask_buf, "%d.%d.%d.%d", netmask[0], netmask[1],
		netmask[2], netmask[3]);

	output_len = UM_KERN_PAGE_SIZE;
	output = uml_kmalloc(output_len, UM_GFP_KERNEL);
	if (output == NULL)
		printk(UM_KERN_ERR "change : failed to allocate output "
		       "buffer\n");

	pid = change_tramp(argv, output, output_len);
	if (pid < 0) {
		kfree(output);
		return;
	}

	if (output != NULL) {
		printk("%s", output);
		kfree(output);
	}
}

void open_addr(unsigned char *addr, unsigned char *netmask, void *arg)
{
	change(arg, "add", addr, netmask);
}

void close_addr(unsigned char *addr, unsigned char *netmask, void *arg)
{
	change(arg, "del", addr, netmask);
}

char *split_if_spec(char *str, ...)
{
	char **arg, *end, *ret = NULL;
	va_list ap;

	va_start(ap, str);
	while ((arg = va_arg(ap, char **)) != NULL) {
		if (*str == '\0')
			goto out;
		end = strchr(str, ',');
		if (end != str)
			*arg = str;
		if (end == NULL)
			goto out;
		*end++ = '\0';
		str = end;
	}
	ret = str;
out:
	va_end(ap);
	return ret;
}
