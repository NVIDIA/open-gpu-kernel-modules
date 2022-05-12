// SPDX-License-Identifier: GPL-2.0
/*
 * sys_ipc() is the old de-multiplexer for the SysV IPC calls.
 *
 * This is really horribly ugly, and new architectures should just wire up
 * the individual syscalls instead.
 */
#include <linux/unistd.h>
#include <linux/syscalls.h>
#include <linux/security.h>
#include <linux/ipc_namespace.h>
#include "util.h"

#ifdef __ARCH_WANT_SYS_IPC
#include <linux/errno.h>
#include <linux/ipc.h>
#include <linux/shm.h>
#include <linux/uaccess.h>

int ksys_ipc(unsigned int call, int first, unsigned long second,
	unsigned long third, void __user * ptr, long fifth)
{
	int version, ret;

	version = call >> 16; /* hack for backward compatibility */
	call &= 0xffff;

	switch (call) {
	case SEMOP:
		return ksys_semtimedop(first, (struct sembuf __user *)ptr,
				       second, NULL);
	case SEMTIMEDOP:
		if (IS_ENABLED(CONFIG_64BIT))
			return ksys_semtimedop(first, ptr, second,
			        (const struct __kernel_timespec __user *)fifth);
		else if (IS_ENABLED(CONFIG_COMPAT_32BIT_TIME))
			return compat_ksys_semtimedop(first, ptr, second,
			        (const struct old_timespec32 __user *)fifth);
		else
			return -ENOSYS;

	case SEMGET:
		return ksys_semget(first, second, third);
	case SEMCTL: {
		unsigned long arg;
		if (!ptr)
			return -EINVAL;
		if (get_user(arg, (unsigned long __user *) ptr))
			return -EFAULT;
		return ksys_old_semctl(first, second, third, arg);
	}

	case MSGSND:
		return ksys_msgsnd(first, (struct msgbuf __user *) ptr,
				  second, third);
	case MSGRCV:
		switch (version) {
		case 0: {
			struct ipc_kludge tmp;
			if (!ptr)
				return -EINVAL;

			if (copy_from_user(&tmp,
					   (struct ipc_kludge __user *) ptr,
					   sizeof(tmp)))
				return -EFAULT;
			return ksys_msgrcv(first, tmp.msgp, second,
					   tmp.msgtyp, third);
		}
		default:
			return ksys_msgrcv(first,
					   (struct msgbuf __user *) ptr,
					   second, fifth, third);
		}
	case MSGGET:
		return ksys_msgget((key_t) first, second);
	case MSGCTL:
		return ksys_old_msgctl(first, second,
				   (struct msqid_ds __user *)ptr);

	case SHMAT:
		switch (version) {
		default: {
			unsigned long raddr;
			ret = do_shmat(first, (char __user *)ptr,
				       second, &raddr, SHMLBA);
			if (ret)
				return ret;
			return put_user(raddr, (unsigned long __user *) third);
		}
		case 1:
			/*
			 * This was the entry point for kernel-originating calls
			 * from iBCS2 in 2.2 days.
			 */
			return -EINVAL;
		}
	case SHMDT:
		return ksys_shmdt((char __user *)ptr);
	case SHMGET:
		return ksys_shmget(first, second, third);
	case SHMCTL:
		return ksys_old_shmctl(first, second,
				   (struct shmid_ds __user *) ptr);
	default:
		return -ENOSYS;
	}
}

SYSCALL_DEFINE6(ipc, unsigned int, call, int, first, unsigned long, second,
		unsigned long, third, void __user *, ptr, long, fifth)
{
	return ksys_ipc(call, first, second, third, ptr, fifth);
}
#endif

#ifdef CONFIG_COMPAT
#include <linux/compat.h>

#ifndef COMPAT_SHMLBA
#define COMPAT_SHMLBA	SHMLBA
#endif

struct compat_ipc_kludge {
	compat_uptr_t msgp;
	compat_long_t msgtyp;
};

#ifdef CONFIG_ARCH_WANT_OLD_COMPAT_IPC
int compat_ksys_ipc(u32 call, int first, int second,
	u32 third, compat_uptr_t ptr, u32 fifth)
{
	int version;
	u32 pad;

	version = call >> 16; /* hack for backward compatibility */
	call &= 0xffff;

	switch (call) {
	case SEMOP:
		/* struct sembuf is the same on 32 and 64bit :)) */
		return ksys_semtimedop(first, compat_ptr(ptr), second, NULL);
	case SEMTIMEDOP:
		if (!IS_ENABLED(CONFIG_COMPAT_32BIT_TIME))
			return -ENOSYS;
		return compat_ksys_semtimedop(first, compat_ptr(ptr), second,
						compat_ptr(fifth));
	case SEMGET:
		return ksys_semget(first, second, third);
	case SEMCTL:
		if (!ptr)
			return -EINVAL;
		if (get_user(pad, (u32 __user *) compat_ptr(ptr)))
			return -EFAULT;
		return compat_ksys_old_semctl(first, second, third, pad);

	case MSGSND:
		return compat_ksys_msgsnd(first, ptr, second, third);

	case MSGRCV: {
		void __user *uptr = compat_ptr(ptr);

		if (first < 0 || second < 0)
			return -EINVAL;

		if (!version) {
			struct compat_ipc_kludge ipck;
			if (!uptr)
				return -EINVAL;
			if (copy_from_user(&ipck, uptr, sizeof(ipck)))
				return -EFAULT;
			return compat_ksys_msgrcv(first, ipck.msgp, second,
						 ipck.msgtyp, third);
		}
		return compat_ksys_msgrcv(first, ptr, second, fifth, third);
	}
	case MSGGET:
		return ksys_msgget(first, second);
	case MSGCTL:
		return compat_ksys_old_msgctl(first, second, compat_ptr(ptr));

	case SHMAT: {
		int err;
		unsigned long raddr;

		if (version == 1)
			return -EINVAL;
		err = do_shmat(first, compat_ptr(ptr), second, &raddr,
			       COMPAT_SHMLBA);
		if (err < 0)
			return err;
		return put_user(raddr, (compat_ulong_t __user *)compat_ptr(third));
	}
	case SHMDT:
		return ksys_shmdt(compat_ptr(ptr));
	case SHMGET:
		return ksys_shmget(first, (unsigned int)second, third);
	case SHMCTL:
		return compat_ksys_old_shmctl(first, second, compat_ptr(ptr));
	}

	return -ENOSYS;
}

COMPAT_SYSCALL_DEFINE6(ipc, u32, call, int, first, int, second,
	u32, third, compat_uptr_t, ptr, u32, fifth)
{
	return compat_ksys_ipc(call, first, second, third, ptr, fifth);
}
#endif
#endif
