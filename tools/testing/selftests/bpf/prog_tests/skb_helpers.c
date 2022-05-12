// SPDX-License-Identifier: GPL-2.0
#include <test_progs.h>
#include <network_helpers.h>

void test_skb_helpers(void)
{
	struct __sk_buff skb = {
		.wire_len = 100,
		.gso_segs = 8,
		.gso_size = 10,
	};
	struct bpf_prog_test_run_attr tattr = {
		.data_in = &pkt_v4,
		.data_size_in = sizeof(pkt_v4),
		.ctx_in = &skb,
		.ctx_size_in = sizeof(skb),
		.ctx_out = &skb,
		.ctx_size_out = sizeof(skb),
	};
	struct bpf_object *obj;
	int err;

	err = bpf_prog_load("./test_skb_helpers.o", BPF_PROG_TYPE_SCHED_CLS, &obj,
			    &tattr.prog_fd);
	if (CHECK_ATTR(err, "load", "err %d errno %d\n", err, errno))
		return;
	err = bpf_prog_test_run_xattr(&tattr);
	CHECK_ATTR(err, "len", "err %d errno %d\n", err, errno);
	bpf_object__close(obj);
}
