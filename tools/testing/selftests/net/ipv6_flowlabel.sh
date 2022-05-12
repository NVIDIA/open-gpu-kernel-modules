#!/bin/sh
# SPDX-License-Identifier: GPL-2.0
#
# Regression tests for IPv6 flowlabels
#
# run in separate namespaces to avoid mgmt db conflicts betweent tests

set -e

echo "TEST management"
./in_netns.sh ./ipv6_flowlabel_mgr

echo "TEST datapath"
./in_netns.sh \
  sh -c 'sysctl -q -w net.ipv6.auto_flowlabels=0 && ./ipv6_flowlabel -l 1'

echo "TEST datapath (with auto-flowlabels)"
./in_netns.sh \
  sh -c 'sysctl -q -w net.ipv6.auto_flowlabels=1 && ./ipv6_flowlabel -l 1'

echo OK. All tests passed
