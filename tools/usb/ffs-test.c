// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * ffs-test.c -- user mode filesystem api for usb composite function
 *
 * Copyright (C) 2010 Samsung Electronics
 *                    Author: Michal Nazarewicz <mina86@mina86.com>
 */

/* $(CROSS_COMPILE)cc -Wall -Wextra -g -o ffs-test ffs-test.c -lpthread */


#define _DEFAULT_SOURCE /* for endian.h */

#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <tools/le_byteshift.h>

#include "../../include/uapi/linux/usb/functionfs.h"


/******************** Little Endian Handling ********************************/

/*
 * cpu_to_le16/32 are used when initializing structures, a context where a
 * function call is not allowed. To solve this, we code cpu_to_le16/32 in a way
 * that allows them to be used when initializing structures.
 */

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define cpu_to_le16(x)  (x)
#define cpu_to_le32(x)  (x)
#else
#define cpu_to_le16(x)  ((((x) >> 8) & 0xffu) | (((x) & 0xffu) << 8))
#define cpu_to_le32(x)  \
	((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >>  8) | \
	(((x) & 0x0000ff00u) <<  8) | (((x) & 0x000000ffu) << 24))
#endif

#define le32_to_cpu(x)  le32toh(x)
#define le16_to_cpu(x)  le16toh(x)

/******************** Messages and Errors ***********************************/

static const char argv0[] = "ffs-test";

static unsigned verbosity = 7;

static void _msg(unsigned level, const char *fmt, ...)
{
	if (level < 2)
		level = 2;
	else if (level > 7)
		level = 7;

	if (level <= verbosity) {
		static const char levels[8][6] = {
			[2] = "crit:",
			[3] = "err: ",
			[4] = "warn:",
			[5] = "note:",
			[6] = "info:",
			[7] = "dbg: "
		};

		int _errno = errno;
		va_list ap;

		fprintf(stderr, "%s: %s ", argv0, levels[level]);
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);

		if (fmt[strlen(fmt) - 1] != '\n') {
			char buffer[128];
			strerror_r(_errno, buffer, sizeof buffer);
			fprintf(stderr, ": (-%d) %s\n", _errno, buffer);
		}

		fflush(stderr);
	}
}

#define die(...)  (_msg(2, __VA_ARGS__), exit(1))
#define err(...)   _msg(3, __VA_ARGS__)
#define warn(...)  _msg(4, __VA_ARGS__)
#define note(...)  _msg(5, __VA_ARGS__)
#define info(...)  _msg(6, __VA_ARGS__)
#define debug(...) _msg(7, __VA_ARGS__)

#define die_on(cond, ...) do { \
	if (cond) \
		die(__VA_ARGS__); \
	} while (0)


/******************** Descriptors and Strings *******************************/

