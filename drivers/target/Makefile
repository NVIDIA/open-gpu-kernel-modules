# SPDX-License-Identifier: GPL-2.0

target_core_mod-y		:= target_core_configfs.o \
				   target_core_device.o \
				   target_core_fabric_configfs.o \
				   target_core_fabric_lib.o \
				   target_core_hba.o \
				   target_core_pr.o \
				   target_core_alua.o \
				   target_core_tmr.o \
				   target_core_tpg.o \
				   target_core_transport.o \
				   target_core_sbc.o \
				   target_core_spc.o \
				   target_core_ua.o \
				   target_core_rd.o \
				   target_core_stat.o \
				   target_core_xcopy.o

obj-$(CONFIG_TARGET_CORE)	+= target_core_mod.o

# Subsystem modules
obj-$(CONFIG_TCM_IBLOCK)	+= target_core_iblock.o
obj-$(CONFIG_TCM_FILEIO)	+= target_core_file.o
obj-$(CONFIG_TCM_PSCSI)		+= target_core_pscsi.o
obj-$(CONFIG_TCM_USER2)		+= target_core_user.o

# Fabric modules
obj-$(CONFIG_LOOPBACK_TARGET)	+= loopback/
obj-$(CONFIG_TCM_FC)		+= tcm_fc/
obj-$(CONFIG_ISCSI_TARGET)	+= iscsi/
obj-$(CONFIG_SBP_TARGET)	+= sbp/
