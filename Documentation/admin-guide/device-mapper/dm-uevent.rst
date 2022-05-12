====================
device-mapper uevent
====================

The device-mapper uevent code adds the capability to device-mapper to create
and send kobject uevents (uevents).  Previously device-mapper events were only
available through the ioctl interface.  The advantage of the uevents interface
is the event contains environment attributes providing increased context for
the event avoiding the need to query the state of the device-mapper device after
the event is received.

There are two functions currently for device-mapper events.  The first function
listed creates the event and the second function sends the event(s)::

  void dm_path_uevent(enum dm_uevent_type event_type, struct dm_target *ti,
                      const char *path, unsigned nr_valid_paths)

  void dm_send_uevents(struct list_head *events, struct kobject *kobj)


The variables added to the uevent environment are:

Variable Name: DM_TARGET
------------------------
:Uevent Action(s): KOBJ_CHANGE
:Type: string
:Description:
:Value: Name of device-mapper target that generated the event.

Variable Name: DM_ACTION
------------------------
:Uevent Action(s): KOBJ_CHANGE
:Type: string
:Description:
:Value: Device-mapper specific action that caused the uevent action.
	PATH_FAILED - A path has failed;
	PATH_REINSTATED - A path has been reinstated.

Variable Name: DM_SEQNUM
------------------------
:Uevent Action(s): KOBJ_CHANGE
:Type: unsigned integer
:Description: A sequence number for this specific device-mapper device.
:Value: Valid unsigned integer range.

Variable Name: DM_PATH
----------------------
:Uevent Action(s): KOBJ_CHANGE
:Type: string
:Description: Major and minor number of the path device pertaining to this
	      event.
:Value: Path name in the form of "Major:Minor"

Variable Name: DM_NR_VALID_PATHS
--------------------------------
:Uevent Action(s): KOBJ_CHANGE
:Type: unsigned integer
:Description:
:Value: Valid unsigned integer range.

Variable Name: DM_NAME
----------------------
:Uevent Action(s): KOBJ_CHANGE
:Type: string
:Description: Name of the device-mapper device.
:Value: Name

Variable Name: DM_UUID
----------------------
:Uevent Action(s): KOBJ_CHANGE
:Type: string
:Description: UUID of the device-mapper device.
:Value: UUID. (Empty string if there isn't one.)

An example of the uevents generated as captured by udevmonitor is shown
below

1.) Path failure::

	UEVENT[1192521009.711215] change@/block/dm-3
	ACTION=change
	DEVPATH=/block/dm-3
	SUBSYSTEM=block
	DM_TARGET=multipath
	DM_ACTION=PATH_FAILED
	DM_SEQNUM=1
	DM_PATH=8:32
	DM_NR_VALID_PATHS=0
	DM_NAME=mpath2
	DM_UUID=mpath-35333333000002328
	MINOR=3
	MAJOR=253
	SEQNUM=1130

2.) Path reinstate::

	UEVENT[1192521132.989927] change@/block/dm-3
	ACTION=change
	DEVPATH=/block/dm-3
	SUBSYSTEM=block
	DM_TARGET=multipath
	DM_ACTION=PATH_REINSTATED
	DM_SEQNUM=2
	DM_PATH=8:32
	DM_NR_VALID_PATHS=1
	DM_NAME=mpath2
	DM_UUID=mpath-35333333000002328
	MINOR=3
	MAJOR=253
	SEQNUM=1131
