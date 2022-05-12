/* Copyright (c) 2016 Facebook
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 */
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/version.h>
#include <uapi/linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "trace_common.h"

#define MAX_ENTRIES 1000
#define MAX_NR_CPUS 1024

struct {
	__uint(type, BPF_MAP_TYPE_HASH);
	__type(key, u32);
	__type(value, long);
	__uint(max_entries, MAX_ENTRIES);
} hash_map SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_LRU_HASH);
	__type(key, u32);
	__type(value, long);
	__uint(max_entries, 10000);
} lru_hash_map SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_LRU_HASH);
	__type(key, u32);
	__type(value, long);
	__uint(max_entries, 10000);
	__uint(map_flags, BPF_F_NO_COMMON_LRU);
} nocommon_lru_hash_map SEC(".maps");

struct inner_lru {
	__uint(type, BPF_MAP_TYPE_LRU_HASH);
	__type(key, u32);
	__type(value, long);
	__uint(max_entries, MAX_ENTRIES);
	__uint(map_flags, BPF_F_NUMA_NODE);
	__uint(numa_node, 0);
} inner_lru_hash_map SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_ARRAY_OF_MAPS);
	__uint(max_entries, MAX_NR_CPUS);
	__uint(key_size, sizeof(u32));
	__array(values, struct inner_lru); /* use inner_lru as inner map */
} array_of_lru_hashs SEC(".maps") = {
	/* statically initialize the first element */
	.values = { &inner_lru_hash_map },
};

struct {
	__uint(type, BPF_MAP_TYPE_PERCPU_HASH);
	__uint(key_size, sizeof(u32));
	__uint(value_size, sizeof(long));
	__uint(max_entries, MAX_ENTRIES);
} percpu_hash_map SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_HASH);
	__type(key, u32);
	__type(value, long);
	__uint(max_entries, MAX_ENTRIES);
	__uint(map_flags, BPF_F_NO_PREALLOC);
} hash_map_alloc SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_PERCPU_HASH);
	__uint(key_size, sizeof(u32));
	__uint(value_size, sizeof(long));
	__uint(max_entries, MAX_ENTRIES);
	__uint(map_flags, BPF_F_NO_PREALLOC);
} percpu_hash_map_alloc SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_LPM_TRIE);
	__uint(key_size, 8);
	__uint(value_size, sizeof(long));
	__uint(max_entries, 10000);
	__uint(map_flags, BPF_F_NO_PREALLOC);
} lpm_trie_map_alloc SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_ARRAY);
	__type(key, u32);
	__type(value, long);
	__uint(max_entries, MAX_ENTRIES);
} array_map SEC(".maps");

struct {
	__uint(type, BPF_MAP_TYPE_LRU_HASH);
	__type(key, u32);
	__type(value, long);
	__uint(max_entries, MAX_ENTRIES);
} lru_hash_lookup_map SEC(".maps");

SEC("kprobe/" SYSCALL(sys_getuid))
int stress_hmap(struct pt_regs *ctx)
{
	u32 key = bpf_get_current_pid_tgid();
	long init_val = 1;
	long *value;

	bpf_map_update_elem(&hash_map, &key, &init_val, BPF_ANY);
	value = bpf_map_lookup_elem(&hash_map, &key);
	if (value)
		bpf_map_delete_elem(&hash_map, &key);

	return 0;
}

SEC("kprobe/" SYSCALL(sys_geteuid))
int stress_percpu_hmap(struct pt_regs *ctx)
{
	u32 key = bpf_get_current_pid_tgid();
	long init_val = 1;
	long *value;

	bpf_map_update_elem(&percpu_hash_map, &key, &init_val, BPF_ANY);
	value = bpf_map_lookup_elem(&percpu_hash_map, &key);
	if (value)
		bpf_map_delete_elem(&percpu_hash_map, &key);
	return 0;
}

SEC("kprobe/" SYSCALL(sys_getgid))
int stress_hmap_alloc(struct pt_regs *ctx)
{
	u32 key = bpf_get_current_pid_tgid();
	long init_val = 1;
	long *value;

	bpf_map_update_elem(&hash_map_alloc, &key, &init_val, BPF_ANY);
	value = bpf_map_lookup_elem(&hash_map_alloc, &key);
	if (value)
		bpf_map_delete_elem(&hash_map_alloc, &key);
	return 0;
}

