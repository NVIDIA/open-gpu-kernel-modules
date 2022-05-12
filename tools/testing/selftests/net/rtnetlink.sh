#!/bin/bash
#
# This test is for checking rtnetlink callpaths, and get as much coverage as possible.
#
# set -e

devdummy="test-dummy0"

# Kselftest framework requirement - SKIP code is 4.
ksft_skip=4

# set global exit status, but never reset nonzero one.
check_err()
{
	if [ $ret -eq 0 ]; then
		ret=$1
	fi
}

# same but inverted -- used when command must fail for test to pass
check_fail()
{
	if [ $1 -eq 0 ]; then
		ret=1
	fi
}

kci_add_dummy()
{
	ip link add name "$devdummy" type dummy
	check_err $?
	ip link set "$devdummy" up
	check_err $?
}

kci_del_dummy()
{
	ip link del dev "$devdummy"
	check_err $?
}

kci_test_netconf()
{
	dev="$1"
	r=$ret

	ip netconf show dev "$dev" > /dev/null
	check_err $?

	for f in 4 6; do
		ip -$f netconf show dev "$dev" > /dev/null
		check_err $?
	done

	if [ $ret -ne 0 ] ;then
		echo "FAIL: ip netconf show $dev"
		test $r -eq 0 && ret=0
		return 1
	fi
}

# add a bridge with vlans on top
kci_test_bridge()
{
	devbr="test-br0"
	vlandev="testbr-vlan1"

	local ret=0
	ip link add name "$devbr" type bridge
	check_err $?

	ip link set dev "$devdummy" master "$devbr"
	check_err $?

	ip link set "$devbr" up
	check_err $?

	ip link add link "$devbr" name "$vlandev" type vlan id 1
	check_err $?
	ip addr add dev "$vlandev" 10.200.7.23/30
	check_err $?
	ip -6 addr add dev "$vlandev" dead:42::1234/64
	check_err $?
	ip -d link > /dev/null
	check_err $?
	ip r s t all > /dev/null
	check_err $?

	for name in "$devbr" "$vlandev" "$devdummy" ; do
		kci_test_netconf "$name"
	done

	ip -6 addr del dev "$vlandev" dead:42::1234/64
	check_err $?

	ip link del dev "$vlandev"
	check_err $?
	ip link del dev "$devbr"
	check_err $?

	if [ $ret -ne 0 ];then
		echo "FAIL: bridge setup"
		return 1
	fi
	echo "PASS: bridge setup"

}

kci_test_gre()
{
	gredev=neta
	rem=10.42.42.1
	loc=10.0.0.1

	local ret=0
	ip tunnel add $gredev mode gre remote $rem local $loc ttl 1
	check_err $?
	ip link set $gredev up
	check_err $?
	ip addr add 10.23.7.10 dev $gredev
	check_err $?
	ip route add 10.23.8.0/30 dev $gredev
	check_err $?
	ip addr add dev "$devdummy" 10.23.7.11/24
	check_err $?
	ip link > /dev/null
	check_err $?
	ip addr > /dev/null
	check_err $?

	kci_test_netconf "$gredev"

	ip addr del dev "$devdummy" 10.23.7.11/24
	check_err $?

	ip link del $gredev
	check_err $?

	if [ $ret -ne 0 ];then
		echo "FAIL: gre tunnel endpoint"
		return 1
	fi
	echo "PASS: gre tunnel endpoint"
}

# tc uses rtnetlink too, for full tc testing
# please see tools/testing/selftests/tc-testing.
kci_test_tc()
{
	dev=lo
	local ret=0

	tc qdisc add dev "$dev" root handle 1: htb
	check_err $?
	tc class add dev "$dev" parent 1: classid 1:10 htb rate 1mbit
	check_err $?
	tc filter add dev "$dev" parent 1:0 prio 5 handle ffe: protocol ip u32 divisor 256
	check_err $?
	tc filter add dev "$dev" parent 1:0 prio 5 handle ffd: protocol ip u32 divisor 256
	check_err $?
	tc filter add dev "$dev" parent 1:0 prio 5 handle ffc: protocol ip u32 divisor 256
	check_err $?
	tc filter add dev "$dev" protocol ip parent 1: prio 5 handle ffe:2:3 u32 ht ffe:2: match ip src 10.0.0.3 flowid 1:10
	check_err $?
	tc filter add dev "$dev" protocol ip parent 1: prio 5 handle ffe:2:2 u32 ht ffe:2: match ip src 10.0.0.2 flowid 1:10
	check_err $?
	tc filter show dev "$dev" parent  1:0 > /dev/null
	check_err $?
	tc filter del dev "$dev" protocol ip parent 1: prio 5 handle ffe:2:3 u32
	check_err $?
	tc filter show dev "$dev" parent  1:0 > /dev/null
	check_err $?
	tc qdisc del dev "$dev" root handle 1: htb
	check_err $?

	if [ $ret -ne 0 ];then
		echo "FAIL: tc htb hierarchy"
		return 1
	fi
	echo "PASS: tc htb hierarchy"

}

