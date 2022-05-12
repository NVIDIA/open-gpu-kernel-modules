=============
Ioctl Numbers
=============

19 October 1999

Michael Elizabeth Chastain
<mec@shout.net>

If you are adding new ioctl's to the kernel, you should use the _IO
macros defined in <linux/ioctl.h>:

    ====== == ============================================
    _IO    an ioctl with no parameters
    _IOW   an ioctl with write parameters (copy_from_user)
    _IOR   an ioctl with read parameters  (copy_to_user)
    _IOWR  an ioctl with both write and read parameters.
    ====== == ============================================

'Write' and 'read' are from the user's point of view, just like the
system calls 'write' and 'read'.  For example, a SET_FOO ioctl would
be _IOW, although the kernel would actually read data from user space;
a GET_FOO ioctl would be _IOR, although the kernel would actually write
data to user space.

The first argument to _IO, _IOW, _IOR, or _IOWR is an identifying letter
or number from the table below.  Because of the large number of drivers,
many drivers share a partial letter with other drivers.

If you are writing a driver for a new device and need a letter, pick an
unused block with enough room for expansion: 32 to 256 ioctl commands.
You can register the block by patching this file and submitting the
patch to Linus Torvalds.  Or you can e-mail me at <mec@shout.net> and
I'll register one for you.

The second argument to _IO, _IOW, _IOR, or _IOWR is a sequence number
to distinguish ioctls from each other.  The third argument to _IOW,
_IOR, or _IOWR is the type of the data going into the kernel or coming
out of the kernel (e.g.  'int' or 'struct foo').  NOTE!  Do NOT use
sizeof(arg) as the third argument as this results in your ioctl thinking
it passes an argument of type size_t.

Some devices use their major number as the identifier; this is OK, as
long as it is unique.  Some devices are irregular and don't follow any
convention at all.

Following this convention is good because:

(1) Keeping the ioctl's globally unique helps error checking:
    if a program calls an ioctl on the wrong device, it will get an
    error rather than some unexpected behaviour.

(2) The 'strace' build procedure automatically finds ioctl numbers
    defined with _IO, _IOW, _IOR, or _IOWR.

(3) 'strace' can decode numbers back into useful names when the
    numbers are unique.

(4) People looking for ioctls can grep for them more easily when
    this convention is used to define the ioctl numbers.

(5) When following the convention, the driver code can use generic
    code to copy the parameters between user and kernel space.

This table lists ioctls visible from user land for Linux/x86.  It contains
most drivers up to 2.6.31, but I know I am missing some.  There has been
no attempt to list non-X86 architectures or ioctls from drivers/staging/.

====  =====  ======================================================= ================================================================
Code  Seq#    Include File                                           Comments
      (hex)
====  =====  ======================================================= ================================================================
0x00  00-1F  linux/fs.h                                              conflict!
0x00  00-1F  scsi/scsi_ioctl.h                                       conflict!
0x00  00-1F  linux/fb.h                                              conflict!
0x00  00-1F  linux/wavefront.h                                       conflict!
0x02  all    linux/fd.h
0x03  all    linux/hdreg.h
0x04  D2-DC  linux/umsdos_fs.h                                       Dead since 2.6.11, but don't reuse these.
0x06  all    linux/lp.h
0x09  all    linux/raid/md_u.h
0x10  00-0F  drivers/char/s390/vmcp.h
0x10  10-1F  arch/s390/include/uapi/sclp_ctl.h
0x10  20-2F  arch/s390/include/uapi/asm/hypfs.h
0x12  all    linux/fs.h
             linux/blkpg.h
0x1b  all                                                            InfiniBand Subsystem
                                                                     <http://infiniband.sourceforge.net/>
0x20  all    drivers/cdrom/cm206.h
0x22  all    scsi/sg.h
'!'   00-1F  uapi/linux/seccomp.h
'#'   00-3F                                                          IEEE 1394 Subsystem
                                                                     Block for the entire subsystem
'$'   00-0F  linux/perf_counter.h, linux/perf_event.h
'%'   00-0F  include/uapi/linux/stm.h                                System Trace Module subsystem
                                                                     <mailto:alexander.shishkin@linux.intel.com>
'&'   00-07  drivers/firewire/nosy-user.h
'1'   00-1F  linux/timepps.h                                         PPS kit from Ulrich Windl
                                                                     <ftp://ftp.de.kernel.org/pub/linux/daemons/ntp/PPS/>
'2'   01-04  linux/i2o.h
'3'   00-0F  drivers/s390/char/raw3270.h                             conflict!
'3'   00-1F  linux/suspend_ioctls.h,                                 conflict!
             kernel/power/user.c
