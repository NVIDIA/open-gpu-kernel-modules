###########################################################################
# Makefile for nv-modeset-kernel.o
###########################################################################

NV_MODULE_LOGGING_NAME ?= nvidia-modeset

VERSION_MK_DIR = ../../
include ../../utils.mk

include srcs.mk

# The source files for nv-modeset-kernel.o are all SRCS and SRCS_CXX defined in
# srcs.mk, and the NVIDIA ID string
ALL_SRCS = $(SRCS) $(SRCS_CXX)
ALL_SRCS += $(NVIDSTRING)

SRC_COMMON = ../common

CFLAGS += -include $(SRC_COMMON)/sdk/nvidia/inc/cpuopsys.h

CFLAGS += -I $(SRC_COMMON)/sdk/nvidia/inc
CFLAGS += -I $(SRC_COMMON)/shared/inc
CFLAGS += -I $(SRC_COMMON)/inc
CFLAGS += -I $(SRC_COMMON)/softfloat/nvidia
CFLAGS += -I $(SRC_COMMON)/softfloat/source/include
CFLAGS += -I $(SRC_COMMON)/softfloat/source/8086-SSE
CFLAGS += -I $(SRC_COMMON)/unix/common/utils/interface
CFLAGS += -I $(SRC_COMMON)/unix/common/inc
CFLAGS += -I $(SRC_COMMON)/modeset
CFLAGS += -I os-interface/include
CFLAGS += -I kapi/interface
CFLAGS += -I ../nvidia/arch/nvalloc/unix/include
CFLAGS += -I interface
CFLAGS += -I include
CFLAGS += -I kapi/include
CFLAGS += -I generated
CFLAGS += -I $(SRC_COMMON)/displayport/inc
CFLAGS += -I $(SRC_COMMON)/displayport/inc/dptestutil
CFLAGS += -I $(SRC_COMMON)/inc/displayport

CFLAGS += -DNDEBUG
CFLAGS += -D_LANGUAGE_C
CFLAGS += -D__NO_CTYPE

CFLAGS += -DNV_CPU_INTRINSICS_KERNEL
CFLAGS += -DNVHDMIPKT_RM_CALLS_INTERNAL=0

# XXX it would be nice to only define these for appropriate files...
CFLAGS += -DSOFTFLOAT_ROUND_ODD
CFLAGS += -DSOFTFLOAT_FAST_DIV32TO16
CFLAGS += -DSOFTFLOAT_FAST_DIV64TO32

# Tell nvtiming to use nvkms import functions
CFLAGS += -DNVT_USE_NVKMS

CFLAGS += -Wformat
CFLAGS += -Wreturn-type
CFLAGS += -Wswitch
CFLAGS += -Wunused-local-typedefs
CFLAGS += -Wchar-subscripts
CFLAGS += -Wparentheses
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-qual
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-missing-field-initializers
CFLAGS += -Wno-format-zero-length
CFLAGS += -Wmissing-declarations
CFLAGS += -Wno-cast-qual

CFLAGS += -O2

ifeq ($(TARGET_ARCH),x86_64)
  CFLAGS += -msoft-float
  CFLAGS += -mno-red-zone
  CFLAGS += -mcmodel=kernel
  CFLAGS += -mno-mmx
  CFLAGS += -mno-sse
  CFLAGS += -mno-sse2
  CFLAGS += -mno-3dnow
endif

ifeq ($(TARGET_ARCH),aarch64)
  CFLAGS += -mgeneral-regs-only
  CFLAGS += -march=armv8-a
endif

CFLAGS += -fno-pic
CFLAGS += -fno-common
CFLAGS += -fomit-frame-pointer
CFLAGS += -fno-strict-aliasing
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -ffreestanding

CONDITIONAL_CFLAGS := $(call TEST_CC_ARG, -fcf-protection=none)
CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -Wformat-overflow=2)
CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -Wformat-truncation=1)
ifeq ($(TARGET_ARCH),x86_64)
  CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -mindirect-branch=thunk-extern)
  CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -mindirect-branch-register)
endif

CFLAGS += $(CONDITIONAL_CFLAGS)

CC_ONLY_CFLAGS += -Wimplicit
CC_ONLY_CFLAGS += -Wstrict-prototypes
CC_ONLY_CFLAGS += -Wmissing-prototypes
CC_ONLY_CFLAGS += -std=gnu11

CXX_ONLY_CFLAGS += -std=gnu++11
CXX_ONLY_CFLAGS += -fno-operator-names
CXX_ONLY_CFLAGS += -fno-rtti
CXX_ONLY_CFLAGS += -fno-exceptions
CXX_ONLY_CFLAGS += -fcheck-new

SHADER_OBJS =

  CFLAGS += -DNVKMS_INCLUDE_HEADSURFACE=0

OBJS = $(call BUILD_OBJECT_LIST,$(ALL_SRCS))
OBJS += $(SHADER_OBJS)

# Define how to generate the NVIDIA ID string
$(eval $(call GENERATE_NVIDSTRING, \
  NV_KMS_ID, \
  UNIX Open Kernel Mode Setting Driver, $(OBJS)))

# Define how to build each object file from the corresponding source file.
$(foreach src, $(ALL_SRCS), $(eval $(call DEFINE_OBJECT_RULE,TARGET,$(src))))

NV_MODESET_KERNEL_O = $(OUTPUTDIR)/nv-modeset-kernel.o

.PNONY: all clean
all: $(NV_MODESET_KERNEL_O)

$(NV_MODESET_KERNEL_O): $(OBJS)
	$(call quiet_cmd,LD) -r -o $(NV_MODESET_KERNEL_O) $(OBJS)

clean:
	$(RM) -rf $(OUTPUTDIR)
