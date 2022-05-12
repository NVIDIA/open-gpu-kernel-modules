// SPDX-License-Identifier: GPL-2.0
#include <test_progs.h>
#include <network_helpers.h>
#include <error.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/uio.h>

#include "bpf_flow.skel.h"

#ifndef IP_MF
#define IP_MF 0x2000
#endif

#define CHECK_FLOW_KEYS(desc, got, expected)				\
	CHECK_ATTR(memcmp(&got, &expected, sizeof(got)) != 0,		\
	      desc,							\
	      "nhoff=%u/%u "						\
	      "thoff=%u/%u "						\
	      "addr_proto=0x%x/0x%x "					\
	      "is_frag=%u/%u "						\
	      "is_first_frag=%u/%u "					\
	      "is_encap=%u/%u "						\
	      "ip_proto=0x%x/0x%x "					\
	      "n_proto=0x%x/0x%x "					\
	      "flow_label=0x%x/0x%x "					\
	      "sport=%u/%u "						\
	      "dport=%u/%u\n",						\
	      got.nhoff, expected.nhoff,				\
	      got.thoff, expected.thoff,				\
	      got.addr_proto, expected.addr_proto,			\
	      got.is_frag, expected.is_frag,				\
	      got.is_first_frag, expected.is_first_frag,		\
	      got.is_encap, expected.is_encap,				\
	      got.ip_proto, expected.ip_proto,				\
	      got.n_proto, expected.n_proto,				\
	      got.flow_label, expected.flow_label,			\
	      got.sport, expected.sport,				\
	      got.dport, expected.dport)

struct ipv4_pkt {
	struct ethhdr eth;
	struct iphdr iph;
	struct tcphdr tcp;
} __packed;

struct ipip_pkt {
	struct ethhdr eth;
	struct iphdr iph;
	struct iphdr iph_inner;
	struct tcphdr tcp;
} __packed;

struct svlan_ipv4_pkt {
	struct ethhdr eth;
	__u16 vlan_tci;
	__u16 vlan_proto;
	struct iphdr iph;
	struct tcphdr tcp;
} __packed;

struct ipv6_pkt {
	struct ethhdr eth;
	struct ipv6hdr iph;
	struct tcphdr tcp;
} __packed;

struct ipv6_frag_pkt {
	struct ethhdr eth;
	struct ipv6hdr iph;
	struct frag_hdr {
		__u8 nexthdr;
		__u8 reserved;
		__be16 frag_off;
		__be32 identification;
	} ipf;
	struct tcphdr tcp;
} __packed;

struct dvlan_ipv6_pkt {
	struct ethhdr eth;
	__u16 vlan_tci;
	__u16 vlan_proto;
	__u16 vlan_tci2;
	__u16 vlan_proto2;
	struct ipv6hdr iph;
	struct tcphdr tcp;
} __packed;

struct test {
	const char *name;
	union {
		struct ipv4_pkt ipv4;
		struct svlan_ipv4_pkt svlan_ipv4;
		struct ipip_pkt ipip;
		struct ipv6_pkt ipv6;
		struct ipv6_frag_pkt ipv6_frag;
		struct dvlan_ipv6_pkt dvlan_ipv6;
	} pkt;
	struct bpf_flow_keys keys;
	__u32 flags;
};

#define VLAN_HLEN	4

