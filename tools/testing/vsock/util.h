/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef UTIL_H
#define UTIL_H

#include <sys/socket.h>
#include <linux/vm_sockets.h>

/* Tests can either run as the client or the server */
enum test_mode {
	TEST_MODE_UNSET,
	TEST_MODE_CLIENT,
	TEST_MODE_SERVER
};

/* Test runner options */
struct test_opts {
	enum test_mode mode;
	unsigned int peer_cid;
};

/* A test case definition.  Test functions must print failures to stderr and
 * terminate with exit(EXIT_FAILURE).
 */
struct test_case {
	const char *name; /* human-readable name */

	/* Called when test mode is TEST_MODE_CLIENT */
	void (*run_client)(const struct test_opts *opts);

	/* Called when test mode is TEST_MODE_SERVER */
	void (*run_server)(const struct test_opts *opts);

	bool skip;
};

void init_signals(void);
unsigned int parse_cid(const char *str);
int vsock_stream_connect(unsigned int cid, unsigned int port);
int vsock_stream_accept(unsigned int cid, unsigned int port,
			struct sockaddr_vm *clientaddrp);
void vsock_wait_remote_close(int fd);
void send_byte(int fd, int expected_ret, int flags);
void recv_byte(int fd, int expected_ret, int flags);
void run_tests(const struct test_case *test_cases,
	       const struct test_opts *opts);
void list_tests(const struct test_case *test_cases);
void skip_test(struct test_case *test_cases, size_t test_cases_len,
	       const char *test_id_str);
#endif /* UTIL_H */
