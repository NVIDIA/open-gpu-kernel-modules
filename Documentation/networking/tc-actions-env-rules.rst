.. SPDX-License-Identifier: GPL-2.0

================================
TC Actions - Environmental Rules
================================


The "environmental" rules for authors of any new tc actions are:

1) If you stealeth or borroweth any packet thou shalt be branching
   from the righteous path and thou shalt cloneth.

   For example if your action queues a packet to be processed later,
   or intentionally branches by redirecting a packet, then you need to
   clone the packet.

2) If you munge any packet thou shalt call pskb_expand_head in the case
   someone else is referencing the skb. After that you "own" the skb.

3) Dropping packets you don't own is a no-no. You simply return
   TC_ACT_SHOT to the caller and they will drop it.

The "environmental" rules for callers of actions (qdiscs etc) are:

#) Thou art responsible for freeing anything returned as being
   TC_ACT_SHOT/STOLEN/QUEUED. If none of TC_ACT_SHOT/STOLEN/QUEUED is
   returned, then all is great and you don't need to do anything.

Post on netdev if something is unclear.