static const struct {
	struct usb_functionfs_descs_head_v2 header;
	__le32 fs_count;
	__le32 hs_count;
	__le32 ss_count;
	struct {
		struct usb_interface_descriptor intf;
		struct usb_endpoint_descriptor_no_audio sink;
		struct usb_endpoint_descriptor_no_audio source;
	} __attribute__((packed)) fs_descs, hs_descs;
	struct {
		struct usb_interface_descriptor intf;
		struct usb_endpoint_descriptor_no_audio sink;
		struct usb_ss_ep_comp_descriptor sink_comp;
		struct usb_endpoint_descriptor_no_audio source;
		struct usb_ss_ep_comp_descriptor source_comp;
	} ss_descs;
} __attribute__((packed)) descriptors = {
	.header = {
		.magic = cpu_to_le32(FUNCTIONFS_DESCRIPTORS_MAGIC_V2),
		.flags = cpu_to_le32(FUNCTIONFS_HAS_FS_DESC |
				     FUNCTIONFS_HAS_HS_DESC |
				     FUNCTIONFS_HAS_SS_DESC),
		.length = cpu_to_le32(sizeof descriptors),
	},
	.fs_count = cpu_to_le32(3),
	.fs_descs = {
		.intf = {
			.bLength = sizeof descriptors.fs_descs.intf,
			.bDescriptorType = USB_DT_INTERFACE,
			.bNumEndpoints = 2,
			.bInterfaceClass = USB_CLASS_VENDOR_SPEC,
			.iInterface = 1,
		},
		.sink = {
			.bLength = sizeof descriptors.fs_descs.sink,
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 1 | USB_DIR_IN,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			/* .wMaxPacketSize = autoconfiguration (kernel) */
		},
		.source = {
			.bLength = sizeof descriptors.fs_descs.source,
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 2 | USB_DIR_OUT,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			/* .wMaxPacketSize = autoconfiguration (kernel) */
		},
	},
	.hs_count = cpu_to_le32(3),
	.hs_descs = {
		.intf = {
			.bLength = sizeof descriptors.fs_descs.intf,
			.bDescriptorType = USB_DT_INTERFACE,
			.bNumEndpoints = 2,
			.bInterfaceClass = USB_CLASS_VENDOR_SPEC,
			.iInterface = 1,
		},
		.sink = {
			.bLength = sizeof descriptors.hs_descs.sink,
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 1 | USB_DIR_IN,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			.wMaxPacketSize = cpu_to_le16(512),
		},
		.source = {
			.bLength = sizeof descriptors.hs_descs.source,
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 2 | USB_DIR_OUT,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			.wMaxPacketSize = cpu_to_le16(512),
			.bInterval = 1, /* NAK every 1 uframe */
		},
	},
	.ss_count = cpu_to_le32(5),
	.ss_descs = {
		.intf = {
			.bLength = sizeof descriptors.fs_descs.intf,
			.bDescriptorType = USB_DT_INTERFACE,
			.bNumEndpoints = 2,
			.bInterfaceClass = USB_CLASS_VENDOR_SPEC,
			.iInterface = 1,
		},
		.sink = {
			.bLength = sizeof descriptors.hs_descs.sink,
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 1 | USB_DIR_IN,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			.wMaxPacketSize = cpu_to_le16(1024),
		},
		.sink_comp = {
			.bLength = USB_DT_SS_EP_COMP_SIZE,
			.bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
			.bMaxBurst = 0,
			.bmAttributes = 0,
			.wBytesPerInterval = 0,
		},
		.source = {
			.bLength = sizeof descriptors.hs_descs.source,
			.bDescriptorType = USB_DT_ENDPOINT,
			.bEndpointAddress = 2 | USB_DIR_OUT,
			.bmAttributes = USB_ENDPOINT_XFER_BULK,
			.wMaxPacketSize = cpu_to_le16(1024),
			.bInterval = 1, /* NAK every 1 uframe */
		},
		.source_comp = {
			.bLength = USB_DT_SS_EP_COMP_SIZE,
			.bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
			.bMaxBurst = 0,
			.bmAttributes = 0,
			.wBytesPerInterval = 0,
		},
	},
};

