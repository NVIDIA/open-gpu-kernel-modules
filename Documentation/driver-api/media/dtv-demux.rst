.. SPDX-License-Identifier: GPL-2.0

Digital TV Demux kABI
---------------------

Digital TV Demux
~~~~~~~~~~~~~~~~

The Kernel Digital TV Demux kABI defines a driver-internal interface for
registering low-level, hardware specific driver to a hardware independent
demux layer. It is only of interest for Digital TV device driver writers.
The header file for this kABI is named ``demux.h`` and located in
``include/media``.

The demux kABI should be implemented for each demux in the system. It is
used to select the TS source of a demux and to manage the demux resources.
When the demux client allocates a resource via the demux kABI, it receives
a pointer to the kABI of that resource.

Each demux receives its TS input from a DVB front-end or from memory, as
set via this demux kABI. In a system with more than one front-end, the kABI
can be used to select one of the DVB front-ends as a TS source for a demux,
unless this is fixed in the HW platform.

The demux kABI only controls front-ends regarding to their connections with
demuxes; the kABI used to set the other front-end parameters, such as
tuning, are devined via the Digital TV Frontend kABI.

The functions that implement the abstract interface demux should be defined
static or module private and registered to the Demux core for external
access. It is not necessary to implement every function in the struct
:c:type:`dmx_demux`. For example, a demux interface might support Section filtering,
but not PES filtering. The kABI client is expected to check the value of any
function pointer before calling the function: the value of ``NULL`` means
that the function is not available.

Whenever the functions of the demux API modify shared data, the
possibilities of lost update and race condition problems should be
addressed, e.g. by protecting parts of code with mutexes.

Note that functions called from a bottom half context must not sleep.
Even a simple memory allocation without using ``GFP_ATOMIC`` can result in a
kernel thread being put to sleep if swapping is needed. For example, the
Linux Kernel calls the functions of a network device interface from a
bottom half context. Thus, if a demux kABI function is called from network
device code, the function must not sleep.

Demux Callback API
~~~~~~~~~~~~~~~~~~

This kernel-space API comprises the callback functions that deliver filtered
data to the demux client. Unlike the other DVB kABIs, these functions are
provided by the client and called from the demux code.

The function pointers of this abstract interface are not packed into a
structure as in the other demux APIs, because the callback functions are
registered and used independent of each other. As an example, it is possible
for the API client to provide several callback functions for receiving TS
packets and no callbacks for PES packets or sections.

The functions that implement the callback API need not be re-entrant: when
a demux driver calls one of these functions, the driver is not allowed to
call the function again before the original call returns. If a callback is
triggered by a hardware interrupt, it is recommended to use the Linux
bottom half mechanism or start a tasklet instead of making the callback
function call directly from a hardware interrupt.

This mechanism is implemented by :c:func:`dmx_ts_cb()` and :c:func:`dmx_section_cb()`
callbacks.

Digital TV Demux device registration functions and data structures
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. kernel-doc:: include/media/dmxdev.h

High-level Digital TV demux interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. kernel-doc:: include/media/dvb_demux.h

Driver-internal low-level hardware specific driver demux interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. kernel-doc:: include/media/demux.h