kci_test_polrouting()
{
	local ret=0
	ip rule add fwmark 1 lookup 100
	check_err $?
	ip route add local 0.0.0.0/0 dev lo table 100
	check_err $?
	ip r s t all > /dev/null
	check_err $?
	ip rule del fwmark 1 lookup 100
	check_err $?
	ip route del local 0.0.0.0/0 dev lo table 100
	check_err $?

	if [ $ret -ne 0 ];then
		echo "FAIL: policy route test"
		return 1
	fi
	echo "PASS: policy routing"
}

kci_test_route_get()
{
	local hash_policy=$(sysctl -n net.ipv4.fib_multipath_hash_policy)

	local ret=0

	ip route get 127.0.0.1 > /dev/null
	check_err $?
	ip route get 127.0.0.1 dev "$devdummy" > /dev/null
	check_err $?
	ip route get ::1 > /dev/null
	check_err $?
	ip route get fe80::1 dev "$devdummy" > /dev/null
	check_err $?
	ip route get 127.0.0.1 from 127.0.0.1 oif lo tos 0x1 mark 0x1 > /dev/null
	check_err $?
	ip route get ::1 from ::1 iif lo oif lo tos 0x1 mark 0x1 > /dev/null
	check_err $?
	ip addr add dev "$devdummy" 10.23.7.11/24
	check_err $?
	ip route get 10.23.7.11 from 10.23.7.12 iif "$devdummy" > /dev/null
	check_err $?
	ip route add 10.23.8.0/24 \
		nexthop via 10.23.7.13 dev "$devdummy" \
		nexthop via 10.23.7.14 dev "$devdummy"
	check_err $?
	sysctl -wq net.ipv4.fib_multipath_hash_policy=0
	ip route get 10.23.8.11 > /dev/null
	check_err $?
	sysctl -wq net.ipv4.fib_multipath_hash_policy=1
	ip route get 10.23.8.11 > /dev/null
	check_err $?
	sysctl -wq net.ipv4.fib_multipath_hash_policy="$hash_policy"
	ip route del 10.23.8.0/24
	check_err $?
	ip addr del dev "$devdummy" 10.23.7.11/24
	check_err $?

	if [ $ret -ne 0 ];then
		echo "FAIL: route get"
		return 1
	fi

	echo "PASS: route get"
}

kci_test_addrlft()
{
	for i in $(seq 10 100) ;do
		lft=$(((RANDOM%3) + 1))
		ip addr add 10.23.11.$i/32 dev "$devdummy" preferred_lft $lft valid_lft $((lft+1))
		check_err $?
	done

	sleep 5

	ip addr show dev "$devdummy" | grep "10.23.11."
	if [ $? -eq 0 ]; then
		echo "FAIL: preferred_lft addresses remaining"
		check_err 1
		return
	fi

	echo "PASS: preferred_lft addresses have expired"
}

kci_test_promote_secondaries()
{
	promote=$(sysctl -n net.ipv4.conf.$devdummy.promote_secondaries)

	sysctl -q net.ipv4.conf.$devdummy.promote_secondaries=1

	for i in $(seq 2 254);do
		IP="10.23.11.$i"
		ip -f inet addr add $IP/16 brd + dev "$devdummy"
		ifconfig "$devdummy" $IP netmask 255.255.0.0
	done

	ip addr flush dev "$devdummy"

	[ $promote -eq 0 ] && sysctl -q net.ipv4.conf.$devdummy.promote_secondaries=0

	echo "PASS: promote_secondaries complete"
}

kci_test_addrlabel()
{
	local ret=0

	ip addrlabel add prefix dead::/64 dev lo label 1
	check_err $?

	ip addrlabel list |grep -q "prefix dead::/64 dev lo label 1"
	check_err $?

	ip addrlabel del prefix dead::/64 dev lo label 1 2> /dev/null
	check_err $?

	ip addrlabel add prefix dead::/64 label 1 2> /dev/null
	check_err $?

	ip addrlabel del prefix dead::/64 label 1 2> /dev/null
	check_err $?

	# concurrent add/delete
	for i in $(seq 1 1000); do
		ip addrlabel add prefix 1c3::/64 label 12345 2>/dev/null
	done &

	for i in $(seq 1 1000); do
		ip addrlabel del prefix 1c3::/64 label 12345 2>/dev/null
	done

	wait

	ip addrlabel del prefix 1c3::/64 label 12345 2>/dev/null

	if [ $ret -ne 0 ];then
		echo "FAIL: ipv6 addrlabel"
		return 1
	fi

	echo "PASS: ipv6 addrlabel"
}

