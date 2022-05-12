#!/bin/bash
# SPDX-License-Identifier: GPL-2.0

rndh=$(printf %x $sec)-$(mktemp -u XXXXXX)
ns1="ns1-$rndh"
ns2="ns2-$rndh"
ns3="ns3-$rndh"
capture=false
ksft_skip=4
timeout_poll=30
timeout_test=$((timeout_poll * 2 + 1))
test_cnt=1
ret=0
bail=0

usage() {
	echo "Usage: $0 [ -b ] [ -c ] [ -d ]"
	echo -e "\t-b: bail out after first error, otherwise runs al testcases"
	echo -e "\t-c: capture packets for each test using tcpdump (default: no capture)"
	echo -e "\t-d: debug this script"
}

cleanup()
{
	rm -f "$cin" "$cout"
	rm -f "$sin" "$sout"
	rm -f "$capout"

	local netns
	for netns in "$ns1" "$ns2" "$ns3";do
		ip netns del $netns
	done
}

ip -Version > /dev/null 2>&1
if [ $? -ne 0 ];then
	echo "SKIP: Could not run test without ip tool"
	exit $ksft_skip
fi

#  "$ns1"              ns2                    ns3
#     ns1eth1    ns2eth1   ns2eth3      ns3eth1
#            netem
#     ns1eth2    ns2eth2
#            netem

setup()
{
	large=$(mktemp)
	small=$(mktemp)
	sout=$(mktemp)
	cout=$(mktemp)
	capout=$(mktemp)
	size=$((2048 * 4096))
	dd if=/dev/zero of=$small bs=4096 count=20 >/dev/null 2>&1
	dd if=/dev/zero of=$large bs=4096 count=$((size / 4096)) >/dev/null 2>&1

	trap cleanup EXIT

	for i in "$ns1" "$ns2" "$ns3";do
		ip netns add $i || exit $ksft_skip
		ip -net $i link set lo up
	done

	ip link add ns1eth1 netns "$ns1" type veth peer name ns2eth1 netns "$ns2"
	ip link add ns1eth2 netns "$ns1" type veth peer name ns2eth2 netns "$ns2"
	ip link add ns2eth3 netns "$ns2" type veth peer name ns3eth1 netns "$ns3"

	ip -net "$ns1" addr add 10.0.1.1/24 dev ns1eth1
	ip -net "$ns1" addr add dead:beef:1::1/64 dev ns1eth1 nodad
	ip -net "$ns1" link set ns1eth1 up mtu 1500
	ip -net "$ns1" route add default via 10.0.1.2
	ip -net "$ns1" route add default via dead:beef:1::2

	ip -net "$ns1" addr add 10.0.2.1/24 dev ns1eth2
	ip -net "$ns1" addr add dead:beef:2::1/64 dev ns1eth2 nodad
	ip -net "$ns1" link set ns1eth2 up mtu 1500
	ip -net "$ns1" route add default via 10.0.2.2 metric 101
	ip -net "$ns1" route add default via dead:beef:2::2 metric 101

	ip netns exec "$ns1" ./pm_nl_ctl limits 1 1
	ip netns exec "$ns1" ./pm_nl_ctl add 10.0.2.1 dev ns1eth2 flags subflow
	ip netns exec "$ns1" sysctl -q net.ipv4.conf.all.rp_filter=0

	ip -net "$ns2" addr add 10.0.1.2/24 dev ns2eth1
	ip -net "$ns2" addr add dead:beef:1::2/64 dev ns2eth1 nodad
	ip -net "$ns2" link set ns2eth1 up mtu 1500

	ip -net "$ns2" addr add 10.0.2.2/24 dev ns2eth2
	ip -net "$ns2" addr add dead:beef:2::2/64 dev ns2eth2 nodad
	ip -net "$ns2" link set ns2eth2 up mtu 1500

	ip -net "$ns2" addr add 10.0.3.2/24 dev ns2eth3
	ip -net "$ns2" addr add dead:beef:3::2/64 dev ns2eth3 nodad
	ip -net "$ns2" link set ns2eth3 up mtu 1500
	ip netns exec "$ns2" sysctl -q net.ipv4.ip_forward=1
	ip netns exec "$ns2" sysctl -q net.ipv6.conf.all.forwarding=1

	ip -net "$ns3" addr add 10.0.3.3/24 dev ns3eth1
	ip -net "$ns3" addr add dead:beef:3::3/64 dev ns3eth1 nodad
	ip -net "$ns3" link set ns3eth1 up mtu 1500
	ip -net "$ns3" route add default via 10.0.3.2
	ip -net "$ns3" route add default via dead:beef:3::2

	ip netns exec "$ns3" ./pm_nl_ctl limits 1 1
}

# $1: ns, $2: port
wait_local_port_listen()
{
	local listener_ns="${1}"
	local port="${2}"

	local port_hex i

	port_hex="$(printf "%04X" "${port}")"
	for i in $(seq 10); do
		ip netns exec "${listener_ns}" cat /proc/net/tcp* | \
			awk "BEGIN {rc=1} {if (\$2 ~ /:${port_hex}\$/ && \$4 ~ /0A/) {rc=0; exit}} END {exit rc}" &&
			break
		sleep 0.1
	done
}