SEC("kprobe/" SYSCALL(sys_getegid))
int stress_percpu_hmap_alloc(struct pt_regs *ctx)
{
	u32 key = bpf_get_current_pid_tgid();
	long init_val = 1;
	long *value;

	bpf_map_update_elem(&percpu_hash_map_alloc, &key, &init_val, BPF_ANY);
	value = bpf_map_lookup_elem(&percpu_hash_map_alloc, &key);
	if (value)
		bpf_map_delete_elem(&percpu_hash_map_alloc, &key);
	return 0;
}

SEC("kprobe/" SYSCALL(sys_connect))
int stress_lru_hmap_alloc(struct pt_regs *ctx)
{
	struct pt_regs *real_regs = (struct pt_regs *)PT_REGS_PARM1_CORE(ctx);
	char fmt[] = "Failed at stress_lru_hmap_alloc. ret:%dn";
	union {
		u16 dst6[8];
		struct {
			u16 magic0;
			u16 magic1;
			u16 tcase;
			u16 unused16;
			u32 unused32;
			u32 key;
		};
	} test_params;
	struct sockaddr_in6 *in6;
	u16 test_case;
	int addrlen, ret;
	long val = 1;
	u32 key = 0;

	in6 = (struct sockaddr_in6 *)PT_REGS_PARM2_CORE(real_regs);
	addrlen = (int)PT_REGS_PARM3_CORE(real_regs);

	if (addrlen != sizeof(*in6))
		return 0;

	ret = bpf_probe_read_user(test_params.dst6, sizeof(test_params.dst6),
				  &in6->sin6_addr);
	if (ret)
		goto done;

	if (test_params.magic0 != 0xdead ||
	    test_params.magic1 != 0xbeef)
		return 0;

	test_case = test_params.tcase;
	if (test_case != 3)
		key = bpf_get_prandom_u32();

	if (test_case == 0) {
		ret = bpf_map_update_elem(&lru_hash_map, &key, &val, BPF_ANY);
	} else if (test_case == 1) {
		ret = bpf_map_update_elem(&nocommon_lru_hash_map, &key, &val,
					  BPF_ANY);
	} else if (test_case == 2) {
		void *nolocal_lru_map;
		int cpu = bpf_get_smp_processor_id();

		nolocal_lru_map = bpf_map_lookup_elem(&array_of_lru_hashs,
						      &cpu);
		if (!nolocal_lru_map) {
			ret = -ENOENT;
			goto done;
		}

		ret = bpf_map_update_elem(nolocal_lru_map, &key, &val,
					  BPF_ANY);
	} else if (test_case == 3) {
		u32 i;

		key = test_params.key;

#pragma clang loop unroll(full)
		for (i = 0; i < 32; i++) {
			bpf_map_lookup_elem(&lru_hash_lookup_map, &key);
			key++;
		}
	} else {
		ret = -EINVAL;
	}

done:
	if (ret)
		bpf_trace_printk(fmt, sizeof(fmt), ret);

	return 0;
}

SEC("kprobe/" SYSCALL(sys_gettid))
int stress_lpm_trie_map_alloc(struct pt_regs *ctx)
{
	union {
		u32 b32[2];
		u8 b8[8];
	} key;
	unsigned int i;

	key.b32[0] = 32;
	key.b8[4] = 192;
	key.b8[5] = 168;
	key.b8[6] = 0;
	key.b8[7] = 1;

#pragma clang loop unroll(full)
	for (i = 0; i < 32; ++i)
		bpf_map_lookup_elem(&lpm_trie_map_alloc, &key);

	return 0;
}

SEC("kprobe/" SYSCALL(sys_getpgid))
int stress_hash_map_lookup(struct pt_regs *ctx)
{
	u32 key = 1, i;
	long *value;

#pragma clang loop unroll(full)
	for (i = 0; i < 64; ++i)
		value = bpf_map_lookup_elem(&hash_map, &key);

	return 0;
}

SEC("kprobe/" SYSCALL(sys_getppid))
int stress_array_map_lookup(struct pt_regs *ctx)
{
	u32 key = 1, i;
	long *value;

#pragma clang loop unroll(full)
	for (i = 0; i < 64; ++i)
		value = bpf_map_lookup_elem(&array_map, &key);

	return 0;
}

char _license[] SEC("license") = "GPL";
u32 _version SEC("version") = LINUX_VERSION_CODE;
