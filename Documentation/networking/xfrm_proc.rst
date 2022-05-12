.. SPDX-License-Identifier: GPL-2.0

==================================
XFRM proc - /proc/net/xfrm_* files
==================================

Masahide NAKAMURA <nakam@linux-ipv6.org>


Transformation Statistics
-------------------------

The xfrm_proc code is a set of statistics showing numbers of packets
dropped by the transformation code and why.  These counters are defined
as part of the linux private MIB.  These counters can be viewed in
/proc/net/xfrm_stat.


Inbound errors
~~~~~~~~~~~~~~

XfrmInError:
	All errors which is not matched others

XfrmInBufferError:
	No buffer is left

XfrmInHdrError:
	Header error

XfrmInNoStates:
	No state is found
	i.e. Either inbound SPI, address, or IPsec protocol at SA is wrong

XfrmInStateProtoError:
	Transformation protocol specific error
	e.g. SA key is wrong

XfrmInStateModeError:
	Transformation mode specific error

XfrmInStateSeqError:
	Sequence error
	i.e. Sequence number is out of window

XfrmInStateExpired:
	State is expired

XfrmInStateMismatch:
	State has mismatch option
	e.g. UDP encapsulation type is mismatch

XfrmInStateInvalid:
	State is invalid

XfrmInTmplMismatch:
	No matching template for states
	e.g. Inbound SAs are correct but SP rule is wrong

XfrmInNoPols:
	No policy is found for states
	e.g. Inbound SAs are correct but no SP is found

XfrmInPolBlock:
	Policy discards

XfrmInPolError:
	Policy error

XfrmAcquireError:
	State hasn't been fully acquired before use

XfrmFwdHdrError:
	Forward routing of a packet is not allowed

Outbound errors
~~~~~~~~~~~~~~~
XfrmOutError:
	All errors which is not matched others

XfrmOutBundleGenError:
	Bundle generation error

XfrmOutBundleCheckError:
	Bundle check error

XfrmOutNoStates:
	No state is found

XfrmOutStateProtoError:
	Transformation protocol specific error

XfrmOutStateModeError:
	Transformation mode specific error

XfrmOutStateSeqError:
	Sequence error
	i.e. Sequence number overflow

XfrmOutStateExpired:
	State is expired

XfrmOutPolBlock:
	Policy discards

XfrmOutPolDead:
	Policy is dead

XfrmOutPolError:
	Policy error

XfrmOutStateInvalid:
	State is invalid, perhaps expired
