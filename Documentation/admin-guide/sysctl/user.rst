=================================
Documentation for /proc/sys/user/
=================================

kernel version 4.9.0

Copyright (c) 2016		Eric Biederman <ebiederm@xmission.com>

------------------------------------------------------------------------------

This file contains the documentation for the sysctl files in
/proc/sys/user.

The files in this directory can be used to override the default
limits on the number of namespaces and other objects that have
per user per user namespace limits.

The primary purpose of these limits is to stop programs that
malfunction and attempt to create a ridiculous number of objects,
before the malfunction becomes a system wide problem.  It is the
intention that the defaults of these limits are set high enough that
no program in normal operation should run into these limits.

The creation of per user per user namespace objects are charged to
the user in the user namespace who created the object and
verified to be below the per user limit in that user namespace.

The creation of objects is also charged to all of the users
who created user namespaces the creation of the object happens
in (user namespaces can be nested) and verified to be below the per user
limits in the user namespaces of those users.

This recursive counting of created objects ensures that creating a
user namespace does not allow a user to escape their current limits.

Currently, these files are in /proc/sys/user:

max_cgroup_namespaces
=====================

  The maximum number of cgroup namespaces that any user in the current
  user namespace may create.

max_ipc_namespaces
==================

  The maximum number of ipc namespaces that any user in the current
  user namespace may create.

max_mnt_namespaces
==================

  The maximum number of mount namespaces that any user in the current
  user namespace may create.

max_net_namespaces
==================

  The maximum number of network namespaces that any user in the
  current user namespace may create.

max_pid_namespaces
==================

  The maximum number of pid namespaces that any user in the current
  user namespace may create.

max_time_namespaces
===================

  The maximum number of time namespaces that any user in the current
  user namespace may create.

max_user_namespaces
===================

  The maximum number of user namespaces that any user in the current
  user namespace may create.

max_uts_namespaces
==================

  The maximum number of user namespaces that any user in the current
  user namespace may create.
