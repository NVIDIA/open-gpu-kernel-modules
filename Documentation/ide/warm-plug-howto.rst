===================
IDE warm-plug HOWTO
===================

To warm-plug devices on a port 'idex'::

	# echo -n "1" > /sys/class/ide_port/idex/delete_devices

unplug old device(s) and plug new device(s)::

	# echo -n "1" > /sys/class/ide_port/idex/scan

done

NOTE: please make sure that partitions are unmounted and that there are
no other active references to devices before doing "delete_devices" step,
also do not attempt "scan" step on devices currently in use -- otherwise
results may be unpredictable and lead to data loss if you're unlucky
