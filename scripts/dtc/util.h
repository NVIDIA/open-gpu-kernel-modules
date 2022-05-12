/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>

/*
 * Copyright 2011 The Chromium Authors, All Rights Reserved.
 * Copyright 2008 Jon Loeliger, Freescale Semiconductor, Inc.
 */

#ifdef __GNUC__
#ifdef __clang__
#define PRINTF(i, j)	__attribute__((format (printf, i, j)))
#else
#define PRINTF(i, j)	__attribute__((format (gnu_printf, i, j)))
#endif
#define NORETURN	__attribute__((noreturn))
#else
#define PRINTF(i, j)
#define NORETURN
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define stringify(s)	stringify_(s)
#define stringify_(s)	#s

static inline void NORETURN PRINTF(1, 2) die(const char *str, ...)
{
	va_list ap;

	va_start(ap, str);
	fprintf(stderr, "FATAL ERROR: ");
	vfprintf(stderr, str, ap);
	va_end(ap);
	exit(1);
}

static inline void *xmalloc(size_t len)
{
	void *new = malloc(len);

	if (!new)
		die("malloc() failed\n");

	return new;
}

static inline void *xrealloc(void *p, size_t len)
{
	void *new = realloc(p, len);

	if (!new)
		die("realloc() failed (len=%zd)\n", len);

	return new;
}

extern char *xstrdup(const char *s);

extern int PRINTF(2, 3) xasprintf(char **strp, const char *fmt, ...);
extern int PRINTF(2, 3) xasprintf_append(char **strp, const char *fmt, ...);
extern int xavsprintf_append(char **strp, const char *fmt, va_list ap);
extern char *join_path(const char *path, const char *name);

/**
 * Check a property of a given length to see if it is all printable and
 * has a valid terminator. The property can contain either a single string,
 * or multiple strings each of non-zero length.
 *
 * @param data	The string to check
 * @param len	The string length including terminator
 * @return 1 if a valid printable string, 0 if not
 */
bool util_is_printable_string(const void *data, int len);

/*
 * Parse an escaped character starting at index i in string s.  The resulting
 * character will be returned and the index i will be updated to point at the
 * character directly after the end of the encoding, this may be the '\0'
 * terminator of the string.
 */
char get_escape_char(const char *s, int *i);

/**
 * Read a device tree file into a buffer. This will report any errors on
 * stderr.
 *
 * @param filename	The filename to read, or - for stdin
 * @param len		If non-NULL, the amount of data we managed to read
 * @return Pointer to allocated buffer containing fdt, or NULL on error
 */
char *utilfdt_read(const char *filename, size_t *len);

/**
 * Read a device tree file into a buffer. Does not report errors, but only
 * returns them. The value returned can be passed to strerror() to obtain
 * an error message for the user.
 *
 * @param filename	The filename to read, or - for stdin
 * @param buffp		Returns pointer to buffer containing fdt
 * @param len		If non-NULL, the amount of data we managed to read
 * @return 0 if ok, else an errno value representing the error
 */
int utilfdt_read_err(const char *filename, char **buffp, size_t *len);

/**
 * Write a device tree buffer to a file. This will report any errors on
 * stderr.
 *
 * @param filename	The filename to write, or - for stdout
 * @param blob		Pointer to buffer containing fdt
 * @return 0 if ok, -1 on error
 */
int utilfdt_write(const char *filename, const void *blob);

/**
 * Write a device tree buffer to a file. Does not report errors, but only
 * returns them. The value returned can be passed to strerror() to obtain
 * an error message for the user.
 *
 * @param filename	The filename to write, or - for stdout
 * @param blob		Pointer to buffer containing fdt
 * @return 0 if ok, else an errno value representing the error
 */
int utilfdt_write_err(const char *filename, const void *blob);

/**
 * Decode a data type string. The purpose of this string
 *
 * The string consists of an optional character followed by the type:
 *	Modifier characters:
 *		hh or b	1 byte
 *		h	2 byte
 *		l	4 byte, default
 *
 *	Type character:
 *		s	string
 *		i	signed integer
 *		u	unsigned integer
 *		x	hex
 *
 * TODO: Implement ll modifier (8 bytes)
 * TODO: Implement o type (octal)
 *
 * @param fmt		Format string to process
 * @param type		Returns type found(s/d/u/x), or 0 if none
 * @param size		Returns size found(1,2,4,8) or 4 if none
 * @return 0 if ok, -1 on error (no type given, or other invalid format)
 */
int utilfdt_decode_type(const char *fmt, int *type, int *size);

/*
 * This is a usage message fragment for the -t option. It is the format
 * supported by utilfdt_decode_type.
 */

#define USAGE_TYPE_MSG \
	"<type>\ts=string, i=int, u=unsigned, x=hex\n" \
	"\tOptional modifier prefix:\n" \
	"\t\thh or b=byte, h=2 byte, l=4 byte (default)";

/**
 * Print property data in a readable format to stdout
 *
 * Properties that look like strings will be printed as strings. Otherwise
 * the data will be displayed either as cells (if len is a multiple of 4
 * bytes) or bytes.
 *
 * If len is 0 then this function does nothing.
 *
 * @param data	Pointers to property data
 * @param len	Length of property data
 */
void utilfdt_print_data(const char *data, int len);

/**
 * Show source version and exit
 */
void NORETURN util_version(void);

/**
 * Show usage and exit
 *
 * This helps standardize the output of various utils.  You most likely want
 * to use the usage() helper below rather than call this.
 *
 * @param errmsg	If non-NULL, an error message to display
 * @param synopsis	The initial example usage text (and possible examples)
 * @param short_opts	The string of short options
 * @param long_opts	The structure of long options
 * @param opts_help	An array of help strings (should align with long_opts)
 */
void NORETURN util_usage(const char *errmsg, const char *synopsis,
			 const char *short_opts,
			 struct option const long_opts[],
			 const char * const opts_help[]);

/**
 * Show usage and exit
 *
 * If you name all your usage variables with usage_xxx, then you can call this
 * help macro rather than expanding all arguments yourself.
 *
 * @param errmsg	If non-NULL, an error message to display
 */
#define usage(errmsg) \
	util_usage(errmsg, usage_synopsis, usage_short_opts, \
		   usage_long_opts, usage_opts_help)

/**
 * Call getopt_long() with standard options
 *
 * Since all util code runs getopt in the same way, provide a helper.
 */
#define util_getopt_long() getopt_long(argc, argv, usage_short_opts, \
				       usage_long_opts, NULL)

/* Helper for aligning long_opts array */
#define a_argument required_argument

/* Helper for usage_short_opts string constant */
#define USAGE_COMMON_SHORT_OPTS "hV"

/* Helper for usage_long_opts option array */
#define USAGE_COMMON_LONG_OPTS \
	{"help",      no_argument, NULL, 'h'}, \
	{"version",   no_argument, NULL, 'V'}, \
	{NULL,        no_argument, NULL, 0x0}

/* Helper for usage_opts_help array */
#define USAGE_COMMON_OPTS_HELP \
	"Print this help and exit", \
	"Print version and exit", \
	NULL

/* Helper for getopt case statements */
#define case_USAGE_COMMON_FLAGS \
	case 'h': usage(NULL); \
	case 'V': util_version(); \
	case '?': usage("unknown option");

#endif /* UTIL_H */
