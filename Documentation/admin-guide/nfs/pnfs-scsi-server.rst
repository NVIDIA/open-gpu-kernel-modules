
==================================
pNFS SCSI layout server user guide
==================================

This document describes support for pNFS SCSI layouts in the Linux NFS server.
With pNFS SCSI layouts, the NFS server acts as Metadata Server (MDS) for pNFS,
which in addition to handling all the metadata access to the NFS export,
also hands out layouts to the clients so that they can directly access the
underlying SCSI LUNs that are shared with the client.

To use pNFS SCSI layouts with the Linux NFS server, the exported file
system needs to support the pNFS SCSI layouts (currently just XFS), and the
file system must sit on a SCSI LUN that is accessible to the clients in
addition to the MDS.  As of now the file system needs to sit directly on the
exported LUN, striping or concatenation of LUNs on the MDS and clients
is not supported yet.

On a server built with CONFIG_NFSD_SCSI, the pNFS SCSI volume support is
automatically enabled if the file system is exported using the "pnfs"
option and the underlying SCSI device support persistent reservations.
On the client make sure the kernel has the CONFIG_PNFS_BLOCK option
enabled, and the file system is mounted using the NFSv4.1 protocol
version (mount -o vers=4.1).
