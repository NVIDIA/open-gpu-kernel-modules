#!/bin/bash
# SPDX-License-Identifier: GPL-2.0

# Test traffic distribution when a wECMP route forwards traffic to two GRE
# tunnels.
#
# +-------------------------+
# | H1                      |
# |               $h1 +     |
# |      192.0.2.1/28 |     |
# +-------------------|-----+
#                     |
# +-------------------|------------------------+
# | SW1               |                        |
# |              $ol1 +                        |
# |      192.0.2.2/28                          |
# |                                            |
# |  + g1a (gre)          + g1b (gre)          |
# |    loc=192.0.2.65       loc=192.0.2.81     |
# |    rem=192.0.2.66 --.   rem=192.0.2.82 --. |
# |    tos=inherit      |   tos=inherit      | |
# |  .------------------'                    | |
# |  |                    .------------------' |
# |  v                    v                    |
# |  + $ul1.111 (vlan)    + $ul1.222 (vlan)    |
# |  | 192.0.2.129/28     | 192.0.2.145/28     |
# |   \                  /                     |
# |    \________________/                      |
# |            |                               |
# |            + $ul1                          |
# +------------|-------------------------------+
#              |
# +------------|-------------------------------+
# | SW2        + $ul2                          |
# |     _______|________                       |
# |    /                \                      |
# |   /                  \                     |
# |  + $ul2.111 (vlan)    + $ul2.222 (vlan)    |
# |  ^ 192.0.2.130/28     ^ 192.0.2.146/28     |
# |  |                    |                    |
# |  |                    '------------------. |
# |  '------------------.                    | |
# |  + g2a (gre)        | + g2b (gre)        | |
# |    loc=192.0.2.66   |   loc=192.0.2.82   | |
# |    rem=192.0.2.65 --'   rem=192.0.2.81 --' |
# |    tos=inherit          tos=inherit        |
# |                                            |
# |              $ol2 +                        |
# |     192.0.2.17/28 |                        |
# +-------------------|------------------------+
#                     |
# +-------------------|-----+
# | H2                |     |
# |               $h2 +     |
# |     192.0.2.18/28       |
# +-------------------------+

ALL_TESTS="
	ping_ipv4
	multipath_ipv4
"

NUM_NETIFS=6
source lib.sh

h1_create()
{
	simple_if_init $h1 192.0.2.1/28 2001:db8:1::1/64
	ip route add vrf v$h1 192.0.2.16/28 via 192.0.2.2
}

h1_destroy()
{
	ip route del vrf v$h1 192.0.2.16/28 via 192.0.2.2
	simple_if_fini $h1 192.0.2.1/28
}

sw1_create()
{
	simple_if_init $ol1 192.0.2.2/28
	__simple_if_init $ul1 v$ol1
	vlan_create $ul1 111 v$ol1 192.0.2.129/28
	vlan_create $ul1 222 v$ol1 192.0.2.145/28

	tunnel_create g1a gre 192.0.2.65 192.0.2.66 tos inherit dev v$ol1
	__simple_if_init g1a v$ol1 192.0.2.65/32
	ip route add vrf v$ol1 192.0.2.66/32 via 192.0.2.130

	tunnel_create g1b gre 192.0.2.81 192.0.2.82 tos inherit dev v$ol1
	__simple_if_init g1b v$ol1 192.0.2.81/32
	ip route add vrf v$ol1 192.0.2.82/32 via 192.0.2.146

	ip route add vrf v$ol1 192.0.2.16/28 \
	   nexthop dev g1a \
	   nexthop dev g1b
}

sw1_destroy()
{
	ip route del vrf v$ol1 192.0.2.16/28

	ip route del vrf v$ol1 192.0.2.82/32 via 192.0.2.146
	__simple_if_fini g1b 192.0.2.81/32
	tunnel_destroy g1b

	ip route del vrf v$ol1 192.0.2.66/32 via 192.0.2.130
	__simple_if_fini g1a 192.0.2.65/32
	tunnel_destroy g1a

	vlan_destroy $ul1 222
	vlan_destroy $ul1 111
	__simple_if_fini $ul1
	simple_if_fini $ol1 192.0.2.2/28
}