kci_test_ifalias()
{
	local ret=0
	namewant=$(uuidgen)
	syspathname="/sys/class/net/$devdummy/ifalias"

	ip link set dev "$devdummy" alias "$namewant"
	check_err $?

	if [ $ret -ne 0 ]; then
		echo "FAIL: cannot set interface alias of $devdummy to $namewant"
		return 1
	fi

	ip link show "$devdummy" | grep -q "alias $namewant"
	check_err $?

	if [ -r "$syspathname" ] ; then
		read namehave < "$syspathname"
		if [ "$namewant" != "$namehave" ]; then
			echo "FAIL: did set ifalias $namewant but got $namehave"
			return 1
		fi

		namewant=$(uuidgen)
		echo "$namewant" > "$syspathname"
	        ip link show "$devdummy" | grep -q "alias $namewant"
		check_err $?

		# sysfs interface allows to delete alias again
		echo "" > "$syspathname"

	        ip link show "$devdummy" | grep -q "alias $namewant"
		check_fail $?

		for i in $(seq 1 100); do
			uuidgen > "$syspathname" &
		done

		wait

		# re-add the alias -- kernel should free mem when dummy dev is removed
		ip link set dev "$devdummy" alias "$namewant"
		check_err $?
	fi

	if [ $ret -ne 0 ]; then
		echo "FAIL: set interface alias $devdummy to $namewant"
		return 1
	fi

	echo "PASS: set ifalias $namewant for $devdummy"
}

kci_test_vrf()
{
	vrfname="test-vrf"
	local ret=0

	ip link show type vrf 2>/dev/null
	if [ $? -ne 0 ]; then
		echo "SKIP: vrf: iproute2 too old"
		return $ksft_skip
	fi

	ip link add "$vrfname" type vrf table 10
	check_err $?
	if [ $ret -ne 0 ];then
		echo "FAIL: can't add vrf interface, skipping test"
		return 0
	fi

	ip -br link show type vrf | grep -q "$vrfname"
	check_err $?
	if [ $ret -ne 0 ];then
		echo "FAIL: created vrf device not found"
		return 1
	fi

	ip link set dev "$vrfname" up
	check_err $?

	ip link set dev "$devdummy" master "$vrfname"
	check_err $?
	ip link del dev "$vrfname"
	check_err $?

	if [ $ret -ne 0 ];then
		echo "FAIL: vrf"
		return 1
	fi

	echo "PASS: vrf"
}

kci_test_encap_vxlan()
{
	local ret=0
	vxlan="test-vxlan0"
	vlan="test-vlan0"
	testns="$1"

	ip -netns "$testns" link add "$vxlan" type vxlan id 42 group 239.1.1.1 \
		dev "$devdummy" dstport 4789 2>/dev/null
	if [ $? -ne 0 ]; then
		echo "FAIL: can't add vxlan interface, skipping test"
		return 0
	fi
	check_err $?

	ip -netns "$testns" addr add 10.2.11.49/24 dev "$vxlan"
	check_err $?

	ip -netns "$testns" link set up dev "$vxlan"
	check_err $?

	ip -netns "$testns" link add link "$vxlan" name "$vlan" type vlan id 1
	check_err $?

	# changelink testcases
	ip -netns "$testns" link set dev "$vxlan" type vxlan vni 43 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan group ffe5::5 dev "$devdummy" 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan ttl inherit 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan ttl 64
	check_err $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan nolearning
	check_err $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan proxy 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan norsc 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan l2miss 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan l3miss 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan external 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan udpcsum 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan udp6zerocsumtx 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan udp6zerocsumrx 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan remcsumtx 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan remcsumrx 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan gbp 2>/dev/null
	check_fail $?

	ip -netns "$testns" link set dev "$vxlan" type vxlan gpe 2>/dev/null
	check_fail $?

	ip -netns "$testns" link del "$vxlan"
	check_err $?

	if [ $ret -ne 0 ]; then
		echo "FAIL: vxlan"
		return 1
	fi
	echo "PASS: vxlan"
}

kci_test_encap_fou()
{
	local ret=0
	name="test-fou"
	testns="$1"

	ip fou help 2>&1 |grep -q 'Usage: ip fou'
	if [ $? -ne 0 ];then
		echo "SKIP: fou: iproute2 too old"
		return $ksft_skip
	fi

	if ! /sbin/modprobe -q -n fou; then
		echo "SKIP: module fou is not found"
		return $ksft_skip
	fi
	/sbin/modprobe -q fou
	ip -netns "$testns" fou add port 7777 ipproto 47 2>/dev/null
	if [ $? -ne 0 ];then
		echo "FAIL: can't add fou port 7777, skipping test"
		return 1
	fi

	ip -netns "$testns" fou add port 8888 ipproto 4
	check_err $?

	ip -netns "$testns" fou del port 9999 2>/dev/null
	check_fail $?

	ip -netns "$testns" fou del port 7777
	check_err $?

	if [ $ret -ne 0 ]; then
		echo "FAIL: fou"
		return 1
	fi

	echo "PASS: fou"
}

