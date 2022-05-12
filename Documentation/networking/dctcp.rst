.. SPDX-License-Identifier: GPL-2.0

======================
DCTCP (DataCenter TCP)
======================

DCTCP is an enhancement to the TCP congestion control algorithm for data
center networks and leverages Explicit Congestion Notification (ECN) in
the data center network to provide multi-bit feedback to the end hosts.

To enable it on end hosts::

  sysctl -w net.ipv4.tcp_congestion_control=dctcp
  sysctl -w net.ipv4.tcp_ecn_fallback=0 (optional)

All switches in the data center network running DCTCP must support ECN
marking and be configured for marking when reaching defined switch buffer
thresholds. The default ECN marking threshold heuristic for DCTCP on
switches is 20 packets (30KB) at 1Gbps, and 65 packets (~100KB) at 10Gbps,
but might need further careful tweaking.

For more details, see below documents:

Paper:

The algorithm is further described in detail in the following two
SIGCOMM/SIGMETRICS papers:

 i) Mohammad Alizadeh, Albert Greenberg, David A. Maltz, Jitendra Padhye,
    Parveen Patel, Balaji Prabhakar, Sudipta Sengupta, and Murari Sridharan:

      "Data Center TCP (DCTCP)", Data Center Networks session"

      Proc. ACM SIGCOMM, New Delhi, 2010.

    http://simula.stanford.edu/~alizade/Site/DCTCP_files/dctcp-final.pdf
    http://www.sigcomm.org/ccr/papers/2010/October/1851275.1851192

ii) Mohammad Alizadeh, Adel Javanmard, and Balaji Prabhakar:

      "Analysis of DCTCP: Stability, Convergence, and Fairness"
      Proc. ACM SIGMETRICS, San Jose, 2011.

    http://simula.stanford.edu/~alizade/Site/DCTCP_files/dctcp_analysis-full.pdf

IETF informational draft:

  http://tools.ietf.org/html/draft-bensley-tcpm-dctcp-00

DCTCP site:

  http://simula.stanford.edu/~alizade/Site/DCTCP.html