sw2_create()
{
	simple_if_init $ol2 192.0.2.17/28
	__simple_if_init $ul2 v$ol2
	vlan_create $ul2 111 v$ol2 192.0.2.130/28
	vlan_create $ul2 222 v$ol2 192.0.2.146/28

	tunnel_create g2a gre 192.0.2.66 192.0.2.65 tos inherit dev v$ol2
	__simple_if_init g2a v$ol2 192.0.2.66/32
	ip route add vrf v$ol2 192.0.2.65/32 via 192.0.2.129

	tunnel_create g2b gre 192.0.2.82 192.0.2.81 tos inherit dev v$ol2
	__simple_if_init g2b v$ol2 192.0.2.82/32
	ip route add vrf v$ol2 192.0.2.81/32 via 192.0.2.145

	ip route add vrf v$ol2 192.0.2.0/28 \
	   nexthop dev g2a \
	   nexthop dev g2b

	tc qdisc add dev $ul2 clsact
	tc filter add dev $ul2 ingress pref 111 prot 802.1Q \
	   flower vlan_id 111 action pass
	tc filter add dev $ul2 ingress pref 222 prot 802.1Q \
	   flower vlan_id 222 action pass
}

sw2_destroy()
{
	tc qdisc del dev $ul2 clsact

	ip route del vrf v$ol2 192.0.2.0/28

	ip route del vrf v$ol2 192.0.2.81/32 via 192.0.2.145
	__simple_if_fini g2b 192.0.2.82/32
	tunnel_destroy g2b

	ip route del vrf v$ol2 192.0.2.65/32 via 192.0.2.129
	__simple_if_fini g2a 192.0.2.66/32
	tunnel_destroy g2a

	vlan_destroy $ul2 222
	vlan_destroy $ul2 111
	__simple_if_fini $ul2
	simple_if_fini $ol2 192.0.2.17/28
}

h2_create()
{
	simple_if_init $h2 192.0.2.18/28
	ip route add vrf v$h2 192.0.2.0/28 via 192.0.2.17
}

h2_destroy()
{
	ip route del vrf v$h2 192.0.2.0/28 via 192.0.2.17
	simple_if_fini $h2 192.0.2.18/28
}

setup_prepare()
{
	h1=${NETIFS[p1]}
	ol1=${NETIFS[p2]}

	ul1=${NETIFS[p3]}
	ul2=${NETIFS[p4]}

	ol2=${NETIFS[p5]}
	h2=${NETIFS[p6]}

	vrf_prepare
	h1_create
	sw1_create
	sw2_create
	h2_create

	forwarding_enable
}

cleanup()
{
	pre_cleanup

	forwarding_restore

	h2_destroy
	sw2_destroy
	sw1_destroy
	h1_destroy
	vrf_cleanup
}

multipath4_test()
{
	local what=$1; shift
	local weight1=$1; shift
	local weight2=$1; shift

	sysctl_set net.ipv4.fib_multipath_hash_policy 1
	ip route replace vrf v$ol1 192.0.2.16/28 \
	   nexthop dev g1a weight $weight1 \
	   nexthop dev g1b weight $weight2

	local t0_111=$(tc_rule_stats_get $ul2 111 ingress)
	local t0_222=$(tc_rule_stats_get $ul2 222 ingress)

	ip vrf exec v$h1 \
	   $MZ $h1 -q -p 64 -A 192.0.2.1 -B 192.0.2.18 \
	       -d 1msec -t udp "sp=1024,dp=0-32768"

	local t1_111=$(tc_rule_stats_get $ul2 111 ingress)
	local t1_222=$(tc_rule_stats_get $ul2 222 ingress)

	local d111=$((t1_111 - t0_111))
	local d222=$((t1_222 - t0_222))
	multipath_eval "$what" $weight1 $weight2 $d111 $d222

	ip route replace vrf v$ol1 192.0.2.16/28 \
	   nexthop dev g1a \
	   nexthop dev g1b
	sysctl_restore net.ipv4.fib_multipath_hash_policy
}

ping_ipv4()
{
	ping_test $h1 192.0.2.18
}

multipath_ipv4()
{
	log_info "Running IPv4 multipath tests"
	multipath4_test "ECMP" 1 1
	multipath4_test "Weighted MP 2:1" 2 1
	multipath4_test "Weighted MP 11:45" 11 45
}

trap cleanup EXIT

setup_prepare
setup_wait
tests_run

exit $EXIT_STATUS