# test various encap methods, use netns to avoid unwanted interference
kci_test_encap()
{
	testns="testns"
	local ret=0

	ip netns add "$testns"
	if [ $? -ne 0 ]; then
		echo "SKIP encap tests: cannot add net namespace $testns"
		return $ksft_skip
	fi

	ip -netns "$testns" link set lo up
	check_err $?

	ip -netns "$testns" link add name "$devdummy" type dummy
	check_err $?
	ip -netns "$testns" link set "$devdummy" up
	check_err $?

	kci_test_encap_vxlan "$testns"
	check_err $?
	kci_test_encap_fou "$testns"
	check_err $?

	ip netns del "$testns"
	return $ret
}

kci_test_macsec()
{
	msname="test_macsec0"
	local ret=0

	ip macsec help 2>&1 | grep -q "^Usage: ip macsec"
	if [ $? -ne 0 ]; then
		echo "SKIP: macsec: iproute2 too old"
		return $ksft_skip
	fi

	ip link add link "$devdummy" "$msname" type macsec port 42 encrypt on
	check_err $?
	if [ $ret -ne 0 ];then
		echo "FAIL: can't add macsec interface, skipping test"
		return 1
	fi

	ip macsec add "$msname" tx sa 0 pn 1024 on key 01 12345678901234567890123456789012
	check_err $?

	ip macsec add "$msname" rx port 1234 address "1c:ed:de:ad:be:ef"
	check_err $?

	ip macsec add "$msname" rx port 1234 address "1c:ed:de:ad:be:ef" sa 0 pn 1 on key 00 0123456789abcdef0123456789abcdef
	check_err $?

	ip macsec show > /dev/null
	check_err $?

	ip link del dev "$msname"
	check_err $?

	if [ $ret -ne 0 ];then
		echo "FAIL: macsec"
		return 1
	fi

	echo "PASS: macsec"
}

#-------------------------------------------------------------------
# Example commands
#   ip x s add proto esp src 14.0.0.52 dst 14.0.0.70 \
#            spi 0x07 mode transport reqid 0x07 replay-window 32 \
#            aead 'rfc4106(gcm(aes))' 1234567890123456dcba 128 \
#            sel src 14.0.0.52/24 dst 14.0.0.70/24
#   ip x p add dir out src 14.0.0.52/24 dst 14.0.0.70/24 \
#            tmpl proto esp src 14.0.0.52 dst 14.0.0.70 \
#            spi 0x07 mode transport reqid 0x07
#
# Subcommands not tested
#    ip x s update
#    ip x s allocspi
#    ip x s deleteall
#    ip x p update
#    ip x p deleteall
#    ip x p set
#-------------------------------------------------------------------
kci_test_ipsec()
{
	local ret=0
	algo="aead rfc4106(gcm(aes)) 0x3132333435363738393031323334353664636261 128"
	srcip=192.168.123.1
	dstip=192.168.123.2
	spi=7

	ip addr add $srcip dev $devdummy

	# flush to be sure there's nothing configured
	ip x s flush ; ip x p flush
	check_err $?

	# start the monitor in the background
	tmpfile=`mktemp /var/run/ipsectestXXX`
	mpid=`(ip x m > $tmpfile & echo $!) 2>/dev/null`
	sleep 0.2

	ipsecid="proto esp src $srcip dst $dstip spi 0x07"
	ip x s add $ipsecid \
            mode transport reqid 0x07 replay-window 32 \
            $algo sel src $srcip/24 dst $dstip/24
	check_err $?

	lines=`ip x s list | grep $srcip | grep $dstip | wc -l`
	test $lines -eq 2
	check_err $?

	ip x s count | grep -q "SAD count 1"
	check_err $?

	lines=`ip x s get $ipsecid | grep $srcip | grep $dstip | wc -l`
	test $lines -eq 2
	check_err $?

	ip x s delete $ipsecid
	check_err $?

	lines=`ip x s list | wc -l`
	test $lines -eq 0
	check_err $?

	ipsecsel="dir out src $srcip/24 dst $dstip/24"
	ip x p add $ipsecsel \
		    tmpl proto esp src $srcip dst $dstip \
		    spi 0x07 mode transport reqid 0x07
	check_err $?

	lines=`ip x p list | grep $srcip | grep $dstip | wc -l`
	test $lines -eq 2
	check_err $?

	ip x p count | grep -q "SPD IN  0 OUT 1 FWD 0"
	check_err $?

	lines=`ip x p get $ipsecsel | grep $srcip | grep $dstip | wc -l`
	test $lines -eq 2
	check_err $?

	ip x p delete $ipsecsel
	check_err $?

	lines=`ip x p list | wc -l`
	test $lines -eq 0
	check_err $?

	# check the monitor results
	kill $mpid
	lines=`wc -l $tmpfile | cut "-d " -f1`
	test $lines -eq 20
	check_err $?
	rm -rf $tmpfile

	# clean up any leftovers
	ip x s flush
	check_err $?
	ip x p flush
	check_err $?
	ip addr del $srcip/32 dev $devdummy

	if [ $ret -ne 0 ]; then
		echo "FAIL: ipsec"
		return 1
	fi
	echo "PASS: ipsec"
}

