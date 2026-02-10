###########################################################################
# nvidia-modeset.Kbuild, included by kernel-open/nvidia-modeset/nvidia-modeset.Kbuild
###########################################################################

nvidia_modeset_src ?= .

NV_MODESET_KERNEL_O      = nv-modeset-kernel.clean.o
NV_MODESET_KERNEL_O_OBJS = $(nv-modeset-kernel-objs)

include $(src)/$(nvidia_modeset_src)/srcs.mk
include $(src)/$(nvidia_modeset_src)/defs.mk

# The source files for nv-modeset-kernel.o are all SRCS and SRCS_CXX defined in
# srcs.mk, the NVIDIA ID string and the shaders
SRCS := $(addprefix $(nvidia_modeset_src)/,$(SRCS))
SRCS_CXX := $(addprefix $(nvidia_modeset_src)/,$(SRCS_CXX))
NVIDSTRING := $(addprefix $(nvidia_modeset_src)/,g_nvid_string.c)
ALL_SRCS := $(SRCS) $(SRCS_CXX) $(NVIDSTRING)

# g_${shader}_shaders -> $[shaders}_shaders.xz.o
SHADER_OBJS := \
	$(addprefix $(nvidia_modeset_src)/src/shaders/,\
		$(addsuffix .xz.o,$(addsuffix _shaders,$(SHADERS))))

nv-modeset-kernel-objs := $(SRCS:.c=.o)
nv-modeset-kernel-objs += $(SRCS_CXX:.cpp=.o)
nv-modeset-kernel-objs += $(NVIDSTRING:.c=.o)
nv-modeset-kernel-objs += $(SHADER_OBJS)

# Hack to create a relocatable intermediate object file to avoid '/bin/sh:
# Argument list too long' errors for the real module.
ifeq ($(NV_PREPARE_ONLY),1)
obj-m += nv-modeset-kernel.stub.o
nv-modeset-kernel.stub-y := $(nv-modeset-kernel-objs) $(MOD_STUB)
endif

# for the 'clean' target
targets += $(nv-modeset-kernel-objs)
# ensure to use "our" NVIDSTRING
$(eval targets += $(NVIDSTRING))

# Define how to generate the NVIDIA ID string -- intentionally deviates from
# the OS-agnostic build
$(eval $(call GENERATE_NVIDSTRING, \
  NV_KMS_ID, \
  Linux Open Kernel Mode Setting Driver, $(nv-modeset-kernel-objs)))

# now compile the final ccflags
nv-modeset-kernel-cflags := $(addprefix -include $(src)/$(nvidia_modeset_src)/,$(NV_INCLUDE))
nv-modeset-kernel-cflags += $(addprefix -I $(src)/$(nvidia_modeset_src)/,$(NV_INCDIRS))
nv-modeset-kernel-cflags += $(NV_DEFINES)

# suppress some warnings
nv-modeset-kernel-cflags += -Wno-format-zero-length
nv-modeset-kernel-cflags += -Wno-implicit-fallthrough

# XXX: Using -ffunction-sections / -fdata-sections makes no sense without
# XXX: --gc-sections. However, we cannot make use of --gc-sections as that
# XXX: would also drop crucial sections like .alt_instructions or
# XXX: .return_sites. So just disable these.
#nv-modeset-kernel-cflags += -ffunction-sections
#nv-modeset-kernel-cflags += -fdata-sections

# move early -I... flags to after ours -- what a hack!
ccflags-includes := $(filter -I$(src)%,$(ccflags-y))
nv-modeset-kernel-cflags += $(patsubst -I%,-I %,$(ccflags-includes))
nv-modeset-kernel-cflags-remove += $(ccflags-includes)

$(call ASSIGN_PER_OBJ_CFLAGS_REMOVE, $(nv-modeset-kernel-objs), $(nv-modeset-kernel-cflags-remove))
$(call ASSIGN_PER_OBJ_CFLAGS, $(nv-modeset-kernel-objs), $(nv-modeset-kernel-cflags))

