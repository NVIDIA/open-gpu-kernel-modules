===========================================
IBM's Journaled File System (JFS) for Linux
===========================================

JFS Homepage:  http://jfs.sourceforge.net/

The following mount options are supported:

(*) == default

iocharset=name
                Character set to use for converting from Unicode to
		ASCII.  The default is to do no conversion.  Use
		iocharset=utf8 for UTF-8 translations.  This requires
		CONFIG_NLS_UTF8 to be set in the kernel .config file.
		iocharset=none specifies the default behavior explicitly.

resize=value
                Resize the volume to <value> blocks.  JFS only supports
		growing a volume, not shrinking it.  This option is only
		valid during a remount, when the volume is mounted
		read-write.  The resize keyword with no value will grow
		the volume to the full size of the partition.

nointegrity
                Do not write to the journal.  The primary use of this option
		is to allow for higher performance when restoring a volume
		from backup media.  The integrity of the volume is not
		guaranteed if the system abnormally abends.

integrity(*)
                Commit metadata changes to the journal.  Use this option to
		remount a volume where the nointegrity option was
		previously specified in order to restore normal behavior.

errors=continue
                        Keep going on a filesystem error.
errors=remount-ro(*)
                        Remount the filesystem read-only on an error.
errors=panic
                        Panic and halt the machine if an error occurs.

uid=value
                Override on-disk uid with specified value
gid=value
                Override on-disk gid with specified value
umask=value
                Override on-disk umask with specified octal value. For
                directories, the execute bit will be set if the corresponding
		read bit is set.

discard=minlen, discard/nodiscard(*)
                This enables/disables the use of discard/TRIM commands.
		The discard/TRIM commands are sent to the underlying
                block device when blocks are freed. This is useful for SSD
                devices and sparse/thinly-provisioned LUNs.  The FITRIM ioctl
		command is also available together with the nodiscard option.
		The value of minlen specifies the minimum blockcount, when
		a TRIM command to the block device is considered useful.
		When no value is given to the discard option, it defaults to
		64 blocks, which means 256KiB in JFS.
		The minlen value of discard overrides the minlen value given
		on an FITRIM ioctl().

The JFS mailing list can be subscribed to by using the link labeled
"Mail list Subscribe" at our web page http://jfs.sourceforge.net/
