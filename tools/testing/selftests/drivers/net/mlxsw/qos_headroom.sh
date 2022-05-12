#!/bin/bash
# SPDX-License-Identifier: GPL-2.0

ALL_TESTS="
	test_defaults
	test_dcb_ets
	test_mtu
	test_pfc
	test_int_buf
	test_tc_priomap
	test_tc_mtu
	test_tc_sizes
	test_tc_int_buf
"

lib_dir=$(dirname $0)/../../../net/forwarding

NUM_NETIFS=0
source $lib_dir/lib.sh
source $lib_dir/devlink_lib.sh
source qos_lib.sh

swp=$NETIF_NO_CABLE

cleanup()
{
	pre_cleanup
}

get_prio_pg()
{
	__mlnx_qos -i $swp | sed -n '/^PFC/,/^[^[:space:]]/p' |
		grep buffer | sed 's/ \+/ /g' | cut -d' ' -f 2-
}

get_prio_pfc()
{
	__mlnx_qos -i $swp | sed -n '/^PFC/,/^[^[:space:]]/p' |
		grep enabled | sed 's/ \+/ /g' | cut -d' ' -f 2-
}

get_prio_tc()
{
	__mlnx_qos -i $swp | sed -n '/^tc/,$p' |
		awk '/^tc/ { TC = $2 }
		     /priority:/ { PRIO[$2]=TC }
		     END {
			for (i in PRIO)
			    printf("%d ", PRIO[i])
		     }'
}

get_buf_size()
{
	local idx=$1; shift

	__mlnx_qos -i $swp | grep Receive | sed 's/.*: //' | cut -d, -f $((idx + 1))
}

get_tot_size()
{
	__mlnx_qos -i $swp | grep Receive | sed 's/.*total_size=//'
}

check_prio_pg()
{
	local expect=$1; shift

	local current=$(get_prio_pg)
	test "$current" = "$expect"
	check_err $? "prio2buffer is '$current', expected '$expect'"
}

check_prio_pfc()
{
	local expect=$1; shift

	local current=$(get_prio_pfc)
	test "$current" = "$expect"
	check_err $? "prio PFC is '$current', expected '$expect'"
}

check_prio_tc()
{
	local expect=$1; shift

	local current=$(get_prio_tc)
	test "$current" = "$expect"
	check_err $? "prio_tc is '$current', expected '$expect'"
}

__check_buf_size()
{
	local idx=$1; shift
	local expr=$1; shift
	local what=$1; shift

	local current=$(get_buf_size $idx)
	((current $expr))
	check_err $? "${what}buffer $idx size is '$current', expected '$expr'"
	echo $current
}

check_buf_size()
{
	__check_buf_size "$@" > /dev/null
}

test_defaults()
{
	RET=0

	check_prio_pg "0 0 0 0 0 0 0 0 "
	check_prio_tc "0 0 0 0 0 0 0 0 "
	check_prio_pfc "0 0 0 0 0 0 0 0 "

	log_test "Default headroom configuration"
}

test_dcb_ets()
{
	RET=0

	__mlnx_qos -i $swp --prio_tc=0,2,4,6,1,3,5,7 > /dev/null

	check_prio_pg "0 2 4 6 1 3 5 7 "
	check_prio_tc "0 2 4 6 1 3 5 7 "
	check_prio_pfc "0 0 0 0 0 0 0 0 "

	__mlnx_qos -i $swp --prio_tc=0,0,0,0,0,0,0,0 > /dev/null

	check_prio_pg "0 0 0 0 0 0 0 0 "
	check_prio_tc "0 0 0 0 0 0 0 0 "

	__mlnx_qos -i $swp --prio2buffer=1,3,5,7,0,2,4,6 &> /dev/null
	check_fail $? "prio2buffer accepted in DCB mode"

	log_test "Configuring headroom through ETS"
}

test_mtu()
{
	local what=$1; shift
	local buf0size_2
	local buf0size

	RET=0
	buf0size=$(__check_buf_size 0 "> 0")

	mtu_set $swp 3000
	buf0size_2=$(__check_buf_size 0 "> $buf0size" "MTU 3000: ")
	mtu_restore $swp

	mtu_set $swp 6000
	check_buf_size 0 "> $buf0size_2" "MTU 6000: "
	mtu_restore $swp

	check_buf_size 0 "== $buf0size"

	log_test "${what}MTU impacts buffer size"
}

