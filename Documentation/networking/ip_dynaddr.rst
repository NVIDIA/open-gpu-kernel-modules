.. SPDX-License-Identifier: GPL-2.0

==================================
IP dynamic address hack-port v0.03
==================================

This stuff allows diald ONESHOT connections to get established by
dynamically changing packet source address (and socket's if local procs).
It is implemented for TCP diald-box connections(1) and IP_MASQuerading(2).

If enabled\ [#]_ and forwarding interface has changed:

  1)  Socket (and packet) source address is rewritten ON RETRANSMISSIONS
      while in SYN_SENT state (diald-box processes).
  2)  Out-bounded MASQueraded source address changes ON OUTPUT (when
      internal host does retransmission) until a packet from outside is
      received by the tunnel.

This is specially helpful for auto dialup links (diald), where the
``actual`` outgoing address is unknown at the moment the link is
going up. So, the *same* (local AND masqueraded) connections requests that
bring the link up will be able to get established.

.. [#] At boot, by default no address rewriting is attempted.

  To enable::

     # echo 1 > /proc/sys/net/ipv4/ip_dynaddr

  To enable verbose mode::

    # echo 2 > /proc/sys/net/ipv4/ip_dynaddr

  To disable (default)::

     # echo 0 > /proc/sys/net/ipv4/ip_dynaddr

Enjoy!

Juanjo  <jjciarla@raiz.uncu.edu.ar>
