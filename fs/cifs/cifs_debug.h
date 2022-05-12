/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *
 *   Copyright (c) International Business Machines  Corp., 2000,2002
 *   Modified by Steve French (sfrench@us.ibm.com)
*/

#ifndef _H_CIFS_DEBUG
#define _H_CIFS_DEBUG

#ifdef pr_fmt
#undef pr_fmt
#endif

#define pr_fmt(fmt) "CIFS: " fmt

void cifs_dump_mem(char *label, void *data, int length);
void cifs_dump_detail(void *buf, struct TCP_Server_Info *ptcp_info);
void cifs_dump_mids(struct TCP_Server_Info *);
extern bool traceSMB;		/* flag which enables the function below */
void dump_smb(void *, int);
#define CIFS_INFO	0x01
#define CIFS_RC		0x02
#define CIFS_TIMER	0x04

#define VFS 1
#define FYI 2
extern int cifsFYI;
#ifdef CONFIG_CIFS_DEBUG2
#define NOISY 4
#else
#define NOISY 0
#endif
#define ONCE 8

/*
 *	debug ON
 *	--------
 */
#ifdef CONFIG_CIFS_DEBUG


/*
 * When adding tracepoints and debug messages we have various choices.
 * Some considerations:
 *
 * Use cifs_dbg(VFS, ...) for things we always want logged, and the user to see
 *     cifs_info(...) slightly less important, admin can filter via loglevel > 6
 *     cifs_dbg(FYI, ...) minor debugging messages, off by default
 *     trace_smb3_*  ftrace functions are preferred for complex debug messages
 *                 intended for developers or experienced admins, off by default
 */

/* Information level messages, minor events */
#define cifs_info_func(ratefunc, fmt, ...)				\
	pr_info_ ## ratefunc(fmt, ##__VA_ARGS__)

#define cifs_info(fmt, ...)						\
	cifs_info_func(ratelimited, fmt, ##__VA_ARGS__)

/* information message: e.g., configuration, major event */
#define cifs_dbg_func(ratefunc, type, fmt, ...)				\
do {									\
	if ((type) & FYI && cifsFYI & CIFS_INFO) {			\
		pr_debug_ ## ratefunc("%s: " fmt,			\
				      __FILE__, ##__VA_ARGS__);		\
	} else if ((type) & VFS) {					\
		pr_err_ ## ratefunc("VFS: " fmt, ##__VA_ARGS__);	\
	} else if ((type) & NOISY && (NOISY != 0)) {			\
		pr_debug_ ## ratefunc(fmt, ##__VA_ARGS__);		\
	}								\
} while (0)

#define cifs_dbg(type, fmt, ...)					\
do {									\
	if ((type) & ONCE)						\
		cifs_dbg_func(once, type, fmt, ##__VA_ARGS__);		\
	else								\
		cifs_dbg_func(ratelimited, type, fmt, ##__VA_ARGS__);	\
} while (0)

#define cifs_server_dbg_func(ratefunc, type, fmt, ...)			\
do {									\
	const char *sn = "";						\
	if (server && server->hostname)					\
		sn = server->hostname;					\
	if ((type) & FYI && cifsFYI & CIFS_INFO) {			\
		pr_debug_ ## ratefunc("%s: \\\\%s " fmt,		\
				      __FILE__, sn, ##__VA_ARGS__);	\
	} else if ((type) & VFS) {					\
		pr_err_ ## ratefunc("VFS: \\\\%s " fmt,			\
				    sn, ##__VA_ARGS__);			\
	} else if ((type) & NOISY && (NOISY != 0)) {			\
		pr_debug_ ## ratefunc("\\\\%s " fmt,			\
				      sn, ##__VA_ARGS__);		\
	}								\
} while (0)

#define cifs_server_dbg(type, fmt, ...)					\
do {									\
	if ((type) & ONCE)						\
		cifs_server_dbg_func(once, type, fmt, ##__VA_ARGS__);	\
	else								\
		cifs_server_dbg_func(ratelimited, type, fmt,		\
				     ##__VA_ARGS__);			\
} while (0)

#define cifs_tcon_dbg_func(ratefunc, type, fmt, ...)			\
do {									\
	const char *tn = "";						\
	if (tcon && tcon->treeName)					\
		tn = tcon->treeName;					\
	if ((type) & FYI && cifsFYI & CIFS_INFO) {			\
		pr_debug_ ## ratefunc("%s: %s "	fmt,			\
				      __FILE__, tn, ##__VA_ARGS__);	\
	} else if ((type) & VFS) {					\
		pr_err_ ## ratefunc("VFS: %s " fmt, tn, ##__VA_ARGS__);	\
	} else if ((type) & NOISY && (NOISY != 0)) {			\
		pr_debug_ ## ratefunc("%s " fmt, tn, ##__VA_ARGS__);	\
	}								\
} while (0)

#define cifs_tcon_dbg(type, fmt, ...)					\
do {									\
	if ((type) & ONCE)						\
		cifs_tcon_dbg_func(once, type, fmt, ##__VA_ARGS__);	\
	else								\
		cifs_tcon_dbg_func(ratelimited, type, fmt,		\
				   ##__VA_ARGS__);			\
} while (0)

/*
 *	debug OFF
 *	---------
 */
#else		/* _CIFS_DEBUG */
#define cifs_dbg(type, fmt, ...)					\
do {									\
	if (0)								\
		pr_debug(fmt, ##__VA_ARGS__);				\
} while (0)

#define cifs_server_dbg(type, fmt, ...)					\
do {									\
	if (0)								\
		pr_debug("\\\\%s " fmt,					\
			 server->hostname, ##__VA_ARGS__);		\
} while (0)

#define cifs_tcon_dbg(type, fmt, ...)					\
do {									\
	if (0)								\
		pr_debug("%s " fmt, tcon->treeName, ##__VA_ARGS__);	\
} while (0)

#define cifs_info(fmt, ...)						\
	pr_info(fmt, ##__VA_ARGS__)
#endif

#endif				/* _H_CIFS_DEBUG */
