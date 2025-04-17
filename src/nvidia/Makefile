###########################################################################
# Makefile for nv-kernel.o
###########################################################################

NV_MODULE_LOGGING_NAME ?= nvidia

VERSION_MK_DIR = ../../

include ../../utils.mk

include srcs.mk

##############################################################################
# Helper functions to determine the compiler type
##############################################################################
GET_COMPILER_TYPE = \
  $(shell $(VERSION_MK_DIR)/nv-compiler.sh type $(1))
##############################################################################

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
CFLAGS += -I $(SRC_COMMON)/sdk/nvidia/inc/hw
CFLAGS += -I arch/nvalloc/common/inc
CFLAGS += -I arch/nvalloc/common/inc/gsp
CFLAGS += -I arch/nvalloc/common/inc/deprecated
CFLAGS += -I arch/nvalloc/unix/include
CFLAGS += -I inc
CFLAGS += -I inc/os
CFLAGS += -I $(SRC_COMMON)/shared/inc
CFLAGS += -I $(SRC_COMMON)/shared/msgq/inc
CFLAGS += -I $(SRC_COMMON)/inc

CFLAGS += -I $(SRC_COMMON)/uproc/os/libos-v2.0.0/include
CFLAGS += -I $(SRC_COMMON)/uproc/os/common/include
CFLAGS += -I $(SRC_COMMON)/inc/swref
CFLAGS += -I $(SRC_COMMON)/inc/swref/published

CFLAGS += -I generated
CFLAGS += -I $(SRC_COMMON)/nvswitch/kernel/inc
CFLAGS += -I $(SRC_COMMON)/nvswitch/interface
CFLAGS += -I $(SRC_COMMON)/nvswitch/common/inc/
CFLAGS += -I $(SRC_COMMON)/inc/displayport
CFLAGS += -I $(SRC_COMMON)/nvlink/interface/
CFLAGS += -I $(SRC_COMMON)/nvlink/inband/interface
CFLAGS += -I src/mm/uvm/interface
CFLAGS += -I inc/libraries
CFLAGS += -I src/libraries
CFLAGS += -I inc/kernel

# Libspdm source requires additional include paths and build flags.
include src/libraries/libspdm/nvidia/openspdm.mk
$(call BUILD_OBJECT_LIST,$(LIBSPDM_SOURCES)): CFLAGS += $(addprefix -I ,$(LIBSPDM_INCLUDES))
$(call BUILD_OBJECT_LIST,$(LIBSPDM_SOURCES)): CFLAGS += $(addprefix -D,$(LIBSPDM_DEFINES))

#if NV_USE_MBEDTLS
CFLAGS += -I $(SRC_COMMON)/mbedtls/include
CFLAGS += -I $(SRC_COMMON)/mbedtls/nvidia
CFLAGS += -DMBEDTLS_USER_CONFIG_FILE=\"nvrm_mbedtls_config.h\"
#endif

CFLAGS += -Werror-implicit-function-declaration
CFLAGS += -Wwrite-strings
CFLAGS += -Wundef
CFLAGS += -fno-common
CFLAGS += -ffreestanding
CFLAGS += -fno-stack-protector

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
  CFLAGS += -ffixed-x18
  CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -mno-outline-atomics)
endif

ifeq ($(TARGET_ARCH),riscv64)
  CFLAGS += -march=rv64imac_zicsr_zifencei
  CFLAGS += -mabi=lp64
  CFLAGS += -mcmodel=medany
  CFLAGS += -mno-relax
endif

CFLAGS += -fno-pic

CFLAGS += -D_LANGUAGE_C
CFLAGS += -D__NO_CTYPE
CFLAGS += -DNVRM
CFLAGS += -DLOCK_VAL_ENABLED=0
CFLAGS += -DPORT_ATOMIC_64_BIT_SUPPORTED=1
CFLAGS += -DPORT_IS_KERNEL_BUILD=1
CFLAGS += -DPORT_IS_CHECKED_BUILD=0
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

ifeq ($(TARGET_ARCH),x86_64)
  COMPILER_TYPE := $(call GET_COMPILER_TYPE, $(CC))
  ENDBR_SUPPORTED := $(call AS_HAS_INSTR, endbr64)

  FCF_SUPPORTED =

  #
  # GCC flags -fcf-protection=branch and -mindirect-branch=extern-thunk can
  # be used together after GCC version 9.4.0. See 
  # https://gcc.gnu.org/bugzilla/show_bug.cgi?id=93654 for details.
  # Check if GCC version is appropriate.
  #
  ifeq ($(COMPILER_TYPE),gcc)
    FCF_SUPPORTED := \
      $(shell $(VERSION_MK_DIR)/nv-compiler.sh version_is_at_least $(CC) 90400)
  endif

  #
  # Clang version 14.0.0 is required for -fcf-protection=branch to work
  # correctly. See commit
  # https://github.com/llvm/llvm-project/commit/dfcf69770bc522b9e411c66454934a37c1f35332
  #
  ifeq ($(COMPILER_TYPE),clang)
    FCF_SUPPORTED := \
      $(shell $(VERSION_MK_DIR)/nv-compiler.sh version_is_at_least $(CC) 140000)
  endif

  ifeq ($(FCF_SUPPORTED)-$(ENDBR_SUPPORTED),1-1)
    CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -fcf-protection=branch)
  endif
  CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -fno-jump-tables)
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

.PHONY: all
all: $(NV_KERNEL_O)

LINKER_SCRIPT = nv-kernel.ld

NV_KERNEL_O_LDFLAGS += $(LDFLAGS)

$(NV_KERNEL_O): $(OBJS) $(EXPORTS_LINK_COMMAND) $(LINKER_SCRIPT)
	$(call quiet_cmd,LD) \
	  $(NV_KERNEL_O_LDFLAGS) \
	  -T $(LINKER_SCRIPT) \
	  -r -o $(NV_KERNEL_O) $(OBJS) @$(EXPORTS_LINK_COMMAND)
	$(call quiet_cmd,OBJCOPY) \
	  --localize-symbol=memset \
	  --localize-symbol=memcpy \
	  --remove-section=.note.gnu.property \
	  $@

.PHONY: clean
clean:
	$(RM) -rf $(OUTPUTDIR)
