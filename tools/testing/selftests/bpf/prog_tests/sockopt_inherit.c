// SPDX-License-Identifier: GPL-2.0
#include <test_progs.h>
#include "cgroup_helpers.h"

#define SOL_CUSTOM			0xdeadbeef
#define CUSTOM_INHERIT1			0
#define CUSTOM_INHERIT2			1
#define CUSTOM_LISTENER			2

static int connect_to_server(int server_fd)
{
	struct sockaddr_storage addr;
	socklen_t len = sizeof(addr);
	int fd;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		log_err("Failed to create client socket");
		return -1;
	}

	if (getsockname(server_fd, (struct sockaddr *)&addr, &len)) {
		log_err("Failed to get server addr");
		goto out;
	}

	if (connect(fd, (const struct sockaddr *)&addr, len) < 0) {
		log_err("Fail to connect to server");
		goto out;
	}

	return fd;

out:
	close(fd);
	return -1;
}

static int verify_sockopt(int fd, int optname, const char *msg, char expected)
{
	socklen_t optlen = 1;
	char buf = 0;
	int err;

	err = getsockopt(fd, SOL_CUSTOM, optname, &buf, &optlen);
	if (err) {
		log_err("%s: failed to call getsockopt", msg);
		return 1;
	}

	printf("%s %d: got=0x%x ? expected=0x%x\n", msg, optname, buf, expected);

	if (buf != expected) {
		log_err("%s: unexpected getsockopt value %d != %d", msg,
			buf, expected);
		return 1;
	}

	return 0;
}

static pthread_mutex_t server_started_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t server_started = PTHREAD_COND_INITIALIZER;

static void *server_thread(void *arg)
{
	struct sockaddr_storage addr;
	socklen_t len = sizeof(addr);
	int fd = *(int *)arg;
	int client_fd;
	int err = 0;

	err = listen(fd, 1);

	pthread_mutex_lock(&server_started_mtx);
	pthread_cond_signal(&server_started);
	pthread_mutex_unlock(&server_started_mtx);

	if (CHECK_FAIL(err < 0)) {
		perror("Failed to listed on socket");
		return NULL;
	}

	err += verify_sockopt(fd, CUSTOM_INHERIT1, "listen", 1);
	err += verify_sockopt(fd, CUSTOM_INHERIT2, "listen", 1);
	err += verify_sockopt(fd, CUSTOM_LISTENER, "listen", 1);

	client_fd = accept(fd, (struct sockaddr *)&addr, &len);
	if (CHECK_FAIL(client_fd < 0)) {
		perror("Failed to accept client");
		return NULL;
	}

	err += verify_sockopt(client_fd, CUSTOM_INHERIT1, "accept", 1);
	err += verify_sockopt(client_fd, CUSTOM_INHERIT2, "accept", 1);
	err += verify_sockopt(client_fd, CUSTOM_LISTENER, "accept", 0);

	close(client_fd);

	return (void *)(long)err;
}

static int start_server(void)
{
	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = htonl(INADDR_LOOPBACK),
	};
	char buf;
	int err;
	int fd;
	int i;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		log_err("Failed to create server socket");
		return -1;
	}

	for (i = CUSTOM_INHERIT1; i <= CUSTOM_LISTENER; i++) {
		buf = 0x01;
		err = setsockopt(fd, SOL_CUSTOM, i, &buf, 1);
		if (err) {
			log_err("Failed to call setsockopt(%d)", i);
			close(fd);
			return -1;
		}
	}

	if (bind(fd, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
		log_err("Failed to bind socket");
		close(fd);
		return -1;
	}

	return fd;
}

static int prog_attach(struct bpf_object *obj, int cgroup_fd, const char *title)
{
	enum bpf_attach_type attach_type;
	enum bpf_prog_type prog_type;
	struct bpf_program *prog;
	int err;

	err = libbpf_prog_type_by_name(title, &prog_type, &attach_type);
	if (err) {
		log_err("Failed to deduct types for %s BPF program", title);
		return -1;
	}

	prog = bpf_object__find_program_by_title(obj, title);
	if (!prog) {
		log_err("Failed to find %s BPF program", title);
		return -1;
	}

	err = bpf_prog_attach(bpf_program__fd(prog), cgroup_fd,
			      attach_type, 0);
	if (err) {
		log_err("Failed to attach %s BPF program", title);
		return -1;
	}

	return 0;
}

static void run_test(int cgroup_fd)
{
	struct bpf_prog_load_attr attr = {
		.file = "./sockopt_inherit.o",
	};
	int server_fd = -1, client_fd;
	struct bpf_object *obj;
	void *server_err;
	pthread_t tid;
	int ignored;
	int err;

	err = bpf_prog_load_xattr(&attr, &obj, &ignored);
	if (CHECK_FAIL(err))
		return;

	err = prog_attach(obj, cgroup_fd, "cgroup/getsockopt");
	if (CHECK_FAIL(err))
		goto close_bpf_object;

	err = prog_attach(obj, cgroup_fd, "cgroup/setsockopt");
	if (CHECK_FAIL(err))
		goto close_bpf_object;

	server_fd = start_server();
	if (CHECK_FAIL(server_fd < 0))
		goto close_bpf_object;

	pthread_mutex_lock(&server_started_mtx);
	if (CHECK_FAIL(pthread_create(&tid, NULL, server_thread,
				      (void *)&server_fd)))
		goto close_server_fd;
	pthread_cond_wait(&server_started, &server_started_mtx);
	pthread_mutex_unlock(&server_started_mtx);

	client_fd = connect_to_server(server_fd);
	if (CHECK_FAIL(client_fd < 0))
		goto close_server_fd;

	CHECK_FAIL(verify_sockopt(client_fd, CUSTOM_INHERIT1, "connect", 0));
	CHECK_FAIL(verify_sockopt(client_fd, CUSTOM_INHERIT2, "connect", 0));
	CHECK_FAIL(verify_sockopt(client_fd, CUSTOM_LISTENER, "connect", 0));

	pthread_join(tid, &server_err);

	err = (int)(long)server_err;
	CHECK_FAIL(err);

	close(client_fd);

close_server_fd:
	close(server_fd);
close_bpf_object:
	bpf_object__close(obj);
}

void test_sockopt_inherit(void)
{
	int cgroup_fd;

	cgroup_fd = test__join_cgroup("/sockopt_inherit");
	if (CHECK_FAIL(cgroup_fd < 0))
		return;

	run_test(cgroup_fd);
	close(cgroup_fd);
}
