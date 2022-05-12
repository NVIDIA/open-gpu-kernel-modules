.. _magicnumbers:

Linux magic numbers
===================

This file is a registry of magic numbers which are in use.  When you
add a magic number to a structure, you should also add it to this
file, since it is best if the magic numbers used by various structures
are unique.

It is a **very** good idea to protect kernel data structures with magic
numbers.  This allows you to check at run time whether (a) a structure
has been clobbered, or (b) you've passed the wrong structure to a
routine.  This last is especially useful --- particularly when you are
passing pointers to structures via a void * pointer.  The tty code,
for example, does this frequently to pass driver-specific and line
discipline-specific structures back and forth.

The way to use magic numbers is to declare them at the beginning of
the structure, like so::

	struct tty_ldisc {
		int	magic;
		...
	};

Please follow this discipline when you are adding future enhancements
to the kernel!  It has saved me countless hours of debugging,
especially in the screwy cases where an array has been overrun and
structures following the array have been overwritten.  Using this
discipline, these cases get detected quickly and safely.

Changelog::

					Theodore Ts'o
					31 Mar 94

  The magic table is current to Linux 2.1.55.

					Michael Chastain
					<mailto:mec@shout.net>
					22 Sep 1997

  Now it should be up to date with Linux 2.1.112. Because
  we are in feature freeze time it is very unlikely that
  something will change before 2.2.x. The entries are
  sorted by number field.

					Krzysztof G. Baranowski
					<mailto: kgb@knm.org.pl>
					29 Jul 1998

  Updated the magic table to Linux 2.5.45. Right over the feature freeze,
  but it is possible that some new magic numbers will sneak into the
  kernel before 2.6.x yet.

					Petr Baudis
					<pasky@ucw.cz>
					03 Nov 2002

  Updated the magic table to Linux 2.5.74.

					Fabian Frederick
					<ffrederick@users.sourceforge.net>
					09 Jul 2003


