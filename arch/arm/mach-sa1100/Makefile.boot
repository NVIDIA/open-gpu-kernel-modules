# SPDX-License-Identifier: GPL-2.0-only
ifeq ($(CONFIG_SA1111),y)
   zreladdr-y	+= 0xc0208000
else
   zreladdr-y	+= 0xc0008000
endif
params_phys-y	:= 0xc0000100
initrd_phys-y	:= 0xc0800000

