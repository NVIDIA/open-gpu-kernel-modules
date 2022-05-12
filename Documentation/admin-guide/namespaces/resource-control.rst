===========================
Namespaces research control
===========================

There are a lot of kinds of objects in the kernel that don't have
individual limits or that have limits that are ineffective when a set
of processes is allowed to switch user ids.  With user namespaces
enabled in a kernel for people who don't trust their users or their
users programs to play nice this problems becomes more acute.

Therefore it is recommended that memory control groups be enabled in
kernels that enable user namespaces, and it is further recommended
that userspace configure memory control groups to limit how much
memory user's they don't trust to play nice can use.

Memory control groups can be configured by installing the libcgroup
package present on most distros editing /etc/cgrules.conf,
/etc/cgconfig.conf and setting up libpam-cgroup.
