// SPDX-License-Identifier: GPL-2.0
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>

#include "fs.h"
#include "debug-internal.h"

#define _STR(x) #x
#define STR(x) _STR(x)

#ifndef SYSFS_MAGIC
#define SYSFS_MAGIC            0x62656572
#endif

#ifndef PROC_SUPER_MAGIC
#define PROC_SUPER_MAGIC       0x9fa0
#endif

#ifndef DEBUGFS_MAGIC
#define DEBUGFS_MAGIC          0x64626720
#endif

#ifndef TRACEFS_MAGIC
#define TRACEFS_MAGIC          0x74726163
#endif

#ifndef HUGETLBFS_MAGIC
#define HUGETLBFS_MAGIC        0x958458f6
#endif

#ifndef BPF_FS_MAGIC
#define BPF_FS_MAGIC           0xcafe4a11
#endif

static const char * const sysfs__fs_known_mountpoints[] = {
	"/sys",
	0,
};

static const char * const procfs__known_mountpoints[] = {
	"/proc",
	0,
};

#ifndef DEBUGFS_DEFAULT_PATH
#define DEBUGFS_DEFAULT_PATH "/sys/kernel/debug"
#endif

static const char * const debugfs__known_mountpoints[] = {
	DEBUGFS_DEFAULT_PATH,
	"/debug",
	0,
};


#ifndef TRACEFS_DEFAULT_PATH
#define TRACEFS_DEFAULT_PATH "/sys/kernel/tracing"
#endif

static const char * const tracefs__known_mountpoints[] = {
	TRACEFS_DEFAULT_PATH,
	"/sys/kernel/debug/tracing",
	"/tracing",
	"/trace",
	0,
};

static const char * const hugetlbfs__known_mountpoints[] = {
	0,
};

static const char * const bpf_fs__known_mountpoints[] = {
	"/sys/fs/bpf",
	0,
};

struct fs {
	const char		*name;
	const char * const	*mounts;
	char			 path[PATH_MAX];
	bool			 found;
	bool			 checked;
	long			 magic;
};

enum {
	FS__SYSFS   = 0,
	FS__PROCFS  = 1,
	FS__DEBUGFS = 2,
	FS__TRACEFS = 3,
	FS__HUGETLBFS = 4,
	FS__BPF_FS = 5,
};

#ifndef TRACEFS_MAGIC
#define TRACEFS_MAGIC 0x74726163
#endif

static struct fs fs__entries[] = {
	[FS__SYSFS] = {
		.name	= "sysfs",
		.mounts	= sysfs__fs_known_mountpoints,
		.magic	= SYSFS_MAGIC,
		.checked = false,
	},
	[FS__PROCFS] = {
		.name	= "proc",
		.mounts	= procfs__known_mountpoints,
		.magic	= PROC_SUPER_MAGIC,
		.checked = false,
	},
	[FS__DEBUGFS] = {
		.name	= "debugfs",
		.mounts	= debugfs__known_mountpoints,
		.magic	= DEBUGFS_MAGIC,
		.checked = false,
	},
	[FS__TRACEFS] = {
		.name	= "tracefs",
		.mounts	= tracefs__known_mountpoints,
		.magic	= TRACEFS_MAGIC,
		.checked = false,
	},
	[FS__HUGETLBFS] = {
		.name	= "hugetlbfs",
		.mounts = hugetlbfs__known_mountpoints,
		.magic	= HUGETLBFS_MAGIC,
		.checked = false,
	},
	[FS__BPF_FS] = {
		.name	= "bpf",
		.mounts = bpf_fs__known_mountpoints,
		.magic	= BPF_FS_MAGIC,
		.checked = false,
	},
};

static bool fs__read_mounts(struct fs *fs)
{
	bool found = false;
	char type[100];
	FILE *fp;

	fp = fopen("/proc/mounts", "r");
	if (fp == NULL)
		return NULL;

	while (!found &&
	       fscanf(fp, "%*s %" STR(PATH_MAX) "s %99s %*s %*d %*d\n",
		      fs->path, type) == 2) {

		if (strcmp(type, fs->name) == 0)
			found = true;
	}

	fclose(fp);
	fs->checked = true;
	return fs->found = found;
}

static int fs__valid_mount(const char *fs, long magic)
{
	struct statfs st_fs;

	if (statfs(fs, &st_fs) < 0)
		return -ENOENT;
	else if ((long)st_fs.f_type != magic)
		return -ENOENT;

	return 0;
}

