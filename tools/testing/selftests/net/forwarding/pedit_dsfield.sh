#!/bin/bash
# SPDX-License-Identifier: GPL-2.0

# This test sends traffic from H1 to H2. Either on ingress of $swp1, or on
# egress of $swp2, the traffic is acted upon by a pedit action. An ingress
# filter installed on $h2 verifies that the packet looks like expected.
#
# +----------------------+                             +----------------------+
# | H1                   |                             |                   H2 |
# |    + $h1             |                             |            $h2 +     |
# |    | 192.0.2.1/28    |                             |   192.0.2.2/28 |     |
# +----|-----------------+                             +----------------|-----+
#      |                                                                |
# +----|----------------------------------------------------------------|-----+
# | SW |                                                                |     |
# |  +-|----------------------------------------------------------------|-+   |
# |  | + $swp1                       BR                           $swp2 + |   |
# |  +--------------------------------------------------------------------+   |
# +---------------------------------------------------------------------------+

ALL_TESTS="
	ping_ipv4
	ping_ipv6
	test_ip_dsfield
	test_ip_dscp
	test_ip_ecn
	test_ip_dscp_ecn
	test_ip6_dsfield
	test_ip6_dscp
	test_ip6_ecn
"

NUM_NETIFS=4
source lib.sh
source tc_common.sh

: ${HIT_TIMEOUT:=2000} # ms

h1_create()
{
	simple_if_init $h1 192.0.2.1/28 2001:db8:1::1/64
}

h1_destroy()
{
	simple_if_fini $h1 192.0.2.1/28 2001:db8:1::1/64
}

h2_create()
{
	simple_if_init $h2 192.0.2.2/28 2001:db8:1::2/64
	tc qdisc add dev $h2 clsact
}

h2_destroy()
{
	tc qdisc del dev $h2 clsact
	simple_if_fini $h2 192.0.2.2/28 2001:db8:1::2/64
}

switch_create()
{
	ip link add name br1 up type bridge vlan_filtering 1
	ip link set dev $swp1 master br1
	ip link set dev $swp1 up
	ip link set dev $swp2 master br1
	ip link set dev $swp2 up

	tc qdisc add dev $swp1 clsact
	tc qdisc add dev $swp2 clsact
}

switch_destroy()
{
	tc qdisc del dev $swp2 clsact
	tc qdisc del dev $swp1 clsact

	ip link set dev $swp2 nomaster
	ip link set dev $swp1 nomaster
	ip link del dev br1
}

setup_prepare()
{
	h1=${NETIFS[p1]}
	swp1=${NETIFS[p2]}

	swp2=${NETIFS[p3]}
	h2=${NETIFS[p4]}

	h2mac=$(mac_get $h2)

	vrf_prepare
	h1_create
	h2_create
	switch_create
}

cleanup()
{
	pre_cleanup

	switch_destroy
	h2_destroy
	h1_destroy
	vrf_cleanup
}

ping_ipv4()
{
	ping_test $h1 192.0.2.2
}

ping_ipv6()
{
	ping6_test $h1 2001:db8:1::2
}

do_test_pedit_dsfield_common()
{
	local pedit_locus=$1; shift
	local pedit_action=$1; shift
	local mz_flags=$1; shift

	RET=0

	# TOS 125: DSCP 31, ECN 1. Used for testing that the relevant part is
	# overwritten when zero is selected.
	$MZ $mz_flags $h1 -c 10 -d 20msec -p 100 \
	    -a own -b $h2mac -q -t tcp tos=0x7d,sp=54321,dp=12345

	local pkts
	pkts=$(busywait "$TC_HIT_TIMEOUT" until_counter_is ">= 10" \
			tc_rule_handle_stats_get "dev $h2 ingress" 101)
	check_err $? "Expected to get 10 packets on test probe, but got $pkts."

	pkts=$(tc_rule_handle_stats_get "$pedit_locus" 101)
	((pkts >= 10))
	check_err $? "Expected to get 10 packets on pedit rule, but got $pkts."

	log_test "$pedit_locus pedit $pedit_action"
}

do_test_pedit_dsfield()
{
	local pedit_locus=$1; shift
	local pedit_action=$1; shift
	local match_prot=$1; shift
	local match_flower=$1; shift
	local mz_flags=$1; shift
	local saddr=$1; shift
	local daddr=$1; shift

	tc filter add $pedit_locus handle 101 pref 1 \
	   flower action pedit ex munge $pedit_action
	tc filter add dev $h2 ingress handle 101 pref 1 prot $match_prot \
	   flower skip_hw $match_flower action pass

	do_test_pedit_dsfield_common "$pedit_locus" "$pedit_action" "$mz_flags"

	tc filter del dev $h2 ingress pref 1
	tc filter del $pedit_locus pref 1
}

