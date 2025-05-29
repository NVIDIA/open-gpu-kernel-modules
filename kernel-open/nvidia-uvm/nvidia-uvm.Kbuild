###########################################################################
# Kbuild fragment for nvidia-uvm.ko
###########################################################################

UVM_BUILD_TYPE = release

#
# Define NVIDIA_UVM_{SOURCES,OBJECTS}
#

NVIDIA_UVM_OBJECTS =

include $(src)/nvidia-uvm/nvidia-uvm-sources.Kbuild
NVIDIA_UVM_OBJECTS += $(patsubst %.c,%.o,$(NVIDIA_UVM_SOURCES))

obj-m += nvidia-uvm.o
nvidia-uvm-y := $(NVIDIA_UVM_OBJECTS)

NVIDIA_UVM_KO = nvidia-uvm/nvidia-uvm.ko

#
# Define nvidia-uvm.ko-specific CFLAGS.
#

ifeq ($(UVM_BUILD_TYPE),debug)
  NVIDIA_UVM_CFLAGS += -DDEBUG -g
endif

ifeq ($(UVM_BUILD_TYPE),develop)
  # -DDEBUG is required, in order to allow pr_devel() print statements to
  # work:
  NVIDIA_UVM_CFLAGS += -DDEBUG
  NVIDIA_UVM_CFLAGS += -DNVIDIA_UVM_DEVELOP
endif

NVIDIA_UVM_CFLAGS += -DNVIDIA_UVM_ENABLED
NVIDIA_UVM_CFLAGS += -DNVIDIA_UNDEF_LEGACY_BIT_MACROS

NVIDIA_UVM_CFLAGS += -DLinux
NVIDIA_UVM_CFLAGS += -D__linux__
NVIDIA_UVM_CFLAGS += -I$(src)/nvidia-uvm

$(call ASSIGN_PER_OBJ_CFLAGS, $(NVIDIA_UVM_OBJECTS), $(NVIDIA_UVM_CFLAGS))

#
# Register the conftests needed by nvidia-uvm.ko
#

NV_OBJECTS_DEPEND_ON_CONFTEST += $(NVIDIA_UVM_OBJECTS)

NV_CONFTEST_FUNCTION_COMPILE_TESTS += radix_tree_empty
NV_CONFTEST_FUNCTION_COMPILE_TESTS += radix_tree_replace_slot
NV_CONFTEST_FUNCTION_COMPILE_TESTS += cpumask_of_node
NV_CONFTEST_FUNCTION_COMPILE_TESTS += list_is_first
NV_CONFTEST_FUNCTION_COMPILE_TESTS += ioasid_get
NV_CONFTEST_FUNCTION_COMPILE_TESTS += mm_pasid_drop
NV_CONFTEST_FUNCTION_COMPILE_TESTS += mmget_not_zero
NV_CONFTEST_FUNCTION_COMPILE_TESTS += mmgrab
NV_CONFTEST_FUNCTION_COMPILE_TESTS += iommu_sva_bind_device_has_drvdata_arg
NV_CONFTEST_FUNCTION_COMPILE_TESTS += vm_fault_to_errno
NV_CONFTEST_FUNCTION_COMPILE_TESTS += find_next_bit_wrap
NV_CONFTEST_FUNCTION_COMPILE_TESTS += iommu_is_dma_domain
NV_CONFTEST_FUNCTION_COMPILE_TESTS += for_each_sgtable_dma_page
NV_CONFTEST_FUNCTION_COMPILE_TESTS += folio_test_swapcache
NV_CONFTEST_FUNCTION_COMPILE_TESTS += page_pgmap

NV_CONFTEST_TYPE_COMPILE_TESTS += vm_ops_fault_removed_vma_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += mmu_notifier_ops_invalidate_range
NV_CONFTEST_TYPE_COMPILE_TESTS += mmu_notifier_ops_arch_invalidate_secondary_tlbs
NV_CONFTEST_TYPE_COMPILE_TESTS += migrate_vma_added_flags
NV_CONFTEST_TYPE_COMPILE_TESTS += migrate_device_range
NV_CONFTEST_TYPE_COMPILE_TESTS += handle_mm_fault_has_mm_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += handle_mm_fault_has_pt_regs_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += mempolicy_has_unified_nodes
NV_CONFTEST_TYPE_COMPILE_TESTS += mempolicy_has_home_node
NV_CONFTEST_TYPE_COMPILE_TESTS += mpol_preferred_many_present
NV_CONFTEST_TYPE_COMPILE_TESTS += mmu_interval_notifier
NV_CONFTEST_TYPE_COMPILE_TESTS += fault_flag_remote_present
NV_CONFTEST_TYPE_COMPILE_TESTS += sg_dma_page_iter
NV_CONFTEST_TYPE_COMPILE_TESTS += struct_page_has_zone_device_data

NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_int_active_memcg
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_migrate_vma_setup
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present___iowrite64_lo_hi
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_make_device_exclusive