test_tc_mtu()
{
	# In TC mode, MTU still impacts the threshold below which a buffer is
	# not permitted to go.

	tc qdisc replace dev $swp root handle 1: bfifo limit 1.5M
	test_mtu "TC: "
	tc qdisc delete dev $swp root
}

test_pfc()
{
	RET=0

	__mlnx_qos -i $swp --prio_tc=0,0,0,0,0,1,2,3 > /dev/null

	local buf0size=$(get_buf_size 0)
	local buf1size=$(get_buf_size 1)
	local buf2size=$(get_buf_size 2)
	local buf3size=$(get_buf_size 3)
	check_buf_size 0 "> 0"
	check_buf_size 1 "> 0"
	check_buf_size 2 "> 0"
	check_buf_size 3 "> 0"
	check_buf_size 4 "== 0"
	check_buf_size 5 "== 0"
	check_buf_size 6 "== 0"
	check_buf_size 7 "== 0"

	log_test "Buffer size sans PFC"

	RET=0

	__mlnx_qos -i $swp --pfc=0,0,0,0,0,1,1,1 --cable_len=0 > /dev/null

	check_prio_pg "0 0 0 0 0 1 2 3 "
	check_prio_pfc "0 0 0 0 0 1 1 1 "
	check_buf_size 0 "== $buf0size"
	check_buf_size 1 "> $buf1size"
	check_buf_size 2 "> $buf2size"
	check_buf_size 3 "> $buf3size"

	local buf1size=$(get_buf_size 1)
	check_buf_size 2 "== $buf1size"
	check_buf_size 3 "== $buf1size"

	log_test "PFC: Cable length 0"

	RET=0

	__mlnx_qos -i $swp --pfc=0,0,0,0,0,1,1,1 --cable_len=1000 > /dev/null

	check_buf_size 0 "== $buf0size"
	check_buf_size 1 "> $buf1size"
	check_buf_size 2 "> $buf1size"
	check_buf_size 3 "> $buf1size"

	log_test "PFC: Cable length 1000"

	RET=0

	__mlnx_qos -i $swp --pfc=0,0,0,0,0,0,0,0 --cable_len=0 > /dev/null
	__mlnx_qos -i $swp --prio_tc=0,0,0,0,0,0,0,0 > /dev/null

	check_prio_pg "0 0 0 0 0 0 0 0 "
	check_prio_tc "0 0 0 0 0 0 0 0 "
	check_buf_size 0 "> 0"
	check_buf_size 1 "== 0"
	check_buf_size 2 "== 0"
	check_buf_size 3 "== 0"
	check_buf_size 4 "== 0"
	check_buf_size 5 "== 0"
	check_buf_size 6 "== 0"
	check_buf_size 7 "== 0"

	log_test "PFC: Restore defaults"
}

test_tc_priomap()
{
	RET=0

	__mlnx_qos -i $swp --prio_tc=0,1,2,3,4,5,6,7 > /dev/null
	check_prio_pg "0 1 2 3 4 5 6 7 "

	tc qdisc replace dev $swp root handle 1: bfifo limit 1.5M
	check_prio_pg "0 0 0 0 0 0 0 0 "

	__mlnx_qos -i $swp --prio2buffer=1,3,5,7,0,2,4,6 > /dev/null
	check_prio_pg "1 3 5 7 0 2 4 6 "

	tc qdisc delete dev $swp root
	check_prio_pg "0 1 2 3 4 5 6 7 "

	# Clean up.
	tc qdisc replace dev $swp root handle 1: bfifo limit 1.5M
	__mlnx_qos -i $swp --prio2buffer=0,0,0,0,0,0,0,0 > /dev/null
	tc qdisc delete dev $swp root
	__mlnx_qos -i $swp --prio_tc=0,0,0,0,0,0,0,0 > /dev/null

	log_test "TC: priomap"
}