#-------------------------------------------------------------------
# Example commands
#   ip x s add proto esp src 14.0.0.52 dst 14.0.0.70 \
#            spi 0x07 mode transport reqid 0x07 replay-window 32 \
#            aead 'rfc4106(gcm(aes))' 1234567890123456dcba 128 \
#            sel src 14.0.0.52/24 dst 14.0.0.70/24
#            offload dev sim1 dir out
#   ip x p add dir out src 14.0.0.52/24 dst 14.0.0.70/24 \
#            tmpl proto esp src 14.0.0.52 dst 14.0.0.70 \
#            spi 0x07 mode transport reqid 0x07
#
#-------------------------------------------------------------------
kci_test_ipsec_offload()
{
	local ret=0
	algo="aead rfc4106(gcm(aes)) 0x3132333435363738393031323334353664636261 128"
	srcip=192.168.123.3
	dstip=192.168.123.4
	sysfsd=/sys/kernel/debug/netdevsim/netdevsim0/ports/0/
	sysfsf=$sysfsd/ipsec
	sysfsnet=/sys/bus/netdevsim/devices/netdevsim0/net/
	probed=false

	# setup netdevsim since dummydev doesn't have offload support
	if [ ! -w /sys/bus/netdevsim/new_device ] ; then
		modprobe -q netdevsim
		check_err $?
		if [ $ret -ne 0 ]; then
			echo "SKIP: ipsec_offload can't load netdevsim"
			return $ksft_skip
		fi
		probed=true
	fi

	echo "0" > /sys/bus/netdevsim/new_device
	while [ ! -d $sysfsnet ] ; do :; done
	udevadm settle
	dev=`ls $sysfsnet`

	ip addr add $srcip dev $dev
	ip link set $dev up
	if [ ! -d $sysfsd ] ; then
		echo "FAIL: ipsec_offload can't create device $dev"
		return 1
	fi
	if [ ! -f $sysfsf ] ; then
		echo "FAIL: ipsec_offload netdevsim doesn't support IPsec offload"
		return 1
	fi

	# flush to be sure there's nothing configured
	ip x s flush ; ip x p flush

	# create offloaded SAs, both in and out
	ip x p add dir out src $srcip/24 dst $dstip/24 \
	    tmpl proto esp src $srcip dst $dstip spi 9 \
	    mode transport reqid 42
	check_err $?
	ip x p add dir out src $dstip/24 dst $srcip/24 \
	    tmpl proto esp src $dstip dst $srcip spi 9 \
	    mode transport reqid 42
	check_err $?

	ip x s add proto esp src $srcip dst $dstip spi 9 \
	    mode transport reqid 42 $algo sel src $srcip/24 dst $dstip/24 \
	    offload dev $dev dir out
	check_err $?
	ip x s add proto esp src $dstip dst $srcip spi 9 \
	    mode transport reqid 42 $algo sel src $dstip/24 dst $srcip/24 \
	    offload dev $dev dir in
	check_err $?
	if [ $ret -ne 0 ]; then
		echo "FAIL: ipsec_offload can't create SA"
		return 1
	fi

	# does offload show up in ip output
	lines=`ip x s list | grep -c "crypto offload parameters: dev $dev dir"`
	if [ $lines -ne 2 ] ; then
		echo "FAIL: ipsec_offload SA offload missing from list output"
		check_err 1
	fi

	# use ping to exercise the Tx path
	ping -I $dev -c 3 -W 1 -i 0 $dstip >/dev/null

	# does driver have correct offload info
	diff $sysfsf - << EOF
SA count=2 tx=3
sa[0] tx ipaddr=0x00000000 00000000 00000000 00000000
sa[0]    spi=0x00000009 proto=0x32 salt=0x61626364 crypt=1
sa[0]    key=0x34333231 38373635 32313039 36353433
sa[1] rx ipaddr=0x00000000 00000000 00000000 037ba8c0
sa[1]    spi=0x00000009 proto=0x32 salt=0x61626364 crypt=1
sa[1]    key=0x34333231 38373635 32313039 36353433
EOF
	if [ $? -ne 0 ] ; then
		echo "FAIL: ipsec_offload incorrect driver data"
		check_err 1
	fi

	# does offload get removed from driver
	ip x s flush
	ip x p flush
	lines=`grep -c "SA count=0" $sysfsf`
	if [ $lines -ne 1 ] ; then
		echo "FAIL: ipsec_offload SA not removed from driver"
		check_err 1
	fi

	# clean up any leftovers
	$probed && rmmod netdevsim

	if [ $ret -ne 0 ]; then
		echo "FAIL: ipsec_offload"
		return 1
	fi
	echo "PASS: ipsec_offload"
}

