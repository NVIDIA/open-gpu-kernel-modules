=======================
HD-Audio DP-MST Support
=======================

To support DP MST audio, HD Audio hdmi codec driver introduces virtual pin
and dynamic pcm assignment.

Virtual pin is an extension of per_pin. The most difference of DP MST
from legacy is that DP MST introduces device entry. Each pin can contain
several device entries. Each device entry behaves as a pin.

As each pin may contain several device entries and each codec may contain
several pins, if we use one pcm per per_pin, there will be many PCMs.
The new solution is to create a few PCMs and to dynamically bind pcm to
per_pin. Driver uses spec->dyn_pcm_assign flag to indicate whether to use
the new solution.

PCM
===
To be added

Pin Initialization
==================
Each pin may have several device entries (virtual pins). On Intel platform,
the device entries number is dynamically changed. If DP MST hub is connected,
it is in DP MST mode, and the device entries number is 3. Otherwise, the
device entries number is 1.

To simplify the implementation, all the device entries will be initialized
when bootup no matter whether it is in DP MST mode or not.

Connection list
===============
DP MST reuses connection list code. The code can be reused because
device entries on the same pin have the same connection list.

This means DP MST gets the device entry connection list without the
device entry setting.

Jack
====

Presume:
 - MST must be dyn_pcm_assign, and it is acomp (for Intel scenario);
 - NON-MST may or may not be dyn_pcm_assign, it can be acomp or !acomp;

So there are the following scenarios:
 a. MST (&& dyn_pcm_assign && acomp)
 b. NON-MST && dyn_pcm_assign && acomp
 c. NON-MST && !dyn_pcm_assign && !acomp

Below discussion will ignore MST and NON-MST difference as it doesn't
impact on jack handling too much.

Driver uses struct hdmi_pcm pcm[] array in hdmi_spec and snd_jack is
a member of hdmi_pcm. Each pin has one struct hdmi_pcm * pcm pointer.

For !dyn_pcm_assign, per_pin->pcm will assigned to spec->pcm[n] statically.

For dyn_pcm_assign, per_pin->pcm will assigned to spec->pcm[n]
when monitor is hotplugged.


Build Jack
----------

- dyn_pcm_assign

  Will not use hda_jack but use snd_jack in spec->pcm_rec[pcm_idx].jack directly.

- !dyn_pcm_assign

  Use hda_jack and assign spec->pcm_rec[pcm_idx].jack = jack->jack statically.


Unsolicited Event Enabling
--------------------------
Enable unsolicited event if !acomp.


Monitor Hotplug Event Handling
------------------------------
- acomp

  pin_eld_notify() -> check_presence_and_report() -> hdmi_present_sense() ->
  sync_eld_via_acomp().

  Use directly snd_jack_report() on spec->pcm_rec[pcm_idx].jack for
  both dyn_pcm_assign and !dyn_pcm_assign

- !acomp

  hdmi_unsol_event() -> hdmi_intrinsic_event() -> check_presence_and_report() ->
  hdmi_present_sense() -> hdmi_prepsent_sense_via_verbs()

  Use directly snd_jack_report() on spec->pcm_rec[pcm_idx].jack for dyn_pcm_assign.
  Use hda_jack mechanism to handle jack events.


Others to be added later
========================