test_tc_sizes()
{
	local cell_size=$(devlink_cell_size_get)
	local size=$((cell_size * 1000))

	RET=0

	__mlnx_qos -i $swp --buffer_size=$size,0,0,0,0,0,0,0 &> /dev/null
	check_fail $? "buffer_size should fail before qdisc is added"

	tc qdisc replace dev $swp root handle 1: bfifo limit 1.5M

	__mlnx_qos -i $swp --buffer_size=$size,0,0,0,0,0,0,0 > /dev/null
	check_err $? "buffer_size should pass after qdisc is added"
	check_buf_size 0 "== $size" "set size: "

	mtu_set $swp 6000
	check_buf_size 0 "== $size" "set MTU: "
	mtu_restore $swp

	__mlnx_qos -i $swp --buffer_size=0,0,0,0,0,0,0,0 > /dev/null

	# After replacing the qdisc for the same kind, buffer_size still has to
	# work.
	tc qdisc replace dev $swp root handle 1: bfifo limit 1M

	__mlnx_qos -i $swp --buffer_size=$size,0,0,0,0,0,0,0 > /dev/null
	check_buf_size 0 "== $size" "post replace, set size: "

	__mlnx_qos -i $swp --buffer_size=0,0,0,0,0,0,0,0 > /dev/null

	# Likewise after replacing for a different kind.
	tc qdisc replace dev $swp root handle 2: prio bands 8

	__mlnx_qos -i $swp --buffer_size=$size,0,0,0,0,0,0,0 > /dev/null
	check_buf_size 0 "== $size" "post replace different kind, set size: "

	tc qdisc delete dev $swp root

	__mlnx_qos -i $swp --buffer_size=$size,0,0,0,0,0,0,0 &> /dev/null
	check_fail $? "buffer_size should fail after qdisc is deleted"

	log_test "TC: buffer size"
}

test_int_buf()
{
	local what=$1; shift

	RET=0

	local buf0size=$(get_buf_size 0)
	local tot_size=$(get_tot_size)

	# Size of internal buffer and buffer 9.
	local dsize=$((tot_size - buf0size))

	tc qdisc add dev $swp clsact
	tc filter add dev $swp egress matchall skip_sw action mirred egress mirror dev $swp

	local buf0size_2=$(get_buf_size 0)
	local tot_size_2=$(get_tot_size)
	local dsize_2=$((tot_size_2 - buf0size_2))

	# Egress SPAN should have added to the "invisible" buffer configuration.
	((dsize_2 > dsize))
	check_err $? "Invisible buffers account for '$dsize_2', expected '> $dsize'"

	mtu_set $swp 3000

	local buf0size_3=$(get_buf_size 0)
	local tot_size_3=$(get_tot_size)
	local dsize_3=$((tot_size_3 - buf0size_3))

	# MTU change might change buffer 0, which will show at total, but the
	# hidden buffers should stay the same size.
	((dsize_3 == dsize_2))
	check_err $? "MTU change: Invisible buffers account for '$dsize_3', expected '== $dsize_2'"

	mtu_restore $swp
	tc qdisc del dev $swp clsact

	# After SPAN removal, hidden buffers should be back to the original sizes.
	local buf0size_4=$(get_buf_size 0)
	local tot_size_4=$(get_tot_size)
	local dsize_4=$((tot_size_4 - buf0size_4))
	((dsize_4 == dsize))
	check_err $? "SPAN removed: Invisible buffers account for '$dsize_4', expected '== $dsize'"

	log_test "${what}internal buffer size"
}

test_tc_int_buf()
{
	local cell_size=$(devlink_cell_size_get)
	local size=$((cell_size * 1000))

	tc qdisc replace dev $swp root handle 1: bfifo limit 1.5M
	test_int_buf "TC: "

	__mlnx_qos -i $swp --buffer_size=$size,0,0,0,0,0,0,0 > /dev/null
	test_int_buf "TC+buffsize: "

	__mlnx_qos -i $swp --buffer_size=0,0,0,0,0,0,0,0 > /dev/null
	tc qdisc delete dev $swp root
}

trap cleanup EXIT

bail_on_lldpad
setup_wait
tests_run

exit $EXIT_STATUS
