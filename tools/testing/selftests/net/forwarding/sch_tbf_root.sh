#!/bin/bash
# SPDX-License-Identifier: GPL-2.0

ALL_TESTS="
	ping_ipv4
	tbf_test
"
: ${lib_dir:=.}
source $lib_dir/sch_tbf_core.sh

tbf_test_one()
{
	local bs=$1; shift

	tc qdisc replace dev $swp2 root handle 108: tbf \
	   rate 400Mbit burst $bs limit 1M
	do_tbf_test 10 400 $bs
}

tbf_test()
{
	tbf_test_one 128K
	tc qdisc del dev $swp2 root
}

trap cleanup EXIT

setup_prepare
setup_wait

tests_run

exit $EXIT_STATUS
