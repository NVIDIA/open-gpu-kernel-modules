// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <assert.h>
#include <linux/bpf.h>
#include <bpf/bpf.h>
#include <bpf/libbpf.h>
#include "sock_example.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/resource.h>

struct pair {
	__u64 packets;
	__u64 bytes;
};

int main(int ac, char **argv)
{
	struct bpf_object *obj;
	int map_fd, prog_fd;
	char filename[256];
	int i, sock;
	FILE *f;

	snprintf(filename, sizeof(filename), "%s_kern.o", argv[0]);

	if (bpf_prog_load(filename, BPF_PROG_TYPE_SOCKET_FILTER,
			  &obj, &prog_fd))
		return 1;

	map_fd = bpf_object__find_map_fd_by_name(obj, "hash_map");

	sock = open_raw_sock("lo");

	assert(setsockopt(sock, SOL_SOCKET, SO_ATTACH_BPF, &prog_fd,
			  sizeof(prog_fd)) == 0);

	f = popen("ping -4 -c5 localhost", "r");
	(void) f;

	for (i = 0; i < 5; i++) {
		int key = 0, next_key;
		struct pair value;

		while (bpf_map_get_next_key(map_fd, &key, &next_key) == 0) {
			bpf_map_lookup_elem(map_fd, &next_key, &value);
			printf("ip %s bytes %lld packets %lld\n",
			       inet_ntoa((struct in_addr){htonl(next_key)}),
			       value.bytes, value.packets);
			key = next_key;
		}
		sleep(1);
	}
	return 0;
}
