Video Output Switcher Control
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

2006 luming.yu@intel.com

The output sysfs class driver provides an abstract video output layer that
can be used to hook platform specific methods to enable/disable video output
device through common sysfs interface. For example, on my IBM ThinkPad T42
laptop, The ACPI video driver registered its output devices and read/write
method for 'state' with output sysfs class. The user interface under sysfs is::

  linux:/sys/class/video_output # tree .
  .
  |-- CRT0
  |   |-- device -> ../../../devices/pci0000:00/0000:00:01.0
  |   |-- state
  |   |-- subsystem -> ../../../class/video_output
  |   `-- uevent
  |-- DVI0
  |   |-- device -> ../../../devices/pci0000:00/0000:00:01.0
  |   |-- state
  |   |-- subsystem -> ../../../class/video_output
  |   `-- uevent
  |-- LCD0
  |   |-- device -> ../../../devices/pci0000:00/0000:00:01.0
  |   |-- state
  |   |-- subsystem -> ../../../class/video_output
  |   `-- uevent
  `-- TV0
     |-- device -> ../../../devices/pci0000:00/0000:00:01.0
     |-- state
     |-- subsystem -> ../../../class/video_output
     `-- uevent