'8'   all                                                            SNP8023 advanced NIC card
                                                                     <mailto:mcr@solidum.com>
';'   64-7F  linux/vfio.h
'@'   00-0F  linux/radeonfb.h                                        conflict!
'@'   00-0F  drivers/video/aty/aty128fb.c                            conflict!
'A'   00-1F  linux/apm_bios.h                                        conflict!
'A'   00-0F  linux/agpgart.h,                                        conflict!
             drivers/char/agp/compat_ioctl.h
'A'   00-7F  sound/asound.h                                          conflict!
'B'   00-1F  linux/cciss_ioctl.h                                     conflict!
'B'   00-0F  include/linux/pmu.h                                     conflict!
'B'   C0-FF  advanced bbus                                           <mailto:maassen@uni-freiburg.de>
'C'   all    linux/soundcard.h                                       conflict!
'C'   01-2F  linux/capi.h                                            conflict!
'C'   F0-FF  drivers/net/wan/cosa.h                                  conflict!
'D'   all    arch/s390/include/asm/dasd.h
'D'   40-5F  drivers/scsi/dpt/dtpi_ioctl.h
'D'   05     drivers/scsi/pmcraid.h
'E'   all    linux/input.h                                           conflict!
'E'   00-0F  xen/evtchn.h                                            conflict!
'F'   all    linux/fb.h                                              conflict!
'F'   01-02  drivers/scsi/pmcraid.h                                  conflict!
'F'   20     drivers/video/fsl-diu-fb.h                              conflict!
'F'   20     drivers/video/intelfb/intelfb.h                         conflict!
'F'   20     linux/ivtvfb.h                                          conflict!
'F'   20     linux/matroxfb.h                                        conflict!
'F'   20     drivers/video/aty/atyfb_base.c                          conflict!
'F'   00-0F  video/da8xx-fb.h                                        conflict!
'F'   80-8F  linux/arcfb.h                                           conflict!
'F'   DD     video/sstfb.h                                           conflict!
'G'   00-3F  drivers/misc/sgi-gru/grulib.h                           conflict!
'H'   00-7F  linux/hiddev.h                                          conflict!
'H'   00-0F  linux/hidraw.h                                          conflict!
'H'   01     linux/mei.h                                             conflict!
'H'   02     linux/mei.h                                             conflict!
'H'   03     linux/mei.h                                             conflict!
'H'   00-0F  sound/asound.h                                          conflict!
'H'   20-40  sound/asound_fm.h                                       conflict!
'H'   80-8F  sound/sfnt_info.h                                       conflict!
'H'   10-8F  sound/emu10k1.h                                         conflict!
'H'   10-1F  sound/sb16_csp.h                                        conflict!
'H'   10-1F  sound/hda_hwdep.h                                       conflict!
'H'   40-4F  sound/hdspm.h                                           conflict!
'H'   40-4F  sound/hdsp.h                                            conflict!
'H'   90     sound/usb/usx2y/usb_stream.h
'H'   00-0F  uapi/misc/habanalabs.h                                  conflict!
'H'   A0     uapi/linux/usb/cdc-wdm.h
'H'   C0-F0  net/bluetooth/hci.h                                     conflict!
'H'   C0-DF  net/bluetooth/hidp/hidp.h                               conflict!
'H'   C0-DF  net/bluetooth/cmtp/cmtp.h                               conflict!
'H'   C0-DF  net/bluetooth/bnep/bnep.h                               conflict!
'H'   F1     linux/hid-roccat.h                                      <mailto:erazor_de@users.sourceforge.net>
'H'   F8-FA  sound/firewire.h
'I'   all    linux/isdn.h                                            conflict!
'I'   00-0F  drivers/isdn/divert/isdn_divert.h                       conflict!
'I'   40-4F  linux/mISDNif.h                                         conflict!
'K'   all    linux/kd.h
'L'   00-1F  linux/loop.h                                            conflict!
'L'   10-1F  drivers/scsi/mpt3sas/mpt3sas_ctl.h                      conflict!
'L'   20-2F  linux/lightnvm.h
'L'   E0-FF  linux/ppdd.h                                            encrypted disk device driver
                                                                     <http://linux01.gwdg.de/~alatham/ppdd.html>
'M'   all    linux/soundcard.h                                       conflict!
'M'   01-16  mtd/mtd-abi.h                                           conflict!
      and    drivers/mtd/mtdchar.c