static __u32 duration;
struct test tests[] = {
	{
		.name = "ipv4",
		.pkt.ipv4 = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_IP),
			.iph.ihl = 5,
			.iph.protocol = IPPROTO_TCP,
			.iph.tot_len = __bpf_constant_htons(MAGIC_BYTES),
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.nhoff = ETH_HLEN,
			.thoff = ETH_HLEN + sizeof(struct iphdr),
			.addr_proto = ETH_P_IP,
			.ip_proto = IPPROTO_TCP,
			.n_proto = __bpf_constant_htons(ETH_P_IP),
			.sport = 80,
			.dport = 8080,
		},
	},
	{
		.name = "ipv6",
		.pkt.ipv6 = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_IPV6),
			.iph.nexthdr = IPPROTO_TCP,
			.iph.payload_len = __bpf_constant_htons(MAGIC_BYTES),
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.nhoff = ETH_HLEN,
			.thoff = ETH_HLEN + sizeof(struct ipv6hdr),
			.addr_proto = ETH_P_IPV6,
			.ip_proto = IPPROTO_TCP,
			.n_proto = __bpf_constant_htons(ETH_P_IPV6),
			.sport = 80,
			.dport = 8080,
		},
	},
	{
		.name = "802.1q-ipv4",
		.pkt.svlan_ipv4 = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_8021Q),
			.vlan_proto = __bpf_constant_htons(ETH_P_IP),
			.iph.ihl = 5,
			.iph.protocol = IPPROTO_TCP,
			.iph.tot_len = __bpf_constant_htons(MAGIC_BYTES),
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.nhoff = ETH_HLEN + VLAN_HLEN,
			.thoff = ETH_HLEN + VLAN_HLEN + sizeof(struct iphdr),
			.addr_proto = ETH_P_IP,
			.ip_proto = IPPROTO_TCP,
			.n_proto = __bpf_constant_htons(ETH_P_IP),
			.sport = 80,
			.dport = 8080,
		},
	},
	{
		.name = "802.1ad-ipv6",
		.pkt.dvlan_ipv6 = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_8021AD),
			.vlan_proto = __bpf_constant_htons(ETH_P_8021Q),
			.vlan_proto2 = __bpf_constant_htons(ETH_P_IPV6),
			.iph.nexthdr = IPPROTO_TCP,
			.iph.payload_len = __bpf_constant_htons(MAGIC_BYTES),
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.nhoff = ETH_HLEN + VLAN_HLEN * 2,
			.thoff = ETH_HLEN + VLAN_HLEN * 2 +
				sizeof(struct ipv6hdr),
			.addr_proto = ETH_P_IPV6,
			.ip_proto = IPPROTO_TCP,
			.n_proto = __bpf_constant_htons(ETH_P_IPV6),
			.sport = 80,
			.dport = 8080,
		},
	},
	{
		.name = "ipv4-frag",
		.pkt.ipv4 = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_IP),
			.iph.ihl = 5,
			.iph.protocol = IPPROTO_TCP,
			.iph.tot_len = __bpf_constant_htons(MAGIC_BYTES),
			.iph.frag_off = __bpf_constant_htons(IP_MF),
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.flags = BPF_FLOW_DISSECTOR_F_PARSE_1ST_FRAG,
			.nhoff = ETH_HLEN,
			.thoff = ETH_HLEN + sizeof(struct iphdr),
			.addr_proto = ETH_P_IP,
			.ip_proto = IPPROTO_TCP,
			.n_proto = __bpf_constant_htons(ETH_P_IP),
			.is_frag = true,
			.is_first_frag = true,
			.sport = 80,
			.dport = 8080,
		},
		.flags = BPF_FLOW_DISSECTOR_F_PARSE_1ST_FRAG,
	},
	{
		.name = "ipv4-no-frag",
		.pkt.ipv4 = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_IP),
			.iph.ihl = 5,
			.iph.protocol = IPPROTO_TCP,
			.iph.tot_len = __bpf_constant_htons(MAGIC_BYTES),
			.iph.frag_off = __bpf_constant_htons(IP_MF),
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.nhoff = ETH_HLEN,
			.thoff = ETH_HLEN + sizeof(struct iphdr),
			.addr_proto = ETH_P_IP,
			.ip_proto = IPPROTO_TCP,
			.n_proto = __bpf_constant_htons(ETH_P_IP),
			.is_frag = true,
			.is_first_frag = true,
		},
	},
	{
		.name = "ipv6-frag",
		.pkt.ipv6_frag = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_IPV6),
			.iph.nexthdr = IPPROTO_FRAGMENT,
			.iph.payload_len = __bpf_constant_htons(MAGIC_BYTES),
			.ipf.nexthdr = IPPROTO_TCP,
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.flags = BPF_FLOW_DISSECTOR_F_PARSE_1ST_FRAG,
			.nhoff = ETH_HLEN,
			.thoff = ETH_HLEN + sizeof(struct ipv6hdr) +
				sizeof(struct frag_hdr),
			.addr_proto = ETH_P_IPV6,
			.ip_proto = IPPROTO_TCP,
			.n_proto = __bpf_constant_htons(ETH_P_IPV6),
			.is_frag = true,
			.is_first_frag = true,
			.sport = 80,
			.dport = 8080,
		},
		.flags = BPF_FLOW_DISSECTOR_F_PARSE_1ST_FRAG,
	},
	{
		.name = "ipv6-no-frag",
		.pkt.ipv6_frag = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_IPV6),
			.iph.nexthdr = IPPROTO_FRAGMENT,
			.iph.payload_len = __bpf_constant_htons(MAGIC_BYTES),
			.ipf.nexthdr = IPPROTO_TCP,
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.nhoff = ETH_HLEN,
			.thoff = ETH_HLEN + sizeof(struct ipv6hdr) +
				sizeof(struct frag_hdr),
			.addr_proto = ETH_P_IPV6,
			.ip_proto = IPPROTO_TCP,
			.n_proto = __bpf_constant_htons(ETH_P_IPV6),
			.is_frag = true,
			.is_first_frag = true,
		},
	},
	{
		.name = "ipv6-flow-label",
		.pkt.ipv6 = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_IPV6),
			.iph.nexthdr = IPPROTO_TCP,
			.iph.payload_len = __bpf_constant_htons(MAGIC_BYTES),
			.iph.flow_lbl = { 0xb, 0xee, 0xef },
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.nhoff = ETH_HLEN,
			.thoff = ETH_HLEN + sizeof(struct ipv6hdr),
			.addr_proto = ETH_P_IPV6,
			.ip_proto = IPPROTO_TCP,
			.n_proto = __bpf_constant_htons(ETH_P_IPV6),
			.sport = 80,
			.dport = 8080,
			.flow_label = __bpf_constant_htonl(0xbeeef),
		},
	},
	{
		.name = "ipv6-no-flow-label",
		.pkt.ipv6 = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_IPV6),
			.iph.nexthdr = IPPROTO_TCP,
			.iph.payload_len = __bpf_constant_htons(MAGIC_BYTES),
			.iph.flow_lbl = { 0xb, 0xee, 0xef },
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.flags = BPF_FLOW_DISSECTOR_F_STOP_AT_FLOW_LABEL,
			.nhoff = ETH_HLEN,
			.thoff = ETH_HLEN + sizeof(struct ipv6hdr),
			.addr_proto = ETH_P_IPV6,
			.ip_proto = IPPROTO_TCP,
			.n_proto = __bpf_constant_htons(ETH_P_IPV6),
			.flow_label = __bpf_constant_htonl(0xbeeef),
		},
		.flags = BPF_FLOW_DISSECTOR_F_STOP_AT_FLOW_LABEL,
	},
	{
		.name = "ipip-encap",
		.pkt.ipip = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_IP),
			.iph.ihl = 5,
			.iph.protocol = IPPROTO_IPIP,
			.iph.tot_len = __bpf_constant_htons(MAGIC_BYTES),
			.iph_inner.ihl = 5,
			.iph_inner.protocol = IPPROTO_TCP,
			.iph_inner.tot_len =
				__bpf_constant_htons(MAGIC_BYTES) -
				sizeof(struct iphdr),
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.nhoff = ETH_HLEN,
			.thoff = ETH_HLEN + sizeof(struct iphdr) +
				sizeof(struct iphdr),
			.addr_proto = ETH_P_IP,
			.ip_proto = IPPROTO_TCP,
			.n_proto = __bpf_constant_htons(ETH_P_IP),
			.is_encap = true,
			.sport = 80,
			.dport = 8080,
		},
	},
	{
		.name = "ipip-no-encap",
		.pkt.ipip = {
			.eth.h_proto = __bpf_constant_htons(ETH_P_IP),
			.iph.ihl = 5,
			.iph.protocol = IPPROTO_IPIP,
			.iph.tot_len = __bpf_constant_htons(MAGIC_BYTES),
			.iph_inner.ihl = 5,
			.iph_inner.protocol = IPPROTO_TCP,
			.iph_inner.tot_len =
				__bpf_constant_htons(MAGIC_BYTES) -
				sizeof(struct iphdr),
			.tcp.doff = 5,
			.tcp.source = 80,
			.tcp.dest = 8080,
		},
		.keys = {
			.flags = BPF_FLOW_DISSECTOR_F_STOP_AT_ENCAP,
			.nhoff = ETH_HLEN,
			.thoff = ETH_HLEN + sizeof(struct iphdr),
			.addr_proto = ETH_P_IP,
			.ip_proto = IPPROTO_IPIP,
			.n_proto = __bpf_constant_htons(ETH_P_IP),
			.is_encap = true,
		},
		.flags = BPF_FLOW_DISSECTOR_F_STOP_AT_ENCAP,
	},
};

