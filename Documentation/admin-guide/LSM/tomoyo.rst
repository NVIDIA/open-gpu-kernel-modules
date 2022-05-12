======
TOMOYO
======

What is TOMOYO?
===============

TOMOYO is a name-based MAC extension (LSM module) for the Linux kernel.

LiveCD-based tutorials are available at

http://tomoyo.sourceforge.jp/1.8/ubuntu12.04-live.html
http://tomoyo.sourceforge.jp/1.8/centos6-live.html

Though these tutorials use non-LSM version of TOMOYO, they are useful for you
to know what TOMOYO is.

How to enable TOMOYO?
=====================

Build the kernel with ``CONFIG_SECURITY_TOMOYO=y`` and pass ``security=tomoyo`` on
kernel's command line.

Please see http://tomoyo.osdn.jp/2.5/ for details.

Where is documentation?
=======================

User <-> Kernel interface documentation is available at
https://tomoyo.osdn.jp/2.5/policy-specification/index.html .

Materials we prepared for seminars and symposiums are available at
https://osdn.jp/projects/tomoyo/docs/?category_id=532&language_id=1 .
Below lists are chosen from three aspects.

What is TOMOYO?
  TOMOYO Linux Overview
    https://osdn.jp/projects/tomoyo/docs/lca2009-takeda.pdf
  TOMOYO Linux: pragmatic and manageable security for Linux
    https://osdn.jp/projects/tomoyo/docs/freedomhectaipei-tomoyo.pdf
  TOMOYO Linux: A Practical Method to Understand and Protect Your Own Linux Box
    https://osdn.jp/projects/tomoyo/docs/PacSec2007-en-no-demo.pdf

What can TOMOYO do?
  Deep inside TOMOYO Linux
    https://osdn.jp/projects/tomoyo/docs/lca2009-kumaneko.pdf
  The role of "pathname based access control" in security.
    https://osdn.jp/projects/tomoyo/docs/lfj2008-bof.pdf

History of TOMOYO?
  Realities of Mainlining
    https://osdn.jp/projects/tomoyo/docs/lfj2008.pdf

What is future plan?
====================

We believe that inode based security and name based security are complementary
and both should be used together. But unfortunately, so far, we cannot enable
multiple LSM modules at the same time. We feel sorry that you have to give up
SELinux/SMACK/AppArmor etc. when you want to use TOMOYO.

We hope that LSM becomes stackable in future. Meanwhile, you can use non-LSM
version of TOMOYO, available at http://tomoyo.osdn.jp/1.8/ .
LSM version of TOMOYO is a subset of non-LSM version of TOMOYO. We are planning
to port non-LSM version's functionalities to LSM versions.