'M'   01-03  drivers/scsi/megaraid/megaraid_sas.h
'M'   00-0F  drivers/video/fsl-diu-fb.h                              conflict!
'N'   00-1F  drivers/usb/scanner.h
'N'   40-7F  drivers/block/nvme.c
'O'   00-06  mtd/ubi-user.h                                          UBI
'P'   all    linux/soundcard.h                                       conflict!
'P'   60-6F  sound/sscape_ioctl.h                                    conflict!
'P'   00-0F  drivers/usb/class/usblp.c                               conflict!
'P'   01-09  drivers/misc/pci_endpoint_test.c                        conflict!
'Q'   all    linux/soundcard.h
'R'   00-1F  linux/random.h                                          conflict!
'R'   01     linux/rfkill.h                                          conflict!
'R'   C0-DF  net/bluetooth/rfcomm.h
'R'   E0     uapi/linux/fsl_mc.h
'S'   all    linux/cdrom.h                                           conflict!
'S'   80-81  scsi/scsi_ioctl.h                                       conflict!
'S'   82-FF  scsi/scsi.h                                             conflict!
'S'   00-7F  sound/asequencer.h                                      conflict!
'T'   all    linux/soundcard.h                                       conflict!
'T'   00-AF  sound/asound.h                                          conflict!
'T'   all    arch/x86/include/asm/ioctls.h                           conflict!
'T'   C0-DF  linux/if_tun.h                                          conflict!
'U'   all    sound/asound.h                                          conflict!
'U'   00-CF  linux/uinput.h                                          conflict!
'U'   00-EF  linux/usbdevice_fs.h
'U'   C0-CF  drivers/bluetooth/hci_uart.h
'V'   all    linux/vt.h                                              conflict!
'V'   all    linux/videodev2.h                                       conflict!
'V'   C0     linux/ivtvfb.h                                          conflict!
'V'   C0     linux/ivtv.h                                            conflict!
'V'   C0     media/davinci/vpfe_capture.h                            conflict!
'V'   C0     media/si4713.h                                          conflict!
'W'   00-1F  linux/watchdog.h                                        conflict!
'W'   00-1F  linux/wanrouter.h                                       conflict! (pre 3.9)
'W'   00-3F  sound/asound.h                                          conflict!
'W'   40-5F  drivers/pci/switch/switchtec.c
'W'   60-61  linux/watch_queue.h
'X'   all    fs/xfs/xfs_fs.h,                                        conflict!
             fs/xfs/linux-2.6/xfs_ioctl32.h,
             include/linux/falloc.h,
             linux/fs.h,
'X'   all    fs/ocfs2/ocfs_fs.h                                      conflict!
'X'   01     linux/pktcdvd.h                                         conflict!
'Z'   14-15  drivers/message/fusion/mptctl.h
'['   00-3F  linux/usb/tmc.h                                         USB Test and Measurement Devices
                                                                     <mailto:gregkh@linuxfoundation.org>
'a'   all    linux/atm*.h, linux/sonet.h                             ATM on linux
                                                                     <http://lrcwww.epfl.ch/>
'a'   00-0F  drivers/crypto/qat/qat_common/adf_cfg_common.h          conflict! qat driver
'b'   00-FF                                                          conflict! bit3 vme host bridge
                                                                     <mailto:natalia@nikhefk.nikhef.nl>
'c'   all    linux/cm4000_cs.h                                       conflict!
'c'   00-7F  linux/comstats.h                                        conflict!
'c'   00-7F  linux/coda.h                                            conflict!
'c'   00-1F  linux/chio.h                                            conflict!
'c'   80-9F  arch/s390/include/asm/chsc.h                            conflict!
'c'   A0-AF  arch/x86/include/asm/msr.h conflict!
'd'   00-FF  linux/char/drm/drm.h                                    conflict!
'd'   02-40  pcmcia/ds.h                                             conflict!
'd'   F0-FF  linux/digi1.h
'e'   all    linux/digi1.h                                           conflict!
'f'   00-1F  linux/ext2_fs.h                                         conflict!
'f'   00-1F  linux/ext3_fs.h                                         conflict!
'f'   00-0F  fs/jfs/jfs_dinode.h                                     conflict!
'f'   00-0F  fs/ext4/ext4.h                                          conflict!
'f'   00-0F  linux/fs.h                                              conflict!
'f'   00-0F  fs/ocfs2/ocfs2_fs.h                                     conflict!
'f'   13-27  linux/fscrypt.h
'f'   81-8F  linux/fsverity.h
'g'   00-0F  linux/usb/gadgetfs.h
'g'   20-2F  linux/usb/g_printer.h
'h'   00-7F                                                          conflict! Charon filesystem
                                                                     <mailto:zapman@interlan.net>