kci_test_gretap()
{
	testns="testns"
	DEV_NS=gretap00
	local ret=0

	ip netns add "$testns"
	if [ $? -ne 0 ]; then
		echo "SKIP gretap tests: cannot add net namespace $testns"
		return $ksft_skip
	fi

	ip link help gretap 2>&1 | grep -q "^Usage:"
	if [ $? -ne 0 ];then
		echo "SKIP: gretap: iproute2 too old"
		ip netns del "$testns"
		return $ksft_skip
	fi

	# test native tunnel
	ip -netns "$testns" link add dev "$DEV_NS" type gretap seq \
		key 102 local 172.16.1.100 remote 172.16.1.200
	check_err $?

	ip -netns "$testns" addr add dev "$DEV_NS" 10.1.1.100/24
	check_err $?

	ip -netns "$testns" link set dev $DEV_NS up
	check_err $?

	ip -netns "$testns" link del "$DEV_NS"
	check_err $?

	# test external mode
	ip -netns "$testns" link add dev "$DEV_NS" type gretap external
	check_err $?

	ip -netns "$testns" link del "$DEV_NS"
	check_err $?

	if [ $ret -ne 0 ]; then
		echo "FAIL: gretap"
		ip netns del "$testns"
		return 1
	fi
	echo "PASS: gretap"

	ip netns del "$testns"
}

kci_test_ip6gretap()
{
	testns="testns"
	DEV_NS=ip6gretap00
	local ret=0

	ip netns add "$testns"
	if [ $? -ne 0 ]; then
		echo "SKIP ip6gretap tests: cannot add net namespace $testns"
		return $ksft_skip
	fi

	ip link help ip6gretap 2>&1 | grep -q "^Usage:"
	if [ $? -ne 0 ];then
		echo "SKIP: ip6gretap: iproute2 too old"
		ip netns del "$testns"
		return $ksft_skip
	fi

	# test native tunnel
	ip -netns "$testns" link add dev "$DEV_NS" type ip6gretap seq \
		key 102 local fc00:100::1 remote fc00:100::2
	check_err $?

	ip -netns "$testns" addr add dev "$DEV_NS" fc00:200::1/96
	check_err $?

	ip -netns "$testns" link set dev $DEV_NS up
	check_err $?

	ip -netns "$testns" link del "$DEV_NS"
	check_err $?

	# test external mode
	ip -netns "$testns" link add dev "$DEV_NS" type ip6gretap external
	check_err $?

	ip -netns "$testns" link del "$DEV_NS"
	check_err $?

	if [ $ret -ne 0 ]; then
		echo "FAIL: ip6gretap"
		ip netns del "$testns"
		return 1
	fi
	echo "PASS: ip6gretap"

	ip netns del "$testns"
}

kci_test_erspan()
{
	testns="testns"
	DEV_NS=erspan00
	local ret=0

	ip link help erspan 2>&1 | grep -q "^Usage:"
	if [ $? -ne 0 ];then
		echo "SKIP: erspan: iproute2 too old"
		return $ksft_skip
	fi

	ip netns add "$testns"
	if [ $? -ne 0 ]; then
		echo "SKIP erspan tests: cannot add net namespace $testns"
		return $ksft_skip
	fi

	# test native tunnel erspan v1
	ip -netns "$testns" link add dev "$DEV_NS" type erspan seq \
		key 102 local 172.16.1.100 remote 172.16.1.200 \
		erspan_ver 1 erspan 488
	check_err $?

	ip -netns "$testns" addr add dev "$DEV_NS" 10.1.1.100/24
	check_err $?

	ip -netns "$testns" link set dev $DEV_NS up
	check_err $?

	ip -netns "$testns" link del "$DEV_NS"
	check_err $?

	# test native tunnel erspan v2
	ip -netns "$testns" link add dev "$DEV_NS" type erspan seq \
		key 102 local 172.16.1.100 remote 172.16.1.200 \
		erspan_ver 2 erspan_dir ingress erspan_hwid 7
	check_err $?

	ip -netns "$testns" addr add dev "$DEV_NS" 10.1.1.100/24
	check_err $?

	ip -netns "$testns" link set dev $DEV_NS up
	check_err $?

	ip -netns "$testns" link del "$DEV_NS"
	check_err $?

	# test external mode
	ip -netns "$testns" link add dev "$DEV_NS" type erspan external
	check_err $?

	ip -netns "$testns" link del "$DEV_NS"
	check_err $?

	if [ $ret -ne 0 ]; then
		echo "FAIL: erspan"
		ip netns del "$testns"
		return 1
	fi
	echo "PASS: erspan"

	ip netns del "$testns"
}

