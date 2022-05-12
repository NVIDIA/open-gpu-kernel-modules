// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2002 - 2007 Jeff Dike (jdike@{addtoit,linux.intel}.com)
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/falloc.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/eventfd.h>
#include <poll.h>
#include <os.h>

static void copy_stat(struct uml_stat *dst, const struct stat64 *src)
{
	*dst = ((struct uml_stat) {
		.ust_dev     = src->st_dev,     /* device */
		.ust_ino     = src->st_ino,     /* inode */
		.ust_mode    = src->st_mode,    /* protection */
		.ust_nlink   = src->st_nlink,   /* number of hard links */
		.ust_uid     = src->st_uid,     /* user ID of owner */
		.ust_gid     = src->st_gid,     /* group ID of owner */
		.ust_size    = src->st_size,    /* total size, in bytes */
		.ust_blksize = src->st_blksize, /* blocksize for filesys I/O */
		.ust_blocks  = src->st_blocks,  /* number of blocks allocated */
		.ust_atime   = src->st_atime,   /* time of last access */
		.ust_mtime   = src->st_mtime,   /* time of last modification */
		.ust_ctime   = src->st_ctime,   /* time of last change */
	});
}

int os_stat_fd(const int fd, struct uml_stat *ubuf)
{
	struct stat64 sbuf;
	int err;

	CATCH_EINTR(err = fstat64(fd, &sbuf));
	if (err < 0)
		return -errno;

	if (ubuf != NULL)
		copy_stat(ubuf, &sbuf);
	return err;
}

int os_stat_file(const char *file_name, struct uml_stat *ubuf)
{
	struct stat64 sbuf;
	int err;

	CATCH_EINTR(err = stat64(file_name, &sbuf));
	if (err < 0)
		return -errno;

	if (ubuf != NULL)
		copy_stat(ubuf, &sbuf);
	return err;
}

int os_access(const char *file, int mode)
{
	int amode, err;

	amode = (mode & OS_ACC_R_OK ? R_OK : 0) |
		(mode & OS_ACC_W_OK ? W_OK : 0) |
		(mode & OS_ACC_X_OK ? X_OK : 0) |
		(mode & OS_ACC_F_OK ? F_OK : 0);

	err = access(file, amode);
	if (err < 0)
		return -errno;

	return 0;
}

/* FIXME? required only by hostaudio (because it passes ioctls verbatim) */
int os_ioctl_generic(int fd, unsigned int cmd, unsigned long arg)
{
	int err;

	err = ioctl(fd, cmd, arg);
	if (err < 0)
		return -errno;

	return err;
}

/* FIXME: ensure namebuf in os_get_if_name is big enough */
int os_get_ifname(int fd, char* namebuf)
{
	if (ioctl(fd, SIOCGIFNAME, namebuf) < 0)
		return -errno;

	return 0;
}

int os_set_slip(int fd)
{
	int disc, sencap;

	disc = N_SLIP;
	if (ioctl(fd, TIOCSETD, &disc) < 0)
		return -errno;

	sencap = 0;
	if (ioctl(fd, SIOCSIFENCAP, &sencap) < 0)
		return -errno;

	return 0;
}

int os_mode_fd(int fd, int mode)
{
	int err;

	CATCH_EINTR(err = fchmod(fd, mode));
	if (err < 0)
		return -errno;

	return 0;
}

int os_file_type(char *file)
{
	struct uml_stat buf;
	int err;

	err = os_stat_file(file, &buf);
	if (err < 0)
		return err;

	if (S_ISDIR(buf.ust_mode))
		return OS_TYPE_DIR;
	else if (S_ISLNK(buf.ust_mode))
		return OS_TYPE_SYMLINK;
	else if (S_ISCHR(buf.ust_mode))
		return OS_TYPE_CHARDEV;
	else if (S_ISBLK(buf.ust_mode))
		return OS_TYPE_BLOCKDEV;
	else if (S_ISFIFO(buf.ust_mode))
		return OS_TYPE_FIFO;
	else if (S_ISSOCK(buf.ust_mode))
		return OS_TYPE_SOCK;
	else return OS_TYPE_FILE;
}

