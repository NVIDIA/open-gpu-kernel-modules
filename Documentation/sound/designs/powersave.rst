==========================
Notes on Power-Saving Mode
==========================

AC97 and HD-audio drivers have the automatic power-saving mode.
This feature is enabled via Kconfig ``CONFIG_SND_AC97_POWER_SAVE``
and ``CONFIG_SND_HDA_POWER_SAVE`` options, respectively.

With the automatic power-saving, the driver turns off the codec power
appropriately when no operation is required.  When no applications use
the device and/or no analog loopback is set, the power disablement is
done fully or partially.  It'll save a certain power consumption, thus
good for laptops (even for desktops).

The time-out for automatic power-off can be specified via ``power_save``
module option of snd-ac97-codec and snd-hda-intel modules.  Specify
the time-out value in seconds.  0 means to disable the automatic
power-saving.  The default value of timeout is given via
``CONFIG_SND_AC97_POWER_SAVE_DEFAULT`` and
``CONFIG_SND_HDA_POWER_SAVE_DEFAULT`` Kconfig options.  Setting this to 1
(the minimum value) isn't recommended because many applications try to
reopen the device frequently.  10 would be a good choice for normal
operations.

The ``power_save`` option is exported as writable.  This means you can
adjust the value via sysfs on the fly.  For example, to turn on the
automatic power-save mode with 10 seconds, write to
``/sys/modules/snd_ac97_codec/parameters/power_save`` (usually as root):
::

	# echo 10 > /sys/modules/snd_ac97_codec/parameters/power_save


Note that you might hear click noise/pop when changing the power
state.  Also, it often takes certain time to wake up from the
power-down to the active state.  These are often hardly to fix, so
don't report extra bug reports unless you have a fix patch ;-)

For HD-audio interface, there is another module option,
power_save_controller.  This enables/disables the power-save mode of
the controller side.  Setting this on may reduce a bit more power
consumption, but might result in longer wake-up time and click noise.
Try to turn it off when you experience such a thing too often.