static bool fs__check_mounts(struct fs *fs)
{
	const char * const *ptr;

	ptr = fs->mounts;
	while (*ptr) {
		if (fs__valid_mount(*ptr, fs->magic) == 0) {
			fs->found = true;
			strcpy(fs->path, *ptr);
			return true;
		}
		ptr++;
	}

	return false;
}

static void mem_toupper(char *f, size_t len)
{
	while (len) {
		*f = toupper(*f);
		f++;
		len--;
	}
}

/*
 * Check for "NAME_PATH" environment variable to override fs location (for
 * testing). This matches the recommendation in Documentation/admin-guide/sysfs-rules.rst
 * for SYSFS_PATH.
 */
static bool fs__env_override(struct fs *fs)
{
	char *override_path;
	size_t name_len = strlen(fs->name);
	/* name + "_PATH" + '\0' */
	char upper_name[name_len + 5 + 1];

	memcpy(upper_name, fs->name, name_len);
	mem_toupper(upper_name, name_len);
	strcpy(&upper_name[name_len], "_PATH");

	override_path = getenv(upper_name);
	if (!override_path)
		return false;

	fs->found = true;
	fs->checked = true;
	strncpy(fs->path, override_path, sizeof(fs->path) - 1);
	fs->path[sizeof(fs->path) - 1] = '\0';
	return true;
}

static const char *fs__get_mountpoint(struct fs *fs)
{
	if (fs__env_override(fs))
		return fs->path;

	if (fs__check_mounts(fs))
		return fs->path;

	if (fs__read_mounts(fs))
		return fs->path;

	return NULL;
}

static const char *fs__mountpoint(int idx)
{
	struct fs *fs = &fs__entries[idx];

	if (fs->found)
		return (const char *)fs->path;

	/* the mount point was already checked for the mount point
	 * but and did not exist, so return NULL to avoid scanning again.
	 * This makes the found and not found paths cost equivalent
	 * in case of multiple calls.
	 */
	if (fs->checked)
		return NULL;

	return fs__get_mountpoint(fs);
}

static const char *mount_overload(struct fs *fs)
{
	size_t name_len = strlen(fs->name);
	/* "PERF_" + name + "_ENVIRONMENT" + '\0' */
	char upper_name[5 + name_len + 12 + 1];

	snprintf(upper_name, name_len, "PERF_%s_ENVIRONMENT", fs->name);
	mem_toupper(upper_name, name_len);

	return getenv(upper_name) ?: *fs->mounts;
}

static const char *fs__mount(int idx)
{
	struct fs *fs = &fs__entries[idx];
	const char *mountpoint;

	if (fs__mountpoint(idx))
		return (const char *)fs->path;

	mountpoint = mount_overload(fs);

	if (mount(NULL, mountpoint, fs->name, 0, NULL) < 0)
		return NULL;

	return fs__check_mounts(fs) ? fs->path : NULL;
}

#define FS(name, idx)				\
const char *name##__mountpoint(void)		\
{						\
	return fs__mountpoint(idx);		\
}						\
						\
const char *name##__mount(void)			\
{						\
	return fs__mount(idx);			\
}						\
						\
bool name##__configured(void)			\
{						\
	return name##__mountpoint() != NULL;	\
}

FS(sysfs,   FS__SYSFS);
FS(procfs,  FS__PROCFS);
FS(debugfs, FS__DEBUGFS);
FS(tracefs, FS__TRACEFS);
FS(hugetlbfs, FS__HUGETLBFS);
FS(bpf_fs, FS__BPF_FS);

int filename__read_int(const char *filename, int *value)
{
	char line[64];
	int fd = open(filename, O_RDONLY), err = -1;

	if (fd < 0)
		return -1;

	if (read(fd, line, sizeof(line)) > 0) {
		*value = atoi(line);
		err = 0;
	}

	close(fd);
	return err;
}

static int filename__read_ull_base(const char *filename,
				   unsigned long long *value, int base)
{
	char line[64];
	int fd = open(filename, O_RDONLY), err = -1;

	if (fd < 0)
		return -1;

	if (read(fd, line, sizeof(line)) > 0) {
		*value = strtoull(line, NULL, base);
		if (*value != ULLONG_MAX)
			err = 0;
	}

	close(fd);
	return err;
}

/*
 * Parses @value out of @filename with strtoull.
 * By using 16 for base to treat the number as hex.
 */