static size_t descs_to_legacy(void **legacy, const void *descriptors_v2)
{
	const unsigned char *descs_end, *descs_start;
	__u32 length, fs_count = 0, hs_count = 0, count;

	/* Read v2 header */
	{
		const struct {
			const struct usb_functionfs_descs_head_v2 header;
			const __le32 counts[];
		} __attribute__((packed)) *const in = descriptors_v2;
		const __le32 *counts = in->counts;
		__u32 flags;

		if (le32_to_cpu(in->header.magic) !=
		    FUNCTIONFS_DESCRIPTORS_MAGIC_V2)
			return 0;
		length = le32_to_cpu(in->header.length);
		if (length <= sizeof in->header)
			return 0;
		length -= sizeof in->header;
		flags = le32_to_cpu(in->header.flags);
		if (flags & ~(FUNCTIONFS_HAS_FS_DESC | FUNCTIONFS_HAS_HS_DESC |
			      FUNCTIONFS_HAS_SS_DESC))
			return 0;

#define GET_NEXT_COUNT_IF_FLAG(ret, flg) do {		\
			if (!(flags & (flg)))		\
				break;			\
			if (length < 4)			\
				return 0;		\
			ret = le32_to_cpu(*counts);	\
			length -= 4;			\
			++counts;			\
		} while (0)

		GET_NEXT_COUNT_IF_FLAG(fs_count, FUNCTIONFS_HAS_FS_DESC);
		GET_NEXT_COUNT_IF_FLAG(hs_count, FUNCTIONFS_HAS_HS_DESC);
		GET_NEXT_COUNT_IF_FLAG(count, FUNCTIONFS_HAS_SS_DESC);

		count = fs_count + hs_count;
		if (!count)
			return 0;
		descs_start = (const void *)counts;

#undef GET_NEXT_COUNT_IF_FLAG
	}

	/*
	 * Find the end of FS and HS USB descriptors.  SS descriptors
	 * are ignored since legacy format does not support them.
	 */
	descs_end = descs_start;
	do {
		if (length < *descs_end)
			return 0;
		length -= *descs_end;
		descs_end += *descs_end;
	} while (--count);

	/* Allocate legacy descriptors and copy the data. */
	{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		struct {
			struct usb_functionfs_descs_head header;
			__u8 descriptors[];
		} __attribute__((packed)) *out;
#pragma GCC diagnostic pop

		length = sizeof out->header + (descs_end - descs_start);
		out = malloc(length);
		out->header.magic = cpu_to_le32(FUNCTIONFS_DESCRIPTORS_MAGIC);
		out->header.length = cpu_to_le32(length);
		out->header.fs_count = cpu_to_le32(fs_count);
		out->header.hs_count = cpu_to_le32(hs_count);
		memcpy(out->descriptors, descs_start, descs_end - descs_start);
		*legacy = out;
	}

	return length;
}


#define STR_INTERFACE_ "Source/Sink"

static const struct {
	struct usb_functionfs_strings_head header;
	struct {
		__le16 code;
		const char str1[sizeof STR_INTERFACE_];
	} __attribute__((packed)) lang0;
} __attribute__((packed)) strings = {
	.header = {
		.magic = cpu_to_le32(FUNCTIONFS_STRINGS_MAGIC),
		.length = cpu_to_le32(sizeof strings),
		.str_count = cpu_to_le32(1),
		.lang_count = cpu_to_le32(1),
	},
	.lang0 = {
		cpu_to_le16(0x0409), /* en-us */
		STR_INTERFACE_,
	},
};

#define STR_INTERFACE strings.lang0.str1


/******************** Files and Threads Handling ****************************/

struct thread;

static ssize_t read_wrap(struct thread *t, void *buf, size_t nbytes);
static ssize_t write_wrap(struct thread *t, const void *buf, size_t nbytes);
static ssize_t ep0_consume(struct thread *t, const void *buf, size_t nbytes);
static ssize_t fill_in_buf(struct thread *t, void *buf, size_t nbytes);
static ssize_t empty_out_buf(struct thread *t, const void *buf, size_t nbytes);


static struct thread {
	const char *const filename;
	size_t buf_size;

	ssize_t (*in)(struct thread *, void *, size_t);
	const char *const in_name;

	ssize_t (*out)(struct thread *, const void *, size_t);
	const char *const out_name;

	int fd;
	pthread_t id;
	void *buf;
	ssize_t status;
} threads[] = {
	{
		"ep0", 4 * sizeof(struct usb_functionfs_event),
		read_wrap, NULL,
		ep0_consume, "<consume>",
		0, 0, NULL, 0
	},
	{
		"ep1", 8 * 1024,
		fill_in_buf, "<in>",
		write_wrap, NULL,
		0, 0, NULL, 0
	},
	{
		"ep2", 8 * 1024,
		read_wrap, NULL,
		empty_out_buf, "<out>",
		0, 0, NULL, 0
	},
};


static void init_thread(struct thread *t)
{
	t->buf = malloc(t->buf_size);
	die_on(!t->buf, "malloc");

	t->fd = open(t->filename, O_RDWR);
	die_on(t->fd < 0, "%s", t->filename);
}