===================== ================ ======================== ==========================================
Magic Name            Number           Structure                File
===================== ================ ======================== ==========================================
PG_MAGIC              'P'              pg_{read,write}_hdr      ``include/linux/pg.h``
CMAGIC                0x0111           user                     ``include/linux/a.out.h``
MKISS_DRIVER_MAGIC    0x04bf           mkiss_channel            ``drivers/net/mkiss.h``
HDLC_MAGIC            0x239e           n_hdlc                   ``drivers/char/n_hdlc.c``
APM_BIOS_MAGIC        0x4101           apm_user                 ``arch/x86/kernel/apm_32.c``
DB_MAGIC              0x4442           fc_info                  ``drivers/net/iph5526_novram.c``
DL_MAGIC              0x444d           fc_info                  ``drivers/net/iph5526_novram.c``
FASYNC_MAGIC          0x4601           fasync_struct            ``include/linux/fs.h``
FF_MAGIC              0x4646           fc_info                  ``drivers/net/iph5526_novram.c``
PTY_MAGIC             0x5001                                    ``drivers/char/pty.c``
PPP_MAGIC             0x5002           ppp                      ``include/linux/if_pppvar.h``
SSTATE_MAGIC          0x5302           serial_state             ``include/linux/serial.h``
SLIP_MAGIC            0x5302           slip                     ``drivers/net/slip.h``
STRIP_MAGIC           0x5303           strip                    ``drivers/net/strip.c``
SIXPACK_MAGIC         0x5304           sixpack                  ``drivers/net/hamradio/6pack.h``
AX25_MAGIC            0x5316           ax_disp                  ``drivers/net/mkiss.h``
TTY_MAGIC             0x5401           tty_struct               ``include/linux/tty.h``
MGSL_MAGIC            0x5401           mgsl_info                ``drivers/char/synclink.c``
TTY_DRIVER_MAGIC      0x5402           tty_driver               ``include/linux/tty_driver.h``
MGSLPC_MAGIC          0x5402           mgslpc_info              ``drivers/char/pcmcia/synclink_cs.c``
USB_SERIAL_MAGIC      0x6702           usb_serial               ``drivers/usb/serial/usb-serial.h``
FULL_DUPLEX_MAGIC     0x6969                                    ``drivers/net/ethernet/dec/tulip/de2104x.c``
USB_BLUETOOTH_MAGIC   0x6d02           usb_bluetooth            ``drivers/usb/class/bluetty.c``
RFCOMM_TTY_MAGIC      0x6d02                                    ``net/bluetooth/rfcomm/tty.c``
USB_SERIAL_PORT_MAGIC 0x7301           usb_serial_port          ``drivers/usb/serial/usb-serial.h``
CG_MAGIC              0x00090255       ufs_cylinder_group       ``include/linux/ufs_fs.h``
LSEMAGIC              0x05091998       lse                      ``drivers/fc4/fc.c``
RIEBL_MAGIC           0x09051990                                ``drivers/net/atarilance.c``
NBD_REQUEST_MAGIC     0x12560953       nbd_request              ``include/linux/nbd.h``
RED_MAGIC2            0x170fc2a5       (any)                    ``mm/slab.c``
BAYCOM_MAGIC          0x19730510       baycom_state             ``drivers/net/baycom_epp.c``
ISDN_X25IFACE_MAGIC   0x1e75a2b9       isdn_x25iface_proto_data ``drivers/isdn/isdn_x25iface.h``
ECP_MAGIC             0x21504345       cdkecpsig                ``include/linux/cdk.h``
LSOMAGIC              0x27091997       lso                      ``drivers/fc4/fc.c``
LSMAGIC               0x2a3b4d2a       ls                       ``drivers/fc4/fc.c``
WANPIPE_MAGIC         0x414C4453       sdla_{dump,exec}         ``include/linux/wanpipe.h``
CS_CARD_MAGIC         0x43525553       cs_card                  ``sound/oss/cs46xx.c``
LABELCL_MAGIC         0x4857434c       labelcl_info_s           ``include/asm/ia64/sn/labelcl.h``
ISDN_ASYNC_MAGIC      0x49344C01       modem_info               ``include/linux/isdn.h``
CTC_ASYNC_MAGIC       0x49344C01       ctc_tty_info             ``drivers/s390/net/ctctty.c``
ISDN_NET_MAGIC        0x49344C02       isdn_net_local_s         ``drivers/isdn/i4l/isdn_net_lib.h``
SAVEKMSG_MAGIC2       0x4B4D5347       savekmsg                 ``arch/*/amiga/config.c``
CS_STATE_MAGIC        0x4c4f4749       cs_state                 ``sound/oss/cs46xx.c``
SLAB_C_MAGIC          0x4f17a36d       kmem_cache               ``mm/slab.c``
COW_MAGIC             0x4f4f4f4d       cow_header_v1            ``arch/um/drivers/ubd_user.c``
I810_CARD_MAGIC       0x5072696E       i810_card                ``sound/oss/i810_audio.c``
TRIDENT_CARD_MAGIC    0x5072696E       trident_card             ``sound/oss/trident.c``
ROUTER_MAGIC          0x524d4157       wan_device               [in ``wanrouter.h`` pre 3.9]
SAVEKMSG_MAGIC1       0x53415645       savekmsg                 ``arch/*/amiga/config.c``
GDA_MAGIC             0x58464552       gda                      ``arch/mips/include/asm/sn/gda.h``
RED_MAGIC1            0x5a2cf071       (any)                    ``mm/slab.c``
EEPROM_MAGIC_VALUE    0x5ab478d2       lanai_dev                ``drivers/atm/lanai.c``
HDLCDRV_MAGIC         0x5ac6e778       hdlcdrv_state            ``include/linux/hdlcdrv.h``
PCXX_MAGIC            0x5c6df104       channel                  ``drivers/char/pcxx.h``
KV_MAGIC              0x5f4b565f       kernel_vars_s            ``arch/mips/include/asm/sn/klkernvars.h``
I810_STATE_MAGIC      0x63657373       i810_state               ``sound/oss/i810_audio.c``
TRIDENT_STATE_MAGIC   0x63657373       trient_state             ``sound/oss/trident.c``
M3_CARD_MAGIC         0x646e6f50       m3_card                  ``sound/oss/maestro3.c``
FW_HEADER_MAGIC       0x65726F66       fw_header                ``drivers/atm/fore200e.h``
SLOT_MAGIC            0x67267321       slot                     ``drivers/hotplug/cpqphp.h``
SLOT_MAGIC            0x67267322       slot                     ``drivers/hotplug/acpiphp.h``
LO_MAGIC              0x68797548       nbd_device               ``include/linux/nbd.h``
M3_STATE_MAGIC        0x734d724d       m3_state                 ``sound/oss/maestro3.c``
VMALLOC_MAGIC         0x87654320       snd_alloc_track          ``sound/core/memory.c``
KMALLOC_MAGIC         0x87654321       snd_alloc_track          ``sound/core/memory.c``
PWC_MAGIC             0x89DC10AB       pwc_device               ``drivers/usb/media/pwc.h``
NBD_REPLY_MAGIC       0x96744668       nbd_reply                ``include/linux/nbd.h``
ENI155_MAGIC          0xa54b872d       midway_eprom	        ``drivers/atm/eni.h``
CODA_MAGIC            0xC0DAC0DA       coda_file_info           ``fs/coda/coda_fs_i.h``
YAM_MAGIC             0xF10A7654       yam_port                 ``drivers/net/hamradio/yam.c``
CCB_MAGIC             0xf2691ad2       ccb                      ``drivers/scsi/ncr53c8xx.c``
QUEUE_MAGIC_FREE      0xf7e1c9a3       queue_entry              ``drivers/scsi/arm/queue.c``
QUEUE_MAGIC_USED      0xf7e1cc33       queue_entry              ``drivers/scsi/arm/queue.c``
HTB_CMAGIC            0xFEFAFEF1       htb_class                ``net/sched/sch_htb.c``
NMI_MAGIC             0x48414d4d455201 nmi_s                    ``arch/mips/include/asm/sn/nmi.h``
===================== ================ ======================== ==========================================

Note that there are also defined special per-driver magic numbers in sound
memory management. See ``include/sound/sndmagic.h`` for complete list of them. Many
OSS sound drivers have their magic numbers constructed from the soundcard PCI
ID - these are not listed here as well.

HFS is another larger user of magic numbers - you can find them in
``fs/hfs/hfs.h``.