do_test_ip_dsfield()
{
	local locus=$1; shift
	local dsfield

	for dsfield in 0 1 2 3 128 252 253 254 255; do
		do_test_pedit_dsfield "$locus"				\
				      "ip dsfield set $dsfield"		\
				      ip "ip_tos $dsfield"		\
				      "-A 192.0.2.1 -B 192.0.2.2"
	done
}

test_ip_dsfield()
{
	do_test_ip_dsfield "dev $swp1 ingress"
	do_test_ip_dsfield "dev $swp2 egress"
}

do_test_ip_dscp()
{
	local locus=$1; shift
	local dscp

	for dscp in 0 1 2 3 32 61 62 63; do
		do_test_pedit_dsfield "$locus"				       \
				  "ip dsfield set $((dscp << 2)) retain 0xfc"  \
				  ip "ip_tos $(((dscp << 2) | 1))"	       \
				  "-A 192.0.2.1 -B 192.0.2.2"
	done
}

test_ip_dscp()
{
	do_test_ip_dscp "dev $swp1 ingress"
	do_test_ip_dscp "dev $swp2 egress"
}

do_test_ip_ecn()
{
	local locus=$1; shift
	local ecn

	for ecn in 0 1 2 3; do
		do_test_pedit_dsfield "$locus"				\
				      "ip dsfield set $ecn retain 0x03"	\
				      ip "ip_tos $((124 | $ecn))"	\
				      "-A 192.0.2.1 -B 192.0.2.2"
	done
}

test_ip_ecn()
{
	do_test_ip_ecn "dev $swp1 ingress"
	do_test_ip_ecn "dev $swp2 egress"
}

do_test_ip_dscp_ecn()
{
	local locus=$1; shift

	tc filter add $locus handle 101 pref 1				\
	   flower action pedit ex munge ip dsfield set 124 retain 0xfc	\
		  action pedit ex munge ip dsfield set 1 retain 0x03
	tc filter add dev $h2 ingress handle 101 pref 1 prot ip		\
	   flower skip_hw ip_tos 125 action pass

	do_test_pedit_dsfield_common "$locus" "set DSCP + set ECN"	\
				      "-A 192.0.2.1 -B 192.0.2.2"

	tc filter del dev $h2 ingress pref 1
	tc filter del $locus pref 1
}

test_ip_dscp_ecn()
{
	do_test_ip_dscp_ecn "dev $swp1 ingress"
	do_test_ip_dscp_ecn "dev $swp2 egress"
}

do_test_ip6_dsfield()
{
	local locus=$1; shift
	local dsfield

	for dsfield in 0 1 2 3 128 252 253 254 255; do
		do_test_pedit_dsfield "$locus"				\
				  "ip6 traffic_class set $dsfield"	\
				  ipv6 "ip_tos $dsfield"		\
				  "-6 -A 2001:db8:1::1 -B 2001:db8:1::2"
	done
}

test_ip6_dsfield()
{
	do_test_ip6_dsfield "dev $swp1 ingress"
	do_test_ip6_dsfield "dev $swp2 egress"
}

do_test_ip6_dscp()
{
	local locus=$1; shift
	local dscp

	for dscp in 0 1 2 3 32 61 62 63; do
		do_test_pedit_dsfield "$locus"				       \
			    "ip6 traffic_class set $((dscp << 2)) retain 0xfc" \
			    ipv6 "ip_tos $(((dscp << 2) | 1))"		       \
			    "-6 -A 2001:db8:1::1 -B 2001:db8:1::2"
	done
}

test_ip6_dscp()
{
	do_test_ip6_dscp "dev $swp1 ingress"
	do_test_ip6_dscp "dev $swp2 egress"
}

do_test_ip6_ecn()
{
	local locus=$1; shift
	local ecn

	for ecn in 0 1 2 3; do
		do_test_pedit_dsfield "$locus"				\
				"ip6 traffic_class set $ecn retain 0x3"	\
				ipv6 "ip_tos $((124 | $ecn))"		\
				"-6 -A 2001:db8:1::1 -B 2001:db8:1::2"
	done
}

test_ip6_ecn()
{
	do_test_ip6_ecn "dev $swp1 ingress"
	do_test_ip6_ecn "dev $swp2 egress"
}

trap cleanup EXIT

setup_prepare
setup_wait

tests_run

exit $EXIT_STATUS