static void cleanup_thread(void *arg)
{
	struct thread *t = arg;
	int ret, fd;

	fd = t->fd;
	if (t->fd < 0)
		return;
	t->fd = -1;

	/* test the FIFO ioctls (non-ep0 code paths) */
	if (t != threads) {
		ret = ioctl(fd, FUNCTIONFS_FIFO_STATUS);
		if (ret < 0) {
			/* ENODEV reported after disconnect */
			if (errno != ENODEV)
				err("%s: get fifo status", t->filename);
		} else if (ret) {
			warn("%s: unclaimed = %d\n", t->filename, ret);
			if (ioctl(fd, FUNCTIONFS_FIFO_FLUSH) < 0)
				err("%s: fifo flush", t->filename);
		}
	}

	if (close(fd) < 0)
		err("%s: close", t->filename);

	free(t->buf);
	t->buf = NULL;
}

static void *start_thread_helper(void *arg)
{
	const char *name, *op, *in_name, *out_name;
	struct thread *t = arg;
	ssize_t ret;

	info("%s: starts\n", t->filename);
	in_name = t->in_name ? t->in_name : t->filename;
	out_name = t->out_name ? t->out_name : t->filename;

	pthread_cleanup_push(cleanup_thread, arg);

	for (;;) {
		pthread_testcancel();

		ret = t->in(t, t->buf, t->buf_size);
		if (ret > 0) {
			ret = t->out(t, t->buf, ret);
			name = out_name;
			op = "write";
		} else {
			name = in_name;
			op = "read";
		}

		if (ret > 0) {
			/* nop */
		} else if (!ret) {
			debug("%s: %s: EOF", name, op);
			break;
		} else if (errno == EINTR || errno == EAGAIN) {
			debug("%s: %s", name, op);
		} else {
			warn("%s: %s", name, op);
			break;
		}
	}

	pthread_cleanup_pop(1);

	t->status = ret;
	info("%s: ends\n", t->filename);
	return NULL;
}

static void start_thread(struct thread *t)
{
	debug("%s: starting\n", t->filename);

	die_on(pthread_create(&t->id, NULL, start_thread_helper, t) < 0,
	       "pthread_create(%s)", t->filename);
}

static void join_thread(struct thread *t)
{
	int ret = pthread_join(t->id, NULL);

	if (ret < 0)
		err("%s: joining thread", t->filename);
	else
		debug("%s: joined\n", t->filename);
}


static ssize_t read_wrap(struct thread *t, void *buf, size_t nbytes)
{
	return read(t->fd, buf, nbytes);
}

static ssize_t write_wrap(struct thread *t, const void *buf, size_t nbytes)
{
	return write(t->fd, buf, nbytes);
}


/******************** Empty/Fill buffer routines ****************************/

/* 0 -- stream of zeros, 1 -- i % 63, 2 -- pipe */
enum pattern { PAT_ZERO, PAT_SEQ, PAT_PIPE };
static enum pattern pattern;

static ssize_t
fill_in_buf(struct thread *ignore, void *buf, size_t nbytes)
{
	size_t i;
	__u8 *p;

	(void)ignore;

	switch (pattern) {
	case PAT_ZERO:
		memset(buf, 0, nbytes);
		break;

	case PAT_SEQ:
		for (p = buf, i = 0; i < nbytes; ++i, ++p)
			*p = i % 63;
		break;

	case PAT_PIPE:
		return fread(buf, 1, nbytes, stdin);
	}

	return nbytes;
}

static ssize_t
empty_out_buf(struct thread *ignore, const void *buf, size_t nbytes)
{
	const __u8 *p;
	__u8 expected;
	ssize_t ret;
	size_t len;

	(void)ignore;

	switch (pattern) {
	case PAT_ZERO:
		expected = 0;
		for (p = buf, len = 0; len < nbytes; ++p, ++len)
			if (*p)
				goto invalid;
		break;

	case PAT_SEQ:
		for (p = buf, len = 0; len < nbytes; ++p, ++len)
			if (*p != len % 63) {
				expected = len % 63;
				goto invalid;
			}
		break;

	case PAT_PIPE:
		ret = fwrite(buf, nbytes, 1, stdout);
		if (ret > 0)
			fflush(stdout);
		break;

invalid:
		err("bad OUT byte %zd, expected %02x got %02x\n",
		    len, expected, *p);
		for (p = buf, len = 0; len < nbytes; ++p, ++len) {
			if (0 == (len % 32))
				fprintf(stderr, "%4zd:", len);
			fprintf(stderr, " %02x", *p);
			if (31 == (len % 32))
				fprintf(stderr, "\n");
		}
		fflush(stderr);
		errno = EILSEQ;
		return -1;
	}

	return len;
}