do_transfer()
{
	local cin=$1
	local sin=$2
	local max_time=$3
	local port
	port=$((10000+$test_cnt))
	test_cnt=$((test_cnt+1))

	:> "$cout"
	:> "$sout"
	:> "$capout"

	local addr_port
	addr_port=$(printf "%s:%d" ${connect_addr} ${port})

	if $capture; then
		local capuser
		if [ -z $SUDO_USER ] ; then
			capuser=""
		else
			capuser="-Z $SUDO_USER"
		fi

		local capfile="${rndh}-${port}"
		local capopt="-i any -s 65535 -B 32768 ${capuser}"

		ip netns exec ${ns3}  tcpdump ${capopt} -w "${capfile}-listener.pcap"  >> "${capout}" 2>&1 &
		local cappid_listener=$!

		ip netns exec ${ns1} tcpdump ${capopt} -w "${capfile}-connector.pcap" >> "${capout}" 2>&1 &
		local cappid_connector=$!

		sleep 1
	fi

	timeout ${timeout_test} \
		ip netns exec ${ns3} \
			./mptcp_connect -jt ${timeout_poll} -l -p $port \
				0.0.0.0 < "$sin" > "$sout" &
	local spid=$!

	wait_local_port_listen "${ns3}" "${port}"

	local start
	start=$(date +%s%3N)
	timeout ${timeout_test} \
		ip netns exec ${ns1} \
			./mptcp_connect -jt ${timeout_poll} -p $port \
				10.0.3.3 < "$cin" > "$cout" &
	local cpid=$!

	wait $cpid
	local retc=$?
	wait $spid
	local rets=$?

	local stop
	stop=$(date +%s%3N)

	if $capture; then
		sleep 1
		kill ${cappid_listener}
		kill ${cappid_connector}
	fi

	local duration
	duration=$((stop-start))

	cmp $sin $cout > /dev/null 2>&1
	local cmps=$?
	cmp $cin $sout > /dev/null 2>&1
	local cmpc=$?

	printf "%16s" "$duration max $max_time "
	if [ $retc -eq 0 ] && [ $rets -eq 0 ] && \
	   [ $cmpc -eq 0 ] && [ $cmps -eq 0 ] && \
	   [ $duration -lt $max_time ]; then
		echo "[ OK ]"
		cat "$capout"
		return 0
	fi

	echo " [ fail ]"
	echo "client exit code $retc, server $rets" 1>&2
	echo -e "\nnetns ${ns3} socket stat for $port:" 1>&2
	ip netns exec ${ns3} ss -nita 1>&2 -o "sport = :$port"
	echo -e "\nnetns ${ns1} socket stat for $port:" 1>&2
	ip netns exec ${ns1} ss -nita 1>&2 -o "dport = :$port"
	ls -l $sin $cout
	ls -l $cin $sout

	cat "$capout"
	return 1
}

run_test()
{
	local rate1=$1
	local rate2=$2
	local delay1=$3
	local delay2=$4
	local lret
	local dev
	shift 4
	local msg=$*

	[ $delay1 -gt 0 ] && delay1="delay $delay1" || delay1=""
	[ $delay2 -gt 0 ] && delay2="delay $delay2" || delay2=""

	for dev in ns1eth1 ns1eth2; do
		tc -n $ns1 qdisc del dev $dev root >/dev/null 2>&1
	done
	for dev in ns2eth1 ns2eth2; do
		tc -n $ns2 qdisc del dev $dev root >/dev/null 2>&1
	done
	tc -n $ns1 qdisc add dev ns1eth1 root netem rate ${rate1}mbit $delay1
	tc -n $ns1 qdisc add dev ns1eth2 root netem rate ${rate2}mbit $delay2
	tc -n $ns2 qdisc add dev ns2eth1 root netem rate ${rate1}mbit $delay1
	tc -n $ns2 qdisc add dev ns2eth2 root netem rate ${rate2}mbit $delay2

	# time is measure in ms
	local time=$((size * 8 * 1000 / (( $rate1 + $rate2) * 1024 *1024) ))

	# mptcp_connect will do some sleeps to allow the mp_join handshake
	# completion
	time=$((time + 1350))

	printf "%-50s" "$msg"
	do_transfer $small $large $((time * 11 / 10))
	lret=$?
	if [ $lret -ne 0 ]; then
		ret=$lret
		[ $bail -eq 0 ] || exit $ret
	fi

	printf "%-50s" "$msg - reverse direction"
	do_transfer $large $small $((time * 11 / 10))
	lret=$?
	if [ $lret -ne 0 ]; then
		ret=$lret
		[ $bail -eq 0 ] || exit $ret
	fi
}

while getopts "bcdh" option;do
	case "$option" in
	"h")
		usage $0
		exit 0
		;;
	"b")
		bail=1
		;;
	"c")
		capture=true
		;;
	"d")
		set -x
		;;
	"?")
		usage $0
		exit 1
		;;
	esac
done

setup
run_test 10 10 0 0 "balanced bwidth"
run_test 10 10 1 50 "balanced bwidth with unbalanced delay"

# we still need some additional infrastructure to pass the following test-cases
run_test 30 10 0 0 "unbalanced bwidth"
run_test 30 10 1 50 "unbalanced bwidth with unbalanced delay"
run_test 30 10 50 1 "unbalanced bwidth with opposed, unbalanced delay"
exit $ret
