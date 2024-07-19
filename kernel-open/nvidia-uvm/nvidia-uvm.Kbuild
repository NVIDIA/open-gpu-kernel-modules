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

# Some linux kernel functions rely on being built with optimizations on and
# to work around this we put wrappers for them in a separate file that's built
# with optimizations on in debug builds and skipped in other builds.
# Notably gcc 4.4 supports per function optimization attributes that would be
# easier to use, but is too recent to rely on for now.
NVIDIA_UVM_DEBUG_OPTIMIZED_SOURCE := nvidia-uvm/uvm_debug_optimized.c
NVIDIA_UVM_DEBUG_OPTIMIZED_OBJECT := $(patsubst %.c,%.o,$(NVIDIA_UVM_DEBUG_OPTIMIZED_SOURCE))

ifneq ($(UVM_BUILD_TYPE),debug)
  # Only build the wrappers on debug builds
  NVIDIA_UVM_OBJECTS := $(filter-out $(NVIDIA_UVM_DEBUG_OPTIMIZED_OBJECT), $(NVIDIA_UVM_OBJECTS))
endif

obj-m += nvidia-uvm.o
nvidia-uvm-y := $(NVIDIA_UVM_OBJECTS)

NVIDIA_UVM_KO = nvidia-uvm/nvidia-uvm.ko

#
# Define nvidia-uvm.ko-specific CFLAGS.
#

ifeq ($(UVM_BUILD_TYPE),debug)
  NVIDIA_UVM_CFLAGS += -DDEBUG -O1 -g
else
  ifeq ($(UVM_BUILD_TYPE),develop)
    # -DDEBUG is required, in order to allow pr_devel() print statements to
    # work:
    NVIDIA_UVM_CFLAGS += -DDEBUG
    NVIDIA_UVM_CFLAGS += -DNVIDIA_UVM_DEVELOP
  endif
  NVIDIA_UVM_CFLAGS += -O2
endif

NVIDIA_UVM_CFLAGS += -DNVIDIA_UVM_ENABLED
NVIDIA_UVM_CFLAGS += -DNVIDIA_UNDEF_LEGACY_BIT_MACROS

NVIDIA_UVM_CFLAGS += -DLinux
NVIDIA_UVM_CFLAGS += -D__linux__
NVIDIA_UVM_CFLAGS += -I$(src)/nvidia-uvm

$(call ASSIGN_PER_OBJ_CFLAGS, $(NVIDIA_UVM_OBJECTS), $(NVIDIA_UVM_CFLAGS))

ifeq ($(UVM_BUILD_TYPE),debug)
  # Force optimizations on for the wrappers
  $(call ASSIGN_PER_OBJ_CFLAGS, $(NVIDIA_UVM_DEBUG_OPTIMIZED_OBJECT), $(NVIDIA_UVM_CFLAGS) -O2)
endif

#
# Register the conftests needed by nvidia-uvm.ko
#

NV_OBJECTS_DEPEND_ON_CONFTEST += $(NVIDIA_UVM_OBJECTS)

NV_CONFTEST_FUNCTION_COMPILE_TESTS += wait_on_bit_lock_argument_count
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pde_data
NV_CONFTEST_FUNCTION_COMPILE_TESTS += radix_tree_empty
NV_CONFTEST_FUNCTION_COMPILE_TESTS += radix_tree_replace_slot
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pnv_npu2_init_context
NV_CONFTEST_FUNCTION_COMPILE_TESTS += vmf_insert_pfn
NV_CONFTEST_FUNCTION_COMPILE_TESTS += cpumask_of_node
NV_CONFTEST_FUNCTION_COMPILE_TESTS += list_is_first
NV_CONFTEST_FUNCTION_COMPILE_TESTS += timer_setup
NV_CONFTEST_FUNCTION_COMPILE_TESTS += pci_bus_address
NV_CONFTEST_FUNCTION_COMPILE_TESTS += set_memory_uc
NV_CONFTEST_FUNCTION_COMPILE_TESTS += set_pages_uc
NV_CONFTEST_FUNCTION_COMPILE_TESTS += ktime_get_raw_ts64
NV_CONFTEST_FUNCTION_COMPILE_TESTS += ioasid_get
NV_CONFTEST_FUNCTION_COMPILE_TESTS += mm_pasid_drop
NV_CONFTEST_FUNCTION_COMPILE_TESTS += mmget_not_zero
NV_CONFTEST_FUNCTION_COMPILE_TESTS += mmgrab
NV_CONFTEST_FUNCTION_COMPILE_TESTS += iommu_sva_bind_device_has_drvdata_arg
NV_CONFTEST_FUNCTION_COMPILE_TESTS += vm_fault_to_errno
NV_CONFTEST_FUNCTION_COMPILE_TESTS += find_next_bit_wrap
NV_CONFTEST_FUNCTION_COMPILE_TESTS += iommu_is_dma_domain

NV_CONFTEST_TYPE_COMPILE_TESTS += backing_dev_info
NV_CONFTEST_TYPE_COMPILE_TESTS += mm_context_t
NV_CONFTEST_TYPE_COMPILE_TESTS += get_user_pages_remote
NV_CONFTEST_TYPE_COMPILE_TESTS += get_user_pages
NV_CONFTEST_TYPE_COMPILE_TESTS += pin_user_pages_remote
NV_CONFTEST_TYPE_COMPILE_TESTS += pin_user_pages
NV_CONFTEST_TYPE_COMPILE_TESTS += vm_fault_has_address
NV_CONFTEST_TYPE_COMPILE_TESTS += vm_ops_fault_removed_vma_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += kmem_cache_has_kobj_remove_work
NV_CONFTEST_TYPE_COMPILE_TESTS += sysfs_slab_unlink
NV_CONFTEST_TYPE_COMPILE_TESTS += vm_fault_t
NV_CONFTEST_TYPE_COMPILE_TESTS += mmu_notifier_ops_invalidate_range
NV_CONFTEST_TYPE_COMPILE_TESTS += mmu_notifier_ops_arch_invalidate_secondary_tlbs
NV_CONFTEST_TYPE_COMPILE_TESTS += proc_ops
NV_CONFTEST_TYPE_COMPILE_TESTS += timespec64
NV_CONFTEST_TYPE_COMPILE_TESTS += mm_has_mmap_lock
NV_CONFTEST_TYPE_COMPILE_TESTS += migrate_vma_added_flags
NV_CONFTEST_TYPE_COMPILE_TESTS += migrate_device_range
NV_CONFTEST_TYPE_COMPILE_TESTS += vm_area_struct_has_const_vm_flags
NV_CONFTEST_TYPE_COMPILE_TESTS += handle_mm_fault_has_mm_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += handle_mm_fault_has_pt_regs_arg
NV_CONFTEST_TYPE_COMPILE_TESTS += mempolicy_has_unified_nodes
NV_CONFTEST_TYPE_COMPILE_TESTS += mempolicy_has_home_node
NV_CONFTEST_TYPE_COMPILE_TESTS += mpol_preferred_many_present
NV_CONFTEST_TYPE_COMPILE_TESTS += mmu_interval_notifier
NV_CONFTEST_TYPE_COMPILE_TESTS += fault_flag_remote_present

NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_int_active_memcg
NV_CONFTEST_SYMBOL_COMPILE_TESTS += is_export_symbol_present_migrate_vma_setup