kci_test_ip6erspan()
{
	testns="testns"
	DEV_NS=ip6erspan00
	local ret=0

	ip link help ip6erspan 2>&1 | grep -q "^Usage:"
	if [ $? -ne 0 ];then
		echo "SKIP: ip6erspan: iproute2 too old"
		return $ksft_skip
	fi

	ip netns add "$testns"
	if [ $? -ne 0 ]; then
		echo "SKIP ip6erspan tests: cannot add net namespace $testns"
		return $ksft_skip
	fi

	# test native tunnel ip6erspan v1
	ip -netns "$testns" link add dev "$DEV_NS" type ip6erspan seq \
		key 102 local fc00:100::1 remote fc00:100::2 \
		erspan_ver 1 erspan 488
	check_err $?

	ip -netns "$testns" addr add dev "$DEV_NS" 10.1.1.100/24
	check_err $?

	ip -netns "$testns" link set dev $DEV_NS up
	check_err $?

	ip -netns "$testns" link del "$DEV_NS"
	check_err $?

	# test native tunnel ip6erspan v2
	ip -netns "$testns" link add dev "$DEV_NS" type ip6erspan seq \
		key 102 local fc00:100::1 remote fc00:100::2 \
		erspan_ver 2 erspan_dir ingress erspan_hwid 7
	check_err $?

	ip -netns "$testns" addr add dev "$DEV_NS" 10.1.1.100/24
	check_err $?

	ip -netns "$testns" link set dev $DEV_NS up
	check_err $?

	ip -netns "$testns" link del "$DEV_NS"
	check_err $?

	# test external mode
	ip -netns "$testns" link add dev "$DEV_NS" \
		type ip6erspan external
	check_err $?

	ip -netns "$testns" link del "$DEV_NS"
	check_err $?

	if [ $ret -ne 0 ]; then
		echo "FAIL: ip6erspan"
		ip netns del "$testns"
		return 1
	fi
	echo "PASS: ip6erspan"

	ip netns del "$testns"
}

kci_test_fdb_get()
{
	IP="ip -netns testns"
	BRIDGE="bridge -netns testns"
	brdev="test-br0"
	vxlandev="vxlan10"
	test_mac=de:ad:be:ef:13:37
	localip="10.0.2.2"
	dstip="10.0.2.3"
	local ret=0

	bridge fdb help 2>&1 |grep -q 'bridge fdb get'
	if [ $? -ne 0 ];then
		echo "SKIP: fdb get tests: iproute2 too old"
		return $ksft_skip
	fi

	ip netns add testns
	if [ $? -ne 0 ]; then
		echo "SKIP fdb get tests: cannot add net namespace $testns"
		return $ksft_skip
	fi

	$IP link add "$vxlandev" type vxlan id 10 local $localip \
                dstport 4789 2>/dev/null
	check_err $?
	$IP link add name "$brdev" type bridge &>/dev/null
	check_err $?
	$IP link set dev "$vxlandev" master "$brdev" &>/dev/null
	check_err $?
	$BRIDGE fdb add $test_mac dev "$vxlandev" master &>/dev/null
	check_err $?
	$BRIDGE fdb add $test_mac dev "$vxlandev" dst $dstip self &>/dev/null
	check_err $?

	$BRIDGE fdb get $test_mac brport "$vxlandev" 2>/dev/null | grep -q "dev $vxlandev master $brdev"
	check_err $?
	$BRIDGE fdb get $test_mac br "$brdev" 2>/dev/null | grep -q "dev $vxlandev master $brdev"
	check_err $?
	$BRIDGE fdb get $test_mac dev "$vxlandev" self 2>/dev/null | grep -q "dev $vxlandev dst $dstip"
	check_err $?

	ip netns del testns &>/dev/null

	if [ $ret -ne 0 ]; then
		echo "FAIL: bridge fdb get"
		return 1
	fi

	echo "PASS: bridge fdb get"
}

