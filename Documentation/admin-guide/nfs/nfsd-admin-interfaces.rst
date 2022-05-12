==================================
Administrative interfaces for nfsd
==================================

Note that normally these interfaces are used only by the utilities in
nfs-utils.

nfsd is controlled mainly by pseudofiles under the "nfsd" filesystem,
which is normally mounted at /proc/fs/nfsd/.

The server is always started by the first write of a nonzero value to
nfsd/threads.

Before doing that, NFSD can be told which sockets to listen on by
writing to nfsd/portlist; that write may be:

	-  an ascii-encoded file descriptor, which should refer to a
	   bound (and listening, for tcp) socket, or
	-  "transportname port", where transportname is currently either
	   "udp", "tcp", or "rdma".

If nfsd is started without doing any of these, then it will create one
udp and one tcp listener at port 2049 (see nfsd_init_socks).

On startup, nfsd and lockd grace periods start. nfsd is shut down by a write of
0 to nfsd/threads.  All locks and state are thrown away at that point.

Between startup and shutdown, the number of threads may be adjusted up
or down by additional writes to nfsd/threads or by writes to
nfsd/pool_threads.

For more detail about files under nfsd/ and what they control, see
fs/nfsd/nfsctl.c; most of them have detailed comments.

Implementation notes
====================

Note that the rpc server requires the caller to serialize addition and
removal of listening sockets, and startup and shutdown of the server.
For nfsd this is done using nfsd_mutex.
