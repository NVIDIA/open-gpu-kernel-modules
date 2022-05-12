:orphan:

.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

===================================================================
mac80211_hwsim - software simulator of 802.11 radio(s) for mac80211
===================================================================

:Copyright: |copy| 2008, Jouni Malinen <j@w1.fi>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.


Introduction
============

mac80211_hwsim is a Linux kernel module that can be used to simulate
arbitrary number of IEEE 802.11 radios for mac80211. It can be used to
test most of the mac80211 functionality and user space tools (e.g.,
hostapd and wpa_supplicant) in a way that matches very closely with
the normal case of using real WLAN hardware. From the mac80211 view
point, mac80211_hwsim is yet another hardware driver, i.e., no changes
to mac80211 are needed to use this testing tool.

The main goal for mac80211_hwsim is to make it easier for developers
to test their code and work with new features to mac80211, hostapd,
and wpa_supplicant. The simulated radios do not have the limitations
of real hardware, so it is easy to generate an arbitrary test setup
and always reproduce the same setup for future tests. In addition,
since all radio operation is simulated, any channel can be used in
tests regardless of regulatory rules.

mac80211_hwsim kernel module has a parameter 'radios' that can be used
to select how many radios are simulated (default 2). This allows
configuration of both very simply setups (e.g., just a single access
point and a station) or large scale tests (multiple access points with
hundreds of stations).

mac80211_hwsim works by tracking the current channel of each virtual
radio and copying all transmitted frames to all other radios that are
currently enabled and on the same channel as the transmitting
radio. Software encryption in mac80211 is used so that the frames are
actually encrypted over the virtual air interface to allow more
complete testing of encryption.

A global monitoring netdev, hwsim#, is created independent of
mac80211. This interface can be used to monitor all transmitted frames
regardless of channel.


Simple example
==============

This example shows how to use mac80211_hwsim to simulate two radios:
one to act as an access point and the other as a station that
associates with the AP. hostapd and wpa_supplicant are used to take
care of WPA2-PSK authentication. In addition, hostapd is also
processing access point side of association.

::


    # Build mac80211_hwsim as part of kernel configuration

    # Load the module
    modprobe mac80211_hwsim

    # Run hostapd (AP) for wlan0
    hostapd hostapd.conf

    # Run wpa_supplicant (station) for wlan1
    wpa_supplicant -Dnl80211 -iwlan1 -c wpa_supplicant.conf


More test cases are available in hostap.git:
git://w1.fi/srv/git/hostap.git and mac80211_hwsim/tests subdirectory
(http://w1.fi/gitweb/gitweb.cgi?p=hostap.git;a=tree;f=mac80211_hwsim/tests)