static int create_tap(const char *ifname)
{
	struct ifreq ifr = {
		.ifr_flags = IFF_TAP | IFF_NO_PI | IFF_NAPI | IFF_NAPI_FRAGS,
	};
	int fd, ret;

	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

	fd = open("/dev/net/tun", O_RDWR);
	if (fd < 0)
		return -1;

	ret = ioctl(fd, TUNSETIFF, &ifr);
	if (ret)
		return -1;

	return fd;
}

static int tx_tap(int fd, void *pkt, size_t len)
{
	struct iovec iov[] = {
		{
			.iov_len = len,
			.iov_base = pkt,
		},
	};
	return writev(fd, iov, ARRAY_SIZE(iov));
}

static int ifup(const char *ifname)
{
	struct ifreq ifr = {};
	int sk, ret;

	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

	sk = socket(PF_INET, SOCK_DGRAM, 0);
	if (sk < 0)
		return -1;

	ret = ioctl(sk, SIOCGIFFLAGS, &ifr);
	if (ret) {
		close(sk);
		return -1;
	}

	ifr.ifr_flags |= IFF_UP;
	ret = ioctl(sk, SIOCSIFFLAGS, &ifr);
	if (ret) {
		close(sk);
		return -1;
	}

	close(sk);
	return 0;
}

