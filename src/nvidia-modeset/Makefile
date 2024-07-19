###########################################################################
# Makefile for nv-modeset-kernel.o
###########################################################################

NV_MODULE_LOGGING_NAME ?= nvidia-modeset

VERSION_MK_DIR = ../../
include ../../utils.mk

include srcs.mk

##############################################################################
# Helper functions to determine the compiler type
##############################################################################
GET_COMPILER_TYPE = \
  $(shell $(VERSION_MK_DIR)/nv-compiler.sh type $(1))
##############################################################################

# The source files for nv-modeset-kernel.o are all SRCS and SRCS_CXX defined in
# srcs.mk, and the NVIDIA ID string
ALL_SRCS = $(SRCS) $(SRCS_CXX)
ALL_SRCS += $(NVIDSTRING)

SRC_COMMON = ../common

CONDITIONAL_CFLAGS :=

CFLAGS += -include $(SRC_COMMON)/sdk/nvidia/inc/cpuopsys.h

CFLAGS += -I $(SRC_COMMON)/sdk/nvidia/inc
CFLAGS += -I $(SRC_COMMON)/sdk/nvidia/inc/hw
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
CFLAGS += -fno-common
CFLAGS += -fomit-frame-pointer
CFLAGS += -fno-strict-aliasing
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -ffreestanding
CFLAGS += -fno-stack-protector

CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -Wformat-overflow=2)
CONDITIONAL_CFLAGS += $(call TEST_CC_ARG, -Wformat-truncation=1)

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

  CFLAGS += -DNVKMS_INCLUDE_HEADSURFACE=1

  CFLAGS += -I $(SRC_COMMON)/unix/nvidia-3d/interface
  CFLAGS += -I $(SRC_COMMON)/unix/nvidia-push/interface
  CFLAGS += -I $(SRC_COMMON)/unix/nvidia-3d/include
  CFLAGS += -I $(SRC_COMMON)/unix/nvidia-push/include
  CFLAGS += -I $(SRC_COMMON)/unix/xzminidec/interface
  CFLAGS += -I $(SRC_COMMON)/unix/nvidia-headsurface
  CFLAGS += -I src/shaders

  CFLAGS += -DNV_PUSH_IN_KERNEL
  CFLAGS += -DNV_XZ_CUSTOM_MEM_HOOKS
  CFLAGS += -DNV_XZ_USE_NVTYPES
  CFLAGS += -DXZ_DEC_SINGLE

  # Compress the shaders and embed in ELF object files.
  define COMPRESS_SHADERS
    $$(OUTPUTDIR)/$(1)_shaders.xz: src/shaders/g_$(1)_shaders
	@$(MKDIR) $$(OUTPUTDIR)
	$$(call quiet_cmd,XZ) -ce -C none < $$^ > $$@

    $$(eval $$(call READ_ONLY_OBJECT_FROM_FILE_RULE,$$(OUTPUTDIR)/$(1)_shaders.xz))

    SHADER_OBJS += $$(OUTPUTDIR)/$(1)_shaders.xz.o
  endef

  $(eval $(call COMPRESS_SHADERS,maxwell))
  $(eval $(call COMPRESS_SHADERS,pascal))
  $(eval $(call COMPRESS_SHADERS,volta))
  $(eval $(call COMPRESS_SHADERS,turing))
  $(eval $(call COMPRESS_SHADERS,ampere))
    $(eval $(call COMPRESS_SHADERS,hopper))
    $(eval $(call COMPRESS_SHADERS,blackwell))

OBJS = $(call BUILD_OBJECT_LIST,$(ALL_SRCS))
OBJS += $(SHADER_OBJS)

# Define how to generate the NVIDIA ID string
$(eval $(call GENERATE_NVIDSTRING, \
  NV_KMS_ID, \
  UNIX Open Kernel Mode Setting Driver, $(OBJS)))

# Define how to build each object file from the corresponding source file.
$(foreach src, $(ALL_SRCS), $(eval $(call DEFINE_OBJECT_RULE,TARGET,$(src))))

NV_MODESET_KERNEL_O = $(OUTPUTDIR)/nv-modeset-kernel.o

.PHONY: all
all: $(NV_MODESET_KERNEL_O)

$(NV_MODESET_KERNEL_O): $(OBJS)
	$(call quiet_cmd,LD) -r -o $(NV_MODESET_KERNEL_O) $(OBJS)

.PHONY: clean
clean:
	$(RM) -rf $(OUTPUTDIR)