int os_file_mode(const char *file, struct openflags *mode_out)
{
	int err;

	*mode_out = OPENFLAGS();

	err = access(file, W_OK);
	if (err && (errno != EACCES))
		return -errno;
	else if (!err)
		*mode_out = of_write(*mode_out);

	err = access(file, R_OK);
	if (err && (errno != EACCES))
		return -errno;
	else if (!err)
		*mode_out = of_read(*mode_out);

	return err;
}

int os_open_file(const char *file, struct openflags flags, int mode)
{
	int fd, err, f = 0;

	if (flags.r && flags.w)
		f = O_RDWR;
	else if (flags.r)
		f = O_RDONLY;
	else if (flags.w)
		f = O_WRONLY;
	else f = 0;

	if (flags.s)
		f |= O_SYNC;
	if (flags.c)
		f |= O_CREAT;
	if (flags.t)
		f |= O_TRUNC;
	if (flags.e)
		f |= O_EXCL;
	if (flags.a)
		f |= O_APPEND;

	fd = open64(file, f, mode);
	if (fd < 0)
		return -errno;

	if (flags.cl && fcntl(fd, F_SETFD, 1)) {
		err = -errno;
		close(fd);
		return err;
	}

	return fd;
}

int os_connect_socket(const char *name)
{
	struct sockaddr_un sock;
	int fd, err;

	sock.sun_family = AF_UNIX;
	snprintf(sock.sun_path, sizeof(sock.sun_path), "%s", name);

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		err = -errno;
		goto out;
	}

	err = connect(fd, (struct sockaddr *) &sock, sizeof(sock));
	if (err) {
		err = -errno;
		goto out_close;
	}

	return fd;

out_close:
	close(fd);
out:
	return err;
}

void os_close_file(int fd)
{
	close(fd);
}
int os_fsync_file(int fd)
{
	if (fsync(fd) < 0)
	    return -errno;
	return 0;
}

int os_seek_file(int fd, unsigned long long offset)
{
	unsigned long long actual;

	actual = lseek64(fd, offset, SEEK_SET);
	if (actual != offset)
		return -errno;
	return 0;
}

int os_read_file(int fd, void *buf, int len)
{
	int n = read(fd, buf, len);

	if (n < 0)
		return -errno;
	return n;
}

int os_pread_file(int fd, void *buf, int len, unsigned long long offset)
{
	int n = pread(fd, buf, len, offset);

	if (n < 0)
		return -errno;
	return n;
}

int os_write_file(int fd, const void *buf, int len)
{
	int n = write(fd, (void *) buf, len);

	if (n < 0)
		return -errno;
	return n;
}

int os_sync_file(int fd)
{
	int n = fdatasync(fd);

	if (n < 0)
		return -errno;
	return n;
}

int os_pwrite_file(int fd, const void *buf, int len, unsigned long long offset)
{
	int n = pwrite(fd, (void *) buf, len, offset);

	if (n < 0)
		return -errno;
	return n;
}


int os_file_size(const char *file, unsigned long long *size_out)
{
	struct uml_stat buf;
	int err;

	err = os_stat_file(file, &buf);
	if (err < 0) {
		printk(UM_KERN_ERR "Couldn't stat \"%s\" : err = %d\n", file,
		       -err);
		return err;
	}

	if (S_ISBLK(buf.ust_mode)) {
		int fd;
		long blocks;

		fd = open(file, O_RDONLY, 0);
		if (fd < 0) {
			err = -errno;
			printk(UM_KERN_ERR "Couldn't open \"%s\", "
			       "errno = %d\n", file, errno);
			return err;
		}
		if (ioctl(fd, BLKGETSIZE, &blocks) < 0) {
			err = -errno;
			printk(UM_KERN_ERR "Couldn't get the block size of "
			       "\"%s\", errno = %d\n", file, errno);
			close(fd);
			return err;
		}
		*size_out = ((long long) blocks) * 512;
		close(fd);
	}
	else *size_out = buf.ust_size;

	return 0;
}