int filename__read_xll(const char *filename, unsigned long long *value)
{
	return filename__read_ull_base(filename, value, 16);
}

/*
 * Parses @value out of @filename with strtoull.
 * By using 0 for base, the strtoull detects the
 * base automatically (see man strtoull).
 */
int filename__read_ull(const char *filename, unsigned long long *value)
{
	return filename__read_ull_base(filename, value, 0);
}

#define STRERR_BUFSIZE  128     /* For the buffer size of strerror_r */

int filename__read_str(const char *filename, char **buf, size_t *sizep)
{
	size_t size = 0, alloc_size = 0;
	void *bf = NULL, *nbf;
	int fd, n, err = 0;
	char sbuf[STRERR_BUFSIZE];

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return -errno;

	do {
		if (size == alloc_size) {
			alloc_size += BUFSIZ;
			nbf = realloc(bf, alloc_size);
			if (!nbf) {
				err = -ENOMEM;
				break;
			}

			bf = nbf;
		}

		n = read(fd, bf + size, alloc_size - size);
		if (n < 0) {
			if (size) {
				pr_warn("read failed %d: %s\n", errno,
					strerror_r(errno, sbuf, sizeof(sbuf)));
				err = 0;
			} else
				err = -errno;

			break;
		}

		size += n;
	} while (n > 0);

	if (!err) {
		*sizep = size;
		*buf   = bf;
	} else
		free(bf);

	close(fd);
	return err;
}

int filename__write_int(const char *filename, int value)
{
	int fd = open(filename, O_WRONLY), err = -1;
	char buf[64];

	if (fd < 0)
		return err;

	sprintf(buf, "%d", value);
	if (write(fd, buf, sizeof(buf)) == sizeof(buf))
		err = 0;

	close(fd);
	return err;
}

int procfs__read_str(const char *entry, char **buf, size_t *sizep)
{
	char path[PATH_MAX];
	const char *procfs = procfs__mountpoint();

	if (!procfs)
		return -1;

	snprintf(path, sizeof(path), "%s/%s", procfs, entry);

	return filename__read_str(path, buf, sizep);
}

static int sysfs__read_ull_base(const char *entry,
				unsigned long long *value, int base)
{
	char path[PATH_MAX];
	const char *sysfs = sysfs__mountpoint();

	if (!sysfs)
		return -1;

	snprintf(path, sizeof(path), "%s/%s", sysfs, entry);

	return filename__read_ull_base(path, value, base);
}

int sysfs__read_xll(const char *entry, unsigned long long *value)
{
	return sysfs__read_ull_base(entry, value, 16);
}

int sysfs__read_ull(const char *entry, unsigned long long *value)
{
	return sysfs__read_ull_base(entry, value, 0);
}

int sysfs__read_int(const char *entry, int *value)
{
	char path[PATH_MAX];
	const char *sysfs = sysfs__mountpoint();

	if (!sysfs)
		return -1;

	snprintf(path, sizeof(path), "%s/%s", sysfs, entry);

	return filename__read_int(path, value);
}

int sysfs__read_str(const char *entry, char **buf, size_t *sizep)
{
	char path[PATH_MAX];
	const char *sysfs = sysfs__mountpoint();

	if (!sysfs)
		return -1;

	snprintf(path, sizeof(path), "%s/%s", sysfs, entry);

	return filename__read_str(path, buf, sizep);
}

int sysfs__read_bool(const char *entry, bool *value)
{
	char *buf;
	size_t size;
	int ret;

	ret = sysfs__read_str(entry, &buf, &size);
	if (ret < 0)
		return ret;

	switch (buf[0]) {
	case '1':
	case 'y':
	case 'Y':
		*value = true;
		break;
	case '0':
	case 'n':
	case 'N':
		*value = false;
		break;
	default:
		ret = -1;
	}

	free(buf);

	return ret;
}
int sysctl__read_int(const char *sysctl, int *value)
{
	char path[PATH_MAX];
	const char *procfs = procfs__mountpoint();

	if (!procfs)
		return -1;

	snprintf(path, sizeof(path), "%s/sys/%s", procfs, sysctl);

	return filename__read_int(path, value);
}

int sysfs__write_int(const char *entry, int value)
{
	char path[PATH_MAX];
	const char *sysfs = sysfs__mountpoint();

	if (!sysfs)
		return -1;

	if (snprintf(path, sizeof(path), "%s/%s", sysfs, entry) >= PATH_MAX)
		return -1;

	return filename__write_int(path, value);
}