static int init_prog_array(struct bpf_object *obj, struct bpf_map *prog_array)
{
	int i, err, map_fd, prog_fd;
	struct bpf_program *prog;
	char prog_name[32];

	map_fd = bpf_map__fd(prog_array);
	if (map_fd < 0)
		return -1;

	for (i = 0; i < bpf_map__def(prog_array)->max_entries; i++) {
		snprintf(prog_name, sizeof(prog_name), "flow_dissector/%i", i);

		prog = bpf_object__find_program_by_title(obj, prog_name);
		if (!prog)
			return -1;

		prog_fd = bpf_program__fd(prog);
		if (prog_fd < 0)
			return -1;

		err = bpf_map_update_elem(map_fd, &i, &prog_fd, BPF_ANY);
		if (err)
			return -1;
	}
	return 0;
}

static void run_tests_skb_less(int tap_fd, struct bpf_map *keys)
{
	int i, err, keys_fd;

	keys_fd = bpf_map__fd(keys);
	if (CHECK(keys_fd < 0, "bpf_map__fd", "err %d\n", keys_fd))
		return;

	for (i = 0; i < ARRAY_SIZE(tests); i++) {
		/* Keep in sync with 'flags' from eth_get_headlen. */
		__u32 eth_get_headlen_flags =
			BPF_FLOW_DISSECTOR_F_PARSE_1ST_FRAG;
		struct bpf_prog_test_run_attr tattr = {};
		struct bpf_flow_keys flow_keys = {};
		__u32 key = (__u32)(tests[i].keys.sport) << 16 |
			    tests[i].keys.dport;

		/* For skb-less case we can't pass input flags; run
		 * only the tests that have a matching set of flags.
		 */

		if (tests[i].flags != eth_get_headlen_flags)
			continue;

		err = tx_tap(tap_fd, &tests[i].pkt, sizeof(tests[i].pkt));
		CHECK(err < 0, "tx_tap", "err %d errno %d\n", err, errno);

		err = bpf_map_lookup_elem(keys_fd, &key, &flow_keys);
		CHECK_ATTR(err, tests[i].name, "bpf_map_lookup_elem %d\n", err);

		CHECK_ATTR(err, tests[i].name, "skb-less err %d\n", err);
		CHECK_FLOW_KEYS(tests[i].name, flow_keys, tests[i].keys);

		err = bpf_map_delete_elem(keys_fd, &key);
		CHECK_ATTR(err, tests[i].name, "bpf_map_delete_elem %d\n", err);
	}
}