nv-modeset-kernel-cxxflags := -std=gnu++11
nv-modeset-kernel-cxxflags += -fno-operator-names
nv-modeset-kernel-cxxflags += -fno-rtti
nv-modeset-kernel-cxxflags += -fno-exceptions
nv-modeset-kernel-cxxflags += -fcheck-new

# ... referenced in section `__mcount_loc' of ...: defined in discarded section ... :/
nv-modeset-kernel-cxxflags += -mno-record-mcount

# --- 8< --- taken from scripts/Makefile.lib, adapted for C++
#

# strip C-only options from c_flags
cxx_cflags_filter := -W%implicit
cxx_cflags_filter += -W%implicit-int
cxx_cflags_filter += -W%implicit-function-declaration
cxx_cflags_filter += -W%strict-prototypes
cxx_cflags_filter += -W%missing-prototypes
cxx_cflags_filter += -W%pointer-sign
cxx_cflags_filter += -W%incompatible-pointer-types
cxx_cflags_filter += -W%unterminated-string-initialization
cxx_cflags_filter += -W%designated-init
cxx_cflags_filter += -W%override-init
cxx_cflags_filter += -std=gnu11

CXX_FLAGS := $(nv-modeset-kernel-cxxflags)

cxx_flags = $(filter-out $(cxx_cflags_filter),$(c_flags)) $(CXX_FLAGS)
quiet_cmd_cc_o_cxx = CXX $(quiet_modtag) $@
      cmd_cc_o_cxx = $(CXX) $(cxx_flags) -c -o $@ $< \
		$(cmd_ld_single) #\
		$(cmd_objtool)	# disabled for now, as .return_sites / .call_sites reference to-be-discarded symbols

define rule_cc_o_cxx
    $(call cmd_and_fixdep,cc_o_cxx)
    $(call cmd,checksrc)
    $(call cmd,checkdoc)
    $(call cmd,gen_objtooldep)
    $(call cmd,gen_symversions_c)$(eval # genksyms can only parse C code!)
    $(call cmd,record_mcount)
    $(call cmd,warn_shared_object)
endef

$(obj)/%.o: $(obj)/%.cpp $(recordmcount_source) FORCE
	$(call if_changed_rule,cc_o_cxx)
	$(call cmd,force_checksrc)
#
# --- >8 ---

# shader rules
#
ld_o_xz-ldflags = -r -z noexecstack --format=binary
ld_o_xz-objcopy = --rename-section .data=.rodata,contents,alloc,load,data,readonly
ld_o_xz-tmpfile = $(dir $<)/.tmp_$(notdir $@)

# changing into the directory of the .xz is needed to get sane names for the
# _binary_..._start/_end/_size symbols
quiet_cmd_ld_o_xz = LD XZO  $@
      cmd_ld_o_xz = (cd $(dir $<) && $(LD) $(ld_o_xz-ldflags) $(notdir $<) -o $(notdir $(ld_o_xz-tmpfile))) && $(OBJCOPY) $(ld_o_xz-objcopy) $(ld_o_xz-tmpfile) $@ && rm $(ld_o_xz-tmpfile)

$(obj)/%.xz.o: $(obj)/%.xz FORCE
	$(call if_changed,ld_o_xz)

quiet_cmd_shader_xz = XZRAW   $@
      cmd_shader_xz = cat $< | $(XZ) -ce -C none > $@

# This one is needed because we want to change the basename of the file and '%'
# would match the full path, including directories,prefixing the 'g_' at the
# wrong spot.
define mk_shader_rules
$$(obj)/$(1).xz: $$(obj)/$(dir $(1))/g_$(notdir $(1)) FORCE
	$$(call if_changed,shader_xz)

# for 'make clean'
targets += $(1).xz

# not really needed, but to have conftest be the very first
NV_OBJECTS_DEPEND_ON_CONFTEST += $(1).xz
endef

$(foreach shader, $(SHADER_OBJS:.xz.o=), $(eval $(call mk_shader_rules,$(shader))))

