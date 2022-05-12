Kernel Driver Lochnagar
=======================

Supported systems:
  * Cirrus Logic : Lochnagar 2

Author: Lucas A. Tanure Alves

Description
-----------

Lochnagar 2 features built-in Current Monitor circuitry that allows for the
measurement of both voltage and current on up to eight of the supply voltage
rails provided to the minicards. The Current Monitor does not require any
hardware modifications or external circuitry to operate.

The current and voltage measurements are obtained through the standard register
map interface to the Lochnagar board controller, and can therefore be monitored
by software.

Sysfs attributes
----------------

======================= =======================================================
temp1_input             The Lochnagar board temperature (milliCelsius)
in0_input               Measured voltage for DBVDD1 (milliVolts)
in0_label               "DBVDD1"
curr1_input             Measured current for DBVDD1 (milliAmps)
curr1_label             "DBVDD1"
power1_average          Measured average power for DBVDD1 (microWatts)
power1_average_interval Power averaging time input valid from 1 to 1708mS
power1_label            "DBVDD1"
in1_input               Measured voltage for 1V8 DSP (milliVolts)
in1_label               "1V8 DSP"
curr2_input             Measured current for 1V8 DSP (milliAmps)
curr2_label             "1V8 DSP"
power2_average          Measured average power for 1V8 DSP (microWatts)
power2_average_interval Power averaging time input valid from 1 to 1708mS
power2_label            "1V8 DSP"
in2_input               Measured voltage for 1V8 CDC (milliVolts)
in2_label               "1V8 CDC"
curr3_input             Measured current for 1V8 CDC (milliAmps)
curr3_label             "1V8 CDC"
power3_average          Measured average power for 1V8 CDC (microWatts)
power3_average_interval Power averaging time input valid from 1 to 1708mS
power3_label            "1V8 CDC"
in3_input               Measured voltage for VDDCORE DSP (milliVolts)
in3_label               "VDDCORE DSP"
curr4_input             Measured current for VDDCORE DSP (milliAmps)
curr4_label             "VDDCORE DSP"
power4_average          Measured average power for VDDCORE DSP (microWatts)
power4_average_interval Power averaging time input valid from 1 to 1708mS
power4_label            "VDDCORE DSP"
in4_input               Measured voltage for AVDD 1V8 (milliVolts)
in4_label               "AVDD 1V8"
curr5_input             Measured current for AVDD 1V8 (milliAmps)
curr5_label             "AVDD 1V8"
power5_average          Measured average power for AVDD 1V8 (microWatts)
power5_average_interval Power averaging time input valid from 1 to 1708mS
power5_label            "AVDD 1V8"
curr6_input             Measured current for SYSVDD (milliAmps)
curr6_label             "SYSVDD"
power6_average          Measured average power for SYSVDD (microWatts)
power6_average_interval Power averaging time input valid from 1 to 1708mS
power6_label            "SYSVDD"
in6_input               Measured voltage for VDDCORE CDC (milliVolts)
in6_label               "VDDCORE CDC"
curr7_input             Measured current for VDDCORE CDC (milliAmps)
curr7_label             "VDDCORE CDC"
power7_average          Measured average power for VDDCORE CDC (microWatts)
power7_average_interval Power averaging time input valid from 1 to 1708mS
power7_label            "VDDCORE CDC"
in7_input               Measured voltage for MICVDD (milliVolts)
in7_label               "MICVDD"
curr8_input             Measured current for MICVDD (milliAmps)
curr8_label             "MICVDD"
power8_average          Measured average power for MICVDD (microWatts)
power8_average_interval Power averaging time input valid from 1 to 1708mS
power8_label            "MICVDD"
======================= =======================================================

Note:
    It is not possible to measure voltage on the SYSVDD rail.