static void test_skb_less_prog_attach(struct bpf_flow *skel, int tap_fd)
{
	int err, prog_fd;

	prog_fd = bpf_program__fd(skel->progs._dissect);
	if (CHECK(prog_fd < 0, "bpf_program__fd", "err %d\n", prog_fd))
		return;

	err = bpf_prog_attach(prog_fd, 0, BPF_FLOW_DISSECTOR, 0);
	if (CHECK(err, "bpf_prog_attach", "err %d errno %d\n", err, errno))
		return;

	run_tests_skb_less(tap_fd, skel->maps.last_dissection);

	err = bpf_prog_detach2(prog_fd, 0, BPF_FLOW_DISSECTOR);
	CHECK(err, "bpf_prog_detach2", "err %d errno %d\n", err, errno);
}

static void test_skb_less_link_create(struct bpf_flow *skel, int tap_fd)
{
	struct bpf_link *link;
	int err, net_fd;

	net_fd = open("/proc/self/ns/net", O_RDONLY);
	if (CHECK(net_fd < 0, "open(/proc/self/ns/net)", "err %d\n", errno))
		return;

	link = bpf_program__attach_netns(skel->progs._dissect, net_fd);
	if (CHECK(IS_ERR(link), "attach_netns", "err %ld\n", PTR_ERR(link)))
		goto out_close;

	run_tests_skb_less(tap_fd, skel->maps.last_dissection);

	err = bpf_link__destroy(link);
	CHECK(err, "bpf_link__destroy", "err %d\n", err);
out_close:
	close(net_fd);
}

void test_flow_dissector(void)
{
	int i, err, prog_fd, keys_fd = -1, tap_fd;
	struct bpf_flow *skel;

	skel = bpf_flow__open_and_load();
	if (CHECK(!skel, "skel", "failed to open/load skeleton\n"))
		return;

	prog_fd = bpf_program__fd(skel->progs._dissect);
	if (CHECK(prog_fd < 0, "bpf_program__fd", "err %d\n", prog_fd))
		goto out_destroy_skel;
	keys_fd = bpf_map__fd(skel->maps.last_dissection);
	if (CHECK(keys_fd < 0, "bpf_map__fd", "err %d\n", keys_fd))
		goto out_destroy_skel;
	err = init_prog_array(skel->obj, skel->maps.jmp_table);
	if (CHECK(err, "init_prog_array", "err %d\n", err))
		goto out_destroy_skel;

	for (i = 0; i < ARRAY_SIZE(tests); i++) {
		struct bpf_flow_keys flow_keys;
		struct bpf_prog_test_run_attr tattr = {
			.prog_fd = prog_fd,
			.data_in = &tests[i].pkt,
			.data_size_in = sizeof(tests[i].pkt),
			.data_out = &flow_keys,
		};
		static struct bpf_flow_keys ctx = {};

		if (tests[i].flags) {
			tattr.ctx_in = &ctx;
			tattr.ctx_size_in = sizeof(ctx);
			ctx.flags = tests[i].flags;
		}

		err = bpf_prog_test_run_xattr(&tattr);
		CHECK_ATTR(tattr.data_size_out != sizeof(flow_keys) ||
			   err || tattr.retval != 1,
			   tests[i].name,
			   "err %d errno %d retval %d duration %d size %u/%zu\n",
			   err, errno, tattr.retval, tattr.duration,
			   tattr.data_size_out, sizeof(flow_keys));
		CHECK_FLOW_KEYS(tests[i].name, flow_keys, tests[i].keys);
	}

	/* Do the same tests but for skb-less flow dissector.
	 * We use a known path in the net/tun driver that calls
	 * eth_get_headlen and we manually export bpf_flow_keys
	 * via BPF map in this case.
	 */

	tap_fd = create_tap("tap0");
	CHECK(tap_fd < 0, "create_tap", "tap_fd %d errno %d\n", tap_fd, errno);
	err = ifup("tap0");
	CHECK(err, "ifup", "err %d errno %d\n", err, errno);

	/* Test direct prog attachment */
	test_skb_less_prog_attach(skel, tap_fd);
	/* Test indirect prog attachment via link */
	test_skb_less_link_create(skel, tap_fd);

	close(tap_fd);
out_destroy_skel:
	bpf_flow__destroy(skel);
}