int os_file_modtime(const char *file, long long *modtime)
{
	struct uml_stat buf;
	int err;

	err = os_stat_file(file, &buf);
	if (err < 0) {
		printk(UM_KERN_ERR "Couldn't stat \"%s\" : err = %d\n", file,
		       -err);
		return err;
	}

	*modtime = buf.ust_mtime;
	return 0;
}

int os_set_exec_close(int fd)
{
	int err;

	CATCH_EINTR(err = fcntl(fd, F_SETFD, FD_CLOEXEC));

	if (err < 0)
		return -errno;
	return err;
}

int os_pipe(int *fds, int stream, int close_on_exec)
{
	int err, type = stream ? SOCK_STREAM : SOCK_DGRAM;

	err = socketpair(AF_UNIX, type, 0, fds);
	if (err < 0)
		return -errno;

	if (!close_on_exec)
		return 0;

	err = os_set_exec_close(fds[0]);
	if (err < 0)
		goto error;

	err = os_set_exec_close(fds[1]);
	if (err < 0)
		goto error;

	return 0;

 error:
	printk(UM_KERN_ERR "os_pipe : Setting FD_CLOEXEC failed, err = %d\n",
	       -err);
	close(fds[1]);
	close(fds[0]);
	return err;
}

int os_set_fd_async(int fd)
{
	int err, flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0)
		return -errno;

	flags |= O_ASYNC | O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0) {
		err = -errno;
		printk(UM_KERN_ERR "os_set_fd_async : failed to set O_ASYNC "
		       "and O_NONBLOCK on fd # %d, errno = %d\n", fd, errno);
		return err;
	}

	if ((fcntl(fd, F_SETSIG, SIGIO) < 0) ||
	    (fcntl(fd, F_SETOWN, os_getpid()) < 0)) {
		err = -errno;
		printk(UM_KERN_ERR "os_set_fd_async : Failed to fcntl F_SETOWN "
		       "(or F_SETSIG) fd %d, errno = %d\n", fd, errno);
		return err;
	}

	return 0;
}

int os_clear_fd_async(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0)
		return -errno;

	flags &= ~(O_ASYNC | O_NONBLOCK);
	if (fcntl(fd, F_SETFL, flags) < 0)
		return -errno;
	return 0;
}

int os_set_fd_block(int fd, int blocking)
{
	int flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0)
		return -errno;

	if (blocking)
		flags &= ~O_NONBLOCK;
	else
		flags |= O_NONBLOCK;

	if (fcntl(fd, F_SETFL, flags) < 0)
		return -errno;

	return 0;
}

int os_accept_connection(int fd)
{
	int new;

	new = accept(fd, NULL, 0);
	if (new < 0)
		return -errno;
	return new;
}

#ifndef SHUT_RD
#define SHUT_RD 0
#endif

#ifndef SHUT_WR
#define SHUT_WR 1
#endif

#ifndef SHUT_RDWR
#define SHUT_RDWR 2
#endif

int os_shutdown_socket(int fd, int r, int w)
{
	int what, err;

	if (r && w)
		what = SHUT_RDWR;
	else if (r)
		what = SHUT_RD;
	else if (w)
		what = SHUT_WR;
	else
		return -EINVAL;

	err = shutdown(fd, what);
	if (err < 0)
		return -errno;
	return 0;
}

int os_rcv_fd(int fd, int *helper_pid_out)
{
	int new, n;
	char buf[CMSG_SPACE(sizeof(new))];
	struct msghdr msg;
	struct cmsghdr *cmsg;
	struct iovec iov;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	iov = ((struct iovec) { .iov_base  = helper_pid_out,
				.iov_len   = sizeof(*helper_pid_out) });
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = buf;
	msg.msg_controllen = sizeof(buf);
	msg.msg_flags = 0;

	n = recvmsg(fd, &msg, 0);
	if (n < 0)
		return -errno;
	else if (n != iov.iov_len)
		*helper_pid_out = -1;

	cmsg = CMSG_FIRSTHDR(&msg);
	if (cmsg == NULL) {
		printk(UM_KERN_ERR "rcv_fd didn't receive anything, "
		       "error = %d\n", errno);
		return -1;
	}
	if ((cmsg->cmsg_level != SOL_SOCKET) ||
	    (cmsg->cmsg_type != SCM_RIGHTS)) {
		printk(UM_KERN_ERR "rcv_fd didn't receive a descriptor\n");
		return -1;
	}

	new = ((int *) CMSG_DATA(cmsg))[0];
	return new;
}

