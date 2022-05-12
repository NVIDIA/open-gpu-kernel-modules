=========================================
The TCM v4 fabric module script generator
=========================================

Greetings all,

This document is intended to be a mini-HOWTO for using the tcm_mod_builder.py
script to generate a brand new functional TCM v4 fabric .ko module of your very own,
that once built can be immediately be loaded to start access the new TCM/ConfigFS
fabric skeleton, by simply using::

	modprobe $TCM_NEW_MOD
	mkdir -p /sys/kernel/config/target/$TCM_NEW_MOD

This script will create a new drivers/target/$TCM_NEW_MOD/, and will do the following

	1) Generate new API callers for drivers/target/target_core_fabric_configs.c logic
	   ->make_tpg(), ->drop_tpg(), ->make_wwn(), ->drop_wwn().  These are created
	   into $TCM_NEW_MOD/$TCM_NEW_MOD_configfs.c
	2) Generate basic infrastructure for loading/unloading LKMs and TCM/ConfigFS fabric module
	   using a skeleton struct target_core_fabric_ops API template.
	3) Based on user defined T10 Proto_Ident for the new fabric module being built,
	   the TransportID / Initiator and Target WWPN related handlers for
	   SPC-3 persistent reservation are automatically generated in $TCM_NEW_MOD/$TCM_NEW_MOD_fabric.c
	   using drivers/target/target_core_fabric_lib.c logic.
	4) NOP API calls for all other Data I/O path and fabric dependent attribute logic
	   in $TCM_NEW_MOD/$TCM_NEW_MOD_fabric.c

tcm_mod_builder.py depends upon the mandatory '-p $PROTO_IDENT' and '-m
$FABRIC_MOD_name' parameters, and actually running the script looks like::

  target:/mnt/sdb/lio-core-2.6.git/Documentation/target# python tcm_mod_builder.py -p iSCSI -m tcm_nab5000
  tcm_dir: /mnt/sdb/lio-core-2.6.git/Documentation/target/../../
  Set fabric_mod_name: tcm_nab5000
  Set fabric_mod_dir:
  /mnt/sdb/lio-core-2.6.git/Documentation/target/../../drivers/target/tcm_nab5000
  Using proto_ident: iSCSI
  Creating fabric_mod_dir:
  /mnt/sdb/lio-core-2.6.git/Documentation/target/../../drivers/target/tcm_nab5000
  Writing file:
  /mnt/sdb/lio-core-2.6.git/Documentation/target/../../drivers/target/tcm_nab5000/tcm_nab5000_base.h
  Using tcm_mod_scan_fabric_ops:
  /mnt/sdb/lio-core-2.6.git/Documentation/target/../../include/target/target_core_fabric_ops.h
  Writing file:
  /mnt/sdb/lio-core-2.6.git/Documentation/target/../../drivers/target/tcm_nab5000/tcm_nab5000_fabric.c
  Writing file:
  /mnt/sdb/lio-core-2.6.git/Documentation/target/../../drivers/target/tcm_nab5000/tcm_nab5000_fabric.h
  Writing file:
  /mnt/sdb/lio-core-2.6.git/Documentation/target/../../drivers/target/tcm_nab5000/tcm_nab5000_configfs.c
  Writing file:
  /mnt/sdb/lio-core-2.6.git/Documentation/target/../../drivers/target/tcm_nab5000/Kbuild
  Writing file:
  /mnt/sdb/lio-core-2.6.git/Documentation/target/../../drivers/target/tcm_nab5000/Kconfig
  Would you like to add tcm_nab5000to drivers/target/Kbuild..? [yes,no]: yes
  Would you like to add tcm_nab5000to drivers/target/Kconfig..? [yes,no]: yes

At the end of tcm_mod_builder.py. the script will ask to add the following
line to drivers/target/Kbuild::

	obj-$(CONFIG_TCM_NAB5000)       += tcm_nab5000/

and the same for drivers/target/Kconfig::

	source "drivers/target/tcm_nab5000/Kconfig"

#) Run 'make menuconfig' and select the new CONFIG_TCM_NAB5000 item::

	<M>   TCM_NAB5000 fabric module

