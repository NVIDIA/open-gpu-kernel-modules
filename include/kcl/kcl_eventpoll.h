/* SPDX-License-Identifier: GPL-2.0+ WITH Linux-syscall-note */
/*
 *  include/linux/eventpoll.h ( Efficient event polling implementation )
 *  Copyright (C) 2001,...,2006	 Davide Libenzi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */
#ifndef AMDKCL_EVENTPOLL_H
#define AMDKCL_EVENTPOLL_H

#include <linux/poll.h>

/* Copied from include/uapi/linux/eventpoll.h */
#ifndef EPOLLIN
#define EPOLLIN        0x00000001
#define EPOLLPRI       0x00000002
#define EPOLLOUT       0x00000004
#define EPOLLERR       0x00000008
#define EPOLLHUP       0x00000010
#define EPOLLRDNORM    0x00000040
#define EPOLLRDBAND    0x00000080
#define EPOLLWRNORM    0x00000100
#define EPOLLWRBAND    0x00000200
#define EPOLLMSG       0x00000400
#define EPOLLRDHUP     0x00002000
#endif
#endif