kci_test_neigh_get()
{
	dstmac=de:ad:be:ef:13:37
	dstip=10.0.2.4
	dstip6=dead::2
	local ret=0

	ip neigh help 2>&1 |grep -q 'ip neigh get'
	if [ $? -ne 0 ];then
		echo "SKIP: fdb get tests: iproute2 too old"
		return $ksft_skip
	fi

	# ipv4
	ip neigh add $dstip lladdr $dstmac dev "$devdummy"  > /dev/null
	check_err $?
	ip neigh get $dstip dev "$devdummy" 2> /dev/null | grep -q "$dstmac"
	check_err $?
	ip neigh del $dstip lladdr $dstmac dev "$devdummy"  > /dev/null
	check_err $?

	# ipv4 proxy
	ip neigh add proxy $dstip dev "$devdummy" > /dev/null
	check_err $?
	ip neigh get proxy $dstip dev "$devdummy" 2>/dev/null | grep -q "$dstip"
	check_err $?
	ip neigh del proxy $dstip dev "$devdummy" > /dev/null
	check_err $?

	# ipv6
	ip neigh add $dstip6 lladdr $dstmac dev "$devdummy"  > /dev/null
	check_err $?
	ip neigh get $dstip6 dev "$devdummy" 2> /dev/null | grep -q "$dstmac"
	check_err $?
	ip neigh del $dstip6 lladdr $dstmac dev "$devdummy"  > /dev/null
	check_err $?

	# ipv6 proxy
	ip neigh add proxy $dstip6 dev "$devdummy" > /dev/null
	check_err $?
	ip neigh get proxy $dstip6 dev "$devdummy" 2>/dev/null | grep -q "$dstip6"
	check_err $?
	ip neigh del proxy $dstip6 dev "$devdummy" > /dev/null
	check_err $?

	if [ $ret -ne 0 ];then
		echo "FAIL: neigh get"
		return 1
	fi

	echo "PASS: neigh get"
}

kci_test_bridge_parent_id()
{
	local ret=0
	sysfsnet=/sys/bus/netdevsim/devices/netdevsim
	probed=false

	if [ ! -w /sys/bus/netdevsim/new_device ] ; then
		modprobe -q netdevsim
		check_err $?
		if [ $ret -ne 0 ]; then
			echo "SKIP: bridge_parent_id can't load netdevsim"
			return $ksft_skip
		fi
		probed=true
	fi

	echo "10 1" > /sys/bus/netdevsim/new_device
	while [ ! -d ${sysfsnet}10 ] ; do :; done
	echo "20 1" > /sys/bus/netdevsim/new_device
	while [ ! -d ${sysfsnet}20 ] ; do :; done
	udevadm settle
	dev10=`ls ${sysfsnet}10/net/`
	dev20=`ls ${sysfsnet}20/net/`

	ip link add name test-bond0 type bond mode 802.3ad
	ip link set dev $dev10 master test-bond0
	ip link set dev $dev20 master test-bond0
	ip link add name test-br0 type bridge
	ip link set dev test-bond0 master test-br0
	check_err $?

	# clean up any leftovers
	ip link del dev test-br0
	ip link del dev test-bond0
	echo 20 > /sys/bus/netdevsim/del_device
	echo 10 > /sys/bus/netdevsim/del_device
	$probed && rmmod netdevsim

	if [ $ret -ne 0 ]; then
		echo "FAIL: bridge_parent_id"
		return 1
	fi
	echo "PASS: bridge_parent_id"
}

kci_test_rtnl()
{
	local ret=0
	kci_add_dummy
	if [ $ret -ne 0 ];then
		echo "FAIL: cannot add dummy interface"
		return 1
	fi

	kci_test_polrouting
	check_err $?
	kci_test_route_get
	check_err $?
	kci_test_addrlft
	check_err $?
	kci_test_promote_secondaries
	check_err $?
	kci_test_tc
	check_err $?
	kci_test_gre
	check_err $?
	kci_test_gretap
	check_err $?
	kci_test_ip6gretap
	check_err $?
	kci_test_erspan
	check_err $?
	kci_test_ip6erspan
	check_err $?
	kci_test_bridge
	check_err $?
	kci_test_addrlabel
	check_err $?
	kci_test_ifalias
	check_err $?
	kci_test_vrf
	check_err $?
	kci_test_encap
	check_err $?
	kci_test_macsec
	check_err $?
	kci_test_ipsec
	check_err $?
	kci_test_ipsec_offload
	check_err $?
	kci_test_fdb_get
	check_err $?
	kci_test_neigh_get
	check_err $?
	kci_test_bridge_parent_id
	check_err $?

	kci_del_dummy
	return $ret
}

#check for needed privileges
if [ "$(id -u)" -ne 0 ];then
	echo "SKIP: Need root privileges"
	exit $ksft_skip
fi

for x in ip tc;do
	$x -Version 2>/dev/null >/dev/null
	if [ $? -ne 0 ];then
		echo "SKIP: Could not run test without the $x tool"
		exit $ksft_skip
	fi
done

kci_test_rtnl

exit $?