#) Build using 'make modules', once completed you will have::

    target:/mnt/sdb/lio-core-2.6.git# ls -la drivers/target/tcm_nab5000/
    total 1348
    drwxr-xr-x 2 root root   4096 2010-10-05 03:23 .
    drwxr-xr-x 9 root root   4096 2010-10-05 03:22 ..
    -rw-r--r-- 1 root root    282 2010-10-05 03:22 Kbuild
    -rw-r--r-- 1 root root    171 2010-10-05 03:22 Kconfig
    -rw-r--r-- 1 root root     49 2010-10-05 03:23 modules.order
    -rw-r--r-- 1 root root    738 2010-10-05 03:22 tcm_nab5000_base.h
    -rw-r--r-- 1 root root   9096 2010-10-05 03:22 tcm_nab5000_configfs.c
    -rw-r--r-- 1 root root 191200 2010-10-05 03:23 tcm_nab5000_configfs.o
    -rw-r--r-- 1 root root  40504 2010-10-05 03:23 .tcm_nab5000_configfs.o.cmd
    -rw-r--r-- 1 root root   5414 2010-10-05 03:22 tcm_nab5000_fabric.c
    -rw-r--r-- 1 root root   2016 2010-10-05 03:22 tcm_nab5000_fabric.h
    -rw-r--r-- 1 root root 190932 2010-10-05 03:23 tcm_nab5000_fabric.o
    -rw-r--r-- 1 root root  40713 2010-10-05 03:23 .tcm_nab5000_fabric.o.cmd
    -rw-r--r-- 1 root root 401861 2010-10-05 03:23 tcm_nab5000.ko
    -rw-r--r-- 1 root root    265 2010-10-05 03:23 .tcm_nab5000.ko.cmd
    -rw-r--r-- 1 root root    459 2010-10-05 03:23 tcm_nab5000.mod.c
    -rw-r--r-- 1 root root  23896 2010-10-05 03:23 tcm_nab5000.mod.o
    -rw-r--r-- 1 root root  22655 2010-10-05 03:23 .tcm_nab5000.mod.o.cmd
    -rw-r--r-- 1 root root 379022 2010-10-05 03:23 tcm_nab5000.o
    -rw-r--r-- 1 root root    211 2010-10-05 03:23 .tcm_nab5000.o.cmd

#) Load the new module, create a lun_0 configfs group, and add new TCM Core
   IBLOCK backstore symlink to port::

    target:/mnt/sdb/lio-core-2.6.git# insmod drivers/target/tcm_nab5000.ko
    target:/mnt/sdb/lio-core-2.6.git# mkdir -p /sys/kernel/config/target/nab5000/iqn.foo/tpgt_1/lun/lun_0
    target:/mnt/sdb/lio-core-2.6.git# cd /sys/kernel/config/target/nab5000/iqn.foo/tpgt_1/lun/lun_0/
    target:/sys/kernel/config/target/nab5000/iqn.foo/tpgt_1/lun/lun_0# ln -s /sys/kernel/config/target/core/iblock_0/lvm_test0 nab5000_port

    target:/sys/kernel/config/target/nab5000/iqn.foo/tpgt_1/lun/lun_0# cd -
    target:/mnt/sdb/lio-core-2.6.git# tree /sys/kernel/config/target/nab5000/
    /sys/kernel/config/target/nab5000/
    |-- discovery_auth
    |-- iqn.foo
    |   `-- tpgt_1
    |       |-- acls
    |       |-- attrib
    |       |-- lun
    |       |   `-- lun_0
    |       |       |-- alua_tg_pt_gp
    |       |       |-- alua_tg_pt_offline
    |       |       |-- alua_tg_pt_status
    |       |       |-- alua_tg_pt_write_md
    |	|	`-- nab5000_port -> ../../../../../../target/core/iblock_0/lvm_test0
    |       |-- np
    |       `-- param
    `-- version

    target:/mnt/sdb/lio-core-2.6.git# lsmod
    Module                  Size  Used by
    tcm_nab5000             3935  4
    iscsi_target_mod      193211  0
    target_core_stgt        8090  0
    target_core_pscsi      11122  1
    target_core_file        9172  2
    target_core_iblock      9280  1
    target_core_mod       228575  31
    tcm_nab5000,iscsi_target_mod,target_core_stgt,target_core_pscsi,target_core_file,target_core_iblock
    libfc                  73681  0
    scsi_debug             56265  0
    scsi_tgt                8666  1 target_core_stgt
    configfs               20644  2 target_core_mod

----------------------------------------------------------------------

Future TODO items
=================

	1) Add more T10 proto_idents
	2) Make tcm_mod_dump_fabric_ops() smarter and generate function pointer
	   defs directly from include/target/target_core_fabric_ops.h:struct target_core_fabric_ops
	   structure members.

October 5th, 2010

Nicholas A. Bellinger <nab@linux-iscsi.org>