'h'   00-1F  linux/hpet.h                                            conflict!
'h'   80-8F  fs/hfsplus/ioctl.c
'i'   00-3F  linux/i2o-dev.h                                         conflict!
'i'   0B-1F  linux/ipmi.h                                            conflict!
'i'   80-8F  linux/i8k.h
'i'   90-9F  `linux/iio/*.h`                                         IIO
'j'   00-3F  linux/joystick.h
'k'   00-0F  linux/spi/spidev.h                                      conflict!
'k'   00-05  video/kyro.h                                            conflict!
'k'   10-17  linux/hsi/hsi_char.h                                    HSI character device
'l'   00-3F  linux/tcfs_fs.h                                         transparent cryptographic file system
                                                                     <http://web.archive.org/web/%2A/http://mikonos.dia.unisa.it/tcfs>
'l'   40-7F  linux/udf_fs_i.h                                        in development:
                                                                     <http://sourceforge.net/projects/linux-udf/>
'm'   00-09  linux/mmtimer.h                                         conflict!
'm'   all    linux/mtio.h                                            conflict!
'm'   all    linux/soundcard.h                                       conflict!
'm'   all    linux/synclink.h                                        conflict!
'm'   00-19  drivers/message/fusion/mptctl.h                         conflict!
'm'   00     drivers/scsi/megaraid/megaraid_ioctl.h                  conflict!
'n'   00-7F  linux/ncp_fs.h and fs/ncpfs/ioctl.c
'n'   80-8F  uapi/linux/nilfs2_api.h                                 NILFS2
'n'   E0-FF  linux/matroxfb.h                                        matroxfb
'o'   00-1F  fs/ocfs2/ocfs2_fs.h                                     OCFS2
'o'   00-03  mtd/ubi-user.h                                          conflict! (OCFS2 and UBI overlaps)
'o'   40-41  mtd/ubi-user.h                                          UBI
'o'   01-A1  `linux/dvb/*.h`                                         DVB
'p'   00-0F  linux/phantom.h                                         conflict! (OpenHaptics needs this)
'p'   00-1F  linux/rtc.h                                             conflict!
'p'   40-7F  linux/nvram.h
'p'   80-9F  linux/ppdev.h                                           user-space parport
                                                                     <mailto:tim@cyberelk.net>
'p'   A1-A5  linux/pps.h                                             LinuxPPS
                                                                     <mailto:giometti@linux.it>
'q'   00-1F  linux/serio.h
'q'   80-FF  linux/telephony.h                                       Internet PhoneJACK, Internet LineJACK
             linux/ixjuser.h                                         <http://web.archive.org/web/%2A/http://www.quicknet.net>
'r'   00-1F  linux/msdos_fs.h and fs/fat/dir.c
's'   all    linux/cdk.h
't'   00-7F  linux/ppp-ioctl.h
't'   80-8F  linux/isdn_ppp.h
't'   90-91  linux/toshiba.h                                         toshiba and toshiba_acpi SMM
'u'   00-1F  linux/smb_fs.h                                          gone
'u'   20-3F  linux/uvcvideo.h                                        USB video class host driver
'u'   40-4f  linux/udmabuf.h                                         userspace dma-buf misc device
'v'   00-1F  linux/ext2_fs.h                                         conflict!
'v'   00-1F  linux/fs.h                                              conflict!
'v'   00-0F  linux/sonypi.h                                          conflict!
'v'   00-0F  media/v4l2-subdev.h                                     conflict!
'v'   20-27  arch/powerpc/include/uapi/asm/vas-api.h		     VAS API
'v'   C0-FF  linux/meye.h                                            conflict!
'w'   all                                                            CERN SCI driver
'y'   00-1F                                                          packet based user level communications
                                                                     <mailto:zapman@interlan.net>
'z'   00-3F                                                          CAN bus card conflict!
                                                                     <mailto:hdstich@connectu.ulm.circular.de>
'z'   40-7F                                                          CAN bus card conflict!
                                                                     <mailto:oe@port.de>
'z'   10-4F  drivers/s390/crypto/zcrypt_api.h                        conflict!
'|'   00-7F  linux/media.h
0x80  00-1F  linux/fb.h
0x89  00-06  arch/x86/include/asm/sockios.h
0x89  0B-DF  linux/sockios.h
0x89  E0-EF  linux/sockios.h                                         SIOCPROTOPRIVATE range
0x89  E0-EF  linux/dn.h                                              PROTOPRIVATE range
0x89  F0-FF  linux/sockios.h                                         SIOCDEVPRIVATE range
0x8B  all    linux/wireless.h
0x8C  00-3F                                                          WiNRADiO driver
                                                                     <http://www.winradio.com.au/>