int os_create_unix_socket(const char *file, int len, int close_on_exec)
{
	struct sockaddr_un addr;
	int sock, err;

	sock = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (sock < 0)
		return -errno;

	if (close_on_exec) {
		err = os_set_exec_close(sock);
		if (err < 0)
			printk(UM_KERN_ERR "create_unix_socket : "
			       "close_on_exec failed, err = %d", -err);
	}

	addr.sun_family = AF_UNIX;

	snprintf(addr.sun_path, len, "%s", file);

	err = bind(sock, (struct sockaddr *) &addr, sizeof(addr));
	if (err < 0)
		return -errno;

	return sock;
}

void os_flush_stdout(void)
{
	fflush(stdout);
}

int os_lock_file(int fd, int excl)
{
	int type = excl ? F_WRLCK : F_RDLCK;
	struct flock lock = ((struct flock) { .l_type	= type,
					      .l_whence	= SEEK_SET,
					      .l_start	= 0,
					      .l_len	= 0 } );
	int err, save;

	err = fcntl(fd, F_SETLK, &lock);
	if (!err)
		goto out;

	save = -errno;
	err = fcntl(fd, F_GETLK, &lock);
	if (err) {
		err = -errno;
		goto out;
	}

	printk(UM_KERN_ERR "F_SETLK failed, file already locked by pid %d\n",
	       lock.l_pid);
	err = save;
 out:
	return err;
}

unsigned os_major(unsigned long long dev)
{
	return major(dev);
}

unsigned os_minor(unsigned long long dev)
{
	return minor(dev);
}

unsigned long long os_makedev(unsigned major, unsigned minor)
{
	return makedev(major, minor);
}

int os_falloc_punch(int fd, unsigned long long offset, int len)
{
	int n = fallocate(fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, offset, len);

	if (n < 0)
		return -errno;
	return n;
}

int os_eventfd(unsigned int initval, int flags)
{
	int fd = eventfd(initval, flags);

	if (fd < 0)
		return -errno;
	return fd;
}

int os_sendmsg_fds(int fd, const void *buf, unsigned int len, const int *fds,
		   unsigned int fds_num)
{
	struct iovec iov = {
		.iov_base = (void *) buf,
		.iov_len = len,
	};
	union {
		char control[CMSG_SPACE(sizeof(*fds) * OS_SENDMSG_MAX_FDS)];
		struct cmsghdr align;
	} u;
	unsigned int fds_size = sizeof(*fds) * fds_num;
	struct msghdr msg = {
		.msg_iov = &iov,
		.msg_iovlen = 1,
		.msg_control = u.control,
		.msg_controllen = CMSG_SPACE(fds_size),
	};
	struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
	int err;

	if (fds_num > OS_SENDMSG_MAX_FDS)
		return -EINVAL;
	memset(u.control, 0, sizeof(u.control));
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(fds_size);
	memcpy(CMSG_DATA(cmsg), fds, fds_size);
	err = sendmsg(fd, &msg, 0);

	if (err < 0)
		return -errno;
	return err;
}

int os_poll(unsigned int n, const int *fds)
{
	/* currently need 2 FDs at most so avoid dynamic allocation */
	struct pollfd pollfds[2] = {};
	unsigned int i;
	int ret;

	if (n > ARRAY_SIZE(pollfds))
		return -EINVAL;

	for (i = 0; i < n; i++) {
		pollfds[i].fd = fds[i];
		pollfds[i].events = POLLIN;
	}

	ret = poll(pollfds, n, -1);
	if (ret < 0)
		return -errno;

	/* Return the index of the available FD */
	for (i = 0; i < n; i++) {
		if (pollfds[i].revents)
			return i;
	}

	return -EIO;
}