/******************** Endpoints routines ************************************/

static void handle_setup(const struct usb_ctrlrequest *setup)
{
	printf("bRequestType = %d\n", setup->bRequestType);
	printf("bRequest     = %d\n", setup->bRequest);
	printf("wValue       = %d\n", le16_to_cpu(setup->wValue));
	printf("wIndex       = %d\n", le16_to_cpu(setup->wIndex));
	printf("wLength      = %d\n", le16_to_cpu(setup->wLength));
}

static ssize_t
ep0_consume(struct thread *ignore, const void *buf, size_t nbytes)
{
	static const char *const names[] = {
		[FUNCTIONFS_BIND] = "BIND",
		[FUNCTIONFS_UNBIND] = "UNBIND",
		[FUNCTIONFS_ENABLE] = "ENABLE",
		[FUNCTIONFS_DISABLE] = "DISABLE",
		[FUNCTIONFS_SETUP] = "SETUP",
		[FUNCTIONFS_SUSPEND] = "SUSPEND",
		[FUNCTIONFS_RESUME] = "RESUME",
	};

	const struct usb_functionfs_event *event = buf;
	size_t n;

	(void)ignore;

	for (n = nbytes / sizeof *event; n; --n, ++event)
		switch (event->type) {
		case FUNCTIONFS_BIND:
		case FUNCTIONFS_UNBIND:
		case FUNCTIONFS_ENABLE:
		case FUNCTIONFS_DISABLE:
		case FUNCTIONFS_SETUP:
		case FUNCTIONFS_SUSPEND:
		case FUNCTIONFS_RESUME:
			printf("Event %s\n", names[event->type]);
			if (event->type == FUNCTIONFS_SETUP)
				handle_setup(&event->u.setup);
			break;

		default:
			printf("Event %03u (unknown)\n", event->type);
		}

	return nbytes;
}

static void ep0_init(struct thread *t, bool legacy_descriptors)
{
	void *legacy;
	ssize_t ret;
	size_t len;

	if (legacy_descriptors) {
		info("%s: writing descriptors\n", t->filename);
		goto legacy;
	}

	info("%s: writing descriptors (in v2 format)\n", t->filename);
	ret = write(t->fd, &descriptors, sizeof descriptors);

	if (ret < 0 && errno == EINVAL) {
		warn("%s: new format rejected, trying legacy\n", t->filename);
legacy:
		len = descs_to_legacy(&legacy, &descriptors);
		if (len) {
			ret = write(t->fd, legacy, len);
			free(legacy);
		}
	}
	die_on(ret < 0, "%s: write: descriptors", t->filename);

	info("%s: writing strings\n", t->filename);
	ret = write(t->fd, &strings, sizeof strings);
	die_on(ret < 0, "%s: write: strings", t->filename);
}


/******************** Main **************************************************/

int main(int argc, char **argv)
{
	bool legacy_descriptors;
	unsigned i;

	legacy_descriptors = argc > 2 && !strcmp(argv[1], "-l");

	init_thread(threads);
	ep0_init(threads, legacy_descriptors);

	for (i = 1; i < sizeof threads / sizeof *threads; ++i)
		init_thread(threads + i);

	for (i = 1; i < sizeof threads / sizeof *threads; ++i)
		start_thread(threads + i);

	start_thread_helper(threads);

	for (i = 1; i < sizeof threads / sizeof *threads; ++i)
		join_thread(threads + i);

	return 0;
}
