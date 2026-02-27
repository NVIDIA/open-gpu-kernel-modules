###########################################################################
# nvidia.Kbuild, included by kernel-open/nvidia/nvidia.Kbuild
###########################################################################

nvidia_src ?= .

NV_KERNEL_O      = nv-kernel.clean.o
NV_KERNEL_O_OBJS = $(nv-kernel-objs)

include $(src)/$(nvidia_src)/srcs.mk
include $(src)/$(nvidia_src)/defs.mk

# The source files for nv-kernel.o are all SRCS, except gcc_helper.c, and
# SRCS_CXX defined in srcs.mk, and the NVIDIA ID string
#
# We filter gcc_helper.c to avoid creating loops, os_mem_set() calling memset()
# calling os_mem_set()... The approach of the OS-agnostic build of localizing
# symbols and having --gc-sections drop them doesn't work here, unfortunately.
SRCS := $(filter-out %/gcc_helper.c,$(SRCS))
SRCS := $(addprefix $(nvidia_src)/,$(SRCS))
SRCS_CXX := $(addprefix $(nvidia_src)/,$(SRCS_CXX))
NVIDSTRING := $(addprefix $(nvidia_src)/,g_nvid_string.c)

nv-kernel-objs := $(SRCS:.c=.o)
nv-kernel-objs += $(SRCS_CXX:.cpp=.o)
nv-kernel-objs += $(NVIDSTRING:.c=.o)

# Hack to create a relocatable intermediate object file to avoid '/bin/sh:
# Argument list too long' errors for the real module.
ifeq ($(NV_PREPARE_ONLY),1)
obj-m += nv-kernel.stub.o
nv-kernel.stub-y := $(nv-kernel-objs) $(MOD_STUB)
endif

# for the 'clean' target
targets += $(nv-kernel-objs)
# ensure to use "our" NVIDSTRING
$(eval targets += $(NVIDSTRING))

# Define how to generate the NVIDIA ID string -- intentionally deviates from
# the OS-agnostic build
$(eval $(call GENERATE_NVIDSTRING, \
  NVRM_ID, \
  Linux Open Kernel Module, $(nv-kernel-objs)))

# now compile the final ccflags
nv-kernel-cflags := $(addprefix -include $(src)/$(nvidia_src)/,$(NV_INCLUDE))
nv-kernel-cflags += $(addprefix -I $(src)/$(nvidia_src)/,$(NV_INCDIRS))
nv-kernel-cflags += $(NV_DEFINES)

# quite some implicit fall-through cases
nv-kernel-cflags += -Wno-implicit-fallthrough
# lots of missing prototypes, all accross the board
nv-kernel-cflags-remove := -Wmissing-declarations -Wmissing-prototypes

# XXX: We cannot make use of --gc-sections, as that would also drop crucial
# XXX: sections like .alt_instructions or .return_sites.
## Define how to perform dead code elimination: place each symbol in its own
## section at compile time, and garbage collect unreachable sections at link
## time.  exports_link_command.txt tells the linker which symbols need to be
## exported from $(NV_KERNEL_O) so the linker can determine which symbols are
## unreachable.
#nv-kernel-cflags += -ffunction-sections
#nv-kernel-cflags += -fdata-sections
#
#nv-kernel-ldflags := --gc-sections
#nv-kernel-ldflags += @$(src)/$(nvidia_src)/$(EXPORTS_LINK_COMMAND)
#nv-kernel-ldflags += -T $(src)/$(nvidia_src)/$(LINKER_SCRIPT)

#nv-kernel-objcopyflags := --localize-symbol=memset
#nv-kernel-objcopyflags += --localize-symbol=memcpy
#nv-kernel-objcopyflags += --remove-section=.note.gnu.property

# move early -I... flags to after ours -- what a hack!
ccflags-includes := $(filter -I$(src)%,$(ccflags-y))
nv-kernel-cflags += $(patsubst -I%,-I %,$(ccflags-includes))
nv-kernel-cflags-remove += $(ccflags-includes)

$(call ASSIGN_PER_OBJ_CFLAGS_REMOVE, $(nv-kernel-objs), $(nv-kernel-cflags-remove))
$(call ASSIGN_PER_OBJ_CFLAGS, $(nv-kernel-objs), $(nv-kernel-cflags))
$(call ASSIGN_PER_OBJ_LDFLAGS, nv-kernel.stub.o, $(nv-kernel-ldflags))
$(call ASSIGN_PER_OBJ_OBJCOPYFLAGS, nv-kernel.clean.o, $(nv-kernel-objcopyflags))

$(obj)/nv-kernel.stub.o: $(addprefix $(obj)/$(nvidia_src)/,\
	$(EXPORTS_LINK_COMMAND) $(LINKER_SCRIPT))

## Libspdm source requires additional include paths and build flags.
include $(src)/$(nvidia_src)/src/libraries/libspdm/nvidia/openspdm.mk
# <nv-stddef.h> is sufficient to get all required defines and types
LIBSPDM_DEFINES += "LIBSPDM_STDINT_ALT=<nv-stddef.h>"
LIBSPDM_DEFINES += "LIBSPDM_STDBOOL_ALT=<nv-stddef.h>"
LIBSPDM_DEFINES += "LIBSPDM_STDDEF_ALT=<nv-stddef.h>"

libspdm-objs := $(addprefix $(nvidia_src)/,$(LIBSPDM_SOURCES:.c=.o))

libspdm-cflags := $(addprefix -I $(src)/$(nvidia_src)/,$(LIBSPDM_INCLUDES))
libspdm-cflags += $(addprefix -D,$(LIBSPDM_DEFINES))

$(call ASSIGN_PER_OBJ_CFLAGS, $(libspdm-objs), $(libspdm-cflags))
