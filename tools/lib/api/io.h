/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Lightweight buffered reading library.
 *
 * Copyright 2019 Google LLC.
 */
#ifndef __API_IO__
#define __API_IO__

#include <stdlib.h>
#include <unistd.h>

struct io {
	/* File descriptor being read/ */
	int fd;
	/* Size of the read buffer. */
	unsigned int buf_len;
	/* Pointer to storage for buffering read. */
	char *buf;
	/* End of the storage. */
	char *end;
	/* Currently accessed data pointer. */
	char *data;
	/* Set true on when the end of file on read error. */
	bool eof;
};

static inline void io__init(struct io *io, int fd,
			    char *buf, unsigned int buf_len)
{
	io->fd = fd;
	io->buf_len = buf_len;
	io->buf = buf;
	io->end = buf;
	io->data = buf;
	io->eof = false;
}

/* Reads one character from the "io" file with similar semantics to fgetc. */
static inline int io__get_char(struct io *io)
{
	char *ptr = io->data;

	if (io->eof)
		return -1;

	if (ptr == io->end) {
		ssize_t n = read(io->fd, io->buf, io->buf_len);

		if (n <= 0) {
			io->eof = true;
			return -1;
		}
		ptr = &io->buf[0];
		io->end = &io->buf[n];
	}
	io->data = ptr + 1;
	return *ptr;
}

/* Read a hexadecimal value with no 0x prefix into the out argument hex. If the
 * first character isn't hexadecimal returns -2, io->eof returns -1, otherwise
 * returns the character after the hexadecimal value which may be -1 for eof.
 * If the read value is larger than a u64 the high-order bits will be dropped.
 */
static inline int io__get_hex(struct io *io, __u64 *hex)
{
	bool first_read = true;

	*hex = 0;
	while (true) {
		int ch = io__get_char(io);

		if (ch < 0)
			return ch;
		if (ch >= '0' && ch <= '9')
			*hex = (*hex << 4) | (ch - '0');
		else if (ch >= 'a' && ch <= 'f')
			*hex = (*hex << 4) | (ch - 'a' + 10);
		else if (ch >= 'A' && ch <= 'F')
			*hex = (*hex << 4) | (ch - 'A' + 10);
		else if (first_read)
			return -2;
		else
			return ch;
		first_read = false;
	}
}

/* Read a positive decimal value with out argument dec. If the first character
 * isn't a decimal returns -2, io->eof returns -1, otherwise returns the
 * character after the decimal value which may be -1 for eof. If the read value
 * is larger than a u64 the high-order bits will be dropped.
 */
static inline int io__get_dec(struct io *io, __u64 *dec)
{
	bool first_read = true;

	*dec = 0;
	while (true) {
		int ch = io__get_char(io);

		if (ch < 0)
			return ch;
		if (ch >= '0' && ch <= '9')
			*dec = (*dec * 10) + ch - '0';
		else if (first_read)
			return -2;
		else
			return ch;
		first_read = false;
	}
}

#endif /* __API_IO__ */
