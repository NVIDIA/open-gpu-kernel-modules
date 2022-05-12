// SPDX-License-Identifier: GPL-2.0
/* Copyright (c) 2020 Facebook */
#include "bpf_iter.h"
#include "bpf_tracing_net.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char _license[] SEC("license") = "GPL";

struct {
	__uint(type, BPF_MAP_TYPE_SK_STORAGE);
	__uint(map_flags, BPF_F_NO_PREALLOC);
	__type(key, int);
	__type(value, int);
} sk_stg_map SEC(".maps");

__u32 val_sum = 0;
__u32 ipv6_sk_count = 0;

SEC("iter/bpf_sk_storage_map")
int dump_bpf_sk_storage_map(struct bpf_iter__bpf_sk_storage_map *ctx)
{
	struct sock *sk = ctx->sk;
	__u32 *val = ctx->value;

	if (sk == (void *)0 || val == (void *)0)
		return 0;

	if (sk->sk_family == AF_INET6)
		ipv6_sk_count++;

	val_sum += *val;
	return 0;
}