0x90  00     drivers/cdrom/sbpcd.h
0x92  00-0F  drivers/usb/mon/mon_bin.c
0x93  60-7F  linux/auto_fs.h
0x94  all    fs/btrfs/ioctl.h                                        Btrfs filesystem
             and linux/fs.h                                          some lifted to vfs/generic
0x97  00-7F  fs/ceph/ioctl.h                                         Ceph file system
0x99  00-0F                                                          537-Addinboard driver
                                                                     <mailto:buk@buks.ipn.de>
0xA0  all    linux/sdp/sdp.h                                         Industrial Device Project
                                                                     <mailto:kenji@bitgate.com>
0xA1  0      linux/vtpm_proxy.h                                      TPM Emulator Proxy Driver
0xA2  all    uapi/linux/acrn.h                                       ACRN hypervisor
0xA3  80-8F                                                          Port ACL  in development:
                                                                     <mailto:tlewis@mindspring.com>
0xA3  90-9F  linux/dtlk.h
0xA4  00-1F  uapi/linux/tee.h                                        Generic TEE subsystem
0xA4  00-1F  uapi/asm/sgx.h                                          <mailto:linux-sgx@vger.kernel.org>
0xA5  01     linux/surface_aggregator/cdev.h                         Microsoft Surface Platform System Aggregator
                                                                     <mailto:luzmaximilian@gmail.com>
0xA5  20-2F  linux/surface_aggregator/dtx.h                          Microsoft Surface DTX driver
                                                                     <mailto:luzmaximilian@gmail.com>
0xAA  00-3F  linux/uapi/linux/userfaultfd.h
0xAB  00-1F  linux/nbd.h
0xAC  00-1F  linux/raw.h
0xAD  00                                                             Netfilter device in development:
                                                                     <mailto:rusty@rustcorp.com.au>
0xAE  00-1F  linux/kvm.h                                             Kernel-based Virtual Machine
                                                                     <mailto:kvm@vger.kernel.org>
0xAE  40-FF  linux/kvm.h                                             Kernel-based Virtual Machine
                                                                     <mailto:kvm@vger.kernel.org>
0xAE  20-3F  linux/nitro_enclaves.h                                  Nitro Enclaves
0xAF  00-1F  linux/fsl_hypervisor.h                                  Freescale hypervisor
0xB0  all                                                            RATIO devices in development:
                                                                     <mailto:vgo@ratio.de>
0xB1  00-1F                                                          PPPoX
                                                                     <mailto:mostrows@styx.uwaterloo.ca>
0xB3  00     linux/mmc/ioctl.h
0xB4  00-0F  linux/gpio.h                                            <mailto:linux-gpio@vger.kernel.org>
0xB5  00-0F  uapi/linux/rpmsg.h                                      <mailto:linux-remoteproc@vger.kernel.org>
0xB6  all    linux/fpga-dfl.h
0xB7  all    uapi/linux/remoteproc_cdev.h                            <mailto:linux-remoteproc@vger.kernel.org>
0xB7  all    uapi/linux/nsfs.h                                       <mailto:Andrei Vagin <avagin@openvz.org>>
0xC0  00-0F  linux/usb/iowarrior.h
0xCA  00-0F  uapi/misc/cxl.h
0xCA  10-2F  uapi/misc/ocxl.h
0xCA  80-BF  uapi/scsi/cxlflash_ioctl.h
0xCB  00-1F                                                          CBM serial IEC bus in development:
                                                                     <mailto:michael.klein@puffin.lb.shuttle.de>
0xCC  00-0F  drivers/misc/ibmvmc.h                                   pseries VMC driver
0xCD  01     linux/reiserfs_fs.h
0xCE  01-02  uapi/linux/cxl_mem.h                                    Compute Express Link Memory Devices
0xCF  02     fs/cifs/ioctl.c
0xDB  00-0F  drivers/char/mwave/mwavepub.h
0xDD  00-3F                                                          ZFCP device driver see drivers/s390/scsi/
                                                                     <mailto:aherrman@de.ibm.com>
0xE5  00-3F  linux/fuse.h
0xEC  00-01  drivers/platform/chrome/cros_ec_dev.h                   ChromeOS EC driver
0xF3  00-3F  drivers/usb/misc/sisusbvga/sisusb.h                     sisfb (in development)
                                                                     <mailto:thomas@winischhofer.net>
0xF6  all                                                            LTTng Linux Trace Toolkit Next Generation
                                                                     <mailto:mathieu.desnoyers@efficios.com>
0xFD  all    linux/dm-ioctl.h
0xFE  all    linux/isst_if.h
====  =====  ======================================================= ================================================================
