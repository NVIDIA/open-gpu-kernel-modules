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
ALL_SRCS := $(SRCS) $(SRCS_CXX) $(NVIDSTRING)

COCCI  := $(SHELL) cocci.sh
PATCH  ?= patch
SPATCH ?= spatch

# generated/ is most insteresting to us, but some cocci scripts require
# overriding the --dir option, making the leading './' important to still have
# --ignore filter the bindata files which don't need any fixups but make spatch
# complain about exhausting its stack space.
SPATCH_OPTS := --dir ./generated/
SPATCH_OPTS += --ignore ./generated/g_bindata
SPATCH_OPTS += -I generated/
SPATCH_OPTS += --include-headers	# headers should be processed (patched) too
SPATCH_OPTS += --patch .		# for 'patch -p1 …'
SPATCH_OPTS += --smpl-spacing		# don't mess with spacing too much to keep diffs small
SPATCH_OPTS += --very-quiet

JOBS := $(patsubst -j%,%,$(filter -j%,$(MAKEFLAGS)))
ifneq ($(JOBS),)
SPATCH_OPTS += --jobs $(JOBS)
endif

# order here is important and defines patch order too!
COCCI_SCRIPTS_ARGS := fix_nvoc_dtor.cocci	# only two instances left!
COCCI_SCRIPTS_ARGS += fix_nvoc_pfunc_type.cocci
COCCI_SCRIPTS_ARGS += fix_nvoc_pfunc_null.cocci:pfunc
COCCI_SCRIPTS_ARGS += fix_nvoc_pfunc_cast.cocci:pfunc
COCCI_SCRIPTS_ARGS += fix_nvoc_pfunc_2args.cocci
COCCI_SCRIPTS_ARGS += fix_nvoc_pfunc_use.cocci
COCCI_SCRIPTS_ARGS += fix_hal_iface_init.cocci
COCCI_SCRIPTS_ARGS += fix_rpc_hal_init.cocci:rpc_hal_init
COCCI_SCRIPTS_ARGS += fix_nv_inline.cocci

COCCI_SCRIPTS := $(filter %.cocci,$(subst :, ,$(COCCI_SCRIPTS_ARGS)))
COCCI_PATCHES  = $(addprefix 0???-,$(COCCI_SCRIPTS:.cocci=.diff))
COCCI_PATCH_MARKER := .cocci_patched

# XXX:
# Unfortunately, some cocci scripts override the --dir option, making more
# than generated/ be affected. Even worse, some changes have to be done to
# header files, impacting potentially all sources. Be safe and prevent any
# compilation from happening until all files are patched.
PATCH_CANDIDATES := $(ALL_SRCS)
PATCH_CANDIDATES := $(PATCH_CANDIDATES:.c=.o)
PATCH_CANDIDATES := $(PATCH_CANDIDATES:.cpp=.o)
$(addprefix $(obj)/,$(PATCH_CANDIDATES)): $(obj)/$(nvidia_src)/$(COCCI_PATCH_MARKER)

$(obj)/$(nvidia_src)/$(COCCI_PATCH_MARKER): $(addprefix $(obj)/$(nvidia_src)/,$(COCCI_SCRIPTS))
	@echo '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!'
	@echo '!!! Generating cocci patches, this may take a while. !!!'
	@echo '!!! DO NOT INTERRUPT, OR SOURCES WILL BE MESSED UP!  !!!'
	@echo '!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!'
	@cd $(src)/$(nvidia_src) && i=0 && \
	 for s in $(COCCI_SCRIPTS_ARGS); do \
		i=$$((i+1)); \
		c=$${s%:*}; \
		p=$$(printf "%04d-%s" $$i "$${c%.cocci}.diff"); \
		t=.tmp.$$p; \
		echo "  COCCI $$c"; \
		$(COCCI) "$$s" "$(SPATCH)" $(SPATCH_OPTS) > $$t || exit 1; \
		mv $$t $$p; \
		echo "  PATCH $$p"; \
		$(PATCH) -p1 <$$p; \
	 done
	@touch $@

# XXX: better reverse apply the patches on clean
#clean-files += $(addprefix $(nvidia_src)/,$(COCCI_PATCH_MARKER) $(COCCI_PATCHES))
clean-files += $(addprefix $(nvidia_src)/,pfunc.list)

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
