# shared make defines for Makefile and Kbuild

SRC_COMMON := ../common

NV_INCLUDE :=
NV_INCDIRS :=
NV_DEFINES :=

NV_INCLUDE += $(SRC_COMMON)/sdk/nvidia/inc/cpuopsys.h

NV_INCDIRS += $(SRC_COMMON)/sdk/nvidia/inc
NV_INCDIRS += $(SRC_COMMON)/sdk/nvidia/inc/hw
NV_INCDIRS += $(SRC_COMMON)/shared/inc
NV_INCDIRS += $(SRC_COMMON)/inc
NV_INCDIRS += $(SRC_COMMON)/softfloat/nvidia
NV_INCDIRS += $(SRC_COMMON)/softfloat/source/include
NV_INCDIRS += $(SRC_COMMON)/softfloat/source/8086-SSE
NV_INCDIRS += $(SRC_COMMON)/unix/common/utils/interface
NV_INCDIRS += $(SRC_COMMON)/unix/common/inc
NV_INCDIRS += $(SRC_COMMON)/modeset
NV_INCDIRS += os-interface/include
NV_INCDIRS += kapi/interface
NV_INCDIRS += ../nvidia/arch/nvalloc/unix/include
NV_INCDIRS += interface
NV_INCDIRS += include
NV_INCDIRS += kapi/include
NV_INCDIRS += generated
NV_INCDIRS += $(SRC_COMMON)/displayport/inc
NV_INCDIRS += $(SRC_COMMON)/displayport/inc/dptestutil
NV_INCDIRS += $(SRC_COMMON)/inc/displayport

NV_DEFINES += -DNDEBUG
NV_DEFINES += -D_LANGUAGE_C
NV_DEFINES += -D__NO_CTYPE

NV_DEFINES += -DNV_CPU_INTRINSICS_KERNEL
NV_DEFINES += -DNVHDMIPKT_RM_CALLS_INTERNAL=0
NV_DEFINES += -DNVHDMIPKT_NVKMS

# XXX it would be nice to only define these for appropriate files...
NV_DEFINES += -DSOFTFLOAT_ROUND_ODD
NV_DEFINES += -DSOFTFLOAT_FAST_DIV32TO16
NV_DEFINES += -DSOFTFLOAT_FAST_DIV64TO32

# Tell nvtiming to use nvkms import functions
NV_DEFINES += -DNVT_USE_NVKMS

# Tell SMG we're being compiled into kernel
NV_DEFINES += -DNV_SMG_IN_NVKMS

NV_INCDIRS += $(SRC_COMMON)/unix/nvidia-3d/interface
NV_INCDIRS += $(SRC_COMMON)/unix/nvidia-push/interface
NV_INCDIRS += $(SRC_COMMON)/unix/nvidia-3d/include
NV_INCDIRS += $(SRC_COMMON)/unix/nvidia-push/include
NV_INCDIRS += $(SRC_COMMON)/unix/xzminidec/interface
NV_INCDIRS += $(SRC_COMMON)/unix/nvidia-headsurface
NV_INCDIRS += src/shaders

NV_DEFINES += -DNV_PUSH_IN_KERNEL
NV_DEFINES += -DNV_XZ_CUSTOM_MEM_HOOKS
NV_DEFINES += -DNV_XZ_USE_NVTYPES
NV_DEFINES += -DXZ_DEC_SINGLE
