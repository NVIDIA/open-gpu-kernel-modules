###########################################################################
# Makefile for nv-kernel.o
###########################################################################

NV_MODULE_LOGGING_NAME ?= nvidia

VERSION_MK_DIR = ../../

include ../../utils.mk

include srcs.mk

# The source files for nv-kernel.o are all SRCS and SRCS_CXX defined in srcs.mk,
# and the NVIDIA ID string
ALL_SRCS = $(SRCS) $(SRCS_CXX)
ALL_SRCS += $(NVIDSTRING)

SRC_COMMON = ../common
CONDITIONAL_CFLAGS :=

CFLAGS += -include $(SRC_COMMON)/sdk/nvidia/inc/cpuopsys.h

CFLAGS += -I kernel/inc
CFLAGS += -I interface
CFLAGS += -I $(SRC_COMMON)/sdk/nvidia/inc
CFLAGS += -I arch/nvalloc/common/inc
CFLAGS += -I arch/nvalloc/common/inc/deprecated
CFLAGS += -I arch/nvalloc/unix/include
CFLAGS += -I inc
CFLAGS += -I inc/os
CFLAGS += -I $(SRC_COMMON)/shared/inc
CFLAGS += -I $(SRC_COMMON)/shared/msgq/inc
CFLAGS += -I $(SRC_COMMON)/inc

CFLAGS += -I $(SRC_COMMON)/uproc/os/libos-v2.0.0/include
CFLAGS += -I $(SRC_COMMON)/uproc/os/libos-v2.0.0/debug
CFLAGS += -I $(SRC_COMMON)/inc/swref
CFLAGS += -I $(SRC_COMMON)/inc/swref/published

CFLAGS += -I generated
CFLAGS += -I $(SRC_COMMON)/nvswitch/kernel/inc
CFLAGS += -I $(SRC_COMMON)/nvswitch/interface
CFLAGS += -I $(SRC_COMMON)/nvswitch/common/inc/
CFLAGS += -I $(SRC_COMMON)/inc/displayport
CFLAGS += -I $(SRC_COMMON)/nvlink/interface/
CFLAGS += -I src/mm/uvm/interface
CFLAGS += -I inc/libraries
CFLAGS += -I src/libraries
CFLAGS += -I inc/kernel

CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Wwrite-strings
CFLAGS += -fno-common
CFLAGS += -ffreestanding

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
  CFLAGS += -mstrict-align
  CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -mno-outline-atomics)
endif

CFLAGS += -fno-pic

CFLAGS += -D_LANGUAGE_C
CFLAGS += -D__NO_CTYPE
CFLAGS += -DNVRM
CFLAGS += -DLOCK_VAL_ENABLED=0
CFLAGS += -DPORT_ATOMIC_64_BIT_SUPPORTED=1
CFLAGS += -DPORT_IS_KERNEL_BUILD=1
CFLAGS += -DPORT_IS_CHECKED_BUILD=1
CFLAGS += -DPORT_MODULE_atomic=1
CFLAGS += -DPORT_MODULE_core=1
CFLAGS += -DPORT_MODULE_cpu=1
CFLAGS += -DPORT_MODULE_crypto=1
CFLAGS += -DPORT_MODULE_debug=1
CFLAGS += -DPORT_MODULE_memory=1
CFLAGS += -DPORT_MODULE_safe=1
CFLAGS += -DPORT_MODULE_string=1
CFLAGS += -DPORT_MODULE_sync=1
CFLAGS += -DPORT_MODULE_thread=1
CFLAGS += -DPORT_MODULE_util=1
CFLAGS += -DPORT_MODULE_example=0
CFLAGS += -DPORT_MODULE_mmio=0
CFLAGS += -DPORT_MODULE_time=0
CFLAGS += -DRS_STANDALONE=0
CFLAGS += -DRS_STANDALONE_TEST=0
CFLAGS += -DRS_COMPATABILITY_MODE=1
CFLAGS += -DRS_PROVIDES_API_STATE=0
CFLAGS += -DNV_CONTAINERS_NO_TEMPLATES

CFLAGS += -DINCLUDE_NVLINK_LIB
CFLAGS += -DINCLUDE_NVSWITCH_LIB

CFLAGS += -DNV_PRINTF_STRINGS_ALLOWED=1
CFLAGS += -DNV_ASSERT_FAILED_USES_STRINGS=1
CFLAGS += -DPORT_ASSERT_FAILED_USES_STRINGS=1

ifeq ($(DEBUG),1)
  CFLAGS += -gsplit-dwarf
endif

# Define how to perform dead code elimination: place each symbol in its own
# section at compile time, and garbage collect unreachable sections at link
# time.  exports_link_command.txt tells the linker which symbols need to be
# exported from nv-kernel.o so the linker can determine which symbols are
# unreachable.
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
NV_KERNEL_O_LDFLAGS += --gc-sections
EXPORTS_LINK_COMMAND = exports_link_command.txt

CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -fcf-protection=none)

ifeq ($(TARGET_ARCH),x86_64)
  CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -mindirect-branch-register)
  CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -mindirect-branch=thunk-extern)
endif

CFLAGS += $(CONDITIONAL_CFLAGS)

CC_ONLY_CFLAGS += --std=gnu11
CXX_ONLY_CFLAGS += --std=gnu++11

OBJS = $(call BUILD_OBJECT_LIST,$(ALL_SRCS))

# Define how to generate the NVIDIA ID string
$(eval $(call GENERATE_NVIDSTRING, \
  NVRM_ID, \
  UNIX Open Kernel Module, $(OBJS)))

# Define how to build each object file from the corresponding source file.
$(foreach src, $(ALL_SRCS), $(eval $(call DEFINE_OBJECT_RULE,TARGET,$(src))))

NV_KERNEL_O = $(OUTPUTDIR)/nv-kernel.o

.PNONY: all clean
all: $(NV_KERNEL_O)

LINKER_SCRIPT = nv-kernel.ld

$(NV_KERNEL_O): $(OBJS) $(EXPORTS_LINK_COMMAND) $(LINKER_SCRIPT)
	$(call quiet_cmd,LD) \
	  $(NV_KERNEL_O_LDFLAGS) \
	  -T $(LINKER_SCRIPT) \
	  -r -o $(NV_KERNEL_O) $(OBJS) @$(EXPORTS_LINK_COMMAND)
	$(call quiet_cmd,OBJCOPY) \
	  --localize-symbol=memset \
	  --localize-symbol=memcpy \
	  $@

clean:
	$(RM) -rf $(OUTPUTDIR)
